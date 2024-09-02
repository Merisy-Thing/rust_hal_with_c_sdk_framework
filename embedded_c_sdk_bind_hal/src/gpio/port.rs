use super::ToGpioInitFlag;
use crate::ll_api::{ll_cmd::*, GpioInitFlag, PortNum};
use core::marker::PhantomData;

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum PortModeInput {
    InFloating,
    InPullUp,
    InPullDown,
}

impl ToGpioInitFlag for PortModeInput {
    fn to_flag(&self) -> u32 {
        match self {
            PortModeInput::InFloating => GpioInitFlag::InFloating as u32,
            PortModeInput::InPullUp => GpioInitFlag::InPU as u32,
            PortModeInput::InPullDown => GpioInitFlag::InPD as u32,
        }
    }
}

pub enum PortModeOutput {
    OutOD,
    OutPP,
}

impl ToGpioInitFlag for PortModeOutput {
    fn to_flag(&self) -> u32 {
        match self {
            PortModeOutput::OutOD => GpioInitFlag::OutOD as u32,
            PortModeOutput::OutPP => GpioInitFlag::OutPP as u32,
        }
    }
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(C)]
pub struct PortReg {
    /// Port input data register
    pub idr: *mut u16,
    /// Port output data register
    pub odr: *mut u16,
    /// Port bit set register
    pub bsr: *mut u16,
    /// Port bit reset register
    pub bcr: *mut u16,
}

pub struct InputPort;
pub struct OutputPort;

#[derive(Clone, Copy, Debug)]
pub struct Port<MODE> {
    gpio: PortNum,
    mask: u16,
    regs: &'static PortReg,
    _mode: PhantomData<MODE>,
}

impl Port<()> {
    /// Creates a new GPIO port instance.
    ///
    /// # Arguments
    /// * `gpio` - The port number.
    /// * `mask` - A bit mask specifying which pins are part of this port.
    ///
    /// # Returns
    /// A new `Port` instance with uninitialized mode.
    pub fn new(gpio: PortNum, mask: u16, regs: &'static PortReg) -> Self {
        let port = Port {
            gpio,
            mask,
            regs,
            _mode: PhantomData,
        };

        port
    }

    /// Converts the port to an input mode.
    ///
    /// # Arguments
    /// * `mode` - The input mode configuration.
    ///
    /// # Returns
    /// A new `Port` instance configured as an input port.
    pub fn into_input(&self, mode: PortModeInput) -> Port<InputPort> {
        let pin_mode = mode.to_flag();
        self.port_init(pin_mode);

        Port {
            gpio: self.gpio,
            mask: self.mask,
            regs: self.regs,
            _mode: PhantomData,
        }
    }

    /// Converts the port to an output mode.
    ///
    /// # Arguments
    /// * `mode` - The output mode configuration.
    ///
    /// # Returns
    /// A new `Port` instance configured as an output port.
    pub fn into_output(&self, mode: PortModeOutput) -> Port<OutputPort> {
        let pin_mode = mode.to_flag();
        self.port_init(pin_mode);

        Port {
            gpio: self.gpio,
            mask: self.mask,
            regs: self.regs,
            _mode: PhantomData,
        }
    }

    /// Initializes individual pins on the port according to the given mode.
    ///
    /// # Arguments
    /// * `mode` - The initialization flag for the pins.
    fn port_init(&self, mode: u32) {
        for pin in 0..16 {
            if (self.mask & (1 << pin)) > 0 {
                ll_invoke_inner!(INVOKE_ID_GPIO_INIT, self.gpio, pin, mode);
            }
        }
    }
}

impl Port<OutputPort> {
    /// Sets the specified bits of the output register without checking bounds or validity.
    ///
    /// # Safety
    /// This function must only be called when it is safe to modify the output register directly.
    #[inline]
    pub unsafe fn set_bits_uncheck(&mut self, bits: u16) {
        core::ptr::write_volatile(self.regs.bsr, bits)
    }

    /// Clears the specified bits of the output register without checking bounds or validity.
    ///
    /// # Safety
    /// This function must only be called when it is safe to modify the output register directly.
    #[inline]
    pub unsafe fn clr_bits_uncheck(&mut self, bits: u16) {
        core::ptr::write_volatile(self.regs.bcr, bits);
    }

    /// Reads the current value of the output register without checking bounds or validity.
    ///
    /// # Safety
    /// This function must only be called when it is safe to read from the output register directly.
    #[inline]
    pub unsafe fn read_output_bits_uncheck(&mut self) -> u16 {
        core::ptr::read_volatile(self.regs.odr)
    }

    /// Writes a value to the output register without checking bounds or validity.
    ///
    /// # Safety
    /// This function must only be called when it is safe to modify the output register directly.
    #[inline]
    pub unsafe fn write_bits_uncheck(&mut self, bits: u16) {
        core::ptr::write_volatile(self.regs.odr, bits);
    }

    /// Sets the specified bits of the output register, applying the bit mask.
    pub fn set_bits(&mut self, bits: u16) {
        if !self.regs.bsr.is_null() {
            unsafe { self.set_bits_uncheck(bits & self.mask) }
        } else {
            let read = self.read_output_bits();
            self.write_bits(bits | read);
        }
    }

    /// Clears the specified bits of the output register, applying the bit mask.
    pub fn clr_bits(&mut self, bits: u16) {
        if !self.regs.bcr.is_null() {
            unsafe { self.clr_bits_uncheck(bits & self.mask) }
        } else {
            let read = self.read_output_bits();
            self.write_bits(!bits & read);
        }
    }

    /// Reads the current value of the output register, applying the bit mask.
    pub fn read_output_bits(&mut self) -> u16 {
        if !self.regs.odr.is_null() {
            return unsafe { self.read_output_bits_uncheck() } & self.mask;
        }
        return 0;
    }

    /// Writes a value to the output register, applying the bit mask.
    pub fn write_bits(&mut self, bits: u16) {
        if !self.regs.odr.is_null() {
            unsafe {
                let read = self.read_output_bits_uncheck() & !self.mask;
                self.write_bits_uncheck((bits & self.mask) | read);
            }
        }
    }
    
    /// Reads the current value of the input register, applying the bit mask.
    pub fn read_input_bits(&mut self) -> u16 {
        if !self.regs.idr.is_null() {
            return unsafe { core::ptr::read_volatile(self.regs.idr) } & self.mask;
        }
        return 0;
    }
}

impl Port<InputPort> {
    /// Reads the current value of the input register without checking bounds or validity.
    ///
    /// # Safety
    /// This function must only be called when it is safe to read from the input register directly.
    #[inline]
    pub unsafe fn read_input_bits_uncheck(&mut self) -> u16 {
        core::ptr::read_volatile(self.regs.idr)
    }

    /// Reads the current value of the input register, applying the bit mask.
    pub fn read_input_bits(&mut self) -> u16 {
        if !self.regs.idr.is_null() {
            return unsafe { self.read_input_bits_uncheck() } & self.mask;
        }
        return 0;
    }
}

impl<MODE> embedded_hal::digital::ErrorType for Port<MODE> {
    type Error = core::convert::Infallible;
}

impl embedded_hal::digital::OutputPin for Port<OutputPort> {
    #[inline(always)]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.set_bits(self.mask);
        Ok(())
    }

    #[inline(always)]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.clr_bits(self.mask);
        Ok(())
    }
}

impl embedded_hal::digital::StatefulOutputPin for Port<OutputPort> {
    #[inline(always)]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok(self.read_output_bits() > 0)
    }

    #[inline(always)]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok(self.read_output_bits() == 0)
    }
}

impl embedded_hal::digital::InputPin for Port<InputPort> {
    #[inline(always)]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(self.read_input_bits() > 0)
    }

    #[inline(always)]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(self.read_input_bits() == 0)
    }
}

impl embedded_hal::digital::InputPin for Port<OutputPort> {
    #[inline(always)]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(self.read_input_bits() > 0)
    }

    #[inline(always)]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(self.read_input_bits() == 0)
    }
}
