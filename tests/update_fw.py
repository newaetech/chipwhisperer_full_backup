import chipwhisperer as cw

print(" You need first to short J5 and reset\n\n")
fw_path = "../hardware/victims/cw305_artixtarget/fw/sam3u/CW305_SAM3U_FW/build/CW305_SAM3UFW.bin"
programmer = cw.SAMFWLoader(scope=None)
programmer.program("/dev/ttyACM1", fw_path)
print("\n\n Reset the target again")

