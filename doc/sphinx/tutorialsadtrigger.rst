.. _tutorialsadtrigger:

Tutorial: SAD Trigger for SCA and Glitch
===========================================

This advanced tutorial will demonstrate the use of analog Sum of Absolute Difference (SAD) triggering system. The SAD trigger can be used to
trigger an event (e.g. capture or glitch) based on a pattern in the analog waveform. This pattern typically comes from the Device Under Test (DUT)
performing an operation of interest such as cryptographic or authentication code.

The SAD Criteria
-------------------

The Sum of Absolute Differences (SAD) measures the similarity between two blocks of data. This is done simply by first subtracting the two arrays
in an element-wise fashion, then converting that into an absolute value, and finally summing up the absolute difference between each element. You
can see other examples of the `SAD Algorith on Wikipedia <http://en.wikipedia.org/wiki/Sum_of_absolute_differences>`_.

In this system, a trigger is generated whenever the output of the SAD algorithm falls `below` some threshold. The FPGA calculates the SAD critera
for every incoming sample, where the SAD is calculated over 128 samples. The following diagram shows the system design:

.. image:: /images/sad_diagram.png

Both the reference waveform and the input must have the same scaling. For example if you are using the SAD trigger module, you cannot adjust the
analog gain on the input without recording a new reference waveform.


Using the SAD Trigger
-----------------------

Currently the SAD Trigger is demonstrated in this video:

|YouTubeSAD|_

.. |YouTubeSAD| image:: /images/youtube-sad.png
.. _YouTubeSAD: http://www.youtube.com/watch?v=qnKxOpGRo-Q&hd=1





