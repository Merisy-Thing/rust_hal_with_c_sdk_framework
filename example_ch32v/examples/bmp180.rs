#![no_main]
#![no_std]

use bmp180_embedded_hal::blocking::UninitBMP180;
use bmp180_embedded_hal::Mode::UltraHighResolution;
use embedded_c_sdk_bind_hal::gpio::fast_pin_num::*;
use embedded_c_sdk_bind_hal::gpio::{
    FastPin, FastPinModeInput, FastPinModeOutput, FastPinReg, OutputFastPin,
};
use soft_i2c::{OpenDrainPin, SoftI2C, FREQ_I2C_SCL_100K};
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
//use panic_halt as _;

struct PA;
impl FastPinReg for PA {
    const PORT: PortNum = PortNum::PA;
    const IDR: usize = 0x40010808;
    const ODR: usize = 0x4001080C;
    const BSR: usize = 0x40010810;
    const BCR: usize = 0x40010814;
}

struct PB;
impl FastPinReg for PB {
    const PORT: PortNum = PortNum::PB;
    const IDR: usize = 0x40010C08;
    const ODR: usize = 0x40010C0C;
    const BSR: usize = 0x40010C10;
    const BCR: usize = 0x40010C14;
}

// const PA_REG: PortReg = PortReg {
// 	idr: 0x40010808 as *mut _,
//     odr: 0x4001080C as *mut _,
//     bsr: 0x40010810 as *mut _,
//     bcr: 0x40010814 as *mut _,
// };

//const PA11:FastPin<PA, 0x0800> = FastPin::new();
const PA8: FastPin<PA, FastPin8, ()> = FastPin::new();
//const PA10:FastPin<PA, FastPin10, ()> = FastPin::new();
const PB2: FastPin<PB, FastPin2, ()> = FastPin::new();

struct SDA<'a>(&'a mut FastPin<PB, FastPin14, OutputFastPin>);

impl<'a> OpenDrainPin for SDA<'a> {
    fn set(&mut self, level: bool) {
        if level {
            self.0.output_high();
        } else {
            self.0.output_low();
        }
    }

    fn get(&mut self) -> bool {
        self.0.is_input_high()
    }
}

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut delay = Delay::new();

    let mut led = PA8.into_output(FastPinModeOutput::OutPP);
    let mut key = PB2.into_input(FastPinModeInput::InPullUp);

    println!("bmp180 over crate softi2c start");

    let mut bmp180;
    // let sda = Pin::new(PortNum::PB, PinNum::Pin14);
    // let scl = Pin::new(PortNum::PB, PinNum::Pin13);
    let mut sda_pin = FastPin::<PB, FastPin14, OutputFastPin>::new().into_output(FastPinModeOutput::OutOD);
    let mut sda = SDA(&mut sda_pin);
    let mut scl = FastPin::<PB, FastPin13, OutputFastPin>::new().into_output(FastPinModeOutput::OutOD);
    let mut i2c_delay = Delay::new();
    let soft_i2c = SoftI2C::<_, _, _, FREQ_I2C_SCL_100K>::new(&mut scl, &mut sda, &mut i2c_delay);
    let uninit_bmp180 = UninitBMP180::builder(soft_i2c, Delay::new())
        .mode(UltraHighResolution)
        .build();
    match uninit_bmp180.initialize() {
        Ok(bmp) => {
            bmp180 = bmp;
        }
        Err(e) => {
            println!("bmp180 error: {:?}", e);
            loop {}
        }
    }

    let calibration = bmp180.calibration();

    println!("calibration: {:?}", calibration);
    loop {
        if let Err(e) = bmp180.update() {
            println!("update err: {:?}", e);
        }

        let tempreture = bmp180.temperature_celsius();
        println!("tempreture: {} *C", tempreture);

        let pressure = bmp180.pressure();
        println!("pressure: {} Pa", pressure);

        led.set_high().ok();
        delay.delay_ms(1000);
        led.set_low().ok();
        delay.delay_ms(1000);

        if key.is_low().unwrap_or(false) {
            delay.delay_ms(1000);
        }
    }
}

use core::panic::PanicInfo;
use core::sync::atomic;

#[inline(never)]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    //println!("panic: {:?}", _info);
    println!("panic");

    loop {
        atomic::compiler_fence(atomic::Ordering::SeqCst);
    }
}
