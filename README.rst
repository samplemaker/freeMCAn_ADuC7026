freemcan
========

.. contents::



What is freemcan?
-----------------

Originally the basic approach of freeMCAn was to implement a multi channel
analyzer software used for e.g. gamma spectroscopy on easy obtainable silicon
chips like ATMega (Atmel) or ADuC (Analog Devices). Improved over time
freeMCAn is no longer only a so called "multi channel analyzer". Moreover
it employs several functions of a data logger like data time
recording (the software provides an oscilloscope functionality) and
statistics counting.

There are two different software ports available:

  * Related to the ADuC7026 (Analog Devices) w. ARM7TDMI core:
    https://github.com/samplemaker/FreeMCAn_on_ADUC7026

  * Related to the ATMega644 (Atmel) silicon device:
    https://github.com/ndim/freemcan


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
  * POSIX/GNU/Linux/Unix host system
  * gcc_ compiler for host system

Toolchain: freeMCAn-ADuC is known to be build on Fedora 16 with newlib
and gcc based on an ARM Toolchain derived from following packages:

  * binutils-2.22
  * gcc-4.6.2
  * newlib-1.19.0
  * insight-6.8-1a

Note: FreeMCAn_ADUC7026 comes with a full target initialization and
has its own asic driver. That means beside a compiler (gcc) and a
standard c-library (newlib) no additional software is needed.

For building the internal code documentation (mostly of interest to
hackers), you additionally need

  * doxygen_
  * graphviz_

For creating the source lines of code (SLOC) summary, you additionally
need

  * sloccount_

.. _doxygen:   http://www.stack.nl/~dimitri/doxygen/index.html
.. _gcc:       http://gcc.gnu.org/
.. _graphviz:  http://www.graphviz.org/
.. _make:      http://www.gnu.org/software/make/
.. _sloccount: http://www.dwheeler.com/sloccount



Usage
-----

TBA.



The License
-----------

LGPLv2.1+



Hacking
-------


Subdirectory Contents
~~~~~~~~~~~~~~~~~~~~~

   aduc/
           Software driver for ADuC7026 / ARM7TDMI microcontroller

   aduc_docs/
           Some extra documentation. E.g. configuration of the
           level to edge ADC-trigger via PLA

   firmware/
           The device firmwares for ADuC7026 microcontroller
           (multichannel analyzing, oscilloscope ...)

   code-comparison/
           Some common tasks our firmware needs written in portable C
           and compiled for all platforms we have a cross compiler
           for. This lets us compare the assembly language generated
           for those platforms.

   hostware/
           All the software running on the PC host. For lack of a
           better word, we called it "hostware" to distinguish it from
           the "firmware". The firmware is running inside a linux terminal


Ideas
-----
