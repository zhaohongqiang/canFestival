#ifndef __Dev_CAN_AM335x_H_INCLUDED
#define __Dev_CAN_AM335x_H_INCLUDED

#include <dcan.h>
#include "canam335x.h"
#include <errno.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <atomic.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG_DRVR
//#define CLRINTPNDSTAT	//20131029 ��CAN�жϴ�������������жϹ���״̬,��û�����õ�
//#define DISDAR	//20131029  ��ֹ�Զ��ط�
#define 	TXDONOTWAIT	//sundh 140323	�ڷ����в��ȴ�
int device_init(int port, unsigned int bitrate);
const struct sigevent *can_intr(void *area, int id);
extern struct sigevent	event_can;
extern CAN_DUPLEX_Activity_AM335X AM335X_CANDEV_Parameter;
extern CANDEV_AM335X AM335X_CANDEV[2];
//void dcan_setmask(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id);
void dcan_rxack(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id);
void dcan_txack(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id);
void can_am335x_tx(CANDEV_AM335X *dev, canmsg_t *txmsg);
int Transfer(appMessage_t *msg,unsigned char port);
int device_destory(int instance);
void can_print_reg(CANDEV_AM335X_INFO *devinfo);
#endif