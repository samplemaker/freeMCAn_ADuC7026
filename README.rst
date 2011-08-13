freemcan
========

.. contents::



What is freemcan?
-----------------

(FIXME) freemcan is stuff.


Why the name?
~~~~~~~~~~~~~

  * We wanted it to be Free Software.
  * MCA is the acronym for Multi Channel Analyzer which is the common
    term for the device.
  * "freemca" produced a few hits on Google. "freemcan" did not.
    Brand name collision wise, an unused word appeared to be the
    better choice.



The Plan
~~~~~~~~

(FIXME) No plan.




Building
--------

  $ make

Installation is not supported at this time.


Software Requirements
~~~~~~~~~~~~~~~~~~~~~

  * GNU make_
  * arm-elf-gcc based ARM toolchain
  * POSIX/GNU/Linux/Unix host system
  * gcc_ compiler for host system

For building the internal code documentation (mostly of interest to
hackers), you additionally need

  * doxygen_
  * graphviz_

For creating the source lines of code (SLOC) summary, you additionally
need

  * sloccount_

.. _arm-elf-gcc:   http://gcc.gnu.org/
.. _doxygen:   http://www.stack.nl/~dimitri/doxygen/index.html
.. _gcc:       http://gcc.gnu.org/
.. _graphviz:  http://www.graphviz.org/
.. _make:      http://www.gnu.org/software/make/
.. _sloccount: http://www.dwheeler.com/sloccount



Programming
-----------

After building, you need to write the firmware to your hardware. You may
use OPENOCD to do that.


The License
-----------

LGPLv2.1+



Hacking
-------


Subdirectory Contents
~~~~~~~~~~~~~~~~~~~~~


   firmware/
           The device firmware for Atmel ATmega644 microcontroller

   code-comparison/
           Some common tasks our firmware needs written in portable C
           and compiled for all platforms we have a cross compiler
           for. This lets us compare the assembly language generated
           for those platforms.

   hostware/
           All the software running on the PC host. For lack of a
           better word, we called it "hostware" to distinguish it from
           the "firmware".

   ADUC/
           ADUC7026 driver and chipset hardware support.


