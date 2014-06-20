.. _theory:

Theory
=========

Correlation Power Analysis
-----------------------------

Here we will assume the attack has a power trace :math:`t_{d,j}`, where :math:`j = 1,2,\cdots,T` is the time index in the trace, 
and :math:`d = 1,2,\cdots,D` is the trace number. Thus the attacker makes :math:`D` measurements, each one :math:`T` points long. If the attacker knew
*exactly* where a cryptographic operation occurred, they would need to only measure a single point such that :math:`T=1`. For each trace
:math:`d`, the attacker also knows the plaintext or ciphertext corresponding to that power trace, defined as :math:`p_d`.

Assume also the attacker has a model of how the power consumption of the device depends on some intermediate value. For example the attacker
could assume the power consumption of a microcontroller was dependent on the hamming weight of the intermediate value. We will define
:math:`h_{d,i} = l( w( p_d, i ))`, where :math:`l(x)` is the leakage model for a given intermediate value, and :math:`w(p, i)` generates
an intermediate value given the input plaintext and the guess number :math:`i = 1,2,\cdots,I`.

This intermediate value will be selected to depend on the input plaintext and a small portion of the secret key. For example with AES, each
byte of the plaintext is XOR'd with each byte (subkey) of the secret key. In this example we would have:

    .. math::

        l(x) = HammingWeight(x)
        
        w(p, i) = p \oplus i
    
This implies that the input plaintext :math:`p` is being attacked a single byte at a time, which means we are attacking a single byte of the
AES key at a time. While we still need to enumerate all possibilities for this subkey, we now only have :math:`16 \times 2^8` instead of
:math:`2^{128}` possibilities for AES-128.

We will be next using the correlation coefficient to look for a linear relationship between the predicted power consumption :math:`l(x)` and
the measured power consumption :math:`t_{d,j}`. For this reason it is desirable to have a non-linear relationship between :math:`w(p, i)` and
either :math:`p` or :math:`i`, as we will otherwise see a linear relationship for all values of :math:`i`. In this case we take advantage of
the non-linear substitution boxes (S-Boxes) in the algorithm, which are simply lookup tables which have been selected to have minimal possible
correlation between the input and output. The emphasis on minimum possible correlation between input and output is a requirement to avoid certain
linear cryptographic attacks.

Finally, we can calculate the correlation coefficient for each point :math:`j` over all traces :math:`D`, for each of the possible subkey values
:math:`I`, as in the following:

    .. math::

        {r_{i,j}} = \frac{{\sum\nolimits_{d = 1}^D {\left[ {\left( {{h_{d,i}} - \overline {{h_i}} } \right)\left( {{t_{d,j}} - \overline {{t_j}} } \right)} \right]} }}{{\sqrt {\sum\nolimits_{d = 1}^D {{{\left( {{h_{d,i}} - \overline {{h_i}} } \right)}^2}} \sum\nolimits_{d = 1}^D {{{\left( {{t_{d,j}} - \overline {{t_j}} } \right)}^2}} } }}

This is simply Pearson's correlation coefficient, given below, where :math:`X = p`, and :math:`Y = t`:

    .. math::

        {\rho _{X,Y}} = \frac{{{\mathop{\rm cov}} \left( {X,Y} \right)}}{{{\sigma _X}{\sigma _Y}}} = \frac{{E\left[ {\left( {X - {\mu _X}} \right)\left( {Y - {\mu _Y}} \right)} \right]}}{{\sqrt {E\left[ {{{\left( {X - {\mu _X}} \right)}^2}} \right]} \sqrt {E\left[ {{{\left( {Y - {\mu _Y}} \right)}^2}} \right]} }}

The problem of finding a known signal in a noisy measurement exists in many other fields beyond side-channel analysis. These two equations are
referred to as the \emph{normalized cross-correlation}, and frequently used in digital imaging for matching a known `template' to an image, e.g.
finding the location of some specific item in a photo of a room.


Template Attacks
------------------


T-Test Analysis
------------------


