#include "bsp_ds1302.h"
#include "bsp_delay.h"

#include <reg51.h>
#include <intrins.h>

sbit DS1302_IO   = P3 ^ 3;
sbit DS1302_SCLK = P3 ^ 4;
sbit DS1302_RST  = P3 ^ 5;

void posedge()
{
        DS1302_SCLK = 0;
        delay_us(1);
        DS1302_SCLK = 1;
}

void negedge()
{
        DS1302_SCLK = 1;
        delay_us(1);
        DS1302_SCLK = 0;
}

void DS1302_Write(u8 date)
{
        u8 i;
        for(i = 0; i < 8; i++)
        {
                /* 先写入低位 */
                DS1302_IO = date & 0x01;
                
                /* 上升沿时刻写入 */
                posedge();
                
                /* 数据在上升沿以前准备好 */
                date >>= 1;
        }
}

u8 DS1302_Read()
{
        u8 i, date = 0;
        for(i = 0; i < 8; i++)
        {
                if(DS1302_IO)
                {
                        date |= 0x80;
                }
                negedge();
                date >>= 1;
        }
        return date;
}

u8 DS1302_ReadRegister(u8 addr)
{
        u8 Date;
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        DS1302_RST = 1;
        DS1302_Write(addr);
        Date = DS1302_Read();
        DS1302_SCLK = 1;
        DS1302_RST = 0;
        return (Date);
}

void DS1302_WriteRegister(u8 addr, u8 Date)
{
        DS1302_RST = 0;
        DS1302_SCLK = 0;
        DS1302_RST = 1;
        DS1302_Write(addr);
        DS1302_Write(Date);
        DS1302_SCLK = 1;
        DS1302_RST = 0;
}
