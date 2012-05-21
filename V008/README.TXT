G & D Android Linux release

This package contains 2 parts:

1) Mobicore Linux Daemon & client lib
  All the components are in the *mobicore* folder.
  To enable building the components all you need is to copy the mobicore folder
to the Android source tree.

  It has been tested with Android 2.3.6 and Android 4.0.1
  The source tree tested has been:

Android -
 - external
   - mobicore
     - common
     - daemon
     - include

Other places for the folder might work but have never been tested!    

2) Mobicore Linux Driver

  The Mobicore Linux drivers are located in the *gud* folder in this package.
  The package is now designed to be dropped in the Linux source tree in the 
drivers folder.

  The source tree tested has been:

Linux -
 - drivers
   - gud
     - include
     - Kconfig
     - Makefile
     - MobicoreDriver
     - MobicoreKernelApi

  Please note that dropping it in a different folder in the Linux source tree
will *NOT* work!

  To enable automatic building inside the kernel follow these steps:
1) copy the gud folder from this package to linux/drivers folder
2) add this line:
source "drivers/gud/Kconfig" 
to the file in linux/drivers/Kconfig just BEFORE the last endmenu line
3) add this line:
obj-y               += gud/
at the end of the file linux/drivers/Makefile
4) run make menuconfig and in the "Device Drivers" page please select 
"Linux Mobicore Support" and also "Linux Mobicore API":

<*> Linux Mobicore Support
[ ]   Mobicore Module debug mode
<*>   Linux Mobicore API

5) Run make again and the mobicore components will be included in the kernel
image
