#ifndef __INCLUDE_UART_H
#define __INCLUDE_UART_H

#include <p33Fxxxx.h>
#include "types.h"

//***Constants***//

#define UART_ERR 		-1
#define UART_SUC 		0

#define BIT9			0x06
#define BIT8_ODD		0x04
#define BIT8_EVEN		0x02
#define BIT8_NO			0x00

#define BIT_STOP_1		0x00
#define BIT_STOP_2		0x01

#define CTRL_SIMPLE		0x00
#define CTRL_FLOW		0x01

#define TX_INT_SINGLE	0x8000
#define TX_INT_EMPTY	0x2000
#define TX_INT_LAST		0x0000

#define RX_INT_FULL		0x00C0
#define RX_INT_3OF4		0x0080
#define RX_INT_SINGLE	0x0040

//***UART2 Functions***//

inline void uart2Init(unsigned long baud, unsigned int byteformat, unsigned int mode);

//***UART2 Read Functions***//

//Basic Read/Util Functions
inline uint8 uart2RxComplete(void);
inline uint16 uart2Read(void);

//Reads to data-width union
inline BYTE uart2ReadBYTE(void); 
inline WORD uart2ReadWORD(void);
inline DWORD uart2ReadDWORD(void);
inline QWORD uart2ReadQWORD(void);

//Reads specific dataTypes
inline uint8 uart2ReadByte(void);
inline int8 uart2ReadSignedByte(void);
inline uint16 uart2ReadWord(void);
inline int16 uart2ReadSignedWord(void);
inline uint32 uart2ReadDWord(void);
inline int32 uart2ReadSignedDWord(void);
inline float uart2ReadFloat(void);
inline double uart2ReadDouble(void);
inline uint64 uart2ReadQWord(void);
inline int64 uart2ReadSignedQWord(void);
inline long double uart2ReadLongDouble(void);

//Block Reads
void uart2ReadLine(char *mem, uint16 maxSize);
void uart2ReadString(char *mem, uint16 maxSize);
void uart2ReadBlock(unsigned char *mem, uint16 size);
		
//***UART2 Write Functions***//

//Basic Read/Util Functions
inline void uart2Write(uint16 data);
inline uint8 uart2TxReady(void);
inline uint8 uart2TxComplete(void);

//Writes from a data-width union
inline void uart2WriteBYTE(BYTE data);
inline void uart2WriteWORD(WORD data);
inline void uart2WriteDWORD(DWORD data);
inline void uart2WriteQWORD(QWORD data);

//Writes specific dataTypes
inline void uart2WriteByte(uint8 data);
inline void uart2WriteSignedByte(int8 data);
inline void uart2WriteWord(uint16 data);
inline void uart2WriteSignedWord(int16 data);
inline void uart2WriteDWord(uint32 data);
inline void uart2WriteSignedDWord(int32 data);
inline void uart2WriteFloat(float data);
inline void uart2WriteDouble(double data);
inline void uart2WriteQWord(uint64 data);
inline void uart2WriteSignedQWord(int64 data);
inline void uart2WriteLongDouble(long double data);

//Block write functions
void uart2WriteLine(char *mem);
void uart2WriteString(char *mem);
void uart2WriteBlock(unsigned char *mem, uint16 size);

#endif
