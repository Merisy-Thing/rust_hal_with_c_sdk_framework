use crate::gpio::Pin;
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

impl<'d> super::pin::Input<'d> {
    /// Configures the EXTI (External Interrupt) mode for the pin.
    ///
    /// # Arguments
    /// * `mode` - The EXTI mode to set. This can be rising edge, falling edge, both edges, etc.
    pub fn exti_config(&self, mode: ExtiMode) {
        let port = self.pin.pin.port();
        let pin = self.pin.pin.pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_EXTI, port, pin, mode.to_flag());
    }

    /// Enables the interrupt for the pin.
    ///
    /// This function can be reenabled after disabling it.
    pub fn enable_interrupt(&self) {
        let port = self.pin.pin.port();
        let pin = self.pin.pin.pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_EXTI, port, pin, GpioExtiFlag::Enable);
    }

    /// Disables the interrupt for the pin.
    ///
    /// This function can be used to temporarily disable the interrupt without changing the EXTI mode.
    pub fn disable_interrupt(&self) {
        let port = self.pin.pin.port();
        let pin = self.pin.pin.pin();

        ll_invoke_inner!(INVOKE_ID_GPIO_EXTI, port, pin, GpioExtiFlag::Disable);
    }
}

#[allow(non_snake_case)]
#[no_mangle]
/// An unsafe C-compatible external function for hook EXTI IRQs.
/// # Arguments
/// * `_line` - The EXTI line number that triggered the interrupt. This corresponds to the pin number.
unsafe extern "C" fn EXTI_IRQ_hook_rs(_line: u8) {
    #[cfg(all(feature = "embassy", feature = "exti-async"))]
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

#[cfg(all(feature = "embassy", feature = "exti-async"))]
pub mod exti_future {
    use crate::gpio::Pin;
    use core::future::Future;
    use core::task::{Context, Poll};
    use embassy_sync::waitqueue::AtomicWaker;
    use portable_atomic::{AtomicU16, Ordering};

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

    impl<'d> super::super::pin::Input<'d> {
        pub async fn wait_for_high(&mut self) {
            let port = self.pin.pin.port();
            let pin = self.pin.pin.pin();
            let fut = ExtiInputFuture::new(port, pin, super::ExtiMode::Rising);
            if Self::is_high(self) {
                return;
            }
            fut.await
        }

        pub async fn wait_for_low(&mut self) {
            let port = self.pin.pin.port();
            let pin = self.pin.pin.pin();
            let fut = ExtiInputFuture::new(port, pin, super::ExtiMode::Falling);
            if !Self::is_high(self) {
                return;
            }
            fut.await
        }

        pub async fn wait_for_rising_edge(&mut self) {
            let port = self.pin.pin.port();
            let pin = self.pin.pin.pin();
            ExtiInputFuture::new(port, pin, super::ExtiMode::Rising).await
        }

        pub async fn wait_for_falling_edge(&mut self) {
            let port = self.pin.pin.port();
            let pin = self.pin.pin.pin();
            ExtiInputFuture::new(port, pin, super::ExtiMode::Falling).await
        }

        pub async fn wait_for_any_edge(&mut self) {
            let port = self.pin.pin.port();
            let pin = self.pin.pin.pin();
            ExtiInputFuture::new(port, pin, super::ExtiMode::RisingFalling).await
        }
    }

    struct ExtiInputFuture {
        port: u8,
        pin: u8,
    }

    impl ExtiInputFuture {
        fn new(port: u8, pin: u8, mode: super::ExtiMode) -> Self {
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

        fn poll(self: core::pin::Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output> {
            EXTI_WAKERS[self.pin as usize].register(cx.waker());

            if EXTI_STATUS.bit_clear(self.pin as u32, Ordering::Relaxed) {
                Poll::Ready(())
            } else {
                Poll::Pending
            }
        }
    }

    impl<'d> embedded_hal_async::digital::Wait for super::super::Input<'d> {
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
