/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>

   ----------------------------------------------------------------------
   	Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */

#include "STM32_sh1106_oled.h"


//===========================USER CONFIG================================//

#define I2C_PORT hi2c2

//===========================END USER CONFIG============================//


extern I2C_HandleTypeDef I2C_PORT;
/* Write command */
#define SH1106_WRITECOMMAND(command)      sh1106_I2C_Write(SH1106_I2C_ADDR, 0x00, (command))
/* Write data */
#define SH1106_WRITEDATA(data)            sh1106_I2C_Write(SH1106_I2C_ADDR, 0x40, (data))
/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* SH1106 data buffer */
static uint8_t SH1106_Buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

/* Private SH1106 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SH1106_t;

/* Private variable */
static SH1106_t SH1106;


#define SH1106_RIGHT_HORIZONTAL_SCROLL              0x26
#define SH1106_LEFT_HORIZONTAL_SCROLL               0x27
#define SH1106_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SH1106_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SH1106_DEACTIVATE_SCROLL                    0x2E // Stop scroll
#define SH1106_ACTIVATE_SCROLL                      0x2F // Start scroll
#define SH1106_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range

#define SH1106_NORMALDISPLAY       0xA6
#define SH1106_INVERTDISPLAY       0xA7


void SH1106_ScrollRight(uint8_t start_row, uint8_t end_row)
{
  SH1106_WRITECOMMAND (SH1106_RIGHT_HORIZONTAL_SCROLL);  // send 0x26
  SH1106_WRITECOMMAND (0x00);  // send dummy
  SH1106_WRITECOMMAND(start_row);  // start page address
  SH1106_WRITECOMMAND(0X00);  // time interval 5 frames
  SH1106_WRITECOMMAND(end_row);  // end page address
  SH1106_WRITECOMMAND(0X00);
  SH1106_WRITECOMMAND(0XFF);
  SH1106_WRITECOMMAND (SH1106_ACTIVATE_SCROLL); // start scroll
}


void SH1106_ScrollLeft(uint8_t start_row, uint8_t end_row)
{
  SH1106_WRITECOMMAND (SH1106_LEFT_HORIZONTAL_SCROLL);  // send 0x26
  SH1106_WRITECOMMAND (0x00);  // send dummy
  SH1106_WRITECOMMAND(start_row);  // start page address
  SH1106_WRITECOMMAND(0X00);  // time interval 5 frames
  SH1106_WRITECOMMAND(end_row);  // end page address
  SH1106_WRITECOMMAND(0X00);
  SH1106_WRITECOMMAND(0XFF);
  SH1106_WRITECOMMAND (SH1106_ACTIVATE_SCROLL); // start scroll
}


void SH1106_Scrolldiagright(uint8_t start_row, uint8_t end_row)
{
  SH1106_WRITECOMMAND(SH1106_SET_VERTICAL_SCROLL_AREA);  // sect the area
  SH1106_WRITECOMMAND (0x00);   // write dummy
  SH1106_WRITECOMMAND(SH1106_HEIGHT);

  SH1106_WRITECOMMAND(SH1106_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  SH1106_WRITECOMMAND (0x00);
  SH1106_WRITECOMMAND(start_row);
  SH1106_WRITECOMMAND(0X00);
  SH1106_WRITECOMMAND(end_row);
  SH1106_WRITECOMMAND (0x01);
  SH1106_WRITECOMMAND (SH1106_ACTIVATE_SCROLL);
}


void SH1106_Scrolldiagleft(uint8_t start_row, uint8_t end_row)
{
  SH1106_WRITECOMMAND(SH1106_SET_VERTICAL_SCROLL_AREA);  // sect the area
  SH1106_WRITECOMMAND (0x00);   // write dummy
  SH1106_WRITECOMMAND(SH1106_HEIGHT);

  SH1106_WRITECOMMAND(SH1106_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  SH1106_WRITECOMMAND (0x00);
  SH1106_WRITECOMMAND(start_row);
  SH1106_WRITECOMMAND(0X00);
  SH1106_WRITECOMMAND(end_row);
  SH1106_WRITECOMMAND (0x01);
  SH1106_WRITECOMMAND (SH1106_ACTIVATE_SCROLL);
}


void SH1106_Stopscroll(void)
{
	SH1106_WRITECOMMAND(SH1106_DEACTIVATE_SCROLL);
}



void SH1106_InvertDisplay (int i)
{
  if (i) SH1106_WRITECOMMAND (SH1106_INVERTDISPLAY);

  else SH1106_WRITECOMMAND (SH1106_NORMALDISPLAY);

}


void SH1106_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++)
    {
        for(int16_t i=0; i<w; i++)
        {
            if(i & 7)
            {
               byte <<= 1;
            }
            else
            {
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80) SH1106_DrawPixel(x+i, y, color);
        }
    }
}








uint8_t SH1106_Init(void) {

	/* Init I2C */
	sh1106_I2C_Init();

	/* Check if LCD connected to I2C */
	if (HAL_I2C_IsDeviceReady(&I2C_PORT, SH1106_I2C_ADDR, 1, 20000) != HAL_OK) {
		/* Return false */
		return 0;
	}

	/* A little delay */
	uint32_t p = 2500;
	while(p>0)
		p--;
	/* Init LCD */
	SH1106_WRITECOMMAND(0xAE); //ok display off
	SH1106_WRITECOMMAND(0x20); //Set Memory Addressing Mode
	SH1106_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	SH1106_WRITECOMMAND(0xB0); //ok Set Page Start Address for Page Addressing Mode,0-7
	SH1106_WRITECOMMAND(0xC8); //ok Set COM Output Scan Direction
	SH1106_WRITECOMMAND(0x00); //ok ---set low column address
	SH1106_WRITECOMMAND(0x10); //ok ---set high column address
	SH1106_WRITECOMMAND(0x40); //ok --set start line address
	SH1106_WRITECOMMAND(0x00); // Toan them // Start line : 0
	SH1106_WRITECOMMAND(0x81); //ok --set contrast control register
	SH1106_WRITECOMMAND(0xFF);
	SH1106_WRITECOMMAND(0xA1); //ok --set segment re-map 0 to 127
	SH1106_WRITECOMMAND(0xA6); //ok --set normal display
	SH1106_WRITECOMMAND(0xA8); //ok --set multiplex ratio(1 to 64)
	SH1106_WRITECOMMAND(0x3F); //
	SH1106_WRITECOMMAND(0xA4); //ok 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	SH1106_WRITECOMMAND(0xD3); //ok -set display offset
	SH1106_WRITECOMMAND(0x00); //ok -not offset
	SH1106_WRITECOMMAND(0xD5); //ok --set display clock divide ratio/oscillator frequency
	SH1106_WRITECOMMAND(0xF0); //--set divide ratio
	SH1106_WRITECOMMAND(0xD9); //--set pre-charge period
	SH1106_WRITECOMMAND(0x22); //
	SH1106_WRITECOMMAND(0xDA); //--set com pins hardware configuration
	SH1106_WRITECOMMAND(0x12);
	SH1106_WRITECOMMAND(0xDB); //--set vcomh
	SH1106_WRITECOMMAND(0x20); //0x20,0.77xVcc
	SH1106_WRITECOMMAND(0x8D); //--set DC-DC enable
	SH1106_WRITECOMMAND(0x14); //
	SH1106_WRITECOMMAND(0xAF); //ok --turn on SH1106 panel


	SH1106_WRITECOMMAND(SH1106_DEACTIVATE_SCROLL);

	/* Set default values */
	SH1106.CurrentX = 0;
	SH1106.CurrentY = 0;

	/* Clear screen */
	SH1106_Fill(SH1106_COLOR_BLACK);

	/* Update screen */
	SH1106_UpdateScreen();

	/* Initialized OK */
	SH1106.Initialized = 1;

	/* Return OK */
	return 1;
}

void SH1106_UpdateScreen(void) {
	uint8_t m;

	for (m = 0; m < 8; m++) {
		SH1106_WRITECOMMAND(0xB0 + m);
		SH1106_WRITECOMMAND(0x00);
		SH1106_WRITECOMMAND(0x10);

		/* Write multi data */
		sh1106_I2C_WriteMulti(SH1106_I2C_ADDR, 0x40, &SH1106_Buffer[SH1106_WIDTH * m], SH1106_WIDTH);
	}
}

void SH1106_ToggleInvert(void) {
	uint16_t i;

	/* Toggle invert */
	SH1106.Inverted = !SH1106.Inverted;

	/* Do memory toggle */
	for (i = 0; i < sizeof(SH1106_Buffer); i++) {
		SH1106_Buffer[i] = ~SH1106_Buffer[i];
	}
}

void SH1106_Fill(SH1106_COLOR_t color) {
	/* Set memory */
	memset(SH1106_Buffer, (color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SH1106_Buffer));
}

void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_COLOR_t color) {
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (SH1106.Inverted) {
		color = (SH1106_COLOR_t)!color;
	}

	/* Set color */
	if (color == SH1106_COLOR_WHITE) {
		SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= 1 << (y % 8);
	} else {
		SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
	}
}

void SH1106_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SH1106.CurrentX = x+2;
	SH1106.CurrentY = y;
}

char SH1106_Putc(char ch, FontDef_t* Font, SH1106_COLOR_t color) {
	uint32_t i, b, j;

	FontDef_t* _Font = &*Font ;
	if( _Font == &Font_5x7){

		/* Check available space in LCD */
		if (
			SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
			SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
		) {
			/* Error */
			return 0;
		}

		/* Go through font */
		for (i = 0; i < Font->FontHeight; i++) {
			b = Font->data[(ch - 32) * Font->FontHeight + i];

			for (j = Font->FontWidth; j > 0; j--) {

				if ((b << j) & 0x8000) {
					SH1106_DrawPixel(SH1106.CurrentX + i, (SH1106.CurrentY + (Font->FontWidth -j)), (SH1106_COLOR_t) color);
				} else {
					SH1106_DrawPixel(SH1106.CurrentX + i, (SH1106.CurrentY + (Font->FontWidth -j)), (SH1106_COLOR_t)!color);
				}
			}
		}

		/* Increase pointer */
		if( ch == ' '){
			SH1106.CurrentX += 4;
		}else{
			SH1106.CurrentX += Font->FontHeight + 1;
		}

		/* Return character written */
		return ch;
	}
	else{
		/* Check available space in LCD */
		if (
			SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
			SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)
		) {
			/* Error */
			return 0;
		}

		/* Go through font */
		for (i = 0; i < Font->FontHeight; i++) {
			b = Font->data[(ch - 32) * Font->FontHeight + i];
			for (j = 0; j < Font->FontWidth; j++) {
				if ((b << j) & 0x8000) {
					SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t) color);
				} else {
					SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR_t)!color);
				}
			}
		}

		/* Increase pointer */
		SH1106.CurrentX += Font->FontWidth;

		/* Return character written */
		return ch;
	}
}

char SH1106_Puts(char* str, FontDef_t* Font, SH1106_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (SH1106_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}


void SH1106_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SH1106_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= SH1106_WIDTH) {
		x0 = SH1106_WIDTH - 1;
	}
	if (x1 >= SH1106_WIDTH) {
		x1 = SH1106_WIDTH - 1;
	}
	if (y0 >= SH1106_HEIGHT) {
		y0 = SH1106_HEIGHT - 1;
	}
	if (y1 >= SH1106_HEIGHT) {
		y1 = SH1106_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			SH1106_DrawPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			SH1106_DrawPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		SH1106_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

void SH1106_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= SH1106_WIDTH) {
		w = SH1106_WIDTH - x;
	}
	if ((y + h) >= SH1106_HEIGHT) {
		h = SH1106_HEIGHT - y;
	}

	/* Draw 4 lines */
	SH1106_DrawLine(x, y, x + w, y, c);         /* Top line */
	SH1106_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	SH1106_DrawLine(x, y, x, y + h, c);         /* Left line */
	SH1106_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SH1106_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SH1106_COLOR_t c) {
	uint8_t i;

	/* Check input parameters */
	if (
		x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= SH1106_WIDTH) {
		w = SH1106_WIDTH - x;
	}
	if ((y + h) >= SH1106_HEIGHT) {
		h = SH1106_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		SH1106_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void SH1106_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SH1106_COLOR_t color) {
	/* Draw lines */
	SH1106_DrawLine(x1, y1, x2, y2, color);
	SH1106_DrawLine(x2, y2, x3, y3, color);
	SH1106_DrawLine(x3, y3, x1, y1, color);
}


void SH1106_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SH1106_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SH1106_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void SH1106_DrawCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SH1106_DrawPixel(x0, y0 + r, c);
    SH1106_DrawPixel(x0, y0 - r, c);
    SH1106_DrawPixel(x0 + r, y0, c);
    SH1106_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SH1106_DrawPixel(x0 + x, y0 + y, c);
        SH1106_DrawPixel(x0 - x, y0 + y, c);
        SH1106_DrawPixel(x0 + x, y0 - y, c);
        SH1106_DrawPixel(x0 - x, y0 - y, c);

        SH1106_DrawPixel(x0 + y, y0 + x, c);
        SH1106_DrawPixel(x0 - y, y0 + x, c);
        SH1106_DrawPixel(x0 + y, y0 - x, c);
        SH1106_DrawPixel(x0 - y, y0 - x, c);
    }
}

void SH1106_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SH1106_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SH1106_DrawPixel(x0, y0 + r, c);
    SH1106_DrawPixel(x0, y0 - r, c);
    SH1106_DrawPixel(x0 + r, y0, c);
    SH1106_DrawPixel(x0 - r, y0, c);
    SH1106_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SH1106_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        SH1106_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        SH1106_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        SH1106_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}



void SH1106_Clear (void)
{
	SH1106_Fill (0);
    SH1106_UpdateScreen();
}
void SH1106_ON(void) {
	SH1106_WRITECOMMAND(0x8D);
	SH1106_WRITECOMMAND(0x14);
	SH1106_WRITECOMMAND(0xAF);
}
void SH1106_OFF(void) {
	SH1106_WRITECOMMAND(0x8D);
	SH1106_WRITECOMMAND(0x10);
	SH1106_WRITECOMMAND(0xAE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  _____ ___   _____
// |_   _|__ \ / ____|
//   | |    ) | |
//   | |   / /| |
//  _| |_ / /_| |____
// |_____|____|\_____|
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void sh1106_I2C_Init() {
	//MX_I2C1_Init();
	uint32_t p = 250000;
	while(p>0)
		p--;
	//HAL_I2C_DeInit(&I2C_PORT);
	//p = 250000;
	//while(p>0)
	//	p--;
	//MX_I2C1_Init();
}

void sh1106_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
uint8_t dt[256];
dt[0] = reg;
uint8_t i;
for(i = 0; i < count; i++)
dt[i+1] = data[i];
HAL_I2C_Master_Transmit(&I2C_PORT, address, dt, count+1, 10);
}


void sh1106_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
	uint8_t dt[2];
	dt[0] = reg;
	dt[1] = data;
	HAL_I2C_Master_Transmit(&I2C_PORT, address, dt, 2, 10);
}
