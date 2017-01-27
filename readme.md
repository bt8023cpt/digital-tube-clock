# 基于 STC89C52RC 的可调式电子日历时钟

## 项目简介

本项目基于 STC89C52RC 处理器 + DS1302 时钟芯片 + DS18B20 温湿度传感器，采用四位数码管作为显示装置，通过六个按键输入控制信号，另外通过控制一个有源蜂鸣器作为闹钟响铃设备，制作了一个简单的电子表。

## 功能介绍

### 显示模式分别有
- 时钟显示模式：显示当前时间。示例 12:20
- 时钟设置模式：设置当期时间。
- 日期显示模式：显示当前日期。示例 01.22
- 日期设置模式：设置当前日期。
- 年份显示模式：显示当前年份。示例 2017
- 年份设置模式：设置当前年份。
- 星期显示模式：显示当前是星期几。示例 --3-
- 星期设置模式：设置当前是星期几。
- 闹钟显示模式：显示闹钟设定时间。示例 07:10
- 闹钟设置模式：设置闹钟响铃时刻。
- 温度显示模式：显示当前温度。示例 23.6C

### 六个按键功能分别为：
- K0 - 左/年份
- K1 - 右/日期
- K2 - 设置
- K3 - 切换/星期
- K4 - 加/闹钟
- K5 - 减/温度

### 功能概述

上电直接进入显示模式，显示模式下可以显示当期的年份、日期、星期几、设置的闹钟值以及当前的温度值。

显示模式默认为时钟显示模式，此时4位数码管显示当前时刻，格式为 12:20 。在在该模式下，6个按键分别为(K0-K5)：查看年份、查看日期、进入设置模式、查看星期、查看闹钟设定值、查看当前温度。此时，按下设置按键 K2 则将进入时钟设置模式，按下其他的查看类按键，数码管将显示对应功能查询结果，并维持两秒时间，之后自动返回到时钟显示模式。

在时钟显示界面下按下设置按键按键即可进入设置模式，设置模式下当前设置的位会闪烁显示，指示当前设置的是哪一位。设置模式下可以设置当前的年份、日期、星期以及当前的时间。

设置模式默认为时钟设置模式，此时可以通过上下加减4个按键设置值。通过切换键可以切换到年份、日期、星期的设置，设置过程也是通过下加减4个按键设置相应的值。设置完成之后，再次按下设置按键 K2 退出设置模式，进入显示模式。此时显示模式下显示的值已经被更新为刚刚的设置值了。

## 本工程使用到的相关资源

### 硬件资源
- STC89C52RC 单片机 一块
- DS1302 时钟芯片 一块
- DS18B20 温湿度传感器 一块
- 4位数码管 一个
- 轻触按键 六个
- HD74LS21P 四输入与门芯片 一块
- 有源蜂鸣器 一个
- 面包板 两块
- PL2303 USB转TTL模块 一块 (下载程序用)
- 12MHz 晶体振荡器 一个
- 32.768kHz 晶体振荡器 一个
- 33pF 电容 两个
- A05-103 排阻 一个
- A09-103 排阻 两个
- ... ...
- 未列举完全，待后期追加补充

### 软件环境
- Proteus 7.10 电路仿真软件
- Altium Designer 6.9 电路板绘制软件
- Keil v5.20.0.39 程序编写软件
