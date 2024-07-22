#![no_main]
#![no_std]
mod interrupt;

#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    println,
    pwm::{Pwm, PwmChannel, PwmPolarity},
    gpio::{ Pin, PinNum, PinModeInput, PinModeOutput, Port, PortNum, PortModeInput, ExtiMode},
    tick::{Tick, Delay}
};
use embedded_hal::{self, delay::DelayNs, digital::*};
use ll_bind_hk32F0301mxxc as mcu;
use panic_halt as _;

#[no_mangle]
pub fn exti_irq_callback(_line: u8) {
    let mut led = Pin::new(PortNum::PD, PinNum::Pin1).into_output(PinModeOutput::OutPP);
    for _ in 0..5 {
        led.toggle().ok();
        for _ in 0..300001 {
            unsafe {
                core::arch::asm!("nop");
            }
        }
    }
}

#[cortex_m_rt::entry]
fn main() -> ! {
    mcu::ll_drv_init();
    let mut delay = Delay::<1000>::new();

    let mut led = Pin::new(PortNum::PD, PinNum::Pin2).into_output(PinModeOutput::OutPP);
    let mut key = Pin::new(PortNum::PD, PinNum::Pin4).into_input(PinModeInput::InPullUp);

    loop {
        //println!("hello world!");
        led.toggle().ok();

        //Timer::after_ticks(250 as u64).await;
        delay.delay_ms(250);

        if key.is_high().unwrap() {
            delay.delay_ms(250);
            //Timer::after_ticks(500 as u64).await;
        }
    }
}