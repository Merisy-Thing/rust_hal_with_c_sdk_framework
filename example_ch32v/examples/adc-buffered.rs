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
    let adc_ch0 = AdcBuffered::new(AdcChannel::CH0, &adc::ADC_CH0_DATA, None);

    let mut adc_ch0_buf: [u16; 16] = [0; 16];
    adc_ch0.set_buf(&mut adc_ch0_buf);

    adc_ch0.start();
    println!("hello embassy!");
    let mut adc_count = 0;
    loop {
        println!("adc buf: {:?}", &adc_ch0_buf);
        Timer::after_ticks(1000 as u64).await;

        if adc_count < 10 {
            adc_count += 1;
            println!("adc_count = {}", adc_count);
            if adc_count == 10 {
                adc_ch0.stop();
            }
        }
    }
}
