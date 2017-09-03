#include "BillValidator.h"
#include <string.h>
#include "main.h"

BV_SETUP	bvSetup;
BV_EXPANSION02	bvExpansion02;
u8	billType[2];
u8	billEscrowEnable[2];
u8	escrowStatus=0;
u8	stackerStatus[2]={0x00,0x01};

//SETUP					0x31u
static void BillValidator_Setup_Init(void)
{
		bvSetup.level=0x02;
		bvSetup.country=0x00;
		bvSetup.currency=0x01;
		bvSetup.billScalingFctor[0]=0x00;
		bvSetup.billScalingFctor[1]=0x64;
		bvSetup.decimalPlaces=0x02;
		bvSetup.stackerCapacity[0]=0x01;
		bvSetup.stackerCapacity[1]=0x90;
		bvSetup.securityLevels[0]=0x00;
		bvSetup.securityLevels[1]=0x00;
		bvSetup.escrowIsOrNot=0xFF;
		memset(bvSetup.billTypeCredit,0,sizeof(bvSetup.billTypeCredit));
}

static void BillValidator_Init_IEXPANSION02(void)
{
	//p_MyPeripheral.Id = 0x09;
#ifdef NSL_MPAY//added for different vending machine
	strncpy(bvExpansion02.manufacturerCode, "KWE", 3); //iKawe for other vending machine
	
	strncpy(bvExpansion02.serialNumber, "KW00260114", 12);//Next for TULAN Addr#2 from 0017
	//strncpy(p_MyPeripheral.SerialNumber, "KW00260200", 12);
	//strncpy(p_MyPeripheral.SerialNumber, "KW00260074", 12);//Kawe test sample#1
	
	strncpy(bvExpansion02.modelTuningRevision, "iKAWE 260", 12);
#else		
	strncpy(bvExpansion02.manufacturerCode, "MCC", 3); //ikawe for Spengler SL coffee vending machine

	strncpy(bvExpansion02.serialNumber, "KW00160200", 12);//ikawe for Tulan
	//strncpy(p_MyPeripheral.SerialNumber, "KW00160130", 12);//ikawe for internal testing
	
	strncpy(bvExpansion02.modelTuningRevision, "iKAWE 160", 12);
#endif
	bvExpansion02.softwareVersion[0] = 0x04;
	
	bvExpansion02.softwareVersion[1] = 0x31;
#ifdef MDBLEVEL3
	bvExpansion02.optionalFeatures[0] = 0x22;//set the always idle option for TW cashless
	bvExpansion02.optionalFeatures[1] = 0x00;
	bvExpansion02.optionalFeatures[2] = 0x00;
	bvExpansion02.optionalFeatures[3] = 0x00;
#endif	
}

void BillValidator_SetSecurityLevels(u8 data[])
{
		bvSetup.securityLevels[0]=data[0];
		bvSetup.securityLevels[1]=data[1];
}

void BillValidator_SetBillType(u8 data[])
{
		billType[0]=data[0];
		billType[1]=data[1];
}

void BillValidator_SetBillEscrowEnable(u8 data[])
{
		billEscrowEnable[0]=data[0];
		billEscrowEnable[1]=data[1];
}

void BillValidator_SetEscrowStatus(u8 data)
{
		escrowStatus=data;
}

void BillValidator_Init(void)
{
		BillValidator_Setup_Init();
		BillValidator_Init_IEXPANSION02();
}

