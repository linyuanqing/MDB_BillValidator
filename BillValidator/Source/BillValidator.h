#ifndef __BILLVALIDATOR_INTERFACE__
#define __BILLVALIDATOR_INTERFACE__

#include <RTL.h> 
//#include "lpc12xx.h"
#include "lpc12xx_libcfg_default.h"

//addr  cmd
//#define MDBCMD_BILLVALIDATOR_BASE						0x30
#define MDBCMD_BILLVALIDATOR_RESET					0x30//MDBCMD_BILLVALIDATOR_BASE
#define MDBCMD_BILLVALIDATOR_SETUP					0x31//(MDBCMD_BILLVALIDATOR_BASE+1)
#define MDBCMD_BILLVALIDATOR_SECURITY				0x32//(MDBCMD_BILLVALIDATOR_BASE+2)
#define MDBCMD_BILLVALIDATOR_POLL						0x33//(MDBCMD_BILLVALIDATOR_BASE+3)
#define MDBCMD_BILLVALIDATOR_BILLTYPE				0x34//(MDBCMD_BILLVALIDATOR_BASE+4)
#define MDBCMD_BILLVALIDATOR_ESCROW					0x35//(MDBCMD_BILLVALIDATOR_BASE+5)
#define MDBCMD_BILLVALIDATOR_STACKER				0x36//(MDBCMD_BILLVALIDATOR_BASE+6)
#define MDBCMD_BILLVALIDATOR_EXPANSIONCMD		0x37//(MDBCMD_BILLVALIDATOR_BASE+7)

typedef struct
{
	u8	level;							//1
	u8 	country,currency;		//2
	u8	billScalingFctor[2];//2
	u8	decimalPlaces;			//1
	u8	stackerCapacity[2];	//2
	u8	securityLevels[2];	//2
	u8	escrowIsOrNot;			//1
	u8	billTypeCredit[16];	//16
}BV_SETUP;								//27 bytes

typedef	struct
{
	char	manufacturerCode[3];		//3
	char	serialNumber[12];				//12
	char	modelTuningRevision[12];//12
	u8		softwareVersion[2];			//2
	u8		optionalFeatures[4];		//4
}BV_EXPANSION02;								//33 bytes

extern BV_SETUP	bvSetup;
extern BV_EXPANSION02	bvExpansion02;
extern u8	stackerStatus[2];

void BillValidator_Init(void);
void BillValidator_SetSecurityLevels(u8 data[]);
void BillValidator_SetBillType(u8 data[]);
void BillValidator_SetBillEscrowEnable(u8 data[]);
void BillValidator_SetEscrowStatus(u8 data);

#endif
