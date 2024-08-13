Doxygen Coding guideline
########################

Overview
********

This page is intended to provide guidelines for how doxygen comments should be
styled to provide a consistent look and feel throughout QMSI.

.. contents::

General guidelines
******************

* Use Java style documentation
  (https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html)
* Treat Javadoc autobrief as being enabled.
* When writing a brief description, everything until the first  "." is
  considered the description.
* All descriptions are completed with a ".". The only exception is lists of
  items.
* "For Quark Microcontrollers" should not be in the doxygen comments as this is
  the Documentation
* for QMSI, and leads to a tautology.

Files
*****

Files guidelines
================

* First line is the detailed description for the file.
* A blank line.
* @defgroup to define which group the functions belong to
* @{ to open the group file
* Include all defines, enums, structs, functions for that API
* @} to close the group

Sample file
===========
::

  /**
   * General Purpose IO.
   *
   * @defgroup groupGPIO GPIO
   * @{
   */
  <Defines go here>
  <Enums go here>
  <Structs go here>
  <Functions go here>
  /**
   * @}
   */

Structs, Enums, Defines
***********************

Structs, Enums, Defines guidelines
==================================

Documenting structs, enums and defines is left up to the discretion of the
developer.

For cases where it is blindingly obvious what the define does, documentation is
not required.

In the cases where documentation is required, the following are the guidelines:

* Before the struct, enum or define begins:
        * @{ to open the group
        * A Brief description of the structure, enum or define in a single
          sentence.
        * If required, a more detailed description to follow.
* For short member descriptions a single line with the description:
        * /\*\*< <Brief description of member>. \*/
* For longer member descriptions:
        * First sentence is the brief description.
        * Followed by more detailed description of the member.
* Functions inside structs should follow the guidelines provided in the
  functions section below.
* @} to close the group

Sample Struct
=============

::

  /**
   * GPIO port configuration struct.
   */

  typedef struct {
          uint32_t direction; /**< GPIO direction. 0b: input, 1b: output. */
          uint32_t int_en; /**< Interrupt enable. */
          uint32_t int_type; /**< Interrupt type. 0b: level; 1b: edge. */
          uint32_t int_polarity; /**< Interrupt polarity. 0b: low, 1b: high. */
          uint32_t int_debounce; /**< Debounce on/off. */
          /**
           * Rising / Falling edge interrupt. Interrupt can be set to
           * trigger on both rising and falling edges.
           * This option is user selectable <insert more text here>.
           * (This demonstrates a member variable with a long
           * description.)
           */
          uint32_t int_bothedge;
          /**
           * User callback for GPIO ISR.
           *
           * ISR for GPIO
           *
           * @param[in] uint32_t Interrupt status.
           */
          void (*callback)(uint32_t int_status);
  } qm_gpio_port_config_t;

Sample Define
=============

::

  /** Flash mask to clear timing. */
  #define QM_FLASH_TMG_DEF_MASK (0xFFFFFC00)

Sample Enum
===========

::

  /**
   * GPIO pin state enumerator.
   */
  typedef enum {
          QM_GPIO_LOW, /**< Set pin low. */
          QM_GPIO_HIGH, /**< Set pin high. */
          QM_GPIO_STATE_NUM
  } qm_gpio_state_t;

Functions
*********

Functions guidelines
====================

Doxygen comments must include:

* The first line of a function is  the brief for that function.
* A blank line.
* The longer description of the function if needed.
* A blank line.
* A list of @param with [in], [out] or [in,out].
* A blank line.
* @return to describe the return value in general.
* For non-trivial return values (e.g..: status list) A list of possible return
  values using @retval. (on separate lines)

By default, functions are expected to be non-blocking, execute in user context
and not use any mutual exclusion primitives. If a function deviates from this,
then the Doxygen comments must also include:

* Blocking or non-blocking behavior.
* Allowable calling context (e.g. ISR).
* Mutual exclusion primitives (if used).

Sample function
===============

::

 /**
  * Set GPIO port configuration.
  *
  * Set GPIO port configuration. This includes if interrupts are enabled or not,
  * the level on which an interrupt is generated, the polarity of interrupts and
  * if GPIO-debounce is enabled or not. If interrupts are enabled it also
  * registers an ISR with the user defined callback function.
  *
  * @param[in] gpio GPIO port index to configure.
  * @param[in] cfg New configuration for GPIO port.
  *
  * @return Standard errno return type for QMSI.
  * @retval 0 on success.
  * @retval Negative @ref errno for possible error codes.
  */
