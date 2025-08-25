//! General-purpose Input/Output (GPIO)

use crate::ll_api::{ll_cmd::*, GpioInitFlag};
use core::convert::Infallible;
use embassy_hal_internal::{Peri, PeripheralType};

/// GPIO flexible pin.
///
/// This pin can either be a disconnected, input, or output pin, or both. The level register bit will remain
/// set while not in output mode, so the pin's level will be 'remembered' when it is not in output
/// mode.
pub struct Flex<'d> {
    pub(crate) pin: Peri<'d, AnyPin>,
}

impl<'d> Flex<'d> {
    /// Wrap the pin in a `Flex`.
    ///
    /// The pin remains disconnected. The initial output level is unspecified, but can be changed
    /// before the pin is put into output mode.
    ///
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>) -> Self {
        // Pin will be in disconnected state.
        Self { pin: pin.into() }
    }

    /// Put the pin into input mode.
    ///
    /// The internal weak pull-up and pull-down resistors will be enabled according to `pull`.
    #[inline(never)]
    pub fn set_as_input(&mut self, pull: Pull) {
        let port = self.pin._port();
        let pin = self.pin._pin();
        let flag = pull.to_pupdr();

        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, port, pin, flag);
    }

    /// Put the pin into push-pull output mode.
    ///
    /// The pin level will be whatever was set before (or low by default). If you want it to begin
    /// at a specific level, call `set_high`/`set_low` on the pin first.
    ///
    /// The internal weak pull-up and pull-down resistors will be disabled.
    #[inline(never)]
    pub fn set_as_output(&mut self) {
        let port = self.pin._port();
        let pin = self.pin._pin();
        let flag = GpioInitFlag::OutPP as u32;

        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, port, pin, flag);
    }

    /// Put the pin into open-drain output mode.
    #[inline(never)]
    pub fn set_as_open_drain(&mut self) {
        let port = self.pin._port();
        let pin = self.pin._pin();
        let flag = GpioInitFlag::OutOD as u32;

        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, port, pin, flag);
    }

    /// Put the pin into alternate function mode.
    pub fn set_as_alternate(&mut self, mode: AltMode) {
        let port = self.pin._port();
        let pin = self.pin._pin();
        let flag = mode.to_flag();

        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, port, pin, flag);
    }

    /// Put the pin into analog mode.
    pub fn set_as_analog(&mut self) {
        let port = self.pin._port();
        let pin = self.pin._pin();
        let flag = GpioInitFlag::Analog as u32;

        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, port, pin, flag);
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        let port = self.pin._port();
        let pin = self.pin._pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_GET_INPUT, port, pin) > 0
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        let port = self.pin._port();
        let pin = self.pin._pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_GET_INPUT, port, pin) == 0
    }

    /// Get the current pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.is_high().into()
    }

    /// Get whether the output level is set to high.
    #[inline]
    pub fn is_set_high(&self) -> bool {
        !self.is_set_low()
    }

    /// Get whether the output level is set to low.
    #[inline]
    pub fn is_set_low(&self) -> bool {
        let port = self.pin._port();
        let pin = self.pin._pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_GET_OUTPUT, port, pin) == 0
    }

    /// Get the current output level.
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.is_set_high().into()
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high();
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        match level {
            Level::Low => self.pin.set_low(),
            Level::High => self.pin.set_high(),
        }
    }

    /// Toggle the output level.
    #[inline]
    pub fn toggle(&mut self) {
        if self.is_set_low() {
            self.set_high()
        } else {
            self.set_low()
        }
    }
}

impl<'d> Drop for Flex<'d> {
    #[inline]
    fn drop(&mut self) {
        //Do nothing, hold status
    }
}

/// Pull setting for an input.
#[derive(Debug, Eq, PartialEq, Copy, Clone)]
pub enum Pull {
    /// No pull
    None,
    /// Pull up
    Up,
    /// Pull down
    Down,
}

impl Pull {
    const fn to_pupdr(self) -> u32 {
        match self {
            Pull::None => GpioInitFlag::InFloating as u32,
            Pull::Up => GpioInitFlag::InPU as u32,
            Pull::Down => GpioInitFlag::InPD as u32,
        }
    }
}

/// GPIO input driver.
pub struct Input<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> Input<'d> {
    /// Create GPIO input driver for a [Pin] with the provided [Pull] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, pull: Pull) -> Self {
        let mut pin = Flex::new(pin);
        pin.set_as_input(pull);
        Self { pin }
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        self.pin.is_high()
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        self.pin.is_low()
    }

    /// Get the current pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.pin.get_level()
    }
}

/// Digital input or output level.
#[derive(Debug, Eq, PartialEq, Copy, Clone)]
pub enum Level {
    /// Low
    Low,
    /// High
    High,
}

impl From<bool> for Level {
    fn from(val: bool) -> Self {
        match val {
            true => Self::High,
            false => Self::Low,
        }
    }
}

impl From<Level> for bool {
    fn from(level: Level) -> bool {
        match level {
            Level::Low => false,
            Level::High => true,
        }
    }
}

/// GPIO output driver.
///
/// Note that pins will **return to their floating state** when `Output` is dropped.
/// If pins should retain their state indefinitely, either keep ownership of the
/// `Output`, or pass it to [`core::mem::forget`].
pub struct Output<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> Output<'d> {
    /// Create GPIO output driver for a [Pin] with the provided [Level] and [Speed] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, initial_output: Level) -> Self {
        let mut pin = Flex::new(pin);
        match initial_output {
            Level::High => pin.set_high(),
            Level::Low => pin.set_low(),
        }
        pin.set_as_output();
        Self { pin }
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high();
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        self.pin.set_level(level)
    }

    /// Is the output pin set as high?
    #[inline]
    pub fn is_set_high(&self) -> bool {
        self.pin.is_set_high()
    }

    /// Is the output pin set as low?
    #[inline]
    pub fn is_set_low(&self) -> bool {
        self.pin.is_set_low()
    }

    /// What level output is set to
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.pin.get_output_level()
    }

    /// Toggle pin output
    #[inline]
    pub fn toggle(&mut self) {
        self.pin.toggle();
    }
}

/// GPIO output open-drain driver.
///
/// Note that pins will **return to their floating state** when `OutputOpenDrain` is dropped.
/// If pins should retain their state indefinitely, either keep ownership of the
/// `OutputOpenDrain`, or pass it to [`core::mem::forget`].
pub struct OutputOpenDrain<'d> {
    pub(crate) pin: Flex<'d>,
}

impl<'d> OutputOpenDrain<'d> {
    /// Create a new GPIO open drain output driver for a [Pin] with the provided [Level] and [Speed].
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, initial_output: Level) -> Self {
        let mut pin = Flex::new(pin);
        match initial_output {
            Level::High => pin.set_high(),
            Level::Low => pin.set_low(),
        }
        pin.set_as_open_drain();
        Self { pin }
    }

    /// Get whether the pin input level is high.
    #[inline]
    pub fn is_high(&self) -> bool {
        !self.pin.is_low()
    }

    /// Get whether the pin input level is low.
    #[inline]
    pub fn is_low(&self) -> bool {
        self.pin.is_low()
    }

    /// Get the current pin input level.
    #[inline]
    pub fn get_level(&self) -> Level {
        self.pin.get_level()
    }

    /// Set the output as high.
    #[inline]
    pub fn set_high(&mut self) {
        self.pin.set_high();
    }

    /// Set the output as low.
    #[inline]
    pub fn set_low(&mut self) {
        self.pin.set_low();
    }

    /// Set the output level.
    #[inline]
    pub fn set_level(&mut self, level: Level) {
        self.pin.set_level(level);
    }

    /// Get whether the output level is set to high.
    #[inline]
    pub fn is_set_high(&self) -> bool {
        self.pin.is_set_high()
    }

    /// Get whether the output level is set to low.
    #[inline]
    pub fn is_set_low(&self) -> bool {
        self.pin.is_set_low()
    }

    /// Get the current output level.
    #[inline]
    pub fn get_output_level(&self) -> Level {
        self.pin.get_output_level()
    }

    /// Toggle pin output
    #[inline]
    pub fn toggle(&mut self) {
        self.pin.toggle()
    }
}

pub enum AltMode {
    AF(u8),
    AFOD,
    AFPP,
}

impl AltMode {
    fn to_flag(&self) -> u32 {
        match self {
            AltMode::AF(idx) => {
                use GpioInitFlag::*;
                const AF_FLAGS: [GpioInitFlag; 8] = [AF0, AF1, AF2, AF3, AF4, AF5, AF6, AF7];
                AF_FLAGS.get(*idx as usize).copied().unwrap_or(AF0) as u32
            }
            AltMode::AFOD => GpioInitFlag::AFOD as u32,
            AltMode::AFPP => GpioInitFlag::AFPP as u32,
        }
    }
}

/// GPIO alternate driver.
pub struct Alternate<'d> {
    pub(crate) _pin: Flex<'d>,
}

impl<'d> Alternate<'d> {
    /// Create GPIO input driver for a [Pin] with the provided [Pull] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>, mode: AltMode) -> Self {
        let mut pin = Flex::new(pin);
        pin.set_as_alternate(mode);
        Self { _pin: pin }
    }
}

/// GPIO analog driver.
pub struct Analog<'d> {
    pub(crate) _pin: Flex<'d>,
}

impl<'d> Analog<'d> {
    /// Create GPIO input driver for a [Pin] with the provided [Pull] configuration.
    #[inline]
    pub fn new(pin: Peri<'d, impl Pin>) -> Self {
        let mut pin = Flex::new(pin);
        pin.set_as_analog();
        Self { _pin: pin }
    }
}

pub(crate) trait SealedPin {
    fn pin_port(&self) -> u8;

    #[inline]
    fn _pin(&self) -> u8 {
        self.pin_port() % 16
    }

    #[inline]
    fn _port(&self) -> u8 {
        self.pin_port() / 16
    }

    // #[inline]
    // fn block(&self) -> gpio::Gpio {
    //     crate::chip::gpio_block(self._port() as _)
    // }

    /// Set the output as high.
    #[inline]
    fn set_high(&self) {
        let port = self._port();
        let pin = self._pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_SET, port, pin, 1);
    }

    /// Set the output as low.
    #[inline]
    fn set_low(&self) {
        let port = self._port();
        let pin = self._pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_SET, port, pin, 0);
    }
}

/// GPIO pin trait.
#[allow(private_bounds)]
pub trait Pin: PeripheralType + Into<AnyPin> + SealedPin + Sized + 'static {
    /// Number of the pin within the port (0..31)
    #[inline]
    fn pin(&self) -> u8 {
        self._pin()
    }

    /// Port of the pin
    #[inline]
    fn port(&self) -> u8 {
        self._port()
    }
}

/// Type-erased GPIO pin
pub struct AnyPin {
    pin_port: u8,
}

impl AnyPin {
    /// Unsafely create an `AnyPin` from a pin+port number.
    ///
    /// `pin_port` is `port_num * 16 + pin_num`, where `port_num` is 0 for port `A`, 1 for port `B`, etc...
    #[inline]
    pub unsafe fn steal(pin_port: u8) -> Peri<'static, Self> {
        Peri::new_unchecked(Self { pin_port })
    }

    #[inline]
    fn _port(&self) -> u8 {
        self.pin_port / 16
    }
}

impl Copy for AnyPin {}

impl Clone for AnyPin {
    fn clone(&self) -> Self {
        *self
    }
}
impl PeripheralType for AnyPin {}

impl Pin for AnyPin {}

impl SealedPin for AnyPin {
    #[inline]
    fn pin_port(&self) -> u8 {
        self.pin_port
    }
}

crate::foreach_pin!(
    ($pin_name:ident, $port_num:expr, $pin_num:expr) => {
        impl SealedPin for $crate::peripherals::$pin_name {
            #[inline]
            fn pin_port(&self) -> u8 {
                $port_num * 16 + $pin_num
            }
        }

        impl From<$crate::peripherals::$pin_name> for AnyPin {
            fn from(val: $crate::peripherals::$pin_name) -> Self {
                Self {
                    pin_port: val.pin_port(),
                }
            }
        }
    };
);

impl<'d> embedded_hal::digital::ErrorType for Input<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal::digital::InputPin for Input<'d> {
    #[inline]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Input::is_high(self))
    }

    #[inline]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(Input::is_low(self))
    }
}

impl<'d> embedded_hal::digital::ErrorType for Output<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal::digital::OutputPin for Output<'d> {
    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        Output::set_high(self);
        Ok(())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        Output::set_low(self);
        Ok(())
    }
}

impl<'d> embedded_hal::digital::StatefulOutputPin for Output<'d> {
    #[inline]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Output::is_set_high(self))
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok(Output::is_set_low(self))
    }
}

impl<'d> embedded_hal::digital::ErrorType for OutputOpenDrain<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal::digital::InputPin for OutputOpenDrain<'d> {
    #[inline]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(OutputOpenDrain::is_high(self))
    }

    #[inline]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(OutputOpenDrain::is_low(self))
    }
}

impl<'d> embedded_hal::digital::OutputPin for OutputOpenDrain<'d> {
    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        OutputOpenDrain::set_high(self);
        Ok(())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        OutputOpenDrain::set_low(self);
        Ok(())
    }
}

impl<'d> embedded_hal::digital::StatefulOutputPin for OutputOpenDrain<'d> {
    #[inline]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok(OutputOpenDrain::is_set_high(self))
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok(OutputOpenDrain::is_set_low(self))
    }
}

impl<'d> embedded_hal::digital::InputPin for Flex<'d> {
    #[inline]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Flex::is_high(self))
    }

    #[inline]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(Flex::is_low(self))
    }
}

impl<'d> embedded_hal::digital::OutputPin for Flex<'d> {
    #[inline]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        Flex::set_high(self);
        Ok(())
    }

    #[inline]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        Flex::set_low(self);
        Ok(())
    }
}

impl<'d> embedded_hal::digital::ErrorType for Flex<'d> {
    type Error = Infallible;
}

impl<'d> embedded_hal::digital::StatefulOutputPin for Flex<'d> {
    #[inline]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Flex::is_set_high(self))
    }

    /// Is the output pin set as low?
    #[inline]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok(Flex::is_set_low(self))
    }
}
