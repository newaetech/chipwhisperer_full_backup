#!/usr/bin/env python3

import argparse
import subprocess
import random
import re
import time
import os
from tqdm import tqdm

parser = argparse.ArgumentParser()
group = parser.add_mutually_exclusive_group()
group.add_argument("--runs", type=int, help="Number of iterations.", default=1)
group.add_argument("--test", help="Testcase to run")
parser.add_argument("--seed", type=int, help="Seed to use when running a single test with --test.")
parser.add_argument("--tests", help="Run all tests whose name contains TESTS", default='')
parser.add_argument("--exclude", help="Exclude tests whose name contains TESTS", default='')
parser.add_argument("--list", help="List available tests.", action='store_true')
parser.add_argument("--dump", help="Enable waveform dumping.", action='store_true')
parser.add_argument("--proc", type=int, help="Maximum number of parallel jobs to dispatch.", default=32)
parser.add_argument("--cocodebug", help="Cocotb debug level", default='INFO')
args = parser.parse_args()

# Optionally compile only once, for faster regressions (which prevents changing defines such as TINYFIFO from their defaults):

# Define testcases:
tests = []
tests.append(dict(name  = 'basic',
             testcase = 'ss2_test',
             frequency = 1,
             PERIOD = 1000,
             REPS = 20,
             description = ''))


def print_tests():
    print("Available tests:")
    for test in tests:
       print("%s: %s" % (test['name'], test['description']))
    quit()

def check_pass_fail(logfile):
    log = open(logfile, 'r')
    passed = None
    failed = None
    warnings = 0
    errors = 0
    for line in log:
       stat_matches = stat_regex.search(line)
       if stat_matches:
          #print("CPF MATCHED ON LINE: %s" % line)
          passed = int(stat_matches.group(2))
          failed = int(stat_matches.group(3))
          break
       #else:
       #    print("CPF couldn't parse line: %s" % line)
    log.close()
    if passed is None:
        print("*** parsing error on %s ***" % logfile)
    if failed:
        errors = 1
    # TODO: count warnings and errors
    return passed, warnings, errors


if (args.list):
    print_tests()
    quit()

# if running a single testcase:
if (args.test):
   found = False
   for test in tests:
      if test['name'] == args.test:
         found = True
         tests = [test]
         break
   if not found:
      print("Error: test %s undefined." % args.test)
      print_tests()


stat_regex = re.compile(r'TESTS=(\d+) PASS=(\d+) FAIL=(\d+) SKIP=(\d+)')
test_regex = re.compile(args.tests)
exclude_regex = re.compile(args.exclude)

exefile = 'coco.vvp'
outfile = open('coco_compile.out', 'w')
makeargs = ['make', 'compile_ss2', 'EXEFILE=%s' % exefile]
if args.dump:
    makeargs.append('DUMP=1')
result = subprocess.run(makeargs, stdout=outfile, stderr=outfile)
if result.returncode:
    print ("Compilation for failed (return code: %d), check coco_compile.out." % result.returncode)
    quit()

# Run tests:
start_time = int(time.time())
processes = []
jobs_to_submit = []
exefiles = []

print("Building list of jobs... ", end='')
for test in tests:
   if args.tests:
      if test_regex.search(test['name']) == None:
          continue
   if args.exclude:
      if exclude_regex.search(test['name']) != None:
          continue

   for i in range(args.runs):

      make_target = 'all'
      makeargs = ['make', make_target, 'TESTCASE=%s' % test['testcase'], 'COCOTB_LOG_LEVEL=%s' % args.cocodebug]
      if (args.seed):
          seed = args.seed
      else:
          # not easy/clean to get cocotb-generated seed from logfile, so force ours instead
          seed = random.randint(0, 2**31-1)
      makeargs.append("RANDOM_SEED=%d" % seed)

      run_test = True
      # build make command:
      for key in test.keys():
         if key == 'name':
            logfile = "results/%s%d.log" % (test[key], i) 
            exefile = "results/%s%d.vvp" % (test[key], i) 
            if args.dump:
               makeargs.append('DUMP=1')
            makeargs.append("LOGFILE=%s" % logfile)
            makeargs.append("EXEFILE=%s" % exefile)
         elif key == 'description':
            pass
         elif key == 'frequency':
            if test[key] == 0:
               run_test = False
            elif i % test[key]:
               run_test = False
         else:
            if type(test[key]) == list:
               value = random.randint(test[key][0], test[key][1])
            else:
               value = test[key]
            makeargs.append("%s=%s" % (key, value))

      # run:
      if run_test:
         #print("Adding job: %s" % makeargs)
         jobs_to_submit.append((makeargs, logfile, outfile, seed))
         exefiles.append(exefile)

num_processes = len(jobs_to_submit)
print("done. %d tests to run." % num_processes)

warns = []
fails = []

pbar_dispatched = tqdm(total=len(jobs_to_submit), desc='Tests dispatched')
pbar_finished   = tqdm(total=len(jobs_to_submit), desc='Tests finished ')
pbar_passed     = tqdm(total=len(jobs_to_submit), desc='Tests passing  ')

# submit first batch of jobs (up to args.proc):
if args.proc > len(jobs_to_submit):
    num_first_batch = len(jobs_to_submit)
else:
    num_first_batch = args.proc
for i in range(num_first_batch):
    makeargs, logfile, outfile, seed = jobs_to_submit.pop()
    p = subprocess.Popen(makeargs, stdout=outfile, stderr=outfile)
    processes.append((p,logfile,seed))

pbar_dispatched.update(num_first_batch)
pbar_dispatched.refresh()

oldfinished = 0
finished = 0
oldpass_count = 0
fail_count = 0
pass_count = 0
while len(processes):
    for p,l,s in processes:
        if not p.poll() is None:
            finished += 1
            passed, warnings, errors = check_pass_fail(l)
            pass_count += passed
            if warnings:
                warns.append("%s: %0d warnings" % (l, warnings))
            if errors:
                fail_count += 1
                fails.append("%s: %d errors (seed=%d)" % (l, errors, s))
            processes.remove((p,l,s))
            if len(jobs_to_submit) > 0:
                makeargs, logfile, outfile, seed = jobs_to_submit.pop()
                p = subprocess.Popen(makeargs, stdout=outfile, stderr=outfile)
                processes.append((p,logfile,seed))
                pbar_dispatched.update(1)
                pbar_dispatched.refresh()

    pbar_finished.update(finished - oldfinished)
    pbar_passed.update(pass_count - oldpass_count)
    oldfinished = finished
    oldpass_count = pass_count
    time.sleep(1)

pbar_dispatched.close()
pbar_finished.close()
pbar_passed.close()

# just to be sure:
#exit_codes = [p.wait() for p,l,s in processes]

# sanity check:
assert num_processes == pass_count + fail_count, "pass=%d, fail=%d" % (pass_count, fail_count)

# Summarize results:
print('\n*** RESULTS SUMMARY ***')
print('%d tests passing, %d tests failing.' % (pass_count, fail_count))
print('Elapsed time: %d seconds' % (int(time.time() - start_time)))
if warns:
    for w in warns:
        print(w)
if fails:
    for f in fails:
        print(f)

# clean up .vvp files:
for v in exefiles:
    try:
        os.remove(v)
    except OSError as e:
        print("Error removing %s: %s" % (v, e))

