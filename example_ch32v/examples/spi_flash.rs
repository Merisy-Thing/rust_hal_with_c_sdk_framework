#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL,
    gpio::{AltMode, Alternate, AnyPin, Input, Level, Output, Pull},
    print, println,
    spi::{Config, SpiBus, SpiBusId},
};
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;
use w25q32jv::*;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(_spawner: Spawner) -> ! {
    let p = CSDK_HAL::init();
    println!("hello embassy!");

    let key = Input::new(p.PB2.into::<AnyPin>(), Pull::Up);
    let mut _sck = Alternate::new(p.PA5.into::<AnyPin>(), AltMode::AFPP);
    let mut _mosi = Alternate::new(p.PA7.into::<AnyPin>(), AltMode::AFPP);
    let mut _miso = Input::new(p.PA6.into::<AnyPin>(), Pull::Up);
    let mut nss = Output::new(p.PA2.into::<AnyPin>(), Level::High);
    nss.set_high();

    let hold = Output::new(p.PA0.into::<AnyPin>(), Level::High);
    let wp = Output::new(p.PA1.into::<AnyPin>(), Level::High);

    let spi_cfg = Config::default();
    let spi_dev = SpiBus::new(SpiBusId::Bus1, &spi_cfg).to_device(nss);
    let mut w25 = W25q32jv::new(spi_dev, hold, wp).unwrap();
    match w25.device_id() {
        Ok(id) => {
            println!("device_id {:?}", &id);
        }
        Err(_e) => {
            println!("get device_id err");
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

        if key.is_low() {
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
