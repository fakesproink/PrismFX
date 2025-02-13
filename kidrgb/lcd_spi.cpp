#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "kidbright32.h"
#include "lcd_spi.h"
#include "font_5x7.h"
#include "font_9x15.h"

// 26 is out OUT1, 27 is OUT2, 0 is default
#define CS_PIN	GPIO_NUM_0
const uint16_t BUFFER_SIZE = 32;	// max number of bytes that can sent with spi->write

uint16_t  swapBytes(uint16_t val){
	return (val >> 8) | (val << 8);
}

// INITIALIZATION OF properties

LCD_SPI::LCD_SPI(int bus_ch, int dev_addr, uint8_t lcd_column, uint8_t lcd_row) {
	channel = bus_ch;
	address = dev_addr;
	printf("new LCD_SPI object %p\n", this);
}

void LCD_SPI::init(void) {
	printf("init %02x %p\n", address, this);
	flag = 0;
	set_flag = 0;
	clr_flag = 0;
	state = s_detect;
	cmdCount = cmdPut = cmdPull = 0;
	mcpPort = MCP23S17_REG_GPIOA;		// varies by display if multiple displays
	acmd = 0xe7;						// which IOs are connected to the display
	adat = 0xef;
	arst = 0xfb;
//	acmd = 0x3F;						// 2nd display
//	adat = 0x7f;
//	arst = 0xBF;

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
	gpio_config(&io_conf_0in);	// change pin 0 to an input
#endif
	return rc;
}

void LCD_SPI::writecommand(uint8_t cmd){	// sets up ST7789 pins using the MCP23S17, then send command to ST7789
  	wrMCP(mcpPort, acmd);
	spi->write(channel, address, &cmd, 1);
  	wrMCP(mcpPort, 0xff);
}

void LCD_SPI::writedata(uint8_t data){	// sets up ST7789 pins using the MCP23S17, then 1 byte of data to ST7789
  	wrMCP(mcpPort, adat);
 	spi->write(channel, address, &data, 1);
  	wrMCP(mcpPort, 0xff);
}

void LCD_SPI::writedata16(uint16_t data){	// sets up ST7789 pins using the MCP23S17, then 2 bytes of data to ST7789
	uint16_t tmp = swapBytes(data);
 	wrMCP(mcpPort, adat);
  	spi->write(channel, address, (uint8_t *)&tmp, 2);
  	wrMCP(mcpPort, 0xff);
}

// sets up ST7789 to receive bulk data, 2 * (1+x1-x0) * (1+y1-y0) bytes at specific coordinates
// working to optimize this function as it is called often, 1x/point, 1x/character and the hardware is slow
void LCD_SPI::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	printf("rotation %d\n", display_rotation);
	if(display_rotation == 3){	x1 += 80;	x2 += 80;	}	// pins on the right
	if(display_rotation == 2){	y1 += 80;	y2 += 80;	}	// pins on the bottom
	uint32_t xData = swapBytes(x2)<<16 | swapBytes(x1), yData = swapBytes(y2)<<16 | swapBytes(y1);
#define RELIABLE_SLOW
#ifdef RELIABLE_SLOW
// 1.6 ms for drawPoint - changes ST7789 CS to high, then CS low with DC
	writecommand(0x2a); 		// Column addr set
	bufferOut((uint8_t *)&xData, 4);	// original code sent 4 separate bytes
	writecommand(0x2b); 		// Row addr set
	bufferOut((uint8_t *)&yData, 4);	// original code sent 4 separate bytes
	writecommand(0x2c);			// write to RAM
#else
	uint8_t cmdCol = 0x2a, cmdRow = 0x2b, cmdColor = 0x2c;
// 1.1 ms for drawPoint - changes ST7789 DC pin while holding CS low - seems OKay
  	wrMCP(mcpPort, acmd);						// DC lo, CS lo
	spi->write(channel, address, &cmdCol, 1);	// Column addr set 0x2a
  	wrMCP(mcpPort, adat);						// DC hi, CS lo
	spi->write(channel, address, (uint8_t *)&xData , 4);	// x start, end

  	wrMCP(mcpPort, acmd);						// DC lo, CS lo
	spi->write(channel, address, &cmdRow, 1);	// row addr set 0x2a
  	wrMCP(mcpPort, adat);						// DC hi, CS lo
	spi->write(channel, address, (uint8_t *)&yData , 4);	// y start, end

  	wrMCP(mcpPort, acmd);						// DC lo, CS lo
	spi->write(channel, address, &cmdColor, 1);	// Column addr set 0x2a
#endif
}

void LCD_SPI::bufferOut(uint8_t *bufr, uint16_t sz){	// for sending bulk data in BUFFER_SIZE chunks
	wrMCP(mcpPort, adat);  		// cs active (low)
	while(sz > BUFFER_SIZE){
		spi->write(channel, address, bufr, BUFFER_SIZE);
		bufr += BUFFER_SIZE;
		sz -= BUFFER_SIZE;
	}
	if(sz)						// remainder of buffer, if any
		spi->write(channel, address, bufr, sz);
	wrMCP(mcpPort, 0xff);      	// rst, dc, & cs all high
}

// Geometric Functions: Point, Line, Circle, Rectange, Bitmap, maybe triangle?

void LCD_SPI::drawPixel(uint16_t x, uint16_t y, uint16_t color){
	if(x > _W || y > _H)  return;		// when cropping a single pixel, the result is a pixel or no pixel
	setAddrWindow(x, y, x, y);
	writedata16(color);	// writedata16 swaps Bytes
}

void LCD_SPI::drawLineH(uint16_t x, uint16_t y, uint16_t w, uint16_t color){
	uint16_t buf[w];
	for(int i=0; i<w; i++)
		buf[i] = swapBytes(color);
	setAddrWindow(x, y, x+w, y);
	bufferOut( (uint8_t *)buf, w*2);
}

void LCD_SPI::drawLineV(uint16_t x, uint16_t y, uint16_t h, uint16_t color){
	uint16_t buf[h];
	for(int i=0; i<h; i++)
		buf[i] = swapBytes(color);
	setAddrWindow(x, y, x, y+h);
	bufferOut( (uint8_t *)buf, h*2);
}

void LCD_SPI::drawLine(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t color){
	if(x == x1)	drawLineV(x, y, 1+y1-y, color);		// draw FAST vertical line
	if(y == y1)	drawLineH(x, y, 1+x1-x, color);		// draw FAST horizontal line
// TODO draw an oblique line}
}

// TODO drawCircle
// TODO drawBitmap
// TODO drawTriangle, maybe?? code exists from buydisplay.com but how useful is it?

void LCD_SPI::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, bool fill){
	if(fill){
		uint8_t bufr[BUFFER_SIZE];		// max number of bytes that can be sent over SPI bus
		uint16_t *buf=(uint16_t *)bufr, i=(1+x2-x1)*(1+y2-y1), k=0;
		setAddrWindow(x1, y1, x2, y2);
		wrMCP(mcpPort, adat);  // dc high, cs active (low)
		while(i--){
			buf[k++] = swapBytes(color);
			if(k*2 == BUFFER_SIZE){
				spi->write(channel, address, bufr, BUFFER_SIZE);
				k = 0;
			}
		}
		if(k)	spi->write(channel, address, bufr, k*2);
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
	uint16_t i, j, k, buf[48], index = (character & 0xff)*5;

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
			for(i=5; i<48; i+=6)// fill character buffer with background color
				buf[i] = swapBytes(backC);
			for(i=0; i<5; i++){
				k = font_5x7[index + i];
				for(j=0; j<8; j++)
					buf[6*j+i] = (k & (1<<j)) ? swapBytes(foreC) : swapBytes(backC);
			}
			setAddrWindow(colS*6, rowS*8, colS*6+5, rowS*8+7);
			bufferOut( (uint8_t *)buf, sizeof(buf));
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
	uint8_t	chr = (character & 0x7f) - 0x20;	// trim to only printable ascii characters
	uint16_t i, j, k, buf[15][10], index = chr * 9 / 8;
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
			for(j=0; j<10; j++)	buf[14][j] = swapBytes(backC);	// fill inter line gap
			// The font file is a bit array, 9 bits/char packed together, 95 chars/107 bytes
			for(i=0; i<14; i++){
				k = (font_9x15[index + i*107]) | (font_9x15[index + 1 + i*107] << 8);
				k >>= chr & 7;
				for(j=0; j<9; j++)
					buf[i][j] = swapBytes( (k & 1<<j) ? foreC : backC);
				buf[i][9] = swapBytes(backC);	// fill inter character gap
			}
			setAddrWindow(colM*10, rowM*15, colM*10+9, rowM*15+14);
			bufferOut( (uint8_t *)buf, sizeof(buf));
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

// Main loop, initialize and then look for blocks commands to process
// This runs in a different thread than the code that processes the block commands

void LCD_SPI::process(Driver *drv) {
// TODO review sequence of states
	esp_err_t rc;
	spi = (SPIDev *)drv;		// don't want to keep passing this to functions
	spi->sck_speed_set( SCK_SPEED_HIGH );	// 4MHz isn't very high

	switch (state) {
		case s_detect:
			printf("s_detect-->");
			// detect spi device
			state = s_error;													// assume failure
			if (spi->detect(channel, address) == ESP_OK) {
				// init MCP23S17									making all but the 3 required pins inputs is safer???
				if( (rc = wrMCP(MCP23S17_REG_IOCONA, 8)) != ESP_OK)	return;		// EN_HA - enable hardware addressing
				if( (rc = wrMCP(MCP23S17_REG_IOCONB, 8)) != ESP_OK)	return;
				if( (rc = wrMCP(MCP23S17_REG_IODIRA, 0)) != ESP_OK)	return;		// all outputs - all pins are outputs
				if( (rc = wrMCP(MCP23S17_REG_IODIRB, 0)) != ESP_OK)	return;
				if( (rc = wrMCP(MCP23S17_REG_GPIOA , 0xff)) != ESP_OK)	return;	// all outputs high
				if( (rc = wrMCP(MCP23S17_REG_GPIOB , 0xff)) != ESP_OK)	return;	
				// reset the ST7789 display
				tickcnt = get_tickcnt();
				while (! is_tickcnt_elapsed(tickcnt, 5)) ;

				wrMCP(mcpPort, arst);
				tickcnt = get_tickcnt();
				while (! is_tickcnt_elapsed(tickcnt, 20)) ;

				wrMCP(mcpPort, 0xff);
				tickcnt = get_tickcnt();
				while (! is_tickcnt_elapsed(tickcnt, 150)) ;
				printf("s_cmd_init\n");

				// init ST7789
				writecommand(0x36); 
				writedata(0x00);
				//writedata(0x70);
					
				writecommand(0x3A); // COLMOD
				writedata(0x05);

				writecommand(0xB2); // PORCTRL
				writedata(0x0C);
				writedata(0x0C);
				writedata(0x00);
				writedata(0x33);
				writedata(0x33); 

				writecommand(0xB7);   // GCTRL
				writedata(0x35);  

				writecommand(0xBB);
				writedata(0x19);

				writecommand(0xC0);
				writedata(0x2C);

				writecommand(0xC2);
				writedata(0x01);

				writecommand(0xC3);
				writedata(0x12);   

				writecommand(0xC4);
				writedata(0x20);  

				writecommand(0xC6); 
				writedata(0x0F);    

				writecommand(0xD0); 
				writedata(0xA4);
				writedata(0xA1);

				writecommand(0xE0);
				writedata(0xD0);
				writedata(0x04);
				writedata(0x0D);
				writedata(0x11);
				writedata(0x13);
				writedata(0x2B);
				writedata(0x3F);
				writedata(0x54);
				writedata(0x4C);
				writedata(0x18);
				writedata(0x0D);
				writedata(0x0B);
				writedata(0x1F);
				writedata(0x23);

				writecommand(0xE1);
				writedata(0xD0);
				writedata(0x04);
				writedata(0x0C);
				writedata(0x11);
				writedata(0x13);
				writedata(0x2C);
				writedata(0x3F);
				writedata(0x44);
				writedata(0x51);
				writedata(0x2F);
				writedata(0x1F);
				writedata(0x1F);
				writedata(0x20);
				writedata(0x23);

				writecommand(0x21); 

				writecommand(0x11);    //Exit Sleep

				tickcnt = get_tickcnt();
	while(!is_tickcnt_elapsed(tickcnt, 120));
//	delay(120);

				writecommand(0x29);    //Display on

				state = s_cmd_init;														// MCP23S17 initialized
			}
			break;

		case s_cmd_init:
			state = s_idle;
			break;

		case s_idle:
// TODO problem, the cmds buffer can be overrun! needs to be a sync'ing mechanism!
			if( cmdCount > 0 ){				
				blkCmd *cmd = &cmds[cmdPull];
				switch( cmd->code ){
					case cmd_printTiny:
						printf("cmd_printTiny, rot=%d\n", display_rotation);
						if(cmd->parm[0] > 0)	colS = cmd->parm[0]-1;
						if(cmd->parm[1] > 0)	rowS = cmd->parm[1]-1;
						drawStr5x7 (cmd->str);
						break;
					case cmd_print:
						printf("cmd_print, rot=%d\n", display_rotation);
						if(cmd->parm[0] > 0)	colM = cmd->parm[0]-1;
						if(cmd->parm[1] > 0)	rowM = cmd->parm[1]-1;
						drawStr9x14(cmd->str);
						break;
					case cmd_clear:
						printf("cmd_clear1, rot=%d\n", display_rotation);
						display_rotation = cmd->parm[0];
						printf("cmd_clear2, rot=%d\n", display_rotation);
						drawRect(0,0,_W,_H,cmd->parm[1],true);	// fill entire screen with color in parm[0]
						gpio_config(&io_conf_0out);	// restore pin 0 as output - might be required for other boards
						writecommand(0x36);
						writedata(rot[display_rotation]);
						gpio_config(&io_conf_0out);	// restore pin 0 as output - might be required for other boards
						rowS = colS = rowM = colM = 0;	// reset cursors
						break;
					case cmd_rect:
						printf("cmd_rect, rot=%d\n", display_rotation);
						drawRect(cmd->parm[0], cmd->parm[1], cmd->parm[2], cmd->parm[3], cmd->parm[4], cmd->flag);
						break;
				}
				cmdPull++;
				cmdPull %= MAX_CMD_BUF;
				cmdCount--;
			}
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
#if CS_PIN == 0
	gpio_config(&io_conf_0out);	// restore pin 0 as output - might be required for other boards
#endif
}

// commands from BLOCKLY - cannot call functions defined above.
// Just store the command info in a circular buffer for the process function

bool LCD_SPI::testAndFix(uint16_t *x1, uint16_t *x2, uint16_t *y1, uint16_t *y2){	// returns true if bad parameters, can't fix
//	if(*x1>*x2)	swap(x1,x2);
//	if(*y1>*y2)	swap(y1,y2);	// not sure what the right thing to do is
	if(*x2 > _W)	*x1 = _W;	// crop to fit screen
	if(*y2 > _H)	*y2 = _H;
	return false;
}

void LCD_SPI::print    (uint8_t row, uint8_t col, char *message, bool tiny) {
	char *p = message;
	while(*p){		// the str member of the cmds structure is only 32 bytes - long strings must be split into smaller cmds
		char *q = cmds[cmdPut].str;
		cmds[cmdPut].code = tiny ? cmd_printTiny : cmd_print;
		cmds[cmdPut].parm[0] = row;
		cmds[cmdPut].parm[1] = col;
		for(int i=1; i<sizeof(cmds[cmdPut].str) && *p; i++)
			*q++ = *p++;// copy the str, or part of it, into the cmd buffer
		*q = 0;			// terminate the c-str with a null chharacter
		cmdPut++;
		cmdPut %= MAX_CMD_BUF;
		cmdCount++;
		row = col = 0;	// ignore for subsequent commands - just continue from where the last print char ended
	}
}

void LCD_SPI::line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color){
	if(testAndFix(&x1, &y1, &x2, &y2))	return;		// return if parameters are bad and can't be fixed
	cmds[cmdPut].code = cmd_line;
	cmds[cmdPut].parm[0] = x1;
	cmds[cmdPut].parm[1] = y1;
	cmds[cmdPut].parm[2] = x2;
	cmds[cmdPut].parm[3] = y2;
	cmds[cmdPut].parm[4] = color565(color);
	cmdPut++;
	cmdPut %= MAX_CMD_BUF;
	cmdCount++;
}

void LCD_SPI::rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, bool fill){
	if(testAndFix(&x1, &y1, &x2, &y2))	return;		// return if parameters are bad and can't be fixed
	cmds[cmdPut].code = cmd_rect;
	cmds[cmdPut].flag = fill;
	cmds[cmdPut].parm[0] = x1;
	cmds[cmdPut].parm[1] = y1;
	cmds[cmdPut].parm[2] = x2;
	cmds[cmdPut].parm[3] = y2;
	cmds[cmdPut].parm[4] = color565(color);
	cmdPut++;
	cmdPut %= MAX_CMD_BUF;
	cmdCount++;
}

void LCD_SPI::clear(uint8_t rotation, uint32_t color) {
	cmds[cmdPut].code = cmd_clear;
	cmds[cmdPut].parm[0] = rotation & 3;
	cmds[cmdPut].parm[1] = color565(color);
	cmdPut++;
	cmdPut %= MAX_CMD_BUF;
	cmdCount++;
}

void LCD_SPI::setTextColors(uint32_t foreground, uint32_t background){
	cmds[cmdPut].code = cmd_setColor;
	cmds[cmdPut].parm[0] = color565(foreground);
	cmds[cmdPut].parm[1] = color565(background);
	cmdPut++;
	cmdPut %= MAX_CMD_BUF;
	cmdCount++;
}

uint16_t LCD_SPI::color565(uint32_t rgb) {	// convert 24 bit RGB888 to 16 bit RGB565
  return ((rgb >> 8) & 0xF800) | ((rgb >> 3) & 0x7e0) | ((rgb >> 3) & 0x1f);
}

// Unused but seemingly required functions

int  LCD_SPI::prop_count(void) 						{	return false;}	// not supported
bool LCD_SPI::prop_name (int index, char *name  ) 	{	return false;}	// not supported
bool LCD_SPI::prop_unit (int index, char *unit  ) 	{	return false;}	// not supported
bool LCD_SPI::prop_attr (int index, char *attr  ) 	{	return false;}	// not supported
bool LCD_SPI::prop_read (int index, char *value ) 	{	return false;}	// not supported
bool LCD_SPI::prop_write(int index, char *value ) 	{	return false;}	// not supported