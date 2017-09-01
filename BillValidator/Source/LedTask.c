#include "LedTask.h"
//#include <RTL.h> 
#include "Uart.h"
#include "Main.h"
/*-------------------------------------------------------------------------
	MDB_LED  GPIO0_20
-------------------------------------------------------------------------*/
#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
static void MDB_LedOn(void)
{
	GPIO_SetHighLevel(LPC_GPIO0, 20, 1); 	// MDB LED ON
}

static void MDB_LedOff(void)
{
	GPIO_SetLowLevel(LPC_GPIO0, 20, 1);	// MDB LED OFF	
}

static void MDB_LedFlash(void)
{
	if (GPIO_GetPinValue(LPC_GPIO0, 20))
	{
		MDB_LedOff();
	}
	else
	{
		MDB_LedOn();
	}
}
#else
static void MDB_LedOn(void)
{
	GPIO_SetHighLevel(LPC_GPIO0, 9, 1); 	// MDB LED ON
}

static void MDB_LedOff(void)
{
	GPIO_SetLowLevel(LPC_GPIO0, 9, 1);		// MDB LED OFF
}

static void MDB_LedFlash(void)
{
	if (GPIO_GetPinValue(LPC_GPIO0, 9))
	{
		MDB_LedOff();
	}
	else
	{
		MDB_LedOn();
	}
}
#endif

static void MDB_LedFlashLoop(U8 Times)
{
	U8 i;
	
	for (i = 0; i < Times; i++)
	{
		MDB_LedFlash();
		os_dly_wait(5);
	}
}

/*-------------------------------------------------------------------------
	RS232_LED  GPIO0_21		RS232_U1
-------------------------------------------------------------------------*/
#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
static void COM_LedOn(void)
{
	GPIO_SetHighLevel(LPC_GPIO0, 21, 1); 	// COM LED ON
}

static void COM_LedOff(void)
{
	GPIO_SetLowLevel(LPC_GPIO0, 21, 1);	// COM LED OFF
}

static void COM_LedFlash(void)
{
	if (GPIO_GetPinValue(LPC_GPIO0, 21))
	{
		COM_LedOff();
	}
	else
	{
		COM_LedOn();
	}
}

/*-------------------------------------------------------------------------
	VTS_LED  GPIO0_19
-------------------------------------------------------------------------*/
static void VTS_LedOn(void)//to indicate MDB cashless address #1
{
	GPIO_SetHighLevel(LPC_GPIO0, 19, 1); 	// VTS LED ON
}

static void VTS_LedOFF(void)//to indicate MDB cashless address #2
{
	GPIO_SetLowLevel(LPC_GPIO0, 19, 1); 	// VTS LED OFF
}

static void VTS_LedFlash(void)
{
	if (GPIO_GetPinValue(LPC_GPIO0, 19))
	{
		VTS_LedOFF();
	}
	else
	{
		VTS_LedOn();
	}
}

/*-------------------------------------------------------------------------
	RS232_U0_LED  GPIO0_22   RS232_U0
-------------------------------------------------------------------------*/
static void FUJI_LedOn(void)//to indicate MDB decimal place
{
	GPIO_SetHighLevel(LPC_GPIO0, 22, 1); 	// FUJI LED ON
}

static void FUJI_LedOFF(void)//to indicate MDB decimal place
{
	GPIO_SetLowLevel(LPC_GPIO0, 22, 1); 	// VTS LED OFF
}

static void FUJI_LedFlash(void)
{
	if (GPIO_GetPinValue(LPC_GPIO0, 22))
	{
		FUJI_LedOFF();
	}
	else
	{
		FUJI_LedOn();
	}
}

static void FUJI_LedFlashLoop(U8 Times)
{
	U8 i;
	
	for (i = 0; i < Times; i++)
	{
		FUJI_LedFlash();
		os_dly_wait(50);
	}
}
#else//76032v1
static void COM_LedOn(void)
{
	GPIO_SetHighLevel(LPC_GPIO0, 7, 1); 	// COM LED ON
}

static void COM_LedOff(void)
{
	GPIO_SetLowLevel(LPC_GPIO0, 7, 1);	// COM LED OFF
}

static void COM_LedFlash(void)
{
	if (GPIO_GetPinValue(LPC_GPIO0, 7))
	{
		COM_LedOff();
	}
	else
	{
		COM_LedOn();
	}
}
#endif

static void COM_LedFlashLoop(U8 Times)
{
	U8 i;
	
	for (i = 0; i < Times; i++)
	{
		COM_LedFlash();
		os_dly_wait(20);
	}
}

/*
Led task*********************************************************************
VTS_LED  			GPIO0_19   
MDB_LED  			GPIO0_20   
RS232_U1_LED  GPIO0_21		RS232_U1   
RS232_U0_LED  GPIO0_22   	RS232_U0
*/
__task void tsk_Led(void)	//
{
	while(1)
	{
		os_dly_wait(10); // 100ms

		if (u8_MdbAddr_Mode)//LED on when MDB cashless addr#1
		{
			//VTS_LedOn();
			GPIO_SetLowLevel(LPC_GPIO0, 19, 1);
		}
		else//DMB cashless address #2
		{
			//os_dly_wait(20);	// 200ms
			VTS_LedFlash();//LED Flash  when MDB cashless addr#2
		}

		if (u8_DecimalPoint_Mode)//Added to indicate MDB decimal point 2 by flash 2times DJI@20160523
		{
			FUJI_LedFlashLoop(4);			
		}
		else//Decimal point 1
		{
			FUJI_LedFlash();
			os_dly_wait(10);
		}	

		if ((u8_ComHost_State < COMHOST_ENABLE)&&(u8_MdbSlave_State < WS_ENABLED))
		{			
			COM_LedFlashLoop(9);
			COM_LedFlash();
			os_dly_wait(10);
			MDB_LedFlashLoop(9);
			MDB_LedFlash();
		}
		else if (u8_ComHost_State < COMHOST_ENABLE)//&&(u8_MdbSlave_State >= WS_ENABLED))
		{
			COM_LedFlashLoop(9);
			COM_LedFlash();
			MDB_LedFlash();
		}
		else if (u8_MdbSlave_State < WS_ENABLED)//&& (u8_ComHost_State >= COMHOST_ENABLE))
		{
			MDB_LedFlashLoop(9);
			MDB_LedFlash();
			COM_LedFlash();
		}
		else
		{
			os_dly_wait(100);	// 1000ms
			COM_LedFlash();
			MDB_LedFlash();			
			//if (!u8_MdbAddr_Mode)//Added to indicate MDB addr #2 by DJI@20160523				
		}
	}	
}
