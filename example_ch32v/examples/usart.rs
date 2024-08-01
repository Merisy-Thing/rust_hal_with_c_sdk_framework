#![no_main]
#![no_std]

#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    self, ll_invoke, print, println,
    adc::{self, Adc, AdcBuffered, AdcChannel}, 
    gpio::{ Pin, PinNum, PinModeOutput, PinModeInput, PinModeAlternate, PortNum, ExtiMode, Port, PortModeOutput, }, 
    pwm::{Pwm, PwmChannel, PwmPolarity}, 
    tick::{Tick, Delay},
    usart::{self, Usart}
};
use embedded_c_sdk_bind_hal as csdk_hal;
use embedded_hal::{self, delay::DelayNs};
//use embedded_hal_nb::serial::Read;
use embedded_io::{self, Write};
use embedded_io_async::{self, Read};
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(spawner: Spawner) -> ! {
    let mut delay = Delay::new();
    let _tick: Tick = Tick::now();

    ll_invoke!(csdk_hal::INVOKE_ID_LOG_PUTS, b"123".as_ptr(), 3);

    let mut usart2 = usart::Usart::new(&usart::USART2);
    let mut usart2_buf: [u8; 64] = [0; 64];
    usart2.set_rx_buf(usart2_buf.as_mut_slice());
    let config = usart::Config::default();
    usart2.init(&config);

    let _usart2_tx = Pin::new(PortNum::PA, PinNum::Pin2).into_alternate(PinModeAlternate::AFPP);
    let _usart2_rx = Pin::new(PortNum::PA, PinNum::Pin3).into_input(PinModeInput::InPullUp);
    let _portc = Port::new(PortNum::PC, 0xFFFF).into_output(PortModeOutput::OutPP);

    let _ = spawner.spawn(task_1(usart2.clone()));

    println!("hello embassy!");

    loop {
        Timer::after_ticks(100 as u64).await;
        delay.delay_ms(900);
        println!("usart2 wr!");
        usart2.write(b"123456789abc\r\n").ok();
        
    }
}

#[embassy_executor::task]
async fn task_1(mut usart: Usart<'static>) {
    let mut count = 0;
    let mut buffer = [0_u8; 4];
    loop {
        println!("count = {}", count);
        count += 1;
        buffer.fill(0);
        match usart.read(&mut buffer).await {
            Ok(_) => {
                println!("read: {:?}", buffer);
            },
            Err(e) => {
                println!("err: {:?}", e);
            },
        }
        
        // while let Ok(byte) = usart.read() {
        //     print!("_{}", byte as char);
        // }

        //Timer::after_ticks(1000 as u64).await;
    }
}

#[no_mangle]
pub fn exti_irq_callback(line: u8) {
    print!("exti_irq_callback {}", line);
}
