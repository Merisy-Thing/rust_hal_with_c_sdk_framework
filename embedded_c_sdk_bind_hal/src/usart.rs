use crate::common::atomic_ring_buffer::RingBuffer;
use crate::ll_api::ll_cmd::*;
#[cfg(feature = "embassy")]
use embassy_sync::waitqueue::AtomicWaker;

pub use crate::ll_api::{
    UsartDataBits, UsartHwFlowCtrl, UsartId, UsartMode, UsartParity, UsartStopBits,
};

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
/// Config
pub struct Config {
    /// Baud rate
    pub baudrate: u32,
    /// Number of data bits
    pub data_bits: UsartDataBits,
    /// Number of stop bits
    pub stop_bits: UsartStopBits,
    /// Parity type
    pub parity: UsartParity,
    /// Hardware Flow Control
    pub hw_flow: UsartHwFlowCtrl,
    /// Tx Rx mode
    pub tx_rx_mode: UsartMode,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            baudrate: 115200,
            data_bits: UsartDataBits::DataBits8,
            stop_bits: UsartStopBits::STOP1,
            parity: UsartParity::ParityNone,
            hw_flow: UsartHwFlowCtrl::None,
            tx_rx_mode: UsartMode::TxRx,
        }
    }
}

impl Config {
    pub fn flags(&self) -> u32 {
        let mut flags: u32;

        flags = self.data_bits as u32;
        flags |= self.stop_bits as u32;
        flags |= self.parity as u32;
        flags |= self.hw_flow as u32;
        flags |= self.tx_rx_mode as u32;

        flags
    }
}

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum Error {
    Code(i32),
}

pub struct UsartInner {
    id: UsartId,
    rx_buf: RingBuffer<u8>,
    #[cfg(feature = "embassy")]
    rx_waker: AtomicWaker,
}

#[derive(Clone)]
pub struct Usart<'a> {
    inner: &'a UsartInner,
}

impl<'a> Usart<'a> {
    /// Creates a new Usart instance wrapping around the given UsartInner reference.
    ///
    /// # Arguments
    /// * `usart` - A reference to the inner Usart implementation.
    ///
    /// # Returns
    /// * A new Usart instance.
    pub fn new(usart: &'a UsartInner) -> Self {
        Usart { inner: usart }
    }

    /// Initializes the USART with the provided configuration.
    ///
    /// # Arguments
    /// * `config` - A reference to the configuration settings for the USART.
    ///
    /// # Returns
    /// * An i32 indicating the status of the initialization.
    pub fn init(&self, config: &Config) -> i32 {
        ll_invoke_inner!(
            INVOKE_ID_USART_INIT,
            self.inner.id,
            config.flags(),
            config.baudrate
        )
    }

    /// Sets the receive buffer for the USART.
    ///
    /// # Arguments
    /// * `rx_buffer` - A mutable slice representing the receive buffer.
    pub fn set_rx_buf(&self, rx_buffer: &mut [u8]) {
        let len = rx_buffer.len();
        unsafe { self.inner.rx_buf.init(rx_buffer.as_mut_ptr(), len) };
    }

    /// Writes a buffer to the USART in a blocking manner.
    ///
    /// # Arguments
    /// * `buf` - A slice containing the data to write.
    ///
    /// # Returns
    /// * An i32 indicating the status of the write operation.
    pub fn blocking_write(&self, buf: &[u8]) -> i32 {
        ll_invoke_inner!(
            INVOKE_ID_USART_WRITE,
            self.inner.id,
            buf.as_ptr(),
            buf.len()
        )
    }

    /// Reads into a buffer from the USART in a blocking manner.
    ///
    /// # Arguments
    /// * `buffer` - A mutable slice representing the read buffer.
    pub fn blocking_read(&self, buffer: &mut [u8]) {
        let rx_buf = &self.inner.rx_buf;
        let mut reader = unsafe { rx_buf.reader() };

        for b in buffer {
            while rx_buf.is_empty() {}
            *b = reader.pop_one().unwrap_or(0);
        }
    }

    /// Attempts to read a byte from the USART in a non-blocking manner.
    ///
    /// # Returns
    /// * A Result containing the read byte or an error if the buffer is empty.
    pub(crate) fn nb_read(&mut self) -> Result<u8, nb::Error<Error>> {
        let rx_buf = &self.inner.rx_buf;

        if !rx_buf.is_empty() {
            let mut reader = unsafe { rx_buf.reader() };
            Ok(reader.pop_one().unwrap_or(0))
        } else {
            Err(nb::Error::WouldBlock)
        }
    }
    
    /// An asynchronous read function for reading data into a provided buffer.
    /// 
    /// # Arguments
    /// * `buffer` - A mutable slice where the read bytes will be stored.
    ///
    /// # Returns
    /// * Returns the number of bytes read as an `i32`, or `-1` if the buffer is empty,
    ///   or `-2` if there was an error during the read operation.
    #[cfg(feature = "embassy")]
    async fn async_read(&self, buffer: &mut [u8]) -> i32 {
        if buffer.is_empty() {
            return -1;
        }

        let buffer_len = buffer.len();
        let mut buffer_idx = 0;

        let abort = core::future::poll_fn(move |cx| {
            self.inner.rx_waker.register(cx.waker());
            
            let mut reader = unsafe { self.inner.rx_buf.reader() };
            while let Some(byte) = reader.pop_one() {
                buffer[buffer_idx] = byte;
                buffer_idx += 1;
                if buffer_idx >= buffer_len {
                    return core::task::Poll::Ready(Ok::<(), Error>(()));
                }
            }
            
            return core::task::Poll::Pending;
        });

        let result = match abort.await {
            Ok(_) => buffer_len as i32,
            Err(_) => -2,
        };

        result
    }
}

impl Drop for Usart<'_> {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_USART_DEINIT, self.inner.id);
    }
}

impl embedded_io::Error for Error {
    fn kind(&self) -> embedded_io::ErrorKind {
        embedded_io::ErrorKind::Other
    }
}

impl embedded_io::ErrorType for Usart<'_> {
    type Error = Error;
}

impl<'d> embedded_io::Read for Usart<'_> {
    fn read(&mut self, buf: &mut [u8]) -> Result<usize, Self::Error> {
        self.blocking_read(buf);
        Ok(buf.len())
    }
}

impl embedded_io::Write for Usart<'_> {
    fn write(&mut self, buf: &[u8]) -> Result<usize, Self::Error> {
        let result = self.blocking_write(buf);
        if result == 0 {
            return Ok(buf.len());
        }
        return Err(Error::Code(result));
    }

    fn flush(&mut self) -> Result<(), Self::Error> {
        Ok(())
    }
}

#[cfg(feature = "embassy")]
impl embedded_io_async::Write for Usart<'_> {
    async fn write(&mut self, buf: &[u8]) -> Result<usize, Self::Error> {
        let result = self.blocking_write(buf);
        if result == 0 {
            return Ok(buf.len());
        }
        return Err(Error::Code(result));
    }
}

#[cfg(feature = "embassy")]
impl embedded_io_async::Read for Usart<'_> {
    async fn read(&mut self, buf: &mut [u8]) -> Result<usize, Self::Error> {
        let result = self.async_read(buf).await;
        if result > 0 {
            return Ok(result as usize);
        }
        return Err(Error::Code(result));
    }
}

impl embedded_hal_nb::serial::Error for Error {
    fn kind(&self) -> embedded_hal_nb::serial::ErrorKind {
        embedded_hal_nb::serial::ErrorKind::Other
    }
}
impl<'d> embedded_hal_nb::serial::ErrorType for Usart<'_> {
    type Error = Error;
}

impl<'d> embedded_hal_nb::serial::Read for Usart<'_> {
    fn read(&mut self) -> nb::Result<u8, Self::Error> {
        self.nb_read()
    }
}

impl<'d> embedded_hal_nb::serial::Write for Usart<'_> {
    fn write(&mut self, char: u8) -> nb::Result<(), Self::Error> {
        let result = self.blocking_write(&[char]);
        if result == 0 {
            return Ok(());
        }
        return Err(nb::Error::Other(Error::Code(result)));
    }

    fn flush(&mut self) -> nb::Result<(), Self::Error> {
        Ok(())
    }
}

#[cfg(feature = "_usart_impl")]
macro_rules! impl_usart {
    ($USART_id:ident) => {
        pub static $USART_id: UsartInner = UsartInner {
            id: UsartId::$USART_id,
            rx_buf: RingBuffer::new(),
            #[cfg(feature = "embassy")]
            rx_waker: AtomicWaker::new(),
        };

        paste::paste! {
            #[allow(non_snake_case)]
            #[no_mangle]
            #[inline]
            unsafe extern "C" fn [<$USART_id _rx_hook_rs>] (val: u8) {
                $USART_id.rx_buf.writer().push_one(val);
                #[cfg(feature = "embassy")]
                $USART_id.rx_waker.wake();
            }
        }
    };
}

#[cfg(feature = "USART-0")]
impl_usart!(USART0);

#[cfg(feature = "USART-1")]
impl_usart!(USART1);

#[cfg(feature = "USART-2")]
impl_usart!(USART2);

#[cfg(feature = "USART-3")]
impl_usart!(USART3);

#[cfg(feature = "USART-4")]
impl_usart!(USART4);

#[cfg(feature = "USART-5")]
impl_usart!(USART5);

#[cfg(feature = "USART-6")]
impl_usart!(USART6);

#[cfg(feature = "USART-7")]
impl_usart!(USART7);
