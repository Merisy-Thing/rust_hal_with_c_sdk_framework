mod exti;
mod fast_pin;
mod pin;
mod port;

pub use exti::*;
pub use fast_pin::*;
pub use pin::*;
pub use port::*;

pub use crate::ll_api::PinNum;
pub use crate::ll_api::PortNum;

/// Trait for converting types into GPIO initialization flags.
pub trait ToGpioInitFlag {
    fn to_flag(&self) -> u32;
}
