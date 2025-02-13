#ifndef __LCD_SPI_H__
#define __LCD_SPI_H__


#define MCP23S17_REG_IODIRA				0x00
#define MCP23S17_REG_IODIRB				0x01
#define MCP23S17_REG_IOCONA				0x0A
#define MCP23S17_REG_IOCONB				0x0B
#define MCP23S17_REG_GPIOA				0x12
#define MCP23S17_REG_GPIOB				0x13

#define MAX_CMD_BUF	99

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

#include "driver.h"
#include "device.h"
#include "spi-dev.h"

const uint16_t _H = 239;	// screen height, max Y
const uint16_t _W = 239;	// screen width, max x

typedef struct blkCommand{
	uint16_t code;
	uint16_t parm[5];
	bool flag;
	char str[32];
} blkCmd;


class LCD_SPI : public Device {

	private:
// properties
		enum { cmd_clear, cmd_print, cmd_printTiny, cmd_setColor, cmd_point, cmd_line, cmd_rect, cmd_circle} cmd_code;
		enum { s_detect, s_cmd_init, s_idle, s_error, s_wait } state;
		SPIDev *spi;
		gpio_config_t io_conf_0in, io_conf_0out;
		TickType_t tickcnt;

		const uint8_t rot[4] = {0, TFT_MAD_MX | TFT_MAD_MV, TFT_MAD_MX | TFT_MAD_MY, TFT_MAD_MV | TFT_MAD_MY};
		blkCmd cmds[MAX_CMD_BUF];				// command buffer
		uint16_t cmdPut, cmdPull, cmdCount;		// indices and count to buffer
		uint16_t foreC, backC;
		uint8_t rowS, colS,  rowM, colM, display_rotation;
		uint8_t mcpPort, acmd, adat, arst;
		uint8_t flag, set_flag, clr_flag;


// methods (functions)
		esp_err_t wrMCP(uint8_t reg, uint8_t val);
		void writecommand(uint8_t cmd);
		void writedata(uint8_t data);
		void writedata16(uint16_t data);
		void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void drawLineV(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
		void drawLineH(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
		void drawLine (uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t color);
		void drawPixel(uint16_t x, uint16_t y, uint16_t color);
		void drawRect(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1, uint16_t color, bool fill);
		void drawChar5x7(char character);
		void drawStr5x7(char *message);
		void drawChar9x14(char character);
		void drawStr9x14(char *message);
		void bufferOut(uint8_t *bufr, uint16_t sz);
		bool testAndFix(uint16_t *x1, uint16_t *x2, uint16_t *y1, uint16_t *y2);
		uint16_t color565(uint32_t rgb);

	public:
		// constructor
		LCD_SPI(int bus_ch, int dev_addr, uint8_t lcd_column, uint8_t lcd_row);
		// override
		void init(void);
		void process(Driver *drv);
		int prop_count(void);
		bool prop_name(int index, char *name);
		bool prop_unit(int index, char *unit);
		bool prop_attr(int index, char *attr);
		bool prop_read(int index, char *value);
		bool prop_write(int index, char *value);
		// method

		void clear      (uint8_t rotation, uint32_t color);
		void print      (uint8_t row, uint8_t col, char *message, bool tiny);
		void line		(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
		void rectangle  (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, bool fill );
		void setTextColors(uint32_t foreground, uint32_t background);
};

#endif