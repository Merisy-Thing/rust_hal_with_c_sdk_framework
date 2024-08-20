use core::marker::PhantomData;

use crate::ll_api::{ll_cmd::*, GpioInitFlag, PinNum, PortNum};

trait PinModeFlag {
    fn to_flag(&self) -> GpioInitFlag;
}

pub enum PinModeAnalog {
    Analog,
}

impl PinModeFlag for PinModeAnalog {
    fn to_flag(&self) -> GpioInitFlag {
        GpioInitFlag::Analog
    }
}

pub enum PinModeInput {
    InFloating,
    InPullUp,
    InPullDown,
}

impl PinModeFlag for PinModeInput {
    fn to_flag(&self) -> GpioInitFlag {
        match self {
            PinModeInput::InFloating => GpioInitFlag::InFloating,
            PinModeInput::InPullUp => GpioInitFlag::InPU,
            PinModeInput::InPullDown => GpioInitFlag::InPD,
        }
    }
}
pub enum PinModeOutput {
    OutOD,
    OutPP,
}
impl PinModeFlag for PinModeOutput {
    fn to_flag(&self) -> GpioInitFlag {
        match self {
            PinModeOutput::OutOD => GpioInitFlag::OutOD,
            PinModeOutput::OutPP => GpioInitFlag::OutPP,
        }
    }
}

pub enum PinModeAlternate {
    AF(u8),
    AFOD,
    AFPP,
}

impl PinModeFlag for PinModeAlternate {
    fn to_flag(&self) -> GpioInitFlag {
        match self {
            PinModeAlternate::AF(idx) => {
                use GpioInitFlag::*;
                const AF_FLAGS: [GpioInitFlag; 8] = [AF0, AF1, AF2, AF3, AF4, AF5, AF6, AF7];
                AF_FLAGS.get(*idx as usize).copied().unwrap_or(AF0)
            }
            PinModeAlternate::AFOD => GpioInitFlag::AFOD,
            PinModeAlternate::AFPP => GpioInitFlag::AFPP,
        }
    }
}
pub struct Analog;
pub struct Input;
pub struct Output;
pub struct Alternate;

pub struct Pin<MODE> {
    pub(crate) port: PortNum,
    pub(crate) pin: PinNum,
    _mode: PhantomData<MODE>,
}

impl Pin<()> {
    /// Creates a new GPIO pin instance.
    ///
    /// # Arguments
    /// * `port` - The port number where the pin resides.
    /// * `pin` - The pin number on the specified port.
    ///
    /// # Returns
    /// A new `Pin` instance with uninitialized mode.
    pub const fn new(port: PortNum, pin: PinNum) -> Pin<()> {
        Pin {
            port,
            pin,
            _mode: PhantomData,
        }
    }

    /// Converts the pin to an input mode.
    ///
    /// # Arguments
    /// * `mode` - The input mode configuration.
    ///
    /// # Returns
    /// A new `Pin` instance configured as an input pin.
    pub fn into_input(&self, mode: PinModeInput) -> Pin<Input> {
        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, self.port, self.pin, mode.to_flag());
        Pin {
            port: self.port,
            pin: self.pin,
            _mode: PhantomData,
        }
    }

    /// Converts the pin to an output mode.
    ///
    /// # Arguments
    /// * `mode` - The output mode configuration.
    ///
    /// # Returns
    /// A new `Pin` instance configured as an output pin.
    pub fn into_output(&self, mode: PinModeOutput) -> Pin<Output> {
        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, self.port, self.pin, mode.to_flag());
        Pin {
            port: self.port,
            pin: self.pin,
            _mode: PhantomData,
        }
    }

    /// Converts the pin to an alternate function mode.
    ///
    /// # Arguments
    /// * `mode` - The alternate function mode configuration.
    ///
    /// # Returns
    /// A new `Pin` instance configured as an alternate function pin.
    pub fn into_alternate(&self, mode: PinModeAlternate) -> Pin<Alternate> {
        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, self.port, self.pin, mode.to_flag());
        Pin {
            port: self.port,
            pin: self.pin,
            _mode: PhantomData,
        }
    }

    /// Converts the pin to an analog mode.
    ///
    /// # Arguments
    /// * `mode` - The analog mode configuration.
    ///
    /// # Returns
    /// A new `Pin` instance configured as an analog pin.
    pub fn into_analog(&self, mode: PinModeAnalog) -> Pin<Analog> {
        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, self.port, self.pin, mode.to_flag());
        Pin {
            port: self.port,
            pin: self.pin,
            _mode: PhantomData,
        }
    }
}

impl Pin<Output> {
    /// Sets the pin to a low state.
    fn set_low(&mut self) {
        ll_invoke_inner!(INVOKE_ID_GPIO_SET, self.port, self.pin, false);
    }

    /// Sets the pin to a high state.
    fn set_high(&mut self) {
        ll_invoke_inner!(INVOKE_ID_GPIO_SET, self.port, self.pin, true);
    }

    /// Checks if the pin is currently set to a high state.
    ///
    /// # Returns
    /// `true` if the pin is set high, `false` otherwise.
    fn is_set_high(&mut self) -> bool {
        ll_invoke_inner!(INVOKE_ID_GPIO_GET_OUTPUT, self.port, self.pin) > 0
    }
}

impl Pin<Input> {
    /// Reads the current state of the pin.
    ///
    /// # Returns
    /// `true` if the pin reads high, `false` otherwise.
    pub(crate) fn is_high(&mut self) -> bool {
        ll_invoke_inner!(INVOKE_ID_GPIO_GET_INPUT, self.port, self.pin) > 0
    }
}

impl Pin<Alternate> {
    /// Reads the current state of the pin.
    ///
    /// # Returns
    /// `true` if the pin reads high, `false` otherwise.
    pub(crate) fn is_high(&mut self) -> bool {
        ll_invoke_inner!(INVOKE_ID_GPIO_GET_INPUT, self.port, self.pin) > 0
    }
}

impl<MODE> embedded_hal::digital::ErrorType for Pin<MODE> {
    type Error = core::convert::Infallible;
}

impl embedded_hal::digital::OutputPin for Pin<Output> {
    #[inline(always)]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_high();
        Ok(())
    }

    #[inline(always)]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.set_low();
        Ok(())
    }
}

impl embedded_hal::digital::StatefulOutputPin for Pin<Output> {
    #[inline(always)]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Self::is_set_high(self))
    }

    #[inline(always)]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok(!Self::is_set_high(self))
    }
}

impl embedded_hal::digital::InputPin for Pin<Input> {
    #[inline(always)]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Self::is_high(self))
    }

    #[inline(always)]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(!Self::is_high(self))
    }
}

impl embedded_hal::digital::InputPin for Pin<Alternate> {
    #[inline(always)]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(Self::is_high(self))
    }

    #[inline(always)]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(!Self::is_high(self))
    }
}
