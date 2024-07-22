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

use embassy_executor::Spawner;
use embassy_time::Timer;

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

#[embassy_executor::main]
async fn main(spawner: Spawner) -> ! {
    mcu::ll_drv_init();
    let mut delay = Delay::<1000>::new();

    let _tick = Tick::now();
    let _ = spawner.spawn(task_1());
    let mut led0 = Pin::new(PortNum::PC, PinNum::Pin7).into_output(PinModeOutput::OutPP);
    led0.set_low().ok();

    let mut led = Pin::new(PortNum::PC, PinNum::Pin3).into_output(PinModeOutput::OutPP);
    let mut key = Pin::new(PortNum::PD, PinNum::Pin4).into_input(PinModeInput::InPullUp);
    key.exti_config(ExtiMode::Falling);

    loop {
        println!("hello main!");
        led.toggle().ok();

        Timer::after_ticks(200 as u64).await;

        // if key.is_low().unwrap() {
        //     Timer::after_ticks(500 as u64).await;
        // }
        key.wait_for_falling_edge().await;
        delay.delay_ms(100);
    }
}

#[embassy_executor::task]
async fn task_1() {
    loop {
        println!("hello task_1!");

        Timer::after_ticks(500 as u64).await;
    }
}
