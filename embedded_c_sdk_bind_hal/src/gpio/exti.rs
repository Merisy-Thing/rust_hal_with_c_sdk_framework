use crate::ll_api::{ll_cmd::*, GpioExtiFlag};

pub enum ExtiMode {
    Rising,
    Falling,
    RisingFalling,
}

impl ExtiMode {
    pub(crate) fn to_flag(&self) -> GpioExtiFlag {
        match self {
            ExtiMode::Rising => GpioExtiFlag::Rising,
            ExtiMode::Falling => GpioExtiFlag::Falling,
            ExtiMode::RisingFalling => GpioExtiFlag::RisingFalling,
        }
    }
}

impl super::Pin<super::pin::Input> {
    /// Configures the EXTI (External Interrupt) mode for the pin.
    ///
    /// # Arguments
    /// * `mode` - The EXTI mode to set. This can be rising edge, falling edge, both edges, etc.
    pub fn exti_config(&self, mode: ExtiMode) {
        ll_invoke_inner!(INVOKE_ID_GPIO_EXTI, self.port, self.pin, mode.to_flag());
    }

    /// Enables the interrupt for the pin.
    ///
    /// This function can be reenabled after disabling it.
    pub fn enable_interrupt(&self) {
        ll_invoke_inner!(
            INVOKE_ID_GPIO_EXTI,
            self.port,
            self.pin,
            GpioExtiFlag::Enable
        );
    }

    /// Disables the interrupt for the pin.
    ///
    /// This function can be used to temporarily disable the interrupt without changing the EXTI mode.
    pub fn disable_interrupt(&self) {
        ll_invoke_inner!(
            INVOKE_ID_GPIO_EXTI,
            self.port,
            self.pin,
            GpioExtiFlag::Disable
        );
    }
}

#[allow(non_snake_case)]
#[no_mangle]
#[inline]
/// An unsafe C-compatible external function for hook EXTI IRQs.
/// # Arguments
/// * `_line` - The EXTI line number that triggered the interrupt. This corresponds to the pin number.
unsafe extern "C" fn EXTI_IRQ_hook_rs(_line: u8) {
    #[cfg(feature = "embassy")]
    // If the 'embassy' feature is enabled, update the status of the EXTI event in the embassy framework's future system.
    exti_future::exti_set_status(_line);

    #[cfg(feature = "exti-irq-callback")]
    {
        // If the 'exti-irq-callback' feature is enabled, call the user-provided callback function.
        // This allows users to define their own ISR handling logic.
        extern "Rust" {
            fn exti_irq_callback(line: u8);
        }
        exti_irq_callback(_line);
    }
}

#[cfg(feature = "embassy")]
pub mod exti_future {
    use core::future::Future;
    use core::pin::Pin;
    use core::task::{Context, Poll};
    use embassy_sync::waitqueue::AtomicWaker;
    use portable_atomic::{AtomicU16, Ordering};

    use crate::gpio::{PinNum, PortNum};

    const EXTI_COUNT: usize = 16;
    const NEW_AW: AtomicWaker = AtomicWaker::new();
    static EXTI_WAKERS: [AtomicWaker; EXTI_COUNT] = [NEW_AW; EXTI_COUNT];
    static EXTI_STATUS: AtomicU16 = AtomicU16::new(0);

    #[inline]
    pub fn exti_set_status(line: u8) {
        if line < EXTI_COUNT as u8 {
            EXTI_STATUS.bit_set(line as u32, Ordering::Relaxed);
            EXTI_WAKERS[line as usize].wake();
        }
    }

    impl super::super::Pin<super::super::pin::Input> {
        pub async fn wait_for_high(&mut self) {
            let fut = ExtiInputFuture::new(self.port, self.pin, super::ExtiMode::Rising);
            if Self::is_high(self) {
                return;
            }
            fut.await
        }

        pub async fn wait_for_low(&mut self) {
            let fut = ExtiInputFuture::new(self.port, self.pin, super::ExtiMode::Falling);
            if !Self::is_high(self) {
                return;
            }
            fut.await
        }

        pub async fn wait_for_rising_edge(&mut self) {
            ExtiInputFuture::new(self.port, self.pin, super::ExtiMode::Rising).await
        }

        pub async fn wait_for_falling_edge(&mut self) {
            ExtiInputFuture::new(self.port, self.pin, super::ExtiMode::Falling).await
        }

        pub async fn wait_for_any_edge(&mut self) {
            ExtiInputFuture::new(self.port, self.pin, super::ExtiMode::RisingFalling).await
        }
    }

    struct ExtiInputFuture {
        port: PortNum,
        pin: PinNum,
    }

    impl ExtiInputFuture {
        fn new(port: PortNum, pin: PinNum, mode: super::ExtiMode) -> Self {
            ll_invoke_inner!(crate::INVOKE_ID_GPIO_EXTI, port, pin, mode.to_flag());
            //ll_invoke_inner!(crate::INVOKE_ID_GPIO_EXTI, pin, super::GpioExtiFlag::Enable);
            Self { port, pin }
        }
    }

    impl Drop for ExtiInputFuture {
        fn drop(&mut self) {
            ll_invoke_inner!(
                crate::INVOKE_ID_GPIO_EXTI,
                self.port,
                self.pin,
                super::GpioExtiFlag::Disable
            );
        }
    }

    impl Future for ExtiInputFuture {
        type Output = ();

        fn poll(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output> {
            EXTI_WAKERS[self.pin as usize].register(cx.waker());

            if EXTI_STATUS.bit_clear(self.pin as u32, Ordering::Relaxed) {
                Poll::Ready(())
            } else {
                Poll::Pending
            }
        }
    }

    impl embedded_hal_async::digital::Wait for super::super::Pin<super::super::pin::Input> {
        async fn wait_for_high(&mut self) -> Result<(), Self::Error> {
            self.wait_for_high().await;
            Ok(())
        }

        async fn wait_for_low(&mut self) -> Result<(), Self::Error> {
            self.wait_for_low().await;
            Ok(())
        }

        async fn wait_for_rising_edge(&mut self) -> Result<(), Self::Error> {
            self.wait_for_rising_edge().await;
            Ok(())
        }

        async fn wait_for_falling_edge(&mut self) -> Result<(), Self::Error> {
            self.wait_for_falling_edge().await;
            Ok(())
        }

        async fn wait_for_any_edge(&mut self) -> Result<(), Self::Error> {
            self.wait_for_any_edge().await;
            Ok(())
        }
    }
}
