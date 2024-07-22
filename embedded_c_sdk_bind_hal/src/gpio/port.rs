use core::marker::PhantomData;

use crate::ll_api::{ll_cmd::*, GpioInitFlag, PortNum};

trait PortModeFlag {
    fn to_flag(&self) -> GpioInitFlag;
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum PortModeInput {
    InFloating,
    InPullUp,
    InPullDown,
}

impl PortModeFlag for PortModeInput {
    fn to_flag(&self) -> GpioInitFlag {
        match self {
            PortModeInput::InFloating => GpioInitFlag::InFloating,
            PortModeInput::InPullUp => GpioInitFlag::InPU,
            PortModeInput::InPullDown => GpioInitFlag::InPD,
        }
    }
}

pub enum PortModeOutput {
    OutOD,
    OutPP,
}

impl PortModeFlag for PortModeOutput {
    fn to_flag(&self) -> GpioInitFlag {
        match self {
            PortModeOutput::OutOD => GpioInitFlag::OutOD,
            PortModeOutput::OutPP => GpioInitFlag::OutPP,
        }
    }
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(C)]
pub struct PortReg {
    /// Port input data register
    idr: *mut u16,
    /// Port output data register
    odr: *mut u16,
    /// Port bit set register
    bsr: *mut u16,
    /// Port bit reset register
    bcr: *mut u16,
}

pub struct InputPort;
pub struct OutputPort;

pub struct Port<MODE> {
    gpio: PortNum,
    mask: u16,
    regs: PortReg,
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
    pub fn new(gpio: PortNum, mask: u16) -> Port<()> {
        let regs: PortReg = PortReg {
            idr: core::ptr::null_mut::<u16>(),
            odr: core::ptr::null_mut::<u16>(),
            bsr: core::ptr::null_mut::<u16>(),
            bcr: core::ptr::null_mut::<u16>(),
        };
        let port = Port {
            gpio,
            mask,
            regs,
            _mode: PhantomData,
        };

        // Initialize the port registers.
        ll_invoke_inner!(
            INVOKE_ID_GPIO_GET_PORT_REG,
            &(port.regs) as *const _ as usize
        );

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
            regs: self.regs.clone(),
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
            regs: self.regs.clone(),
            _mode: PhantomData,
        }
    }

    /// Initializes individual pins on the port according to the given mode.
    ///
    /// # Arguments
    /// * `mode` - The initialization flag for the pins.
    fn port_init(&self, mode: GpioInitFlag) {
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
