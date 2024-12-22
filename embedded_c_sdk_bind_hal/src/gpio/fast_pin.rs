use super::{PinNum, ToGpioInitFlag};
use crate::ll_api::{ll_cmd::*, GpioInitFlag, PortNum};
use core::marker::PhantomData;

pub trait FastPinReg {
    /// Port number, define by ll_api
    const PORT: PortNum;
    /// Port input data register address
    const IDR: usize;
    /// Port output data register address
    const ODR: usize;
    /// Port bit set register address
    const BSR: usize;
    /// Port bit clear register address
    const BCR: usize;
}

pub trait FastPinNum {
    const PIN: PinNum;
}

/// Automatically implement the `FastPinNum` trait for pin numbers 0 to 15.
pub mod fast_pin_num {
    use super::FastPinNum;
    use crate::gpio::PinNum;
    macro_rules! impl_fast_pin_num {
        ($( $pin_num:expr ),*) => {
            $(
                paste::paste! {
                    pub struct [<FastPin $pin_num>];
                    impl FastPinNum for [<FastPin $pin_num>] {
                        const PIN: PinNum = PinNum::[<Pin $pin_num>];
                    }
                }
            )*
        };
    }

    impl_fast_pin_num![0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum FastPinModeInput {
    /// Floating input mode.
    InFloating,
    /// Pull-up input mode.
    InPullUp,
    /// Pull-down input mode.
    InPullDown,
}

impl ToGpioInitFlag for FastPinModeInput {
    fn to_flag(&self) -> u32 {
        match self {
            FastPinModeInput::InFloating => GpioInitFlag::InFloating as u32,
            FastPinModeInput::InPullUp => GpioInitFlag::InPU as u32,
            FastPinModeInput::InPullDown => GpioInitFlag::InPD as u32,
        }
    }
}

pub enum FastPinModeOutput {
    /// Open-drain output mode.
    OutOD,
    /// Push-pull output mode.
    OutPP,
}

impl ToGpioInitFlag for FastPinModeOutput {
    fn to_flag(&self) -> u32 {
        match self {
            FastPinModeOutput::OutOD => GpioInitFlag::OutOD as u32,
            FastPinModeOutput::OutPP => GpioInitFlag::OutPP as u32,
        }
    }
}

pub struct InputFastPin;
pub struct OutputFastPin;

#[derive(Clone, Copy, Debug)]
pub struct FastPin<R, N, MODE> {
    phantom: PhantomData<(R, N, MODE)>,
}

impl<R: FastPinReg, N: FastPinNum, MODE> FastPin<R, N, MODE> {
    /// Create a new instance of `FastPin`.
    pub const fn new() -> Self {
        FastPin {
            phantom: PhantomData,
        }
    }

    /// Configure the pin as an input pin and return a new `FastPin` instance.
    pub fn into_input(&self, mode: FastPinModeInput) -> FastPin<R, N, InputFastPin> {
        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, R::PORT, N::PIN, mode.to_flag());

        FastPin {
            phantom: PhantomData,
        }
    }

    /// Configure the pin as an output pin and return a new `FastPin` instance.
    pub fn into_output(&self, mode: FastPinModeOutput) -> FastPin<R, N, OutputFastPin> {
        ll_invoke_inner!(INVOKE_ID_GPIO_INIT, R::PORT, N::PIN, mode.to_flag());

        FastPin {
            phantom: PhantomData,
        }
    }
}

impl<R: FastPinReg, N: FastPinNum> FastPin<R, N, InputFastPin> {
    /// Check if the input pin is high.
    #[inline]
    pub fn is_input_high(&self) -> bool {
        (unsafe { core::ptr::read_volatile(R::IDR as *const u16) } & (1 << N::PIN as u8)) != 0
    }

    /// Check if the input pin is low.
    #[inline]
    pub fn is_input_low(&self) -> bool {
        (unsafe { core::ptr::read_volatile(R::IDR as *const u16) } & (1 << N::PIN as u8)) == 0
    }
}

impl<R: FastPinReg, N: FastPinNum> FastPin<R, N, OutputFastPin> {
    /// Set the output pin high.
    #[inline]
    pub fn output_high(&self) {
        unsafe { core::ptr::write_volatile(R::BSR as *mut u16, 1 << N::PIN as u8) };
    }

    /// Set the output pin low.
    #[inline]
    pub fn output_low(&self) {
        unsafe { core::ptr::write_volatile(R::BCR as *mut u16, 1 << N::PIN as u8) };
    }

    /// Check if the output pin is high.
    #[inline]
    pub fn is_output_high(&self) -> bool {
        (unsafe { core::ptr::read_volatile(R::ODR as *const u16) } & (1 << N::PIN as u8)) != 0
    }

    /// Check if the output pin is low.
    #[inline]
    pub fn is_output_low(&self) -> bool {
        (unsafe { core::ptr::read_volatile(R::ODR as *const u16) } & (1 << N::PIN as u8)) == 0
    }

    /// Check if the open drain pin is high.
    #[inline]
    pub fn is_input_high(&self) -> bool {
        (unsafe { core::ptr::read_volatile(R::IDR as *const u16) } & (1 << N::PIN as u8)) != 0
    }

    /// Check if the open drain pin is low.
    #[inline]
    pub fn is_input_low(&self) -> bool {
        (unsafe { core::ptr::read_volatile(R::IDR as *const u16) } & (1 << N::PIN as u8)) == 0
    }
}

impl<R: FastPinReg, N: FastPinNum, MODE> embedded_hal::digital::ErrorType for FastPin<R, N, MODE> {
    type Error = core::convert::Infallible;
}

impl<R: FastPinReg, N: FastPinNum> embedded_hal::digital::OutputPin
    for FastPin<R, N, OutputFastPin>
{
    #[inline(always)]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        self.output_high();
        Ok(())
    }

    #[inline(always)]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        self.output_low();
        Ok(())
    }

    fn set_state(&mut self, state: embedded_hal::digital::PinState) -> Result<(), Self::Error> {
        match state {
            embedded_hal::digital::PinState::Low => self.set_low(),
            embedded_hal::digital::PinState::High => self.set_high(),
        }
    }
}

impl<R: FastPinReg, N: FastPinNum> embedded_hal::digital::StatefulOutputPin
    for FastPin<R, N, OutputFastPin>
{
    #[inline(always)]
    fn is_set_high(&mut self) -> Result<bool, Self::Error> {
        Ok(self.is_output_high())
    }

    #[inline(always)]
    fn is_set_low(&mut self) -> Result<bool, Self::Error> {
        Ok(self.is_output_low())
    }
}

impl<R: FastPinReg, N: FastPinNum> embedded_hal::digital::InputPin for FastPin<R, N, InputFastPin> {
    #[inline(always)]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(self.is_input_high())
    }

    #[inline(always)]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(self.is_input_low())
    }
}

impl<R: FastPinReg, N: FastPinNum> embedded_hal::digital::InputPin
    for FastPin<R, N, OutputFastPin>
{
    #[inline(always)]
    fn is_high(&mut self) -> Result<bool, Self::Error> {
        Ok(self.is_input_high())
    }

    #[inline(always)]
    fn is_low(&mut self) -> Result<bool, Self::Error> {
        Ok(self.is_input_low())
    }
}
