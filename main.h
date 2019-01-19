#ifndef MAIN_H
#define MAIN_H

int open();
int listAllDevs();
void printdev(libusb_device *dev);
unsigned char *getPacket(int count);
void writeBytes(unsigned char arr[], struct libusb_device_handle *dev_handle);
#endif
