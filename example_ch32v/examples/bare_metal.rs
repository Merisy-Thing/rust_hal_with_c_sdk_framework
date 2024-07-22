#![no_main]
#![no_std]

#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    self, ll_invoke, print, println,
    adc::{self, Adc, AdcBuffered, AdcChannel}, 
    gpio::{ Pin, PinNum, PinModeOutput, PinModeInput, PinModeAlternate, PortNum, ExtiMode, Port, PortModeOutput, }, 
    pwm::{Pwm, PwmChannel, PwmPolarity}, 
    tick::{Tick, Delay},
    usart::{self, Usart}
};

use embedded_hal::{self, delay::DelayNs, digital::*};
use ll_bind_ch32v20x as _;
use panic_halt as _;

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut tick = Delay::<1000>::new();

    let mut led = Pin::new(PortNum::PB, PinNum::Pin3).into_output(PinModeOutput::OutPP);
    let mut key = Pin::new(PortNum::PB, PinNum::Pin15).into_input(PinModeInput::InPullUp);

    loop {
        println!("hello world!");
        led.toggle().ok();

        //Timer::after_ticks(250 as u64).await;
        tick.delay_ms(500);

        if key.is_high().unwrap() {
            tick.delay_ms(250);
            //Timer::after_ticks(500 as u64).await;
        }
    }
}
