/* 
 * File:   usbdriver.cpp
 *
 * Goal: Implementation of USB Panic Button "driver" in user mode
 *
 * Author: Stéphane Mouton
 *
 * Created on 8 juin 2008, 20:27
 */

#include <vector>


#include <vector>

#include "usbdriver.hpp"
#include <syslog.h>
#include <string.h>

//  (0202, 1130)
#define VENDOR_ID  0x1130 
#define PRODUCT_ID 0x0202
#define MESSAGE_SIZE 8

static char panicButtonMessage[] = {'U','S','B','C',0x00,0x08,0x01,0x00};

USBDriver::USBDriver(){
    buffer[0]='\0';
    nbDevices=0;
}

int USBDriver::findDevices(struct usb_bus *busses){
    struct usb_bus *bus;
    struct usb_device *dev;
    devs.clear();
    // Handles of found buttons are stored in a vector
    for (bus=busses; bus; bus=bus->next){
        for (dev=bus->devices; dev; dev=dev->next){
            if (dev->descriptor.idVendor==VENDOR_ID && dev->descriptor.idProduct==PRODUCT_ID){
                struct usb_dev_handle *handle;
                syslog (LOG_INFO,"USB Button with Vendor ID: %x, Product ID: %x and Serial Number:%x found\n", VENDOR_ID, PRODUCT_ID, dev->descriptor.iSerialNumber);
                if (!(handle=usb_open(dev))){
                    syslog( LOG_ERR,"Could not open USB device\n");
                    return nbDevices;
                }
                devs.push_back(handle);
                nbDevices++;
            }
        }
    }
    syslog (LOG_INFO,"Found %d panic button(s)\n", nbDevices);
    return nbDevices;
}

// Init USB communication and find device
int USBDriver::init(void){
    usb_init();
    usb_find_busses();
    usb_find_devices();
    busses=usb_get_busses();
    nbDevices=findDevices(busses);
    
    if (nbDevices==0){
        syslog( LOG_ERR,"Unable to find any Panic Button device...\n");
        return(0);
    }

    int count = devs.end()-devs.begin();
    for (int j  = 0; j < count; j++){
        usb_detach_kernel_driver_np(devs[j],0);
        usb_detach_kernel_driver_np(devs[j],1);
        usb_claim_interface(devs[j],0);
        usb_claim_interface(devs[j],1);
    }
    
    // Don't forget to allocate result space for the number of connected buttons:
    // they are stored separately from button handles
    result.numberPressed=0;
    result.total=nbDevices;
    // Switch from old style C memory allocation to C++?
    result.value=(unsigned char*)malloc(sizeof(unsigned char)*nbDevices);
    return(nbDevices);
}    


int USBDriver::getNumberButtons(void){
    return(nbDevices);
}

ButtonResult* USBDriver::requestButtonStatus(void){
    result.total=0;
    result.numberPressed=0;
    unsigned char read=0;
    int count = devs.end()-devs.begin();
    for (int j  = 0; j < count; j++){
        // Send request to read status of panic button
        usb_control_msg(devs[j],
                        USB_TYPE_CLASS|USB_RECIP_INTERFACE|USB_ENDPOINT_OUT,
                        0x09, 0x200, 0x01,
                        panicButtonMessage,
                        MESSAGE_SIZE, 0);
        memset((void*)buffer, 0, MESSAGE_SIZE);

        // Then get value and fill result table
        usb_control_msg(devs[j],
                        USB_TYPE_CLASS|USB_RECIP_INTERFACE|USB_ENDPOINT_IN,
                        0x01, 0x300, 0x00, buffer, MESSAGE_SIZE, 0);
        read=*buffer;
        result.total++;
        result.value[j]=read;
        if (read) {result.numberPressed++;}
    }
    return(&result);
}


void USBDriver::close(void){
    // the following memory space was allocated in init
    free(result.value);
    // Cleanly close USB communication
    int count = devs.end()-devs.begin();
    for (int j  = 0; j < count; j++){
        usb_release_interface(devs[j],0);
        usb_release_interface(devs[j],1);
        usb_close(devs[j]);
    }
}
