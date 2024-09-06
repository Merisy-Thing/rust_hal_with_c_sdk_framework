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

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut tick = Delay::new();

    let mut led = Pin::new(PortNum::PA, PinNum::Pin8).into_output(PinModeOutput::OutPP);
    let mut key = Pin::new(PortNum::PB, PinNum::Pin2).into_input(PinModeInput::InPullUp);

    let mut count = 0;

    let test_str = "test str!!";
    let test_bool = true;
    let test_array = [11_u8; 16];
    let test_int = 123;
    let test_hex_0 = 0x0;
    let test_hex_1 = 0x55;
    let test_hex_2 = 0xa99a;
    let test_zero = 0;

    loop {
        count += 1;

        led.toggle().ok();

        if key.is_low().unwrap() {
            tick.delay_ms(500);
        } else {
            tick.delay_ms(1000);
        }

        println!("\r\nR count = {} {}", count, count);
        println!("R {} {} {} {}", 111, 112, true, false);
        println!("R {} {}", 333, test_bool);
        println!("R {} {} {}", 222, "bbbb", &test_str);
        println!("R {} {}", 4442, &test_array);
        println!("R {:?}", &test_array);
        println!("R 0x{:x} 0x{:x} 0x{:x}", test_hex_0, test_hex_1, test_hex_2);
        println!("R {} {} {} {}\n", 555, test_int, test_int, test_zero);
        print!("R {} {} ", 11, 0x10);
        print!("R {} {} ", 22, test_int);
        print!("R {} {} \n\n", 33, test_zero);

        println!("C count = %d %d", count, count);
        println!("C %d %d %d %d", 111, 112, true, false);
        println!("C %d %d", 333, test_bool);
        println!("C %d %s %s", 222, "bbbb", &test_str);
        println!("C %d %y", 4442, &test_array);
        println!("C %y", &test_array);
        println!("C 0x%x 0x%X 0x%x", test_hex_0, test_hex_1, test_hex_2);
        println!("C %d %d %d %d\n", 555, test_int, test_int, test_zero);
        print!("R %d %d ", 11, 0x10);
        print!("R %d 0x%X ", 22, test_int);
        print!("R %d %d \n\n\n", 33, test_zero);

        tick.delay_ms(5000);
    }
}
