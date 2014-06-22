.. _hwdiffprobe:

Differential Probe
========================================

The most basic method of measuring the power usage of a device is doing so by measuring the single-ended voltage across a resistor. 
Single-ended means we have a probe connected to one side of the resistor, and a common ground connection. But besides just measuring
the drop across the resistor, you will also measure any *variation* in the power supply. This could be due to the regulator, environmental
noise, or voltage variations from other areas of the circuit switching.
 
Rather than use a single-ended probe, a differential probe ignores that variation in voltage that is common to both sides of the resistor,
also called *common mode* voltage. The measurement is now *only* the voltage drop across the resistor.

Hardware Diagram
--------------------

.. image:: /images/diffprobe/diffprobe-top.jpg

Features
--------------------

* Can power from single or double-ended supply

* Reverse polarity protection on supply (**NB: 'V-' done via diode shunt, if 'V-' supply has more than 100mA permanant damage may still
  occur from reverse polarity**).

* Adjustable DC offset to compensate for null constant differential voltage

* 200+ MHz Bandwidth

* 10x gain

Usage Information
--------------------------

Supply & Voltage Ranges
^^^^^^^^^^^^^^^^^^^^^^^^^
The supply voltages of the differential probe must be at least 2V above/below (for +V & -V respectively) the common mode voltage. The ChipWhisperer Capture Rev2
Hardware provides a +/- 8V supply for you, allowing you to use the probe on any reasonable shunt inserted into VCC and GND. If measuring the power on a 3.3V system
for example, you should power the probe with at least +5V on the positive rail!

Be aware that if powering the probe via a single-ended supply (see below), you can only use the probe on a VCC shunt. The following shows the
pinout of the 6-pin connector:

.. image:: /images/diffprobe/pinout.jpg


Single-Ended Supply Usage
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Using a single-ended supply means connecting the -V input to the GND pad. This is done via a jumper mounted as shown on the 6-pin connector:

.. image:: /images/diffprobe/gndshunt.jpg

Note again the common-mode limits still apply. This means the common-mode voltage must be at least +2V since the -V supply is 0V. Thus you can
**only use the single-ended power supply for a VCC shunt**. 

Common Mode Reference
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The common-mode reference point can be used to connect your system ground to the probe ground.
Note you need to ensure the common-mode input voltage is with-in the allowed range (e.g. at least 2V away from either supply voltage).
If you are powering the differential probe with another power supply, you must make sure they are referenced to each other by connecting the
0V/GND points together. Using a resistor reduces a small differential from causing a large current to flow if you accidently have ground loops
(multiple connections of system grounds).

Zero Null Adjustment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When using the probe, there will often be a constant voltage we wish to null out. If measuring the current signature of a system, there will be a constant
current usage we don't care about. Without a null adjustment, this constant current would cause the output of the differential probe to stick at a rail or otherwise
clip the output.

When setting up the probe, first connect it into the system with power, clock, etc connected. Then measure the constant output voltage from the output of the
differential probe. It should be half-way between the -V and +V supply: e.g. if using symmetrical supplies ti should be around 0V, if using single-ended supplies
it should be half of the +V voltage.

If this is not the case, adjust the trim-pot until the output voltage is within the expected range.


Schematic
-----------------



