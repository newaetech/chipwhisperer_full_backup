#!/usr/bin/env python
# coding: utf-8

# # RSA FaultAttack

# This advanced tutorial will demonstrate an attack on RSA signatures that use the Chinese Remainder Theorem as an optimization. This tutorial will make use of glitching, so it's recommended that you complete at least Fault_1-Introduction_to_Clock_Glitch_Attacks before attempting this tutorial. You may also want to install gmpy2 before running this tutorial, as it makes a later section of the tutorial much faster. Linux users can install this via their package manager, Mac users via Brew, and Windows users via the [Unofficial Windows Python Binaries](https://www.lfd.uci.edu/~gohlke/pythonlibs/).
# 
# Additionally, this tutorial has been designed for Arm targets only. Users of other hardware will likely need to make changes to available RSA libraries to complete this tutorial.
# 
# This attack is was originally detailed in a [1997 paper by Boneh, Demillo, and Lipton](https://www.researchgate.net/publication/2409434_On_the_Importance_of_Checking_Computations). This tutorial draws heavily from a blog post by David Wong, which you can find [here](https://www.cryptologie.net/article/371/fault-attacks-on-rsas-signatures/).

# In[ ]:


#Parameters
SCOPETYPE = 'OPENADC'
PLATFORM = 'CWLITEARM'
GLITCHTYPE = 'CLOCK'


# ## Attack Theory

# We won't cover much about what RSA (there's a [Wikipedia article](https://en.wikipedia.org/wiki/RSA_(cryptosystem)) for that), but we will give a quick summary.
# 
# * RSA is a public key crypto system. It can be used in a few different ways, but we'll be using it for signing messages in this case. In this mode, User A can sign a message using their private information. User B can then verify that User A was the one who signed the message using publically available information.
# * This means that some information is public, while other information is private.
# * Public information includes the public modulus n, and the public exponent e
# * Private information includes the private exponent d, as well as p and q, which are prime factors of n
# * The public information can be freely shared, but learning any private information compromises the whole system
# 
# The math of RSA (once you have all the key parts generated) is actually pretty simple. To sign the message, the following equation can be applied (with signature s, message m, private exponent d, and public modulus n):
# 
# $$s = m^d({mod}\ n)$$
# 
# To verify a signature, the following equation is used (with signature s, public exponent e, message m, and public modulus n):
# 
# $$s^e = m(mod\ n)$$
# 
# Despite the simplicity of these equations, signing messages in particular is a very slow operation, with the implementation from MBEDTLS, a popular crypto library for Arm devices, taking over 12M cycles for RSA-1024 (and this is with the optimization we make in the next section). This is because all of the numbers used in these equations are huge (n and d are 1024 bits long in this case). As you can imagine, any improvement we can make to the speed of this operation is very important. It turns out there is a large speed optimization that we can make.

# ### Chinese Remainder Theorem (CRT)

# Instead of computing $s = m^d(mod\ n)$, we can instead break n into two primes, p and q, such that $n = pq$, then use those to calculate the signature instead of n. As you might have guessed, p and q are the same private information we talked about earlier. Bascially, if we learn either, we'll be able to derive the rest of the private information fairly easily. We won't go into all the math, but here's the important operations:
# 
# * Derive $d_p$ from d and p and $d_q$ from d and q
# * Calculate: $s_1 = m^{d_p}(mod\ p)$ and $s_2 = m^{d_q}(mod\ q)$
# * Combine $s_1$ and $s_2$ into $s$ via CRT
# 
# Since p and q are much smaller than n, creating signatures is much much faster this way. As such, many popular RSA implementations (including MBEDTLS) use CRT to speed up RSA.

# ### CRT Attack

# Suppose that instead of everything going smoothly as above, a fault happens during the calculation of $s_1$ or $s_2$ (we'll assume that the fault was with $s_2$ here, which will become $s^{'}_{2}$). If that happens, the following becomes true (with faulty signatures $s_2'$, which generates $s'$):
# 
# $$s'^e = m(mod\ p) \Rightarrow s'^e - m = 0 (mod\ p)$$
# $$s'^e \neq m(mod\ q) \Rightarrow s'^e - m \neq 0 (mod\ q)$$
# 
# The result of this is that p will be a factor of $s'^e - m$, but q and n will not be. Since p is also a factor of N, what follows is that:
# 
# $$p = gcd(s'^e - m, N)$$
# 
# Thus, if we introduce a fault in the calculation of either $s_1$ or $s_2$, we'll be able to get either q or p, and from there the rest of the private values!

# ## Firmware

# Next, let's take a look at the RSA implementation we're attacking. For this attack, we'll be using the `simpleserial-rsa-arm` project folder. There's a few files here, but the important one is `simpleserial-arm-rsa.c`. Open it. As you scroll through, you'll find all our public/private values. Next, navigate to `real_dec()`:
# 
# ```C
# uint8_t buf[128];
# uint8_t hash[32];
# uint8_t real_dec(uint8_t *pt)
# {
#      int ret = 0;
# 
#      //first need to hash our message
#      memset(buf, 0, 128);
#      mbedtls_sha256(MESSAGE, 12, hash, 0);
# 
#      trigger_high();
#      ret = simpleserial_mbedtls_rsa_rsassa_pkcs1_v15_sign(&rsa_ctx, NULL, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, 32, hash, buf);
#      trigger_low();
# 
#      //send back first 48 bytes
#      simpleserial_put('r', 48, buf);
#      return ret;
# }
# ```
# 
# You'll notice that we first hash our message (`"Hello World!"`) using SHA256. Once this is passed to the signature function, it will be padded according to the PKCS#1 v1.5 standard. This isn't too important now, but it will be important later. Next we sign our message using `simpleserial_mbedtls_rsa_rsassa_pkcs1_v15_sign()`, then send back the first 48 bytes of it. We'll be sending the signature back in multiple chunks to avoid overflowing the CWLite's buffer of 128 bytes via `sig_chunk_1()` and `sig_chunk_2()` directly below this function.
# 
# We'll actually skip over `simpleserial_mbedtls_rsa_rsassa_pkcs1_v15_sign()` here, since most of the important stuff actually happens in a different function. You should note, however, that this function has been modified to remove a signature check, which would need to be bypassed in a real attack.
# 
# Next, find the function `simpleserial_mbedtls_rsa_private()`, a cleaned up version of `mbedtls_rsa_private()`, where the signature calculation actually happens:
# ```C
# /*
#  * Do an RSA private key operation
#  */
# static int simpleserial_mbedtls_rsa_private( mbedtls_rsa_context *ctx,
#                  int (*f_rng)(void *, unsigned char *, size_t),
#                  void *p_rng,
#                  const unsigned char *input,
#                  unsigned char *output )
# 
# ```
# 
# scrolling down a bit, we do indeed find that this function uses CRT to speed up the calculation:
# 
# ```C
#     /*
#      * Faster decryption using the CRT
#      *
#      * T1 = input ^ dP mod P
#      * T2 = input ^ dQ mod Q
#      */
#     MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T1, &T, DP, &ctx->P, &ctx->RP ) );
#     MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &T2, &T, DQ, &ctx->Q, &ctx->RQ ) );
# ```
# 
# You can view more of the firmware if you want, but for now let's build our firmware and then move over to our python script:

# In[ ]:


CRYPTO_TARGET="MBEDTLS"
CRYPTO_OPTIONS="RSA"


# In[ ]:


get_ipython().run_cell_magic('bash', '-s "$PLATFORM" "$CRYPTO_TARGET" "$CRYPTO_OPTIONS"', 'cd ../../hardware/victims/firmware/simpleserial-rsa\nmake PLATFORM=$1 CRYPTO_TARGET=$2 CRYPTO_OPTIONS=$3 OPT=2')


# ## Attack Script

# Start by initializing the ChipWhisperer:

# In[ ]:


import chipwhisperer as cw
import binascii
scope = cw.scope()
target = cw.target(scope)
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x1"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"


# Next, program it with our new firmware:

# In[ ]:


import time
prog = cw.programmers.STM32FProgrammer
fw_path = "../../hardware/victims/firmware/simpleserial-rsa/simpleserial-rsa-CWLITEARM.hex"
cw.programTarget(scope, prog, fw_path)
time.sleep(1)


# ### Verifying Signatures

# Let's start by seeing if we can verify the signature that we get back. First, we run the signature calculation (there's a `time.sleep()` here to make sure the calculation finishes. You may need to increase this):

# In[ ]:


import time
target.go_cmd = 't\\n'
scope.arm()
target.go()

while target.isDone() is False:
    timeout -= 1
    time.sleep(0.01)

try:
    ret = scope.capture()
    if ret:
        print('Timeout happened during acquisition')
except IOError as e:
    print('IOError: %s' % str(e))
    
time.sleep(2)
num_char = target.ser.inWaiting()
output = target.ser.read(num_char, timeout=10)


# In[ ]:


print(scope.adc.trig_count)


# As you can see, the signature takes a long time! For the STM32F3, it should be around 12.7M cycles. Next, let's get the rest of the signature back and see what it looks like.

# In[ ]:


target.go_cmd = '1\\n'
target.go()
time.sleep(0.2)
num_char = target.ser.inWaiting()
output += target.ser.read(num_char, timeout=10)

target.go_cmd = '2\\n'
target.go()
time.sleep(0.2)
num_char = target.ser.inWaiting()
output += target.ser.read(num_char, timeout=10)


# In[ ]:


print(output)


# You should see something like:
# ```
# r4F09799F6A59081B725599753330B7A2440ABC42606601622FE0C582646E32555303E1062A2989D9B4C265431ADB58DD
# z00
# r85BB33C4BB237A311BC40C1279528FD6BB36F94F534A4D8284A18AB8E5670E734C55A6CCAB5FB5EAE02BA37E2D56648D
# z00
# r7A13BBF17A0E07D607C07CBB72C7A7A77076376E8434CE6E136832DC95DB3D80
# z00
# ```
# 
# We'll need to strip all the extra simpleserial stuff out. This can be done like so:

# In[ ]:


newout = output.replace("r", "").replace("\nz00","").replace("\n","")
print(newout)


# Then we can convert this to binary using binascii:

# In[ ]:


import binascii
sig = binascii.unhexlify(newout)


# Finally, we can verify that the signature is correct using the PyCryptodome package:

# In[ ]:


from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_v1_5 

from Crypto.Hash import SHA256

E = 0x10001
N = 0x9292758453063D803DD603D5E777D7888ED1D5BF35786190FA2F23EBC0848AEADDA92CA6C3D80B32C4D109BE0F36D6AE7130B9CED7ACDF54CFC7555AC14EEBAB93A89813FBF3C4F8066D2D800F7C38A81AE31942917403FF4946B0A83D3D3E05EE57C6F5F5606FB5D4BC6CD34EE0801A5E94BB77B07507233A0BC7BAC8F90F79
m = b"Hello World!"

hash_object = SHA256.new(data=m)
pub_key = RSA.construct((N, E))
signer = PKCS1_v1_5.new(pub_key) 
sig_check = signer.verify(hash_object, sig)
print(sig_check)

assert sig_check, f"Failed to verify signature on device. Got: {newout}"


# If everything worked out correctly, you should see `True` printed above. Now onto the actual attack.

# ### Getting a Glitch

# As usual, we'll start off by setting up the glitch module:

# In[ ]:


scope.glitch.clk_src = "clkgen"
scope.glitch.output = "clock_xor"
scope.glitch.trigger_src = "ext_single"
scope.glitch.repeat = 1
scope.glitch.width = -9
scope.glitch.offset = -38.3
scope.io.hs2 = "glitch"
print(scope.glitch)
from collections import namedtuple
Range = namedtuple('Range', ['min', 'max', 'step'])


# Now for our actual attack loop. There's a lot going on here, so we'll move through a little slower than usual. Overall, what we want to do is:
# * Insert a glitch
# * Read the signature back
# * Verify that it's correct
# 
# The first step is the same as earlier. For the last two, we'll cheat a little by checking the for the beginning of the correct signature before proceeding, but we could also read back the whole thing:
# 
# ```python
# # Read back signature
# num_char = target.ser.inWaiting()
# output = target.ser.read(num_char, timeout=10)
#     if "4F09799" not in output:
#         #Something abnormal has happened
# ```
# 
# Now that we've found some abnormal behaviour, we need to verify that the target hasn't crashed. This can be done pretty easily by checking if we got anything at all:
# 
# ```python
# if "4F09799" not in output:
#     #Something abnormal has happened
#     if len(output) > 0:
#         # Possible glitch!
#     else:
#         # Crash, reset and try again
#         print(f"Probably crash at {scope.glitch.ext_offset}")
#         reset_target(scope)
#         time.sleep(0.5)
# ```
# 
# As a last step, we'll build our full signature and do one final check to make sure everything looks okay:
# 
# ```python
# if len(output) > 0:
#     # Possible glitch!
#     print(f"Possible glitch at offset {scope.glitch.ext_offset}\nOutput: {output}")
#     
#     # get rest of signature back
#     target.go_cmd = '1\\n'
#     target.go()
#     time.sleep(0.2)
#     num_char = target.ser.inWaiting()
#     output += target.ser.read(num_char, timeout=10)
# 
#     target.go_cmd = '2\\n'
#     target.go()
#     time.sleep(0.2)
#     num_char = target.ser.inWaiting()
#     output += target.ser.read(num_char, timeout=10)
#     
#     # strip out extra simpleserial stuff
#     newout = output.replace("r", "").replace("\nz00","").replace("\n","")
#     
#     print(f"Full output: {newout}")
#     if (len(newout) == 256) and "r0001F" not in output:
#         print("Very likely glitch!")
#         break
# ```
# 
# We'll add in scanning over different over different offsets as well. We'll start at an offset of 7M cycles. We actually have a lot of area that we could place the glitch in, so the starting point is fairly arbitrary. For the STM32F3, this places the glitch near the beginning of the calculation for $s_2$. If you'd like, you can move `trigger_low()` into `simpleserial_mbedtls_rsa_private()` to see how long different parts of the algorithm take.
# 
# All together, our attack loops looks like this:

# In[ ]:


from tqdm import tnrange
import binascii
import time
for i in tnrange(7000000, 7100000):
    scope.glitch.ext_offset = i
    target.go_cmd = 't\\n'
    scope.adc.timeout = 3
    scope.arm()
    target.go()

    while target.isDone() is False:
        timeout -= 1
        time.sleep(0.01)

    try:
        ret = scope.capture()
        if ret:
            print('Timeout happened during acquisition')
    except IOError as e:
        print('IOError: %s' % str(e))
    time.sleep(2)
    
    # Read back signature
    num_char = target.ser.inWaiting()
    output = target.ser.read(num_char, timeout=10)
    if "4F09799" not in output:
        # Something abnormal happened
        if len(output) > 0:
            # Possible glitch!
            print(f"Possible glitch at offset {scope.glitch.ext_offset}\nOutput: {output}")
            
            # Get rest of signature back
            target.go_cmd = '1\\n'
            target.go()
            time.sleep(0.2)
            num_char = target.ser.inWaiting()
            output += target.ser.read(num_char, timeout=10)

            target.go_cmd = '2\\n'
            target.go()
            time.sleep(0.2)
            num_char = target.ser.inWaiting()
            output += target.ser.read(num_char, timeout=10)
            
            # Strip out extra simpleserial stuff
            newout = output.replace("r", "").replace("\nz00","").replace("\n","")
            print(f"Full output: {newout}")
            if (len(newout) == 256) and "r0001F" not in output:
                print("Very likely glitch!")
                break
        else:
            # Crash, reset and try again
            print(f"Probably crashed at {scope.glitch.ext_offset}")
            reset_target(scope)
            time.sleep(0.5)


# Now, let's convert our glitched signature to binary and move on to the final part of the tutorial

# In[ ]:


sig = binascii.unhexlify(newout)
print(sig)


# ### Completing The Attack

# We've got our glitched signature, but we've still got a little work to do. As was mentioned earlier, the message that's signed isn't the original message, it's a PKCS#1 v1.5 padded hash of it. Luckily, this standard's fairly simple. PKCS#1 v1.5 padding looks like:
# 
# |00|01|ff...|00|hash_prefix|message_hash|
# 
# Here, the ff... part is a string of ff bytes long enough to make the size of the padded message the same as n, while hash_prefix is an identifier number for the hash algorithm used on message_hash. In our case, SHA256 has the hash prefix `3031300d060960864801650304020105000420`.
# 
# Altogether, the function to build this message looks like:

# In[ ]:


def build_message(m, N):
    sha_id = "3031300d060960864801650304020105000420"
    N_len = (len(bin(N)) - 2 + 7) // 8
    pad_len = (len(hex(N)) - 2) // 2 - 3 - len(m)//2 - len(sha_id)//2
    padded_m = "0001" + "ff" * pad_len + "00" + sha_id + m
    return padded_m


# Now that we've got our function, we can build our message:

# In[ ]:


from Crypto.Hash import SHA256
import binascii

hash_object = SHA256.new(data=b"Hello World!")
hashed_m = binascii.hexlify(hash_object.digest()).decode()
padded_m = build_message(hashed_m, N)
print(hashed_m)
print(padded_m)


# Now all that's left is to use our gcd formula from earlier:
# 
# $$p = gcd(s'^e - m, N)$$
# 
# And we should get either p or q! These calculations can take a while (the Python version takes a few minutes), so the next block will try to use gmpy2 (a high precision library that runs much quicker than base Python). If you don't have gmpy2 installed, it will fall back to Python.

# In[ ]:


from math import gcd
N = 0x9292758453063D803DD603D5E777D7888ED1D5BF35786190FA2F23EBC0848AEADDA92CA6C3D80B32C4D109BE0F36D6AE7130B9CED7ACDF54CFC7555AC14EEBAB93A89813FBF3C4F8066D2D800F7C38A81AE31942917403FF4946B0A83D3D3E05EE57C6F5F5606FB5D4BC6CD34EE0801A5E94BB77B07507233A0BC7BAC8F90F79
e = 0x10001
try:
    import gmpy2
    from gmpy2 import mpz
    sig_int = mpz(int.from_bytes(sig, "big"))
    m_int = mpz(int.from_bytes(binascii.unhexlify(padded_m), "big"))
    p_test = gmpy2.gcd(sig_int**e - m_int, N)
except (ImportError, ModuleNotFoundError) as error:
    print("gmpy2 not found, falling back to Python")
    sig_int = int.from_bytes(sig, "big")
    padded_m_int = int.from_bytes(binascii.unhexlify(padded_m), "big")
    a = int.from_bytes(sig, "big")**e - int.from_bytes(binascii.unhexlify(padded_m), "big")
    p_test = gcd(a, N)
    
print(hex(p_test))


# Open up `simpleserial-arm-rsa.c` and see if the value printed out is either p or q!

# ### Getting the Rest of the Private Values

# As mentioned earler, now that we have either p or q, we can derive the rest of the private values. The easiest is the other prime factor, which is just:

# In[ ]:


q_test = N//p_test
print(hex(q_test))


# Finally, there's d, which can be derived by:

# In[ ]:


phi = (q_test - 1)*(p_test - 1)
def egcd(a, b):
    x,y, u,v = 0,1, 1,0
    while a != 0:
        q, r = b//a, b%a
        m, n = x-u*q, y-v*q
        b,a, x,y, u,v = a,r, u,v, m,n
        gcd = b
    return gcd, x, y

gcd, d, b = egcd(e, phi)

print(hex(d))


# ## Going Further

# There's still more you can do with this attack:
# 
# * You can try glitching the other part of the signature calculation to verify that you get the other prime factor of n out
# * We used clock glitching in this tutorial. You may want to try it with voltage glitching as well
# 
# As mentioned earlier in the tutorial, a verification of the calculated signature was removed:
# ```C
#     /* Compare in constant time just in case */
#     /* for( diff = 0, i = 0; i < ctx->len; i++ ) */
#     /*     diff |= verif[i] ^ sig[i]; */
#     /* diff_no_optimize = diff; */
# 
#     /* if( diff_no_optimize != 0 ) */
#     /* { */
#     /*     ret = MBEDTLS_ERR_RSA_PRIVATE_FAILED; */
#     /*     goto cleanup; */
#     /* } */
# 
# ```
# 
# This part is near the end of `simpleserial_mbedtls_rsa_rsassa_pkcs1_v15_sign()`. If you want a larger challenge, you can try uncommenting that and trying to glitch past it as well.

# ## Tests

# In[ ]:


real_p = "0xC36D0EB7FCD285223CFB5AABA5BDA3D82C01CAD19EA484A87EA4377637E75500FCB2005C5C7DD6EC4AC023CDA285D796C3D9E75E1EFC42488BB4F1D13AC30A57".lower()
real_q = "0xC000DF51A7C77AE8D7C7370C1FF55B69E211C2B9E5DB1ED0BF61D0D9899620F4910E4168387E3C30AA1E00C339A795088452DD96A9A5EA5D9DCA68DA636032AF".lower()
assert (hex(p_test) == real_p) or (hex(p_test) == real_q), f"Failed to break p or q. Got {hex(p_test)}, excepted {real_p} or {real_q}"


# In[ ]:


assert (hex(q_test) == real_p) or (hex(q_test) == real_q), f"Failed to break p or q. Got {hex(p_test)}, excepted {real_p} or {real_q}"


# In[ ]:


real_d = "0x24bf6185468786fdd303083d25e64efc66ca472bc44d253102f8b4a9d3bfa75091386c0077937fe33fa3252d28855837ae1b484a8a9a45f7ee8c0c634f99e8cddf79c5ce07ee72c7f123142198164234cabb724cf78b8173b9f880fc86322407af1fedfdde2beb674ca15f3e81a1521e071513a1e85b5dfa031f21ecae91a34d"
assert (hex(d) == real_d), f"Failed to break private key d. Got {hex(d)}, expected {real_d}"

