import chipwhisperer as cw
scope = cw.scope()
prog = cw.SAMFWLoader(scope)
prog.enter_bootloader(True)
import time
time.sleep(0.1)
prog.program("COM3", "SAM3U_CW1173.bin")
