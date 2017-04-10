# ChipWhisperer

[Wiki](https://wiki.newae.com/Main_Page) | [Knowledge Base](https://wiki.newae.com/Category:Knowledge_Base) | [Forum](http://www.newae.com/forum) | [Store](https://store.newae.com) | [NewAE](http://newae.com)

ChipWhisperer is an open source toolchain dedicated to hardware security research. This toolchain consists of several layers of open source components:
* __Hardware__: The ChipWhisperer uses a _capture_ board and a _target_ board. Schematics and PCB layouts for the ChipWhisperer-Lite capture board and a number of target boards are freely available.
* __Firmware__: Three separate pieces of firmware are used on the ChipWhisperer hardware. The capture board has a USB controller (in C) and an FPGA for high-speed captures (in Verilog) with open-source firmware. Also, the target device has its own firmware; this repository includes many firmware examples for different targets.
* __Software__: The ChipWhisperer software is made up of a _capture_ program to control the hardware and an _analyzer_ program to process captured data.

---

ChipWhisperer is a trademark of NewAE Technology Inc., registered in the US and Europe.
