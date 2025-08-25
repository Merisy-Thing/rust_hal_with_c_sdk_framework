#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL,
    gpio::{AltMode, Alternate, AnyPin, Input, Pull},
    print, println,
    pwm::{Pwm, PwmChannel, PwmPolarity},
};
use fugit::ExtU32;
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(_spawner: Spawner) -> ! {
    let p = CSDK_HAL::init();
    println!("hello embassy!");

    let key = Input::new(p.PB15.into::<AnyPin>(), Pull::Up);
    let mut _led = Alternate::new(p.PA8.into::<AnyPin>(), AltMode::AFPP);
    let pwm0 = Pwm::<2_000_000>::new(PwmChannel::CH0); //1K
    pwm0.set_polarity(PwmPolarity::ActiveHigh);
    pwm0.set_duty(100);
    pwm0.set_period(1_u32.millis());
    pwm0.enable();
    let max_duty = pwm0.get_max_duty();
    println!(
        "max_duty: {}, time:{}us",
        max_duty,
        pwm0.get_duty_time().to_micros()
    );
    let mut duty = 0;
    let mut period = 1_u32;
    loop {
        if key.is_low() {
            if duty < pwm0.get_max_duty() {
                duty += 100;
            } else {
                duty = 0;
                period += 1;
                pwm0.set_period(period.millis());
                println!(
                    "period: {}ms, max_duty {}",
                    pwm0.get_period().to_millis(),
                    pwm0.get_max_duty()
                );
            }
            pwm0.set_duty(duty);
            println!(
                "duty: {}, get duty: {} time:{}us",
                duty,
                pwm0.get_duty(),
                pwm0.get_duty_time().to_micros()
            );
            Timer::after_ticks(100 as u64).await;
        }
        Timer::after_ticks(100 as u64).await;
    }
}

#[no_mangle]
pub fn exti_irq_callback(line: u8) {
    print!("exti_irq_callback {}", line);
}
