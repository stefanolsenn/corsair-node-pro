#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "main.h"

int main() {
    return open();
}

void printdev(libusb_device *dev) {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    printf("Device Class: %d\n", desc.bDeviceClass);
    printf("VendorID: %d\n", desc.idVendor);
    printf("VendorID: %X\n", desc.idVendor);
    printf("ProductID: %d\n", desc.idProduct);
    printf("ProductID: %X\n", desc.idProduct);

    struct libusb_config_descriptor *config;
    libusb_get_config_descriptor(dev, 0, &config);

    struct libusb_interface *inter;
    struct libusb_interface_descriptor *interdesc;
    struct libusb_endpoint_descriptor *epdesc;

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

   // unsigned char arr[64];
  /*  for (int i = 0; i < 64; i++) {
        arr[i] = 0x00;
    } */




    int k, actual_length;
    k = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_OUT), arr, 64, &actual_length, 0);
    if (k == 0 && actual_length == 64)
        printf("64 bytes was written to device\n");
    else
        printf("Error writing bytes to device\n");
    // ---------------------------------
    // Release interface, close session
    // ---------------------------------
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 0;
}

char *fillArray(char arr[]) {
    return NULL;
}


