# C++ USB panic button management #

## Summary ##
* version 2.5

The usbpanic _daemon_ allows to use the gadget USB panic button (see below) identified by Vendor_ID:Product_ID 0x1130:0x0202under linux.
![USB panic button](./usbpanicbutton.jpg)
A command is associated to a button press using configuration file and is launched synchronously or in background.
It is possible to drive several panic buttons at once using usbpanic.

## Build & installation ##
### Dependencies ###
The libusb library must be installed to run
Development version of libusb needs also to be deployed in order to buld usbpanic (directive -l usb is ued during build)

### Build ###



-lusb

iniparser


* Deployment instructions

-

### How to use ###


* Configuration


### Who do I talk to? ###



* Repo owner or admin

* Other community or team contact

## Contributions ##

This software uses iniparser library written by Nicolas Devilla.
Mare info at http://ndevilla.free.fr/iniparser/

### Related projects and documentation ###

* John Bruin wrote a [python program](http://www.johnbruin.net/index.php/2008/11/30/new-software-for-your-usb-panic-button/) to exploit the USB panic button
* Article, in French, on USB programming [Linux Magazine Num 101 : 90> Programmation noyau sous Linux Partie 4 : les pilotes USB](http://ed-diamond.com/produit.php?ref=lmag101&id_rubrique=1&caracteristique=1-2-&caracdisp=2-3-)

https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html

## License ##
This software is released under MIT License. See LICENSE for full informations
