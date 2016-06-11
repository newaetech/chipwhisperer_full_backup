#
# This script packs firmware files into Python modules to allow easy distribution
#
import os
import base64
import datetime

hwdir = r"../../../../hardware/"

#List of file-names & object name
cw305_files = []
cwcr2_files = [("cwrev2_firmware.zip",  os.path.join(hwdir, r"capture/chipwhisperer-rev2/cwrev2_firmware.zip")),
               ("OpenADC.ihx", os.path.join(hwdir, r"capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"))]
cwlite_files = [("cwlite_firmware.zip", os.path.join(hwdir, r"capture/chipwhisperer-lite/cwlite_firmware.zip")),
                ("SAM3U_CW1173.bin", os.path.join(hwdir, r"capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin"))]

#List of files to generate
file_list = [("cw305.py",cw305_files), ("cwcr2.py",cwcr2_files), ("cwlite.py",cwlite_files)]

for fdata in file_list:
    f = open(fdata[0], "w")

    f.write("# This file was auto-generated. Do not manually edit or save. What are you doing looking at it? Close it now!\n")
    f.write("# Generated on %s\n"%datetime.datetime.now())
    f.write("#\n")
    f.write("import base64\n")
    f.write("import StringIO\n\n")
    f.write("def getsome(item, filelike=True):\n")
    f.write("    data = _contents[item]\n")
    f.write("    data = base64.b64decode(data)\n")
    f.write("    if filelike:\n")
    f.write("        data = StringIO.StringIO(data)\n")
    f.write("    return data\n\n")
    f.write("_contents = {\n")
    
    for embdata in fdata[1]:
        with open(embdata[1], "rb") as e_file:
            json_str = base64.b64encode(e_file.read())# json.dumps(e_file.read(), ensure_ascii=False)

            f.write("\n#Contents from %s\n"%embdata[1])
            f.write("'%s':'"%embdata[0])
            f.write(json_str)
            f.write("',\n\n")
            f.flush()
    f.write("}\n")

f.close()
