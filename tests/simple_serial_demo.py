import os, time, sys, struct
script_base = os.path.dirname(__file__)
os.chdir(script_base + '/../software')

import chipwhisperer as cw

# NOTE: This is a work in progress. Just demo's/tests the looper API.

SCOPETYPE = os.environ.get('SCOPETYPE', 'OPENADC')
PLATFORM = os.environ.get('PLATFORM', 'CWLITEARM')
SS_VER = os.environ.get('SS_VER', 'SS_VER_2_1')

if PLATFORM != 'CWLITEARM':
    raise ValueError('Platform not currently supported!')

KEY_GLITCH = 'glitch'
KEY_RESET = 'reset'
KEY_NORMAL = 'normal'
KEY_EDGE = 'edge'

KEY_OFFSET = 'offset'
KEY_WIDTH = 'width'
KEY_EXTOFF = 'extoff'
KEY_REPEAT = 'repeat'
KEY_GLITCHT = 'glitcht'

IDX_GLITCH = 0
IDX_RESET = 1
IDX_NORMAL = 2
IDX_EDGE = 3

FW_PATH = '../hardware/victims/firmware/simpleserial-glitch/simpleserial-glitch-%s.hex' % PLATFORM

class CWGlitchParams(object):
    def __init__(self, params):
        self._params = []
        self._ranges = []
        self._steps = []
        for key, set in params.items():
            self._params.append(key)
            self._ranges.append((set[0], set[1]))
            self._steps.append(set[2])

    def make_gc(self):
        gc = cw.GlitchController(
            groups=[
                KEY_GLITCH,
                KEY_RESET,
                KEY_NORMAL,
                KEY_EDGE,
            ],
            parameters=self._params
        )

        gc.set_global_step(1)
        for i in range(0, gc.param_count):
            key = self._params[i]
            gc.set_step(key, self._steps[i])
            gc.set_range(key, *self._ranges[i])

        return gc

    # Set

    def get_repeat(self, gc, set):
        return set[gc.get_param_index(KEY_REPEAT)]

    def get_offset(self, gc, set):
        return set[gc.get_param_index(KEY_OFFSET)]

    def get_width(self, gc, set):
        return set[gc.get_param_index(KEY_WIDTH)]

    def get_extoff(self, gc, set):
        return set[gc.get_param_index(KEY_EXTOFF)]

    def get_glitcht(self, gc, set):
        return set[gc.get_param_index(KEY_GLITCHT)]

# ADJUST PARAMETERS AS NEEDED...

class CWLiteLoopClkParams(CWGlitchParams):
    def __init__(self):
        super().__init__({
            KEY_OFFSET : (-45, 48, 8),
            KEY_WIDTH : (0, 48, 4),
        })

    def get_repeat(self, gc, set):
        return 10

    def get_extoff(self, gc, set):
        return 2

class CWLiteLoopVCCParamBase(CWGlitchParams):
    def __init__(self, vxx, params):
        if vxx:
            params[KEY_GLITCHT] = (1, 3, 1)
        super().__init__(params)

class CWLiteLoopVCCParams(CWLiteLoopVCCParamBase):
    def __init__(self, vxx):
        super().__init__(vxx, {
            KEY_OFFSET : (-45, 48, 0.4),
            KEY_WIDTH : (-49.8, 49.8, 0.4),
            KEY_EXTOFF : (0, 8, 1),
            KEY_REPEAT : (1, 8, 1)
        })

class ResultRange(object):
    def __init__(self, gc, group):
        self._gc = gc
        self.reset(group)

    @property
    def group(self):
        return self._group

    def reset(self, group):
        self._group = group
        # Set opposite to force update on first result
        self._min = self._gc.parameter_max.copy()
        self._max = self._gc.parameter_min.copy()

    def update_stage(self, params, i):
        val = params[i]
        if val < self._min[i]:
            self._min[i] = val
        if val > self._max[i]:
            self._max[i] = val

    def update_global(self, hits, params, i):
        if not hits[self._group]:
            return
        self.update_stage(params, i)

    def display_range(self):
        gc_count = self._gc.get_group_counter(self._group)
        print('RANGE[%s] hits=%d:' % (self._group, gc_count))
        if not gc_count:
            return
        for i in range(self._gc.param_count):
            print('\t%s: (%s, %s)' % (self._gc.parameters[i], self._min[i], self._max[i]))

# Glitch Context

class SimpleSerialGlitchContext(cw.GlitchLoopGCContext, cw.GlitchLoopTargetContext):
    def __init__(self, scope, params, args):
        self._scope = scope
        self._target = cw.target(scope, cw.targets.SimpleSerial2)
        self._params = params
        self._gc = None
        if params:
            self._gc = params.make_gc()
        self._success_sample = args.success_samples
        self._reset_next = args.next_on_reset
        self.reset_edge()

    @property
    def scope(self):
        return self._scope

    @property
    def target(self):
        return self._target

    @property
    def params(self):
        return self._params

    @property
    def gc(self):
        return self._gc

    @property
    def success_sample(self):
        return self._success_sample

    @property
    def reset_next(self):
        return self._reset_next

    def dis(self):
        if self.target:
            self.target.dis()
            self._target = None

        if self.scope:
            self.scope.glitch_disable()
            self.scope.default_setup()
            self.reset()
            self.scope.dis()
            self._scope = None

    @property
    def rst_pin(self):
        raise 'unimplemented'

    @rst_pin.setter
    def rst_pin(self, rst):
        raise 'unimplemented'

    @property
    def rst_delay(self):
        raise 'unimplemented'

    @property
    def samples_obtained(self):
        if self.success_sample <= 0:
            return False
        return self.gc.get_group_counter(KEY_GLITCH) >= self.success_sample

    @property
    def is_edge(self):
        return self._has_normal and self._has_reset

    def reset_edge(self):
        self._has_normal = False
        self._has_reset = False

    def set_normal(self):
        self._has_normal = True

    def set_reset(self):
        self._has_reset = True

    def gc_add(self, result):
        self.gc.add(result, plot=False)

    def set_rst_and_delay(self, rst):
        self.rst_pin = rst
        time.sleep(self.rst_delay)

    def reset(self, prt=True):
        if prt:
            print('Resetting target...')
        self.set_rst_and_delay(False)
        self.set_rst_and_delay(None)
        if self.target:
            self.target.flush()

    def make_res_range(self, group):
        return ResultRange(self.gc, group)

class ARMSimpleSerialGlitchContext(SimpleSerialGlitchContext):
    @property
    def rst_pin(self):
        return self.scope.io.nrst

    @rst_pin.setter
    def rst_pin(self, rst):
        self.scope.io.nrst = rst

    @property
    def rst_delay(self):
        return 0.05

class XMEGASimpleSerialGlitchContext(SimpleSerialGlitchContext):
    @property
    def rst_pin(self):
        return self.scope.io.pdic

    @rst_pin.setter
    def rst_pin(self, rst):
        self.scope.io.pdic = rst

    @property
    def rst_delay(self):
        return 0.1

# Glitch Method

class SimpleSerialGlitcher(cw.GlitchLoopGlitchMethod):
    @property
    def glitch_method(self):
        raise 'unimplemented'

    def set_params(self, looper, ctx, set):
        if ctx.samples_obtained:
            return False
        ctx.scope.glitch.repeat = ctx.params.get_repeat(ctx.gc, set)
        ctx.scope.glitch.offset = ctx.params.get_offset(ctx.gc, set)
        ctx.scope.glitch.width = ctx.params.get_width(ctx.gc, set)
        ctx.scope.glitch.ext_offset = ctx.params.get_extoff(ctx.gc, set)
        print('params(%s): offset=%s, width=%s, extoff=%s, repeat=%s' % (
            set,
            ctx.scope.glitch.offset,
            ctx.scope.glitch.width,
            ctx.scope.glitch.ext_offset,
            ctx.scope.glitch.repeat,
        ))
        ctx.reset_edge()

class SimpleSerialClkGlitch(SimpleSerialGlitcher, cw.GlitchLoopDefaultClkGlitch):
    @property
    def glitch_method(self):
        return 'Clock'

class SimpleSerialVCCGlitch(SimpleSerialGlitcher, cw.GlitchLoopDefaultVCCGlitch):
    def glitch_sequence(self, looper, ctx):
        self.pre_arm(looper, ctx)
        ctx.scope.arm()
        
        # Run target and capture trigger
        return looper.driver.target_run_vcc(looper, ctx)

class SimpleSerialVXXGlitch(SimpleSerialGlitcher, cw.GlitchLoopDefaultVCCGlitch):
    @property
    def glitcht(self):
        return self._glitcht

    @property
    def glitch_method(self):
        return 'VCC (switch)'

    def set_params(self, looper, ctx, set):
        if super().set_params(looper, ctx, set) == False:
            return False
        self._glitcht = ctx.params.get_glitcht(ctx.gc, set)

class SimpleSerialVBothGlitch(SimpleSerialGlitcher, cw.GlitchLoopDefaultVBothGlitch):
    @property
    def glitch_method(self):
        return 'VCC (both)'

class SimpleSerialVHighGlitch(SimpleSerialGlitcher, cw.GlitchLoopDefaultVHighGlitch):
    @property
    def glitch_method(self):
        return 'VCC (hp)'

class SimpleSerialVLowGlitch(SimpleSerialGlitcher, cw.GlitchLoopDefaultVLowGlitch):
    @property
    def glitch_method(self):
        return 'VCC (lp)'

# Param generator

class SimpleSerialParamGenerator(cw.GlitchLoopParamGenerator):
    def gen_continue(self, looper, ctx):
        return not ctx.samples_obtained

class SimpleSerialSeqParams(SimpleSerialParamGenerator, cw.GlitchLoopGCSeqParams):
    pass

class SimpleSerialRandParams(SimpleSerialParamGenerator, cw.GlitchLoopGCRandParams):
    pass

# App Driver

class SimpleSerialAppDriver(cw.GlitchDefaultTargetDriver):
    def __init__(self, cmd, arg, data_sz):
        self._cmd = cmd
        self._arg = arg
        self._size = data_sz

    @property
    def verbose_default_setup(self):
        return False

    def target_config(self, looper, ctx):
        super().target_config(looper, ctx)
        ctx.reset()
        ctx.scope.adc.timeout = 0.5

    def target_run(self, looper, ctx):
        ctx.target.simpleserial_write(self._cmd, self._arg)

    def target_run_vcc(self, looper, ctx):
        scope = ctx.scope
        ctx.target.simpleserial_write(self._cmd, self._arg)
        return scope.capture_and_vreset()

    def eval_glitch(self, val):
        raise 'unimplemented'

    def read_result(self, ctx):
        return ctx.target.simpleserial_read_witherrors('r', self._size, glitch_timeout=10)

    def process_glitch(self, looper, ctx, timedout):
        if not timedout:
            val = self.read_result(ctx)
            timedout = not val['valid']
        if timedout:
            ctx.reset(prt=False)
            ctx.gc_add(KEY_RESET)
            if ctx.reset_next:
                return False
        else:
            #print('response: %s' % val)
            if self.eval_glitch(val['payload']):
                print('glitched!')
                ctx.gc_add(KEY_GLITCH)
                return False
            else:
                #print('normal...')
                #ctx.set_normal()
                ctx.gc_add(KEY_NORMAL)

class SimpleSerialLoopDriver(SimpleSerialAppDriver):
    def __init__(self):
        super().__init__('g', bytearray([]), 4)

    def eval_glitch(self, val):
        val = struct.unpack('<I', val)[0]
        return val != 2500

class SimpleSerialPassDriver(SimpleSerialAppDriver):
    def __init__(self):
        super().__init__('p', bytearray([0]*5), 1)

    def eval_glitch(self, val):
        return len(val) == 1 and val[0] == 1

# Loop Implementation

class SimpleSerialGlitchLoop(cw.GlitchLoopRunner):
    def __init__(self, glitcher, args):
        if args.password:
            self._driver = SimpleSerialPassDriver()
        else:
            self._driver = SimpleSerialLoopDriver()
        self._glitcher = glitcher
        if args.random:
            self._params = SimpleSerialRandParams()
        else:
            self._params = SimpleSerialSeqParams()

    @property
    def params(self):
        return self._params

    @property
    def driver(self):
        return self._driver

    @property
    def glitcher(self):
        return self._glitcher

    def pre_loop(self, ctx):
        print('Glitch method: %s' % self.glitcher.glitch_method)

    def post_loop(self, ctx):
        super().post_loop(ctx)
        ctx.dis()

if __name__ == '__main__':
    import argparse
    GLITCH_ARG_CLK = 'clk'
    GLITCH_ARG_VCC = 'vcc'
    GLITCH_ARG_VHP = 'vhp'
    GLITCH_ARG_VLP = 'vlp'
    GLITCH_ARG_VXX = 'vxx'
    args = argparse.ArgumentParser()
    args.add_argument('--program', action='store_true', default=False)
    args.add_argument('--password', action='store_true', default=False)
    args.add_argument('--glitch', choices=[ GLITCH_ARG_CLK, GLITCH_ARG_VCC, GLITCH_ARG_VHP, GLITCH_ARG_VLP, GLITCH_ARG_VXX ], default=None)
    args.add_argument('--critical_logging', action='store_true', default=False)
    args.add_argument('--success_samples', type=int, choices=range(0, 10), default=0)
    args.add_argument('--retries', type=int, choices=range(0, 10), default=4)
    args.add_argument('--next_on_reset', action='store_true', default=False)
    args.add_argument('--random', action='store_true', default=False)
    args = args.parse_args()

    scope = cw.scope()
    looper = None
    params = None
    if args.glitch:
        if args.glitch == GLITCH_ARG_CLK:
            glitcher = SimpleSerialClkGlitch()
            params = CWLiteLoopClkParams()
        else:
            if args.glitch == GLITCH_ARG_VCC:
                glitcher = SimpleSerialVBothGlitch()
            elif args.glitch == GLITCH_ARG_VHP:
                glitcher = SimpleSerialVHighGlitch()
            elif args.glitch == GLITCH_ARG_VLP:
                glitcher = SimpleSerialVLowGlitch()
            else:
                glitcher = SimpleSerialVXXGlitch()
            params = CWLiteLoopVCCParams(args.glitch == GLITCH_ARG_VXX)
        looper = SimpleSerialGlitchLoop(glitcher, args)
    context = ARMSimpleSerialGlitchContext(scope, params, args)

    if args.critical_logging:
        cw.set_all_log_levels(cw.logging.CRITICAL)

    try:
        print(scope)
        context.reset()
        print(scope)
        if args.program:
            prog = cw.programmers.STM32FProgrammer
            context.scope.default_setup()
            context.reset()
            cw.program_target(scope, prog, FW_PATH)
            time.sleep(3)

        #print(context.scope)

        if looper:
            looper.run_loop(context, retries=args.retries)

    finally:
        context.dis()
        print('finished')
        if looper:
            print('Analyzing results...')
            range_glitch = context.make_res_range(KEY_GLITCH)
            range_edge = context.make_res_range(KEY_EDGE)
            range_reset = context.make_res_range(KEY_RESET)
            range_normal = context.make_res_range(KEY_NORMAL)
            results = context.gc.results.calc()

            for params,hits in results.items():
                for i in range(len(params)):
                    range_glitch.update_global(hits, params, i)
                    range_edge.update_global(hits, params, i)
                    range_reset.update_global(hits, params, i)
                    range_normal.update_global(hits, params, i)

            # Only edge and normal really matters
            range_glitch.display_range()
            range_edge.display_range()
            range_reset.display_range()
            range_normal.display_range()
