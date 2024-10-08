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
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(_spawner: Spawner) -> ! {
    let _tick: Tick = Tick::now();

    let adc_ch1 = Adc::new(AdcChannel::CH1, None);

    println!("hello embassy!");
    loop {
        Timer::after_ticks(1000 as u64).await;

        let vref = adc_ch1.single_convert();
        let mut vcc = 0;
        if vref > 0 {
            vcc = 1200_u32 * 4095_u32 / vref as u32;
        }
        println!("vref={}, vcc={}mV", vref, vcc);
    }
}
