
How to build Quarks

1. Requirements

To build Quarks, you need a decent GNU compiler collection, GNU make
and GNU binutils. Further you are recommended to build on a Unix-like
system like Linux, *BSD, BeOS or QNX. Building under a CygWin environment
should work as well but is not tested.
If you are cross-compiling for a different target, you need to make a
cross-compiler first. Make sure you change the paths to your buildtools
in the toplevel Makeconf file.

2. Build

Just go into the toplevel directory and type 'make'.
To build a bootable floppy, type 'make floppy'. That creates an image file
suitable for writing directly to a floppy. To get the image on disk,
type 'dd if=image of=/dev/fd0' (or whatever the path to your raw floppy device is).
Under DOS/Windows you can use rawrite.

3. Installation

...