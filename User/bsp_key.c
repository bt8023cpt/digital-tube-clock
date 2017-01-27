#include "bsp_key.h"
#include "bsp_delay.h"
#include "bsp_ds1302.h"

/* 这里需要使用 extern 声明其他 c 文件定义的变量 */
extern u8 value[4];
extern u8 alarm[4];
extern u8 location;

extern enum display_mode state;
extern enum flicker_state flag;

extern u8 InterruptCount;

/** 
 * @Description 按键扫描函数(8个按键同时扫描)
 * @return u8   按键键值
 */
u8 Key_Scan(void)
{
        /* 保存按键键值的变量，默认值0xff表示没有按键按下 */
        u8 keyValue = 0xff;

        /* 用于松手检测的累加变量 */
        int i = 0;

        /* 这里在1ms前后检测两边是为了滤除按键抖动产生的尖峰脉冲 */
        if(GPIO_KEY != 0xff)
        {
                delay(1);

                /* 如果1ms前后检测都是低电平的话，就说明是真的有按键按下 */
                if(GPIO_KEY != 0xff)
                {
                        /* 真的有按键按下，则将按键键值存入keyValue中 */
                        keyValue = GPIO_KEY;

                        /* 这里是松手检测，在20ms内按键没有放开，程序会一直停在此处 */
                        /* 倘若50ms期间，松开了按键，则会跳出此while循环 */
                        /* 也就是说：按键没有松开的话，程序不会去做其他的事情 */
                        /* 当然也有松手检测的超时时间，就是我们设置的20ms */
                        while((i < 20) && (GPIO_KEY != 0xff))
                        {
                                delay(1);
                                i++;
                        }
                }
        }

        /* 返回按键的键值，通过解析这个键值，就能知道是哪一个按键按下 */
        return keyValue;
}

/** 
 * @Description 简单延时函数，它存在的目的是为了解决函数重入的问题
 */
void delay(u32 t)
{
        u32 i, j;
        for(i = t; i > 0; i--)
                for(j = 296; j > 0; j--);
}

/** 
 * @Description 按键控制函数(8个按键同时控制)
 * @param u8    按键扫描到的键值
 */
void Key_Control(u8 keyValue)
{
        u8 i, tmp1, tmp2;
        
        /* 运行界面下按键的功能 */
        if(light == flag)
        {
                /* 根据捕捉的键值解析出按下的键，并作出相应的处理 */
                switch(keyValue)
                {
                        /* 1111 1110 P1.0口对应按键按下 K0 左/年 */
                        case(0xfe):
                                tmp1 = DS1302_ReadRegister(DS1302_YEAR | DS1302_READ);
                        
                                value[0] = 2;
                                value[1] = 0;
                                value[2] = (tmp1 & 0xf0) >> 4;
                                value[3] = tmp1 & 0x0f;
                                
                                InterruptCount = 0;
                                TR0 = 1;
                        
                                state = YearDisplay;
                                break;
                        
                        /* 1111 1101 P1.1口对应按键按下 K1 右/日期 */
                        case(0xfd):     
                                tmp1 = DS1302_ReadRegister(DS1302_MONTH | DS1302_READ);
                                tmp2 = DS1302_ReadRegister(DS1302_DATE | DS1302_READ);
                                
                                value[0] = (tmp1 & 0xf0) >> 4;
                                value[1] = tmp1 & 0x0f;
                                value[2] = (tmp2 & 0xf0) >> 4;
                                value[3] = tmp2 & 0x0f;
                        
                                InterruptCount = 0;
                                TR0 = 1;
                        
                                state = DateDisplay;
                                break;
                        
                        /* 1111 1011 P1.2口对应按键按下 K2 设置 */
                        case(0xfb):     
                                flag = flicker;
                                state = ClockSet;
                                break;
                        
                        /* 1111 0111 P1.3口对应按键按下 K3 切换/星期 */
                        case(0xf7):
                                tmp1 = DS1302_ReadRegister(DS1302_DAY | DS1302_READ);
                        
                                value[0] = 16;
                                value[1] = 16;
                                value[2] = tmp1 & 0x0f;
                                value[3] = 16;
                                
                                InterruptCount = 0;
                                TR0 = 1;
                                
                                state = WeekDisplay;
                                break;
                        
                        /* 1110 1111 P1.4口对应按键按下 K4 加/闹钟 */
                        case(0xef):     
                                for(i = 0; i < 4; i++)
                                {
                                        value[i] = alarm[i];
                                }
                        
                                InterruptCount = 0;
                                TR0 = 1;
                                
                                state = AlarmDisplay;
                                break;
                                
                        /* 1101 1111 P1.5口对应按键按下 K5 减/温度 */
                        case(0xdf):
                                // DS18B20_GetTemperature();
                                
                                InterruptCount = 0;
                                TR0 = 1;

                                state = TemperatureDisplay;
                                break;
                }
        }
        else if(flicker == flag) /* 设置界面下按键的功能 */
        {
                switch(keyValue)
                {
                        /* 1111 1110 P1.0口对应按键按下 K0 左/年 */
                        case(0xfe):
                                if(location > 0)
                                {
                                        location -= 1;
                                }
                                break;
                        
                        /* 1111 1101 P1.1口对应按键按下 K1 右/日期 */
                        case(0xfd):     
                                if(location < 3)
                                {
                                        location += 1;
                                }
                                break;
                        
                        /* 1111 1011 P1.2口对应按键按下 K2 设置 */
                        case(0xfb):     
                                flag = light;
                                state = ClockDisplay;
                                break;
                        
                        /* 1111 0111 P1.3口对应按键按下 K3 切换/星期 */
                        case(0xf7):
                                if(ClockSet == state)
                                {
                                        DS1302_WriteRegister(DS1302_HR | DS1302_WRITE, (value[0] << 4) | value[1]);
                                        DS1302_WriteRegister(DS1302_MIN | DS1302_WRITE, (value[2] << 4) | value[3]);
                                        
                                        tmp1 = DS1302_ReadRegister(DS1302_YEAR | DS1302_READ);
                                        
                                        value[0] = 2;
                                        value[1] = 0;
                                        value[2] = (tmp1 & 0xf0) >> 4;
                                        value[3] = tmp1 & 0x0f;
                                        
                                        state = YearSet;
                                }
                                else if(YearSet == state)
                                {
                                        DS1302_WriteRegister(DS1302_YEAR | DS1302_WRITE, (value[2] << 4) | value[3]);
                                        
                                        tmp1 = DS1302_ReadRegister(DS1302_MONTH | DS1302_READ);
                                        tmp2 = DS1302_ReadRegister(DS1302_DATE | DS1302_READ);
                                        
                                        value[0] = (tmp1 & 0xf0) >> 4;
                                        value[1] = tmp1 & 0x0f;
                                        value[2] = (tmp2 & 0xf0) >> 4;
                                        value[3] = tmp2 & 0x0f;
                                        
                                        state = DateSet;
                                }
                                else if(DateSet == state)
                                {
                                        DS1302_WriteRegister(DS1302_MONTH | DS1302_WRITE, (value[0] << 4) | value[1]);
                                        DS1302_WriteRegister(DS1302_DATE | DS1302_WRITE, (value[2] << 4) | value[3]);
                                        
                                        tmp1 = DS1302_ReadRegister(DS1302_DAY | DS1302_READ);
                                        
                                        value[0] = 16;
                                        value[1] = 16;
                                        value[2] = tmp1 & 0x0f;
                                        value[3] = 16;
                                        
                                        state = WeekSet;
                                }
                                else if(WeekSet == state)
                                {
                                        DS1302_WriteRegister(DS1302_DAY | DS1302_WRITE, (value[2] << 4) | value[3]);
                                        
                                        tmp1 = DS1302_ReadRegister(DS1302_HR | DS1302_READ);
                                        tmp2 = DS1302_ReadRegister(DS1302_MIN | DS1302_READ);
                                        
                                        value[0] = (tmp1 & 0xf0) >> 4;
                                        value[1] = tmp1 & 0x0f;
                                        value[2] = (tmp2 & 0xf0) >> 4;
                                        value[3] = tmp2 & 0x0f;
                                        state = ClockSet;
                                }
                                break;
                        
                        /* 1110 1111 P1.4口对应按键按下 K4 加/闹钟 */
                        case(0xef):     
                                switch(location)
                                {
                                        case (0):
                                                if(value[0] < 2)
                                                {
                                                        value[0] += 1;
                                                }

                                                if(value[0] == 2 && value[1] > 3)
                                                {
                                                        value[1] = 3;
                                                }
                                                break;
                                        case (1):
                                                if(value[0] < 2 && value[1] < 9)
                                                {
                                                        value[1] += 1;
                                                }
                                                else if(value[0] == 2 && value[1] < 3)
                                                {
                                                value[1] += 1;
                                                }
                                                break;
                                        case (2):
                                                if(value[location] < 5)
                                                {
                                                        value[location] += 1;
                                                }
                                                break;
                                        case (3):
                                                if(value[location] < 9)
                                                {
                                                        value[location] += 1;
                                                }
                                                break;
                                }
                                break;
                                
                        /* 1101 1111 P1.5口对应按键按下 K5 减/温度 */
                        case(0xdf):
                                if(value[location] > 0)
                                {
                                value[location] -= 1;
                                }
                                break;
                }
        }
}
