//#include <iostream>
//using namespace std;

/* Linux */
#include <linux/hidraw.h>

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

#include "lbe-142x.h"



int main(int argc, char **argv)
{
    printf("Leo Bodnar LBE-142x GPS locked clock source config\n");

    int fd;
    int i, res, desc_size = 0;
    u_int8_t buf[HID_MAX_DESCRIPTOR_SIZE];
    uint32_t cur_f;
    int blink = -1;
    int enable = -1;
    int save = -1;
    char dev[60];
    uint32_t new_f = 0xffffffff;

    get_args(argc, argv, dev, &new_f, &blink, &enable, &save);

    struct hidraw_devinfo info;
    memset(&info, 0x0, sizeof(info));

    printf("Opening device %s\n", dev);

    fd = open(dev, O_RDWR|O_NONBLOCK);

    if (fd < 0) {
        perror("Unable to open device");
        return 1;
    }

    res = get_device_name(fd);
    res = get_device_info(fd);

    res = get_device_status(fd, &cur_f);

    if ( blink == 1 )
        blink_led(fd);

    if ( enable == 0 )
        enable_output(fd, 0);
    else
        enable_output(fd, 1);

    if (new_f != 0xffffffff)
        res = set_freq(fd, new_f, cur_f, save);

    res = get_device_status(fd, &cur_f);

    close(fd);
    return 0;
}

int enable_output(int fd, int e){

    int res;
    u_int8_t buf[60];
    buf[0] = 1;
    buf[1] = e & 0x01;

    /* Set Feature */
    res = ioctl(fd, HIDIOCSFEATURE(60), buf);
    if (res < 0) {
        perror("HIDIOCSFEATURE");
        return -1;
    }

    return 0;
}

int set_freq(int fd, uint32_t new_f, uint32_t cur_f, int save){
    int res;
    u_int8_t buf[60];

    if (new_f != 0xffffffff && new_f != cur_f) {
        printf ("\tSet Frequecy: %i Hz\n", new_f);
   
    if (save)
        printf ("\tNew frequecy %i Hz saved\n", new_f);

        buf[0] = (save == 1 ? 4 : 3);//4 Save, 3 dont save
        buf[1] = (new_f >>  0) & 0xff;
        buf[2] = (new_f >>  8) & 0xff;
        buf[3] = (new_f >> 16) & 0xff;
        buf[4] = (new_f >> 24) & 0xff;

        /* Set Feature */
        res = ioctl(fd, HIDIOCSFEATURE(60), buf);
        if (res < 0) {
            perror("HIDIOCSFEATURE");
            return -1;
        }
    }
    return 0;
}

int blink_led(int fd){
    int res;
    u_int8_t buf[60];
    buf[0] = 2;

    /* Set Feature */
    res = ioctl(fd, HIDIOCSFEATURE(60), buf);
    if (res < 0) {
        perror("HIDIOCSFEATURE");
        return -1;
    }
    printf ("Blink LED\n");

    return 0;
}

int get_device_name(int fd) {
    int res;
    u_int8_t buf[HID_MAX_DESCRIPTOR_SIZE];

    /* Get Raw Name */
    res = ioctl(fd, HIDIOCGRAWNAME(256), buf);

    if (res < 0) {
          perror("HIDIOCGRAWNAME");
          return -1;
    }
    printf("Connected To: \n\t%s\n\n", buf);

    return 0;
}

int get_device_info(int fd) {

    int res;
    struct hidraw_devinfo info;
    memset(&info, 0x0, sizeof(info));

    // Get Raw Info
    res = ioctl(fd, HIDIOCGRAWINFO, &info);

    if (res < 0) {
        perror("HIDIOCGRAWINFO");
        return -1;
    }
    if (info.vendor == VID_LBE && (info.product == PID_LBE_1420 || info.product == PID_LBE_1421)) {
        printf("Device Info:\n");
        printf("\tbus type: 0x%hx\n", info.bustype);
        printf("\tvendor: 0x%hx\n", info.vendor);
        printf("\tproduct: 0x%hx\n\n", info.product);
    } else {
        printf("Error: Not a valid LBE-142x Device\n\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int get_device_status(int fd, uint32_t *cur_f) {

    int res;
    u_int8_t buf[HID_MAX_DESCRIPTOR_SIZE];
    memset(&buf, 0x0, HID_MAX_DESCRIPTOR_SIZE);

    buf[0] = 0x9;

    res = ioctl(fd, HIDIOCGFEATURE(256), buf);

    if (res < 0) {
        perror("HIDIOCGFEATURE");
        return -1;
    }
    printf("Status:\n");

    if (buf[1] & 0x1)
        printf("\tGPS lock\n");
    else
        printf("\tNo GPS lock\n");

    if (buf[1] >> 3 & 0x1)
        printf("\tGPS antenna connected\n");
    else
        printf("\tGPS antenna short circuit\n");

    if (buf[1] >> 4 & 0x1)
        printf("\tOutput enabled\n");
    else
        printf("\tOutput disabled\n");

    *cur_f = (buf[8] << 16) + (buf[7] << 8) + buf[6];
    printf("\tCurrent Frequency: %i Hz\n\n", *cur_f);

    return 0;
}

int get_args(int argc, char **argv, char *dev, uint32_t *freq, int *blink, int *enable, int *save) {

    // Parse command-line options
    int opt;
    int flag = 0;

    while ((opt = getopt(argc, argv, "hdfbns")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [-h] [-d filename]\n", argv[0]);
                printf("  -h           Display this help message\n");
                printf("  -d device    Specify a file to process (/dev/hidraw0)\n");
                printf("  -f frequency Specify an integer within the range of 1 to 1100000000 (1Hz to 1.1GHz)\n");
                printf("  -b           blink led (2s)\n");
                printf("  -s           Save frequency\n");
                printf("  -n           Deactivate output (output is enabled by default)\n");
                exit(EXIT_SUCCESS);
            case 'd':
                printf("Opening device: %s\n", optarg);
                strncpy(dev, argv[optind], 60*sizeof(char)); 
                flag += 1;
                break;
            case 'f':
                *freq = atoi(argv[optind]);
                if ( *freq < 1 || *freq > 1100000000) {
                    printf("Error: Frequency not in range 1Hz to 1.1GHz\n");
                    exit(EXIT_FAILURE);
                }
                flag += 1;
                break;
            case 'b':
                *blink = 1;
                break;
            case 'n':
                *enable = 0;
                break;
            case 's':
                *save = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option: %c\n", optopt);
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Error parsing options\n");
                exit(EXIT_FAILURE);
            }
    }
    // Check for missing required options
    if (flag < 2) {
        fprintf(stderr, "Error: Missing required option\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

