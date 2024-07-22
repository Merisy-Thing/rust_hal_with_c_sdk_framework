#![allow(non_camel_case_types)]
#![allow(dead_code)]

extern "C" {
    fn WWDG();
    fn EXTI11();
    fn FLASH();
    fn RCC();
    fn EXTI0();
    fn EXTI1();
    fn EXTI2();
    fn EXTI3();
    fn EXTI4();
    fn EXTI5();
    fn TIM1_BRK();
    fn ADC1();
    fn TIM1_UP_TRG_COM();
    fn TIM1_CC();
    fn TIM2();
    fn TIM6();
    fn EXTI6();
    fn EXTI7();
    fn I2C();
    fn SPI();
    fn UART1();
    fn UART2();
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
pub static __INTERRUPTS: [Vector; 29] = [
    Vector { _handler: WWDG },
    Vector { _reserved: 0 },
    Vector { _handler: EXTI11 },
    Vector { _handler: FLASH },
    Vector { _handler: RCC },
    Vector { _handler: EXTI0 },
    Vector { _handler: EXTI1 },
    Vector { _handler: EXTI2 },
    Vector { _handler: EXTI3 },
    Vector { _handler: EXTI4 },
    Vector { _handler: EXTI5 },
    Vector { _handler: TIM1_BRK },
    Vector { _handler: ADC1 },
    Vector {
        _handler: TIM1_UP_TRG_COM,
    },
    Vector { _handler: TIM1_CC },
    Vector { _handler: TIM2 },
    Vector { _reserved: 0 },
    Vector { _handler: TIM6 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _reserved: 0 },
    Vector { _handler: EXTI6 },
    Vector { _handler: EXTI7 },
    Vector { _handler: I2C },
    Vector { _reserved: 0 },
    Vector { _handler: SPI },
    Vector { _reserved: 0 },
    Vector { _handler: UART1 },
    Vector { _handler: UART2 },
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
unsafe impl cortex_m::interrupt::InterruptNumber for Interrupt {
    #[inline(always)]
    fn number(self) -> u16 {
        self as u16
    }
}
