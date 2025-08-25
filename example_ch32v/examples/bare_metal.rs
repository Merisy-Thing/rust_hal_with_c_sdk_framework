#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL,
    gpio::{AnyPin, Input, Level, Output, Pull},
    println, setInterval,
    tick::{Delay, Tick},
};

use embedded_hal::{self, delay::DelayNs};
use ll_bind_ch32v20x as _;
use local_static::LocalStatic;
use panic_halt as _;

fn tick_callback<const PERIOD_MS: u64, F>(f: F)
where
    F: FnOnce(),
{
    static mut TASK_TICK: Tick = Tick::with_value(0);

    if unsafe { TASK_TICK.elapsed_time().to_millis() } > PERIOD_MS {
        unsafe {
            TASK_TICK = Tick::now();
        }
        f()
    }
}

#[riscv_rt_macros::entry]
fn main() -> ! {
    let p = CSDK_HAL::init();
    let mut tick = Delay::new();

    let mut led = Output::new(p.PA8.into::<AnyPin>(), Level::Low);
    let key = Input::new(p.PB15.into::<AnyPin>(), Pull::Up);

    let mut count = 0;
    let mut count_2000 = 0;
    loop {
        count += 1;

        led.toggle();

        if key.is_low() {
            tick.delay_ms(250);
        }

        tick_callback::<1000, _>(|| {
            println!(
                "timer_task0 1000 count = {} {:?}",
                count,
                Tick::now().elapsed()
            );
        });

        setInterval!(
            || {
                count_2000 += 1;
                println!(
                    "setInterval 2000 count_2000 = {} {:?}",
                    count_2000,
                    Tick::now().elapsed()
                );
            },
            2000
        );

        setInterval!(
            || {
                println!(
                    "setInterval 3000 count = {} {:?}",
                    count,
                    Tick::now().elapsed()
                );
            },
            3000
        );

        setInterval!(call_back_1, 500, 123);
        setInterval!(call_back_2, 500, 123, 456);

        {
            static TICK: LocalStatic<Tick> = LocalStatic::new();

            if TICK.get().elapsed_time().to_millis() >= 800 {
                println!("LocalStatic {}", TICK.get().elapsed_time().to_millis());
                TICK.set(Tick::now());
            }
        }
    }
}

fn call_back_1(param0: u32) {
    println!("call_back1 {} {:?}", param0, Tick::now().elapsed());
}
fn call_back_2(param0: u32, param1: u32) {
    println!(
        "call_back2 {} {} {:?}",
        param0,
        param1,
        Tick::now().elapsed()
    );
}
