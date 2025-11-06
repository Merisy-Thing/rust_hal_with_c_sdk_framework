#![no_main]
#![no_std]
mod interrupt;

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAD, gpio::{ AnyPin, ExtiMode, Input, Level, Output, Pull}, println, tick::{Delay, Tick}
};

use embedded_hal::{self, delay::DelayNs};
use ll_bind_hk32F0301mxxc as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;

#[no_mangle]
pub fn exti_irq_callback(line: u8) {
    println!("exti_irq_callback, line: {}", line);
}

#[embassy_executor::main]
async fn main(spawner: Spawner) -> ! {
    let p = CSDK_HAD::init();
    let mut delay = Delay::new();

    let _tick = Tick::now();
    let _ = spawner.spawn(task_1());
    let mut led0 = Output::new(p.PC7.into::<AnyPin>(), Level::Low);
    led0.set_low();

    let mut led = Output::new(p.PC3.into::<AnyPin>(), Level::Low);
    let mut key = Input::new(p.PD4.into::<AnyPin>(), Pull::Up);
    key.exti_config(ExtiMode::Falling);

    loop {
        println!("hello main!");
        led.toggle();

        Timer::after_ticks(200 as u64).await;

        // if key.is_low().unwrap() {
        //     Timer::after_ticks(500 as u64).await;
        // }
        key.wait_for_falling_edge().await;
        delay.delay_ms(100);
    }
}

#[allow(dead_code)]
#[derive(Debug)]
struct Test {
    item1: u32,
    item2: u32,
}

#[embassy_executor::task]
async fn task_1() {
    loop {
        let test1 = Test{item1: 1, item2: 2};
        let test2 = Test{item1: 3, item2: 4};

        println!("hello task_1!>{:?} {:?}<", test1, test2);

        Timer::after_ticks(500 as u64).await;
    }
}
