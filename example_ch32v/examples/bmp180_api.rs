#![no_main]
#![no_std]

use bmp180_embedded_hal::blocking::UninitBMP180;
use bmp180_embedded_hal::Mode::UltraHighResolution;
use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL, self,
    gpio::PortNum,
    gpio::{fast_pin_num::*, FastPin, FastPinModeInput, FastPinModeOutput, FastPinReg},
    i2c::{I2CBusId, I2C},
    println,
    tick::{Delay, Tick},
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

#[riscv_rt_macros::entry]
fn main() -> ! {
    CSDK_HAL::init();
    let mut delay = Delay::new();

    let mut led = PA8.into_output(FastPinModeOutput::OutPP);
    let mut key = PB2.into_input(FastPinModeInput::InPullUp);

    println!("bmp180 over api i2c start");

    let mut bmp180;

    let api_i2c = I2C::<100_000>::new(I2CBusId::Bus0, None);
    let uninit_bmp180 = UninitBMP180::builder(api_i2c, Delay::new())
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
            println!("curr_tick = {:?}", Tick::tick());
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

mod ffi_i2c {
    use core::ptr::addr_of_mut;
    use core::slice;

    use embedded_c_sdk_bind_hal::{
        gpio::fast_pin_num::*,
        gpio::{FastPin, FastPinModeOutput, OutputFastPin},
        tick::Delay,
    };
    use embedded_hal::i2c::I2c;
    use local_static::LocalStatic;
    use soft_i2c::{OpenDrainPin, SoftI2C, FREQ_I2C_SCL_100K};

    const SCL_PIN: FastPin<super::PB, FastPin13, OutputFastPin> = FastPin::new();
    const SDA_PIN: FastPin<super::PB, FastPin14, OutputFastPin> = FastPin::new();
    static SOFT_I2C: LocalStatic<
        SoftI2C<FastPin<crate::PB, FastPin13, OutputFastPin>, SdaWrap, Delay, FREQ_I2C_SCL_100K>,
    > = LocalStatic::new();

    struct SdaWrap(*mut FastPin<super::PB, FastPin14, OutputFastPin>);

    impl OpenDrainPin for SdaWrap {
        fn set(&mut self, level: bool) {
            if level {
                unsafe { self.0.as_ref().unwrap().output_high() };
            } else {
                unsafe { self.0.as_ref().unwrap().output_low() };
            }
        }

        fn get(&mut self) -> bool {
            unsafe { self.0.as_ref().unwrap().is_input_high() }
        }
    }

    #[no_mangle]
    unsafe extern "C" fn ffi_i2c_init() {
        static mut DELAY: Delay = Delay::new();
        static mut SCL: FastPin<crate::PB, FastPin13, OutputFastPin> = FastPin::new();
        static mut SDA: FastPin<crate::PB, FastPin14, OutputFastPin> = FastPin::new();
        static mut SDA_OD: SdaWrap = SdaWrap(&raw mut SDA);

        SCL = SCL_PIN.into_output(FastPinModeOutput::OutOD);
        SDA = SDA_PIN.into_output(FastPinModeOutput::OutOD);

        SOFT_I2C.set(SoftI2C::<_, _, _, FREQ_I2C_SCL_100K>::new(
            &mut *addr_of_mut!(SCL),
            &mut *addr_of_mut!(SDA_OD),
            &mut *addr_of_mut!(DELAY),
        ));
    }

    #[no_mangle]
    unsafe extern "C" fn ffi_i2c_read(addr: u16, read: *mut u8, size: u32) -> core::ffi::c_int {
        let i2c = SOFT_I2C.get_mut();
        let rd_buf = slice::from_raw_parts_mut(read, size as usize);
        match i2c.read(addr as u8, rd_buf) {
            Ok(_) => return 0,
            Err(_) => return -2,
        }
    }

    #[no_mangle]
    unsafe extern "C" fn ffi_i2c_write(addr: u16, write: *const u8, size: u32) -> core::ffi::c_int {
        let i2c = SOFT_I2C.get_mut();
        let wr_buf = slice::from_raw_parts(write, size as usize);
        match i2c.write(addr as u8, wr_buf) {
            Ok(_) => return 0,
            Err(_) => return -2,
        }
    }

    #[no_mangle]
    unsafe extern "C" fn ffi_i2c_write_read(
        addr: u16,
        write: *const u8,
        wr_len: u32,
        read: *mut u8,
        rd_len: u32,
    ) -> core::ffi::c_int {
        let i2c = SOFT_I2C.get_mut();
        let rd_buf = slice::from_raw_parts_mut(read, rd_len as usize);
        let wr_buf = slice::from_raw_parts(write, wr_len as usize);
        match i2c.write_read(addr as u8, wr_buf, rd_buf) {
            Ok(_) => return 0,
            Err(_) => return -2,
        }
    }
}
