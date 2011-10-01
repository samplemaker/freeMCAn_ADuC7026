freemcan
========

.. contents::



What is freemcan?
-----------------

The basic approach of freeMCAn was to implement a multi channel analyzer
software on bare silicon chips like ATMega or ADUC. Improved over time
freeMCAn is no longer only a so called "multi channel analyzer". It employs 
moreover several functions of a data logger like data time recording 
(oscilloscope functionality) and statistics counting.

This software port of freeMCAn is related to the ADUC7026 silicon device.
Note: There are other variants for other devices like ATMEGA.


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

Toolchain: freeMCAn - ADUC is known to be build on Fedora with newlib 
and gcc based on an ARM7 Toolchain:
  * binutils-2.21.tar.gz
  * insight-6.8-1a.tar.bz2
  * gcc-4.4.6.tar.bz2
  * newlib-1.19.0.tar.gz

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

   ADUC/
           Driver for ADUC7026 microcontroller

   firmware/
           The device firmware for ADUC7026 microcontroller

   code-comparison/
           Some common tasks our firmware needs written in portable C
           and compiled for all platforms we have a cross compiler
           for. This lets us compare the assembly language generated
           for those platforms.

   hostware/
           All the software running on the PC host. For lack of a
           better word, we called it "hostware" to distinguish it from
           the "firmware".

   emulator/
           Simple attempt at emulating the device connected to a
           device file by having an Erlang program connected to a
           Unix domain socket.



Ideas
-----
