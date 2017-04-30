/* 
 * File:   usbdriver.hpp
 *
 * Goal: header for USB Panic Button "driver" in user mode
 *
 * Author: Stéphane Mouton
 *
 * Created on 8 juin 2008, 20:27
 */

#ifndef _USBDRIVER_HPP
#define	_USBDRIVER_HPP

#include <vector>
using namespace std ;

extern "C" {
#include <usb.h>
}

#define MESSAGE_SIZE 8

struct ButtonResult {
    unsigned int   total;
    unsigned int   numberPressed;
    unsigned char* value;
};

class USBDriver {
private:
    struct usb_bus *busses;
    //struct usb_dev_handle *dev;
    vector<struct usb_dev_handle *> devs;
    char buffer[MESSAGE_SIZE];
    int nbDevices;
    ButtonResult result;
    int findDevices(struct usb_bus *busses);
public:
    USBDriver();
    // To be used after constructor, init deals with hardware setup
    int init(void);
    // Returns the number of detected buttons
    int getNumberButtons(void);
    // Returns status of detected buttons
    ButtonResult* requestButtonStatus(void);
    // cleanly ends USB communication
    void close(void);
};

#endif	/* _USBDRIVER_HPP */
