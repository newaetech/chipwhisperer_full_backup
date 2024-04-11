from chipwhisperer.capture.targets.CW305 import CW305
import matplotlib.pyplot as plt
import chipwhisperer as cw
from tqdm import tqdm
from Crypto.Cipher import AES
import numpy as np
import time


# Functional testing
NTEST = 100
MBATCHSIZE = 2048

# Benchmark settings
TESTED_BATCHSIZE = 2**np.arange(1,11) 

bitstream_path = "../hardware/victims/cw305_artixtarget/fpga/vivado_examples/aes128_verilog/aes128_verilog.runs/impl_35t/cw305_top.bit"

print("####################")
print("# Programming the target with default bitsteam")
print("####################\n")
target = CW305()
target.con(bsfile=bitstream_path, force=False)

target.clkusbautooff = True
target.clksleeptime = 1

print("####################")
print("# Functional testing")
print("####################")

for i in tqdm(range(NTEST),desc="Functional testing random pt and random key"):
    batchsize = np.random.randint(1,MBATCHSIZE)

    # Do the batch_run
    keys,pts = target.batchRun(batchsize=batchsize,random_key=True,
        random_pt=True)
    
    key = bytes(keys[-1])
    pt = bytes(pts[-1])
    last_ct = target.readOutput()

    # Verify the last cipher text
    cipher = AES.new(key, AES.MODE_ECB)
    ref = cipher.encrypt(pt)

    if ref != bytes(last_ct):
        print("Test Failed")
        exit(-1)

for i in tqdm(range(NTEST),desc="Functional testing random pt and fixed key"):
    batchsize = np.random.randint(1,MBATCHSIZE)

    key = np.random.randint(0,256,16,dtype=np.uint8).tobytes()
    target.loadEncryptionKey(key)

    # Do the batch_run
    keys,pts = target.batchRun(batchsize=batchsize,random_key=False,
        random_pt=True)
    
    pt = bytes(pts[-1])
    last_ct = target.readOutput()

    # Verify the last cipher text
    cipher = AES.new(key, AES.MODE_ECB)
    ref = cipher.encrypt(pt)

    if ref != bytes(last_ct):
        print("Test Failed")
        exit(-1)

for i in tqdm(range(NTEST),desc="Functional testing fixed pt and random key"):
    batchsize = np.random.randint(1,MBATCHSIZE)

    pt = np.random.randint(0,256,16,dtype=np.uint8).tobytes()
    target.loadInput(pt)

    # Do the batch_run
    keys,pts = target.batchRun(batchsize=batchsize,random_key=True,
        random_pt=False)
    
    key = bytes(keys[-1])
    last_ct = target.readOutput()

    # Verify the last cipher text
    cipher = AES.new(key, AES.MODE_ECB)
    ref = cipher.encrypt(pt)

    if ref != bytes(last_ct):
        print("Test Failed")
        exit(-1)

for i in tqdm(range(NTEST),desc="Functional testing fixed pt and fixed key"):
    batchsize = np.random.randint(1,MBATCHSIZE)

    pt = np.random.randint(0,256,16,dtype=np.uint8).tobytes()
    target.loadInput(pt)
    key = np.random.randint(0,256,16,dtype=np.uint8).tobytes()
    target.loadEncryptionKey(key)

    # Do the batch_run
    keys,pts = target.batchRun(batchsize=batchsize,random_key=False,
        random_pt=False)
    
    last_ct = target.readOutput()

    # Verify the last cipher text
    cipher = AES.new(key, AES.MODE_ECB)
    ref = cipher.encrypt(pt)

    if ref != bytes(last_ct):
        print("Test Failed")
        exit(-1)

print("####################")
print("# Benchmark")
print("####################")
tp_all = []
labels = []
labels_default = []
for random_pt in [False,True]:
    for random_key in [False,True]:
        tp = []
        for batchsize in TESTED_BATCHSIZE:
            print("Running benchmark on batchsize of ",batchsize)
            start = time.time()
            for _ in range(NTEST):
                target.batchRun(batchsize,random_key,random_pt)
            end = time.time()
            tp.append(NTEST*batchsize/(end-start))

        k = np.random.randint(0,256,dtype=np.uint8).tobytes()
        p = np.random.randint(0,256,dtype=np.uint8).tobytes()
        start = time.time()
        for _ in range(NTEST):
            if random_key:
                target.loadEncryptionKey(k)
            if random_pt:
                target.loadInput(p)
            target.go()
        end = time.time()
        de = NTEST/(end-start)
        labels.append("rand. pt "+str(random_pt)+" rand. key "+str(random_key))
        tp_all.append((tp,de))


plt.figure()
colors = ["r","g","b","m"]
plt.subplot(211)
for i,(label,tp) in enumerate(zip(labels,tp_all)):
    plt.loglog(TESTED_BATCHSIZE,tp[0],label=label,basex=2,basey=10,color=colors[i])
plt.ylabel("Enc/sec with batchRun")
plt.legend()
plt.grid(True,which="both",ls="--")

plt.subplot(212)
for i,(label,tp) in enumerate(zip(labels,tp_all)):
    plt.loglog(TESTED_BATCHSIZE,np.zeros(len(TESTED_BATCHSIZE)),basex=2,basey=10,color=colors[i])
    plt.axhline(tp[1],color=colors[i],label=label)
plt.ylabel("Enc/sec with loadKey")
plt.xlabel("Batch size")
plt.legend()
plt.grid(True,which="both",ls="--")
plt.show()
