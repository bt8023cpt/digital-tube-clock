#ifndef __MAIN_H
#define __MAIN_H

/* 类型重定义 */
#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long
        
enum display_mode
{
        ClockDisplay = 0,
        ClockSet = 1,
        DateDisplay = 2,
        DateSet = 3,
        YearDisplay = 4,
        YearSet = 5,
        WeekDisplay = 6,
        WeekSet = 7,
        AlarmDisplay = 8,
        AlarmSet = 9,
        TemperatureDisplay = 10
};

enum flicker_state
{
        light = 0,              // 数码管常亮显示
        flicker = 1             // 数码管闪烁显示
};

#endif /* __MAIN_H */