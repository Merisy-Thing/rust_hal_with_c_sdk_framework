#![no_main]
#![no_std]

#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    self, ll_invoke, print, println, setInterval,
    adc::{self, Adc, AdcBuffered, AdcChannel}, 
    gpio::{ Pin, PinNum, PinModeOutput, PinModeInput, PinModeAlternate, PortNum, ExtiMode, Port, PortModeOutput, }, 
    pwm::{Pwm, PwmChannel, PwmPolarity}, 
    tick::{Tick, Delay},
    usart::{self, Usart}
};

use embedded_hal::{self, delay::DelayNs, digital::*};
use ll_bind_ch32v20x as _;
use panic_halt as _;

fn tick_callback<const PERIOD_MS: u64, F>(f: F)
where
    F: FnOnce(),
{
    static mut TASK_TICK: Tick = Tick::with_value(0);

    if unsafe {TASK_TICK.elapsed_time().to_millis()} > PERIOD_MS {
        unsafe {TASK_TICK = Tick::now();}
        f()
    }
}

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut tick = Delay::new();

    let mut led = Pin::new(PortNum::PA, PinNum::Pin8).into_output(PinModeOutput::OutPP);
    let mut key = Pin::new(PortNum::PB, PinNum::Pin15).into_input(PinModeInput::InPullUp);

    let mut count = 0;
    let mut count_2000 = 0;
    loop {
        count += 1;

        led.toggle().ok();

        if key.is_low().unwrap() {
            tick.delay_ms(250);
        }

        tick_callback::<1000, _>(|| {
            println!("timer_task0 1000 count = {} {:?}", count, Tick::now());
        });

        setInterval!(||{
            count_2000 += 1;
            println!("setInterval 2000 count_2000 = {} {:?}", count_2000, Tick::now());
        }, 2000);
        
        setInterval!(||{
            println!("setInterval 3000 count = {} {:?}", count, Tick::now());
        }, 3000);
        
        setInterval!(call_back_1, 500, 123);
        setInterval!(call_back_2, 500, 123, 456);
    }
}

fn call_back_1(param0: u32) {
    println!("call_back1 {param0} {:?}", Tick::now());
}
fn call_back_2(param0: u32, param1: u32) {
    println!("call_back2 {param0} {param1} {:?}", Tick::now());
}
