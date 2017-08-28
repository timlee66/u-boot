/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*                         fuse_wrapper.c                                                                  */
/*            This file contains fuse wrapper implementation. it wraps all access to the otp               */
/*  Project:  Poleg                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/



#include <common.h>
#include "fuse_wrapper.h"

/*---------------------------------------------------------------------------------------------------------*/
/* This global array is used to host the full encloded key                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define        FUSE_ARRAY_MAX_SIZE    512
static UINT8   fuse_encoded[FUSE_ARRAY_MAX_SIZE] = {0};

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WRPR_set                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fuse_address    -                                                                      */
/*                  fuse_length     -                                                                      */
/*                  fuse_ecc        -                                                                      */
/*                  array           -                                                                      */
/*                  value           -                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_set(UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,  FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value)
{

	UINT16 iCnt = 0;
	DEFS_STATUS status = DEFS_STATUS_OK;

	/* actual size for fuses to read: */
	/* UINT16 iSize = fuse_length; */
	ASSERT(iSize < FUSE_ARRAY_MAX_SIZE);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Decode , if needed. If decode fails... TBD                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_NIBBLE_PARITY)
	{
        /* iSize = fuse_length / 2; */
        status = FUSE_NibParEccEncode(value, fuse_encoded, fuse_length);
	}

	else if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_MAJORITY)
	{
        /* iSize = fuse_length / 3; */
        status = FUSE_MajRulEccEncode(value, fuse_encoded, fuse_length);
	}

	if ( status != DEFS_STATUS_OK)
	{
        return status;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* And program to OTP                                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	/* if no parity */
	if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_NONE )
	{
        for (iCnt = 0; iCnt < fuse_length ; iCnt++)
        {
            FUSE_ProgramByte((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , value[iCnt]);
        }
	}
	else
	{
        for (iCnt = 0; iCnt < fuse_length ; iCnt++)
        {
            FUSE_ProgramByte((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , fuse_encoded[iCnt]);
        }
	}

	return status;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WRPR_get                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fuse_address    -   address in the fuse\key array.                                     */
/*                  fuse_length     -   length in bytes inside the fuse array (before encoding)            */
/*                  fuse_ecc        -   nible parity\majority\none                                         */
/*                  array           -   key array\fuse array                                               */
/*                  value           -                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read a value from the fuses.                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,   FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value)
{
	UINT16 iCnt = 0;

	DEFS_STATUS status = DEFS_STATUS_OK;

	DEFS_STATUS_COND_CHECK(fuse_length <= FUSE_ARRAY_MAX_SIZE, DEFS_STATUS_INVALID_PARAMETER);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read the fuses                                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	/* if no parity */
	if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_NONE )
	{
        for (iCnt = 0; iCnt < fuse_length ; iCnt++)
        {
            FUSE_Read((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , &value[iCnt]);
        }
	}
	else
	{
        for (iCnt = 0; iCnt < fuse_length ; iCnt++)
        {
            FUSE_Read((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , &fuse_encoded[iCnt]);
        }

	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Either encode the data or read it as is                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_NIBBLE_PARITY)
	{
        status = FUSE_NibParEccDecode(fuse_encoded, value, fuse_length);
	}

	if ( (FUSE_ECC_TYPE_T)fuse_ecc == FUSE_ECC_MAJORITY)
	{
        status = FUSE_MajRulEccDecode(fuse_encoded, value, fuse_length);
	}


	return status;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        FUSE_WRPR_get_CP_Fustrap                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  fuse_address    -                                                                      */
/*                  fuse_length     -   length in bytes inside the fuse array (before encoding)            */
/*                  fuse_ecc        -   nible parity\majority\none                                         */
/*                  array           -   key array\fuse array                                               */
/*                  value           -                                                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read a value from the fuses. It's for a field that is ten bit :(          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get_CP_Fustrap (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,   FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value)
{

	/* Due to architecture bug , CP_FUSTRAP is only 10 bits. In order not to change the rest of the FUSE_Wrapper, this code is set seperetly */

	UINT16 iCnt = 0;
	UINT  bit;
	UINT8 E1, E2, E3;


	DEFS_STATUS_COND_CHECK(fuse_length <= FUSE_ARRAY_MAX_SIZE, DEFS_STATUS_INVALID_PARAMETER);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read the fuses                                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	for (iCnt = 0; iCnt < fuse_length ; iCnt++)
	{
        FUSE_Read((FUSE_STORAGE_ARRAY_T)array, fuse_address + iCnt , &fuse_encoded[iCnt]);
	}


	for (bit = 0; bit < 10; bit++)
	{
        E1 = READ_VAR_BIT(fuse_encoded[(( 0 + bit ) / 8)], (( 0 + bit ) % 8));
        E2 = READ_VAR_BIT(fuse_encoded[((10 + bit ) / 8)], ((10 + bit ) % 8));
        E3 = READ_VAR_BIT(fuse_encoded[((20 + bit ) / 8)], ((20 + bit ) % 8));
        if ((E1+E2+E3) >= 2)
        {
            SET_VAR_BIT(value[(bit / 8)], (bit % 8));    // Majority is 1
        }
        else
        {
             CLEAR_VAR_BIT(value[(bit / 8)], (bit % 8));  // Majority is 0
        }
	}/*Inner for (bit) */


	return DEFS_STATUS_OK;

	}

/*#define CONFIG_FUSE_DEBUG */
#ifdef CONFIG_FUSE_DEBUG

#define FUSE_GET_ADDRESS 0x2100000
UINT8 *fuse_array_copy = (UINT8 *)FUSE_GET_ADDRESS;
extern UINT8 *fuse_array_mem;


int do_fuse_get(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	FUSE_WRPR_get(oFUSTRAP_PROPERTY, fuse_array_copy+oFUSTRAP_ARRAY*1024+oFUSTRAP_START);
	FUSE_WRPR_get(oDAC_CALIB_PROPERTY, fuse_array_copy+oDAC_CALIB_ARRAY*1024+oDAC_CALIB_START);
	FUSE_WRPR_get(oADC_CALIB_PROPERTY, fuse_array_copy+oADC_CALIB_ARRAY*1024+oADC_CALIB_START);
	FUSE_WRPR_get(oVERIFICATION_FAULT_MODULE_PROTECTION_PROPERTY, fuse_array_copy+oVERIFICATION_FAULT_MODULE_PROTECTION_ARRAY*1024+oVERIFICATION_FAULT_MODULE_PROTECTION_START);
	FUSE_WRPR_get(oFSVFP_PROPERTY, fuse_array_copy+oFSVFP_ARRAY*1024+oFSVFP_START);
	FUSE_WRPR_get(oKAVFP_PROPERTY, fuse_array_copy+oKAVFP_ARRAY*1024+oKAVFP_START);
	FUSE_WRPR_get(oFSAP_PROPERTY, fuse_array_copy+oFSAP_ARRAY*1024+oFSAP_START);
	FUSE_WRPR_get(oKAP_PROPERTY, fuse_array_copy+oKAP_ARRAY*1024+oKAP_START);
	FUSE_WRPR_get(oDERIVATIVE_PROPERTY, fuse_array_copy+oDERIVATIVE_ARRAY*1024+oDERIVATIVE_START);
	FUSE_WRPR_get(oFINAL_TEST_SIGNATURE_PROPERTY, fuse_array_copy+oFINAL_TEST_SIGNATURE_ARRAY*1024+oFINAL_TEST_SIGNATURE_START);
	FUSE_WRPR_get(oDIE_LOCATION_PROPERTY, fuse_array_copy+oDIE_LOCATION_ARRAY*1024+oDIE_LOCATION_START);
	FUSE_WRPR_get(RESERVED1_PROPERTY, fuse_array_copy+RESERVED1_ARRAY*1024+RESERVED1_START);
	FUSE_WRPR_get(oGENERAL_PURPOSE1_PROPERTY, fuse_array_copy+oGENERAL_PURPOSE1_ARRAY*1024+oGENERAL_PURPOSE1_START);
	FUSE_WRPR_get(oGENERAL_PURPOSE2_PROPERTY, fuse_array_copy+oGENERAL_PURPOSE2_ARRAY*1024+oGENERAL_PURPOSE2_START);
	FUSE_WRPR_get(oPKValue2_1_PROPERTY, fuse_array_copy+oPKValue2_1_ARRAY*1024+oPKValue2_1_START);
	FUSE_WRPR_get(oPKValue1_PROPERTY, fuse_array_copy+oPKValue1_ARRAY*1024+oPKValue1_START);
	FUSE_WRPR_get(oAESKEY0_PROPERTY, fuse_array_copy+oAESKEY0_ARRAY*1024+oAESKEY0_START);
	FUSE_WRPR_get(oAESKEY1_PROPERTY, fuse_array_copy+oAESKEY1_ARRAY*1024+oAESKEY1_START);
	FUSE_WRPR_get(oAESKEY2_PROPERTY, fuse_array_copy+oAESKEY2_ARRAY*1024+oAESKEY2_START);
	FUSE_WRPR_get(oAESKEY3_PROPERTY, fuse_array_copy+oAESKEY3_ARRAY*1024+oAESKEY3_START);
	FUSE_WRPR_get(oPKValue2_0_PROPERTY, fuse_array_copy+oPKValue2_0_ARRAY*1024+oPKValue2_0_START);
	FUSE_WRPR_get(oPKValue0_PROPERTY, fuse_array_copy+oPKValue0_ARRAY*1024+oPKValue0_START);

	return 0;
}

int do_fuse_set(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	FUSE_WRPR_set(oFUSTRAP_PROPERTY, fuse_array_copy+oFUSTRAP_ARRAY*1024+oFUSTRAP_START);
	FUSE_WRPR_set(oDAC_CALIB_PROPERTY, fuse_array_copy+oDAC_CALIB_ARRAY*1024+oDAC_CALIB_START);
	FUSE_WRPR_set(oADC_CALIB_PROPERTY, fuse_array_copy+oADC_CALIB_ARRAY*1024+oADC_CALIB_START);
	FUSE_WRPR_set(oVERIFICATION_FAULT_MODULE_PROTECTION_PROPERTY, fuse_array_copy+oVERIFICATION_FAULT_MODULE_PROTECTION_ARRAY*1024+oVERIFICATION_FAULT_MODULE_PROTECTION_START);
	FUSE_WRPR_set(oFSVFP_PROPERTY, fuse_array_copy+oFSVFP_ARRAY*1024+oFSVFP_START);
	FUSE_WRPR_set(oKAVFP_PROPERTY, fuse_array_copy+oKAVFP_ARRAY*1024+oKAVFP_START);
	FUSE_WRPR_set(oFSAP_PROPERTY, fuse_array_copy+oFSAP_ARRAY*1024+oFSAP_START);
	FUSE_WRPR_set(oKAP_PROPERTY, fuse_array_copy+oKAP_ARRAY*1024+oKAP_START);
	FUSE_WRPR_set(oDERIVATIVE_PROPERTY, fuse_array_copy+oDERIVATIVE_ARRAY*1024+oDERIVATIVE_START);
	FUSE_WRPR_set(oFINAL_TEST_SIGNATURE_PROPERTY, fuse_array_copy+oFINAL_TEST_SIGNATURE_ARRAY*1024+oFINAL_TEST_SIGNATURE_START);
	FUSE_WRPR_set(oDIE_LOCATION_PROPERTY, fuse_array_copy+oDIE_LOCATION_ARRAY*1024+oDIE_LOCATION_START);
	FUSE_WRPR_set(RESERVED1_PROPERTY, fuse_array_copy+RESERVED1_ARRAY*1024+RESERVED1_START);
	FUSE_WRPR_set(oGENERAL_PURPOSE1_PROPERTY, fuse_array_copy+oGENERAL_PURPOSE1_ARRAY*1024+oGENERAL_PURPOSE1_START);
	FUSE_WRPR_set(oGENERAL_PURPOSE2_PROPERTY, fuse_array_copy+oGENERAL_PURPOSE2_ARRAY*1024+oGENERAL_PURPOSE2_START);
	FUSE_WRPR_set(oPKValue2_1_PROPERTY, fuse_array_copy+oPKValue2_1_ARRAY*1024+oPKValue2_1_START);
	FUSE_WRPR_set(oPKValue1_PROPERTY, fuse_array_copy+oPKValue1_ARRAY*1024+oPKValue1_START);
	FUSE_WRPR_set(oAESKEY0_PROPERTY, fuse_array_copy+oAESKEY0_ARRAY*1024+oAESKEY0_START);
	FUSE_WRPR_set(oAESKEY1_PROPERTY, fuse_array_copy+oAESKEY1_ARRAY*1024+oAESKEY1_START);
	FUSE_WRPR_set(oAESKEY2_PROPERTY, fuse_array_copy+oAESKEY2_ARRAY*1024+oAESKEY2_START);
	FUSE_WRPR_set(oAESKEY3_PROPERTY, fuse_array_copy+oAESKEY3_ARRAY*1024+oAESKEY3_START);
	FUSE_WRPR_set(oPKValue2_0_PROPERTY, fuse_array_copy+oPKValue2_0_ARRAY*1024+oPKValue2_0_START);
	FUSE_WRPR_set(oPKValue0_PROPERTY, fuse_array_copy+oPKValue0_ARRAY*1024+oPKValue0_START);

	return 0;
}

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


U_BOOT_CMD(
     fuse_get,   1,   1,     do_fuse_get,
     "read from fuse arrays decode and place at " STR(FUSE_GET_ADDRESS),
     ""
);

U_BOOT_CMD(
     fuse_set,   1,   1,     do_fuse_set,
     "read from " STR(FUSE_GET_ADDRESS) " encode and program into fuse arrays" ,
     ""
);

#endif /* CONFIG_FUSE_DEBUG */

