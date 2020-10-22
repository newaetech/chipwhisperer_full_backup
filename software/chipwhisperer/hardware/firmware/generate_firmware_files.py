#
# This script packs firmware files into Python modules to allow easy distribution
#
import os
import base64
import datetime
import binascii

hwdir = r"../../../../hardware/"

#List of versions, file-names, and object name
cw305_v = [0, 32]
cw305_files = [("SAM3U_CW305.bin", os.path.join(hwdir, r"victims/cw305_artixtarget/fw/sam3u/CW305_SAM3U_FW/build/CW305_SAM3UFW.bin")),
                ("SPI_flash_100t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/spiflash_feedthrough/spiflash_feedthrough.runs/impl_100t/cw305_top.bit")),
                ("SPI_flash_35t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/spiflash_feedthrough/spiflash_feedthrough.runs/impl_35t/cw305_top.bit")),
                ("AES_100t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_verilog/aes128_verilog.runs/impl_100t/cw305_top.bit")),
                ("AES_35t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_verilog/aes128_verilog.runs/impl_35t/cw305_top.bit")),
                ("cw305_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/common/cw305_defines.v")),
                ("ECDSA256v1_pmul_100t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/ecc_p256_pmul.runs/impl_100t/cw305_ecc_p256_pmul_top.bit")),
                ("ECDSA256v1_pmul_35t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/ecc_p256_pmul.runs/impl_35t/cw305_ecc_p256_pmul_top.bit")),
                ("cw305_pmul_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/hdl/cw305_pmul_defines.v")),
                ]

cwcr2_v = [0, 11]
cwcr2_files = [("cwrev2_firmware.zip",  os.path.join(hwdir, r"capture/chipwhisperer-rev2/cwrev2_firmware.zip")),
               ("OpenADC.ihx", os.path.join(hwdir, r"capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"))]

cwlite_v = [0, 22]
cwlite_files = [("cwlite_firmware.zip", os.path.join(hwdir, r"capture/chipwhisperer-lite/cwlite_firmware.zip")),
                ("SAM3U_CW1173.bin", os.path.join(hwdir, r"capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin"))]

cw1200_v = [1, 22]
cw1200_files = [("cw1200_firmware.zip", os.path.join(hwdir, r"capture/chipwhisperer-cw1200/cw1200_firmware.zip")),
                ("CW1200_SAM3UFW.bin", os.path.join(hwdir, r"capture/chipwhisperer-cw1200/CW1200_SAM3UFW.bin"))]

cwnano_v = [0, 22]
#hardware\capture\chipwhisperer-nano\firmware\cwnano-firmware\Debug\cwnano-firmware.bin
cwnano_files = [("SAM3U_CWNANO.bin", os.path.join(hwdir, r"capture/chipwhisperer-nano/firmware/cwnano-firmware/Debug/cwnano-firmware.bin"))]
#List of files to generate
file_list = [
     ("cwnano.py" , cwnano_v , cwnano_files),
    ("cw305.py" , cw305_v , cw305_files),
#    ("cwcr2.py" , cwcr2_v , cwcr2_files),
    ("cwlite.py", cwlite_v, cwlite_files),
    ("cw1200.py", cw1200_v, cw1200_files),
]

for fdata in file_list:
    f = open(fdata[0], "w")

    f.write("# This file was auto-generated. Do not manually edit or save. What are you doing looking at it? Close it now!\n")
    f.write("# Generated on %s\n"%datetime.datetime.now())
    f.write("#\n")
    f.write("import binascii\n")
    f.write("import io\n\n")
    f.write("fwver = [%d, %d]\n" % (fdata[1][0], fdata[1][1]))
    f.write("def getsome(item, filelike=True):\n")
    f.write("    data = _contents[item].encode('latin-1')\n")
    f.write("    data = binascii.a2b_base64(data)\n")
    f.write("    if filelike:\n")
    f.write("        data = io.BytesIO(data)\n")
    f.write("    return data\n\n")
    f.write("_contents = {\n")

    f.write("")

    for embdata in fdata[2]:
        with open(embdata[1], "rb") as e_file:
            # json_str = base64.b64encode(e_file.read())# json.dumps(e_file.read(), ensure_ascii=False)
            json_str = binascii.b2a_base64(e_file.read())

            f.write("\n#Contents from %s\n"%embdata[1])
            f.write("'%s':'"%embdata[0])
            f.write(json_str.decode().replace("\n",""))
            f.write("',\n\n")
            f.flush()
    f.write("}\n")

f.close()
