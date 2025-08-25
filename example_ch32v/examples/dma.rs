#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL,
    dma::{self, Dma, DmaChannel},
    println,
};

use ll_bind_ch32v20x as _;
use panic_halt as _;

#[riscv_rt_macros::entry]
fn main() -> ! {
    CSDK_HAL::init();
    let src = [0x12345678_u32; 8];
    let mut dst = [0x00_u32; 10];

    let dma_cfg = dma::Config::new(
        dma::DmaSrc::Ref(&src),
        dma::DmaDst::Ref(&mut dst),
        dma::DmaDir::M2M,
        false,
    );
    let dma3 = Dma::new(DmaChannel::CH3);
    println!("\r\nDMA M2M test");
    match dma3.init(&dma_cfg, None) {
        Ok(_) => {
            println!("DMA init ok");
        }
        Err(code) => {
            println!("DMA init err: {}", code);
        }
    }

    println!("dst before tranfer: {}", &dst);
    match dma3.start() {
        Ok(_) => {
            println!("DMA start ok");
        }
        Err(code) => {
            println!("DMA start err: {}", code);
        }
    }
    match dma3.wait() {
        Ok(_) => {
            println!("DMA wait ok");
        }
        Err(code) => {
            println!("DMA wait err: {}", code);
        }
    }
    match dma3.stop() {
        Ok(_) => {
            println!("DMA stop ok");
        }
        Err(code) => {
            println!("DMA stop err: {}", code);
        }
    }

    println!("dst after tranfer: {}", &dst);

    loop {}
}
