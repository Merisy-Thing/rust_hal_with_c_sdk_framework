#![no_main]
#![no_std]
use embedded_c_sdk_bind_hal::{self as CSDK_HAL, println};
use ll_bind_ch32v20x as _;
use panic_halt as _;

use embassy_executor::Spawner;
use embassy_time::Timer;

#[embassy_executor::main(entry = "riscv_rt_macros::entry")]
async fn main(spawner: Spawner) -> ! {
    CSDK_HAL::init();
    let _ = spawner.spawn(task_1());
    let _ = spawner.spawn(task_2());
    let _ = spawner.spawn(task_3());
    let _ = spawner.spawn(task_4());
    let _ = spawner.spawn(task_5());

    println!("hello embassy!");
    //let mut count = 0;
    loop {
        Timer::after_ticks(100).await;
        //println!("main task {}", count);
        //count += 1;
    }
}

#[embassy_executor::task]
async fn task_1() {
    let mut count = 0;
    loop {
        Timer::after_ticks(1000).await;
        count += 1;
        println!("task_1 {}", count);
    }
}

#[embassy_executor::task]
async fn task_2() {
    let mut count = 0;
    loop {
        Timer::after_ticks(2000).await;
        count += 2;
        println!("task_2 {}", count);
    }
}

#[embassy_executor::task]
async fn task_3() {
    let mut count = 0;
    loop {
        Timer::after_ticks(3000).await;
        count += 3;
        println!("task_3 {}", count);
    }
}

#[embassy_executor::task]
async fn task_4() {
    let mut count = 0;
    loop {
        Timer::after_ticks(4000).await;
        count += 4;
        println!("task_4 {}", count);
    }
}

#[embassy_executor::task]
async fn task_5() {
    let mut count = 0;
    loop {
        Timer::after_ticks(5000).await;
        count += 5;
        println!("task_5 {}", count);
    }
}
