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

#[cfg(feature = "print-log-csdk")]
pub use embedded_c_sdk_bind_print_macros::{print, println};

#[macro_export]
macro_rules! ll_invoke {
    ( $( $x:expr ),* ) => {
        {
            unsafe { $crate::ll_invoke( $( $x as $crate::InvokeParam, )* ) }
        }
    };
}


#[macro_export]
macro_rules! setInterval {
    ($f:expr, $period_ms:expr) => {
        {
            static mut TICK: Tick = Tick::with_value(0);

            if unsafe { TICK.elapsed_time().to_millis() } >= $period_ms {
                unsafe { TICK = Tick::now(); }
                $f();
            }
        }
    };

    ($f:expr, $period_ms:expr, $( $param:expr ),*) => {
        {
            static mut TICK: Tick = Tick::with_value(0);

            if unsafe { TICK.elapsed_time().to_millis() } >= $period_ms {
                unsafe { TICK = Tick::now(); }
                $f($( $param, )*);
            }
        }
    };
}
