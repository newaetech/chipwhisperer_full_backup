import chipwhisperer as cw
prog = cw.SAMFWLoader(None)
import time
time.sleep(0.1)
prog.program("COM24", "SAM3U_CW1173.bin")
