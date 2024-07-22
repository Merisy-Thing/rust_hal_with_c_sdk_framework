pub use crate::ll_api::{SpiBusBitOrder, SpiBusDataSize, SpiBusId, SpiBusMode};
use crate::{
    gpio::{Output, Pin},
    ll_api::ll_cmd::*,
    tick::Delay,
};
use core::{cmp::min, ptr};
use embedded_hal::{delay::DelayNs, digital::OutputPin, spi::Operation};

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum Error {
    Code(i32),
}

/// Configuration structure for initializing a SPI (Serial Peripheral Interface) bus.
///
/// This struct holds the necessary parameters to configure a SPI bus before it can be used for communication.
///
/// # Fields
/// * `baudrate` - The baud rate at which the SPI bus operates, specifying the speed of the serial communication.
/// * `mode` - The SPI bus mode, determining the clock polarity and phase for data transmission and reception.
/// * `size` - The data size per word transferred over the SPI bus, typically 8 or 16 bits.
/// * `order` - The bit order for data transmission, either most significant bit first (MSB) or least significant bit first (LSB).
pub struct Config {
    pub baudrate: u32,
    pub mode: SpiBusMode,
    pub size: SpiBusDataSize,
    pub order: SpiBusBitOrder,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            baudrate: 8_000_000,
            mode: SpiBusMode::Mode0,
            size: SpiBusDataSize::DataSize8,
            order: SpiBusBitOrder::MsbFirst,
        }
    }
}

pub struct SpiBus {
    bus: SpiBusId,
}

impl SpiBus {
    /// Initializes a new SPI bus with the given configuration.
    ///
    /// # Arguments
    /// * `bus` - The identifier for the SPI bus.
    /// * `config` - A reference to the configuration parameters for the SPI bus.
    ///
    /// # Returns
    /// A new `SpiBus` instance configured according to the provided parameters.
    pub fn new(bus: SpiBusId, config: &Config) -> Self {
        let flags = config.size as u32 | config.order as u32;
        ll_invoke_inner!(INVOKE_ID_SPI_INIT, bus, config.mode, flags, config.baudrate);
        SpiBus { bus }
    }

    /// Converts the SPI bus into a SPI device by associating it with a chip select pin.
    ///
    /// # Arguments
    /// * `nss` - The chip select pin configured as an output.
    ///
    /// # Returns
    /// A `SpiDevice` instance ready to communicate with a specific device.
    pub fn to_device(self, nss: Pin<Output>) -> SpiDevice {
        SpiDevice { bus: self, nss }
    }

    /// Performs a blocking read operation on the SPI bus.
    ///
    /// # Arguments
    /// * `words` - A mutable slice where the read data will be stored.
    ///
    /// # Returns
    /// The number of bytes read or an error code.
    fn blocking_read(&mut self, words: &mut [u8]) -> i32 {
        ll_invoke_inner!(
            INVOKE_ID_SPI_BLOCKING_RW,
            self.bus,
            ptr::null::<u8>(),
            words.as_mut_ptr(),
            words.len()
        )
    }

    /// Performs a blocking write operation on the SPI bus.
    ///
    /// # Arguments
    /// * `words` - A slice containing the data to be written.
    ///
    /// # Returns
    /// The number of bytes written or an error code.
    fn blocking_write(&mut self, words: &[u8]) -> i32 {
        ll_invoke_inner!(
            INVOKE_ID_SPI_BLOCKING_RW,
            self.bus,
            words.as_ptr(),
            ptr::null::<u8>(),
            words.len()
        )
    }

    /// Performs a blocking transfer operation on the SPI bus.
    ///
    /// # Arguments
    /// * `read` - A mutable slice where the received data will be stored.
    /// * `write` - A slice containing the data to be sent.
    ///
    /// # Returns
    /// The number of bytes transferred or an error code.
    fn blocking_transfer(&mut self, read: &mut [u8], write: &[u8]) -> i32 {
        let size = min(read.len(), write.len());
        ll_invoke_inner!(
            INVOKE_ID_SPI_BLOCKING_RW,
            self.bus,
            write.as_ptr(),
            read.as_ptr(),
            size
        )
    }

    /// Performs a blocking transfer operation in place on the SPI bus.
    ///
    /// # Arguments
    /// * `words` - A mutable slice for both sending and receiving data.
    ///
    /// # Returns
    /// The number of bytes transferred or an error code.
    fn blocking_transfer_in_place(&mut self, words: &mut [u8]) -> i32 {
        let rw_ptr = words.as_ptr();
        ll_invoke_inner!(
            INVOKE_ID_SPI_BLOCKING_RW,
            self.bus,
            rw_ptr,
            rw_ptr,
            words.len()
        )
    }
}

impl Drop for SpiBus {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_SPI_DEINIT, self.bus);
    }
}

impl embedded_hal::spi::ErrorType for SpiBus {
    type Error = Error;
}

impl embedded_hal::spi::SpiBus for SpiBus {
    fn flush(&mut self) -> Result<(), Self::Error> {
        Ok(())
    }

    fn read(&mut self, words: &mut [u8]) -> Result<(), Self::Error> {
        let result = self.blocking_read(words);
        if result == 0 {
            return Ok(());
        }
        return Err(Error::Code(result));
    }

    fn write(&mut self, words: &[u8]) -> Result<(), Self::Error> {
        let result = self.blocking_write(words);
        if result == 0 {
            return Ok(());
        }
        return Err(Error::Code(result));
    }

    fn transfer(&mut self, read: &mut [u8], write: &[u8]) -> Result<(), Self::Error> {
        let result = self.blocking_transfer(read, write);
        if result == 0 {
            return Ok(());
        }
        return Err(Error::Code(result));
    }

    fn transfer_in_place(&mut self, words: &mut [u8]) -> Result<(), Self::Error> {
        let result = self.blocking_transfer_in_place(words);
        if result == 0 {
            return Ok(());
        }
        return Err(Error::Code(result));
    }
}

impl embedded_hal::spi::Error for Error {
    fn kind(&self) -> embedded_hal::spi::ErrorKind {
        match *self {
            Self::Code(_) => embedded_hal::spi::ErrorKind::Other,
        }
    }
}

pub struct SpiDevice {
    bus: SpiBus,
    nss: Pin<Output>,
}

impl SpiDevice {
    /// Creates a new `SpiDevice` instance.
    ///
    /// # Arguments
    /// * `bus` - A reference to the SPI bus this device will communicate over.
    /// * `nss` - A pin configured as an output, used as the chip select line for the device.
    ///
    /// # Returns
    /// A new `SpiDevice` instance ready to communicate with the device over the provided SPI bus.
    pub fn new(bus: SpiBus, nss: Pin<Output>) -> Self {
        SpiDevice { bus, nss }
    }
}

impl embedded_hal::spi::ErrorType for SpiDevice {
    type Error = Error;
}

impl embedded_hal::spi::SpiDevice for SpiDevice {
    fn transaction(&mut self, operations: &mut [Operation<'_, u8>]) -> Result<(), Self::Error> {
        let mut result = Ok(());

        self.nss.set_low().ok();
        for op in operations {
            match op {
                Operation::Read(words) => {
                    let ret = self.bus.blocking_read(words);
                    if ret != 0 {
                        result = Err(Error::Code(ret));
                    }
                }
                Operation::Write(words) => {
                    let ret = self.bus.blocking_write(words);
                    if ret != 0 {
                        result = Err(Error::Code(ret));
                    }
                }
                Operation::Transfer(rd_words, wr_words) => {
                    let ret = self.bus.blocking_transfer(rd_words, wr_words);
                    if ret != 0 {
                        result = Err(Error::Code(ret));
                    }
                }
                Operation::TransferInPlace(words) => {
                    let ret = self.bus.blocking_transfer_in_place(words);
                    if ret != 0 {
                        result = Err(Error::Code(ret));
                    }
                }
                Operation::DelayNs(ns) => {
                    let mut delay = Delay::<1000>::new();
                    delay.delay_ns(*ns);
                }
            }
        }
        self.nss.set_high().ok();

        result
    }
}
