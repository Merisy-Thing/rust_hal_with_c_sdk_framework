#![no_main]
#![no_std]

#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    self, ll_invoke, print, println, setInterval,
    adc::{self, Adc, AdcBuffered, AdcChannel}, 
    gpio::{ Pin, PinNum, PinModeOutput, PinModeInput, PinModeAlternate, PortNum, ExtiMode, Port, PortModeOutput, PortModeInput, PortReg}, 
    pwm::{Pwm, PwmChannel, PwmPolarity}, 
    tick::{Tick, Delay},
    usart::{self, Usart},
	dma::{self, Dma, DmaChannel},
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
    let src = [0x12345678_u32; 8];
    let mut dst = [0x00_u32; 10];

	let dma_cfg = dma::Config::new(dma::DmaSrc::Ref(&src), dma::DmaDst::Ref(&mut dst), dma::DmaDir::M2M, false);
    let dma3 = Dma::new(DmaChannel::CH3);
    println!("\r\nDMA M2M test");
    match dma3.init(&dma_cfg, None) {
        Ok(_) => {
            println!("DMA init ok");
        },
        Err(code) => {
            println!("DMA init err: {}", code);
        },
    }
    
    println!("dst before tranfer: {}", &dst);
    match dma3.start() {
        Ok(_) => {
            println!("DMA start ok");
        },
        Err(code) => {
            println!("DMA start err: {}", code);
        },
    }
    match dma3.wait() {
        Ok(_) => {
            println!("DMA wait ok");
        },
        Err(code) => {
            println!("DMA wait err: {}", code);
        },
    }
    match dma3.stop() {
        Ok(_) => {
            println!("DMA stop ok");
        },
        Err(code) => {
            println!("DMA stop err: {}", code);
        },
    }
    
    println!("dst after tranfer: {}", &dst);

    loop {
        if key.is_low().unwrap() {
            tick.delay_ms(500);
        }

        led.toggle().ok();
        tick.delay_ms(500);
    }
}
