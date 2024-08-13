Hardware Issues with Software Workarounds.
******************************************

=========== ====================================================================
Tag         FIX_1
----------- --------------------------------------------------------------------
Implication I2C slave: General call interrupt not cleared.
            When the slave controller is addressed by a general call,
            the interrupt may not be cleared when ic_clr_gen_call is read.
            When the SoC core is running at 32MHz, the problem appears when the
            I2C clock is less than ~125kHz.

Affect      Intel Quark D2000, Intel Quark SE C1000

Workaround  Read repeatedly ic_clr_gen_call register until interrupt is
            cleared.

--------------------------------------------------------------------------------

