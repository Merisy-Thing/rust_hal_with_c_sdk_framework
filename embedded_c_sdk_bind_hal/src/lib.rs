#![no_std]

#[macro_use]
mod common;
mod ll_api;
mod tick_freq_hz;

pub mod adc;
pub mod gpio;
pub mod print;
pub mod pwm;
pub mod spi;
pub mod tick;
pub mod usart;

pub use ll_api::ll_cmd::*;

#[macro_export]
macro_rules! ll_invoke {
    ( $( $x:expr ),* ) => {
        {
            unsafe { $crate::ll_invoke( $( $x as $crate::InvokeParam, )* ) }
        }
    };
}
