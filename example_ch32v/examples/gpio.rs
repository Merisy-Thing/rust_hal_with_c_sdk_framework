#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL,
    gpio::{AnyPin, Input, Level, Output, Pull},
    print, println,
    tick::{Delay, Tick},
};
use embedded_hal::{self, delay::DelayNs};
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(_spawner: Spawner) -> ! {
    let p = CSDK_HAL::init();
    let mut delay = Delay::new();
    let _tick: Tick = Tick::now();

    let mut led = Output::new(p.PB3.into::<AnyPin>(), Level::Low);
    let key = Input::new(p.PB15.into::<AnyPin>(), Pull::Up);

    println!("hello embassy!");
    loop {
        led.toggle();

        Timer::after_ticks(100 as u64).await;
        delay.delay_ms(100);

        if key.is_low() {
            Timer::after_ticks(500 as u64).await;
        }
    }
}

#[no_mangle]
pub fn exti_irq_callback(line: u8) {
    print!("exti_irq_callback {}", line);
}
