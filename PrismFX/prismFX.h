#ifndef __PrismFX_H__
#define __PrismFX_H__

#define MCP23S17_REG_IODIRA				0x00
#define MCP23S17_REG_IODIRB				0x01
#define MCP23S17_REG_IOCONA				0x0A
#define MCP23S17_REG_IOCONB				0x0B
#define MCP23S17_REG_GPIOA				0x12
#define MCP23S17_REG_GPIOB				0x13

// Flags for TFT_MADCTL
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_RGB 0x00
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_SS  0x02
#define TFT_MAD_GS  0x01

#define C_BLACK	  0
#define C_WHITE   0xffff
#define C_RED     0xf800
#define C_GREEN	  0x07e0
#define C_BLUE	  0x001f
#define C_CYAN	  0x07ff
#define C_MAGENTA 0xf81f
#define C_YELLOW  0xffe0
#define C_DEFAULT 0xfffa

#include "driver.h"
#include "device.h"
#include "spi-dev.h"
#include "gfxfont.h"

const uint16_t _H = 239;	// screen height, max Y
const uint16_t _W = 239;	// screen width, max x

typedef struct pData{
	float data[240];						// data points from plot data block
	float min, max;							// min and max, from init plot block or dynamic (from data)
	char units[8];							// units from init plot block
	char variable[8];						// variable name from init plot block
	char format[8];							// %7.nf where n comes from init plot block
	uint16_t color;							// color from init plot block
	bool fixedMin, fixedMax;				// true if min and max specified in init plot block	
} plotData;


class PrismFX : public Device {

	private:
// properties
		enum { s_detect, s_cmd_init, s_idle, s_error, s_wait } state;
		SPIDev *spi;
		gpio_config_t io_conf_0in, io_conf_0out;
		TickType_t tickcnt, tickTimer;
		const uint8_t rot[4] = {0, TFT_MAD_MX | TFT_MAD_MV, TFT_MAD_MX | TFT_MAD_MY, TFT_MAD_MV | TFT_MAD_MY};

		plotData pd[3];
		uint16_t pdPos, pdCnt;

		uint16_t rowS, colS, foreC, backC;
		uint8_t rowM, colM, rowC, colC, display_rotation;
		uint8_t mcpPort, acmd, adat, arst;
		bool CMD_SET;	// maintain state of C/D pin to avoid changing it if already in the correct state

		// properties required for printGFX
		uint16_t bufPtr, bufLen, minY, maxY;
		uint64_t gfx_buffer[260];
		const GFXfont *fnt;

// local member functions
		esp_err_t wrMCP(uint8_t reg, uint8_t val);
		void writecommand(uint8_t cmd);
		void writedata(uint8_t data);
		void sendCnD(uint8_t cmd, uint8_t * data, uint16_t dCnt);
		void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void drawLineV(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
		void drawLineH(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
		void drawLine (uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t color);
		void drawPixel(uint16_t x, uint16_t y, uint16_t color);
		void drawRect (uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t color, bool fill);
		void drawChar5x7(char character);
		void drawStr5x7(char *message);
		void drawChar9x14(char character);
		void drawStr9x14(char *message);
		void drawCharCustom(uint8_t character);
		void bufferOut(uint8_t *bufr, uint32_t sz);
		uint16_t color565(uint32_t rgb);
	
	public:
		// constructor
		PrismFX(int dev_addr);
		void init(void);
		void process(Driver *drv);
		int  prop_count(void);
		bool prop_name(int index, char *name);
		bool prop_unit(int index, char *unit);
		bool prop_attr(int index, char *attr);
		bool prop_read(int index, char *value);
		bool prop_write(int index, char *value);

		// blockly functions

		void clear      (uint8_t rotation, uint32_t color);
		void setTextColor(uint32_t foreground, uint32_t background);
		void print      (uint8_t col, uint8_t row, char *message, uint8_t siz);
		void printGFX   (uint8_t col, uint8_t row, char *message);				// message can contain Thai, English, or both
		void bufferChar (uint8_t raw);
		
		void point      (uint16_t x1, uint16_t y1														, uint32_t color);
		void line		(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2								, uint32_t color);
		void rectangle  (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2								, uint32_t color, bool fill );
		void triangle  	(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3	, uint32_t color, bool fill );
		void circle  	(uint16_t x1, uint16_t y1, uint16_t r											, uint32_t color, bool fill );
		static char *num2str	(double val, uint8_t wid, uint8_t dig, uint8_t fmt);
		void initPlot	(uint8_t index, char *variable, char * units, double min, double max, char decimals, uint32_t color);
		void plotPoint	(double v1, double v2, double v3);
		void drawImage	(uint16_t x1, uint16_t y1, uint8_t imageID, uint16_t scale);
};

#endif
