# Rust HAL with C SDK Framework

## 介绍
---
这是一个Rust HAL与 MCU C SDK/BSP 绑定的框架。

## 说明
---
### 启动和运行时
 - 直接使用cortexm-rt或riscv-rt，参考 **_ll_bind_hk32F0301mxxc_** 的实现
 - 使用SDK/BSP的startup_XXX.S，参考 **_ll_bind_ch32v20x_** 的实现

### 中断
 - 可以全用C实现，方便清除标志位
 - 通过rust定义的hook函数回调处理HAL或应用数据

### HAL hook
 - EXTI： **fn EXTI_IRQ_hook_rs(line: u8)** ，参考  **_ll_bind_hk32F0301mxxc\csrc\interrupt.c_** 
 - ADC： **fn ADC_CH{ch}_EOC_hook_rs(val:AdcDataType)** ，参考  **_ll_bind_ch32v20x\csrc\adc.c_** 
 - USART:  **fn USART{id}_rx_hook_rs(val: u8)** ，参考  **_ll_bind_ch32v20x\csrc\usart.c_** 
 - Tick:  **fn sys_tick_inc()** ，参考  **_ll_bind_ch32v20x\csrc\ll_api.c_**  和  **_example_hk32\src\interrupt.rs_**  

### Features
 -  **print-log**  = []： print!,println!串口打印日志
 - **tick-size-64bit** = []： tick计数器使用64位，默认32位
 - **embassy** = [] ： embassy定时器和gpio输入异步支持
 - **USART-[0..7]** = [] ： 串口接收缓存和回调ID定义
 - **adc-data-type-u8** = [] ： 8位ADC数据类型，默认16位
 - **adc-buffered-ch[0..7]** = [] ： ADC缓存和回调定义
 - **exti-irq-callback** = [] ： exti中断回调，需要由APP定义

### 架构
---
![输入图片说明](doc/framework.png)