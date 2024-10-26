//#include <iostream>
//using namespace std;

/* Linux */
//#include <linux/types.h>
#include <linux/hidraw.h>

/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
// #include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <errno.h>
#include <stdint.h>
#include <getopt.h>

//Leo LBE-142x defines, dont need anything external this time
#define VID_LBE		0x1dd2

#define PID_LBE_1420	0x00002443
#define PID_LBE_1421	0x0000ffff


#define GPS_LOCK_BIT 		0x01
#define RES_1_BIT 		0x02
#define ANT_OK_BIT 		0x04
#define RES_3_BIT 		0x08
#define OUT1_EN_BIT 		0x10
//SetFeatureReportIDs
#define LBE_1420_EN_OUT1	0x01
#define LBE_1420_BLINK_OUT1	0x02
#define LBE_1420_SET_F1_NO_SAVE	0x03
#define LBE_1420_SET_F1		0x04

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

#define HIDIOCGRAWNAME(len)     _IOC(_IOC_READ, 'H', 0x04, len)


int processCommandLineArguments(int argc, char **argv, int *freq, int *blink, int *enable, int *save);

