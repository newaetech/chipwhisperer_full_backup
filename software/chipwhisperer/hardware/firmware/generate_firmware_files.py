#
# This script packs firmware files into Python modules to allow easy distribution
#
import os
import base64
import datetime
import binascii
import pathlib

hwdir = r"../../../../hardware/"

#List of versions, file-names, and object name
cw305_v = [0, 53]
cw305_files = [("SAM3U_CW305.bin", os.path.join(hwdir, r"victims/cw305_artixtarget/fw/sam3u/CW305_SAM3U_FW/src/ChipWhisperer-CW305.bin")),
                ("SPI_flash_100t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/spiflash_feedthrough/spiflash_feedthrough.runs/impl_100t/cw305_top.bit")),
                ("SPI_flash_35t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/spiflash_feedthrough/spiflash_feedthrough.runs/impl_35t/cw305_top.bit")),
                ("AES_100t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_verilog/aes128_verilog.runs/impl_100t/cw305_top.bit")),
                ("AES_35t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_verilog/aes128_verilog.runs/impl_35t/cw305_top.bit")),
                ("cw305_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/common/cw305_defines.v")),
                ("ECDSA256v1_pmul_100t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/ecc_p256_pmul.runs/impl_100t/cw305_ecc_p256_pmul_top.bit")),
                ("ECDSA256v1_pmul_35t.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/ecc_p256_pmul.runs/impl_35t/cw305_ecc_p256_pmul_top.bit")),
                ("cw305_pmul_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/hdl/cw305_pmul_defines.v")),
                ("Pipelined_AES_100t_half0.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_full/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_100t_half1.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_half1/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_100t_half2.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_half2/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_100t_half3.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_half3/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_35t_half0.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_full_35t/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_35t_half1.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_half1_35t/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_35t_half2.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_half2_35t/cw305_aes_pipelined_top.bit")),
                ("Pipelined_AES_35t_half3.bit", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/aes128_pipelined.runs/impl_half3_35t/cw305_aes_pipelined_top.bit")),
                ("cw305_aes_pipelined_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/hdl/cw305_aes_pipelined_defines.v")),
                ("version", os.path.join(hwdir, r"victims/cw305_artixtarget/fw/sam3u/CW305_SAM3U_FW/src/version.txt"))
                ]

cwlite_v = [0, 64]
cwlite_files = [("cwlite_firmware.zip", os.path.join(hwdir, r"capture/chipwhisperer-lite/cwlite_firmware.zip")),
                ("SAM3U_CW1173.bin", os.path.join(hwdir, r"capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/src/ChipWhisperer-Lite.bin")),
                ("version", os.path.join(hwdir, r"capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/src/version.txt"))
                ]

cw1200_v = [1, 62]
cw1200_files = [("cw1200_firmware.zip", os.path.join(hwdir, r"capture/chipwhisperer-cw1200/cw1200_firmware.zip")),
                ("CW1200_SAM3UFW.bin", os.path.join(hwdir, r"capture/chipwhisperer-cw1200/ChipWhisperer-Pro.bin")),
                ("version", os.path.join(hwdir, r"capture/chipwhisperer-cw1200/version.txt"))
                ]

cwhusky_v = [1, 5]
cwhusky_files = [("husky_firmware.zip", os.path.join(hwdir, r"capture/chipwhisperer-husky/husky_firmware.zip")),
                ("Husky.bin", os.path.join(hwdir, r"capture/chipwhisperer-husky/ChipWhisperer-Husky.bin")),
                ("version", os.path.join(hwdir, r"capture/chipwhisperer-husky/version.txt"))
                ]

cwbergen_v = [1, 2]
cwbergen_files = [
    ("CW310.bin", os.path.join(hwdir, r"victims/cw310_bergenboard/CW310.bin")),
    ("version", os.path.join(hwdir, r"victims/cw310_bergenboard/version.txt"))
                  ]

cwnano_v = [0, 65]
#hardware\capture\chipwhisperer-nano\firmware\cwnano-firmware\Debug\cwnano-firmware.bin
cwnano_files = [
    ("SAM3U_CWNANO.bin", os.path.join(hwdir, r"capture/chipwhisperer-nano/firmware/cwnano-firmware/src/ChipWhisperer-Nano.bin")),
    ("version", os.path.join(hwdir, r"capture/chipwhisperer-nano/firmware/cwnano-firmware/src/version.txt"))
    ]


target_ice40_neorv32_files = [("neorv32_iCE40CW312_MinimalBoot_directclk_7370KHz.bit", r"C:\dev\neorv32-setups\osflow\neorv32_iCE40CW312_MinimalBoot.bit")]

xc7a35_v = [0, 1]
xc7a35_files = [("AES_cw312t_a35.bit", os.path.join(hwdir, r"victims/cw308_ufo_target/xc7a35/vivado/ss2_cw305_aes.runs/impl_1/ss2_aes_wrapper.bit")),
                ("cw305_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/common/cw305_defines.v")),
                ("ECDSA256v1_pmul_cw312t_a35.bit", os.path.join(hwdir, r"victims/cw308_ufo_target/xc7a35/vivado/ss2_cw305_ecc.runs/impl_1/ss2_ecc_wrapper.bit")),
                ("cw305_pmul_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/ecc_p256_pmul/hdl/cw305_pmul_defines.v")),
                ("Pipelined_AES_cw312t_a35_half0.bit", os.path.join(hwdir, r"victims/cw308_ufo_target/xc7a35/vivado/ss2_cw305_aes_pipelined.runs/impl_full/ss2_aes_pipelined_wrapper.bit")),
                ("Pipelined_AES_cw312t_a35_half1.bit", os.path.join(hwdir, r"victims/cw308_ufo_target/xc7a35/vivado/ss2_cw305_aes_pipelined.runs/impl_half1/ss2_aes_pipelined_wrapper.bit")),
                ("Pipelined_AES_cw312t_a35_half2.bit", os.path.join(hwdir, r"victims/cw308_ufo_target/xc7a35/vivado/ss2_cw305_aes_pipelined.runs/impl_half2/ss2_aes_pipelined_wrapper.bit")),
                ("Pipelined_AES_cw312t_a35_half3.bit", os.path.join(hwdir, r"victims/cw308_ufo_target/xc7a35/vivado/ss2_cw305_aes_pipelined.runs/impl_half3/ss2_aes_pipelined_wrapper.bit")),
                ("cw305_aes_pipelined_defines.v", os.path.join(hwdir, r"victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/hdl/cw305_aes_pipelined_defines.v")),
                ]


#List of files to generate
file_list = [
    ("cwnano.py" , cwnano_v , cwnano_files),
    ("cw305.py" , cw305_v , cw305_files),
    ("xc7a35.py", xc7a35_v, xc7a35_files),
    ("cwlite.py", cwlite_v, cwlite_files),
    ("cw1200.py", cw1200_v, cw1200_files),
    ("cwhusky.py", cwhusky_v, cwhusky_files),
    ("cwbergen.py", cwbergen_v, cwbergen_files),
    ("cwtargetice40.py", [0,0], target_ice40_neorv32_files)
]

def parse_version(file_data):
    lines = file_data.split('\n')
    verarr = [ver.split(' ')[-1] for ver in lines]
    return verarr

for fdata in file_list:
    f = open(fdata[0], "w")
    fw_version = (fdata[1][0], fdata[1][1])
    for embdata in fdata[2]:
        if embdata[0] == "version":
            try:
                with open(embdata[1]) as e_file:
                    e_file = open(embdata[1])
                    data = e_file.read()
                    ver_arr = parse_version(data)
                    fw_version = (ver_arr[0], ver_arr[1])
                    print("Version file found for {}, version = {} ".format(fdata[0], fw_version))
            except:
                print("Version file not found for {}, using version {}".format(fdata[0], fw_version))

    f.write("# This file was auto-generated. Do not manually edit or save. What are you doing looking at it? Close it now!\n")
    f.write("# Generated on %s\n"%datetime.datetime.now())
    f.write("#\n")
    f.write("import binascii\n")
    f.write("import io\n\n")
    f.write("fwver = {}, {}\n".format(fw_version[0], fw_version[1]))
    f.write("def getsome(item, filelike=True):\n")
    f.write("    data = _contents[item].encode('latin-1')\n")
    f.write("    data = binascii.a2b_base64(data)\n")
    f.write("    if filelike:\n")
    f.write("        data = io.BytesIO(data)\n")
    f.write("    return data\n\n")
    f.write("_contents = {\n")

    f.write("")

    for embdata in fdata[2]:
        if embdata[0] == "version":
            continue
        with open(embdata[1], "rb") as e_file: # type: ignore
            # json_str = base64.b64encode(e_file.read())# json.dumps(e_file.read(), ensure_ascii=False)
            json_str = binascii.b2a_base64(e_file.read()) # type: ignore

            f.write("\n#Contents from %s\n"%embdata[1])
            f.write("'%s':'"%embdata[0])
            f.write(json_str.decode().replace("\n",""))
            f.write("',\n\n")
            f.flush()
    f.write("}\n")

f.close()
