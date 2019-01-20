#ifndef MAIN_H
#define MAIN_H

int open();
int listAllDevs();
void printdev(libusb_device *dev);
unsigned char *getPacket(size_t count);
void writeBytes(unsigned char arr[], struct libusb_device_handle *dev_handle);
void sendStartInit(struct libusb_device_handle *dev_handle);
#endif
