################################################################################
# Vivado (TM) v2019.1 (64-bit)
#
# README.txt: Please read the sections below to understand the steps required
#             to simulate the design for a simulator, the directory structure
#             and the generated exported files.
#
################################################################################

1. Simulate Design

To simulate design, cd to the simulator directory and execute the script.

For example:-

% cd questa
% ./top.sh

The export simulation flow requires the Xilinx pre-compiled simulation library
components for the target simulator. These components are referred using the
'-lib_map_path' switch. If this switch is specified, then the export simulation
will automatically set this library path in the generated script and update,
copy the simulator setup file(s) in the exported directory.

If '-lib_map_path' is not specified, then the pre-compiled simulation library
information will not be included in the exported scripts and that may cause
simulation errors when running this script. Alternatively, you can provide the
library information using this switch while executing the generated script.

For example:-

% ./top.sh -lib_map_path /design/questa/clibs

Please refer to the generated script header 'Prerequisite' section for more details.

2. Directory Structure

By default, if the -directory switch is not specified, export_simulation will
create the following directory structure:-

<current_working_directory>/export_sim/<simulator>

For example, if the current working directory is /tmp/test, export_simulation
will create the following directory path:-

/tmp/test/export_sim/questa

If -directory switch is specified, export_simulation will create a simulator
sub-directory under the specified directory path.

For example, 'export_simulation -directory /tmp/test/my_test_area/func_sim'
command will create the following directory:-

/tmp/test/my_test_area/func_sim/questa

By default, if -simulator is not specified, export_simulation will create a
simulator sub-directory for each simulator and export the files for each simulator
in this sub-directory respectively.

IMPORTANT: Please note that the simulation library path must be specified manually
in the generated script for the respective simulator. Please refer to the generated
script header 'Prerequisite' section for more details.

3. Exported script and files

Export simulation will create the driver shell script, setup files and copy the
design sources in the output directory path.

By default, when the -script_name switch is not specified, export_simulation will
create the following script name:-

<simulation_top>.sh  (Unix)
When exporting the files for an IP using the -of_objects switch, export_simulation
will create the following script name:-

<ip-name>.sh  (Unix)
Export simulation will create the setup files for the target simulator specified
with the -simulator switch.

For example, if the target simulator is "ies", export_simulation will create the
'cds.lib', 'hdl.var' and design library diectories and mappings in the 'cds.lib'
file.

