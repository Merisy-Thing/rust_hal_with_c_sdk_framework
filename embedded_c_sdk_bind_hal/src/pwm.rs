use crate::ll_api::{ll_cmd::*, PwmCtrl};
pub use crate::ll_api::{PwmChannel, PwmPolarity};
use core::convert::Infallible;
use fugit::TimerDurationU32;

pub struct Pwm<const FREQ: u32> {
    ch: PwmChannel,
}

impl<const FREQ: u32> Pwm<FREQ> {
    /// Creates a new PWM instance with the specified channel.
    ///
    /// # Arguments
    /// * `ch` - The PWM channel to initialize.
    ///
    /// # Returns
    /// A new `Pwm` instance configured for the given channel and frequency.
    pub fn new(ch: PwmChannel) -> Self {
        ll_invoke_inner!(INVOKE_ID_PWM_INIT, ch, FREQ);
        Pwm { ch }
    }

    /// Enables the PWM output on the specified channel.
    pub fn enable(&self) {
        ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, PwmCtrl::On);
    }

    /// Disables the PWM output on the specified channel.
    pub fn disable(&self) {
        ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, PwmCtrl::Off);
    }

    /// Sets the polarity of the PWM output on the specified channel.
    ///
    /// # Arguments
    /// * `p` - The polarity to set (`ActiveHigh` or `ActiveLow`).
    pub fn set_polarity(&self, p: PwmPolarity) {
        ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, p);
    }

    /// Retrieves the current duty cycle of the PWM output on the specified channel.
    ///
    /// # Returns
    /// The current duty cycle as a `u16`.
    pub fn get_duty(&self) -> u16 {
        let mut result = ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, PwmCtrl::GetDuty);
        if result < 0 {
            result = 0;
        }
        result as u16
    }

    /// Retrieves the current duty cycle of the PWM output on the specified channel and converts it to a duration.
    ///
    /// # Returns
    /// The current duty cycle as a `TimerDurationU32`.
    pub fn get_duty_time(&self) -> TimerDurationU32<FREQ> {
        TimerDurationU32::from_ticks(self.get_duty() as u32)
    }

    /// Sets the duty cycle of the PWM output on the specified channel.
    ///
    /// # Arguments
    /// * `duty` - The duty cycle to set.
    pub fn set_duty(&self, duty: u16) {
        ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, PwmCtrl::SetDuty, duty);
    }

    /// Sets the duty cycle of the PWM output on the specified channel from a duration.
    ///
    /// # Arguments
    /// * `duty` - The duty cycle duration.
    pub fn set_duty_time(&self, duty: TimerDurationU32<FREQ>) {
        self.set_duty(duty.ticks() as u16);
    }

    /// Retrieves the maximum duty cycle value of the PWM output.
    ///
    /// # Returns
    /// The maximum duty cycle as a `u16`. If `0` is returned, it means the max duty cycle is `2^16`.
    pub fn get_max_duty(&self) -> u16 {
        let mut result = ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, PwmCtrl::GetMaxDuty);
        if result < 0 {
            result = 0;
        }
        result as u16
    }

    /// Retrieves the current period of the PWM output as a duration.
    ///
    /// # Returns
    /// The current period as a `TimerDurationU32`.
    pub fn get_period(&self) -> TimerDurationU32<FREQ> {
        let mut result = ll_invoke_inner!(INVOKE_ID_PWM_CTRL, self.ch, PwmCtrl::GetPeriod);
        if result < 0 {
            result = 0;
        }
        TimerDurationU32::from_ticks(result as u32)
    }

    /// Sets the PWM frequency for the PWM output from a duration.
    ///
    /// # Arguments
    /// * `period` - The desired period as a `TimerDurationU32`.
    pub fn set_period(&self, period: TimerDurationU32<FREQ>) {
        if !period.is_zero() {
            ll_invoke_inner!(
                INVOKE_ID_PWM_CTRL,
                self.ch,
                PwmCtrl::SetPeriod,
                period.ticks()
            );
        }
    }
}

impl<const FREQ: u32> Drop for Pwm<FREQ> {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_PWM_DEINIT, self.ch);
    }
}

impl<const FREQ: u32> embedded_hal::pwm::ErrorType for Pwm<FREQ> {
    type Error = Infallible;
}

impl<const FREQ: u32> embedded_hal::pwm::SetDutyCycle for Pwm<FREQ> {
    fn max_duty_cycle(&self) -> u16 {
        self.get_max_duty()
    }
    fn set_duty_cycle(&mut self, duty: u16) -> Result<(), Self::Error> {
        self.set_duty(duty);
        Ok(())
    }
}
