#!/bin/sh

# create directories
start_dir=$(pwd)
cw_dir="$start_dir""/software/chipwhisperer"
min_dir="$start_dir""/chipwhisperer-minimal/chipwhisperer"
mkdir -p "$min_dir"
cd "$min_dir"

for i in hardware/naeusb hardware/firmware capture/targets capture/api common/utils capture/scopes/cwhardware capture/targets/simpleserial_readers; do
    mkdir -p "$i"
    touch "$i"/__init__.py
done

# create __init__.py
for i in hardware capture common capture/scopes; do
    touch "$i"/__init__.py
done

# copy naeusb files
for i in naeusb.py fpga.py bootloader_sam3u.py pll_cdce906.py serial.py spi.py programmer_targetfpga.py; do
    cp "$cw_dir""/hardware/naeusb/""$i" "$min_dir""/hardware/naeusb/""$i"
done

# copy firmware files
for i in cwbergen.py cwlite.py cw305.py cw1200.py cwnano.py cwhusky.py; do
    cp "$cw_dir""/hardware/firmware/""$i" "$min_dir""/hardware/firmware/""$i"
done
# cp "$cw_dir""/hardware/firmware/""cwbergen.py" "$min_dir""/hardware/firmware/""cwbergen.py"

# copy target files
for i in _base.py CW305.py CW310.py SimpleSerial2.py simpleserial_readers/_base.py simpleserial_readers/cwlite.py; do
    cp "$cw_dir""/capture/targets/""$i" "$min_dir""/capture/targets/""$i"
done

cp "$cw_dir""/common/utils/""util.py" "$min_dir""/common/utils/""util.py"

cp "$cw_dir""/capture/scopes/cwhardware/""ChipWhispererSAM3Update.py" "$min_dir""/capture/scopes/cwhardware/""ChipWhispererSAM3Update.py"

cp "$cw_dir""/capture/api/""cwcommon.py" "$min_dir""/capture/api/""cwcommon.py"

cp "$cw_dir""/logging.py" "$min_dir""/logging.py"

cp "$cw_dir""/version.py" "$min_dir""/version.py"

cp "$cw_dir""/../../README.md" "$min_dir""/../README.md"


# create setup.py
cd "$start_dir"/chipwhisperer-minimal

echo "#!/usr/bin/env python" > setup.py
echo "" >> setup.py
echo "from setuptools import setup, find_packages" >> setup.py
echo "" >> setup.py

echo "__version__ = '5.7.0'" >> setup.py
echo "with open('chipwhisperer/version.py') as f:" >> setup.py
echo "    exec(f.read())" >> setup.py
echo "setup(" >> setup.py
echo "    name='chipwhisperer'," >> setup.py
echo "    version=__version__," >> setup.py
echo "    description='ChipWhisperer Side-Channel Analysis Tool'," >> setup.py
echo "    long_description=open('README.md').read()," >> setup.py
echo "    long_description_content_type='text/markdown'," >> setup.py
echo "    author='NewAE Technology Inc'," >> setup.py
echo "    author_email='sales@newae.com'," >> setup.py
echo "    license='apache'," >> setup.py
echo "    url='https://www.chipwhisperer.com'," >> setup.py
echo "    packages=find_packages('.')," >> setup.py
echo "    package_dir={'': '.'}," >> setup.py
echo "    install_requires=[" >> setup.py
echo "        'pyserial'," >> setup.py
echo "        'libusb1'," >> setup.py
echo "        'Cython'," >> setup.py
echo "    ]," >> setup.py
echo "    project_urls={" >> setup.py
echo "        'Documentation': 'https://chipwhisperer.readthedocs.io'," >> setup.py
echo "        'Source': 'https://github.com/newaetech/chipwhisperer-minimal'," >> setup.py
echo "        'Issue Tracker': 'https://github.com/newaetech/chipwhisperer-minimal/issues'," >> setup.py
echo "    }," >> setup.py
echo "    python_requires='~=3.7'," >> setup.py
echo ")" >> setup.py

cd "$min_dir"
# create project __init__.py
echo "" > __init__.py
echo "from .capture.targets import CW310 as targets" >> __init__.py
echo "from .capture.scopes.cwhardware.ChipWhispererSAM3Update import SAMFWLoader, get_at91_ports" >> __init__.py
echo "from .logging import *" >> __init__.py
echo "def target(scope, target_type, **kwargs):" >> __init__.py
echo "    rtn = target_type()" >> __init__.py
echo "    rtn.con(scope, **kwargs)" >> __init__.py
echo "    return rtn" >> __init__.py
echo "" >> __init__.py
echo "def program_sam_firmware(serial_port, hardware_type, fw_path):" >> __init__.py
echo "    if (hardware_type, fw_path) == (None, None):" >> __init__.py
echo "        raise ValueError('Must specify hardware_type or fw_path, see https://chipwhisperer.readthedocs.io/en/latest/firmware.html')" >> __init__.py
echo "    if serial_port is None:" >> __init__.py
echo "        at91_ports = get_at91_ports()" >> __init__.py
echo "        if len(at91_ports) == 0:" >> __init__.py
echo "            raise OSError('Could not find bootloader serial port, please see https://chipwhisperer.readthedocs.io/en/latest/firmware.html')" >> __init__.py
echo "        if len(at91_ports) > 1:" >> __init__.py
echo "            raise OSError('Found multiple bootloaders, please specify com port. See https://chipwhisperer.readthedocs.io/en/latest/firmware.html')" >> __init__.py
echo "        serial_port = at91_ports[0]" >> __init__.py
echo "        print('Found {}'.format(serial_port))" >> __init__.py
echo "    prog = SAMFWLoader(None)" >> __init__.py
echo "    prog.program(serial_port, hardware_type=hardware_type, fw_path=fw_path)" >> __init__.py
