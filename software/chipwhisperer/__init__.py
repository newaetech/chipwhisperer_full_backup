# -*- coding: utf-8 -*-
"""
.. module:: chipwhisperer
   :platform: Unix, Windows
   :synopsis: Test

.. moduleauthor:: NewAE Technology Inc.

Main module for ChipWhisperer.
"""
import os, os.path, time
import warnings
from zipfile import ZipFile

from chipwhisperer.capture import scopes, targets
from chipwhisperer.capture.api import programmers
from chipwhisperer.capture import acq_patterns as key_text_patterns
from chipwhisperer.common.utils.util import camel_case_deprecated
from chipwhisperer.common.api import ProjectFormat as project
from chipwhisperer.common.api.ProjectFormat import PROJECT_DIR
from chipwhisperer.common.traces import Trace
# from chipwhisperer.capture.scopes.cwhardware import ChipWhispererSAM3Update as CWFirmwareUpdate
ktp = key_text_patterns #alias

def program_target(scope, prog_type, fw_path, **kwargs):
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
    if prog_type is None: #[makes] automating notebooks much easier
        return
    prog = prog_type(**kwargs)
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

    Args:
       filename (str): Path to project file.

    Returns:
       A chipwhisperer project object.

    Raises:
       OSError: filename does not exist.
    """
    from chipwhisperer.common.api import ProjectFormat as project
    proj = project.Project()
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
    filename = project.ensure_cwp_extension(filename)
    if os.path.isfile(filename) and (overwrite == False):
        raise OSError("File " + filename + " already exists")

    # If the user gives a relative path including ~, expand to the absolute path
    filename = os.path.expanduser(filename)

    if not os.path.isabs(filename):
        filename = os.path.join(PROJECT_DIR, filename)

    proj = project.Project()
    proj.setFilename(filename)

    return proj


createProject = camel_case_deprecated(create_project)


def import_project(filename, file_type='zip', overwrite=False):
    """Import and open a project.

    Will import the **filename** by extracting to the project
    directory, defined as '~/chipwhisperer/projects'. On Unix based
    systems '~' expands to '/home/user/' directory. On Windows it
    expands to 'C:\\Users\\User'.

    Currently support file types:
     * zip

    Args:
        filename (str): The file name to import.
        file_type (str): The type of file that is being imported.
            Default is zip.
        overwrite (bool): Whether or not to overwrite the project given as
            the **import_as** project.

    .. versionadded:: 5.1
        Add **import_project** function.
    """
    # extract name from input file
    input_dir, input_file = os.path.split(filename)
    input_file_root, input_file_ext = os.path.splitext(input_file)
    input_abs_path = os.path.abspath(filename)

    # use the appropriate type of import
    if file_type == 'zip':
        with ZipFile(input_abs_path, 'r') as project_zip:
            output_path = None
            for path in project_zip.namelist():
                root, ext = os.path.splitext(path)
                if ext == '.cwp':
                    directory, project_name = os.path.split(root)
                    output_path = os.path.join(PROJECT_DIR, ''.join([project_name, '.cwp']))

                    # check if name already exists in projects
                    if os.path.isfile(output_path) and (overwrite == False):
                        raise OSError("File " + output_path + " already exists")

                    # extract the project.cwp file and project_data directory to
                    # the PROJECT_DIR
                    project_zip.extractall(path=PROJECT_DIR)

            if output_path is None:
                raise ValueError('Zipfile does not contain a .cwp file, so it cannot be imported')
    else:
        raise ValueError('Import from file type not supported: {}'.format(file_type))

    proj = project.Project()
    proj.load(output_path)

    return proj


def scope(scope_type=None, sn=None):
    """Create a scope object and connect to it.

    This function allows any type of scope to be created. By default, the
    object created is based on the attached hardware (OpenADC for
    CWLite/CW1200, CWNano for CWNano).

    Scope Types:
     * :class:`scopes.OpenADC` (Pro and Lite)
     * :class:`scopes.CWNano` (Nano)

    If multiple chipwhisperers are connected, the serial number of the one you
    want to connect to can be specified by passing sn=<SERIAL_NUMBER>

    Args:
       scope_type (ScopeTemplate, optional): Scope type to connect to. Types
           can be found in chipwhisperer.scopes. If None, will try to detect
           the type of ChipWhisperer connected. Defaults to None.
       sn (str, optional): Serial number of ChipWhisperer that you want to
           connect to. Required if more than one ChipWhisperer
           of the same type is connected (i.e. two CWNano's or a CWLite and
           CWPro). Defaults to None.

    Returns:
        Connected scope object.

    Raises:
        OSError: Can be raised for issues connecting to the chipwhisperer, such
            as not having permission to access the USB device or no ChipWhisperer
            being connected.
        Warning: Raised if multiple chipwhisperers are connected, but the type
            and/or the serial numbers are not specified

    .. versionchanged:: 5.1
        Added autodetection of scope_type
    """
    from chipwhisperer.common.utils.util import get_cw_type
    if scope_type is None:
        scope_type = get_cw_type(sn)
    scope = scope_type()
    scope.con(sn)
    return scope


def target(scope, target_type=targets.SimpleSerial, **kwargs):
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
            unencoded bytearray (will be converted to SimpleSerial when it's
            sent). If None, don't send plaintext.
        key (bytearray, optional): Key to send to target. Should be unencoded
            bytearray. If None, don't send key. Defaults to None.

    Returns:
        Tuple of scope_data (numpy.ndarray) and response (bytearray) or None
        if capture timed out.

    Raises:
        Warning or OSError: Error during capture.

    Example:
        Capturing a trace::

            import chipwhisperer as cw
            scope = cw.scope()
            scope.default_setup()
            target = cw.target()
            ktp = cw.ktp.Basic()
            key, pt = ktp.new_pair()
            trace, response = cw.capture_trace(scope, target, pt, key)

    .. versionadded:: 5.1
        Added to simplify trace capture.
    """
    if key:
        target.set_key(key)

    scope.arm()

    if plaintext:
        target.simpleserial_write('p', plaintext)

    ret = scope.capture()
    if ret:
        warnings.warn("Timeout happened during capture")
        return None

    response = target.simpleserial_read('r', 16)
    wave = scope.get_last_trace()

    return Trace(wave, plaintext, response, key)


captureTrace = camel_case_deprecated(capture_trace)


