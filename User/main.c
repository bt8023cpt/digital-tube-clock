#include "reg52.h"
#include "main.h"
#include "bsp_delay.h"
#include "bsp_key.h"

#define Segment P0
#define Digital P2

/* 定义模型变量 */
u8 location = 0;								// 记录当期是哪一位闪烁
u8 value[4] = {1, 2, 3, 4};

/* 数码管段选码、位选码 */
u8 SegCode[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};	// 段选高电平有效
u8 DigCode[4] = {0xfe, 0xfd, 0xfb, 0xf7};					// 位选低电平有效

int main()
{
        u8 i, j;
        
        /* 这里设置的值是表示，10次扫描全部显示，10次扫描location位不显示 */
        u8 dispalyCount = 10;
        
        IT0 = 1;
        IE = 0x81;
        
        while(1)
        {
                /* 这里扫描的时候是正常显示6位 */
                for(j = dispalyCount; j > 0; j--)
                {
                        for(i = 0; i < 4; i++)
                        {
                                Digital = DigCode[i];
                                
                                /* 这是是在第一位位置上添加小数点 */
                                if(i == 1)
                                {
                                        /* 通过按位与操作，在不改变低7位的前提下，将位7置为高电平 */
                                        Segment = SegCode[value[i]] | 0x80;
                                }
                                else
                                {
                                        Segment = SegCode[value[i]];
                                }
                                delay_ms(1);
                        }
                }
                
                /* 这里扫描的时候location对应位不显示，只显示其他位 */
                for(j = dispalyCount; j > 0; j--)
                {
                        for(i = 0; i < 4; i++)
                        {
                                /* 判断当期扫描的位是不是location对应位 */
                                if(location == i)
                                {
                                        /* 如果是的话，不选中任何位选，也就是全灭 */
                                        Digital = 0xff;
                                }
                                else
                                {
                                        /* 对于其他为就正常显示就好 */
                                        Digital = DigCode[i];
                                }
                                
                                /* 这里和上面是一样的，加小数点的操作 */
                                if(i == 1)
                                {
                                        Segment = SegCode[value[i]] | 0x80;
                                }
                                else
                                {
                                        Segment = SegCode[value[i]];
                                }
                                delay_ms(1);
                        }
                }
                
        }
}

/**
 * @Descroption 外部中断0服务函数
 */ 
void INT0_Handler() interrupt 0
{
        Key_Control(Key_Scan());
}
