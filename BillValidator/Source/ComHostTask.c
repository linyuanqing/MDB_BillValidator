#include "ComHostTask.h"
#include "Uart.h"
#include "Main.h"

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/

static BOOL Check_ISP_Key_Pressed(void)
{
	return (GPIO_GetPinValue(LPC_GPIO0, 12) == 0);
}

/*-------------------------------------------------------------------------
	service functions comments	
-------------------------------------------------------------------------*/
//const U8 p8_ComHost_Reset[1]	= {0x21};
//const U8 p8_ComHost_Enable[1]	= {0x22};
//const U8 p8_ComHost_Disable[1]	= {0x23};
const U8 p8_ComHost_Poll[1] 	= {0x24};
//const U8 p8_ComHost_Cancel[1] 	= {0x25};
const U8 p8_ComHost_Debit[1]	= {0x26};

const U8 p8_ComHost_Vend[1] 	= {0x28};
const U8 p8_ComHost_Cancel[1] 	= {0x30};

__task void tsk_ComHost(void)
{
	
	//U16 u16_Event;
	T_CARD_STATE * p_CardState;

	//OS_RESULT u32_MyOsResult;

	//BOOL bl_Flag_Reset 	= FALSE;
	//BOOL bl_Flag_Enable = FALSE;
	//BOOL bl_Flag_Cancel = FALSE;

	U16 u16_Wait_Loop_Time = 0;
	U16 u16_Wait_Loop_Time1 = 0;
	U16 u16_Wait_Cancel_Loop_Times = 0;//Repeat vend cancel
	U16 u16_Wait_Rsp_Times = 0;//Response timeout
	//U16 u16_Wait_VendRes_Times = 0;//Repeat vend success or fail
	//U16 u16_Wait_VendSuccess_Time = 0;//added by DJI@20160707
	

	//Uart_Init_ComHost();	// Uart1

	os_sem_init(&sem_ComHost_Rx, 0);

	p_CardPoll.Code 	= 0x24;
	p_CardPoll.Length	= 0x03;

	for (;;)
	{
		//os_dly_wait(20);	// delay 200ms

		switch (u8_ComHost_State)
		{
		case COMHOST_DISABLE:
		case COMHOST_ENABLE:
			
			os_dly_wait(20);	// delay 200ms		
			bl_Flag_QRcode_Request=FALSE;
			bl_Flag_MPAY_Done =FALSE;			

			ComHost_Send((U8 *)&p_CardPoll, sizeof(p_CardPoll));

			if (os_sem_wait(&sem_ComHost_Rx, WAIT_RSP_TIME) != OS_R_TMO)
			{
				if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Poll[0]))
				{
					p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);

					if (p_CardState->CardState == 0x01)	// 用户已刷卡或发起移动支付
					{
						bl_CardAction		= TRUE;
						u32_CardId			= p_CardState->CardId;
					#ifndef	NSL_MPAY//added by DJI @20160325
						if (u8_DecimalPoint_Mode)//added by DJI @20160523
						{
							//u32_Funds			= (p_CardState->Funds*10);
							//u32_Funds			= (((p_CardState->Funds*10)&0x000000FF)<<8)|(((p_CardState->Funds*10)&0x0000FF00)>>8);
							u32_Funds = 0xD007; //2000cents for Decimal point 2 							
						}
						else
							u32_Funds			= p_CardState->Funds;
					#else 						
						if (u8_DecimalPoint_Mode)//added by DJI @20160523
						{							
							//u32_Funds = 0xB80B;//0x0BB8;//3000 cent for GFV vending machine with decimal point 2
							//u32_Funds = 0x8813;//5000 cent for GFV machine with decimal point 2 by DJI @20160726 TULAN
							u32_Funds = 0xFFFF;//No value for DLWSH with decimal point 2 by DJI @20170209
						}
						else
						{
							u32_Funds = 0x2C01;//0x012C, 300cents for decimal point 1
							//u32_Funds = 0xF401;//500cents for decimal point 1 by DJI @20160726 TULAN
						}
					#endif
					
						u16_Wait_Loop_Time	= 0;
						u16_Wait_Loop_Time1	= 0;
						u8_ComHost_State	= COMHOST_SESSION_BEGIN;	// 交易开始，等待扣款
					}
					else if (p_CardState->CardState == 0x07)//待机状态，等待客户发起移动支付申请
					{
						bl_CardAction		= FALSE;
						u8_ComHost_State	= COMHOST_ENABLE;	// 等待刷卡
					}
					else if (p_CardState->CardState == 0x08)//联网失败，无法进行移动支付
					{
						bl_CardAction		= FALSE;
						u8_ComHost_State	= COMHOST_ENABLE;	// 等待刷卡
						u8_MdbSlave_State   = WS_NETWORK_FAILED;		// 移动支付故障，不能进行支付
					}
					else
					{
						bl_CardAction		= FALSE;
						u8_ComHost_State	= COMHOST_ENABLE;	// 等待刷卡						
					}
			
				}
			}
		#if 0
			else//added by DJI@20160528
			{
				u8_ComHost_State = COMHOST_DISABLE;
				bl_CardAction		= FALSE;
				u8_MdbSlave_State = WS_NETWORK_FAILED;
			}
		#endif	
			break;
		case COMHOST_SESSION_BEGIN:
			os_dly_wait(3);	// delay 30ms
			//Wait for petron to select product on vending machine to triger vend request cmd
			if ((bl_Flag_Vend_Request)|| Check_ISP_Key_Pressed()) // isp key pressed to debit
			{
				p_CardDebit.Code		= p8_ComHost_Debit[0];
				p_CardDebit.Length		= 0x0B;
				p_CardDebit.CardId		= u32_CardId;

				if (bl_Flag_Vend_Request)
				{
					bl_Flag_Vend_Request = FALSE;
				#ifdef IKAWE_MPAY
					bl_Flag_QRcode_Request=FALSE;
				#endif
				//#ifndef NSL_MPAY//added by DJI for NSL1000 @20160325
					//p_CardDebit.ItemPrice	= t16_ItemPrice.Word;
				//#else
					if (u8_DecimalPoint_Mode)//added by DJI @20160523
					{
						if (t16_ItemPrice.Word<10)
							p_CardDebit.ItemPrice	= t16_ItemPrice.Word;
						else
							p_CardDebit.ItemPrice	= (t16_ItemPrice.Word/10);
					}
					else 
						p_CardDebit.ItemPrice	= t16_ItemPrice.Word;
				//#endif
					p_CardDebit.ItemNumber	= t16_ItemNuber.Word;
				}
				else
				{
					p_CardDebit.ItemPrice	= 0x0001; //Modified by DJI 20160216
					p_CardDebit.ItemNumber	= 0x000C;//Modified by DJI 20160218
				}

				ComHost_Send((U8 *)&p_CardDebit, sizeof(p_CardDebit));
			
				bl_Flag_QRcode_Request = TRUE;//added by DJI 20160303 to identify the process of vend request and payment
				//u16_Wait_Rsp_Times = WAIT_PAY_TIME;	//timout 210s when no product selected
				
				//Wait for (mobile) deduction result after customer chose product on vending machine
				if (os_sem_wait(&sem_ComHost_Rx, WAIT_RSP_TIME) != OS_R_TMO)//mobile QRcode timeout 10s
				{
					if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Debit[0]))
					{
						p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);
												
					#ifdef IKAWE_MPAY//Added by DJI for ikawe mobile pay
						if ((p_CardState->CardState == 0x02)||(p_CardState->CardState == 0x03)) //  等待客户扫码支付 Added by DJI 20160216
						{							
						    //check QRCode 
							os_dly_wait(3);//wait 30ms, interval for based on 9600bps	
							ComHost_Send((U8 *)&p_CardPoll, sizeof(p_CardPoll));
								
							if (os_sem_wait(&sem_ComHost_Rx, WAIT_QRCODE_RSP_TIME) != OS_R_TMO) // within 1.2s
							{
								if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Poll[0]))
								{
									p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);							
								#if 0// disable for vend approved twice @20160227	
									if (p_CardState->CardState == 0x05)
									{
										u32_CardId			= p_CardState->CardId;
										u32_Funds			= p_CardState->Funds; //MPAY_DEPOSIT，移动支付预充值金额

										//跳转到支付成功状态
										bl_Flag_MPAY_Done = TRUE;
										u16_Wait_Loop_Time	= 0;
										u8_MdbSlave_State 	= WS_VEND_APPROVED;
										u8_ComHost_State	= COMHOST_VEND_APPROVED;											
									}
									else 
								#endif
									if ((p_CardState->CardState == 0x06)||(p_CardState->CardState == 0x08))//移动支付模块取消支付或失败
									{
										bl_Flag_MPAY_Done =FALSE;
										//u8_MdbSlave_State 	= WS_VEND_DENIED;
										u8_MdbSlave_State 	= WS_SESSION_CANCEL;//Cancel if ikawe cancel by DJI @20160326
									
										u8_ComHost_State	= COMHOST_ENABLE;
									}
									else if (p_CardState->CardState == 0x02)//||(p_CardState->CardState == 0x01))
									{
										bl_Flag_QRcode_Request = TRUE;										
									}
								}
							}
							
						}
					#endif
					#if 0//@20170308
						else//扣款失败，退出售卖流程
						{
							u8_MdbSlave_State 	= WS_VEND_DENIED;
							u8_ComHost_State	= COMHOST_ENABLE;
						}
					#endif
					}
					else//扣款失败，退出售卖流程
					{
						u8_MdbSlave_State 	= WS_VEND_DENIED;
						//u8_ComHost_State	= COMHOST_ENABLE;
						u8_ComHost_State = COMHOST_CANCEL;//No response for deduct, force Mobile pay module quit@20170308
					}
				}
				else//支付模块10秒未响应支付请求
				{
					u8_MdbSlave_State 	= WS_VEND_DENIED;
					//u8_ComHost_State	= COMHOST_ENABLE;
					u8_ComHost_State = COMHOST_CANCEL;//No response for deduct, force Mobile pay module quit
				}
			}
			else //if (++u16_Wait_Loop_Time >= 20)//Just send 0x26 one time for vend request of mobile pay
			{
				//u16_Wait_Loop_Time = 0;
				if ((bl_Flag_QRcode_Request==FALSE)||(!u16_Wait_Rsp_Times))
				{	u16_Wait_Rsp_Times=WAIT_SELECTION_TIME;}//defaul timout 60s when no product selected	
				else
				{	u16_Wait_Rsp_Times =WAIT_PAY_TIME;}			

				if (++u16_Wait_Loop_Time1 > u16_Wait_Rsp_Times)//50ms cycle by poll
				{
					u16_Wait_Loop_Time1 = 0;
					
					u32_CardId		= 0;
					u32_Funds		= 0;					
					
					if(bl_Flag_QRcode_Request)
					{	
						u8_MdbSlave_State = WS_VEND_DENIED;//Added by DJI for pay timeout
						//u8_MdbSlave_State = WS_SESSION_CANCEL;
						u8_ComHost_State = COMHOST_CANCEL;//???send cancel 0x30 to DWIN to force quit payment process
					}
					else
					{	
						u8_MdbSlave_State = WS_SESSION_CANCEL;//added by DJI @20160226;//Added by DJI for seletion timeout					
						u8_ComHost_State = COMHOST_CANCEL;//added by DJI to cancel payment
					}
					bl_Flag_QRcode_Request = FALSE;
					bl_Flag_MPAY_Done =FALSE;
				}
				else
				{
					if (u8_MdbSlave_State == WS_SESSIONCOMPLETE)//added to exit pay process when escrow happened @20160316
					{
						if (u8_ComHost_State != COMHOST_CANCEL)
						{
							u8_ComHost_State = COMHOST_CANCEL;
						}
						bl_CardAction = FALSE;
						break;
					}
					
					ComHost_Send((U8 *)&p_CardPoll, sizeof(p_CardPoll));
					
					if (os_sem_wait(&sem_ComHost_Rx, WAIT_DEDUCT_RSP_TIME) != OS_R_TMO) // within 50ms
					{
						if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Poll[0]))
						{
							p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);									
							
							if ((p_CardState->CardState == 0x05)&&(!bl_Flag_MPAY_Done))//Pay successfully
							{
								u32_CardId			= p_CardState->CardId;
							//#ifndef NSL_MPAY//added by DJI for NSL1000 @20160325
								//u32_Funds			= p_CardState->Funds;//是实际扣的金额还是预付的20元余?
								u32_Funds			= p_CardDebit.ItemPrice;//modified for real deducted amount by DJI @20160415								
							//#else
								//u32_Funds = 0xD007; //2000cents for (NSL) coffee vending machine 
								//u32_Funds = 0xB80B;//0x0BB8;//3000 cent for GFV vending machine with decimal point 2								
							//#endif 
							
								//跳转到支付成功状态
								bl_Flag_QRcode_Request = FALSE;
								bl_Flag_MPAY_Done =TRUE;
								u16_Wait_Loop_Time	= 0;
								u16_Wait_Loop_Time1 = 0;
								u8_MdbSlave_State 	= WS_VEND_APPROVED;
								u8_ComHost_State	= COMHOST_VEND_APPROVED;											
							}
							else if (p_CardState->CardState == 0x02)//QRCode done
							{
								bl_Flag_QRcode_Request = TRUE;
							}
							else if (p_CardState->CardState == 0x03)//QRCode done
							{
								bl_Flag_QRcode_Request = TRUE;
							}
							else if ((p_CardState->CardState == 0x06)||(p_CardState->CardState == 0x04)||(p_CardState->CardState == 0x08))//移动支付模块取消支付或失败
							{			
								//if(bl_Flag_QRcode_Request)//Only send after vend request was recevied(selection done)				
								//{	
								//	u8_MdbSlave_State = WS_VEND_DENIED;
								//}
								//else
								u8_MdbSlave_State 	= WS_SESSION_CANCEL;
								
								bl_Flag_QRcode_Request = FALSE;
								bl_Flag_MPAY_Done =FALSE;
								u16_Wait_Loop_Time1	= 0;											
								//u8_ComHost_State	= COMHOST_ENABLE;
								u8_ComHost_State = COMHOST_CANCEL;//added by DJI to cancel payment @20170308
							}
						#if 1//for testing when had 0x07 after 0x01
							else if(p_CardState->CardState == 0x07)//Works only for waiting for select product
							{								
								bl_Flag_QRcode_Request = FALSE;
								bl_Flag_MPAY_Done =FALSE;
								u16_Wait_Loop_Time1	= 0;

								u8_MdbSlave_State 	= WS_SESSION_CANCEL;//Cancel session when no select product
								//u8_ComHost_State	= COMHOST_ENABLE;
								u8_ComHost_State = COMHOST_CANCEL;//added by DJI to cancel payment @20170308
							}
						#endif
								
						}
					}
				}
			}
			
			break;
		case COMHOST_VEND_APPROVED:
			os_dly_wait(2);	// delay 20ms

			if (Check_ISP_Key_Pressed())
			{
				u16_Wait_Loop_Time = 0;
				u8_ComHost_State = COMHOST_VEND_SUCCESS;
			}
			else 
			{

				if (++u16_Wait_Loop_Time > 9000)//6000) //120s; //3000)//60 seconds 
				{
					u16_Wait_Loop_Time = 0;

					if (u8_MdbSlave_State == WS_VEND_SUCCESS)//Make sure state of ComHost RS232 task 
					{
						if (u8_ComHost_State != COMHOST_VEND_SUCCESS)
						{	u8_ComHost_State = COMHOST_VEND_SUCCESS;}						
					}
					else if(u8_MdbSlave_State == WS_VEND_FAILURE)  
					{
						if (u8_ComHost_State != COMHOST_VEND_FAILURE)
						{	u8_ComHost_State = COMHOST_VEND_FAILURE;}
					}
					else
					{
						u8_ComHost_State = COMHOST_VEND_FAILURE;
					}
				}			
				else
				{
					if (u8_MdbSlave_State == WS_VEND_SUCCESS)//Make sure state of ComHost RS232 task 
					{
						if (u8_ComHost_State != COMHOST_VEND_SUCCESS)
						{	u8_ComHost_State = COMHOST_VEND_SUCCESS;}						
					}
					else if(u8_MdbSlave_State == WS_VEND_FAILURE)  
					{
						if (u8_ComHost_State != COMHOST_VEND_FAILURE)
						{	u8_ComHost_State = COMHOST_VEND_FAILURE;}
					}	
				}
			}
			break;
			
		case COMHOST_VEND_SUCCESS:
			os_dly_wait(3);	// delay 30ms added by DJI 20160303
	
			p_VendResult.Code 		= p8_ComHost_Vend[0];
			p_VendResult.Length		= 0x0A;
			p_VendResult.Result 	= 0x01;
			p_VendResult.CardId 	= u32_CardId;
			p_VendResult.ItemNumber = t16_ItemNuber.Word;
			//os_dly_wait(3);	// delay 30ms added by DJI 20160708
			
		  //added to repeat VendSuccess cmd 3 times by DJI@20160820
			//if (++u16_Wait_VendRes_Times > 3)
			//{	
			//	u16_Wait_VendRes_Times = 0;
			//	u8_ComHost_State	= COMHOST_ENABLE;			// 等待刷卡
			//}
			//else
			//{
				ComHost_Send((U8 *)&p_VendResult, sizeof(p_VendResult));

				//if (os_evt_wait_or(EVT_RX_READY, WAIT_RSP_TIME) == OS_R_EVT)	// 5s time out
				if (os_sem_wait(&sem_ComHost_Rx, WAIT_RSP_TIME) != OS_R_TMO)
				{
					if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Vend[0]))
					{
						u8_ComHost_State	= COMHOST_ENABLE;			// 等待刷卡
						//if (u8_MdbSlave_State 	!= WS_SESSIONCOMPLETE)//Force VMC back to idle state @20160308
						//{	u8_MdbSlave_State 	= WS_SESSIONCOMPLETE;}						
					}
					else
					{
						u8_ComHost_State	= COMHOST_CANCEL;			// DWIN退出支付状态再等待刷卡
					}
				}
				else
				{
					//u8_ComHost_State	= COMHOST_ENABLE;			// 等待刷卡
					ComHost_Send((U8 *)&p_VendResult, sizeof(p_VendResult));

					//if (os_evt_wait_or(EVT_RX_READY, WAIT_RSP_TIME) == OS_R_EVT)	// 5s time out
					if (os_sem_wait(&sem_ComHost_Rx, WAIT_RSP_TIME) != OS_R_TMO)
					{
						if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Vend[0]))
						{
							u8_ComHost_State	= COMHOST_ENABLE;			// 等待刷卡
							//if (u8_MdbSlave_State 	!= WS_SESSIONCOMPLETE)//Force VMC back to idle state @20160308
							//{	u8_MdbSlave_State 	= WS_SESSIONCOMPLETE;}						
						}
						else
						{
							u8_ComHost_State	= COMHOST_CANCEL;			// DWIN退出支付状态再等待刷卡
						}
					}
					else
					{
						u8_ComHost_State	= COMHOST_CANCEL;			// DWIN退出支付状态再等待刷卡
					}
				}
			//}

			break;
		case COMHOST_VEND_FAILURE:
			os_dly_wait(3);	// delay 30ms added by DJI 20160303
			p_VendResult.Code 		= p8_ComHost_Vend[0];
			p_VendResult.Length		= 0x0A;
			p_VendResult.Result 	= 0x00;
			p_VendResult.CardId 	= u32_CardId;
			p_VendResult.ItemNumber = t16_ItemNuber.Word;
		
			  //added to repeat VendSuccess cmd 3 times by DJI@20160820
			//if (++u16_Wait_VendRes_Times > 3)
			//{	
			//	u16_Wait_VendRes_Times = 0;
			//	u8_ComHost_State	= COMHOST_CANCEL;			// DWIN退出支付状态再等待刷卡
			//}
			//else
			//{			
				ComHost_Send((U8 *)&p_VendResult, sizeof(p_VendResult));

				//if (os_evt_wait_or(EVT_RX_READY, WAIT_RSP_TIME) == OS_R_EVT)	// 5s time out
				if (os_sem_wait(&sem_ComHost_Rx, WAIT_RSP_TIME) != OS_R_TMO)
				{
					if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Vend[0]))
					{
						u8_ComHost_State = COMHOST_ENABLE;
						//if (u8_MdbSlave_State 	!= WS_SESSIONCOMPLETE)//Force VMC back to idle state @20160308
						//{	u8_MdbSlave_State 	= WS_SESSIONCOMPLETE;}

					#ifndef IKAWE_MPAY//refund by mobile pay	
						p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);

						if (p_CardState->CardState == 0x00)
						{
							u8_MdbSlave_State = WS_REFUND_FAILURE;		// refund failure
						}
						else
						{
							u8_MdbSlave_State = WS_REFUND_SUCCESS;		// 	refund success
						}
					#endif
					}
					else
					{
						u8_ComHost_State = COMHOST_CANCEL;
					#ifndef IKAWE_MPAY
						u8_MdbSlave_State = WS_REFUND_FAILURE;		// refund failure
					#endif
					}
				}
				else
				{
					u8_ComHost_State = COMHOST_ENABLE;
				//#ifndef IKAWE_MPAY
				//	u8_MdbSlave_State = WS_REFUND_FAILURE;		// refund failure
				//#endif
				}
			//}
			break;
		case COMHOST_CANCEL:
			os_dly_wait(3);//delay 30ms
			p_CancelResult.Code 		= p8_ComHost_Cancel[0];
			p_CancelResult.Length		= 0x03;			

			if(++u16_Wait_Cancel_Loop_Times > 5)//send cancel request 5 times worst case
			{	
				u16_Wait_Cancel_Loop_Times = 0;
				u8_ComHost_State = COMHOST_ENABLE;//Force ikawe back to enable state
			}
			else
			{
				ComHost_Send((U8 *)&p_CancelResult, sizeof(p_CancelResult));
				if (os_sem_wait(&sem_ComHost_Rx, WAIT_CANCEL_RSP_TIME) != OS_R_TMO)//Timeout 1s
				{
					if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Cancel[0]))
					{
						p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);
						if (p_CardState->CardState == 0x01) // 已成功取消
						{
							u8_ComHost_State = COMHOST_ENABLE;
						}
					}
					else
						u8_ComHost_State = COMHOST_ENABLE;
				}
			}
			
			break;
		default:
			os_dly_wait(20);	// delay 200ms
			ComHost_Send((U8 *)&p_CardPoll, sizeof(p_CardPoll));
			
			if (os_sem_wait(&sem_ComHost_Rx, WAIT_RSP_TIME) != OS_R_TMO)//timeout 10s
			{
				if ((ComHost_CheckFcc() == TRUE)&&(Uart1RxBuff.Data[0] == p8_ComHost_Poll[0]))
				{
					p_CardState = (T_CARD_STATE *)(Uart1RxBuff.Data);					

					if (p_CardState->CardState == 0x01) // 用户已刷卡
					{
						u32_CardId			= p_CardState->CardId;
					#ifndef NSL_MPAY//added by DJI for NSL1000 @20160325
						u32_Funds			= p_CardState->Funds;
					#else
						//u32_Funds = 0xD007; //2000cents for (NSL) coffee vending machine 
						u32_Funds = 0xB80B;//0x0BB8;//3000 cent for GFV vending machine with decimal point 2
					#endif						
						bl_CardAction		= TRUE;
						u8_ComHost_State	= COMHOST_SESSION_BEGIN;	// 交易开始，等待扣款
						u16_Wait_Loop_Time	= 0;
					}
					else
					{
						bl_CardAction		= FALSE;
						u8_ComHost_State	= COMHOST_ENABLE;			// 等待刷卡
					}
				}
			}
			else
			{
				u8_ComHost_State = COMHOST_ENABLE;
			}
			break;
		}

		u8_Uart1_State = UART_IDLE;
	}

}
