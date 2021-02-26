import chipwhisperer as cw
prog = cw.SAMFWLoader(None)
import time
time.sleep(0.1)
prog.program("COM3", "SAM3U_CW1173.bin")
