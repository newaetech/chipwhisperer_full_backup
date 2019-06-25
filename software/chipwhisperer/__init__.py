# -*- coding: utf-8 -*-
"""
.. module:: chipwhisperer
   :platform: Unix, Windows
   :synopsis: Test

.. moduleauthor:: NewAE

Main module for ChipWhisperer.
"""
import os, os.path, time
import warnings

from chipwhisperer.capture import scopes, targets
from chipwhisperer.capture.api import programmers
from chipwhisperer.capture import acq_patterns as key_text_patterns
from chipwhisperer.common.utils.util import camel_case_deprecated
# from chipwhisperer.capture.scopes.cwhardware import ChipWhispererSAM3Update as CWFirmwareUpdate
ktp = key_text_patterns #alias
from chipwhisperer.common.utils.util import camel_case_deprecated

def program_target(scope, prog_type, fw_path):
    """Program the target using the programmer <type>

    Programmers can be found in the programmers submodule

    Args:
       scope (ScopeTemplate): Connected scope object to use for programming
       prog_type (Programmer): Programmer to use. See chipwhisperer.programmers
           for available programmers
       fw_path (str): Path to hex file to program

    .. versionadded:: 5.0.1
    Simplified programming target
    """
    if type is None: #makes automating notebooks much easier
        return
    prog = prog_type()
    prog.scope = scope
    prog._logging = None
    prog.open()
    prog.find()
    prog.erase()
    prog.program(fw_path, memtype="flash", verify=True)
    prog.close()


programTarget = camel_case_deprecated(program_target)


def open_project(filename):
    """Load an existing project from disk.

    Raise an OSError if no such project exists.

    Args:
       filename (str): Path to project file.

    Returns:
       A chipwhisperer project object.

    Raises:
       OSError: filename does not exist.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    if not os.path.isfile(filename):
        raise OSError("File " + filename + " does not exist or is not a file")
    proj = project.ProjectFormat()
    proj.load(filename)
    return proj


openProject = camel_case_deprecated(open_project)


def create_project(filename, overwrite=False):
    """Create a new project with the path <filename>.

    If <overwrite> is False, raise an OSError if this path already exists.

    Args:
       filename (str): File path to create project file at. Must end with .cwp
       overwrite (bool, optional): Whether or not to overwrite an existing
           project with <filename>. Raises an OSError if path already exists
           and this is false. Defaults to false.

    Returns:
       A chipwhisperer project object.

    Raises:
       OSError: filename exists and overwrite is False.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    if os.path.isfile(filename) and (overwrite == False):
        raise OSError("File " + filename + " already exists")

    proj = project.ProjectFormat()
    proj.setFilename(filename)

    return proj


createProject = camel_case_deprecated(create_project)


def scope(scope_type=scopes.OpenADC, sn=None):
    """Create a scope object and connect to it.

    This function allows any type of scope to be created. By default, the scope
    is a ChipWhisperer OpenADC object, but this can be set to any valid scope
    class.

    Scope Types:
     * :class:`scopes.OpenADC` (Pro and Lite)
     * :class:`scopes.CWNano` (Nano)

    If multiple chipwhisperers are connected, the serial number of the one you
    want to connect to can be specified by passing sn=<SERIAL_NUMBER>

    Args:
       scope_type (ScopeTemplate, optional): Scope type to connect to. Types
           can be found in chipwhisperer.scopes. Defaults to scopes.OpenADC
           (correct for CWLite and CWPro).
       sn (str, optional): Serial number of ChipWhisperer that you want to
           connect to. Required if more than one ChipWhisperer
           of the same type is connected (i.e. two CWNano's or a CWLite and
           CWPro). Defaults to None.

    Returns:
       Connected scope object specified by scope_type

    Raises:
       OSError: Can be raised for issues connecting to the chipwhisperer, such
           as not having permission to access the USB device or no ChipWhisperer
           being connected.
    """
    scope = scope_type()
    scope.con(sn)
    return scope


def target(scope, target_type = targets.SimpleSerial, **kwargs):
    """Create a target object and connect to it.

    Args:
       scope (ScopeTemplate): Scope object that we're connecting to the target
           through.
       target_type (TargetTemplate, optional): Target type to connect to.
           Defaults to targets.SimpleSerial. Types can be found in
           chipwhisperer.targets.
       **kwargs: Additional keyword arguments to pass to target setup. Rarely
           needed.

    Returns:
        Connected target object specified by target_type.
    """
    target = target_type()
    target.con(scope, **kwargs)
    return target


def capture_trace(scope, target, plaintext, key=None):
    """Capture a trace, sending plaintext and key

    Does all individual steps needed to capture a trace (arming the scope
    sending the key/plaintext, getting the trace data back, etc.)

    Args:
        scope (ScopeTemplate): Scope object to use for capture.
        target (TargetTemplate): Target object to read/write text from.
       plaintext (bytearray): Plaintext to send to the target. Should be
            unencoded ByteArray (will be converted to SimpleSerial when it's
            sent). If None, don't send plaintext.
       key (bytearray, optional): Key to send to target. Should be unencoded
            ByteArray. If None, don't send key. Defaults to None.

    Returns:
        Tuple of scope_data (numpy.ndarray) and response (ByteArray) or None
        if capture timed out.

    Raises:
        Warning or OSError: Error during capture.

    Example:
        Capturing a trace::

            import chipwhisperer as cw
            scope = cw.scope()
            scope.defaultSetup()
            target = cw.target()
            ktp = cw.ktp.Basic()
            key, pt = ktp.newPair()
            trace, response = cw.capture_trace(scope, target, plaintext, key)

    .. versionadded:: 5.1
        Added to simplify trace capture.
    """
    if key:
        target.simpleserial_write('k', key)
        target.simpleserial_wait_ack()

    scope.arm()

    if plaintext:
        target.simpleserial_write('p', plaintext)

    while not target.isDone():
        time.sleep(0.01)

    ret = scope.capture()
    if ret:
        warnings.warn("Timeout happened during capture")
        return None

    response = target.simpleserial_read('r', 16)
    trace = scope.getLastTrace()

    return trace, response


captureTrace = camel_case_deprecated(capture_trace)
