/*--------------------------------------------------------------------------
CH554.H
Header file for CH554 microcontrollers.
****************************************
**  Copyright  (C)  W.ch  1999-2014   **
**  Web:              http://wch.cn   **
****************************************
--------------------------------------------------------------------------*/

#ifndef __BASE_TYPE__
#define __BASE_TYPE__

/*----- constant and type define -----------------------------------------*/

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif

#ifndef BOOL
typedef bit BOOL;
#endif
#ifndef UINT8
typedef unsigned char UINT8;
#endif
#ifndef UINT16
typedef unsigned short UINT16;
#endif
#ifndef UINT32
typedef unsigned long UINT32;
#endif
#ifndef UINT8D
typedef unsigned char data UINT8D;
#endif
#ifndef UINT16D
typedef unsigned short data UINT16D;
#endif
#ifndef UINT32D
typedef unsigned long data UINT32D;
#endif
#ifndef UINT8I
typedef unsigned char idata UINT8I;
#endif
#ifndef UINT16I
typedef unsigned short idata UINT16I;
#endif
#ifndef UINT32I
typedef unsigned long idata UINT32I;
#endif
#ifndef UINT8X
typedef unsigned char xdata UINT8X;
#endif
#ifndef UINT16X
typedef unsigned short xdata UINT16X;
#endif
#ifndef UINT32X
typedef unsigned long xdata UINT32X;
#endif
#ifndef UINT8V
typedef unsigned char volatile UINT8V;
#endif
#ifndef UINT8DV
typedef unsigned char volatile data UINT8DV;
#endif
#ifndef UINT8XV
typedef unsigned char volatile xdata UINT8XV;
#endif
#ifndef UINT8PV
typedef unsigned char volatile pdata UINT8PV;
#endif
#ifndef UINT8C
typedef const unsigned char code UINT8C;
#endif
#ifndef PUINT8
typedef unsigned char *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long *PUINT32;
#endif
#ifndef PUINT8I
typedef unsigned char idata *PUINT8I;
#endif
#ifndef PUINT16I
typedef unsigned short idata *PUINT16I;
#endif
#ifndef PUINT32I
typedef unsigned long idata *PUINT32I;
#endif
#ifndef PUINT8X
typedef unsigned char xdata *PUINT8X;
#endif
#ifndef PUINT16X
typedef unsigned short xdata *PUINT16X;
#endif
#ifndef PUINT32X
typedef unsigned long xdata *PUINT32X;
#endif
#ifndef PUINT8V
typedef unsigned char volatile *PUINT8V;
#endif
#ifndef PUINT8DV
typedef unsigned char volatile data *PUINT8DV;
#endif
#ifndef PUINT8XV
typedef unsigned char volatile xdata *PUINT8XV;
#endif
#ifndef PUINT8PV
typedef unsigned char volatile pdata *PUINT8PV;
#endif
#ifndef PUINT8C
typedef const unsigned char code *PUINT8C;
#endif

#ifndef STRUCT_OFFSET
#define STRUCT_OFFSET(s, m)                                                    \
  ((UINT8)(&(((s) *)0)->(                                                      \
      m))) /* get the offset address for a member of a structure */
#endif

#endif // __BASE_TYPE__
