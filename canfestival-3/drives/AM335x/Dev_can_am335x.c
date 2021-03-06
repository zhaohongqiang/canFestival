#include <stdlib.h>
#include <stdio.h>
#include <am335x.h>
#include "hw_cm_wkup.h"
#include "Dev_can_am335x.h"
#include <sys/neutrino.h>

//#define CANDEV_NUM 2

//static volatile unsigned int isrTxFlag = 1;
//static volatile unsigned int isrRxFlag = 1;
unsigned char FrmData[16];
CANDEV_AM335X 	AM335X_CANDEV[2] ;
CAN_DUPLEX_Activity_AM335X AM335X_CANDEV_Parameter;
struct sigevent	event_can;
//static unsigned int data[2];
//#define CAN_DIS
//#define DEBUG_DRVR
// Function prototypes
#ifdef CLRINTPNDSTAT
void dcan_CommandRegSet(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id);
#endif
#ifdef DISDAR
int Is_NewDat(CANDEV_AM335X_INFO *devinfo);
#endif
void create_device(CANDEV_AM335X_INIT *devinit);
void can_init_hw(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit);
void can_init_mailbox(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit);
void can_init_intr(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit);
void dcan_setobj(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id, int domask, int doarb, int doctrl, int dodata, int dodatb, int startxfer, int dodma);
void init_msg_rx(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit, int id, int mxid);
void init_msg_tx(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit, int id, int mxid);
void dcan_getobj(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id, int domask, int doarb, int doctrl, int dodata, int dodatb, int eoi, int dodma);
void can_am335x_tx(CANDEV_AM335X *dev, canmsg_t *txmsg);
void dcan_start_tx(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id);
void dcan_txack(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id);
void can_am335x_debug(CANDEV_AM335X_INFO *devinfo);
void can_print_mailbox(CANDEV_AM335X_INFO *devinfo);
void can_print_reg(CANDEV_AM335X_INFO *devinfo);
void DCANModuleClkConfig(void);
const struct sigevent *can_intr(void *area, int id);
//uintptr_t                   canbase;
/**
 * \brief   This function will enable the module clocks for DCAN.
 *
 * \return  None.
 *
 */
/*************************************************************************\
 * Registers Definition
\*************************************************************************/
#define CM_PER_L4LS_CLKSTCTRL   	(0x0)
#define CM_PER_L3S_CLKSTCTRL   		(0x4)
#define CM_PER_L3_CLKSTCTRL   		(0xc)
#define CM_PER_L4LS_CLKCTRL   		(0x60)
#define CM_PER_DCAN0_CLKCTRL   		(0xc0)
#define CM_PER_DCAN1_CLKCTRL   		(0xc4)
#define CM_PER_L3_INSTR_CLKCTRL   	(0xdc)
#define CM_PER_L3_CLKCTRL   		(0xe0)
#define CM_PER_OCPWP_L3_CLKSTCTRL   (0x12c)
/* L3S_CLKSTCTRL */
#define CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK   (0x00000008u)
#define CM_PER_L3S_CLKSTCTRL_CLKTRCTRL   			(0x00000003u)
#define CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP   	(0x2u)

/* L3_CLKSTCTRL */
#define CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK   	(0x00000010u)
#define CM_PER_L3_CLKSTCTRL_CLKTRCTRL   			(0x00000003u)
#define CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP   	(0x2u)


/* L3_INSTR_CLKCTRL */
#define CM_PER_L3_INSTR_CLKCTRL_MODULEMODE   		(0x00000003u)
#define CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE   (0x2u)

/* L3_CLKCTRL */
#define CM_PER_L3_CLKCTRL_MODULEMODE   			(0x00000003u)
#define CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE   	(0x2u)

/* OCPWP_L3_CLKSTCTRL */
#define CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK   	(0x00000010u)
#define CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK   	(0x00000020u)
#define CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL   					(0x00000003u)
#define CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP   			(0x2u)

/* L4LS_CLKSTCTRL */
#define CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL   				(0x00000003u)
#define CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_CAN_CLK   	(0x00000800u)
#define CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP   		(0x2u)
#define CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK   	(0x00000100u)
/* L4LS_CLKCTRL */
#define CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE   	(0x2u)
#define CM_PER_L4LS_CLKCTRL_MODULEMODE   			(0x00000003u)

/* DCAN1_CLKCTRL */
#define CM_PER_DCAN1_CLKCTRL_MODULEMODE   			(0x00000003u)
#define CM_PER_DCAN1_CLKCTRL_MODULEMODE_ENABLE   	(0x2u)

/* DCAN0_CLKCTRL */
#define CM_PER_DCAN0_CLKCTRL_MODULEMODE   			(0x00000003u)
#define CM_PER_DCAN0_CLKCTRL_MODULEMODE_ENABLE   	(0x2u)

/* L4LS_CLKSTCTRL */
#define CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_CAN_CLK   	(0x00000800u)
#define CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK   	(0x00000100u)
void DCANModuleClkConfig(void)
{
	uintptr_t                   base;
	base = mmap_device_io(CM_PRCM_SIZE, PRCM_BASE);
	out32(base + CM_PER_L3S_CLKSTCTRL,CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP) ;

    while((in32(base + CM_PER_L3S_CLKSTCTRL) &
     CM_PER_L3S_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    out32(base + CM_PER_L3_CLKSTCTRL,CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    while((in32(base + CM_PER_L3_CLKSTCTRL) &
     CM_PER_L3_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    out32(base + CM_PER_L3_INSTR_CLKCTRL,CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    while((in32(base + CM_PER_L3_INSTR_CLKCTRL) &
                               CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
                                   CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    out32(base + CM_PER_L3_CLKCTRL,CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    while((in32(base + CM_PER_L3_CLKCTRL) &
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    out32(base + CM_PER_OCPWP_L3_CLKSTCTRL,CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    while((in32(base + CM_PER_OCPWP_L3_CLKSTCTRL) &
                              CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL) !=
                                CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    out32(base + CM_PER_L4LS_CLKSTCTRL,CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);


    while((in32(base + CM_PER_L4LS_CLKSTCTRL) &
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL) !=
                               CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    out32(base + CM_PER_L4LS_CLKCTRL,CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);


    while((in32(base + CM_PER_L4LS_CLKCTRL) &
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

    out32(base + CM_PER_DCAN1_CLKCTRL,CM_PER_DCAN1_CLKCTRL_MODULEMODE_ENABLE);

    while((in32(base + CM_PER_DCAN1_CLKCTRL) &
                         CM_PER_DCAN1_CLKCTRL_MODULEMODE) !=
                         CM_PER_DCAN1_CLKCTRL_MODULEMODE_ENABLE);

    out32(base + CM_PER_DCAN0_CLKCTRL,CM_PER_DCAN0_CLKCTRL_MODULEMODE_ENABLE);

    while((in32(base + CM_PER_DCAN0_CLKCTRL) &
                         CM_PER_DCAN0_CLKCTRL_MODULEMODE) !=
                         CM_PER_DCAN0_CLKCTRL_MODULEMODE_ENABLE);

    while(!(in32(base + CM_PER_L3S_CLKSTCTRL) &
            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

    while(!(in32(base + CM_PER_L3_CLKSTCTRL) &
            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

    while(!(in32(base + CM_PER_OCPWP_L3_CLKSTCTRL) &
           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK)));

    while(!(in32(base + CM_PER_L4LS_CLKSTCTRL) &
           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK |
            CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_CAN_CLK)));
    munmap_device_io(base,CM_PRCM_SIZE);
}


void DCANMsgRAMInit(int uint)
{
	uintptr_t                   base;
//	printf("DCANMsgRAMInit begin \n");
	base = mmap_device_io(AM335X_CTRL_BASE_SIZE, AM335X_CTRL_BASE);
	if(1 == uint)
	{
		out32(base + AM335X_DCAN_RAMINIT,AM335X_DCAN1_RAMINIT_START);
//		printf("wait for  DCAN1 RAM Initialization complete ...%x \n",in32(base + AM335X_DCAN_RAMINIT));
		while((in32(base + AM335X_DCAN_RAMINIT)&AM335X_DCAN1_RAMINIT_DONE)!=AM335X_DCAN1_RAMINIT_DONE)
			nanospin_ns(5000);//wait until DCAN1 RAM Initialization complete
	}
	else if(0 == uint)
	{
		out32(base + AM335X_DCAN_RAMINIT,AM335X_DCAN0_RAMINIT_START);
//		printf("wait for  DCAN0 RAM Initialization complete ...%x \n",in32(base + AM335X_DCAN_RAMINIT));
		while((in32(base + AM335X_DCAN_RAMINIT)&AM335X_DCAN0_RAMINIT_DONE)!=AM335X_DCAN0_RAMINIT_DONE)
		{
			//delay(500);//wait until DCAN0 RAM Initialization complete
			//printf(" AM335X_DCAN_RAMINIT...%x \n",in32(base + AM335X_DCAN_RAMINIT));
			;

		}
	}
//	printf("DCANMsgRAMInit finish \n");
	munmap_device_io(base,AM335X_CTRL_BASE_SIZE);

}
#ifdef CLRINTPNDSTAT
void dcan_CommandRegSet(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id)
{


   out32(devinfo->base + AM335X_DCAN_IF1CMD, AM335X_DCAN_IFxCMD_CTRLINTPND | (id & 0xff));

}
#endif
void dcan_txack(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id)
{
#ifdef CANAM335x_DIS
	fprintf(stderr, "canam335x.c/dcan_txack  \n");
#endif
	//dcan_getobj(devinfo, msg, id, 0, 0, 0, 0, 0, 1, 1);
	//dcan_getobj(devinfo, msg, id, 0, 1, 0, 0, 0, 1, 1);
#ifdef OPTCANTX

	dcan_getobj(devinfo, msg, id, 0, 0, 1, 0, 0, 1, 1);
#else
	dcan_getobj(devinfo, msg, id, 0, 1, 0, 0, 0, 1, 1);
#endif
#ifdef CLRINTPNDSTAT
	dcan_setobj(devinfo, msg, id, 0, 0, 1, 0, 0, 0, 0);	//Clear the IntPnd bit of DCAN_IFMCTL register
	dcan_CommandRegSet(devinfo, msg, id);
#endif
}
void dcan_start_tx(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id)
{
#ifdef CANAM335x_DIS
	fprintf(stderr, "canam335x.c/dcan_start_tx  \n");
#endif

	dcan_setobj(devinfo, msg, id, 0,1,1,1,1,1,1);

}
/* Transmit a CAN message from the specified mailbox */
void can_am335x_tx(CANDEV_AM335X *dev, canmsg_t *txmsg)
{
    CANDEV_AM335X_INFO     *devinfo = dev->devinfo;
    int                     mbxid = dev->mbxid;
    // Access the data as a uint32_t array
    uint32_t                *val32 = (uint32_t *)txmsg->cmsg.dat;
#ifdef CANAM335x_DIS
	fprintf(stderr, "canam335x.c/can_am335x_tx  \n");
#endif
    // Copy message data into transmit mailbox
    devinfo->canmsg[mbxid].canmdl = val32[0];
    devinfo->canmsg[mbxid].canmdh = val32[1];
    //printf("canmdl=0x%x \n",devinfo->canmsg[mbxid].canmdl);
    //printf("canmdh=0x%x \n",devinfo->canmsg[mbxid].canmdh);
    if(devinfo->iflags & INFO_FLAGS_ENDIAN_SWAP)
    {
        // Convert from Little Endian to Big Endian since data is transmitted MSB
        ENDIAN_SWAP32(&devinfo->canmsg[mbxid].canmdl);
        ENDIAN_SWAP32(&devinfo->canmsg[mbxid].canmdh);
        //printf("ENDIAN_SWAP32 canmdl=0x%x \n",devinfo->canmsg[mbxid].canmdl);
        //printf("ENDIAN_SWAP32 canmdh=0x%x \n",devinfo->canmsg[mbxid].canmdh);
    }

    // Transmit message
	out32(devinfo->base + AM335X_DCAN_IF1DATA, devinfo->canmsg[mbxid].canmdl);
	out32(devinfo->base + AM335X_DCAN_IF1DATB, devinfo->canmsg[mbxid].canmdh);
	devinfo->canmsg[mbxid].dcanmsg.DLC = txmsg->cmsg.len;
	dcan_start_tx(devinfo, &devinfo->canmsg[mbxid], mbxid);
}
void print_bitrate(CANDEV_AM335X_INIT  *devinit)
{
	int tq = ((devinit->br_brp+1)*1000000) / (devinit->clk/1000);	// in ns
	int bittime = (1) + (devinit->br_tseg1+1) + (devinit->br_tseg2+1);
	devinit->bitrate = devinit->clk/((devinit->br_brp+1)*bittime);
	fprintf(stderr, "CAN unit%d: bitrate = %d Hz; tq=%d ns; bit time = %d*tq\n", devinit->cinit.can_unit, devinit->bitrate, tq, bittime);
}

int device_init(int port, unsigned int bitrate)
{
    int                     numcan = 0;
    int						unit_chosen=0;
    // Set default options
#ifdef CAN_DIS
	fprintf(stderr, "driver.c/device_init  \n");
#endif
    CANDEV_AM335X_INIT     devinit =
    {
        {   CANDEV_TYPE_RX,             /* devtype */
            0,                          /* can_unit - set this later */
            0,                          /* dev_unit - set this later*/
            100,                        /* msgnum */
            AM335X_DCANMCTL_DLC_BYTE8,	/* datalen */
        },
        AM335X_CAN1_REG_BASE,			/* DCAN port registers base address for AM335x */
        AM335X_CAN_CLK_24MHZ,			/* clk */
        0,                              /* bitrate */
        AM335X_CAN_BITRATE_BRP_100K_DEFAULT - AM335X_CANBTC_DECREMENT, /* br_brp */
        AM335X_CAN_BITRATE_SJW_DEFAULT      - AM335X_CANBTC_DECREMENT, /* br_sjw */
        AM335X_CAN_BITRATE_TSEG1_DEFAULT    - AM335X_CANBTC_DECREMENT, /* br_tseg1 */
        AM335X_CAN_BITRATE_TSEG2_DEFAULT    - AM335X_CANBTC_DECREMENT, /* br_tseg2 */
        AM335X_DCAN0_INT0,				/* irqmsg for AM3517 board */
        AM335X_DCAN0_INT1,				/* irqsys for AM3517 board */
        AM335X_DCAN0_PARITY,			/* irqerr for AM3517 board */
        (INIT_FLAGS_MDRIVER_INIT),      /* flags */
        AM335X_CAN_RX_NUM,		/* numrx */
        0x100C0000,                     /* midrx */
        0x100C0000,                     /* midtx */
        0x0,                            /* timestamp */
    };
    printf("Rx mail box num %d \n", AM335X_CAN_RX_NUM);
    AM335X_CANDEV[port].devinfo=(void *) _smalloc(sizeof(*AM335X_CANDEV[port].devinfo));
	AM335X_CANDEV[port].devinfo->devlist = (void *) _smalloc(sizeof(CANDEV_AM335X) * AM335X_CAN_NUM_MAILBOX);
	AM335X_CANDEV[port].devinfo->canmsg = (void *) _smalloc(AM335X_CAN_NUM_MAILBOX*sizeof(CAN_MSG_OBJ));
	AM335X_CANDEV[port].txmbfullcount = 0;
    // Process command line options and create associated devices



//	switch( bitrate)
//	{
//		case 5000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_5K_DEFAULT;
//			break;
//		case 10000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_10K_DEFAULT;
//			break;
//		case 20000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_20K_DEFAULT;
//			break;
//		case 25000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_25K_DEFAULT;
//			break;
//		case 50000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_50K_DEFAULT;
//			break;
//		case 100000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_100K_DEFAULT;
//			break;
//		case 125000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_125K_DEFAULT;
//			break;
//		case 250000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_250K_DEFAULT;
//			break;
//		case 500000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_500K_DEFAULT;
//			break;
//		case 1000000:
//			devinit.br_brp = AM335X_CAN_BITRATE_BRP_1M_DEFAULT;
//			break;
//		default:
//			return -1;
//	}
    devinit.br_brp =bitrate -   AM335X_CANBTC_DECREMENT;	//1M sundh:20121127
    //DCANModuleClkConfig();//sundh:add 121119

    //if(AM335X_CANDEV_Parameter.CAN_Intr_Attach_Num>0)
    {
    	//fprintf(stderr, "CAN_Intr_Attach_Num %d\n",AM335X_CANDEV_Parameter.CAN_Intr_Attach_Num);
    	DCANMsgRAMInit(port);	//sundh:add 121119
    }
    devinit.flags |= INIT_FLAGS_EXTENDED_MID;	//exten
    devinit.flags |= INIT_FLAGS_MSGDATA_LSB;
#ifdef DISDAR
    devinit.flags |= INIT_FLAGS_DISAUTORTRNSMSSN;
#endif

    if(port==0)
    {

    	devinit.port   = AM335X_CAN0_REG_BASE;
		devinit.irqmsg = AM335X_DCAN0_INT0;		// DCAN0 dcan_intr0_intr_pend
		devinit.irqsys = AM335X_DCAN0_INT1;		// DCAN0 dcan_intr1_intr_pend
		devinit.irqerr = AM335X_DCAN0_PARITY;	// DCAN0 dcan_uerr_intr_pend
		devinit.cinit.can_unit = 0;
//		GPIO0ModuleClkConfig();
    }
    else if(port==1)
    {

    	devinit.port   = AM335X_CAN1_REG_BASE;
		devinit.irqmsg = AM335X_DCAN1_INT0;		// DCAN1 dcan_intr0_intr_pend
		devinit.irqsys = AM335X_DCAN1_INT1;		// DCAN1 dcan_intr1_intr_pend
		devinit.irqerr = AM335X_DCAN1_PARITY;	// DCAN1 dcan_uerr_intr_pend
		devinit.cinit.can_unit = 1;
    }

    print_bitrate(&devinit);

    // Create the CAN device
	create_device(&devinit);
	AM335X_CANDEV[port].devinfo->canmsg[0].arb=0;

	// Reset unit number for next device
	devinit.cinit.can_unit = 0;
	unit_chosen = 0;
	numcan++;
	return 0;
}
#define NOTMALLOC
void create_device(CANDEV_AM335X_INIT *devinit)
{
    CANDEV_AM335X_INFO     *devinfo;
    CANDEV_AM335X          *devlist;

    int port=0;
    if(devinit->port==AM335X_CAN0_REG_BASE)
     	port=0;
     else
     	port=1;
    // Allocate device info

    devinfo = (void *)AM335X_CANDEV[port].devinfo;
    devlist = (void *)AM335X_CANDEV[port].devinfo->devlist;
    devinfo->canmsg = (void *)AM335X_CANDEV[port].devinfo->canmsg;

    memset(devlist, 0, sizeof(CANDEV_AM335X) * AM335X_CAN_NUM_MAILBOX);




	//fprintf(stderr, "CAN_Intr_Attach_Num %d\n",AM335X_CANDEV_Parameter.CAN_Intr_Attach_Num);
	devinfo->base = mmap_device_io(AM335X_CAN_REG_SIZE, devinit->port);


    if(devinfo->base == MAP_DEVICE_FAILED)
    {
        perror("Can't map device I/O");
        exit(EXIT_FAILURE);
    }


    // Map device message memory
	memset(devinfo->canmsg, 0, AM335X_CAN_NUM_MAILBOX*sizeof(CAN_MSG_OBJ));


    // Setup device info
    devinfo->devlist = devlist;
    // Setup the RX and TX mailbox sizes
    devinfo->numrx = devinit->numrx;
    devinfo->numtx = AM335X_CAN_NUM_MAILBOX - devinit->numrx;

    // Initialize flags
    if(devinit->flags & INIT_FLAGS_RX_FULL_MSG)
        devinfo->iflags |= INFO_FLAGS_RX_FULL_MSG;
    if(!(devinit->flags & INIT_FLAGS_MSGDATA_LSB))
        devinfo->iflags |= INFO_FLAGS_ENDIAN_SWAP;
    AM335X_CANDEV[port].devinfo->iflags=devinfo->iflags;

    can_init_hw(devinfo, devinit);
#ifdef DEBUG_DRVR
    can_print_reg(devinfo);
#endif
	can_init_mailbox(devinfo, devinit);
	can_init_intr(devinfo, devinit);

#ifdef DEBUG_DRVR
	can_am335x_debug(devinfo);
#endif
}
am335x_time_t get_am335x_time(void)
{
	struct timespec		timsp;		// For nanosecond timestamp
	am335x_time_t		rawtime;
#ifdef CANAM335x_DIS
	fprintf(stderr, "canam335x.c/get_am335x_time  \n");
#endif
	if (clock_gettime(CLOCK_REALTIME, &timsp) == 0)
	{
    	rawtime = timsp.tv_sec * 1000 + timsp.tv_nsec/10000000;
    }
	else
	{
		rawtime = 0x00badbad;
	}
	return rawtime;
}
/* Initialize CAN device registers */
void can_init_hw(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit)
{
	uint32_t	val;
	uint32_t	brp;
	uint32_t	brpe;

	devinfo->userstamp = 0;
	devinfo->calibrate = 0;
#ifdef CAN_DIS
	fprintf(stderr, "canam335x.c/can_init_hw  \n");
#endif
	///////////////add by sundh :Reset the DCAN module//////////////
#if 1
	val  = in32(devinfo->base + AM335X_DCAN_CTL);
	val |= AM335X_DCAN_CTL_Init;
	out32(devinfo->base + AM335X_DCAN_CTL, val);
	while(!(in32(devinfo->base + AM335X_DCAN_CTL) & AM335X_DCAN_CTL_Init));
	val  = in32(devinfo->base + AM335X_DCAN_CTL);
	val |= AM335X_DCAN_CTL_SWR;
	out32(devinfo->base + AM335X_DCAN_CTL, val);
	while((in32(devinfo->base + AM335X_DCAN_CTL) & AM335X_DCAN_CTL_SWR));
#endif
	/////////////////////////////////////////////////
	// Configuration of CAN Bit Timing
	// The CAN module must be in initialization mode to configure the CAN bit timing.
\
	//reset Can 20130927
	// Step 1: Enter initialization mode by setting the Init (Initialization) bit in the CAN control register.
	val  = in32(devinfo->base + AM335X_DCAN_CTL);
	val |= AM335X_DCAN_CTL_Init;
	out32(devinfo->base + AM335X_DCAN_CTL, val);



	// Step 2: Set the Configure Change Enable (CCE) bit in the CAN control register.
	val |= AM335X_DCAN_CTL_CCE;
	out32(devinfo->base + AM335X_DCAN_CTL, val);

	// Step 3: Wait for the Init bit to get set. This would make sure that the module has entered Initialization mode.
	val  = in32(devinfo->base + AM335X_DCAN_CTL);
	while (!(val & AM335X_DCAN_CTL_Init))
	{
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
	}

	// Step 4: Write the bit timing values into the bit timing register.
    // Program bitrate
	brp  = devinit->br_brp &  AM335X_CANBTC_BRP_MASK;
	brpe = devinit->br_brp >> AM335X_CANBTC_BRP_MASK_SHIFT;
    out32(devinfo->base + AM335X_DCAN_BTR, (brp               << AM335X_DCAN_BTR_BRP_SHIFT  ) |
    									   (brpe              << AM335X_CANBTC_BRPE_SHIFT   ) |
                                           (devinit->br_sjw   << AM335X_DCAN_BTR_SJW_SHIFT  ) |
                                           (devinit->br_tseg1 << AM335X_DCAN_BTR_TSeg1_SHIFT) |
                                           (devinit->br_tseg2 << AM335X_DCAN_BTR_TSeg2_SHIFT)   );

	// Step 5: Clear the CCE and Init bit.
	val &= ~AM335X_DCAN_CTL_Init;
	val &= ~AM335X_DCAN_CTL_CCE;
	out32(devinfo->base + AM335X_DCAN_CTL, val);

	// Step 6: Wait for the Init bit to clear. This would ensure that the module has come out of initialization mode.
	val  = in32(devinfo->base + AM335X_DCAN_CTL);
	while (val & AM335X_DCAN_CTL_Init)
	{
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
	}

    // Enable self-test/loop-back for testing
    if (devinit->flags & INIT_FLAGS_LOOPBACK)
    {
    	// For all test modes, the test bit in CAN control register needs
    	// to be set to one. If test bit is set, the RDA, EXL, Tx1, Tx0,
    	// LBack and Silent bits are writable.
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
		val |= AM335X_DCAN_CTL_Test;
        out32(devinfo->base + AM335X_DCAN_CTL, val);
		val  = in32(devinfo->base + AM335X_DCAN_TEST);
		val |=  AM335X_DCAN_TEST_LBACK;
		val &= ~AM335X_DCAN_TEST_EXL;
        out32(devinfo->base + AM335X_DCAN_TEST, val);
    }
    else
    if (devinit->flags & INIT_FLAGS_EXTLOOPBACK)
    {
    	// For all test modes, the test bit in CAN control register needs
    	// to be set to one. If test bit is set, the RDA, EXL, Tx1, Tx0,
    	// LBack and Silent bits are writable.
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
		val |= AM335X_DCAN_CTL_Test;
        out32(devinfo->base + AM335X_DCAN_CTL, val);
		val  = in32(devinfo->base + AM335X_DCAN_TEST);
		val |=  AM335X_DCAN_TEST_EXL;
		val &= ~AM335X_DCAN_TEST_LBACK;
        out32(devinfo->base + AM335X_DCAN_TEST, val);
    }
    else
    {
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
		val &= ~AM335X_DCAN_CTL_Test;
        out32(devinfo->base + AM335X_DCAN_CTL, val);
    }

    // Enable auto bus on
    if(devinit->flags & INIT_FLAGS_AUTOBUS)
    {
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
		val |= AM335X_DCAN_CTL_ABO;
        out32(devinfo->base + AM335X_DCAN_CTL, val);
    }
#ifdef DISDAR
    if(devinit->flags & INIT_FLAGS_DISAUTORTRNSMSSN)
	{
		val  = in32(devinfo->base + AM335X_DCAN_CTL);
		val |= AM335X_DCAN_CTL_DAR;
		out32(devinfo->base + AM335X_DCAN_CTL, val);
	}


#endif
    // Set initial value for Local Network Time
    if(devinit->flags & INIT_FLAGS_TIMESTAMP)
    {
    	devinfo->userstamp = devinit->timestamp;
    	devinfo->calibrate = get_am335x_time();
    }

    // Clear interrupt registers
	in32(devinfo->base + AM335X_DCAN_ES_EOI);
}
/* Initialize CAN mailboxes in device memory */
void can_init_mailbox(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit)
{
    int         i;
    int         counter = 0;
    uint32_t	cmd;
#ifdef CAN_DIS
	fprintf(stderr, "canam335x.c/can_init_mailbox  \n");
#endif
    // Configure Receive Mailboxes
    counter = 0;
    for(i = 0; i < devinfo->numrx; i++)
    {
		init_msg_rx(devinfo, devinit, i, counter++);
		dcan_setobj(devinfo, &devinfo->canmsg[i], i, 1,1,1,0,0,0,1);
#ifdef TXDONOTWAIT
		cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		while (cmd & AM335X_DCAN_IFxCMD_BUSY)
		{

			cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		}
#endif
    }

    // Configure Transmit Mailboxes
    counter = 0;
    for(i = devinfo->numrx; i < AM335X_CAN_NUM_MAILBOX; i++)
    {
		init_msg_tx(devinfo, devinit, i, counter++);
		dcan_setobj(devinfo, &devinfo->canmsg[i], i, 1,1,1,0,0,0,1);
#ifdef TXDONOTWAIT
		cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		while (cmd & AM335X_DCAN_IFxCMD_BUSY)
		{

			cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		}
#endif
    }
}
/* Initialize CAN device registers */
//#define INTC_ILR(n)  (0x100 + ((n) * 0x04))

#define INTC_ILR(n)  		(0x100 + ((n) * 0x04))
#define INTC_ILR_PRIORITY   (0x000001FC)
#define INTC_ILR_PRIORITY_SHIFT  (0x00000002u)
/* To route an interrupt to IRQ */
#define AINTC_HOSTINT_ROUTE_IRQ                (0)

#define INTC_MIR_CLEAR(n) 	(0x88 + ((n) * 0x20))
#define INTC_SIR_IRQ 		0x40
#define INTC_SIR_FIQ 		0x44
#define INTC_ISR_SET0 		0x90


void can_init_intr(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit)
{
	uint32_t	val;
	SIGEV_INTR_INIT(&event_can);
	devinfo->iidmsg = InterruptAttach(devinit->irqmsg, can_intr, devinfo, 0, 0);
	if(devinfo->iidmsg == -1)
	{
		perror("InterruptAttach irqmsg");
		exit(EXIT_FAILURE);
	}

	// Attach interrupt handler for system interrupts
	devinfo->iidsys = InterruptAttach(devinit->irqsys, can_intr, devinfo, 0, 0);
	if(devinfo->iidsys == -1)
	{
		perror("InterruptAttach irqsys");
		exit(EXIT_FAILURE);
	}

	// Attach interrupt handler for system interrupts
	devinfo->iiderr = InterruptAttach(devinit->irqerr, can_intr, devinfo, 0, 0);
	if(devinfo->iidsys == -1)
	{
		perror("InterruptAttach irqsys");
		exit(EXIT_FAILURE);
	}


    // Enable all system/error interrupts to be generated on DCAN interrupt lines
    // Note - must do this AFTER calling InterruptAttach since mini-driver clears OMAP5905_CANGIM
    // on the MDRIVER_INTR_ATTACH state.
	val  = in32(devinfo->base + AM335X_DCAN_CTL);
	val |= AM335X_DCAN_CTL_IE0;		// Interrupt line 0 enable
	val |= AM335X_DCAN_CTL_IE1;		// Interrupt line 1 enable
	//val |= AM335X_DCAN_CTL_SIE;		// Status change interrupt enable 20130122 屏蔽来解决CAN冗余测试时出现的问题
	val |= AM335X_DCAN_CTL_EIE;		// Error interrupt enable
	out32(devinfo->base + AM335X_DCAN_CTL, val);
#if 1
	uintptr_t				init_crtl;
	init_crtl = mmap_device_io(AM335X_INTC_SIZE,AM335X_INTC_BASE);//映射中断控制器
	out32(init_crtl+INTC_ILR(devinit->irqmsg),((3<<INTC_ILR_PRIORITY_SHIFT)&INTC_ILR_PRIORITY)|AINTC_HOSTINT_ROUTE_IRQ);//设置优先级
	out32(init_crtl+INTC_ILR(devinit->irqsys),((1<<INTC_ILR_PRIORITY_SHIFT)&INTC_ILR_PRIORITY)|AINTC_HOSTINT_ROUTE_IRQ);//设置优先级
	out32(init_crtl+INTC_ILR(devinit->irqerr),((2<<INTC_ILR_PRIORITY_SHIFT)&INTC_ILR_PRIORITY)|AINTC_HOSTINT_ROUTE_IRQ);//设置优先级
	munmap_device_io(init_crtl, AM335X_INTC_SIZE);
#endif

}
//#define TXT
void dcan_setobj(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id, int domask, int doarb, int doctrl, \
		int dodata, int dodatb, int startxfer, int dodma)
{
	volatile uint32_t	cmd;
#ifndef TXDONOTWAIT
	int count=0;
#endif
	cmd = AM335X_DCAN_IFxCMD_WR;
#ifdef CAN_DIS
	fprintf(stderr, "canam335x.c/dcan_setobj id %x,%d,%d,%d,%d,%d,%d,%d  \n",id,domask,doarb,doctrl,dodata,dodatb,startxfer,dodma);
#endif
	if (domask)
	{
		uint32_t	mask;

		/* IF1/IF2 Mask Registers */
		mask = (msg->dcanmsg.mxtd << AM335X_DCAN_IFxMSK_MXTD_SHIFT) |
			   (msg->dcanmsg.mdir << AM335X_DCAN_IFxMSK_MDIR_SHIFT) |
			   (msg->dcanmsg.msk  << AM335X_DCAN_IFxMSK_MASK_SHIFT);
		out32(devinfo->base + AM335X_DCAN_IF1MSK, mask);
		cmd |= AM335X_DCAN_IFxCMD_MASK;
	}

	if (doarb)
	{
		uint32_t	arb;

		/* IF1/IF2 Arbitration Registers */
		arb  = (msg->dcanmsg.MsgVal << AM335X_DCAN_IFxARB_MSGVAL_SHIFT ) |
			   (msg->dcanmsg.Xtd    << AM335X_DCAN_IFxARB_XTD_SHIFT    ) |
			   (msg->dcanmsg.Dir    << AM335X_DCAN_IFxARB_DIR_SHIFT    ) |
		       (msg->dcanmsg.ID     << AM335X_DCAN_IFxARB_MSK_SHIFT_STD) ;
		out32(devinfo->base + AM335X_DCAN_IF1ARB, arb);

		cmd |= AM335X_DCAN_IFxCMD_ARB;
	}

	if (doctrl)
	{
		uint32_t	ctrl;

		/* IF1/IF2 Message Control Registers */
		ctrl = in32(devinfo->base + AM335X_DCAN_IF1MCTL);
		ctrl &= ~0x0f;
		ctrl |= (msg->dcanmsg.NewDat << AM335X_DCAN_IFxMCTL_NEWDAT_SHIFT) |
			   (msg->dcanmsg.MsgLst << AM335X_DCAN_IFxMCTL_MSGLST_SHIFT) |
			   (msg->dcanmsg.IntPnd << AM335X_DCAN_IFxMCTL_INTPND_SHIFT) |
			   (msg->dcanmsg.UMask  << AM335X_DCAN_IFxMCTL_UMASK_SHIFT ) |
			   (msg->dcanmsg.TxIE   << AM335X_DCAN_IFxMCTL_TXIE_SHIFT  ) |
			   (msg->dcanmsg.RxIE   << AM335X_DCAN_IFxMCTL_RXIE_SHIFT  ) |
			   (msg->dcanmsg.RmtEn  << AM335X_DCAN_IFxMCTL_RMTEN_SHIFT ) |
			   (msg->dcanmsg.TxRqst << AM335X_DCAN_IFxMCTL_TXRQST_SHIFT) |
			   (msg->dcanmsg.EoB    << AM335X_DCAN_IFxMCTL_EOB_SHIFT   ) |
			   (msg->dcanmsg.DLC    << AM335X_DCAN_IFxMCTL_DLC_SHIFT   );
		out32(devinfo->base + AM335X_DCAN_IF1MCTL, ctrl);

		cmd |= AM335X_DCAN_IFxCMD_CTRL;
	}

	if (dodata)
	{
		out32(devinfo->base + AM335X_DCAN_IF1DATA, msg->canmdl);
		cmd |= AM335X_DCAN_IFxCMD_DATAA;
	}

	if (dodatb)
	{
		out32(devinfo->base + AM335X_DCAN_IF1DATB, msg->canmdh);
		cmd |= AM335X_DCAN_IFxCMD_DATAB;
	}

	if (startxfer)
	{
		cmd |= AM335X_DCAN_IFxCMD_TXRNDT;
	}

	if (dodma)
	{
		cmd |= AM335X_DCAN_IFxCMD_DMAACT;
	}

	/* IF1/IF2 Message Control Command */
	cmd |= ((id+1) << AM335X_DCAN_IFxCMD_MSGNO_SHIFT);
	out32(devinfo->base + AM335X_DCAN_IF1CMD, cmd);
	cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
	//count=0;
	//while ((cmd & AM335X_DCAN_IFxCMD_BUSY)&&(count<100))
#ifndef TXDONOTWAIT
	while (cmd & AM335X_DCAN_IFxCMD_BUSY)
	{
		count++;
		cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
	}
#endif


}
#define CANMID_DEFAULT              0x00100000
void init_msg_rx(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit, int id, int mxid)
{
	CAN_MSG_OBJ *msg = &devinfo->canmsg[id];

	/* IF1/IF2 Mask Registers */
	msg->dcanmsg.mxtd   = 0;
	msg->dcanmsg.mdir   = 0;
	msg->dcanmsg.msk    = 0;
#ifdef CAN_DIS
	fprintf(stderr, "canam335x.c/init_msg_rx  \n");
#endif
	/* IF1/IF2 Arbitration Registers */

	if (devinit->flags & INIT_FLAGS_EXTENDED_MID)
	{
		msg->dcanmsg.Xtd  = 1;	/* The 29-bit (extended?Identifier is used for this message object. */
		msg->dcanmsg.mxtd = 1;	/* The extended identifier bit (IDE) is used for acceptance filtering. */
	}
	else
	{
		msg->dcanmsg.Xtd  = 0;	/* The 11-bit (standard?Identifier is used for this message object. */
		msg->dcanmsg.mxtd = 0;	/* The extended identifier bit (IDE) has no effect on the acceptance filtering. */
	}
	msg->dcanmsg.Dir    = 0;	/* Direction = receive */
//	msg->dcanmsg.ID     = devinit->midrx + CANMID_DEFAULT * mxid;
	msg->dcanmsg.ID		= 0;

	/* IF1/IF2 Message Control Registers */
	msg->dcanmsg.NewDat = 0;
	msg->dcanmsg.MsgLst = 0;
	msg->dcanmsg.IntPnd = 0;
	msg->dcanmsg.UMask  = 1;
	msg->dcanmsg.TxIE   = 0;
	msg->dcanmsg.RxIE   = 1;
	msg->dcanmsg.RmtEn  = 0;
	msg->dcanmsg.TxRqst = 0;
	msg->dcanmsg.EoB    = 1;
	msg->dcanmsg.DLC    = devinit->cinit.datalen;
	msg->dcanmsg.MsgVal = 1;




}
#ifdef DISDAR
int Is_NewDat(CANDEV_AM335X_INFO *devinfo)
{
	return(in32(devinfo->base + AM335X_DCAN_IF1MCTL) & (1 << AM335X_DCAN_IFxMCTL_NEWDAT_SHIFT) );
}
#endif

void init_msg_tx(CANDEV_AM335X_INFO *devinfo, CANDEV_AM335X_INIT *devinit, int id, int mxid)
{
	CAN_MSG_OBJ *msg = &devinfo->canmsg[id];

	/* IF1/IF2 Mask Registers */
	msg->dcanmsg.mxtd   = 0;
	msg->dcanmsg.mdir   = 0;
	msg->dcanmsg.msk    = 0;
#ifdef CAN_DIS
	fprintf(stderr, "canam335x.c/init_msg_tx  \n");
#endif
	/* IF1/IF2 Arbitration Registers */
	msg->dcanmsg.MsgVal = 1;
	if (devinit->flags & INIT_FLAGS_EXTENDED_MID)
	{
		msg->dcanmsg.Xtd  = 1;	/* The 29-bit (extended?Identifier is used for this message object. */
		msg->dcanmsg.mxtd = 1;	/* The extended identifier bit (IDE) is used for acceptance filtering. */
	}
	else
	{
		msg->dcanmsg.Xtd  = 0;	/* The 11-bit (standard?Identifier is used for this message object. */
		msg->dcanmsg.mxtd = 0;	/* The extended identifier bit (IDE) has no effect on the acceptance filtering. */
	}
	msg->dcanmsg.Dir    = 1;	/* Direction = transmit: */
	msg->dcanmsg.ID     = devinit->midrx + CANMID_DEFAULT * mxid;

	/* IF1/IF2 Message Control Registers */

	msg->dcanmsg.NewDat = 0;
	msg->dcanmsg.MsgLst = 0;
	msg->dcanmsg.IntPnd = 0;
	msg->dcanmsg.UMask  = 0;
	msg->dcanmsg.TxIE   = 1;
	msg->dcanmsg.RxIE   = 0;
//	msg->dcanmsg.RmtEn  = 1;
	msg->dcanmsg.TxRqst = 0;
	msg->dcanmsg.EoB    = 1;
	msg->dcanmsg.DLC    = devinit->cinit.datalen;
#ifdef OPTCANTX
	//为了避免对实时性的影响，还是屏蔽掉了

	//把发送Message Object的前半部分配置成一个队列

	//if(id > (AM335X_CAN_NUM_MAILBOX-3))
	//每8个邮箱配置成一个队列
	/*
	if(id > 8 && ((id ) %8 ==0))
	{
		msg->dcanmsg.EoB    = 1;
		msg->dcanmsg.TxIE   = 1;
	}

	else
	{
		msg->dcanmsg.EoB    = 0;
		msg->dcanmsg.TxIE   = 0;
	}
	*/

#endif
}
void dcan_getobj(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id, int domask, int doarb, int doctrl, int dodata, int dodatb, int eoi, int dodma)
{
	uint32_t cmd = 0;
#ifdef CAN_DIS
	fprintf(stderr, "canam335x.c/dcan_getobj id %x,%d,%d,%d,%d,%d,%d,%d  \n",id,domask,doarb,doctrl,dodata,dodatb,eoi,dodma);
#endif
	/* IF1/IF2 Message Control Command */
	if (domask)	cmd |= AM335X_DCAN_IFxCMD_MASK;
	if (doarb)  cmd |= AM335X_DCAN_IFxCMD_ARB;
	if (doctrl) cmd |= AM335X_DCAN_IFxCMD_CTRL;
	if (dodata) cmd |= AM335X_DCAN_IFxCMD_DATAA;
	if (dodatb) cmd |= AM335X_DCAN_IFxCMD_DATAB;
	if (dodma)  cmd |= AM335X_DCAN_IFxCMD_DMAACT;
	if (eoi)    cmd |= AM335X_DCAN_IFxCMD_CTRLINTPND | AM335X_DCAN_IFxCMD_TXRNDT;
	cmd |= ((id+1) << AM335X_DCAN_IFxCMD_MSGNO_SHIFT);

	out32(devinfo->base + AM335X_DCAN_IF1CMD, cmd);

	if (domask) {msg->mask = 0;msg->mask   = in32(devinfo->base + AM335X_DCAN_IF1MSK);}
	if (doarb)  {msg->arb = 0;msg->arb    = in32(devinfo->base + AM335X_DCAN_IF1ARB);}
	if (doctrl) {msg->mctrl = 0;msg->mctrl  = in32(devinfo->base + AM335X_DCAN_IF1MCTL);}
	if (dodata) {msg->canmdl = 0;msg->canmdl = in32(devinfo->base + AM335X_DCAN_IF1DATA);}
	if (dodatb) {msg->canmdh = 0;msg->canmdh = in32(devinfo->base + AM335X_DCAN_IF1DATB);}


}
void dcan_rxack(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id)
{
	dcan_getobj(devinfo, msg, id, 1, 1, 1, 1, 1, 1, 1);
#ifdef CLRINTPNDSTAT
	dcan_setobj(devinfo, msg, id, 0, 0, 1, 0, 0, 0, 0);	//Clear the IntPnd bit of DCAN_IFMCTL register
	dcan_CommandRegSet(devinfo, msg, id);
#endif
}
/* CAN Interrupt Handler */

/* Print CAN device registers */
void can_print_reg(CANDEV_AM335X_INFO *devinfo)
{
//	CANDEV_AM335X_INFO *devinfo = AM335X_CANDEV[instance].devinfo;
    fprintf(stderr, "\n************************************************************************************\n");
    fprintf(stderr, "AM335X_DCAN_CTL      = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_CTL));
    fprintf(stderr, "AM335X_DCAN_ES_EOI   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_ES_EOI));
    fprintf(stderr, "AM335X_DCAN_ERRC     = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_ERRC));
    fprintf(stderr, "AM335X_DCAN_BTR      = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_BTR));
    fprintf(stderr, "AM335X_DCAN_INT      = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_INT));
    fprintf(stderr, "AM335X_DCAN_TEST     = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_TEST));
    fprintf(stderr, "AM335X_DCAN_PERR     = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_PERR));
    fprintf(stderr, "AM335X_DCAN_ABOTR    = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_ABOTR));
    fprintf(stderr, "AM335X_DCAN_TXRQ_X   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_TXRQ_X));
    fprintf(stderr, "AM335X_DCAN_TXRQ12   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_TXRQ12));
    fprintf(stderr, "AM335X_DCAN_TXRQ34   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_TXRQ34));
    fprintf(stderr, "AM335X_DCAN_TXRQ56   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_TXRQ56));
    fprintf(stderr, "AM335X_DCAN_TXRQ78   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_TXRQ78));
    fprintf(stderr, "AM335X_DCAN_NWDAT_X  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_NWDAT_X));
    fprintf(stderr, "AM335X_DCAN_NWDAT12  = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_NWDAT12));
    fprintf(stderr, "AM335X_DCAN_NWDAT34  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_NWDAT34));
    fprintf(stderr, "AM335X_DCAN_NWDAT56  = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_NWDAT56));
    fprintf(stderr, "AM335X_DCAN_NWDAT78  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_NWDAT78));
    fprintf(stderr, "AM335X_DCAN_INTPND_X = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_INTPND_X));
    fprintf(stderr, "AM335X_DCAN_INTPND12 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_INTPND12));
    fprintf(stderr, "AM335X_DCAN_INTPND34 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_INTPND34));
    fprintf(stderr, "AM335X_DCAN_INTPND56 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_INTPND56));
    fprintf(stderr, "AM335X_DCAN_INTPND78 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_INTPND78));
    fprintf(stderr, "AM335X_DCAN_MSGVAL_X = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_MSGVAL_X));
    fprintf(stderr, "AM335X_DCAN_MSGVAL12 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_MSGVAL12));
    fprintf(stderr, "AM335X_DCAN_MSGVAL34 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_MSGVAL34));
    fprintf(stderr, "AM335X_DCAN_MSGVAL56 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_MSGVAL56));
    fprintf(stderr, "AM335X_DCAN_MSGVAL78 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_MSGVAL78));
    fprintf(stderr, "AM335X_DCAN_INTMUX12 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_INTMUX12));
    fprintf(stderr, "AM335X_DCAN_INTMUX34 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_INTMUX34));
    fprintf(stderr, "AM335X_DCAN_INTMUX56 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_INTMUX56));
    fprintf(stderr, "AM335X_DCAN_INTMUX78 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_INTMUX78));
    fprintf(stderr, "AM335X_DCAN_IF1CMD   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF1CMD));
    fprintf(stderr, "AM335X_DCAN_IF2CMD   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF2CMD));
    fprintf(stderr, "AM335X_DCAN_IF1MSK   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF1MSK));
    fprintf(stderr, "AM335X_DCAN_IF2MSK   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF2MSK));
    fprintf(stderr, "AM335X_DCAN_IF1ARB   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF1ARB));
    fprintf(stderr, "AM335X_DCAN_IF2ARB   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF2ARB));
    fprintf(stderr, "AM335X_DCAN_IF1MCTL  = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF1MCTL));
    fprintf(stderr, "AM335X_DCAN_IF2MCTL  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF2MCTL));
    fprintf(stderr, "AM335X_DCAN_IF1DATA  = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF1DATA));
    fprintf(stderr, "AM335X_DCAN_IF1DATB  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF1DATB));
    fprintf(stderr, "AM335X_DCAN_IF2DATA  = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF2DATA));
    fprintf(stderr, "AM335X_DCAN_IF2DATB  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF2DATB));
    fprintf(stderr, "AM335X_DCAN_IF3OBS   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF3OBS));
    fprintf(stderr, "AM335X_DCAN_IF3MSK   = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF3MSK));
    fprintf(stderr, "AM335X_DCAN_IF3ARB   = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF3ARB));
    fprintf(stderr, "AM335X_DCAN_IF3MCTL  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF3MCTL));
    fprintf(stderr, "AM335X_DCAN_IF3DATA  = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF3DATA));
    fprintf(stderr, "AM335X_DCAN_IF3DATB  = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF3DATB));
    fprintf(stderr, "AM335X_DCAN_IF3UPD12 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF3UPD12));
    fprintf(stderr, "AM335X_DCAN_IF3UPD56 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF3UPD56));
    fprintf(stderr, "AM335X_DCAN_IF3UPD78 = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_IF3UPD78));
    fprintf(stderr, "AM335X_DCAN_IF3UPD78 = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_IF3UPD78));
    fprintf(stderr, "AM335X_DCAN_TIOC     = 0x%8X  ", in32(devinfo->base + AM335X_DCAN_TIOC));
    fprintf(stderr, "AM335X_DCAN_RIOC     = 0x%8X\n", in32(devinfo->base + AM335X_DCAN_RIOC));
}
/* Print CAN device mailbox memory */
void can_print_mailbox(CANDEV_AM335X_INFO *devinfo)
{
    int         i = 0;
#ifdef CANAM335x_DIS
	fprintf(stderr, "canam335x.c/can_print_mailbox  \n");
#endif
    fprintf(stderr, "RX Mailboxes\n");
    fprintf(stderr, "MB\tMASK\t\tARB\t\tMCTRL\t\tMDL\t\tMDH\n");
    fprintf(stderr, "==================================================================================================\n");
    for(i = 0; i < devinfo->numrx; i++)
    {
    	dcan_getobj(devinfo,&devinfo->canmsg[i],i,1,1,1,1,1,0,1);
    	fprintf(stderr, "RX%d\t0x%8X\t0x%8X\t0x%8X\t0x%8X\t0x%8X\n",i,
                devinfo->canmsg[i].mask, devinfo->canmsg[i].arb,
                devinfo->canmsg[i].mctrl, devinfo->canmsg[i].canmdl,
                devinfo->canmsg[i].canmdh);
    }
    fprintf(stderr, "\nTX Mailboxes\n");
    fprintf(stderr, "MB\tMASK\t\tARB\t\tMCTRL\t\tMDL\t\tMDH\n");
    fprintf(stderr, "==================================================================================================\n");
    for(i = devinfo->numrx; i < AM335X_CAN_NUM_MAILBOX; i++)
    {
    	dcan_getobj(devinfo,&devinfo->canmsg[i],i,1,1,1,1,1,0,1);
        fprintf(stderr, "TX%d\t0x%8X\t0x%8X\t0x%8X\t0x%8X\t0x%8X\n",i,
                devinfo->canmsg[i].mask, devinfo->canmsg[i].arb,
                devinfo->canmsg[i].mctrl, devinfo->canmsg[i].canmdl,
                devinfo->canmsg[i].canmdh);
    }
}
void can_am335x_debug(CANDEV_AM335X_INFO *devinfo)
{
    fprintf(stderr, "\nCAN REG\n");
    can_print_reg(devinfo);
    fprintf(stderr, "\nMailboxes\n");
    can_print_mailbox(devinfo);
}
void dcan_clrobj(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id)
{
	volatile uint32_t	cmd;
	out32(devinfo->base + AM335X_DCAN_IF1MSK , 0);
	out32(devinfo->base + AM335X_DCAN_IF1ARB , 0);
	out32(devinfo->base + AM335X_DCAN_IF1MCTL, 0);
	out32(devinfo->base + AM335X_DCAN_IF1DATA, 0);
	out32(devinfo->base + AM335X_DCAN_IF1DATB, 0);

	/* IF1/IF2 Message Control Command */
	cmd  = AM335X_DCAN_IFxCMD_WR;
	cmd |= AM335X_DCAN_IFxCMD_MASK;
	cmd |= AM335X_DCAN_IFxCMD_ARB;
	cmd |= AM335X_DCAN_IFxCMD_CTRL;
	cmd |= AM335X_DCAN_IFxCMD_DATAA;
	cmd |= AM335X_DCAN_IFxCMD_DATAB;
	cmd |= AM335X_DCAN_IFxCMD_DMAACT;
	cmd |= ((id+1) << AM335X_DCAN_IFxCMD_MSGNO_SHIFT);
	out32(devinfo->base + AM335X_DCAN_IF1CMD, cmd);

	cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
	while (cmd & AM335X_DCAN_IFxCMD_BUSY)
	{
		cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
	}
}
/*
void dcan_setmask(CANDEV_AM335X_INFO *devinfo, CAN_MSG_OBJ *msg, int id)
{
	// If the UMask bit is set to one, the message object's mask bits have to be
	// programmed during initialization of the message object before MsgVal is
	// set to one.
	// Disable message object
	uint32_t cmd;
	dcan_clrobj(devinfo, msg, id);

	// Write mask
	msg->dcanmsg.MsgVal = 0;
	msg->dcanmsg.UMask  = 1;
	dcan_setobj(devinfo, msg, id, 1,1,1,0,0,0,1);
#ifdef TXDONOTWAIT
		cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		while (cmd & AM335X_DCAN_IFxCMD_BUSY)
		{

			cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		}
#endif

	// Enable message object
	msg->dcanmsg.MsgVal = 1;
	dcan_setobj(devinfo, msg, id, 0,1,1,0,0,0,1);
#ifdef TXDONOTWAIT
		cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		while (cmd & AM335X_DCAN_IFxCMD_BUSY)
		{

			cmd = in32(devinfo->base + AM335X_DCAN_IF1CMD);
		}
#endif
}
*/
#if 1
#endif

//CAN中断处理函数
const struct sigevent *can_intr(void *area, int id)
{
	CANDEV_AM335X_INFO       *devinfo = area;
	uint32_t                 errst;
	uint32_t                 err;
	uint8_t					 intid[2];
	uint32_t				 val;
	uint8_t                  mbxid,port;      /* mailbox index */
	uint8_t 				loopflag = 1;
	uint8_t					pos = 0;


	errst= in32(devinfo->base + AM335X_DCAN_ES_EOI);

	AM335X_CANDEV_Parameter.CAN_UINT = 3;
	if(devinfo->base==AM335X_CANDEV[0].devinfo->base)
		AM335X_CANDEV_Parameter.CAN_UINT=0;
	else if(devinfo->base==AM335X_CANDEV[1].devinfo->base)
		AM335X_CANDEV_Parameter.CAN_UINT=1;
	port = AM335X_CANDEV_Parameter.CAN_UINT;
	AM335X_CANDEV_Parameter.Errst_Reg[port]=errst;


	// Check for System and Error Interrupts - log the error and clear the interrupt source.
	if (errst & AM335X_DCAN_ES_EOI_PDA)
	{
		// Application request for setting DCAN to local power-down mode was successful. DCAN is in local power-down mode.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_PDA);
	}
	if (errst & AM335X_DCAN_ES_EOI_WUPND)
	{
		// DCAN has initiated a wake up of the system due to dominant CAN bus while module power down.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_WUPND);
		// This bit will be reset if error and status register is read.
	}
	if (errst & AM335X_DCAN_ES_EOI_PER)
	{
		// The parity check mechanism has detected a parity error in the Message RAM.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_PER);
		// This bit will be reset if error and status register is read.
	}
	if (errst & AM335X_DCAN_ES_EOI_BOFF)
	{
		// The CAN module is in bus-off state.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_BOFF);

	}
	if (errst & AM335X_DCAN_ES_EOI_EWARN)
	{
		// At least one of the error counters has reached the error warning limit of 96.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_EWARN);

	}
	if (errst & AM335X_DCAN_ES_EOI_EPASS)
	{
		// The CAN core is in the error passive state as defined in the CAN Specification.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_EPASS);
	}
	if (errst & AM335X_DCAN_ES_EOI_RXOK)
	{
		// The CAN core is in the error passive state as defined in the CAN Specification.
		//atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_RXOK);	//130319 把这个标志移到下面设置，
		//因为在测试中发现当intid[0]不为0时，也会把这个标志置起，导致CAN数据的错误处理

	}
	if (errst & AM335X_DCAN_ES_EOI_TXOK)
	{
		// The CAN core is in the error passive state as defined in the CAN Specification.
		atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_TXOK);
		// This bit will be reset if error and status register is read.
	}
	if (errst & AM335X_DCAN_ES_EOI_LEC_MASK)
	{
		err = errst & AM335X_DCAN_ES_EOI_LEC_MASK;
		switch (err)
		{
		case AM335X_DCAN_ES_EOI_LEC_STUFF:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_STUFF);
			break;
		case AM335X_DCAN_ES_EOI_LEC_FORM:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_FORM);
			break;
		case AM335X_DCAN_ES_EOI_LEC_ACK:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_ACK);
			break;
		case AM335X_DCAN_ES_EOI_LEC_BIT1:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_BIT1);
			break;
		case AM335X_DCAN_ES_EOI_LEC_BIT0:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_BIT0);
			break;
		case AM335X_DCAN_ES_EOI_LEC_CRC:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_CRC);
			break;
		case AM335X_DCAN_ES_EOI_LEC_EVENT:
			atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_CRC);
			break;
		}
	}

	while(loopflag)
	{
		val = in32(devinfo->base + AM335X_DCAN_INT);

		intid[0] = (val & AM335X_DCAN_INT1_MASK) >> 16;
		intid[1] = (val & AM335X_DCAN_INT0_MASK) & 0xFF;
		//AM335X_CANDEV_Parameter.INT_Reg[port]=val;

		if (intid[1] < 0x8000 && intid[1] > 0 )
		{
			mbxid = intid[1]-1;
#ifndef OPTCANHOP
			AM335X_CANDEV_Parameter.Node_Active_Count[port] = OUT_TIME;
#endif
#ifdef PRI_MBXIDCOUNT
			AM335X_CANDEV_Parameter.rcvintrcount[port] ++;

#endif

			if (mbxid < devinfo->numrx) {
				//receive intreeupt
				uint32_t arb=0;
				uint32_t cmd = 0;

				atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_ERR_RXOK); //130319 把接收中断标志放到这里


					//dcan_rxack(devinfo, &devinfo->canmsg[mbxid], mbxid);
				cmd = 0x7f << 16;
				out32(devinfo->base + AM335X_DCAN_IF1CMD, cmd);
			readNext:
				out32(devinfo->base + AM335X_DCAN_IF1CMD, (cmd |(mbxid + 1)));



				AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl = in32(devinfo->base + AM335X_DCAN_IF1MCTL);



				if(AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl & (1 << AM335X_DCAN_IFxMCTL_NEWDAT_SHIFT))
				{
#ifdef PRI_MBXIDCOUNT
					AM335X_CANDEV_Parameter.rcvdatmbxid[port][mbxid]++;
					if(AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl&(1 << AM335X_DCAN_IFxMCTL_MSGLST_SHIFT))
					{
						uint32_t mctl;
						AM335X_CANDEV_Parameter.msglst[port] ++;
						mctl = AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl;
						mctl &= ~(1 << AM335X_DCAN_IFxMCTL_MSGLST_SHIFT);
						//AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl = mctl;
						//dcan_setobj(devinfo, &AM335X_CANDEV[port].devinfo->canmsg[mbxid], mbxid, 0,0,1,0,0,0,0);
						out32(devinfo->base + AM335X_DCAN_IF1MCTL, mctl);
						out32(devinfo->base + AM335X_DCAN_IF1CMD, ((1 << 23)|(AM335X_DCAN_IFxCMD_CTRL) | (mbxid + 1)));
					}

#endif
					//1404210029 当缓冲区满的时候，不接收数据，并且在出现这种情况时，把当前can控制器的接收邮箱的数据读完
					if( ((AM335X_CANDEV_Parameter.MsgTail[port] + 1)%MAXMSGNUM) != AM335X_CANDEV_Parameter.MsgHead[port])
					{

						AM335X_CANDEV[port].devinfo->canmsg[mbxid].arb = in32(devinfo->base + AM335X_DCAN_IF1ARB);
						AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdl = in32(devinfo->base + AM335X_DCAN_IF1DATA);
						AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdh = in32(devinfo->base + AM335X_DCAN_IF1DATB);
						if( AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdl == 0 || \
								AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdh == 0)
						{
//							Rx_Zero_count ++;
						}
						pos = 0;
//						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]][pos++]=0x88;
						arb=AM335X_CANDEV[port].devinfo->canmsg[mbxid].arb & 0x1fffffff;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].cob_id = arb;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++] = (AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdl&0x000000ff);
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdl&0x0000ff00)>>8;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdl&0x00ff0000)>>16;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdl&0xff000000)>>24;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdh&0x000000ff);
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdh&0x0000ff00)>>8;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdh&0x00ff0000)>>16;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].data[pos++]	=	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].canmdh&0xff000000)>>24;
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].len	= 	(AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl & 0x0f) -4;		//DLC
						//ARB的bit29 DIR可以指示当前的帧是远程帧还是数据帧
						AM335X_CANDEV_Parameter.DCAN_REV_DATA[port][AM335X_CANDEV_Parameter.MsgTail[port]].rtr	= ( AM335X_CANDEV[port].devinfo->canmsg[mbxid].arb >> AM335X_DCAN_IFxARB_DIR_SHIFT) & 1;

						AM335X_CANDEV_Parameter.MsgTail[port] = (AM335X_CANDEV_Parameter.MsgTail[port]+1) % MAXMSGNUM;



					}
					else
					{
						atomic_set(&AM335X_CANDEV[port].devinfo->errstatus, AM335X_DCAN_DATA_OVERFLOW); //1408180004 设置标志
					}

						//
					if(!(AM335X_CANDEV[port].devinfo->canmsg[mbxid].mctrl & (1 << AM335X_DCAN_IFxMCTL_EOB_SHIFT)))
					{
						mbxid ++;
						goto readNext;

					}
					else {
						loopflag =0;	//finish

					}
				}
			}
			else
			{
				//transmit intreeupt
//				Tx_intr_count ++;
				AM335X_CANDEV_Parameter.INT_Reg[port]=val;
				dcan_txack(devinfo, &devinfo->canmsg[mbxid], mbxid);
				AM335X_CANDEV_Parameter.Node_Active_Count[port] = OUT_TIME;
#ifdef OPTCANTX

				//devinfo->canmsg[mbxid].dcanmsg.NewDat = (devinfo->canmsg[mbxid].mctrl & (1 << AM335X_DCAN_IFxMCTL_NEWDAT_SHIFT) == 0)? 0 : 1;

				if(devinfo->canmsg[mbxid].mctrl & (1 << AM335X_DCAN_IFxMCTL_NEWDAT_SHIFT))
					devinfo->canmsg[mbxid].dcanmsg.NewDat = 1;
				else
					devinfo->canmsg[mbxid].dcanmsg.NewDat = 0;
#endif

			}
		}	//if (mbxid < devinfo->numrx)
		else
			loopflag = 0;	//finish
	}



    return (&event_can);
}


//CAN数据发送函数

int Transfer(appMessage_t *sendmsg,unsigned char port)
{
	canmsg_t msg;
	uint32_t id=0;



	static int send_count = 0;
	AM335X_CANDEV[port].mbxid=(AM335X_CAN_RX_NUM);
	//sundh 140325 如果这路CAN链路正常，那么按照正常的规则发送，否则就只向第一个发送邮箱发送数据

	//远程帧只能从接收邮箱发送
	if( sendmsg->rtr)
	{
		AM335X_CANDEV[port].mbxid = 0;
	}

	while(AM335X_CANDEV[port].devinfo->canmsg[AM335X_CANDEV[port].mbxid].dcanmsg.NewDat)
	{
		AM335X_CANDEV[port].mbxid++;
		AM335X_CANDEV[port].mbxid %= AM335X_CAN_NUM_MAILBOX;
		if((AM335X_CANDEV[port].mbxid>(AM335X_CAN_NUM_MAILBOX - 1)))	//最后一个发送邮箱用于应急邮箱
		{
			return EXIT_FAILURE;
		}

	}

	//

	if( AM335X_CANDEV[port].mbxid >= AM335X_CAN_RX_NUM && sendmsg->rtr)
	{
		AM335X_CANDEV[port].mbxid = 0;
	}
	memset(&msg.cmsg.dat, 0, AM335X_DCANMCTL_DLC_BYTE8);
	memcpy(&msg.cmsg.dat, sendmsg->data, sendmsg->len);
	id = sendmsg->cob_id & 0x1fffffff;
	msg.cmsg.len = sendmsg->len + 4;

	AM335X_CANDEV[port].devinfo->canmsg[AM335X_CANDEV[port].mbxid].dcanmsg.ID=id;
	AM335X_CANDEV[port].devinfo->canmsg[AM335X_CANDEV[port].mbxid].dcanmsg.NewDat = 1;
	AM335X_CANDEV[port].devinfo->canmsg[AM335X_CANDEV[port].mbxid].dcanmsg.TxRqst = sendmsg->rtr;
	if( sendmsg->rtr)
	{
		AM335X_CANDEV[port].devinfo->canmsg[AM335X_CANDEV[port].mbxid].dcanmsg.TxRqst = 1;


	}

	can_am335x_tx(&AM335X_CANDEV[port],&msg);

	send_count ++;
	if( send_count > 1000)
		send_count = 1000;

	return EXIT_SUCCESS;


}

int device_destory(int instance)
{
//	uint32_t val = 0;
//	val  = in32(AM335X_CANDEV[instance].devinfo->base + AM335X_DCAN_CTL);
//	val |= AM335X_DCAN_CTL_SWR;
//	out32(AM335X_CANDEV[instance].devinfo->base + AM335X_DCAN_CTL, val);
//	while((in32(AM335X_CANDEV[instance].devinfo->base + AM335X_DCAN_CTL) & AM335X_DCAN_CTL_SWR));

	munmap_device_io( AM335X_CANDEV[instance].devinfo->base, AM335X_CAN_REG_SIZE);
	_sfree(AM335X_CANDEV[instance].devinfo->canmsg, AM335X_CAN_NUM_MAILBOX*sizeof(CAN_MSG_OBJ));
	_sfree(AM335X_CANDEV[instance].devinfo->devlist, sizeof(CANDEV_AM335X) * AM335X_CAN_NUM_MAILBOX);
	_sfree(AM335X_CANDEV[instance].devinfo, sizeof(CANDEV_AM335X_INFO));

	return EXIT_SUCCESS;

}

