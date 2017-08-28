/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014-2016 by Nuvoton Technology Corporation                                              */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*                         fuse_wrapper.h                                                                  */
/*            This file contains fuse wrapper implementation. it wraps all access to the otp.              */
/*            For the user: call FUSE_WRPR_set or FUSE_WRPR_get with the property fields.                  */
/*  Project:  Poleg                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _FUSE_WRAPPER_
#define _FUSE_WRAPPER_

/*---------------------------------------------------------------------------------------------------------*/
/* Address of fuse elemnt                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum FUSE_ADDRESS_START_tag
{
	oFUSTRAP_START                              = 0                     ,    /* Start address of FUSTRAP   */
	oCP_FUSTRAP_START                           = 12                    ,    /* Start address of Coprocessor CP_FUSTRAP   */
	oDAC_CALIB_START                            = 16                    ,    /* Start address of DAC Calibration Word   */
	oADC_CALIB_START                            = 24                    ,    /* Start address of ADC Calibration Word   */
	oVERIFICATION_FAULT_MODULE_PROTECTION_START = 32                    ,    /* Start address of Verification Fault Module Protection   */
	oFSVFP_START                                = 40                    ,    /* Start address of Fuse Strap Array Verification Fault Protection (oFSVFP)   */
	oKAVFP_START                                = 46                    ,    /* Start address of Key Array Verification Fault Protection (oKAVFP)   */
	oFSAP_START                                 = 52                    ,    /* Start address of Fuse Strap Array Access Protection (oFSAP)   */
	oKAP_START                                  = 58                    ,    /* Start address of Key Array Access Protection (oKAP)   */
	RESERVED_START                              = 64                    ,    /* Start address of Reserved for Nuvoton use   */
	oDERIVATIVE_START                           = 64                    ,    /* Start address of Derivative Word   */
	oFINAL_TEST_SIGNATURE_START                 = 72                    ,    /* Start address of Final test signature   */
	oDIE_LOCATION_START                         = 74                    ,    /* Start address of Die location: Horizontal   , Vertical, Wafer#   */
	RESERVED1_START                             = 77                    ,    /* Start address of Reserved   */
	oGENERAL_PURPOSE1_START                     = 80                    ,    /* Start address of General Purpose   */
	oGENERAL_PURPOSE2_START                     = 128                   ,    /* Start address of General Purpose   */
	oPKValue2_1_START                           = 256                   ,    /* Start address of Second Half of RSA key 2 (oPKValue2[1024:2047)   */
	oPKValue1_START                             = 512                   ,    /* Start address of RSA Key 1 (oPKValue1)   */
	oAESKEY0_START                              = 0                     ,    /* Start address of AES Key 0 (oAESKEY0)   */
	oAESKEY1_START                              = 64                    ,    /* Start address of AES Key 1 (oAESKEY1)   */
	oAESKEY2_START                              = 128                   ,    /* Start address of AES Key 2 (oAESKEY2)   */
	oAESKEY3_START                              = 192                   ,    /* Start address of AES Key 3 (oAESKEY3)   */
	oPKValue2_0_START                           = 256                   ,    /* Start address of First Half of RSA key 2 (oPKValue2[0:1023])   */
	oPKValue0_START                             = 512                   ,    /* Start address of RSA Key 0 (oPKValue0)   */
} FUSE_ADDRESS_START_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Length of fuse element. This is the full length, not the length after decode which is smaller           */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FUSE_LENGTH_tag
{
	oFUSTRAP_LENGTH                             = 12                    ,    /* length of OTP field FUSTRAP   */
	oCP_FUSTRAP_LENGTH                          = 4                     ,    /* length of OTP field Coprocessor CP_FUSTRAP   */
	oDAC_CALIB_LENGTH                           = 8                     ,    /* length of OTP field DAC Calibration Word   */
	oADC_CALIB_LENGTH                           = 8                     ,    /* length of OTP field ADC Calibration Word   */
	oVERIFICATION_FAULT_MODULE_PROTECTION_LENGTH= 8                     ,    /* length of OTP field Verification Fault Module Protection   */
	oFSVFP_LENGTH                               = 6                     ,    /* length of OTP field Fuse Strap Array Verification Fault Protection (oFSVFP)   */
	oKAVFP_LENGTH                               = 6                     ,    /* length of OTP field Key Array Verification Fault Protection (oKAVFP)   */
	oFSAP_LENGTH                                = 6                     ,    /* length of OTP field Fuse Strap Array Access Protection (oFSAP)   */
	oKAP_LENGTH                                 = 6                     ,    /* length of OTP field Key Array Access Protection (oKAP)   */
	RESERVED_LENGTH                             = 16                    ,    /* length of OTP field Reserved for Nuvoton use   */
	oDERIVATIVE_LENGTH                          = 8                     ,    /* length of OTP field Derivative Word   */
	oFINAL_TEST_SIGNATURE_LENGTH                = 2                     ,    /* length of OTP field Final test signature   */
	oDIE_LOCATION_LENGTH                        = 3                     ,    /* length of OTP field Die location: Horizontal, Vertical, Wafer#   */
	RESERVED1_LENGTH                            = 3                     ,    /* length of OTP field Reserved   */
	oGENERAL_PURPOSE1_LENGTH                    = 48                    ,    /* length of OTP field General Purpose   */
	oGENERAL_PURPOSE2_LENGTH                    = 128                   ,    /* length of OTP field General Purpose   */
	oPKValue2_1_LENGTH                          = 256                   ,    /* length of OTP field Second Half of RSA key 2 (oPKValue2[1024:2047)   */
	oPKValue1_LENGTH                            = 512                   ,    /* length of OTP field RSA Key 1 (oPKValue1)   */
	oAESKEY0_LENGTH                             = 64                    ,    /* length of OTP field AES Key 0 (oAESKEY0)   */
	oAESKEY1_LENGTH                             = 64                    ,    /* length of OTP field AES Key 1 (oAESKEY1)   */
	oAESKEY2_LENGTH                             = 64                    ,    /* length of OTP field AES Key 2 (oAESKEY2)   */
	oAESKEY3_LENGTH                             = 64                    ,    /* length of OTP field AES Key 3 (oAESKEY3)   */
	oPKValue2_0_LENGTH                          = 256                   ,    /* length of OTP field First Half of RSA key 2 (oPKValue2[0:1023])   */
	oPKValue0_LENGTH                            = 512                   ,    /* length of OTP field RSA Key 0 (oPKValue0)   */
} FUSE_LENGTH_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Type of ECC of each element in otp                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FUSE_ECC_TYPES_tag  /* FUSE_ECC_TYPE_T */
{
	oFUSTRAP_ECC                                = FUSE_ECC_MAJORITY      ,    /*  FUSTRAP   */
	oCP_FUSTRAP_ECC                             = FUSE_ECC_MAJORITY      ,    /*  Coprocessor CP_FUSTRAP   */
	oDAC_CALIB_ECC                              = FUSE_ECC_NIBBLE_PARITY ,    /*  DAC Calibration Word   */
	oADC_CALIB_ECC                              = FUSE_ECC_NIBBLE_PARITY ,    /*  ADC Calibration Word   */
	oVERIFICATION_FAULT_MODULE_PROTECTION_ECC   = FUSE_ECC_NIBBLE_PARITY ,    /*  Verification Fault Module Protection   */
	oFSVFP_ECC                                  = FUSE_ECC_MAJORITY      ,    /*  Fuse Strap Array Verification Fault Protection (oFSVFP)   */
	oKAVFP_ECC                                  = FUSE_ECC_MAJORITY      ,    /*  Key Array Verification Fault Protection (oKAVFP)   */
	oFSAP_ECC                                   = FUSE_ECC_MAJORITY      ,    /*  Fuse Strap Array Access Protection (oFSAP)   */
	oKAP_ECC                                    = FUSE_ECC_MAJORITY      ,    /*  Key Array Access Protection (oKAP)   */
	RESERVED_ECC                                = FUSE_ECC_NONE          ,    /*  Reserved for Nuvoton use   */
	oDERIVATIVE_ECC                             = FUSE_ECC_NIBBLE_PARITY ,    /*  Derivative Word   */
	oFINAL_TEST_SIGNATURE_ECC                   = FUSE_ECC_NONE          ,    /*  Final test signature   */
	oDIE_LOCATION_ECC                           = FUSE_ECC_NONE          ,    /*  Die location: Horizontal, Vertical, Wafer#   */
	RESERVED1_ECC                               = FUSE_ECC_NONE          ,    /*  Reserved   */
	oGENERAL_PURPOSE1_ECC                       = FUSE_ECC_NONE          ,    /*  General Purpose   */
	oGENERAL_PURPOSE2_ECC                       = FUSE_ECC_NONE          ,    /*  General Purpose   */
	oPKValue2_1_ECC                             = FUSE_ECC_NIBBLE_PARITY ,    /*  Second Half of RSA key 2 (oPKValue2[1024:2047)   */
	oPKValue1_ECC                               = FUSE_ECC_NIBBLE_PARITY ,    /*  RSA Key 1 (oPKValue1)   */
	oAESKEY0_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 0 (oAESKEY0)   */
	oAESKEY1_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 1 (oAESKEY1)   */
	oAESKEY2_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 2 (oAESKEY2)   */
	oAESKEY3_ECC                                = FUSE_ECC_NIBBLE_PARITY ,    /*  AES Key 3 (oAESKEY3)   */
	oPKValue2_0_ECC                             = FUSE_ECC_NIBBLE_PARITY ,    /*  First Half of RSA key 2 (oPKValue2[0:1023])   */
	oPKValue0_ECC                               = FUSE_ECC_NIBBLE_PARITY ,    /*  RSA Key 0 (oPKValue0)   */
} FUSE_ECC_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Lacation of each element in otp                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum  FUSE_ARRAY_tag /* FUSE_STORAGE_ARRAY_T */
{
	oFUSTRAP_ARRAY                              =  FUSE_SA              ,  /* FUSTRAP */
	oCP_FUSTRAP_ARRAY                           =  FUSE_SA              ,  /* Coprocessor CP_FUSTRAP */
	oDAC_CALIB_ARRAY                            =  FUSE_SA              ,  /* DAC Calibration Word */
	oADC_CALIB_ARRAY                            =  FUSE_SA              ,  /* ADC Calibration Word */
	oVERIFICATION_FAULT_MODULE_PROTECTION_ARRAY =  FUSE_SA              ,  /* Verification Fault Module Protection */
	oFSVFP_ARRAY                                =  FUSE_SA              ,  /* Fuse Strap Array Verification Fault Protection (oFSVFP) */
	oKAVFP_ARRAY                                =  FUSE_SA              ,  /* Key Array Verification Fault Protection (oKAVFP) */
	oFSAP_ARRAY                                 =  FUSE_SA              ,  /* Fuse Strap Array Access Protection (oFSAP) */
	oKAP_ARRAY                                  =  FUSE_SA              ,  /* Key Array Access Protection (oKAP) */
	RESERVED_ARRAY                              =  FUSE_SA              ,  /* Reserved for Nuvoton use */
	oDERIVATIVE_ARRAY                           =  FUSE_SA              ,  /* Derivative Word */
	oFINAL_TEST_SIGNATURE_ARRAY                 =  FUSE_SA              ,  /* Final test signature */
	oDIE_LOCATION_ARRAY                         =  FUSE_SA              ,  /* Die location: Horizontal                      , Vertical, Wafer# */
	RESERVED1_ARRAY                             =  FUSE_SA              ,  /* Reserved */
	oGENERAL_PURPOSE1_ARRAY                     =  FUSE_SA              ,  /* General Purpose */
	oGENERAL_PURPOSE2_ARRAY                     =  FUSE_SA              ,  /* General Purpose */
	oPKValue2_1_ARRAY                           =  FUSE_SA              ,  /* Second Half of RSA key 2 (oPKValue2[1024:2047) */
	oPKValue1_ARRAY                             =  FUSE_SA              ,  /* RSA Key 1 (oPKValue1) */
	oAESKEY0_ARRAY                              =  KEY_SA               ,  /* AES Key 0 (oAESKEY0) */
	oAESKEY1_ARRAY                              =  KEY_SA               ,  /* AES Key 1 (oAESKEY1) */
	oAESKEY2_ARRAY                              =  KEY_SA               ,  /* AES Key 2 (oAESKEY2) */
	oAESKEY3_ARRAY                              =  KEY_SA               ,  /* AES Key 3 (oAESKEY3) */
	oPKValue2_0_ARRAY                           =  KEY_SA               ,  /* First Half of RSA key 2 (oPKValue2[0:1023]) */
	oPKValue0_ARRAY                             =  KEY_SA               ,  /* RSA Key 0 (oPKValue0) */
} FUSE_ARRAY_T;


/*---------------------------------------------------------------------------------------------------------*/
/* OTP elements. use them in setter in getter below                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define      oFUSTRAP_PROPERTY                                (UINT16)oFUSTRAP_START, (UINT16)oFUSTRAP_LENGTH, oFUSTRAP_ECC, oFUSTRAP_ARRAY
#define      oCP_FUSTRAP_PROPERTY                             (UINT16)oCP_FUSTRAP_START, (UINT16)oCP_FUSTRAP_LENGTH, oCP_FUSTRAP_ECC, oCP_FUSTRAP_ARRAY
#define      oDAC_CALIB_PROPERTY                              (UINT16)oDAC_CALIB_START, (UINT16)oDAC_CALIB_LENGTH, oDAC_CALIB_ECC, oDAC_CALIB_ARRAY
#define      oADC_CALIB_PROPERTY                              (UINT16)oADC_CALIB_START, (UINT16)oADC_CALIB_LENGTH, oADC_CALIB_ECC, oADC_CALIB_ARRAY
#define      oVERIFICATION_FAULT_MODULE_PROTECTION_PROPERTY   (UINT16)oVERIFICATION_FAULT_MODULE_PROTECTION_START,(UINT16) oVERIFICATION_FAULT_MODULE_PROTECTION_LENGTH, oVERIFICATION_FAULT_MODULE_PROTECTION_ECC, oVERIFICATION_FAULT_MODULE_PROTECTION_ARRAY
#define      oFSVFP_PROPERTY                                  (UINT16)oFSVFP_START, (UINT16)oFSVFP_LENGTH, oFSVFP_ECC, oFSVFP_ARRAY
#define      oKAVFP_PROPERTY                                  (UINT16)oKAVFP_START, (UINT16)oKAVFP_LENGTH, oKAVFP_ECC, oKAVFP_ARRAY
#define      oFSAP_PROPERTY                                   (UINT16)oFSAP_START, (UINT16)oFSAP_LENGTH, oFSAP_ECC, oFSAP_ARRAY
#define      oKAP_PROPERTY                                    (UINT16)oKAP_START, (UINT16)oKAP_LENGTH, oKAP_ECC, oKAP_ARRAY
#define      RESERVED_PROPERTY                                (UINT16)RESERVED_START, (UINT16)RESERVED_LENGTH, RESERVED_ECC, RESERVED_ARRAY
#define      oDERIVATIVE_PROPERTY                             (UINT16)oDERIVATIVE_START, (UINT16)oDERIVATIVE_LENGTH, oDERIVATIVE_ECC, oDERIVATIVE_ARRAY
#define      oFINAL_TEST_SIGNATURE_PROPERTY                   (UINT16)oFINAL_TEST_SIGNATURE_START, (UINT16)oFINAL_TEST_SIGNATURE_LENGTH, oFINAL_TEST_SIGNATURE_ECC, oFINAL_TEST_SIGNATURE_ARRAY
#define      oDIE_LOCATION_PROPERTY                           (UINT16)oDIE_LOCATION_START, (UINT16)oDIE_LOCATION_LENGTH, oDIE_LOCATION_ECC, oDIE_LOCATION_ARRAY
#define      RESERVED1_PROPERTY                               (UINT16)RESERVED1_START, (UINT16)RESERVED1_LENGTH, RESERVED1_ECC, RESERVED1_ARRAY
#define      oGENERAL_PURPOSE1_PROPERTY                       (UINT16)oGENERAL_PURPOSE1_START, (UINT16)oGENERAL_PURPOSE1_LENGTH, oGENERAL_PURPOSE1_ECC, oGENERAL_PURPOSE1_ARRAY
#define      oGENERAL_PURPOSE2_PROPERTY                       (UINT16)oGENERAL_PURPOSE2_START, (UINT16)oGENERAL_PURPOSE2_LENGTH, oGENERAL_PURPOSE2_ECC, oGENERAL_PURPOSE2_ARRAY
#define      oPKValue2_1_PROPERTY                             (UINT16)oPKValue2_1_START, (UINT16)oPKValue2_1_LENGTH, oPKValue2_1_ECC, oPKValue2_1_ARRAY
#define      oPKValue1_PROPERTY                               (UINT16)oPKValue1_START, (UINT16)oPKValue1_LENGTH, oPKValue1_ECC, oPKValue1_ARRAY
#define      oAESKEY0_PROPERTY                                (UINT16)oAESKEY0_START, (UINT16)oAESKEY0_LENGTH, oAESKEY0_ECC, oAESKEY0_ARRAY
#define      oAESKEY1_PROPERTY                                (UINT16)oAESKEY1_START, (UINT16)oAESKEY1_LENGTH, oAESKEY1_ECC, oAESKEY1_ARRAY
#define      oAESKEY2_PROPERTY                                (UINT16)oAESKEY2_START, (UINT16)oAESKEY2_LENGTH, oAESKEY2_ECC, oAESKEY2_ARRAY
#define      oAESKEY3_PROPERTY                                (UINT16)oAESKEY3_START, (UINT16)oAESKEY3_LENGTH, oAESKEY3_ECC, oAESKEY3_ARRAY
#define      oPKValue2_0_PROPERTY                             (UINT16)oPKValue2_0_START, (UINT16)oPKValue2_0_LENGTH, oPKValue2_0_ECC, oPKValue2_0_ARRAY
#define      oPKValue0_PROPERTY                               (UINT16)oPKValue0_START, (UINT16)oPKValue0_LENGTH, oPKValue0_ECC, oPKValue0_ARRAY


/*---------------------------------------------------------------------------------------------------------*/
/* Setter for otp elements                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_set (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length, FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value);

/*---------------------------------------------------------------------------------------------------------*/
/* Getter for otp elements                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length, FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value);

/*---------------------------------------------------------------------------------------------------------*/
/* Getter for cp fustrp. It has a design bug. 10 bits instead of 16 :)                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS FUSE_WRPR_get_CP_Fustrap (UINT16 /*FUSE_ADDRESS_START_T*/  fuse_address, UINT16 /*FUSE_LENGTH_T*/  fuse_length,   FUSE_ECC_T fuse_ecc, FUSE_ARRAY_T array, UINT8* value);

#endif /* _FUSE_WRAPPER_ */
