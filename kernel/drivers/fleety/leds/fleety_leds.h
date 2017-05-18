#ifndef _FLEETY_LEDS_H_
#define _FLEETY_LEDS_H_

int fleety_usb_switch(bool host);
int fleety_usbhub_reset(void);
int fleety_usb2eth_reset(void);

int fleety_amp_standby(int on);
int fleety_amp_mute(int on);

#endif
