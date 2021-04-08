/******************************************************************************
    Copyright:  © 2008 Nuvoton (TA)

	created:	Oct 19, 2004   14:43
	filename: 	tl_common.h
	author:		Avi Fishman
	
	purpose:	Common definition
******************************************************************************/

#ifndef __tl_common_h__
#define __tl_common_h__

#if defined(NULL)
#undef NULL
#endif

#define	NULL 0

#if defined(BYTE)
#undef BYTE
#endif

typedef unsigned char BYTE;

#if defined(WORD)
#undef WORD
#endif

typedef unsigned short WORD;

#if defined(DWORD)
#undef DWORD
#endif

typedef unsigned int DWORD;

#ifdef WIN32

#if defined(QWORD)
#undef QWORD
#endif

#if (_MSC_VER >= 1500)
	typedef unsigned long long QWORD;
#endif
#endif

#if defined(BOOL)
#undef BOOL
#endif

typedef int BOOL;

#if defined(WINAPI)
#undef WINAPI
#endif

#define WINAPI __stdcall


#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif


#define	TEST_PASS	0
#define	TIMEOUT		1
#define	ZBD_ERR		2
#define INTERNAL_ERROR	3
#define	TEST_FAIL	-1

#ifndef UNIT_SIZE_DEF
#define UNIT_SIZE_DEF
enum UNIT_SIZE {BYTE_SIZE=1, WORD_SIZE=2, DWORD_SIZE=4} ;
#endif

#define	LOG_MSG()

#endif //__tl_common_h__
