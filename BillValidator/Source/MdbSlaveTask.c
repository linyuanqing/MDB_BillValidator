#include "MdbSlaveTask.h"
#include "Uart.h"
#include "Main.h"
#include "BillValidator.h"
/*-------------------------------------------------------------------------
	MDB slave service functions comments	
-------------------------------------------------------------------------*/
const U8 p8_Ack[1]				= {ACK};
const U8 p8_Session_Cancel[1]	= {0x04}; //modified by DJI @20160227
const U8 p8_Vend_Approved[1]	= {0x05};
const U8 p8_Session_End[1]		= {0x07};
const U8 p8_Vend_Denied[1]		= {0x06};
const U8 p8_Mulfunction[2]		= {0x0A, 0xC0};
const U8 p8_Mulfunction_mPay[2]	= {0x0A, 0x10};//No 3G network

//const U8 p8_ReaderCancelled[1]	= {0x08};
//const U8 p8_RevalueApproved[1]	= {0x0D};
//const U8 p8_RevalueDenied[1]	= {0x0E};
__task void tsk_MdbSlave(void)
{	
	U8 mdbCmd;
	U8 mdbSubCmd;
	//U8 u8_CmdHead;

	//Uart_Init_MdbSlave();	// Uart0
	
	//Timer_Init_MdbSlave();
	
	//Cashless_SetupConfig_Init();
	//Cashless_RequestId_Init();
	//Cashless_BeginSession_Init();
	
	BillValidator_Init();

	//p_VendApproved.Code = p8_Vend_Approved[0];	
	os_sem_init(&sem_MdbSlave_Rx, 0);
	
	u8_MdbSlave_State = WS_INACTIVE; 	

	//os_tsk_prio_self(250);

	for (;;)
	{
		os_sem_wait(&sem_MdbSlave_Rx, 0xFFFF);		
		
		if (MdbSlave_CheckFcc() == TRUE)
		{		
			u8_Uart0_State = UART_WAIT;
			mdbCmd = Uart0RxBuff.Data[0];
					
			switch(mdbCmd)
			{
					case MDBCMD_BILLVALIDATOR_RESET:	//0x30 Reset--------------------------------------------------------------------------
								u8_MdbSlave_State = WS_INACTIVE;
							
								//u8_FlagReset = 1;
								MdbSlave_Response(ACK);
								break;
					
					case MDBCMD_BILLVALIDATOR_SETUP:			//0x31 Setup--------------------------------------------------------------------------
								u8_MdbSlave_State = WS_DISABLED;

								MdbSlave_Responses(&bvSetup, sizeof(bvSetup));
								break;
					
					case MDBCMD_BILLVALIDATOR_SECURITY:		//0x32 Set bill validator Security Levels
								u8_MdbSlave_State = WS_DISABLED;
					
								BillValidator_SetSecurityLevels(&Uart0RxBuff.Data[1]);
								MdbSlave_Response(ACK);
								break;
					
					case MDBCMD_BILLVALIDATOR_POLL:				//0x33
								MdbSlave_Response(ACK);
								break;
					
					case MDBCMD_BILLVALIDATOR_BILLTYPE:		//0x34 Set bill validator BILL TYPE  && Bill Escrow Enable
								u8_MdbSlave_State = WS_DISABLED;
					
								BillValidator_SetBillType(&Uart0RxBuff.Data[1]);
								BillValidator_SetBillEscrowEnable(&Uart0RxBuff.Data[3]);
								MdbSlave_Response(ACK);
								break;
					
					case MDBCMD_BILLVALIDATOR_ESCROW:			//0x35 Set bill validator ESCROW
								u8_MdbSlave_State = WS_DISABLED;
					
								BillValidator_SetEscrowStatus(Uart0RxBuff.Data[1]);
								MdbSlave_Response(ACK);
								break;
					
					case MDBCMD_BILLVALIDATOR_STACKER:		//0x36 Set bill validator STACKER
								u8_MdbSlave_State = WS_DISABLED;
					
								MdbSlave_Responses(&stackerStatus, sizeof(stackerStatus));
								break;
					
					case MDBCMD_BILLVALIDATOR_EXPANSIONCMD://0x37 Expansion--------------------------------------------------------------------------
								mdbSubCmd = Uart0RxBuff.Data[1];
								switch (mdbSubCmd)
								{
										case 0x02:	
												MdbSlave_Responses(&bvExpansion02, sizeof(bvExpansion02));
												break;
										default:
												MdbSlave_Response(ACK);
												break;
								}
								break;

			case MDBCMD_CASHLESS_POLL:	// Poll--------------------------------------------------------------------------
				if (u8_MdbSlave_State == WS_INACTIVE)	//	
				{
					MdbSlave_Responses((void*)p8_Ack, 1);		// 0x100 0x000
				}
				else if (u8_MdbSlave_State == WS_ENABLED) // patron card swiped
				{
					if ((bl_CardAction))	// ||(GPIO_GetPinValue(LPC_GPIO0, 12) == 0))	// card inserted
					{// begin session
						bl_CardAction 			= FALSE;
						u8_MdbSlave_State		= WS_SESSIONIDLE;

				#ifdef NSL_MPAY
					#ifndef IKAWE_MPAY
						p_BeginSession.CardId	= u32_CardId;
					#else
						p_BeginSession.CardId	= 0xFFFFFFFF;//Unknow for Mobile pay
					#endif
				#endif
				#ifdef KAWE_TEST
						p_BeginSession.Funds	= 0xFFFF;
				#else				
						p_BeginSession.Funds	= u32_Funds;
				#endif
					
						MdbSlave_Responses((void *)&p_BeginSession, sizeof(p_BeginSession));
					}
					else
					{
						MdbSlave_Response(ACK);	// 0x100
					}
					
				}
				else if ((u8_MdbSlave_State == WS_VEND_BEGIN)||(u8_MdbSlave_State == WS_SESSIONIDLE)) // vend begin 
				{
					MdbSlave_Response(ACK);					
				}
				else if (u8_MdbSlave_State == WS_VEND_APPROVED) // vend approved 
				{					
					p_VendApproved.Code 	= p8_Vend_Approved[0];	
				#ifdef IKAWE_MPAY//Use the original item price for turnover record in EVA by DJI @20160427
					//p_VendApproved.Amount = u32_Funds; //is 20RMB or 30RMB reported by DWIN module
					p_VendApproved.Amount 	= (t16_ItemPrice.Bytes.Low <<8) | t16_ItemPrice.Bytes.High;//to get right cashless turnovre for VM
				#else
					p_VendApproved.Amount 	= t16_ItemPrice.Bytes.Low * 0x100 + t16_ItemPrice.Bytes.High;
					
				#endif 
				
					if (bl_Flag_MPAY_Done == TRUE)
					{
						bl_Flag_MPAY_Done = FALSE;
						MdbSlave_Responses((void *)&p_VendApproved, sizeof(p_VendApproved));						
					}
					else
						MdbSlave_Response(ACK);				

					u8_MdbSlave_State = WS_VEND_BEGIN;//added by DJI for avoid vend approved twice issue						
				}
				else if (u8_MdbSlave_State == WS_VEND_SUCCESS) // vend success 
				{
					MdbSlave_Response(ACK);
					//MdbSlave_Responses((void *) p8_Vend_Cancel, 1);
				}
				else if (u8_MdbSlave_State == WS_VEND_DENIED) // vend denied or error 
				{
					MdbSlave_Responses((void *)p8_Vend_Denied, 1);

					//u8_MdbSlave_State = WS_SESSIONCOMPLETE;//added by DJI @20160316
				}
				else if (u8_MdbSlave_State == WS_VEND_FAILURE) // vend failure
				{
					//keep silence
				}
				else if (u8_MdbSlave_State == WS_REFUND_SUCCESS) // refund success
				{
					MdbSlave_Response(ACK);
				}
				else if (u8_MdbSlave_State == WS_REFUND_FAILURE) // refund error
				{
					MdbSlave_Responses((void *)p8_Mulfunction,2); // refund error
				}
				else if (u8_MdbSlave_State 	== WS_SESSIONCOMPLETE)
				{	
					if ((u8_ComHost_State != COMHOST_ENABLE)&&!bl_CardAction)//added to exit pay after vend by DJI @20160316
					{
						u8_ComHost_State = COMHOST_ENABLE;
					}
					
					MdbSlave_Responses((void *)p8_Session_End,1);					
					u8_MdbSlave_State 	= WS_ENABLED;
					
				}
				else if (u8_MdbSlave_State == WS_NETWORK_FAILED)
				{
					MdbSlave_Responses((void *)p8_Mulfunction_mPay,2); // Mobile network error
				}
				else if (u8_MdbSlave_State 	== WS_SESSION_CANCEL)//added by DJI @20160227
				{
					if (u8_ComHost_State != COMHOST_CANCEL)//added to exit pay when by DJI @20160316
					{
						u8_ComHost_State = COMHOST_CANCEL;
					}
					
					MdbSlave_Responses((void *) p8_Session_Cancel, 1);	
					u8_MdbSlave_State = WS_SESSIONIDLE;
				}
				else
				{
					MdbSlave_Response(ACK);
					//u8_Shakehands = 1;
				}
				break;

			case MDBCMD_CASHLESS_VEND:	// Vend--------------------------------------------------------------------------
				mdbSubCmd = Uart0RxBuff.Data[1];
				switch (mdbSubCmd)
				{
				case MDBSUB_VEND_REQUEST:
					u8_MdbSlave_State = WS_VEND_BEGIN;//Wait for mobile payment done							 
					//MdbSlave_Response(ACK);

 					// vend approved or vend denied! according to the result of charge the card
 				#ifdef IKAWE_MPAY
					t16_ItemPrice.Word = (Uart0RxBuff.Data[2] <<8)|Uart0RxBuff.Data[3];
					t16_ItemNuber.Word = (Uart0RxBuff.Data[4] <<8)| Uart0RxBuff.Data[5];
				#else
					t16_ItemPrice.Word = Uart0RxBuff.Data[2] * 0x100 + Uart0RxBuff.Data[3];
					t16_ItemNuber.Word = Uart0RxBuff.Data[4] * 0x100 + Uart0RxBuff.Data[5];
				#endif

					bl_Flag_Vend_Request = TRUE;	// Flag to debit on card
 
					MdbSlave_Response(ACK);//Moved by DJI 20160226

					break;
				case MDBSUB_VEND_CANCEL:
			#if 1//def IKAWE_MPAY					
					u8_ComHost_State = COMHOST_CANCEL;//added by DJI @20160222
					
					MdbSlave_Response(ACK);//Moved by DJI 20160226
					u8_MdbSlave_State = WS_VEND_DENIED;//Modifed for pay timeout by DJI @20160308
			#else
					u8_MdbSlave_State 	= WS_SESSIONIDLE;
					MdbSlave_Responses((void*)p8_Vend_Denied, 1);
			#endif		
					
					break;
				case MDBSUB_VEND_SUCCESS:
					u8_ComHost_State  	= COMHOST_VEND_SUCCESS;

					MdbSlave_Response(ACK);
					u8_MdbSlave_State 	= WS_VEND_SUCCESS;	//WS_SESSIONIDLE;										
					
					break;
				case MDBSUB_VEND_FAILURE:
					u8_ComHost_State	= COMHOST_VEND_FAILURE;
					
					MdbSlave_Response(ACK);
					u8_MdbSlave_State 	= WS_VEND_FAILURE;	//WS_SESSIONIDLE;
					// startup the refund function
					
					break;
				case MDBSUB_SESSION_COMPLETE:
			#if 1//def IKAWE_MPAY
					if (bl_CardAction)//added to exit pay by escrow by DJI @20160316
					{
						u8_ComHost_State = COMHOST_CANCEL;
					}
					else
					{	u8_ComHost_State = COMHOST_ENABLE;}
				
					MdbSlave_Response(ACK);		
					u8_MdbSlave_State 	= WS_SESSIONCOMPLETE;
			#else//added by DJI for NSL @20160325					
					if (bl_CardAction)//added to exit pay by escrow by DJI @20160316
					{
						u8_ComHost_State = COMHOST_CANCEL;
					}
					else
					{	u8_ComHost_State = COMHOST_ENABLE;}
					
					MdbSlave_Responses((void *)p8_Session_End,1);
					u8_MdbSlave_State 	= WS_ENABLED;
			#endif		
					break;
				case MDBSUB_CASH_SALE:
					break;
				case MDBSUB_NEGATIVE_VEND_REQUEST:
					u8_MdbSlave_State = WS_NEGATIVE;
					break;
				default:
					MdbSlave_Response(ACK);
					break;
				}
				break;

			case MDBCMD_CASHLESS_READER:	// Reader--------------------------------------------------------------------------
				mdbSubCmd = Uart0RxBuff.Data[1];
				switch (mdbSubCmd)
				{
				case MDBSUB_READER_DISABLE:
					u8_MdbSlave_State = WS_DISABLED;
					MdbSlave_Response(ACK);
					break;
				case MDBSUB_READER_ENABLE:
					u8_MdbSlave_State = WS_ENABLED;
					MdbSlave_Response(ACK);
					break;
				default:
					MdbSlave_Response(ACK);
					break;
				}
				break;

			case MDBCMD_CASHLESS_REVALUE:	// Revalue--------------------------------------------------------------------------
				u8_MdbSlave_State = WS_REVALUE;
				mdbSubCmd = Uart0RxBuff.Data[1];
				switch (mdbSubCmd)
				{
				case MDBSUB_REQUEST_ID:
					MdbSlave_Responses(&p_MyPeripheral, sizeof(p_MyPeripheral));
					break;
				default:
					MdbSlave_Response(ACK);
					break;
				}
				break;

			case MDBCMD_CASHLESS_EXPASION:	// Expansion--------------------------------------------------------------------------
				mdbSubCmd = Uart0RxBuff.Data[1];
				switch (mdbSubCmd)
				{
				case MDBSUB_REQUEST_ID:
					MdbSlave_Responses(&p_MyPeripheral, sizeof(p_MyPeripheral));
					break;
				default:
					MdbSlave_Response(ACK);
					break;
				}
				break;

			default:
				break;
			}
		}
		else	// FCC Error
		{
			MdbSlave_Response(RET);
		}		
		Uart0RxBuff.Index = 0;
		u8_Uart0_State = UART_IDLE;

		UART_ConfigInts(LPC_UART0, UART_INTCFG_RBR, ENABLE);	
		//UART_ConfigInts(LPC_UART0, UART_INTCFG_THRE, ENABLE);	
		UART_ConfigInts(LPC_UART0, UART_INTCFG_RLS, ENABLE);		
	}
}
