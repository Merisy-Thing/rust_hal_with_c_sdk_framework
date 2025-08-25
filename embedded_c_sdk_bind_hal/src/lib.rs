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

macro_rules! peripherals_struct {
    ($($(#[$cfg:meta])? $name:ident),*$(,)?) => {
        /// Struct containing all the peripheral.
        ///
        /// To obtain the peripherals, you must initialize the HAL, by calling [`crate::init`].
        #[allow(non_snake_case)]
        pub struct Peripherals {
            $(
                #[doc = concat!(stringify!($name), " peripheral")]
                $(#[$cfg])?
                pub $name: $crate::gpio::Peri<'static, peripherals::$name>,
            )*
        }

        impl Peripherals {
            ///Returns all the peripherals
            #[inline]
            pub(crate) fn take() -> Self {
                // safety: OK because we're inside a CS.
                unsafe {
                    Self::steal()
                }
            }
        }

        impl Peripherals {
            #[inline]
            pub unsafe fn steal() -> Self {
                Self {
                    $(
                        $(#[$cfg])?
                        $name: peripherals::$name::steal(),
                    )*
                }
            }
        }
    };
}

/// Initialize the C-SDK HAL.
///
/// This returns the peripheral singletons that can be used for creating drivers.
///
/// This should only be called once at startup, otherwise it panics.
pub fn init() -> Peripherals {
    ll_invoke!(INVOKE_ID_LL_DRV_INIT);

    Peripherals::take()
}

#[rustfmt::skip]
embassy_hal_internal::peripherals_definition!(
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15, 
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15, 
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15, 
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15, 
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15, 
    PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15, 
    PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15, 
    PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15, 
);

#[rustfmt::skip]
peripherals_struct!(
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15, 
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15, 
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15, 
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15, 
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15, 
    PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15, 
    PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15, 
    PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15, 
);