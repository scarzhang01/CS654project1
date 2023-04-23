#include "types.h"
#include "uart.h"

inline void uart2Init(unsigned long baud, unsigned int byteformat, unsigned int mode)
{
	/* Stop UART port */
	U2MODEbits.UARTEN = 0;
	
	/* Disable Interrupts */
	IEC1bits.U2RXIE = 0;
	IEC1bits.U2TXIE = 0;
	
	/* Clear Interrupt flag bits */
	IFS1bits.U2RXIF = 0;
	IFS1bits.U2TXIF = 0;
		
	/* Set IO pins */
	TRISFbits.TRISF4 = 1;	// RX Pin
	TRISFbits.TRISF5 = 0;	// TX Pin
	if (mode) {
		TRISFbits.TRISF12 = 1;	// CTS Pin
		TRISFbits.TRISF13 = 0;	// RTS Pin
	}
	
	/* baud rate */
	U2BRG  =82; // (2500000ul / baud) - 1; // With BRGH = 0
	//U2BRG = 21; // For 115200 with Fcy = 40MHz
	
	/* Operation settings and start port */
	U2MODE = 0;
	U2MODEbits.BRGH = 0;
	//Start port
	U2MODEbits.UARTEN = 1;
	if (mode) {	// Flow Control Mode
		U2MODEbits.RTSMD = 1;
		U2MODEbits.UEN = 2;
	} else {	// Simple Mode
		U2MODEbits.RTSMD = 0;
		U2MODEbits.UEN = 0;	
	}
	U2MODE |= byteformat & 0x07; // Number of bit, Parity and Stop bits

	/* TX & RX interrupt modes */
	U2STA = 0;
	U2STAbits.UTXEN = 1;
	int wait =0;
	while(wait < 9600)
		wait++;	

}

//***UART2 Receive Functions***//

inline uint8 uart2RxComplete(void) {
	return(U2STAbits.URXDA);
}

inline uint16 uart2Read(void) {
	while(uart2RxComplete() == 0);	
	if(U2MODEbits.PDSEL == 3)
        return (U2RXREG);
    else
        return (U2RXREG & 0xFF);
}

//Reads to data-width union

inline BYTE uart2ReadBYTE(void) {
	BYTE data;
	data.byte[0] = (uint8) uart2Read();
	return data;
}

inline WORD uart2ReadWORD(void){
	WORD data;
	uart2ReadBlock(&(data.byte[0]), 2);
	return data;
}

inline DWORD uart2ReadDWORD(void) {
	DWORD data;
	uart2ReadBlock(&(data.byte[0]), 4);
	return data;
}

inline QWORD uart2ReadQWORD(void) {
	QWORD data;
	uart2ReadBlock(&(data.byte[0]), 8);
	return data;
}

//Reads specific dataTypes

inline uint8 uart2ReadByte(void) {
	BYTE retVal = uart2ReadBYTE();
	return retVal.uByte;
}

inline int8 uart2ReadSignedByte(void) {
	BYTE retVal = uart2ReadBYTE();
	return retVal.sByte;
}

inline uint16 uart2ReadWord(void) {
	WORD retVal = uart2ReadWORD();
	return retVal.uWord;
}

inline int16 uart2ReadSignedWord(void) {
	WORD retVal = uart2ReadWORD();
	return retVal.sWord;
}

inline uint32 uart2ReadDWord(void) {
	DWORD retVal = uart2ReadDWORD();
	return retVal.uDWord;
}

inline int32 uart2ReadSignedDWord(void) {
	DWORD retVal = uart2ReadDWORD();
	return retVal.sDWord;
}

inline float uart2ReadFloat(void) {
	DWORD retVal = uart2ReadDWORD();
	return retVal.sFloat;
}

inline double uart2ReadDouble(void) {
	DWORD retVal = uart2ReadDWORD();
	return retVal.sDouble;
}

inline uint64 uart2ReadQWord(void) {
	QWORD retVal = uart2ReadQWORD();
	return retVal.uQWord;
}

inline int64 uart2ReadSignedQWord(void) {
	QWORD retVal = uart2ReadQWORD();
	return retVal.sQWord;
}

inline long double uart2ReadLongDouble(void) {
	QWORD retVal = uart2ReadQWORD();
	return retVal.lDouble;
}

void uart2ReadLine(char *mem, uint16 maxSize) {
	char byte = 0xFF;
	while ((maxSize > 0) && (byte != NEW_LINE)) {
		byte = (char) uart2Read();
		*mem = byte;
		mem++;
		maxSize--;
	}
}
	
void uart2ReadString(char *mem, uint16 maxSize){
	char byte = 0xFF;
	while ((maxSize > 0) && (byte != NULL)) {
		byte = (char) uart2Read();
		*mem = byte;
		mem++;
		maxSize--;
	}
}

void uart2ReadBlock(unsigned char *mem, uint16 size) {
	unsigned char* stop = mem;
	stop = stop + size;
	while (mem < stop) {
		*mem = (char) uart2Read();
		mem++;
	}
}
		
//***UART2 Write Functions***//

inline void uart2Write(uint16 data) {
	while(!uart2TxReady());
    if(U2MODEbits.PDSEL == 3)
        U2TXREG = data;
    else
        U2TXREG = (data & 0xFF);  
	while(!uart2TxComplete());
}

inline uint8 uart2TxReady() {
	return (!U2STAbits.UTXBF);
}

inline uint8 uart2TxComplete() {
	return U2STAbits.TRMT;
}

//Writes from a data-width union

inline void uart2WriteBYTE(BYTE data) {
	uart2Write(data.uByte);
}

inline void uart2WriteWORD(WORD data){
	uart2WriteBlock(&(data.byte[0]), 2);
}

inline void uart2WriteDWORD(DWORD data) {
	uart2WriteBlock(&(data.byte[0]), 4);
}

inline void uart2WriteQWORD(QWORD data) {
	uart2WriteBlock(&(data.byte[0]), 8);
}

//Writes specific dataTypes

inline void uart2WriteByte(uint8 data) {
	uart2Write(data);
}

inline void uart2WriteSignedByte(int8 data) {
	uart2Write(data);
}

inline void uart2WriteWord(uint16 data) {
	WORD word;
	word.uWord = data;
	uart2WriteWORD(word);
}

inline void uart2WriteSignedWord(int16 data) {
	WORD word;
	word.sWord = data;
	uart2WriteWORD(word);
}

inline void uart2WriteDWord(uint32 data) {
	DWORD dword;
	dword.uDWord = data;
	uart2WriteDWORD(dword);
}

inline void uart2WriteSignedDWord(int32 data) {
	DWORD dword;
	dword.sDWord = data;
	uart2WriteDWORD(dword);
}

inline void uart2WriteFloat(float data) {
	DWORD dword;
	dword.sFloat = data;
	uart2WriteDWORD(dword);
}

inline void uart2WriteDouble(double data) {
	DWORD dword;
	dword.sDouble = data;
	uart2WriteDWORD(dword);
}

inline void uart2WriteQWord(uint64 data) {
	QWORD qword;
	qword.sQWord = data;
	uart2WriteQWORD(qword);
}

inline void uart2WriteSignedQWord(int64 data) {
	QWORD qword;
	qword.sQWord = data;
	uart2WriteQWORD(qword);
}

inline void uart2WriteLongDouble(long double data) {
	QWORD qword;
	qword.lDouble = data;
	uart2WriteQWORD(qword);
}

void uart2WriteLine(char *mem) {
	while (*mem != NEW_LINE) {
		uart2Write(*mem);
		mem++;
	}
}
	
void uart2WriteString(char *mem){
	while (*mem != NULL) {
		uart2Write(*mem);
		mem++;
	}
}

void uart2WriteBlock(unsigned char *mem, uint16 size) {
	unsigned char *stop = mem + size;
	while (mem < stop) {
		uart2Write(*mem);
		mem++;
	}
}
