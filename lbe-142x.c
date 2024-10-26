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
      u_int8_t buf[256];
      uint32_t current_f;

      struct hidraw_devinfo info;

//      GPSSettings *currentSettings = new GPSSettings;

   /* Open the Device with non-blocking reads. In real life,
      don't use a hard coded path; use libudev instead. 
   */
      if (argc == 1)
      {
	    printf("Usage: lbe-142x /dev/hidraw??\n\n");
            printf("        --f1:  integer within the range of 1 to 1100000000 (1Hz to 1.1GHz)\n               the frequency is saved in flash\n\n");
            printf(" --f1_nosave:  integer within the range of 1 to 1100000000 (1Hz to 1.1GHz)\n               the frequency is not saved\n\n");
            printf("      --out1:  [0,1]\n\n");
            printf("    --blink1   blinks output 1 LED for 3 seconds\n\n");
            return -1;
      }

      printf("Opening device %s\n", argv[1]);

      fd = open(argv[1], O_RDWR|O_NONBLOCK);

      if (fd < 0) 
      {
            perror("    Unable to open device");
            return 1;
      }

      //Device connected, setup report structs
      memset(&info, 0x0, sizeof(info));

      // Get Raw Info
      res = ioctl(fd, HIDIOCGRAWINFO, &info);
      
      if (res < 0) 
      {
            perror("HIDIOCGRAWINFO");
      } 
      else
      {
            if (info.vendor != VID_LBE || (info.product != PID_LBE_1420 && info.product != PID_LBE_1421)) {
                  printf("    Not a valid LBE-142x Device\n\n");
                  printf("    Device Info:\n");
                  printf("        vendor: 0x%04hx\n", info.vendor);
                  printf("        product: 0x%04hx\n", info.product);
                  return -1;//Device not valid
            }
      }

      /* Get Raw Name */
      res = ioctl(fd, HIDIOCGRAWNAME(256), buf);

      if (res < 0) {
            perror("HIDIOCGRAWNAME");
      }
      else {
            printf("Connected To: %s\n\n", buf);
      }

      /* Get Feature */
      buf[0] = 0x9; /* Report Number */
      res = ioctl(fd, HIDIOCGFEATURE(256), buf);

      if (res < 0) {
            perror("HIDIOCGFEATURE");
      } else {
	      printf("  Status:\n");
            //currentSettings->setParamsFromReadBuffer(buf,res);
            if (buf[0] == 0) {
		    if ((buf[1] & 0x15) == 0x15) {
		    	printf("    Device OK");
		    } else {
		    	if ((buf[1] & GPS_LOCK_BIT) != GPS_LOCK_BIT) {
		    		printf("\n    No GPS lock\n");	
		    	}
		    	if ((buf[1] & ANT_OK_BIT) != ANT_OK_BIT) {
		    		printf("\n    GPS antenna short circuit\n");	
		    	}
		    	if ((buf[1] & OUT1_EN_BIT) != OUT1_EN_BIT) {
		    		printf("\n    GPS antenna short circuit\n");	
		    	}
		    }
		    current_f = (buf[5] << 24) + (buf[4] << 16) + (buf[3] << 8) + buf[2];
	            printf("\n    Current Frequency: %i\n", current_f);
            }

            printf("\n");
      }

      /* Get Raw Name */
      res = ioctl(fd, HIDIOCGRAWNAME(256), buf);

      if (res < 0) {
            perror("HIDIOCGRAWNAME");
      }
      else {

	//Get CLI values as vars
	int blink = -1;
	int enable = -1;
	int save = -1;
	uint32_t new_f = 0xffffffff;
	processCommandLineArguments(argc, argv, &new_f, &blink, &enable, &save);
      	printf("  Changes:\n");
      	int changed = 0;
	if (new_f != 0xffffffff && new_f != current_f) {
	    //Set Frequency
	    printf ("    Setting Frequecy: %i\n", new_f);
	    
	    buf[0] = (save == 1 ? 4 : 3);//4 Save, 3 dont save
	    buf[1] = (new_f >>  0) & 0xff;
	    buf[2] = (new_f >>  8) & 0xff;
	    buf[3] = (new_f >> 16) & 0xff;
     	    buf[4] = (new_f >> 24) & 0xff;
	    /* Set Feature */
            res = ioctl(fd, HIDIOCSFEATURE(60), buf);
            if (res < 0) perror("HIDIOCSFEATURE");
            changed = 1;
	}
	if (enable != -1) {
	    buf[0] = 1;
	    buf[1] = enable & 0x01;
	    printf ("    Enable State :%i\n", enable);
	    /* Set Feature */
            res = ioctl(fd, HIDIOCSFEATURE(60), buf);
            if (res < 0) perror("HIDIOCSFEATURE");
            changed = 1;
	}
	if (blink != -1) {
	    buf[0] = 2;
	    printf ("    Blink LED\n");
	    /* Set Feature */
            res = ioctl(fd, HIDIOCSFEATURE(60), buf);
            if (res < 0) perror("HIDIOCSFEATURE");
            changed = 1;
	}
	if (!changed) {
	    printf("    No changes made\n");
	}
      }
      close(fd);

      return 0;
}


int processCommandLineArguments(int argc, char **argv, uint32_t *freq, int *blink, int *enable,int *save)
{
    int c;
    
    while (1)
    {
        static struct option long_options[] =
        {
                /* These options set a flag. */
                {"blink1", no_argument, 0, 0},
                /* These options don’t set a flag.
                    We distinguish them by their indices. */
                {"f1",    required_argument, 0, 'a'},
                {"f1_nosave",     required_argument, 0, 'b'},
                {"out1",   required_argument, 0, 'c'},
                {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "abc:d:f:",
                    long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        break;

        switch (c)
        {
            case 0:
            	*blink = 1;
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                        break;
                break;
            
            case 'a'://f1
                *freq = atoi(optarg);
                *save = 1;
                break;

            case 'b'://f1_nosave
                *freq = atoi(optarg);
                *save = 0;
                break;

            case 'c'://N2_HS
                *enable = atoi(optarg);
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }
    return 0;
}

