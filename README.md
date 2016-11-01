# Capacitive sensing with Cypress PSoC-4
*2015/2016 Jeff Snyder, Till Bovermann*

This document explains how to use a [Cypress PSoC 4 CY8CKIT-049 Prototyping Kit](http://www.cypress.com/?rid=92146) as a capacitive sensing element that forwards the measured data via a serial connection. The board I use is build around a [PSoC 4 chip](http://www.cypress.com/psoc4/), which is a cheap and powerful programmable microcontroller with integrated re-programmable analogue cirquitry.

Unfortunately, the whole process requires a windows computer but once it is set up, it is fairly easy to use. You may also install the whole software in a virtual machine on linux or OSX. Wine is unfortunately not supported.

## Materials

+ Cypress PSoC® 4 CY8CKIT-049 Prototyping Kit ([Mouser](http://www.mouser.de/search/refine.aspx?Ntk=P_MarCom&Ntt=165961099))
+ PC running Windows 7 (or a virtual installation e.g., on VirtualBox)
    + Windows 7 ISO available e.g. [here](http://answers.microsoft.com/en-us/windows/forum/windows_7-windows_install/download-windows-7-enterprise-edition-sp1-iso/37cfd9f3-48cd-4f88-becb-0de4ab74841b)
+ Cypress software available from [here](http://www.cypress.com/?rid=92146)
  + [PSoC Creator](http://www.cypress.com/go/psoccreator)
  + [USB-Serial Software Development Kit](http://www.cypress.com/?rID=83110)
  + [CY8CKIT-049-42xx Kit Only](http://www.cypress.com/?docID=51317&dlm=1) or  [CY8CKIT-049-41xx Kit Only](http://www.cypress.com/?docID=51314&dlm=1)

## Installation

+ Install *PSoc Creator*
+ Install *USB-Serial Software Development Kit*
+ Install the *CY8CKIT-049* add-on
+ At some point, you will be asked to install *Microsoft VisualStudio*. Just proceed as asked.


## Example Code

Copy the capsense example folder to your machine. If on a virtual machine, *do not* place it in a folder that is shared with the host as this seem to cause trouble when compiling.

      C:\Users\lfsaw\Documents\jeffsStuff\jeffsStuff\USB-UART
