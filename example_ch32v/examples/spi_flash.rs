#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::spi::SpiBus;
#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    self, ll_invoke, print, println,
    adc::{self, Adc, AdcBuffered, AdcChannel}, 
    gpio::{ Pin, PinNum, PinModeOutput, PinModeInput, PinModeAlternate, PortNum, ExtiMode, Port, PortModeOutput}, 
    pwm::{Pwm, PwmChannel, PwmPolarity}, 
    tick::{Tick, Delay},
    usart::{self, Usart},
	spi::{SpiBusId, Config},
};
use embedded_hal::digital::{InputPin, OutputPin};
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;
use w25q32jv::*;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(_spawner: Spawner) -> ! {
    println!("hello embassy!");

    let mut key = Pin::new(PortNum::PB, PinNum::Pin2).into_input(PinModeInput::InPullUp);

    let mut _sck = Pin::new(PortNum::PA, PinNum::Pin5).into_alternate(PinModeAlternate::AFPP);
    let mut _mosi = Pin::new(PortNum::PA, PinNum::Pin7).into_alternate(PinModeAlternate::AFPP);
    let mut _miso = Pin::new(PortNum::PA, PinNum::Pin6).into_input(PinModeInput::InPullUp);
    let mut nss = Pin::new(PortNum::PA, PinNum::Pin2).into_output(PinModeOutput::OutPP);
    nss.set_high().ok();

    let hold = Pin::new(PortNum::PA, PinNum::Pin0).into_output(PinModeOutput::OutPP);
    let wp = Pin::new(PortNum::PA, PinNum::Pin1).into_output(PinModeOutput::OutPP);

    let spi_cfg = Config::default();
    let spi_dev = SpiBus::new(SpiBusId::Bus1, &spi_cfg).to_device(nss);
    let mut w25 = W25q32jv::new(spi_dev, hold, wp).unwrap();
    match w25.device_id() {
        Ok(id) => {
            println!("device_id {:?}", id);
        }
        Err(e) => {
            println!("get device_id err {:?}", e);
        }
    }
    let mut read_buf = [0_u8; 32];
    let mut addr = 0;
    loop {
        if let Ok(_) = w25.read(addr, &mut read_buf) {
            print!("{:08X}: ", addr);
            for b in read_buf {
                print!("{:02X} ", b);
            }
            println!("");
        }
        addr += read_buf.len() as u32;

        if key.is_low().unwrap() {
            Timer::after_ticks(5 as u64).await;
        } else {
            Timer::after_ticks(1000 as u64).await;
        }
    }
}

#[no_mangle]
pub fn exti_irq_callback(line: u8) {
    print!("exti_irq_callback {}", line);
}
