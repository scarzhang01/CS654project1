/*
 * lcd.c
 *
 * Adapted for CS Embedded System Lab - FLEX UI board from:
 * http://sourceforge.net/projects/serialglcdlib/
 *
 * Adapted by: N. Edwards - 3 Aug, 2011 - U of Illinois
 *
 * Modified by: Bach Bui - 11 Aug, 2011

SUMMARY:
		A library for use with Sparkfuns Serial Graphic LCD 128x64 
		(http://www.sparkfun.com/products/9351). Flashed with this 
		firmware: (https://sourceforge.net/projects/serialglcd/). 
		By a beginner, for beginners.
 */


#include "lcd.h"
#include <uart.h>
#define FCY 12800000UL 
#include <libpic30.h>

void lcd_initialize(void){
	OpenUART1(0x8000,0x400,6);
	__delay_ms(20);
}

void lcd_send(uint8_t const data){
	while (U1STAbits.UTXBF) Nop();
	U1TXREG = (uint8_t)(data); 
	while(!U1STAbits.TRMT) Nop();
    
///* DEBUG - Switch to UART2/PC connection */
//	while (U2STAbits.UTXBF) Nop();
//	U2TXREG = (uint8_t)(data); 
//	while(!U2STAbits.TRMT) Nop();
//
	/* Blocking implementation, would be better if interrupt driven */
	//while (U1STA & (1<<UTXBF)) Nop();
	//U1TXREG = (uint8_t)(data); 
	//while  ((U1STA & (1<<TRMT)) >>TRMT) Nop();
}

void lcd_send_str(uint8_t const data[], uint8_t chars){
	uint8_t i = 0;
	while (i < chars){
		lcd_send(data[i]);
		i++;
	}
}

void lcd_send_cmd(uint8_t const cmd){
		lcd_send(0x7c);
		lcd_send(cmd);
}

/*
 *************************************************************************
 *************************************************************************
 *  BEGIN ADAPTED CODE FROM serialGLCDlib 
 *  http://sourceforge.net/projects/serialglcdlib/
 *************************************************************************
 *************************************************************************
 */

void clearLCD(void) 
{   /* Clears the display, eraseBlock is probably faster if you dont need to erase the entire screen.
	 Example: lcd.clearLCD();
	 */  
	lcd_send(0x7C);
	lcd_send(0x00);
}

void setDebug(int l)
{	/*
	 0 = no messages
	 1 = binary messages, the display will echo a byte = command when the command is started and a byte = 0x00 when the command is finished
	 2 = text messages, useful when using a terminal. will echo "Entered Command n" when command n is started
	 and will echo "Exited Command n, RX_buffer=y" when the command is done, and will report how many bytes are in the recieve buffer
	 Example: lcd.setDebug(1);
	 */
	lcd_send(0x7C);
	lcd_send(0x01);
	lcd_send(l);
}

void backLight(int x) 
{   /* Sets backlight duty cycle. 0-100, 0=off.
	 Example: lcd.backLight(50);
	 */
	lcd_send(0x7C);
	lcd_send(0x02);
	lcd_send(x);
}

void drawCircle(int x, int y, int r, int z) 
{   /* Draws a circle at (x,y) with radius r, draw/erase 
	 Example: lcd.drawCircle(30,30,10,1);
	 */
	lcd_send(0x7C);
	lcd_send(0x03);
	lcd_send(x);
	lcd_send(y);
	lcd_send(r);
	lcd_send(z);
}

void toggleCRLF(void)
{	/* Toggles CR/LF. each time this command is sent, 
	 wheather or not a CR automatically executes a LF is toggled. 
	 this is saved to EEPROM and is persistant over power cycles.
	 Example: lcd.toggleCRLF();
	 */
	lcd_send(0x7C);
	lcd_send(0x04);
}

void eraseBlock(int x1, int y1, int x2, int y2) 
{	/* Draws a block on the screen with clear or set pixels (depending on if the screen is inverted or not)
	 Example: lcd.eraseBlock(10,10,20,20);
	 */
	lcd_send(0x7C);
	lcd_send(0x05);
	lcd_send(x1);
	lcd_send(y1);
	lcd_send(x2);
	lcd_send(y2);
}

void resetLCD(void) 
{	/* Resets the LCD, clears the screen and resets x,y offsets to 0,0
	 Example: lcd.resetLCD();
	 */
	lcd_send(0x7C);
	lcd_send(0x06);
}

void changeBaud(int x)
{	/* Changes the baudrate.
	 1=4800
	 2=9600
	 3=19200
	 4=38400
	 5=57600
	 6=115200
	 Persistant over power-cycles. Transmitting to the screen during splash resets the baudrate to 115200
	 Example: lcd.changeBaud(2); // (9600)
	 */
	if(x<1) {
		x=1;
	}
	if(x>6) {
		x=6;
	}
	lcd_send(0x7C);
	lcd_send(0x07);
	lcd_send(x);
}

void toggleFont(void)
{	/* Toggles between the standard and AUX font. Only affects text written after the command.
	 Example: lcd.toggleFont();
	 */
	lcd_send(0x7C);
	lcd_send(0x08);
}

void setFontMode(int x)
{	/* Sets the mode for which text is written to the text.
	 0= AND
	 1= NAND
	 2= OR
	 3= NOR
	 4= XOR
	 5= NXOR
	 6= don't uses this one, will result in corrupted characters
	 7= COPY (this is the default, overwrites whatever is in the background, and is the only one that respects reverse)
	 Example: lcd.setFontMode(2); // (AND)
	 */
	lcd_send(0x7C);
	lcd_send(0x0A);
	lcd_send(x);
}

void drawSprite(int x, int y, int n, int mode)
{	/* Draws a sprite saved in the backpack. x and y sets the upper left corner, 
	 n is the number of the stored sprite, n sets the mode (same modes ad text).
	 sprite 0 is the sparkfun logo, other uploaded sprites will be deleted if power is removed.
	 Example: lcd.drawSprite(10,10,0,7); // Draws the sparkfun logo
	 */
	lcd_send(0x7C);
	lcd_send(0x0B);
	lcd_send(x);
	lcd_send(y);
	lcd_send(n);
	lcd_send(mode);
}

void drawLine(int x1, int y1, int x2, int y2, int z) 
{	/* Draws a line from x1,y1 to x2,y2. z=1 to draw, z=0 to erase.
	 Example: lcd.drawLine(10,10,20,20,1); 
	 */
	lcd_send(0x7C);
	lcd_send(0x0C);
	lcd_send(x1);
	lcd_send(y1);
	lcd_send(x2);
	lcd_send(y2);
	lcd_send(z);
}

void uploadSprite(int n, int w, int h, uint8_t *data)
{	/* Uploads a sprite to the backpack, data must be 32 bytes long. 8 sprites can be stored (0-7).
	 The sparkfun logo is number 0, can be overwritten but reverts when power is removed. 
	 All uploaded sprites will be erased upon removal of power
	 Consult with firmware README for more info
	 Example: byte logo[] = {
	 0x80,0xc0,0x40,0x0c,0x3e,0xfe,0xf2,0xe0,0xf0,0xe0,
	 0xff,0x7f,0x3f,0x1f,0x1f,0x1f,0x1f,0x0f,0x07,0x03,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00};
	 lcd.uploadSprite(7,10,16,logo); // Uploads the sparkfun logo as sprite number 7
	 */
	int x;

	lcd_send(0x7C);
	lcd_send(0x0D);
	lcd_send(n);
	lcd_send(w);
	lcd_send(h);;
	for(x = 0; x < 32;x++)
	{
		lcd_send(data[x]);
	}
}

void drawBox(int x1, int y1, int x2, int y2, int z) 
{	/* Draws a rectangle starting from x1,y1 to x2,y2. z=1 for draw, z=0 for erase.
	 Example: lcd.drawBox(10,10,20,20,1);
	 */
	lcd_send(0x7C);
	lcd_send(0x0F);
	lcd_send(x1);
	lcd_send(y1);
	lcd_send(x2);
	lcd_send(y2);
	lcd_send(z);
}

void togglePixel(int x, int y, int z) 
{     /* Toggles a pixel. x and y for coord, z=1 sets, z=0 erases.
	   Example: lcd.togglePixel(30,30,1);
	   */
	lcd_send(0x7C);
	lcd_send(0x10);
	if(x > 128) {
		lcd_send(128);
	}
	else if(x < 0) {
		lcd_send(0);
	}
	else {
		lcd_send(x);
	}
	if(y > 64) {
		lcd_send(64);
	}
	else if(y < 0) {
		lcd_send(0);
	}
	else {
		lcd_send(y);
	}
	if(z != 1 && z != 0) {
		lcd_send(1);
	}
	else {
		lcd_send(z);
	}
}
void drawFilledBox(int x1, int y1, int x2, int y2, uint8_t fill)
{	/* Same as drawBox, but accepts a fill byte. 0xFF for black, 0x00 for white. 
	 Other values can be used to create patterns (like 0x55). 
	 Example: drawFilledBox(10,10,20,20,0xFF);
	 */
	lcd_send(0x7C);
	lcd_send(0x12);
	lcd_send(x1);
	lcd_send(y1);
	lcd_send(x2);
	lcd_send(y2);
	lcd_send(fill);
	
}

void reverseColor() 
{	/* Reverses the "color" (black on white / white on black)
	 Example: lcd.reverseColor();
	 */
	lcd_send(0x7C);
	lcd_send(0x14);
}

void toggleSplash(void)
{	/* Toggles the sparkfun logo during boot.
	 Example: lcd.toggleSplash();
	 */
	lcd_send(0x7C);
	lcd_send(0x15);
}
void drawData(int x, int y, int mode, int w, int h, uint8_t *data)
{	/* Allows you to draw graphics to the screen like sprites, but the data doesn't have to be uploaded first, and there is no size
	 restrictions.(other than the drawable area of the display). 
	 Consult with firmware README for more info
	 Example:
	 byte data[]={0x00,0x12.....};
	 lcd.drawData(0,0,4,128,64,data); 
	 */
	lcd_send(0x7C);
	lcd_send(0x16);
	lcd_send(x);
	lcd_send(y);
	lcd_send(mode);
	lcd_send(w);
	lcd_send(h);
	int length = w*h/8;
	for(x = 0; x < length; x++) {
		lcd_send(data[x]);
	}
}

void gotoPosition(int x, int y) 
{  	/* Sets the x and y offsets, text written after this command will start from x,y.
	 Example: lcd.gotoPosition(40,40);
	 */
	lcd_send(0x7C);
	lcd_send(0x18);
	lcd_send(x);
	lcd_send(0x7C);
	lcd_send(0x19);
	lcd_send(y);
}

void gotoLine(int line)
{	/* Uses the gotoPosition function to select "line" 1-8 on the display. 
	 Text can be written between these lines using gotoPosition. This function makes it simpler.
	 Example: lcd.gotoLine(2);
	 */
	int y;
	if(line > 8) {
		line = 8;
	}
	else if(line < 1) {
		line = 1;
	}
	else {
		y = -8 + line * 8;
		gotoPosition(1,y);
	}
}
