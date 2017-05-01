# C++ USB panic button management #

## Summary ##
* version 2.5

The usbpanic _daemon_ allows to use the gadget USB panic button (see below) identified by Vendor ID & Product_ID (0x1130:0x0202) under linux.

![USB panic button](./usbpanicbutton.jpg)

A command is associated to a button press using configuration file and is launched synchronously or in background.
It is possible to drive several panic buttons at once using usbpanic.

## Build & installation ##
### Dependencies ###
The libusb library must be installed to run.

Development version of libusb needs also to be deployed in order to buld usbpanic (directive -l usb is used during build)

This software relies on iniparser library.

### Build ###
To compile usbpanic, just type

`make -f Makefile.written`

The resulting executable is in the build directory.

### Deployment instructions ###
Install in /usr/local/bin using Makefile

`make -f Makefile.written install`

## How to use ##

Launch the daemon at startup (systemd file provided) after having prepared a configuration file

Default location of configuration file is /etc/usbpanic.conf

This value can be overridden with -f parameters

## Configuration ##

The configuration file follow structure of .INI files
 
More details about syntax can be found in documentation of [iniparser parser library](./iniparser/html/index.html)

An sample of configuration file is provided: see [usbpanic.conf.example](./usbpanic.conf.example)

### Parameters ###

The configuration file is divided into two sections : the Global and the Action one.

[Global] section contains _daemons_'settings
* scanperiod (int): amount of time in milliseconds between two polls of button status (defaut: 1000)
* asynchronous (boolean): define if actions on button press are launched asynchronously or not (default is true)

[Action] section gathers definition of action to execute on button press
* defaultscript (string): command or script to launch when no button specific action is defined (defaut: /bin/true)
* scriptX where X is button number (0, 1, ...) (string): action to perform when button X is pressed.
 
Please note that it is not possible to reliably guess buttons number before plugin and testing them.   

## Contact & Contributions ##

This software was started in 2007 in order to be used at review of AssessGrid EU FP6 project

For any question, contact stephane@mouton.com

This software uses iniparser library written by Nicolas Devilla.

More info at http://ndevilla.free.fr/iniparser/

### Related projects and documentation ###

* John Bruin wrote a [python program](http://www.johnbruin.net/index.php/2008/11/30/new-software-for-your-usb-panic-button/) to exploit the USB panic button
* Article, in French, on USB programming [Linux Magazine Num 101 : 90> Programmation noyau sous Linux Partie 4 : les pilotes USB](http://ed-diamond.com/produit.php?ref=lmag101&id_rubrique=1&caracteristique=1-2-&caracdisp=2-3-)

https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

## License ##
This software is released under MIT License. See LICENSE for full informations
