#![no_std]

mod soft_i2c;

pub use soft_i2c::*;

pub const FREQ_I2C_SCL_100K: u32 = 100_000;
pub const FREQ_I2C_SCL_400K: u32 = 400_000;

/// Trait for controlling bidirectional GPIO pins.
pub trait OpenDrainPin {
    /// Sets the level of the GPIO pin.
    fn set(&mut self, level: bool);

    /// Gets the current level of the GPIO pin.
    fn get(&mut self) -> bool;
}
