/*
 * test.c
 *
 *  Created on: 17-Jun-2019
 *      Author: poe
 */

#include "test.h"
#include "STM32_sh1106_oled.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

void TestLines(uint8_t color)
{
    uint8_t x1, y1, x2, y2;

    SH1106_Clear ();

    x1 = y1 = 0;
    y2    = SH1106_HEIGHT - 1;
    for (x2 = 0; x2 < SH1106_WIDTH; x2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		HAL_Delay(1);
    		SH1106_UpdateScreen();
    	}
    x2    = SH1106_WIDTH - 1;
    for (y2 = 0; y2 < SH1106_HEIGHT; y2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		HAL_Delay(1);
    		SH1106_UpdateScreen();
    	}

    HAL_Delay(10);


    SH1106_Clear ();

    x1    = SH1106_WIDTH - 1;
    y1    = 0;
    y2    = SH1106_HEIGHT - 1;
    for (x2 = 0; x2 < SH1106_WIDTH; x2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		SH1106_UpdateScreen();
    		HAL_Delay(1);
    	}
    x2    = 0;
    for (y2 = 0; y2 < SH1106_HEIGHT; y2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		SH1106_UpdateScreen();
    		HAL_Delay(1);
    	}

    HAL_Delay(10);


    SH1106_Clear ();

    x1    = 0;
    y1    = SH1106_HEIGHT - 1;
    y2    = 0;
    for (x2 = 0; x2 < SH1106_WIDTH; x2 += 6)
    {
    	SH1106_DrawLine(x1, y1, x2, y2, color);
    	HAL_Delay(1);
    	SH1106_UpdateScreen();
    }
    x2    = SH1106_WIDTH - 1;
    for (y2 = 0; y2 < SH1106_HEIGHT; y2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		HAL_Delay(1);
    		SH1106_UpdateScreen();
    	}

    HAL_Delay(10);


    SH1106_Clear ();

    x1    = SH1106_WIDTH - 1;
    y1    = SH1106_HEIGHT - 1;
    y2    = 0;
    for (x2 = 0; x2 < SH1106_WIDTH; x2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		HAL_Delay(1);
    		SH1106_UpdateScreen();
    	}
    x2    = 0;
    for (y2 = 0; y2 < SH1106_HEIGHT; y2 += 6)
    	{
    		SH1106_DrawLine(x1, y1, x2, y2, color);
    		HAL_Delay(1);
    		SH1106_UpdateScreen();
    	}
}


void TestRectangles (uint8_t color)
{
    uint8_t n, i, i2;

    SH1106_Clear ();

    n = min(SH1106_WIDTH, SH1106_HEIGHT);

    for (i = 2; i < n; i += 6)
    {
        i2 = i / 2;
        SH1106_DrawRectangle((SH1106_WIDTH/2) - i2, (SH1106_HEIGHT/2) - i2, i, i, color);
        SH1106_UpdateScreen();
        HAL_Delay (10);
    }
}

void TestFilledRectangles (uint8_t color)
{
    uint8_t n, i, i2,
            cx = SH1106_WIDTH / 2 - 1,
            cy = SH1106_HEIGHT / 2 - 1;

    SH1106_Clear ();

    n = min(SH1106_WIDTH, SH1106_HEIGHT);
    for (i = n; i > 0; i -= 6)
    {
        i2    = i / 2;
        SH1106_DrawFilledRectangle(cx - i2, cy - i2, i, i, color);
        SH1106_UpdateScreen();
        HAL_Delay (10);
    }
}

void TestFilledCircles(uint8_t radius, uint16_t color)
{
    uint8_t x, y, w = SH1106_WIDTH, h = SH1106_HEIGHT, r2 = radius * 2;

    SH1106_Clear ();

    for (x = radius; x < w; x += r2)
    {
        for (y = radius; y < h; y += r2)
        {
            SH1106_DrawFilledCircle(x, y, radius, color);
            SH1106_UpdateScreen();
            HAL_Delay (10);
        }
    }
}

void TestCircles(uint8_t radius, uint16_t color)
{
	SH1106_Clear ();

	uint8_t x, y, r2 = radius * 2,
            w = SH1106_WIDTH + radius,
            h = SH1106_HEIGHT + radius;

    for (x = 0; x < w; x += r2)
    {
        for (y = 0; y < h; y += r2)
        {
            SH1106_DrawCircle(x, y, radius, color);
            SH1106_UpdateScreen();
            HAL_Delay (10);
        }
    }
}

void TestTriangles(uint8_t color)
{
    uint8_t n, i, cx = SH1106_WIDTH  / 2 - 1,
            cy = SH1106_HEIGHT / 2 - 1;

    SH1106_Clear ();

    n = min(cx, cy);
    for (i = 0; i < n; i += 5)
    {
        SH1106_DrawTriangle(cx , cy - i, cx - i, cy + i, cx + i, cy + i, color);
        SH1106_UpdateScreen();
        HAL_Delay (10);
    }

}




