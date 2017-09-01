/*
*****************************************************************************
**
**      Project     : KAWE_76032
**
**      Component   : UART (LPC1225)
**
**      Modulename  : PUBLIC
**
**      Filename    : Uart.c
**
**      Abstract    : This is the uart driver file for the RTX real time chip 
**                    module.
**
**      Compiler    : KEIL C compiler
**
**      Date        : 2014-09-28 16:31:15
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
**===========================================================================
**  
** 
**===========================================================================
*/

/*
**===========================================================================
**  2.      INCLUDE FILES
**  2.1     Standard include files
**===========================================================================
*/

#include "lpc12xx.h"
#include "core_cm0.h"
#include "lpc_types.h"
#include <stdlib.h>
#include <stdio.h>

#include <RTL.h> 
//#include "RT_Agent.h"
#include "Lpc12xx_libcfg_default.h"


/*
**===========================================================================
**  2.2     Application include files
**===========================================================================
*/
#ifndef _IS_INCLUDED_UART_C_
#define _IS_INCLUDED_UART_C_

#include "Uart.h"

#include "Main.h"

#include "Time.h"

#include "string.h"

#endif

/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Internal constants
**===========================================================================
*/


/*
**===========================================================================
**  3.2     Internal macros
**===========================================================================
*/

/*
**===========================================================================
**  3.3     Internal type definitions
**===========================================================================
*/

/*
**===========================================================================
**  3.4     Global variables (declared as 'extern' in some header file)
**===========================================================================
*/


/*
**===========================================================================
**  3.5     Internal function prototypes (defined in Section 5)
**===========================================================================
*/

/*
**===========================================================================
**  3.6     Internal variables
**===========================================================================
*/

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/
void Uart0_Bit9Set(BOOL on)
{
	register U8 u8_temp;
	if (on)
	{
		u8_temp 		= LPC_UART0->LCR & 0xCF;
		LPC_UART0->LCR	= u8_temp | 0x20;
	}
	else
	{
		u8_temp 		= LPC_UART0->LCR & 0xCF;
		LPC_UART0->LCR	= u8_temp | 0x30;
	}
}

BOOL Uart0_Bit9Get(void)
{
	register U8 u8_temp;
	
	u8_temp = LPC_UART0->LCR & 0x30;

	if (u8_temp == 0x20)
	{
		return 1;
	}
	else
	{
		return 0;
	}
		
}

void Uart1_Bit9Set(BOOL on)
{
	register U8 u8_temp;
	if (on)
	{
		u8_temp 		= LPC_UART1->LCR & 0xCF;
		LPC_UART1->LCR	= u8_temp | 0x20;
	}
	else
	{
		u8_temp 		= LPC_UART1->LCR & 0xCF;
		LPC_UART1->LCR	= u8_temp | 0x30;
	}
}

BOOL Uart1_Bit9Get(void)
{
	register U8 u8_temp;
	
	u8_temp = LPC_UART1->LCR & 0x30;

	if (u8_temp == 0x20)
	{
		return 1;
	}
	else
	{
		return 0;
	}
		
}

// Set the first char in buffer without FCC
void Uart0_BuffSetChar(U8 u8_Data)
{
	Uart0TxBuff.InPos	= 0;
	Uart0TxBuff.OutPos	= 0;
	Uart0TxBuff.OutLen	= 0;
	Uart0TxBuff.Fcc		= u8_Data;

	Uart0TxBuff.Data[Uart0TxBuff.InPos++] = u8_Data;
}

// add a char to buffer with a new FCC
void Uart0_BuffAddChar(U8 u8_Data)
{

	if (Uart0TxBuff.InPos == 0)
	{
		Uart0TxBuff.Fcc = 0;
	}

	Uart0TxBuff.Fcc += u8_Data;

	Uart0TxBuff.Data[Uart0TxBuff.InPos++] = u8_Data;

	//Uart0TxBuff.OutLen = Uart0TxBuff.InPos + 1;	// include Fcc byte

}

// send the char in buffer
void Uart0_BuffFill(void)
{
	U8 i;

	// Fill the UART0 FIFO
	for (i = 0; i < 16; i++)
    {
		if(Uart0TxBuff.InPos == Uart0TxBuff.OutPos) 		
		{
			break;
		}
		else
		{
			LPC_UART0->THR = Uart0TxBuff.Data[Uart0TxBuff.OutPos++];				
		}
    }
}

// add a char to buffer with a new FCC
void Uart1_BuffAddChar(U8 u8_Data)
{

	if (Uart1TxBuff.InPos == 0)
	{
		Uart1TxBuff.Fcc = 0;
	}

	Uart1TxBuff.Fcc += u8_Data;

	Uart1TxBuff.Data[Uart1TxBuff.InPos++] = u8_Data;

	//Uart1TxBuff.OutLen = Uart1TxBuff.InPos + 1;	// include Fcc byte

}

// send the char in buffer
void Uart1_BuffFill(void)
{
	U8 i;

	// Fill the UART1 FIFO
	for (i = 0; i < 16; i++)
    {
		if(Uart1TxBuff.InPos == Uart1TxBuff.OutPos) 		
		{
			break;
		}
		else
		{
			LPC_UART1->THR = Uart1TxBuff.Data[Uart1TxBuff.OutPos++];				
		}
    }
}

void MdbSlave_Response(U8 u8_Code)
{
	//Uart0_BuffSetChar(u8_Code);
	//Uart0_BuffSend();
	Uart0_Bit9Set(1);
	
	LPC_UART0->THR 	= u8_Code;

	// Open the TX interrupt
	UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
}

void MdbSlave_Responses(void * p8_Code, U8 u8_Length)
{
	U8 i;

	for (i = 0; i < u8_Length; i++)
	{
		Uart0_BuffAddChar(*((U8 *)p8_Code + i));
	}
	
	Uart0_Bit9Set(0);

	Uart0_BuffFill();

	// Open the TX interrupt
	UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
}

void MdbSlave_ReResponse(void)
{
	Uart0TxBuff.InPos = Uart0TxBuff.OutLen;
	
	Uart0_Bit9Set(0);

	Uart0_BuffFill();

	// Open the TX interrupt
	UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
}

void ComHost_Send(U8 * p8_Code, U8 u8_Length)
{
	U8 i;

	//Uart1TxBuff.Fcc 	= 0;
	Uart1TxBuff.InPos	= 0;
	Uart1TxBuff.OutPos	= 0;

	for (i = 0; i < u8_Length; i++)
	{
		Uart1_BuffAddChar(*(p8_Code + i));
	}

	Uart1_BuffAddChar(Uart1TxBuff.Fcc);

	u8_Uart1_State = UART_TX;
	
	Uart1_BuffFill();

	// Open the TX interrupt
	UART_ConfigInts(LPC_UART1, UART_INTCFG_THRE, ENABLE);	
}

/*-------------------------------------------------------------------------
function: MdbSlave_Rx
function: As a MDB slave, the mode bit High means the ADDRESS byte
-------------------------------------------------------------------------*/
//void UART0_Int_Tx(void)
void MdbSlave_Tx(void)
{

	//clear_interrupt();		// acknowlege the interrupt
	
	if (Uart0_Bit9Get() == 1)	// last byte is over
	{
		Uart0TxBuff.OutLen	= Uart0TxBuff.OutPos;
		Uart0TxBuff.OutPos	= 0;
		Uart0TxBuff.InPos	= 0;
		
		Uart0_Bit9Set(0);

		// Disable THRE interrupt	 
		UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, DISABLE);	
		
		//isr_sem_send(&sem_MdbSlave_Tx);		// transmit is over
	}
	else if (Uart0TxBuff.InPos == Uart0TxBuff.OutPos)	// all data is over
	{
		while (( UART_GetLineStatus(LPC_UART0) & UART_LS_TX_EMPTY_ALL ) == 0); // wait the previous byte shift over
		
		Uart0_Bit9Set(1);

		LPC_UART0->THR = Uart0TxBuff.Fcc;
	}
	else	// continue to fill the FIFO
	{
		Uart0_BuffFill();
	}
	
}

void ComHost_Tx(void)
{
	//clear_interrupt();		// acknowlege the interrupt
	
	if (Uart1TxBuff.InPos == Uart1TxBuff.OutPos)	// all data is over
	{
		Uart1TxBuff.OutLen	= Uart1TxBuff.OutPos;
		Uart1TxBuff.OutPos	= 0;
		Uart1TxBuff.InPos	= 0;
		
		// Disable THRE interrupt	 
		UART_ConfigInts(LPC_UART1, UART_INTCFG_THRE, DISABLE);	

		u8_Uart1_State = UART_IDLE;
	}
	else	// continue to fill the FIFO
	{
		Uart1_BuffFill();
	}
	
}

/*-------------------------------------------------------------------------
function: MdbSlave_Rx
function: As a MDB slave, the mode bit High means the ADDRESS byte
-------------------------------------------------------------------------*/
void ComHost_Rx(U8 u8_RBR)	// , BOOL bl_Bit9)
{
//	static U8 u8_DataLen = 0;
	
	//Timer_Stop_ComHost();

	if (u8_Uart1_State == UART_IDLE)
	{
		//if ((u8_RBR == 0x24) || (u8_RBR == 0x26) || (u8_RBR == 0x27))
		if ((u8_RBR == 0x24) || (u8_RBR == 0x26) || (u8_RBR == 0x28) || (u8_RBR == 0x30))//Modified by DJI @20160509
		{
			u8_Uart1_State = UART_RX;
			
			Uart1RxBuff.Index = 0;
			Uart1RxBuff.Data[Uart1RxBuff.Index++] = u8_RBR;
			//Uart1RxBuff.Fcc	= u8_RBR;
		}
	}
	else if (u8_Uart1_State == UART_RX)
	{
		Uart1RxBuff.Data[Uart1RxBuff.Index++] = u8_RBR;
		//Uart1RxBuff.Fcc += u8_RBR;
		if (Uart1RxBuff.Index == Uart1RxBuff.Data[1])
		{
			u8_Uart1_State = UART_WAIT;
			
			//isr_evt_set(EVT_RX_READY, tid_ComHost);
			isr_sem_send(&sem_ComHost_Rx);
		}
	}

}


/*-------------------------------------------------------------------------
function: MdbSlave_Rx
function: As a MDB slave, the mode bit High means the ADDRESS byte
-------------------------------------------------------------------------*/
void MdbSlave_Rx(U8 u8_RBR, BOOL bl_Bit9)
{
	static U8 u8_AddrCode 	= 0;
	//static U8 u8_MyLevel	= 1;
	static U8 u8_DataLen 	= 0;
	static BOOL bl_Expansion= FALSE;
	
	
	if (u8_Uart0_State == UART_IDLE)
	{
		if (bl_Bit9)// receive address byte
		{
			u8_Uart0_State = UART_RX;
			
			//u8_MdbSlave_RxIndex = 0;
			Uart0RxBuff.Index 	= 0;
			//Uart0RxBuff.Fcc		= u8_RBR;

			Uart0RxBuff.Data[Uart0RxBuff.Index++] = u8_RBR;
			u8_AddrCode = u8_RBR;
			

			if (u8_MdbAddr_Mode)
			{
				switch (u8_AddrCode)
				{
					case 0x10: // reset
						u8_DataLen = 2;
						break;
					case 0x11:	// setup
						u8_DataLen = 7;
						break;
					case 0x12:	// Poll
						u8_DataLen = 2;
						break;
					//case 0x13:
					//	u8_DataLen = 7;
					//	break;
					case 0x14:
						u8_DataLen = 3;
						break;
					//case 0x17:
					//	u8_DataLen = 32;
					//	break;
					default:
						u8_DataLen = 0;
						break;
					}
			}
			else //Cashless addr #2 when SEL0 = 1 by default by DJI@20160506
			{
				switch (u8_AddrCode)
				{
					case 0x60: // reset
						u8_DataLen = 2;
						break;
					case 0x61:	// setup
						u8_DataLen = 7;
						break;
					case 0x62:	// Poll
						u8_DataLen = 2;
						break;
					//case 0x63:
					//	u8_DataLen = 7;
					//	break;
					case 0x64:
						u8_DataLen = 3;
						break;
					//case 0x67:
					//	u8_DataLen = 32;
					//	break;
					default:
						u8_DataLen = 0;
						break;
				}
			}
			//Timer_Start_MdbSlave(200); // 2ms interbyte time limit
			//Timer_Stop_MdbSlave();
			//Timer_Start_MdbSlave(150); // 6ms interbyte time limit just for test
		}
		else
		{
			switch (u8_AddrCode)
			{
			case  ACK:
				break;
			case  NAK:
				break;
			case  RET:
				//MdbSlave_ReResponse();
				break;
			}
		}
	}
	else if (u8_Uart0_State == UART_RX)
	{
		if (bl_Bit9 == 0)
		{
			if (Uart0RxBuff.Index + 1 < MDBSLAVE_BUFFSIZE)
			{
				Uart0RxBuff.Data[Uart0RxBuff.Index++] = u8_RBR;

				if (u8_MdbAddr_Mode)
				{
					if ((u8_AddrCode == 0x11) && (Uart0RxBuff.Index == 3) && (Uart0RxBuff.Data[1] == 0x00))
					{
						//u8_MyLevel = u8_RBR;
					}

					if ((u8_AddrCode == 0x11) && (Uart0RxBuff.Data[1] == 0x01) && (Uart0RxBuff.Index == 7))
					{
						if (!MdbSlave_CheckFcc())
						{
							u8_DataLen 		= 13;
							bl_Expansion 	= TRUE;
						}
					}

					if ((u8_AddrCode == 0x13) && (Uart0RxBuff.Index == 2))
					{
						switch (u8_RBR)
						{
						case 0x00:
							if (bl_Expansion)
							{
								u8_DataLen = 9;
							}
							else
							{
								u8_DataLen = 7;
							}
							break;
						case 0x01:
							u8_DataLen = 3;
							break;
						case 0x02:
							u8_DataLen = 5;
							break;
						case 0x03:	// failure
							u8_DataLen = 3;
							break;
						case 0x04:
							u8_DataLen = 3;
							break;
						default:
							u8_DataLen = 0;
							break;
						}
					}

					if ((u8_AddrCode == 0x14) && (Uart0RxBuff.Index == 2))
					{
						switch (u8_RBR)
						{
						case 0x00:
						case 0x01:
						case 0x02:
							u8_DataLen = 3;
							break;
						default:
							u8_DataLen = 0;
							break;
						}
					}

					if ((u8_AddrCode == 0x17) && (Uart0RxBuff.Index == 2) && (u8_RBR == 0x00))
					{
						u8_DataLen = 32;
					}
				}
				else//Cashless addr #2 when SEL0 = 1 by default by DJI@20160506
				{
					if ((u8_AddrCode == 0x61) && (Uart0RxBuff.Index == 3) && (Uart0RxBuff.Data[1] == 0x00))
					{
						//u8_MyLevel = u8_RBR;
					}

					if ((u8_AddrCode == 0x61) && (Uart0RxBuff.Data[1] == 0x01) && (Uart0RxBuff.Index == 7))
					{
						if (!MdbSlave_CheckFcc())
						{
							u8_DataLen 		= 13;
							bl_Expansion 	= TRUE;
						}
					}

					if ((u8_AddrCode == 0x63) && (Uart0RxBuff.Index == 2))
					{
						switch (u8_RBR)
						{
						case 0x00:
							if (bl_Expansion)
							{
								u8_DataLen = 9;
							}
							else
							{
								u8_DataLen = 7;
							}
							break;
						case 0x01:
							u8_DataLen = 3;
							break;
						case 0x02:
							u8_DataLen = 5;
							break;
						case 0x03:	// failure
							u8_DataLen = 3;
							break;
						case 0x04:
							u8_DataLen = 3;
							break;
						default:
							u8_DataLen = 0;
							break;
						}
					}

					if ((u8_AddrCode == 0x64) && (Uart0RxBuff.Index == 2))
					{
						switch (u8_RBR)
						{
						case 0x00:
						case 0x01:
						case 0x02:
							u8_DataLen = 3;
							break;
						default:
							u8_DataLen = 0;
							break;
						}
					}

					if ((u8_AddrCode == 0x67) && (Uart0RxBuff.Index == 2) && (u8_RBR == 0x00))
					{
						u8_DataLen = 32;
					}
				}

				if (u8_DataLen == 0)
				{
					Timer_Start_MdbSlave(200); // 2ms interbyte time limit
				}
				else if (u8_DataLen == Uart0RxBuff.Index)
				{
					Timer_Stop_MdbSlave();
					
					UART_ConfigInts(LPC_UART0, UART_INTCFG_RBR, DISABLE);	
					UART_ConfigInts(LPC_UART0, UART_INTCFG_RLS, DISABLE);	
					
					isr_sem_send(&sem_MdbSlave_Rx);
					//isr_evt_set(0x0001, tid_MdbSlave);
				}

			
			}
		}
	}

	//u8_GlobalRBR = u8_RBR;
	//isr_evt_set(0x0001, tid_MdbSlave);
	
}

BOOL MdbSlave_CheckFcc(void)
{
	U8 i;
	U8 Fcc = 0;
	U8 Len;

	Len = Uart0RxBuff.Index - 1;

	if (Len == 0)
	{
		return TRUE;
	}

	for (i = 0; i < Len; i++)
	{
		Fcc += Uart0RxBuff.Data[i];
	}

	if (Uart0RxBuff.Data[Len] == Fcc)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL ComHost_CheckFcc(void)
{
	U8 i;
	U8 Fcc = 0;
	U8 Len;

	Len = Uart1RxBuff.Index - 1;

	if (Len == 0)
	{
		return TRUE;
	}

	for (i = 0; i < Len; i++)
	{
		Fcc += Uart1RxBuff.Data[i];
	}

	if (Uart1RxBuff.Data[Len] == Fcc)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*-------------------------------------------------------------------------
function:
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void UART_IRQHandler_ComHost(void)
{
	uint16_t	u16_IIR;
	uint8_t 	u8_LSR;
	uint8_t 	u8_RBR;
	
	/* Determine the interrupt source */
	u16_IIR = UART_GetINTStatus(LPC_UART1) & 0x0E;

	/* Check line status*/
	u8_LSR	= UART_GetLineStatus(LPC_UART1);


	if (u16_IIR == UART_INTSTAT_RX_LINE_STAT)
	{
		/* Receive Line Status*/

		if (u8_LSR & UART_LS_RX_DATA_READY)
		{
			u8_RBR = LPC_UART1->RBR;	// dummy read

			if (u8_LSR & UART_LS_PARITY_ERROR) // bit9 = 1
			{
				//ComHost_Rx(u8_RBR, 1);
				//isr_evt_set(0x0001,tid_ComHost);
				//isr_sem_send(&sem_ComHost_Rx);
			}
		}
		//else if (u8_LSR & (UART_LS_RX_FIFO_FULL | UART_LS_FRAMING_ERROR | UART_LS_BREAK_INT | UART_LS_RX_FIFO_ERROR))
		//{
			/* Call Call-back function with error input value*/
			//vccs_rxerr_ipt();
		//}
		
	}
	else if ((u16_IIR == UART_INTSTAT_RX_DATA_AVAILABLE ) || (u16_IIR == UART_INTSTAT_RX_TIMEOUT))
	{
		/* Receive Data Available or Character time-out*/
		u8_RBR = LPC_UART1->RBR;

		/* Call Rx call back function*/
		ComHost_Rx(u8_RBR);	// , 0);
		//isr_evt_set(0x0004,tid_ComHost);
		//isr_sem_send(&sem_ComHost_Rx);
	}
	else if (u16_IIR == UART_INTSTAT_TX_READY)
	{
		/* Transmit Holding Empty*/
		/* Call Tx call back function*/
		//UART0_Int_Tx();
		ComHost_Tx();
	}

}

void UART_IRQHandler_MdbSlave(void)
{
	uint16_t	u16_IIR;
	uint8_t 	u8_LSR;
	uint8_t 	u8_RBR;
	
	/* Determine the interrupt source */
	u16_IIR = UART_GetINTStatus(LPC_UART0) & 0x0E;

	/* Check line status*/
	u8_LSR	= UART_GetLineStatus(LPC_UART0);


	if (u16_IIR == UART_INTSTAT_RX_LINE_STAT)
	{
		/* Receive Line Status*/

		if (u8_LSR & UART_LS_RX_DATA_READY)
		{
			u8_RBR = LPC_UART0->RBR;
			//u8_GlobalRBR = u8_RBR;

			if (u8_LSR & UART_LS_PARITY_ERROR) // bit9 = 1
			{
				MdbSlave_Rx(u8_RBR, 1);
				//u16_Counter1++;
				//u8_GlobalRBR += 1;
				//isr_evt_set(0x0001,tid_MdbSlave);
				//isr_sem_send(&sem_MdbSlave_Rx);
			}
		}
		//else if (u8_LSR & (UART_LS_RX_FIFO_FULL | UART_LS_FRAMING_ERROR | UART_LS_BREAK_INT | UART_LS_RX_FIFO_ERROR))
		//{
			/* Call Call-back function with error input value*/
			//vccs_rxerr_ipt();
		//}
		
	}
	else if ((u16_IIR == UART_INTSTAT_RX_DATA_AVAILABLE ) || (u16_IIR == UART_INTSTAT_RX_TIMEOUT))
	{
		/* Receive Data Available or Character time-out*/
		u8_RBR = LPC_UART0->RBR;
		//u8_GlobalRBR = u8_RBR;

		/* Call Rx call back function*/
		MdbSlave_Rx(u8_RBR, 0);
		//u16_Counter3++;
		//isr_evt_set(0x0004,tid_MdbSlave);
		//isr_sem_send(&sem_MdbSlave_Rx);
	}
	else if (u16_IIR == UART_INTSTAT_TX_READY)
	{
		/* Transmit Holding Empty*/
		/* Call Tx call back function*/
		//UART0_Int_Tx();
		MdbSlave_Tx();
	}

}


/*-------------------------------------------------------------------------
function:
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
	UART_IRQHandler_MdbSlave();

#if 0
	switch (u8_WorkMode)
	{
	case WM_SLV_VTS_HST_MDB:
		break;
	case WM_SLV_232_HST_MDB:
		break;
	case WM_SLV_MDB_HST_232:
		UART_IRQHandler_MdbSlave();
		break;
	case WM_SLV_VTS_HST_232:
		break;
	default:
		break;
	}
#endif
}

/*-------------------------------------------------------------------------
function:
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
	UART_IRQHandler_ComHost();
	
#if 0
	switch (u8_WorkMode)
	{
	case WM_SLV_VTS_HST_MDB:
		break;
	case WM_SLV_232_HST_MDB:
		break;
	case WM_SLV_MDB_HST_232:
		UART_IRQHandler_ComHost();
		break;
	case WM_SLV_VTS_HST_232:
		break;
	default:
		break;
	}
#endif
}

void Cashless_SetupConfig_Init(void)
{
	pCashlessSetup.Config		= 0x01;
#ifndef NSL_MPAY	
	pCashlessSetup.Level		= 0x01;		// reader level 1 must for SL
#else	
	pCashlessSetup.Level		= 0x02;//Level2 for NSL by DJI @20160325
#endif
	pCashlessSetup.Country_Hi	= 0x11;
	pCashlessSetup.Country_Lo	= 0x56;
	pCashlessSetup.Scale		= 0x01;
#ifndef NSL_MPAY		
	pCashlessSetup.Decimal		= 0x01;//for Baixue GFV machine;//0x01; For SL China by DJI;
	pCashlessSetup.RespTime		= 0x3C;//58s for iKawe SL  //0x07;// according to NAYAX
#else	
	if (u8_DecimalPoint_Mode)
		pCashlessSetup.Decimal		= 0x02;//For NSLVending machine by DJI @20160325
	else
		pCashlessSetup.Decimal		= 0x01;//Decimal point 1 For Necta machine by DJI @20160523

	pCashlessSetup.RespTime		= 0x3C;//60s for iKawe GFV //0x07;// according to NAYAX
#endif	
	
#ifndef NSL_MPAY			
	pCashlessSetup.Option		= 0x01;		// refunding only,multivend uncapable, no display, vend/cash sale uncapable
#else	
	pCashlessSetup.Option		= 0x00;//0x09;		// refunding not ok,multivend uncapable, no display, vend/cash sale capable
#endif
}

void Cashless_RequestId_Init(void)
{
	p_MyPeripheral.Id = 0x09;
#ifdef NSL_MPAY//added for different vending machine
	strncpy(p_MyPeripheral.Manufacturer, "KWE", 3); //iKawe for other vending machine
	
	strncpy(p_MyPeripheral.SerialNumber, "KW00260114", 12);//Next for TULAN Addr#2 from 0017
	//strncpy(p_MyPeripheral.SerialNumber, "KW00260200", 12);
	//strncpy(p_MyPeripheral.SerialNumber, "KW00260074", 12);//Kawe test sample#1
	
	strncpy(p_MyPeripheral.ModelNumber, "iKAWE 260", 12);
#else		
	strncpy(p_MyPeripheral.Manufacturer, "MCC", 3); //ikawe for Spengler SL coffee vending machine

	strncpy(p_MyPeripheral.SerialNumber, "KW00160200", 12);//ikawe for Tulan
	//strncpy(p_MyPeripheral.SerialNumber, "KW00160130", 12);//ikawe for internal testing
	
	strncpy(p_MyPeripheral.ModelNumber, "iKAWE 160", 12);
#endif
	p_MyPeripheral.SoftwareVersion[0] = 0x04;
	
	p_MyPeripheral.SoftwareVersion[1] = 0x31;
#ifdef MDBLEVEL3
	p_MyPeripheral.OptionalBits[0] = 0x22;//set the always idle option for TW cashless
	p_MyPeripheral.OptionalBits[1] = 0x00;
	p_MyPeripheral.OptionalBits[2] = 0x00;
	p_MyPeripheral.OptionalBits[3] = 0x00;
#endif	
	
}

#ifdef NSL_MPAY
void Cashless_BeginSession_Init(void)
{
	p_BeginSession.Code		= 0x03;
	p_BeginSession.Funds	= 0xD007;//0x07D0;//2000 cent for decimal point 2  	
	p_BeginSession.CardId	= 0xF4010000;//0xFFFFFFFF;//	//0x55aa55aa;
	p_BeginSession.PayType	= 0;
	p_BeginSession.PayData	= 0;
	//p_BeginSession.Language	= (U16)('h' << 8 | 'z');
	//p_BeginSession.CurrencyCode=0x5611;
	//p_BeginSession.Option	= 0x00;
}
#else//Level 1 only has two fields
void Cashless_BeginSession_Init(void)
{
	p_BeginSession.Code		= 0x03;
	p_BeginSession.Funds    = 0x00C8; //20YUAN for decimal point 1 
}
#endif

/*-------------------------------------------------------------------------
function:
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void Uart_Init_VtsSlave(void)
{
	/* UART Configuration structure variable*/
	UART_CFG_Type UARTConfigStruct;
	IOCON_PIO_CFG_Type PIO_mode;


	IOCON_StructInit(&PIO_mode);		// setup VTS Flash LED
	PIO_mode.type = IOCON_PIO_0_8;	
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 8, 1); // p0.8 output

	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE);
	NVIC_DisableIRQ(UART0_IRQn);

	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD0_LOC1;
	IOCON_SetFunc(&PIO_mode);
	PIO_mode.type = IOCON_UART_TXD0_LOC1;
	IOCON_SetFunc(&PIO_mode);	 

	SYS_ResetPeripheral(SYS_PRESETCTRL_UART0_RST, DISABLE);
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE); 
	SYS_SetUART0ClockDiv(1);

	UART_Init(LPC_UART0);

	UART_GetConfig(LPC_UART0, &UARTConfigStruct);
	UARTConfigStruct.baudrate	= 4800;
	UARTConfigStruct.databits	= UART_CFG_DATABIT_8;
	UARTConfigStruct.parity 	= UART_CFG_PARITY_EVEN;
	UARTConfigStruct.stopbits	= UART_CFG_STOPBIT_1;
	UARTConfigStruct.fifolevel	= UART_CFG_FIFOTRG_1;
	UARTConfigStruct.fifodma	= UART_CFG_DMAMODE_DISABLE;
	UARTConfigStruct.txdbreak	= UART_CFG_TXDBREAK_DISABLE;

	/* Initialize UART0 peripheral with given to corresponding parameter*/
	UART_SetConfig(LPC_UART0, &UARTConfigStruct);
	
	UART_ConfigTXD(LPC_UART0, ENABLE);

	UART_ConfigInts(LPC_UART0, UART_INTCFG_RBR, ENABLE);	
	//UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
	UART_ConfigInts(LPC_UART0, UART_INTCFG_RLS, ENABLE);	
	NVIC_EnableIRQ(UART0_IRQn);

	//os_sem_init(&sem_VtsSlave_Rx, 0);
	//os_sem_init(&sem_VtsSlave_Tx, 0);
}


/*-------------------------------------------------------------------------
function: This function works as MDB slave converted to RS232 host(card reader and mobile payment) protocol
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void Uart_Init_ComHost(void)
{
	U8 i;
	/* UART Configuration structure variable*/
	UART_CFG_Type UARTConfigStruct;
	IOCON_PIO_CFG_Type PIO_mode;

	for (i = 0; i < 36; i++)
	{
		Uart1RxBuff.Data[i] = 0;
		Uart1TxBuff.Data[i] = 0;
	}

	Uart1RxBuff.Fcc 	= 0;
	Uart1RxBuff.Index	= 0;

	Uart1TxBuff.Fcc		= 0;
	Uart1TxBuff.InPos	= 0;
	Uart1TxBuff.OutPos	= 0;
	Uart1TxBuff.OutLen	= 0;
	
	IOCON_StructInit(&PIO_mode);		// setup RS232 Flash LED
#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
	PIO_mode.type = IOCON_PIO_0_21;	
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 21, 1); 		// p0.21 output
#else//76032v1
	PIO_mode.type = IOCON_PIO_0_7;	
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 7, 1); 		// p0.7 output
#endif

	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART1, ENABLE);
	NVIC_DisableIRQ(UART1_IRQn);

#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD1_LOC0;
	IOCON_SetFunc(&PIO_mode);
	PIO_mode.type = IOCON_UART_TXD1_LOC0;
	IOCON_SetFunc(&PIO_mode);
#else//76032v1
	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD1_LOC2;
	IOCON_SetFunc(&PIO_mode);
	PIO_mode.type = IOCON_UART_TXD1_LOC2;
	IOCON_SetFunc(&PIO_mode);	 
#endif
	SYS_ResetPeripheral(SYS_PRESETCTRL_UART1_RST,DISABLE);
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART1, ENABLE); 
	SYS_SetUART1ClockDiv(1);

	UART_Init(LPC_UART1);

	UART_GetConfig(LPC_UART1, &UARTConfigStruct);
	UARTConfigStruct.baudrate	= 9600;
	UARTConfigStruct.databits	= UART_CFG_DATABIT_8;
	UARTConfigStruct.parity 	= UART_CFG_PARITY_NONE;
	UARTConfigStruct.stopbits	= UART_CFG_STOPBIT_1;
	UARTConfigStruct.fifolevel	= UART_CFG_FIFOTRG_1;
	UARTConfigStruct.fifodma	= UART_CFG_DMAMODE_DISABLE;
	UARTConfigStruct.txdbreak	= UART_CFG_TXDBREAK_DISABLE;
	
	/* Initialize UART0 peripheral with given to corresponding parameter*/
	UART_SetConfig(LPC_UART1, &UARTConfigStruct);

	UART_ConfigTXD(LPC_UART1, ENABLE);

	UART_ConfigInts(LPC_UART1, UART_INTCFG_RBR, ENABLE);	
	//UART_ConfigInts(LPC_UART1, UART_INTCFG_THRE, ENABLE);	
	//UART_ConfigInts(LPC_UART1, UART_INTCFG_RLS, ENABLE);	
	NVIC_EnableIRQ(UART1_IRQn);

	u8_ComHost_State 	= COMHOST_ENABLE;
	u32_CardId			= 0;
	u32_Funds			= 0;
	bl_CardAction		= FALSE;
	bl_Flag_Vend_Request= FALSE;
//	bl_Flag_Debit_Result= FALSE;
#ifdef IKAWE_MPAY
	bl_Flag_QRcode_Request=FALSE;
	bl_Flag_MPAY_Done=FALSE;
#endif

	//os_sem_init(&sem_ComHost_Rx, 0);
	//os_sem_init(&sem_ComHost_Tx, 0);

	u8_Uart1_State = UART_IDLE;
}


/*-------------------------------------------------------------------------
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void Uart_Init_ComSlave(void)
{
	/* UART Configuration structure variable*/
	UART_CFG_Type UARTConfigStruct;
	IOCON_PIO_CFG_Type PIO_mode;


	IOCON_StructInit(&PIO_mode);		// setup RS232 Flash LED
	PIO_mode.type = IOCON_PIO_0_7;	
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 7, 1); // p0.7 output


	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE);
	NVIC_DisableIRQ(UART0_IRQn);


	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD0_LOC1;
	IOCON_SetFunc(&PIO_mode);
	PIO_mode.type = IOCON_UART_TXD0_LOC1;
	IOCON_SetFunc(&PIO_mode);	 

	SYS_ResetPeripheral(SYS_PRESETCTRL_UART0_RST, DISABLE);
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE); 
	SYS_SetUART0ClockDiv(1);

	UART_Init(LPC_UART0);

	UART_GetConfig(LPC_UART0, &UARTConfigStruct);
	UARTConfigStruct.baudrate	= 9600;
	UARTConfigStruct.databits	= UART_CFG_DATABIT_8;
	UARTConfigStruct.parity 	= UART_CFG_PARITY_NONE;
	UARTConfigStruct.stopbits	= UART_CFG_STOPBIT_1;
	UARTConfigStruct.fifolevel	= UART_CFG_FIFOTRG_1;
	UARTConfigStruct.fifodma	= UART_CFG_DMAMODE_DISABLE;
	UARTConfigStruct.txdbreak	= UART_CFG_TXDBREAK_DISABLE;

	/* Initialize UART0 peripheral with given to corresponding parameter*/
	UART_SetConfig(LPC_UART0, &UARTConfigStruct);
	
	UART_ConfigTXD(LPC_UART0, ENABLE);

	UART_ConfigInts(LPC_UART0, UART_INTCFG_RBR, ENABLE);	
	//UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
	UART_ConfigInts(LPC_UART0, UART_INTCFG_RLS, ENABLE);	
	NVIC_EnableIRQ(UART0_IRQn);
	
	//os_sem_init(&sem_ComSlave_Rx, 0);
	//os_sem_init(&sem_ComSlave_Tx, 0);
}


/*-------------------------------------------------------------------------
function: Converter works as RS232 to MDB host interface,
This function is to control MDB slave device like MDB coin changer or bill reader;
Switch:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/

void Uart_Init_MdbHost(void)
{
	/* UART Configuration structure variable*/
	UART_CFG_Type UARTConfigStruct;
	IOCON_PIO_CFG_Type PIO_mode;


	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_PIO_0_9;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 9, 1); // (MDB Flash Led)p0.9 output


	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_GPIO2, ENABLE);

	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_PIO_2_13;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO2, 13, 1); // (TXD1)p2.13 output
	
	GPIO_SetLowLevel(LPC_GPIO2, 13, 1);
	os_dly_wait(MSEC(250));				/* 200��������, reset the MDB bus */
	GPIO_SetHighLevel(LPC_GPIO2, 13, 1);


	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART1, ENABLE);
	NVIC_DisableIRQ(UART1_IRQn);

	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD1_LOC2;
	IOCON_SetFunc(&PIO_mode);
	PIO_mode.type = IOCON_UART_TXD1_LOC2;
	IOCON_SetFunc(&PIO_mode);	 

	SYS_ResetPeripheral(SYS_PRESETCTRL_UART1_RST,DISABLE);
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART1, ENABLE); 
	SYS_SetUART1ClockDiv(1);

	UART_Init(LPC_UART1);

	UART_GetConfig(LPC_UART1, &UARTConfigStruct);
	UARTConfigStruct.baudrate	= 9600;
	UARTConfigStruct.databits	= UART_CFG_DATABIT_8;
	UARTConfigStruct.parity 	= UART_CFG_PARITY_SPACE;
	UARTConfigStruct.stopbits	= UART_CFG_STOPBIT_1;
	UARTConfigStruct.fifolevel	= UART_CFG_FIFOTRG_1;
	UARTConfigStruct.fifodma	= UART_CFG_DMAMODE_DISABLE;
	UARTConfigStruct.txdbreak	= UART_CFG_TXDBREAK_DISABLE;
	
	/* Initialize UART0 peripheral with given to corresponding parameter*/
	UART_SetConfig(LPC_UART1, &UARTConfigStruct);
	UART_ConfigTXD(LPC_UART1, ENABLE);

	UART_ConfigInts(LPC_UART1, UART_INTCFG_RBR, ENABLE);	
	//UART_ConfigInts(LPC_UART1, UART_INTCFG_THRE, ENABLE);	
	UART_ConfigInts(LPC_UART1, UART_INTCFG_RLS, ENABLE);	
	NVIC_EnableIRQ(UART1_IRQn);
	
	//os_sem_init(&sem_MdbHost_Rx, 0);
	//os_sem_init(&sem_MdbHost_Tx, 0);
}



/*-------------------------------------------------------------------------
function: This function works as MDB slave converted to RS232 host(card reader and mobile payment) protocol
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void Uart_Init_MdbSlave(void)
{
	U8 i;
	/* UART Configuration structure variable*/
	UART_CFG_Type UARTConfigStruct;
	IOCON_PIO_CFG_Type PIO_mode;

	for (i = 0; i < 36; i++)
	{
		Uart0RxBuff.Data[i] = 0;
		Uart0TxBuff.Data[i] = 0;
	}

	Uart0RxBuff.Fcc		= 0;
	Uart0RxBuff.Index 	= 0;

	Uart0TxBuff.Fcc		= 0;
	Uart0TxBuff.InPos	= 0;
	Uart0TxBuff.OutPos 	= 0;
	Uart0TxBuff.OutLen	= 0;
	
	IOCON_StructInit(&PIO_mode);
#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
	PIO_mode.type = IOCON_PIO_0_20;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 20, 1); 	// (MDB Flash Led)p0.20 output
#else//76032_v1
	PIO_mode.type = IOCON_PIO_0_9;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 9, 1); 	// (MDB Flash Led)p0.9 output
#endif


	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE);
	NVIC_DisableIRQ(UART0_IRQn);

#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD0_LOC0;
	PIO_mode.invert = 1<<6;			// invert the input signal
	IOCON_SetFunc(&PIO_mode);
	
	PIO_mode.type = IOCON_UART_TXD0_LOC0;
	IOCON_SetFunc(&PIO_mode);	 
#else//76032v1
	IOCON_StructInit(&PIO_mode);
	PIO_mode.type = IOCON_UART_RXD0_LOC1;
	PIO_mode.invert = 1<<6;			// invert the input signal
	IOCON_SetFunc(&PIO_mode);
	
	PIO_mode.type = IOCON_UART_TXD0_LOC1;
	IOCON_SetFunc(&PIO_mode);	 
#endif

	SYS_ResetPeripheral(SYS_PRESETCTRL_UART0_RST, DISABLE);
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE); 
	SYS_SetUART0ClockDiv(1);

	UART_Init(LPC_UART0);

	UART_GetConfig(LPC_UART0, &UARTConfigStruct);
	UARTConfigStruct.baudrate	= 9600;
	UARTConfigStruct.databits	= UART_CFG_DATABIT_8;
	UARTConfigStruct.parity 	= UART_CFG_PARITY_SPACE;
	UARTConfigStruct.stopbits	= UART_CFG_STOPBIT_1;
	UARTConfigStruct.fifolevel	= UART_CFG_FIFOTRG_1;
	UARTConfigStruct.fifodma	= UART_CFG_DMAMODE_DISABLE;
	UARTConfigStruct.txdbreak	= UART_CFG_TXDBREAK_DISABLE;

	/* Initialize UART0 peripheral with given to corresponding parameter*/
	UART_SetConfig(LPC_UART0, &UARTConfigStruct);
	
	UART_ConfigTXD(LPC_UART0, ENABLE);

	UART_ConfigInts(LPC_UART0, UART_INTCFG_RBR, ENABLE);	
	//UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
	UART_ConfigInts(LPC_UART0, UART_INTCFG_RLS, ENABLE);	

	/* preemption = 0, sub-priority = 5 */
	NVIC_SetPriority(UART0_IRQn, ((0x00 << 3) | 0x05));

	/* Enable interrupt for timer */
	NVIC_EnableIRQ(UART0_IRQn);

	//os_sem_init(&sem_MdbSlave_Rx, 0);
	//os_sem_init(&sem_MdbSlave_Tx, 0);

	u8_Uart0_State = UART_IDLE;
	
}
/*-------------------------------------------------------------------------
Function: 
Description: Choose MDB cashless address for different vending machine
	u8_DecimalPoint_Mode = 0; for MDB decimal point 1
	u8_DecimalPoint_Mode = 1; for MDB decimal point 2
-------------------------------------------------------------------------*/
void MDB_Decimal_Place_Init(void)
{
	IOCON_PIO_CFG_Type PIO_mode;

	u8_DecimalPoint_Mode = 1;//Default is MDB decimal point 2
	
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_GPIO1, ENABLE);
	
	IOCON_StructInit(&PIO_mode);
	//Initial PIO 0.22 as output to drive VTS LED for indicator of MDB decimal place mode
	PIO_mode.type = IOCON_PIO_0_22;	
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 22, 1); 
	GPIO_SetHighLevel(LPC_GPIO0, 22, 1); 	// Decimal LED OFF
	//Initial PIO 1.3 as input for decimal mode
	PIO_mode.type = IOCON_PIO_1_3;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO1, 3, 0); // (SEL0)p1.3 input

	u8_DecimalPoint_Mode = GPIO_GetPinValue(LPC_GPIO1, 3);

}


/*-------------------------------------------------------------------------
	Choose MDB cashless address for different vending machine
	u8_MdbAddr_Mode = 0; for MDB cashless address#2: 0x60
	u8_MdbAddr_Mode = 1; for MDB cashless address#1: 0x10
-------------------------------------------------------------------------*/
void MDB_Address_Mode_Init(void)
{
	IOCON_PIO_CFG_Type PIO_mode;
	//U8 SEL0;

	u8_MdbAddr_Mode = 1;//Default is MDB cashless addrss #1
	
	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_GPIO1, ENABLE);
	
	IOCON_StructInit(&PIO_mode);
	//Initial PIO 0.19 as output to drive VTS LED for indicator of MDB cashless address mode
	PIO_mode.type = IOCON_PIO_0_19;	
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 19, 1); 
	GPIO_SetLowLevel(LPC_GPIO0, 19, 1); 	// VTS LED ON
	//Initial PIO 1.6 as input for address mode
	PIO_mode.type = IOCON_PIO_1_6;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO1, 6, 0); // (2CODE)p1.6 input

	u8_MdbAddr_Mode = GPIO_GetPinValue(LPC_GPIO1, 6);
	//u8_MdbAddr_Mode = 1;//Testing for cashless addr #1
	//u8_MdbAddr_Mode = 0;//Testing for cashless addr #2

}

/*-------------------------------------------------------------------------
function:
function:
SEL1 	SEL0	CHOICE
0	 	0		VTS 	Slave(Uart0)	-	MDB 	Host(Uart1)
0 `	 	1		RS232 	Slave(Uart0)	-	MDB 	Host(Uart1)
1		0		MDB 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
1		1		VTS 	Slave(Uart0)	-	RS232 	Host for card read(Uart1)
-------------------------------------------------------------------------*/
void Uart_Mode_Init(void)
{
	U32 SEL1,SEL2;//modified 76032v03 by DJI @20160406
	IOCON_PIO_CFG_Type PIO_mode;

	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_GPIO0, ENABLE);
	
	IOCON_StructInit(&PIO_mode);

	PIO_mode.type = IOCON_PIO_0_12;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO0, 12, 0); // (ISP)p0.12 input	

	SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_GPIO1, ENABLE);
	
	IOCON_StructInit(&PIO_mode);
#ifdef HW76032V3//Modified for 76032v3 hw by DJI @20160406
	PIO_mode.type = IOCON_PIO_1_5;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO1, 5, 0); // (SEL2)p1.5 input

	PIO_mode.type = IOCON_PIO_1_4;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO1, 4, 0); // (SEL1)p1.4 input

	SEL1 = GPIO_GetPinValue(LPC_GPIO1, 4);
	SEL2 = GPIO_GetPinValue(LPC_GPIO1, 5);
	
	u8_WorkMode	 = (U8)(SEL1 << 1 | SEL2);//Not updated ???by DJI @20160406
	MDB_Address_Mode_Init();//added by DJI for cashless address selection @20160411
	MDB_Decimal_Place_Init(); //added by DJI for MDB decimal point @20160523
#else
	PIO_mode.type = IOCON_PIO_1_1;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO1, 1, 0); // (SEL1)p1.1 input

	PIO_mode.type = IOCON_PIO_1_2;
	IOCON_SetFunc(&PIO_mode);
	GPIO_SetDir(LPC_GPIO1, 2, 0); // (SEL0)p1.2 input

	SEL0 = GPIO_GetPinValue(LPC_GPIO1, 2);
	SEL1 = GPIO_GetPinValue(LPC_GPIO1, 1);

	u8_WorkMode	 = (U8)(SEL1 << 1 | SEL0);
	
#endif
	//return (u8_WorkMode);
	
#if 0
	switch (u8_WorkMode)
	{
	case WM_SLV_VTS_HST_MDB:
		//Uart_Init_VtsSlave();	// Uart0
		//Uart_Init_MdbHost();	// Uart1
		break;
	case WM_SLV_232_HST_MDB:
		//Uart_Init_ComSlave();	// Uart0
		//Uart_Init_MdbHost();	// Uart1
		break;
	case WM_SLV_MDB_HST_232:
		//Uart_Init_MdbSlave();	// Uart0
		//Uart_Init_ComHost();	// Uart1
		
		//Timer_Init_MdbSlave();

		//Cashless_SetupConfig_Init();
		//Cashless_RequestId_Init();
		//Card_BeginSession_Init();
		//p_VendApproved.Code = 0x05;

		//u8_MdbSlave_State = WS_INACTIVE;		

		break;
	case WM_SLV_VTS_HST_232:
		//Uart_Init_VtsSlave();	// Uart0
		//Uart_Init_ComHost();	// Uart1
		break;
	}
#endif

}

/*
**===========================================================================
** END OF FILE
**===========================================================================
*/ 

