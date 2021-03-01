import chipwhisperer as cw
prog = cw.SAMFWLoader(None)
import time
time.sleep(0.1)
prog.program("COM9", "SAM3U_CW1173.bin")
