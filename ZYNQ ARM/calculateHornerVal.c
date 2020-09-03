/***************************** Include Files *********************************/
#include "xil_io.h"
#include "xparameters.h"
#include "horner_ip.h"

/**************************** user definitions ********************************/

//Horner processor base addres redefinition
#define HORNER_BASE_ADDR      XPAR_HORNER_IP_0_S00_AXI_BASEADDR
//Horner processor registers' offset redefinition
#define CONTROL_REG_OFFSET    HORNER_IP_S00_AXI_SLV_REG0_OFFSET
#define ARG_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG1_OFFSET
#define CN0_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG2_OFFSET
#define CN1_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG3_OFFSET
#define CN2_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG4_OFFSET
#define CN3_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG5_OFFSET
#define CN4_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG6_OFFSET
#define CN5_REG_OFFSET        HORNER_IP_S00_AXI_SLV_REG7_OFFSET
#define STATUS_REG_OFFSET     HORNER_IP_S00_AXI_SLV_REG8_OFFSET
#define RESULT_REG_OFFSET     HORNER_IP_S00_AXI_SLV_REG9_OFFSET
//Cordic processor bits masks
#define CONTROL_REG_START_MASK (u32)(0x01)
#define STATUS_REG_READY_MASK (u32)(0x01)



int32_t calculateHornerValInFPGA(s32 arg, s32 cn0, s32 cn1, s32 cn2, s32 cn3, s32 cn4, s32 cn5)
{
	int32_t retVal = 0;
//Send data to data register of Horner processor
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, ARG_REG_OFFSET, arg);
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CN0_REG_OFFSET, cn0);
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CN1_REG_OFFSET, cn1);
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CN2_REG_OFFSET, cn2);
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CN3_REG_OFFSET, cn3);
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CN4_REG_OFFSET, cn4);
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CN5_REG_OFFSET, cn5);
//Start Horner processor - pulse start bit in control register
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CONTROL_REG_OFFSET, CONTROL_REG_START_MASK);
//Wait for ready bit in status register
	while( (HORNER_IP_mReadReg(HORNER_BASE_ADDR, STATUS_REG_OFFSET) & STATUS_REG_READY_MASK) == 0);
//finish reading
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CONTROL_REG_OFFSET, 0);
//Start Horner processor - pulse start bit in control register
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CONTROL_REG_OFFSET, CONTROL_REG_START_MASK);
//Wait for ready bit in status register
	while( (HORNER_IP_mReadReg(HORNER_BASE_ADDR, STATUS_REG_OFFSET) & STATUS_REG_READY_MASK) == 0);
//Get results
	retVal = HORNER_IP_mReadReg(HORNER_BASE_ADDR, RESULT_REG_OFFSET);
//finish reading
	HORNER_IP_mWriteReg(HORNER_BASE_ADDR, CONTROL_REG_OFFSET, 0);

	return retVal;
}
