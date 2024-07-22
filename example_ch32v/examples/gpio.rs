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

use embassy_executor::Spawner;
use embassy_time::Timer;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(_spawner: Spawner) -> ! {
    let mut delay = Delay::<1000>::new();
    let _tick: Tick = Tick::now();

    let mut led = Pin::new(PortNum::PB, PinNum::Pin3).into_output(PinModeOutput::OutPP);
    let mut key = Pin::new(PortNum::PB, PinNum::Pin15).into_input(PinModeInput::InPullUp);

    println!("hello embassy!");
    loop {
        led.toggle().ok();

        Timer::after_ticks(100 as u64).await;
        delay.delay_ms(100);

        if key.is_low().unwrap() {
            Timer::after_ticks(500 as u64).await;
        }
    }
}

#[no_mangle]
pub fn exti_irq_callback(line: u8) {
    print!("exti_irq_callback {}", line);
}
