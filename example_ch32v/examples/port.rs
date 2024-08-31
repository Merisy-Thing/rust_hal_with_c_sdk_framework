#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::gpio::{PortModeInput, PortReg};
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

const PA_REG: PortReg = PortReg {
	idr: 0x40010808 as *mut _,
    odr: 0x4001080C as *mut _,
    bsr: 0x40010810 as *mut _,
    bcr: 0x40010814 as *mut _,
};

const PB_REG: PortReg = PortReg {
	idr: 0x40010C08 as *mut _,
    odr: 0x40010C0C as *mut _,
    bsr: 0x40010810_u32 as *mut _,
    bcr: 0x40010814_u32 as *mut _,
};

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut tick = Delay::new();
	let mut pa11 = Port::new(PortNum::PA, 1 << 11, &PA_REG).into_output(PortModeOutput::OutPP);
	pa11.set_low().ok();

	let mut led = Port::new(PortNum::PA, 1 << 8 | 1 << 10, &PA_REG).into_output(PortModeOutput::OutPP);
    let mut key = Port::new(PortNum::PB, 1 << 2, &PB_REG).into_input(PortModeInput::InPullUp);

    loop {
		led.set_high().ok();//215ns H
		led.set_low().ok(); //215ns L
		led.set_high().ok();
		led.set_low().ok();

        if key.is_low().unwrap() {
            tick.delay_ms(250);
        }
    }
}
