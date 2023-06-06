from ...capture.scopes.cwhardware.ChipWhispererExtra import GPIOSettings

class GlitchLoopContext(object):
    """Base interface that this API expects to supply for the glitch loop process.
    """

    @property
    def scope(self):
        """Gets the scope of the context being ran through the glitch loop.

        Return:
            A CW scope object.
        """
        return None

class GlitchLoopGCContext(GlitchLoopContext):
    """Interface that provides functionality for a GlitchController driven loop. 
    """

    @property
    def gc(self):
        """Gets the GlitchController instance for this context.

        Return:
            A GlitchController object.
        """
        return None

class GlitchLoopTargetContext(GlitchLoopContext):
    """Interface that provides functionality for a CW target instance.
    """

    @property
    def target(self):
        """Gets the target instance attached to the scope.

        Return:
            A CW target object.
        """
        return None

class GlitchLoopGlitchMethod(object):
    """Base class that provides the functionality for a method of glitching.
    """

    def set_params(self, looper, ctx, set):
        """Applies a parameter set that defines the glitch.
        """
        raise NotImplementedError()

    def glitch_config(self, looper, ctx):
        """Configures the glitch hardware before the loop.

        Return:
            False to skip the parameter set and generate the next set. Any other return continues
        the glitch sequence.
        """
        raise NotImplementedError()

    def pre_arm(self, looper, ctx):
        """Notification when the loop is about to arm the scope for the glitch.
        """
        self.glitch_config(looper, ctx)

    def post_capture(self, looper, ctx, timedout):
        """Notification when the loop just finished capturing a glitch trigger.
        """
        self.glitch_disable(looper, ctx)

    def glitch_sequence(self, looper, ctx):
        """Default glitch loop sequence.

        Return:
            Value returned from scope.capture().
        """
        scope = ctx.scope
        self.pre_arm(looper, ctx)
        scope.arm()
        
        # Run target and capture trigger
        looper.driver.target_run(looper, ctx)
        timedout = scope.capture()
        self.post_capture(looper, ctx, timedout)
        return timedout

    def glitch_disable(self, looper, ctx):
        """Disables the glitch hardware.
        """
        ctx.scope.glitch_disable()

class GlitchLoopDefaultClkGlitch(GlitchLoopGlitchMethod):
    """Configures the scope to use the default clock glitch hardware settings.  This does not set
    default_setup, use GlitchDefaultTargetDriver to initialize default_setup settings.

    This enables and disables clock glitching hardware every glitch attempt.
    """

    def glitch_config(self, looper, ctx):
        pass

    def pre_arm(self, looper, ctx):
        ctx.scope.cglitch_setup(default_setup=False)

class GlitchLoopDefaultVCCGlitch(GlitchLoopGlitchMethod):
    """Base class to provide the functionality for VCC glitching.  This does not set default_setup,
    use GlitchDefaultTargetDriver to initialize default_setup settings.

    This configures the glitch mosfets each iteration and resets the mosfets immediately after
    capture.
    """

    @property
    def glitcht(self):
        """Gets the target glitch mode.  Can be either the VCC_EGLITCHT_* bitmask or the
        VCC_GLITCHT_* strings.

        Return:
            The VCC glitch mode to use.
        """
        raise NotImplementedError()

    def glitch_config(self, looper, ctx):
        pass

    def pre_arm(self, looper, ctx):
        ctx.scope.vglitch_setup(self.glitcht, default_setup=False)

    def post_capture(self, looper, ctx, timedout):
        ctx.scope.io.vglitch_reset()

class GlitchLoopDefaultVCCFastGlitch(GlitchLoopDefaultVCCGlitch):
    """Base class meant to implement a constant VCC glitch method.  This only configures glitch
    hardware before the loop and only resets the glitch mosfets after each capture iteration.

    This class can be initialized directly to specify a runtime configuration.
    """

    def __init__(self, glitcht):
        self._glitcht = glitcht

    @property
    def glitcht(self):
        return self._glitcht

    def glitch_config(self, looper, ctx):
        super().pre_arm(looper, ctx)

    def pre_arm(self, looper, ctx):
        pass

class GlitchLoopDefaultVLowGlitch(GlitchLoopDefaultVCCFastGlitch):
    """Class that statically sets the glitch method to use the low power mosfet only.
    """

    def __init__(self):
        pass

    @property
    def glitcht(self):
        return GPIOSettings.VCC_GLITCHT_LP

class GlitchLoopDefaultVHighGlitch(GlitchLoopDefaultVCCFastGlitch):
    """Class that statically sets the glitch method to use the high power mosfet only.
    """

    def __init__(self):
        pass

    @property
    def glitcht(self):
        return GPIOSettings.VCC_GLITCHT_HP

class GlitchLoopDefaultVBothGlitch(GlitchLoopDefaultVCCFastGlitch):
    """Class that statically sets the glitch method to use both the mosfets.
    """

    def __init__(self):
        pass

    @property
    def glitcht(self):
        return GPIOSettings.VCC_GLITCHT_BOTH

class GlitchLoopParamGenerator(object):
    """Base class to drive glitch parameter generation.
    """

    def gen_continue(self, looper, ctx):
        """Indicates if the parameter generator should continue generating sets.

        Return:
            True to continue generating sets, else False to break out of the loop.
        """
        return True

    def generate(self, looper, ctx):
        """Generates glitch parameters for the glitch loop.

        Return:
            An iterable object for parameter sets.
        """
        return ()

class GlitchLoopGCSeqParams(GlitchLoopParamGenerator):
    """Generates glitch parameters utilizing the GlitchLoopGCContext.gc GlitchController instance.
    """

    def generate(self, looper, ctx):
        for set in ctx.gc.glitch_values():
            if not self.gen_continue(looper, ctx):
                break
            yield set

class GlitchLoopGCRandParams(GlitchLoopParamGenerator):
    """Continuously generates random parameter sets utilizing the parameter ranges and steps from a
    GlitchLoopGCContext.gc GlitchController instance.
    """

    def generate(self, looper, ctx):
        for set in ctx.gc.rand_glitch_values():
            if not self.gen_continue(looper, ctx):
                break
            yield set

class GlitchLoopTargetDriver(object):
    """Base class to drive the target device and evaluate if a glitch was successful.
    """

    def target_config(self, looper, ctx):
        """Called before the glitch loop to configure and prep scope and target hardware for glitch loop.
        """
        pass

    def target_run(self, looper, ctx):
        """Called when glitch hardware is armed and ready to trigger target for the glitch. 
        """
        raise NotImplementedError()
        
    def process_glitch(self, looper, ctx, timedout):
        """Called after the glitch sequence happened and needs to evaluate if it was successful or not.

        Return:
            False to break out of the retry loop.  Any other return value continues the loop.
        """
        raise NotImplementedError()

class GlitchDefaultTargetDriver(object):
    @property
    def verbose_default_setup(self):
        return False

    def target_config(self, looper, ctx):
        # NOTE: default_setup() sets trigger to TIO4
        ctx.scope.default_setup(verbose=self.verbose_default_setup)

class GlitchLoopRunner(object):
    """Top level base class that drives the end to end glitch test sequence.
    """

    @property
    def params(self):
        """A parameter generation instance.

        Return:
            Default functionality expects a GlitchLoopParamGenerator instance.
        """
        return None
        
    @property
    def glitcher(self):
        """A glitch hardware driver instance.

        Return:
            Default functionality expects a GlitchLoopGlitchMethod instance.
        """
        return None
        
    @property
    def driver(self):
        """A target hardware driver instance.

        Return:
            Default functionality expects a GlitchLoopTargetDriver instance.
        """
        return None

    def pre_loop(self, ctx):
        """Called at the entry of the run_loop method.

        Return:
            False to immediate return from the loop.  Any other return executes the test loop.
        """
        pass

    def pre_glitch(self, ctx):
        """Called before executing the glitch iteration sequence.

        Return:
            False to break out of the retry loop.  Any other return continues the glitch sequence.
        """
        pass

    def post_loop(self, ctx):
        """Called after the glitch loop has finished.
        """
        self.glitcher.glitch_disable(self, ctx)
        
    def run_loop(self, ctx, retries=10):
        """This is the main method that executes all steps of the different components to perform
        an end to end glitch test.
        """
        if self.pre_loop(ctx) == False:
            return

        # Prep target and glitch configuration
        self.driver.target_config(self, ctx)
        self.glitcher.glitch_config(self, ctx)

        retries += 1
        for set in self.params.generate(self, ctx):
            if self.glitcher.set_params(self, ctx, set) == False:
                continue
                
            remain = retries
            while remain > 0:
                if self.pre_glitch(ctx) == False:
                    break

                try:
                    # Run glitch sequence
                    timedout = self.glitcher.glitch_sequence(self, ctx)
                except:
                    # Disable the glitch on error
                    self.glitcher.glitch_disable(self, ctx)
                    raise

                # Process glitch and analyze result
                if self.driver.process_glitch(self, ctx, timedout) == False: # ex. detect and log result
                    break

                remain -= 1

        self.post_loop(ctx)