
/*
*****************************************************************************
**
**      Project     : KAWE_76032
**
**      Component   : MAIN (LPC1225)
**
**      Modulename  : PUBLIC
**
**      Filename    : Main.h
**
**      Abstract    : This is the device driver header file for the RXT real time chip 
**                    module.
**
**      Compiler    : KEIL C compiler
**
**      Date        : 2012-11-16 16:31:15
**
**      License no. : 9538-154-209-9667     Frank Tsang
**
**      Warning     : This file has been automatically generated.
**                    Do not edit this file if you intend to regenerate it.
**
**      This device driver was created by IAR MakeApp version 
**      4.10C (Atmel megaAVR: 4.00B) for the Atmel megaAVR series of
**      microcontrollers.
**
**      (c)Copyright 2003 IAR Systems.
**      Your rights to this file are explained in the IAR MakeApp 
**      License Agreement. All other rights reserved.
**
*****************************************************************************
*/

/*
**===========================================================================
**  1       GENERAL
**  1.1     Revisions
**
**  Please read the IAR MakeApp for Atmel megaAVR readme file 
**  V1.0
**
**===========================================================================
*/

/*
**===========================================================================
**  1.2     References
** 
**  No   Identification          Name or Description
**  ==   ===================     ================================
**  
** 
**===========================================================================
*/

/*
**===========================================================================
**  1.3     Re-definition guard
**===========================================================================
*/

/*--- Avoid including this file more than once ---*/
#ifndef _IS_INCLUDED_MAIN_H_
#define _IS_INCLUDED_MAIN_H_

/*
**===========================================================================
**  2.      INCLUDE FILES
**  2.1     Standard include files
**===========================================================================
*/
#include <lpc12xx.h>
#include <core_cm0.h>
#include <lpc_types.h>
#include <stdlib.h>
#include <stdio.h>
	
#include <RTL.h> 
//#include "RT_Agent.h"
//#include "RTX_Libs.h"
	

/*
**===========================================================================
**  2.2     Application include files
**===========================================================================
*/


//#include "typedef.h"


/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Global constants
**===========================================================================
*/

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer
//   <i> Default: 10000  (10ms)
#ifndef OS_TICK
 #define OS_TICK        10000
#endif


#define OS_CLK_MS		(OS_TICK / 1000)				// OS tick in milliseconds

#ifndef IKAWE_MPAY //design special for mobile payment(wechat and alipay)
#define IKAWE_MPAY 
#define MPAY_DEPOSIT  0x00C80000
#endif

//#ifndef KAWE_TEST
//#define KAWE_TEST
//#endif

#ifndef NSL_MPAY//added for NSL and Baixue GFV vending machine
#define NSL_MPAY
#endif

#ifndef MDBLEVEL3 //To extend cashless card reader always idle option by DJI@20170810
#define MDBLEVEL3
#endif


#ifndef HW76032V3//added by DJI for 76032V3 HW with LPC1225FBD48 321 @20160406
#define HW76032V3
#endif

#ifndef WRONG76032V3 //for testing UART1 for MDB slave, and UART0 for RS232 host
#define WRONG76032V3
#endif

#define	WAIT_RSP_TIME	        1000	// 11s
#define WAIT_QRCODE_RSP_TIME	500//5s//2s//1.2s added for QRCODE timeout by DJI 20160218
#define WAIT_PAY_TIME	    	2500//1600// 190s added for QRCODE timeout by DJI 20160218, longer than DWIN timeout
#define WAIT_SELECTION_TIME     1500//1200 //wait 60s after people click ikawe screen with credit by DJI @20160303
#define WAIT_DEDUCT_RSP_TIME	1000	// 20s added for QRCODE timeout by DJI 20160218
#define WAIT_CANCEL_RSP_TIME    100 //1s timeout when during timetout state


/*
**===========================================================================
**  3.2     Global macros
**===========================================================================
*/

#define MSEC(x)      	(U32)(x / (OS_TICK / 1000))
#define SEC(x)       	(U32)(x * MSEC(1000))
#define MINUTE(x)    	(U32)(x * SEC(60))









/*
**===========================================================================
**  3.3     Global type definitions
**===========================================================================
*/

#if 0
typedef struct 
{
  	OS_TID 		task;
  	OS_ID  		timer;
  	T_TIK  		initial_period;
  	T_TIK  		cycle_time;
  	T_SEM   	sema;
	U16			updateFlag;
  	U32    		timVal;
  	U16    		info;	//ndx;     the info parameter in os_tmr_call()
} T_TMR;		// type of system timer
#endif

typedef union	
{
	U16	Word;
	struct 
	{
		U8	Low;
		U8	High;
	}Bytes;
}T16;		 
 
typedef __packed struct 
{
  	U8 		Id;
  	char	Manufacturer[3];
  	char	SerialNumber[12];
  	char	ModelNumber[12];
  	U8		SoftwareVersion[2];
#ifdef MDBLEVEL3	
	U8		OptionalBits[4];	//only for level 3
#endif	
 } T_PERIPHERAL;		// type of system timer

typedef __packed struct 
{
	U8 	Order;
	U8 	Length;
	U8	CardState;
	U32 CardId;
	U32 Funds;
}T_CARD_STATE;

typedef __packed struct
{
	U8	Code;
	U8	Length;
}T_CARD_POLL;

typedef __packed struct
{
	U8	Code;
	U8	Length;
	U16	ItemPrice;
	U16 ItemNumber;
	U32 CardId;
}T_CARD_DEBIT;

typedef __packed struct
{
	U8	Code;
	U8	Length;
	U8	Result;
	//U16	ItemPrice;
	U16 ItemNumber;
	U32 CardId;
}T_VEND_RESULT;

#ifdef NSL_MPAY//MDB Level 2
typedef __packed struct
{
	U8	Code;
	U16 Funds;
	U32 CardId;
	U8	PayType;
	U16	PayData;
	//U16 Language;
	//U16 CurrencyCode;
	//U8	Option;
}T_BEGINSESSION;

#else//MDB Level 1

typedef __packed struct
{
	U8	Code;
	U16 Funds;
}T_BEGINSESSION;

#endif

typedef __packed struct
{
	U8 	Code;
	U16	Amount;	// U16
}T_VEND_APPROVED;

typedef __packed struct
{
	U8	Code;
	U8	Length;
}T_cancel_RESULT;


/*
**===========================================================================
**  3.4     Global variables (defined in some implementation file)
**===========================================================================
*/
#ifdef	_IS_INCLUDED_MAIN_C_
	#define	PUBLIC
#else
	#define	PUBLIC	extern
#endif


//PUBLIC		T_TMR 			TmrArray[SUM_TMR];

//PUBLIC 	volatile	T_SEM	SemaSysTmr;
PUBLIC	volatile	OS_TID	tid_MdbSlave;
PUBLIC	volatile	OS_TID	tid_VtsSlave;
PUBLIC	volatile	OS_TID	tid_ComSlave;

PUBLIC	volatile	OS_TID	tid_ComHost;
PUBLIC	volatile	OS_TID	tid_MdbHost;

PUBLIC	volatile	U32	u32_CardId;
PUBLIC	volatile	U32	u32_Funds;
PUBLIC	volatile	BOOL bl_CardAction;
PUBLIC	volatile	BOOL bl_Flag_Vend_Request;
#ifdef IKAWE_MPAY
PUBLIC	volatile	BOOL bl_Flag_QRcode_Request;
PUBLIC  volatile    BOOL bl_Flag_MPAY_Done;//Mobile pay done by DJI 20160226
#endif

PUBLIC	volatile	T16	t16_ItemPrice;
PUBLIC	volatile	T16	t16_ItemNuber;
//PUBLIC	volatile	BOOL bl_Flag_Debit_Result;


PUBLIC	OS_SEM	sem_MdbSlave_Rx;
//PUBLIC	OS_SEM	sem_MdbSlave_Tx;

PUBLIC	OS_SEM	sem_VtsSlave_Rx;
PUBLIC	OS_SEM	sem_VtsSlave_Tx;

PUBLIC	OS_SEM	sem_ComSlave_Rx;
PUBLIC	OS_SEM	sem_ComSlave_Tx;

PUBLIC	OS_SEM	sem_MdbHost_Rx;
PUBLIC	OS_SEM	sem_MdbHost_Tx;

PUBLIC	OS_SEM	sem_ComHost_Rx;
//PUBLIC	OS_SEM	sem_ComHost_Tx;

PUBLIC	volatile U32 u32_Err_Code;

PUBLIC	T_PERIPHERAL	p_MyPeripheral;
PUBLIC	T_BEGINSESSION	p_BeginSession;
PUBLIC	T_CARD_POLL		p_CardPoll;
PUBLIC	T_CARD_DEBIT	p_CardDebit;
PUBLIC	T_VEND_RESULT	p_VendResult;
PUBLIC	T_VEND_APPROVED	p_VendApproved;
PUBLIC  T_cancel_RESULT p_CancelResult;

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (defined in some implementation file)
**===========================================================================
*/

//PUBLIC		void 		rtx_Tmr_Init(void);





#undef	PUBLIC

#endif /* Match the re-definition guard */


/*
**===========================================================================
** END OF FILE
**===========================================================================
*/ 
