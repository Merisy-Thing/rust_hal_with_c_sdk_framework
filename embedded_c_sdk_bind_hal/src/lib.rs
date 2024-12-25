#![no_std]

#[macro_use]
mod common;
mod ll_api;
mod tick_freq_hz;

pub mod adc;
pub mod dma;
pub mod gpio;
pub mod i2c;
pub mod print;
pub mod pwm;
pub mod spi;
pub mod tick;
pub mod usart;

pub use common::format;
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
macro_rules! sys_tick_handler {
    () => {
        <$crate::tick::Tick as $crate::tick::HalTickHandler>::on_sys_tick_interrupt();
    };
}

/// Macro for setting up a periodic interval to execute a function.
///
/// # Syntax
/// ```rust
/// setInterval!(function, period_ms);
/// setInterval!(function, period_ms, param1, param2, ...);
/// ```
///
/// # Examples
/// ```rust
/// setInterval!(my_function, 1000);
/// setInterval!(my_function_with_params, 1000, arg1, arg2);
/// ```
///
/// # Arguments
/// * `$f` - The function to be executed.
/// * `$period_ms` - The period in milliseconds after which the function should be called.
/// * `$( $param:expr ),*` - Optional parameters to pass to the function.
///
/// # Behavior
/// * Initializes a static `Tick` variable.
/// * Checks if the elapsed time since the last execution is greater than or equal to the specified period.
/// * If so, resets the `Tick` and calls the function with or without parameters.
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
