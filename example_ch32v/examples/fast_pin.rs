#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::gpio::{FastPin, FastPinModeInput, FastPinModeOutput, FastPinReg, PortReg};
use embedded_c_sdk_bind_hal::gpio::fast_pin_num::*;
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

struct PA;
impl FastPinReg for PA {
    const PORT:PortNum = PortNum::PA;
    const IDR:usize = 0x40010808;
    const ODR:usize = 0x4001080C;
    const BSR:usize = 0x40010810;
    const BCR:usize = 0x40010814;
}

struct PB;
impl FastPinReg for PB {
    const PORT:PortNum = PortNum::PB;
    const IDR:usize = 0x40010C08;
    const ODR:usize = 0x40010C0C;
    const BSR:usize = 0x40010C10;
    const BCR:usize = 0x40010C14;
}

const PA_REG: PortReg = PortReg {
	idr: 0x40010808 as *mut _,
    odr: 0x4001080C as *mut _,
    bsr: 0x40010810 as *mut _,
    bcr: 0x40010814 as *mut _,
};

//const PA11:FastPin<PA, 0x0800> = FastPin::new();
const PA8:FastPin<PA, FastPin8, ()> = FastPin::new();
const PA10:FastPin<PA, FastPin10, ()> = FastPin::new();
const PB2:FastPin<PB, FastPin2, ()> = FastPin::new();

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut delay = Delay::new();

    let mut gnd = Pin::new(PortNum::PA, PinNum::Pin11).into_output(PinModeOutput::OutPP);
    gnd.set_low().ok();


    let mut led = PA8.into_output(FastPinModeOutput::OutPP);
    let mut key = PB2.into_input(FastPinModeInput::InPullUp);

    let mut api_pin = Pin::new(PortNum::PA, PinNum::Pin10).into_output(PinModeOutput::OutPP);
	let mut port_pin = Port::new(PortNum::PA, 1 << 10, &PA_REG).into_output(PortModeOutput::OutPP);
    let mut fast_pin = PA10.into_output(FastPinModeOutput::OutPP);
    
    loop {
		led.set_high().ok();
        delay.delay_ms(250);
		led.set_low().ok();
        delay.delay_ms(250);


        api_pin.set_high().ok();//1.02us H
		api_pin.set_low().ok(); //1.02us L
		api_pin.set_high().ok();//1.02us H
		api_pin.set_low().ok();

        delay.delay_ms(6);

        port_pin.set_high().ok();//200ns H
		port_pin.set_low().ok(); //200ns L
		port_pin.set_high().ok();//200ns H
		port_pin.set_low().ok();

        delay.delay_ms(4);

        fast_pin.set_high().ok();//30ns H
		fast_pin.set_low().ok(); //30ns L
		fast_pin.set_high().ok();//30ns H
		fast_pin.set_low().ok();

        delay.delay_ms(2);

        fast_pin.output_high();//30ns H
		fast_pin.output_low(); //30ns L
		fast_pin.output_high();//30ns H
		fast_pin.output_low();

        if key.is_low().unwrap_or(false) {
            delay.delay_ms(1000);
            println!("curr_tick = %d", Tick::tick() as u32);
        }
    }
}
