#ifndef LBE_142x_H
#define LBE_142x_H

#include <stdint.h>

//Leo LBE-142x defines, dont need anything external this time
#define VID_LBE                  0x1dd2

#define PID_LBE_1420             0x2443
#define PID_LBE_1421             0x244f


#define GPS_LOCK_BIT             0x01
#define RES_1_BIT                0x02
#define ANT_OK_BIT               0x04
#define RES_3_BIT                0x08
#define OUT1_EN_BIT              0x10

//SetFeatureReportIDs
#define LBE_1420_EN_OUT1         0x01
#define LBE_1420_BLINK_OUT1      0x02
#define LBE_1420_SET_F1_NO_SAVE  0x03
#define LBE_1420_SET_F1          0x04


int get_args(int argc, char **argv, char *dev, uint32_t *freq, int *blink, int *enable, int *save);

int enable_output(int fd, int e);
int set_freq(int fd, uint32_t new_f, uint32_t cur_f, int save);
int blink_led(int fd);
int get_device_name(int fd);
int get_device_info(int fd);
int get_device_status(int fd, uint32_t *cur_f);

#endif // LBE_142x_H
