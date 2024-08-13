COMMIT MSG GUIDELINES
#####################

Overview
********

This page provides details on how to write a good commit messages for the QMSI
project.

.. contents::

Subject
*******

* Limit the subject line to 69 characters
* Use the subject to explain WHAT the commit is about
* Separate subject from body with a blank line
* Capitalize the subject line
* Do not end the subject line with a period
* Use the imperative mood in the subject line
* Add a lower-case topic to subject (i.e. spi: Fix RX FIFO overflow)

Body
****

* Wrap the body at 72 characters
* Use the body to explain the commit, WHY and HOW
* Add one empty line between body and commit meta-data (i.e. Change-Id:,
  Signed-off-by:)
* Add one Signed-off-by line per developer who worked on that patch
  ('git commit -s'). Note the sign-off line can be longer than 72 characters.
* You should answer the questions below when writing the commit msg body:

  1) Why is this patch necessary?
  It may fix a bug, it may add a feature, it may improve performance,
  reliability, stability, or just be a change for the sake of correctness.

  2) How does it address the issue?
  For short obvious patches this part can be omitted, but it should be
  a high level description of what the approach was.

  3) Which effects does the patch have?
  In addition to the obvious ones, this may include benchmarks,
  side effects, etc.

  4) How did I test this?
  Make it explicit if this was tested by you through an example, which
  unit tests cover it, etc.

Example
*******

::

  album: Modify name to reflect tracks contents

  No tracks on this album are about reality, thus its name turned out to be
  misleading. Calling it 'Keep it Real' was breaking consistency.

  Address this by first revisiting all songs and making sure none of them
  resembles reality.As an output, the album name is changed to 'Keep it Unreal'.
  There is no impact on footprint, but album cover art has to be re-worked.

  Tested with fans directly.

  Change-Id: I21765928735afas9996a65a7787698a9699a96435257a
  Signed-off-by: Mr Scruff <mr.scruff@ninjatune.com>
  Signed-off-by: R Smith <r.smith@ninjatune.com>
