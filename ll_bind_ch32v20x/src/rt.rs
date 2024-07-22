#![allow(non_camel_case_types)]
#![allow(dead_code)]

extern "C" {
    fn WWDG();
    fn PVD();
    fn TAMPER();
    fn RTC();
    fn FLASH();
    fn RCC();
    fn EXTI0();
    fn EXTI1();
    fn EXTI2();
    fn EXTI3();
    fn EXTI4();
    fn DMA1_CHANNEL1();
    fn DMA1_CHANNEL2();
    fn DMA1_CHANNEL3();
    fn DMA1_CHANNEL4();
    fn DMA1_CHANNEL5();
    fn DMA1_CHANNEL6();
    fn DMA1_CHANNEL7();
    fn ADC();
    fn USB_HP_CAN1_TX();
    fn USB_LP_CAN1_RX0();
    fn CAN1_RX1();
    fn CAN1_SCE();
    fn EXTI9_5();
    fn TIM1_BRK();
    fn TIM1_UP_();
    fn TIM1_TRG_COM();
    fn TIM1_CC();
    fn TIM2();
    fn TIM3();
    fn TIM4();
    fn I2C1_EV();
    fn I2C1_ER();
    fn I2C2_EV();
    fn I2C2_ER();
    fn SPI1();
    fn SPI2();
    fn USART1();
    fn USART2();
    fn USART3();
    fn EXTI15_10();
    fn RTCALARM();
    fn USBWAKE_UP();
    fn TIM8_BRK();
    fn TIM8_UP_();
    fn TIM8_TRG_COM();
    fn TIM8_CC();
    fn TIM5();
    fn SPI3();
    fn UART4();
    fn UART5();
    fn ETH();
    fn ETH_WKUP();
    fn OTG_FS();
    fn USBHSWAKEUP();
    fn USBHS();
    fn UART6();
    fn UART7();
    fn UART8();
    fn TIM9_BRK();
    fn TIM9_UP_();
    fn TIM9_TRG_COM();
    fn TIM9_CC();
    fn TIM10_BRK();
    fn TIM10_UP_();
    fn TIM10_TRG_COM();
    fn TIM10_CC();
}
#[doc(hidden)]
#[repr(C)]
pub union Vector {
    _handler: unsafe extern "C" fn(),
    _reserved: u32,
}

#[doc(hidden)]
#[link_section = ".vector_table.interrupts"]
#[no_mangle]
pub static __INTERRUPT_LIST: [Vector; 98] = [
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _handler: WWDG },
    Vector { _handler: PVD },
    Vector { _handler: TAMPER },
    Vector { _handler: RTC },
    Vector { _handler: FLASH },
    Vector { _handler: RCC },
    Vector { _handler: EXTI0 },
    Vector { _handler: EXTI1 },
    Vector { _handler: EXTI2 },
    Vector { _handler: EXTI3 },
    Vector { _handler: EXTI4 },
    Vector {
        _handler: DMA1_CHANNEL1,
    },
    Vector {
        _handler: DMA1_CHANNEL2,
    },
    Vector {
        _handler: DMA1_CHANNEL3,
    },
    Vector {
        _handler: DMA1_CHANNEL4,
    },
    Vector {
        _handler: DMA1_CHANNEL5,
    },
    Vector {
        _handler: DMA1_CHANNEL6,
    },
    Vector {
        _handler: DMA1_CHANNEL7,
    },
    Vector { _handler: ADC },
    Vector {
        _handler: USB_HP_CAN1_TX,
    },
    Vector {
        _handler: USB_LP_CAN1_RX0,
    },
    Vector { _handler: CAN1_RX1 },
    Vector { _handler: CAN1_SCE },
    Vector { _handler: EXTI9_5 },
    Vector { _handler: TIM1_BRK },
    Vector { _handler: TIM1_UP_ },
    Vector {
        _handler: TIM1_TRG_COM,
    },
    Vector { _handler: TIM1_CC },
    Vector { _handler: TIM2 },
    Vector { _handler: TIM3 },
    Vector { _handler: TIM4 },
    Vector { _handler: I2C1_EV },
    Vector { _handler: I2C1_ER },
    Vector { _handler: I2C2_EV },
    Vector { _handler: I2C2_ER },
    Vector { _handler: SPI1 },
    Vector { _handler: SPI2 },
    Vector { _handler: USART1 },
    Vector { _handler: USART2 },
    Vector { _handler: USART3 },
    Vector {
        _handler: EXTI15_10,
    },
    Vector { _handler: RTCALARM },
    Vector {
        _handler: USBWAKE_UP,
    },
    Vector { _handler: TIM8_BRK },
    Vector { _handler: TIM8_UP_ },
    Vector {
        _handler: TIM8_TRG_COM,
    },
    Vector { _handler: TIM8_CC },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _handler: TIM5 },
    Vector { _handler: SPI3 },
    Vector { _handler: UART4 },
    Vector { _handler: UART5 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _handler: ETH },
    Vector { _handler: ETH_WKUP },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _handler: OTG_FS },
    Vector {
        _handler: USBHSWAKEUP,
    },
    Vector { _handler: USBHS },
    Vector { _reserved: 0 },
    Vector { _handler: UART6 },
    Vector { _handler: UART7 },
    Vector { _handler: UART8 },
    Vector { _handler: TIM9_BRK },
    Vector { _handler: TIM9_UP_ },
    Vector {
        _handler: TIM9_TRG_COM,
    },
    Vector { _handler: TIM9_CC },
    Vector {
        _handler: TIM10_BRK,
    },
    Vector {
        _handler: TIM10_UP_,
    },
    Vector {
        _handler: TIM10_TRG_COM,
    },
    Vector { _handler: TIM10_CC },
];
#[doc = r"Enumeration of all the interrupts."]
#[derive(Copy, Clone, Debug, PartialEq, Eq)]
#[repr(u16)]
pub enum Interrupt {
    #[doc = "0 - Window Watchdog interrupt"]
    WWDG = 0,
    #[doc = "2 - AWUT WKP"]
    EXTI11 = 2,
    #[doc = "3 - Flash global interrupt"]
    FLASH = 3,
    #[doc = "4 - RCC global interrupt"]
    RCC = 4,
    #[doc = "5 - EXTI Line0 interrupt"]
    EXTI0 = 5,
    #[doc = "6 - EXTI Line1 interrupt"]
    EXTI1 = 6,
    #[doc = "7 - EXTI Line2 interrupt"]
    EXTI2 = 7,
    #[doc = "8 - EXTI Line3 interrupt"]
    EXTI3 = 8,
    #[doc = "9 - EXTI Line4 interrupt"]
    EXTI4 = 9,
    #[doc = "10 - EXTI Line5 interrupt"]
    EXTI5 = 10,
    #[doc = "11 - TIM1 Break interrupt"]
    TIM1_BRK = 11,
    #[doc = "12 - ADC interrupt, combined with EXTI Line8"]
    ADC1 = 12,
    #[doc = "13 - TIM1 Update/Trigger Com interrupt"]
    TIM1_UP_TRG_COM = 13,
    #[doc = "14 - TIM1 Capture/Compare interrupt"]
    TIM1_CC = 14,
    #[doc = "15 - TIM2 global interrupt"]
    TIM2 = 15,
    #[doc = "17 - TIM6 global interrupt"]
    TIM6 = 17,
    #[doc = "21 - EXTI Line6 interrupt"]
    EXTI6 = 21,
    #[doc = "22 - EXTI Line7 interrupt"]
    EXTI7 = 22,
    #[doc = "23 - I2C global interrupt, combined with EXTI Line 10"]
    I2C = 23,
    #[doc = "25 - SPI global interrupt"]
    SPI = 25,
    #[doc = "27 - UART1 global interrupt"]
    UART1 = 27,
    #[doc = "28 - UART2 global interrupt"]
    UART2 = 28,
}
