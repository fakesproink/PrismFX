#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "kidbright32.h"
#include "lcd_spi.h"
#include "font_5x7.h"
#include "font_9x15.h"
#include "font3.h"		// Any GFX font, with slight modifications

// 26 is out OUT1, 27 is OUT2, 0 is default
#define CS_PIN	GPIO_NUM_0
const uint16_t BUFFER_SIZE = 32;	// max number of bytes that can sent with spi->write

// ST7789 initialization commands and data

static const char cmd0[] = {0x36,0x00};		// MADCTL Memory data access control
static const char cmd1[] = {0x3a,0x05};		// COLMOD interface pixel format
static const char cmd2[] = {0xb2,0x0c,0x0c,0x00,0x33,0x33};	// PORCTRL Porch control
static const char cmd3[] = {0xb7,0x35};		// GCTRL gate control
static const char cmd4[] = {0xbb,0x19};		// VCOMS VCOM setting
static const char cmd5[] = {0xc0,0x2c};		// LCMCTRL LCM control
static const char cmd6[] = {0xc2,0x01};		// VDVVRHEN VDV and VRH command
static const char cmd7[] = {0xc3,0x12};		// VRHS VRH Set
static const char cmd8[] = {0xc4,0x20};		// VDVSET VDV Setting
static const char cmd9[] = {0xc6,0x0f};		// FRCTR2 FR Control 2
static const char cmda[] = {0xd0,0xa4,0xa1};// PWCTRL1 Power Control 1
											// PVGAMCTRL Positive Voltage Gamma Control
static const char cmdb[] = {0xe0,0xD0,0x04,0x0D,0x11,0x13,0x2B,0x3F,0x54,0x4C,0x18,0x0D,0x0B,0x1F,0x23};
											// GAMCTRL Negative Voltage Gamma Control
static const char cmdc[] = {0xe1,0xD0,0x04,0x0C,0x11,0x13,0x2C,0x3F,0x44,0x51,0x2F,0x1F,0x1F,0x20,0x23};
static const char cmdd[] = {0x21};			// INVON Display inversion on
static const char cmde[] = {0x11};			// SLPOUT Sleep out (wake up)

static const char *initST7789[] = {cmd0, cmd1, cmd2, cmd3, cmd4, cmd5, cmd6, cmd7, cmd8, cmd9, cmda, cmdb, cmdc, cmdd, cmde};
static const char cmdSize[]={sizeof(cmd0),sizeof(cmd1),sizeof(cmd2),sizeof(cmd3),sizeof(cmd4),sizeof(cmd5),sizeof(cmd6),sizeof(cmd7),sizeof(cmd8),sizeof(cmd9),sizeof(cmda),sizeof(cmdb),sizeof(cmdc),sizeof(cmdd),sizeof(cmde)};

// INITIALIZATION OF properties

LCD_SPI::LCD_SPI(int dev_addr) {
	channel = 0;
	address = dev_addr;
	printf("new LCD_SPI object %p\n", this);
}

void LCD_SPI::init(void) {
	printf("init %02x %p\n", address, this);
	state = s_detect;
	mcpPort = MCP23S17_REG_GPIOA;		// varies by display if multiple displays
	acmd = 0xcf;						// which MCP23S17 IOs are connected to the display
	adat = 0xdf;
	arst = 0xf7;

	backC = 0;		// black
	foreC = 0xffff; // white
	colS = rowS = colM = rowM = display_rotation = 0;
		
// these are used to switch GPIO0 to an input or output, switch to input to disable it		
	io_conf_0in.intr_type = GPIO_INTR_DISABLE; // disable interrupt
	io_conf_0in.mode = GPIO_MODE_INPUT; // set as input mode
	io_conf_0in.pin_bit_mask = 1ULL; // pin 0 bit mask for GPIO0
	io_conf_0in.pull_down_en = GPIO_PULLDOWN_DISABLE; // disable pull-down mode
	io_conf_0in.pull_up_en = GPIO_PULLUP_ENABLE; // disable pull-up mode

	io_conf_0out.intr_type = GPIO_INTR_DISABLE; // disable interrupt
	io_conf_0out.mode = GPIO_MODE_OUTPUT; // set as output mode
	io_conf_0out.pin_bit_mask = 1ULL; // pin 0 bit mask for GPIO0
	io_conf_0out.pull_down_en = GPIO_PULLDOWN_DISABLE; // disable pull-down mode
	io_conf_0out.pull_up_en = GPIO_PULLUP_DISABLE; // disable pull-up mode

	initialized = true;
}

// HARDWARE OUTPUT FUNCTIONS (There are no hardware input functions)

esp_err_t LCD_SPI::wrMCP(uint8_t reg, uint8_t val){		// configures the MCP23S17 output for the St7789 RST, DAT, & CS pins
	esp_err_t rc;
	uint8_t data[3];
	data[0] = address << 1;
	data[1] = reg;
	data[2] = val;
	CMD_SET = val == acmd;
/*
The KB software configures pin 0 as the CS pin for SPI. 
CS is asserted whether we are accessing the MCP23S17 GPIO chip or the ST7789 Display
Having both chips selected at the same time is very bad. It won't work.
Solutions
1.	Use out1 or out2, pins 26 or 27, for CS and leave pin 0 unconnected.
	This works well but may break user projects that are using the selected output pin.
2.	Change pin 0 to an input when we want to access the ST7789 Display.
	Seems to work but needs more testing with other SPI devices.

Neither of these solutions is faster than the other. drawPixel takes about same time either way.
*/
#if CS_PIN == 0
	gpio_config(&io_conf_0out);	// change pin 0 an output
#endif
	gpio_set_level(CS_PIN, CS_PIN==26 || CS_PIN==27);	// invert for OUT1 (26) and OUT2 (27)
	rc = spi->write(channel, address, data, 3);
// the MCP23S017 PAx pins change after 24us, but spi-write returns after 80 us ???
	gpio_set_level(CS_PIN, CS_PIN!=26 && CS_PIN!=27);
#if CS_PIN == 0
	if(val != 0xff)		// if true then there is no intention of writing to the ST7789 display
		gpio_config(&io_conf_0in);	// change pin 0 to an input fo ST7789 writes
#endif
	return rc;
}

// these next 3 functions are for SPI writes to the ST7789 
void LCD_SPI::writecommand(uint8_t cmd){	// sets up ST7789 pins using the MCP23S17, then send command to ST7789
	if(!CMD_SET)	wrMCP(mcpPort, acmd);	// if C/D control is not already set, then set it
	spi->write(channel, address, &cmd, 1);	// send the command byte
}

void LCD_SPI::writedata(uint8_t data){		// sets up ST7789 pins using the MCP23S17, then 1 byte of data to ST7789
	if( CMD_SET)	wrMCP(mcpPort, adat);	// if C/D conntrol is not set to D, then set it so
 	spi->write(channel, address, &data, 1);	// send the data byte
}

void LCD_SPI::sendCnD(uint8_t cmd, uint8_t * data, uint16_t dCnt){
	if(!CMD_SET)	wrMCP(mcpPort, acmd);	// DC lo, CS lo
	spi->write(channel, address, &cmd, 1);	// cmd
	if( CMD_SET)	wrMCP(mcpPort, adat);	// DC hi, CS lo
	spi->write(channel, address, data, dCnt);// data
}

// sets up ST7789 to receive bulk data, 2 * (1+x1-x0) * (1+y1-y0) bytes at specific coordinates
// working to optimize this function as it is called often, 1x/point, 1x/character and the hardware is slow
void LCD_SPI::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	if(x1 > x2){
		uint16_t temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if(y1 > y2){
		uint16_t temp = y1;
		y1 = y2;
		y2 = temp;
	}
	if(display_rotation == 3){	x1 += 80;	x2 += 80;	}	// pins on the right
	if(display_rotation == 2){	y1 += 80;	y2 += 80;	}	// pins on the bottom

	uint8_t buf[4], cmdCol = 0x2a, cmdRow = 0x2b, cmdColor = 0x2c;	// st7789 command bytes
	buf[0] = x1>>8; buf[1] = x1&255; buf[2] = x2>>8; buf[3] = x2&255;	// vertical bounds
	sendCnD(cmdCol, buf, 4);
	buf[0] = y1>>8; buf[1] = y1&255; buf[2] = y2>>8; buf[3] = y2&255;	// horizontal bounds
	sendCnD(cmdRow, buf, 4);
	if(!CMD_SET)	wrMCP(mcpPort, acmd);	// DC lo, CS lo
	spi->write(channel, address, &cmdColor, 1);	// now prepared to receive color data
}

void LCD_SPI::bufferOut(uint8_t *bufr, uint16_t sz){	// for sending bulk data in BUFFER_SIZE chunks
	if( CMD_SET)	wrMCP(mcpPort, adat);  				// DC hi, CS lo
	while(sz > BUFFER_SIZE){							// for all BUFFER_SIZE chunks
		spi->write(channel, address, bufr, BUFFER_SIZE);
		bufr += BUFFER_SIZE;
		sz -= BUFFER_SIZE;
	}
	if(sz)												// remainder of buffer, if any
		spi->write(channel, address, bufr, sz);
	wrMCP(mcpPort, 0xff);      							// rst, dc, & cs all high
}

// Geometric Functions: Point, Line, Circle, Rectange, Bitmap, maybe triangle?

void LCD_SPI::drawPixel(uint16_t x, uint16_t y, uint16_t color){
	if(x > _W || y > _H)  return;		// when cropping a single pixel, the result is a pixel or no pixel
	setAddrWindow(x, y, x, y);
	uint8_t buf[2];	buf[0] = color>>8;	buf[1] = color & 255;
	bufferOut(buf, sizeof(buf));
}

void LCD_SPI::drawLineH(uint16_t x, uint16_t y, uint16_t w, uint16_t color){
	uint8_t buf[w*2], *p=buf;
	for(int i=0; i<w; i++){
		*p++ = color >> 8;
		*p++ = color & 255;
	}
	setAddrWindow(x, y, x+w, y);		// wide, short rectangle
	bufferOut( buf, sizeof(buf));
}

void LCD_SPI::drawLineV(uint16_t x, uint16_t y, uint16_t h, uint16_t color){
	uint8_t buf[h*2], *p=buf;	// max size will be 240x2
	for(int i=0; i<h; i++){
		*p++ = color >> 8;
		*p++ = color & 255;
	}
	setAddrWindow(x, y, x, y+h);		// tall, thin rectangle 
	bufferOut( buf, sizeof(buf));
}

void LCD_SPI::drawLine(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t color){
	if(x == x1)	drawLineV(x, y, y1-y+1, color);		// draw FAST vertical line
	if(y == y1)	drawLineH(x, y, x1-x+1, color);		// draw FAST horizontal line
// TODO draw an oblique line}
}

// TODO drawCircle
// TODO drawBitmap
// TODO drawTriangle, maybe?? code exists from buydisplay.com but how useful is it?

void LCD_SPI::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, bool fill){
	if(fill){
		uint8_t bufr[BUFFER_SIZE], *p = bufr;		// max number of bytes that can be sent over SPI bus
		uint16_t i=(1+x2-x1)*(1+y2-y1);
		setAddrWindow(x1, y1, x2, y2);
		wrMCP(mcpPort, adat);  // dc high, cs active (low)
		while(i--){
			*p++ = color >> 8;
			*p++ = color & 255;
			if(p-bufr == BUFFER_SIZE){	// size = difference of two pointers
				spi->write(channel, address, bufr, BUFFER_SIZE);
				p = bufr;
			}
		}
		if(p-bufr)	spi->write(channel, address, bufr, p-bufr);
		wrMCP(mcpPort, 0xff);        // rst, dc, & cs all high
	}else{
		drawLineH(x1,y1,1+x2-x1,color);
		drawLineH(x1,y2,1+x2-x1,color);
		drawLineV(x1,y1,1+y2-y1,color);
		drawLineV(x2,y1,1+y2-y1,color);
	}
}

// Character and string functions

void LCD_SPI::drawChar5x7(char character){
	uint8_t buf[96], *p = buf;
	uint16_t i, j, k, c, index = (character & 0xff)*5;

	if(foreC == backC){			// transparent printing. Just write pixels over the background
		if( character != '\n'){
			for(i=0; i<5; i++){
				k = font_5x7[index + i];
				for(j=0; j<8; j++)
					if(k & (1<<j))
						drawPixel(colS*6+i, rowS*8+j, foreC);
			}
			colS++;				// advance column position
			if(colS<40)	return;	// return if not wrapping
		}
	}else{						// fill background and foreground
		if(character == '\n'){	// if a newline
			for( i=0; i<8; i++)	// clear to the end of the line
				drawLineH(colS*6, rowS*8+i, 240-colS*6, backC);
		}else{					// printable character
			for(j=0; j<8; j++){								// for each line
				for(i=0; i<6; i++){							// for each column
					k = i<5 ? font_5x7[index + i] : 0;		// last column is inter character gap, all bits off
					c = (k & (1<<j)) ? foreC : backC;		// if bit set then use foreground color, else background
					*p++ = c >> 8;							// put appropriate color in buffer
					*p++ = c & 255;
				}
			}
			setAddrWindow(colS*6, rowS*8, colS*6+5, rowS*8+7);	
			bufferOut(buf, sizeof(buf));					// write the character to the display
			colS++;				// advance column position
			if(colS<40)	return;	// return if not wrapping
		}
	}
	colS = 0;	// wrap to next line
	rowS ++;
	rowS %= 30;	// wrap to top of display if passed the end
}

void LCD_SPI::drawStr5x7(char *message){
	char *p = message;
	while(*p)	drawChar5x7(*p++);
}

void LCD_SPI::drawChar9x14(char character){
	uint8_t	buf[15][10][2], chr = (character & 0x7f) - 0x20;	// trim to only printable ascii characters
	uint16_t i, j, k, c, index = chr * 9 / 8;
	if(foreC == backC){				// transparent printing. Just write pixels over the background
		if(character > ' '){		// printable character?
			for(i=0; i<14; i++){
				k = (font_9x15[index + i*107]) | (font_9x15[index + 1 + i*107] << 8);
				k >>= chr & 7;
				for(j=0; j<9; j++)
					if(k & 1<<j)
						drawPixel(colM*10+j, rowM*15+i, foreC);
			}
		}else if(character == '\n')		// newline character?
			colM = 99;					// force newline
		colM++;
		if( colM < 24 )	return;
	}else{
		if(character == '\n'){			// if a newline character
			for(int  i=0; i<15; i++)	// clear to the end of the line
				drawLineH(colM*10, rowM*15+i, 240-colS*10, backC);
		}else{							// printable character - not transparent
			for(j=0; j<10; j++){		// fill inter line gap
				buf[14][j][0] = backC >> 8;
				buf[14][j][1] = backC & 255;
			}
			// The font file is a bit array, 9 bits/char packed together, 95 chars/107 bytes
			for(i=0; i<14; i++){		// for each line
				k = (font_9x15[index + i*107]) | (font_9x15[index + 1 + i*107] << 8);
				k >>= chr & 7;
				for(j=0; j<9; j++){		// for each column
					c = (k & (1<<j)) ? foreC : backC;
					buf[i][j][0] = c >> 8;
					buf[i][j][1] = c & 255;
				}
				buf[i][9][0] = backC >> 8;	// fill inter character gap
				buf[i][9][1] = backC & 255;
			}
			setAddrWindow(colM*10, rowM*15, colM*10+9, rowM*15+14);
			bufferOut( (uint8_t *)buf, sizeof(buf) );
			colM++;
			if( colM < 24)	return;
		}
	}
	colM = 0;		// wrap to next line
	rowM++;
	rowM &= 15;		// 16 rows, 15 pixels high, wrap to first if past last
}

void LCD_SPI::drawStr9x14(char *message){
	char *p = message;
	while(*p)	drawChar9x14(*p++);
}

// Main loop, initialize 

void LCD_SPI::process(Driver *drv) {
// TODO review sequence of states
	esp_err_t rc;
	spi = (SPIDev *)drv;		// don't want to keep passing this to functions
	spi->sck_speed_set( SCK_SPEED_HIGH );	// 4MHz isn't very high

	switch (state) {
		case s_detect:
			printf("s_detect-->");
			// detect spi device
			state = s_error;														// assume failure
			if (spi->detect(channel, address) == ESP_OK) {
				// init MCP23S17									making all but the 3 required pins inputs is safer???
				if( (rc = wrMCP(MCP23S17_REG_IOCONA,    8)) != ESP_OK)	return;		// EN_HA - enable hardware addressing
				if( (rc = wrMCP(MCP23S17_REG_IOCONB,    8)) != ESP_OK)	return;
				if( (rc = wrMCP(MCP23S17_REG_IODIRA,    0)) != ESP_OK)	return;		// all outputs - all pins are outputs
				if( (rc = wrMCP(MCP23S17_REG_IODIRB,    0)) != ESP_OK)	return;
				if( (rc = wrMCP(MCP23S17_REG_GPIOA , 0xff)) != ESP_OK)	return;		// all outputs high
				if( (rc = wrMCP(MCP23S17_REG_GPIOB , 0xff)) != ESP_OK)	return;	
			// reset the ST7789 display
				wrMCP(mcpPort, 0xff);						// ST7789 reset pin HI
				tickcnt = get_tickcnt();					
				while (! is_tickcnt_elapsed(tickcnt, 5)) ;	// wait 5ms

				wrMCP(mcpPort, arst);						// ST7789 reset pin LO (reset)
				tickcnt = get_tickcnt();					
				while (! is_tickcnt_elapsed(tickcnt, 20)) ;	// wait 20ms

				wrMCP(mcpPort, 0xff);						// ST7789 reset pin HI
				tickcnt = get_tickcnt();
				while (! is_tickcnt_elapsed(tickcnt, 150));	// wait 150ms
				printf("s_cmd_init\n");

				// init ST7789 - send all of the commands from the command list initST7789
				for(int i=0; i<sizeof(initST7789)/sizeof(char *); i++){
					if(cmdSize[i] > 1)	sendCnD(initST7789[i][0], (uint8_t *)initST7789[i]+1, cmdSize[i]-1);
					else				writecommand(initST7789[i][0]);
				}

				wrMCP(mcpPort, 0xff);	// CS off

				tickcnt = get_tickcnt();
				while(!is_tickcnt_elapsed(tickcnt, 120));	//	delay(120);

				writecommand(0x29);    	//Display on
				wrMCP(mcpPort, 0xff);	// CS off

				state = s_cmd_init;					// MCP23S17 initialized
			}
			break;

		case s_cmd_init:
			state = s_idle;
			break;

		case s_idle:
			break;

		case s_error:
			error = true;				// set error flag
			initialized = false;		// clear initialized flag
			tickcnt = get_tickcnt();	// get current tickcnt
			state = s_wait;				// goto wait and retry with detect state
			break;

		case s_wait:
			if (is_tickcnt_elapsed(tickcnt, 1000)) {	// delay 1000ms before retry detect
				state = s_detect;
			}
			break;
	}
}

// commands from BLOCKLY generator

void LCD_SPI::print(uint8_t col, uint8_t row, char *message, uint8_t siz) {
	if(siz == 0){
		if(col)	colS = col-1;
		if(row)	rowS = row-1;
		drawStr5x7 (message);
	}
	if(siz == 1){
		if(col)	colM = col-1;
		if(row)	rowM = row-1;
		drawStr9x14(message);
	}
	if(siz == 2){	// custom font, GFX format
		if(col)	colC = col;
		if(row)	rowC = row;

		char *p = message;
		while(*p)	drawCharCustom((uint8_t)(*p++));
	}
}

void LCD_SPI::drawCharCustom(uint8_t c){
	if(c == '\n'){				// new line character
		if( foreC != backC )	// don't clear anything if transparent - transparent is standard for GFX
			for(int i=2; i<font3.yAdvance; i++)
				drawLine(colC, rowC+i, _W, rowC+i, backC);
		colC = 0;				// advance cursor to the start of the next line
		rowC += font3.yAdvance;	// move down distance dependings on font size
		return;
	}

	if(c > font3.last)	return;	// character code is out of range
	if(c < font3.first)	return;	// character code is out of range
	c -= font3.first;			// draw a printable character

	if( foreC == backC ){	// Transparent printing - default for GFX fonts
		int16_t h = font3.glyph[c].height, w = font3.glyph[c].width;
		uint8_t mask = 0x80, *p = font3.bitmap + font3.glyph[c].bitmapOffset;

		for(int i=0; i<h; i++){			// scan the bits in the bitmap table
			for(int j=0; j<w; j++){
				if(*p & mask)			// if bit set, set pixel
					drawPixel(colC+j+font3.glyph[c].xOffset, rowC+i+font3.glyph[c].yOffset+font3.yAdvance, foreC);
				mask >>= 1;				// ready to test next bit
				if(mask == 0){			// if done with this byte
					mask = 0x80;		// prepare to test the next byte
					*p++;				// this is the next byte from the bitmap
				}
			}
		}
	}else{					// non-transparent printing, might not clear the background properly with all fonts
		int16_t x, y, h = font3.glyph[c].height, w = font3.glyph[c].width, bufr[w*h], colour;
		uint8_t mask = 0x80, *p = font3.bitmap + font3.glyph[c].bitmapOffset, *q = (uint8_t *)bufr;

		for(int i=0; i<font3.yAdvance; i++)			// clear the rectangle where the next character should be written
			drawLine(colS, rowS+i, colS+font3.glyph[c].xAdvance, rowS+i, backC);

		while(q < (uint8_t *)bufr + sizeof(bufr)){	// scan the bits in the bitmap table
			colour = *p & mask ? foreC : backC;		// if bit set, then use foreC, else backC
			*q++ = colour >> 8;						// put the color in the bufr, byte order may be different with different architectures
			*q++ = colour & 0xff;
			mask >>= 1;								// ready to test next bit
			if(mask == 0){							// if done with this byte
				mask = 0x80;						// prepare to test the next byte
				*p++;								// this is the next byte from the bit bitmap
			}
		}

		x  = colC + font3.glyph[c].xOffset;			// x position on the display
		y  = rowC + font3.glyph[c].yOffset;			// y position on the display
		y += font3.yAdvance;
		setAddrWindow(x, y, x+w-1, y+h-1);			// window in ST7789 to for character
		bufferOut((uint8_t *)bufr, sizeof(bufr));	// send the character
	}
	colC += font3.glyph[c].xAdvance;				// advance the cursor in the x direction
}

void LCD_SPI::point(uint16_t x1, uint16_t y1, uint32_t color){
	drawPixel(x1, y1, color565(color));
}

void LCD_SPI::line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){
	drawLine(x1, y1, x2, y2, color565(color));
}

void LCD_SPI::rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, bool fill){
	drawRect(x1, y1, x2, y2, color565(color), fill);
}

void LCD_SPI::clear(uint8_t rotation, uint32_t color) {
	display_rotation = rotation & 3;
	writecommand(0x36);
	writedata(rot[display_rotation]);
	wrMCP(mcpPort, 0xff);	// CS off
	drawRect(0,0,_W,_H,color565(color),true);			// fill entire screen with color
	rowS = colS = rowM = colM = colC = rowC = 0;		// reset cursors
}

void LCD_SPI::setTextColor(uint32_t foreground, uint32_t background){
	foreC = color565(foreground);
	backC = color565(background);
}

void LCD_SPI::triangle  (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3	, uint32_t color, bool fill ){
	printf("%7.3f seconds\n", (double)(get_tickcnt() - tickTimer) / 100. );
	tickTimer = get_tickcnt();
// stub for blockly testing
}

void LCD_SPI::circle  	(uint16_t x1, uint16_t y1, uint16_t r											, uint32_t color, bool fill ){
// stub for blockly testing
}

void LCD_SPI::image  	(uint16_t x1, uint16_t y1, uint16_t imageID, double scale){
// stub for blockly testing
}

// this function should go away. It should be done completely in blockly. We just don't know blockly/js well enough yet.
char * LCD_SPI::num2str(double val, uint8_t wid, uint8_t dig, uint8_t fmt){
	static char buf[40], *p; // buffer for results
	if( wid > sizeof(buf) - 1)	wid = sizeof(buf) - 1;
	switch(fmt){
		case 0:	// integer
			sprintf(buf, "%*ld", wid, (long)val);
			break;
		case 1: // hex
			sprintf(buf, "%*lX", wid, (long)val);
			break;
		case 2: // hex with leading 0
			sprintf(buf, "%0*lX", wid, (long)val);
			break;
		case 3: // fixed point
			sprintf(buf, "%*.*f", wid, dig, val);
			break;
		case 4: // exponential
			sprintf(buf, "%*.*E", wid, dig, val);
			break;
		default:
			p = buf;
			while(wid--)	*p++ = '?';
			*p = 0;
	}
	return buf;
}

// utility function - blockly uses RGB888 (24bit color) while the ST7789 used RGB565 (16bit color)
uint16_t LCD_SPI::color565(uint32_t rgb) {	// convert 24 bit RGB888 to 16 bit RGB565
  return ((rgb >> 8) & 0xF800) | ((rgb >> 3) & 0x7e0) | ((rgb >> 3) & 0x1f);
}

void LCD_SPI::initPlot(uint8_t index, char * variable, char * units, double plotMin, double plotMax, char decimals, uint32_t color ){
	if(index > 2)	return;		// cannot do this
	memset(&pd[index], 0, sizeof(plotData));		// zero out the entire structure
	char tmp[8];

	// save the fixed min and max values, if specified (default is to use auto ranging)
	pd[index].fixedMin = plotMin == plotMin;		// NaN is NOT equal to NaN
	if(pd[index].fixedMin)	pd[index].min = plotMin;// store the fixed Min value
	pd[index].fixedMax = plotMax == plotMax;		// NaN is NOT equal to NaN
	if(pd[index].fixedMax)	pd[index].max = plotMax;// store the fixed Max value

	strncpy(tmp, variable, 7);	// format the variable name, right justified, 7 chars max, pad w/ leading spaces
	tmp[7] = 0;		// null terminator
	while(strlen(tmp) < 7){
		for(int i=7; i; i--)
			tmp[i] = tmp[i-1];					// shift 1 character
		*tmp = ' ';
	}
	strcpy(pd[index].variable, tmp);

	strncpy(tmp, units, 7);		// format the units string, right justified, 7 chars max, pad w/ leading spaces
	tmp[7] = 0;		// null terminator
	while(strlen(tmp) < 7){
		for(int i=7; i; i--)
			tmp[i] = tmp[i-1];					// shift 1 character
		*tmp = ' ';
	}
	strcpy(pd[index].units, tmp);

	sprintf(pd[index].format,"%%7.%df", (int8_t)decimals);	// create sprintf format for float to str conversion

	if(color == 0xfffffa){									// store colors
		if(index == 0)	pd[index].color = C_RED;			// default colors if color inputs are empty
		if(index == 1)	pd[index].color = C_GREEN;
		if(index == 2)	pd[index].color = C_BLUE;
	}else
		pd[index].color = color565(color);					// color input is specified - use it
	
	uint16_t saveFC = foreC, saveBC = backC, saveCM = colM, saveRM = rowM;
	backC = C_BLACK;	// black background

	rowM = 11;	colM =  0;	
	foreC = C_WHITE;		drawStr9x14("Var");
	foreC = pd[0].color;	drawStr9x14(pd[0].variable);
	foreC = pd[1].color;	drawStr9x14(pd[1].variable);
	foreC = pd[2].color;	drawStr9x14(pd[2].variable);

	rowM = 15;	colM =  0;
	foreC = C_WHITE;		drawStr9x14("Unt");
	foreC = pd[0].color;	drawStr9x14(pd[0].units);
	foreC = pd[1].color;	drawStr9x14(pd[1].units);
	foreC = pd[2].color;	drawStr9x14(pd[2].units);

	foreC = saveFC; backC = saveBC; colM = saveCM; rowM = saveRM;
	tickTimer = get_tickcnt();
}


// plot new data point - this take 8.45s to plot 240 data points. The points divisible by 40 redraw the plot area to refresh the grid lines.
// It takes 2.00s to plot 240 data points without restoring the gridlines. Thr graph doesn't look good without the light gray grid lines.
void LCD_SPI::plotPoint(double v1, double v2, double v3){	// new data point
	float min, max, oldmin[3], oldmax[3];
	char tmp[8];
	int y;

// output data to serial monitor. This can be copied and pasted into a CSV file.
/*
	if(pdCnt == 0)					// write header for CSV file
		printf("\ntime (s),%s (%s),%s (%s),%s (%s)\n", pd[0].variable, pd[0].units, pd[1].variable, pd[1].units, pd[2].variable, pd[2].units);
	printf("%.2f,",((double)get_tickcnt())/100.0);	// write data for CSV file
	printf(pd[0].format, v1);
	printf(",");
	printf(pd[1].format, v2);
	printf(",");
	printf(pd[2].format, v3);
	printf("\n");
*/
	pd[0].data[pdPos] = v1;			// add new point to data array
	pd[1].data[pdPos] = v2;
	pd[2].data[pdPos] = v3;
	pdCnt++;
	if(pdCnt > 240)	pdCnt = 240;

	for(int i=0; i<3; i++){			// check if the min or max changed
		oldmin[i] = pd[i].min;		// change happens when a new min or max is added
		oldmax[i] = pd[i].max;		// or when an old min or max is overwritten in the data buffer
	}
	for(int j=0; j<3; j++){
		min = 1e20;
		max = -1e20;
		for(int i=0; i<pdCnt; i++){
			if(! pd[j].fixedMin && pd[j].data[i] < min)	min = pd[j].data[i];
			if(! pd[j].fixedMax && pd[j].data[i] > max)	max = pd[j].data[i];
		}
		if( ! pd[j].fixedMax)	pd[j].max = max;
		if( ! pd[j].fixedMin)	pd[j].min = min;
	}

	// plot it
	bool replot = false;		// assume no change in any of the mins or maxes
	for(int i=0; i<3; i++){		// we will need to redraw the entire plot area if a min or max changed
		replot |= pd[i].min != oldmin[i];
		replot |= pd[i].max != oldmax[i];
	}

	uint16_t saveFC = foreC, saveBC = backC, saveCM = colM, saveRM = rowM;

	rowM = 13; colM = 0;
	foreC = C_WHITE;		drawStr9x14("Cur");
	foreC = pd[0].color;	snprintf(tmp, 8, pd[0].format, pd[0].data[pdPos]);	drawStr9x14(tmp);
	foreC = pd[1].color;	snprintf(tmp, 8, pd[1].format, pd[1].data[pdPos]);	drawStr9x14(tmp);
	foreC = pd[2].color;	snprintf(tmp, 8, pd[2].format, pd[2].data[pdPos]);	drawStr9x14(tmp);

	if(replot || pdPos % 40 == 0){		// replot all data points if any min or max changed or if grid line overwritten
		backC = C_BLACK;		// black background

		rowM = 12; colM = 0;
		foreC = C_WHITE;		drawStr9x14("Max");
		foreC = pd[0].color;	snprintf(tmp, 8, pd[0].format, pd[0].max);	drawStr9x14(tmp);
		foreC = pd[1].color;	snprintf(tmp, 8, pd[1].format, pd[1].max);	drawStr9x14(tmp);
		foreC = pd[2].color;	snprintf(tmp, 8, pd[2].format, pd[2].max);	drawStr9x14(tmp);
	
		rowM = 14; colM = 0;
		foreC = C_WHITE;		drawStr9x14("Min");
		foreC = pd[0].color;	snprintf(tmp, 8, pd[0].format, pd[0].min);	drawStr9x14(tmp);
		foreC = pd[1].color;	snprintf(tmp, 8, pd[1].format, pd[1].min);	drawStr9x14(tmp);
		foreC = pd[2].color;	snprintf(tmp, 8, pd[2].format, pd[2].min);	drawStr9x14(tmp);

		drawRect(0, 64, 239, 164, C_BLACK, true);		// clear graph
		for(int i=64; i<=164; i+=20)	drawLineH(0,i ,240,0x2084);	// horizontal grid
		for(int i=0 ; i<=200; i+=40)	drawLineV(i,64,101,0x2084);	// vertical grid
		float denominator[3];			// TODO combine rgb pixels if point overlap
		for(int j=0; j<3; j++)
			denominator[j] = 100.0 / (pd[j].max - pd[j].min);
		for(int i=0; i<pdCnt; i++){
			for(int j=0; j<3; j++){
				y = (pd[j].data[i] - pd[j].min) * denominator[j] + 0.5;
				if(y<  0)	y=0;						// might be out of bounds if fixed min or max used
				if(y>100)	y=100;
				drawPixel(i, 164-y, pd[j].color);
			}
		}
	}
	for(int j=0; j<3; j++){						// plot new data point
		float denom = 100.0 / (pd[j].max - pd[j].min);
		y = (pd[j].data[pdPos] - pd[j].min) * denom + 0.5;
		if(y<  0)	y=0;						// might be out of bounds if fixed min or max used
		if(y>100)	y=100;
		drawPixel(pdPos, 164-y, pd[j].color);
	}
	drawLineV((pdPos+1)%240, 64, 101, C_BLACK);	// cursor to show end/start of new data
	drawLineV((pdPos+2)%240, 64, 101, C_CYAN);
	drawLineV((pdPos+3)%240, 64, 101, C_BLACK);

	foreC = saveFC; backC = saveBC; colM = saveCM; rowM = saveRM;
	pdPos++;
	pdPos %= 240;
}

// Unused but seemingly required functions

int  LCD_SPI::prop_count(void) 						{	return false;}	// not supported
bool LCD_SPI::prop_name (int index, char *name  ) 	{	return false;}	// not supported
bool LCD_SPI::prop_unit (int index, char *unit  ) 	{	return false;}	// not supported
bool LCD_SPI::prop_attr (int index, char *attr  ) 	{	return false;}	// not supported
bool LCD_SPI::prop_read (int index, char *value ) 	{	return false;}	// not supported
bool LCD_SPI::prop_write(int index, char *value ) 	{	return false;}	// not supported