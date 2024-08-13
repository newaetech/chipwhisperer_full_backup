Fixed Issues and Workarounds
****************************

Issues fixed since version: QMSI 1.2.0.

=========== ====================================================================
Issue       Spurious character is printed on UART example for SE C1000.
----------- --------------------------------------------------------------------
Implication SE C1000 prints an additional ü character when printing.
----------- --------------------------------------------------------------------
Workaround  Ignore first character that is printed.
=========== ====================================================================

=========== ====================================================================
Issue       SPI 16 MHz transfer failing on SE C1000 development platform
----------- --------------------------------------------------------------------
Implication On SE C1000, comparison of RX and TX is not correct when using the
            16 MHz speed.
----------- --------------------------------------------------------------------
Workaround  Use a transfer speed slower than 16 MHz.
=========== ====================================================================