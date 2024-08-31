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

/// Represents different modes for configuring a bidirectional GPIO pin.
pub enum BidirectionPinMode {
    /// Configures the pin as floating input.
    InFloating,
    /// Configures the pin as pull-up input.
    InPullUp,
    /// Configures the pin as pull-down input.
    InPullDown,
    /// Configures the pin as open-drain output.
    OutOD,
    /// Configures the pin as push-pull output.
    OutPP,
}

/// Trait for converting `BidirectionPinMode` into a GPIO initialization flag.
impl ToGpioInitFlag for BidirectionPinMode {
    fn to_flag(&self) -> u32 {
        match self {
            BidirectionPinMode::InFloating => crate::ll_api::GpioInitFlag::InFloating as u32,
            BidirectionPinMode::InPullUp => crate::ll_api::GpioInitFlag::InPU as u32,
            BidirectionPinMode::InPullDown => crate::ll_api::GpioInitFlag::InPD as u32,
            BidirectionPinMode::OutOD => crate::ll_api::GpioInitFlag::OutOD as u32,
            BidirectionPinMode::OutPP => crate::ll_api::GpioInitFlag::OutPP as u32,
        }
    }
}

/// Trait for controlling bidirectional GPIO pins.
pub trait BidirectionPin {
    /// Sets the mode of the GPIO pin.
    fn mode_ctrl(&self, mode: BidirectionPinMode);

    /// Sets the level of the GPIO pin.
    fn set(&self, level: bool);

    /// Gets the current level of the GPIO pin.
    fn get(&self) -> bool;
}

/// Trait for converting types into GPIO initialization flags.
pub trait ToGpioInitFlag {
    fn to_flag(&self) -> u32;
}