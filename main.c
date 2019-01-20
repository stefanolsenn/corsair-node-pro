#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include "main.h"

int main() {
    return open();
}

void printdev(libusb_device *dev) {
    struct libusb_device_descriptor desc;
     libusb_get_device_descriptor(dev, &desc);
    printf("Device Class: %d\n", desc.bDeviceClass);
    printf("VendorID: %d\n", desc.idVendor);
    printf("VendorID: %X\n", desc.idVendor);
    printf("ProductID: %d\n", desc.idProduct);
    printf("ProductID: %X\n", desc.idProduct);

    struct libusb_config_descriptor *config;
    libusb_get_config_descriptor(dev, 0, &config);

    const struct libusb_interface *inter;
    const struct libusb_interface_descriptor *interdesc;
    const struct libusb_endpoint_descriptor *epdesc;

    for (int i = 0; i < (int) config->bNumInterfaces; i++) {
        inter = &config->interface[i];

        for (int j = 0; j < inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            printf("InterfaceNumber:%d\n", interdesc->bInterfaceNumber);
            printf("NumEndpoints:%d\n", interdesc->bNumEndpoints);

            for (int k = 0; k < (int) interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];
                printf("DescriptorType:%d\n", epdesc->bDescriptorType);
                printf("EndpointAddress:%d\n", epdesc->bEndpointAddress);
            }
        }
    }
    libusb_free_config_descriptor(config);
    printf("--------------------\n");
}

int listAllDevs() {
    libusb_device **devs; // Pointer to pointer of device - Gets array of devices
    libusb_context *ctx = NULL;
    int r;
    ssize_t cnt;
    r = libusb_init(&ctx);
    if (r < 0) {
        printf("Error loading libusb");
        return 1;
    }
    libusb_set_option(ctx, 3);
    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        printf("Error - Could not load list of devices");
        return 1;
    }

    ssize_t i;
    for (i = 0; i < cnt; i++) {
        printdev(devs[i]);
    }
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
    return 0;
}

int open() {
    libusb_device **devs;
    libusb_device_handle *dev_handle;
    libusb_context *ctx = NULL;
    int r;
    ssize_t cnt;
    r = libusb_init(&ctx);

    if (r < 0) {
        printf("Init error.");
        return 1;
    }

    libusb_set_option(ctx, 3);
    cnt = libusb_get_device_list(ctx, &devs);
    dev_handle = libusb_open_device_with_vid_pid(ctx, 6940,
                                                 3083); // VendorID & ProductID of the corsair lighting pro hub

    if (dev_handle == NULL) {
        printf("Could not open device.");
        return 1;
    } else {
        printf("Device opened.\n");
    }
    libusb_free_device_list(devs, 1);

    // Find out if the kernel driver is attached, and detach it, so we can claim the interface.
    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
        printf("Kernel driver active\n");
        if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
            printf("Kernel driver detached\n");
        }
    }

    int i;
    i = libusb_claim_interface(dev_handle, 0);
    if (i < 0) {
        printf("Could not claim interface\n");
        return 1;
    }
    printf("Interface claimed\n");

    sendStartInit(dev_handle);
    // ---------------------------------
    // Release interface, close session
    // ---------------------------------
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 0;
}

void sendStartInit(struct libusb_device_handle *dev_handle) {
	unsigned char *one = getPacket(64);
	one[0] = 0x37; // get into color mode
	writeBytes(one, dev_handle);
	free(one);
	
	unsigned char *two = getPacket(64);
	two[0] = 0x35;
	two[1] = 0x00;
	two[2] = 0x00;
	two[3] = 0x1e; // for 3 led strips
	two[4] = 0x00;
	two[5] = 0x01;
	two[6] = 0x01;
	writeBytes(two, dev_handle);
	free(two);
	
/* 	unsigned char *three = getPacket(64);
	three[0] = 0x3b;
	three[1] = 0x00;
	three[2] = 0x01;
	writeBytes(three, dev_handle);
	free(three);
	
	unsigned char *four = getPacket(64);
	four[0] = 0x38;
	four[1] = 0x00;
	four[2] = 0x02;
	writeBytes(four, dev_handle);
	free(four);

	unsigned char *five = getPacket(64);
	five[0] = 0x34;
	writeBytes(five, dev_handle);
	free(five);

 	unsigned char *six = getPacket(64);
	six[0] = 0x37;
	six[1] = 0x01;
	writeBytes(six, dev_handle);
	free(six);

	unsigned char *seven = getPacket(64);
	seven[0] = 0x34;
	seven[1] = 0x01;
	writeBytes(seven, dev_handle);
	free(seven);

	unsigned char *eigth = getPacket(64);
	eigth[0] = 0x38;
	eigth[1] = 0x01;
	eigth[2] = 0x01;
	writeBytes(eigth, dev_handle);
	free(eigth);

	unsigned char *nine = getPacket(64);
	nine[0] = 0x33;
	nine[1] = 0xff;
	writeBytes(nine, dev_handle);
	free(nine);
*/
	//-------------------------------------
	// Color packets
	//-------------------------------------
 	unsigned char *ten = getPacket(64);
	ten[0] = 0x32;
	ten[1] = 0x00;
	ten[2] = 0x00;
	ten[3] = 0x1e;
	ten[4] = 0x00;
	for (int i = 5; i< 35; i++) {
		ten[i] = 0xff;
	}
	writeBytes(ten, dev_handle);
	free(ten);
	unsigned char *el = getPacket(64);
	el[0] = 0x32;
	el[1] = 0x00;
	el[2] = 0x00;
	el[3] = 0x1e;
	el[4] = 0x01;
	for (int i = 5; i< 35; i++) {
		el[i] = 0xff;
	}
	writeBytes(el, dev_handle);
	free(el);
	unsigned char *tw = getPacket(64);
	tw[0] = 0x32;
	tw[1] = 0x00;
	tw[2] = 0x00;
	tw[3] = 0x1e;
	tw[4] = 0x02;
	for (int i = 5; i< 35; i++) {
		tw[i] = 0xff;
	}
	writeBytes(tw, dev_handle);
	free(tw);

}

unsigned char *getPacket(size_t bcount) {
	unsigned char *arr = malloc(bcount);
    	
	if (!arr)
		return NULL;

	for (size_t i = 0; i < bcount; i++) {
		arr[i] = 0x00;
	}

	return arr;	
}

void writeBytes(unsigned char arr[], struct libusb_device_handle *dev_handle) {
	int i, actual_length;
	i = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_OUT), arr, 64, &actual_length, 0); // dont hardcore the actual bytes to be written - measure the size of allocated pointer

    	if (i == 0 && actual_length == 64) // same here
        	printf("64 bytes was written to device\n");
    	else
        	printf("Error writing bytes to device\n");
}

