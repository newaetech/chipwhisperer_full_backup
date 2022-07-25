TARGET = ChipWhisperer-Lite

SRC += main.c naeusb/usb_xmem.c naeusb/fpga_program.c naeusb/V2Protocol.c naeusb/V2ProtocolParams.c  ui.c
SRC += naeusb/XMEGANVM.c naeusb/XPROGNewAE.c naeusb/XPROGTarget.c naeusb/ISPProtocol.c naeusb/ISPTarget.c 
SRC += naeusb/XPROGTimeout.c naeusb/circbuffer.c
SRC += naeusb/udc.c
SRC += naeusb/naeusb.c naeusb/naeusb_default.c naeusb/naeusb_openadc.c naeusb/naeusb_usart.c
SRC += naeusb/naeusb_mpsse.c
LINKERFILE = naeusb/flash_sam3u2.ld

include naeusb/makefile.cw