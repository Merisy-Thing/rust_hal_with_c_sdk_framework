use crate::ll_api::{ll_cmd::*, I2CBusAddrBits};
use embedded_hal::i2c::{NoAcknowledgeSource, SevenBitAddress, TenBitAddress};
pub use crate::ll_api::I2CBusId;

pub const FREQ_I2C_SCL_100K: u32 = 100_000;
pub const FREQ_I2C_SCL_400K: u32 = 400_000;

/// Represents errors that can occur during I2C communication.
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum I2CError {
    Code(i32),
}

/// Represents I2C addresses.
#[derive(Debug, Clone, Copy)]
pub enum I2CAddr {
    /// A 7-bit address.
    SevenBitAddress(u8),
    /// A 10-bit address.
    TenBitAddress(u16),
}

/// Implementation of the I2C interface.
#[derive(Clone, Debug)]
pub struct I2C<const CLK_HZ: u32> {
    bus: I2CBusId,
}

impl<const CLK_HZ: u32> I2C<CLK_HZ> {
    /// Creates a new `I2C` instance.
    pub fn new(bus: I2CBusId, flags: Option<u32>) -> Self {
        let flag_param = if let Some(flags) = flags {
            flags
        } else {
            0
        };

        ll_invoke_inner!(INVOKE_ID_I2C_INIT, bus, CLK_HZ, flag_param);

        I2C { bus }
    }

    /// Writes bytes and reads bytes over the I2C bus.
    ///
    /// # Examples
    /// ```
    /// let mut buffer = [0; 4];
    /// i2c.blocking_write_read(I2CAddr::SevenBitAddress(0x48), &[0x00, 0x01], &mut buffer)?;
    /// assert_eq!(buffer, [0x02, 0x03, 0x04, 0x05]);
    /// ```
    /// ``` text
    /// Master: ST SAD+W     O0     O1     ... OM     SR SAD+R        MAK    MAK ...    NMAK SP
    /// Slave:           SAK    SAK    SAK ...    SAK          SAK I0     I1     ... IN
    /// ```
    pub fn blocking_write_read(
        &mut self,
        address: I2CAddr,
        write: &[u8],
        read: &mut [u8],
    ) -> Result<(), I2CError> {
        let (bus_addr_bits, addr) = match address {
            I2CAddr::SevenBitAddress(addr) => (I2CBusAddrBits::SevenBitAddr, addr as u16),
            I2CAddr::TenBitAddress(addr) => (I2CBusAddrBits::TenBitAddr, addr),
        };
        let result = ll_invoke_inner!(
            INVOKE_ID_I2C_WRITE_READ,
            self.bus,
            bus_addr_bits,
            addr,
            write.as_ptr(),
            write.len(),
            read.as_mut_ptr(),
            read.len()
        );

        if result < 0 {
            return Err(I2CError::Code(result));
        }

        Ok(())
    }

    /// Writes bytes over the I2C bus.
    ///
    /// # Examples
    /// ```
    /// i2c.blocking_write(I2CAddr::SevenBitAddress(0x48), &[0x00, 0x01])?;
    /// ```
    ///  ``` text
    /// Master: ST SAD+W     B0     B1     ... BN     SP
    /// Slave:           SAK    SAK    SAK ...    SAK
    /// ```
    pub fn blocking_write(&mut self, address: I2CAddr, write: &[u8]) -> Result<(), I2CError> {
        let (bus_addr_bits, addr) = match address {
            I2CAddr::SevenBitAddress(addr) => (I2CBusAddrBits::SevenBitAddr, addr as u16),
            I2CAddr::TenBitAddress(addr) => (I2CBusAddrBits::TenBitAddr, addr),
        };
        let result = ll_invoke_inner!(
            INVOKE_ID_I2C_WRITE,
            self.bus,
            bus_addr_bits,
            addr,
            write.as_ptr(),
            write.len()
        );

        if result < 0 {
            return Err(I2CError::Code(result));
        }
        Ok(())
    }

    /// Reads bytes over the I2C bus.
    ///
    /// # Examples
    /// ```
    /// let mut buffer = [0; 4];
    /// i2c.blocking_read(I2CAddr::SevenBitAddress(0x48), &mut buffer)?;
    /// assert_eq!(buffer, [0x02, 0x03, 0x04, 0x05]);
    /// ```
    /// ``` text
    /// Master: ST SAD+R        MAK    MAK ...    NMAK SP
    /// Slave:           SAK B0     B1     ... BN
    /// ```
    pub fn blocking_read(&mut self, address: I2CAddr, read: &mut [u8]) -> Result<(), I2CError> {
        let (bus_addr_bits, addr) = match address {
            I2CAddr::SevenBitAddress(addr) => (I2CBusAddrBits::SevenBitAddr, addr as u16),
            I2CAddr::TenBitAddress(addr) => (I2CBusAddrBits::TenBitAddr, addr),
        };
        let result = ll_invoke_inner!(
            INVOKE_ID_I2C_READ,
            self.bus,
            bus_addr_bits,
            addr,
            read.as_mut_ptr(),
            read.len()
        );

        if result < 0 {
            return Err(I2CError::Code(result));
        }
        Ok(())
    }
}

impl<const CLK_HZ: u32> Drop for I2C<CLK_HZ> {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_I2C_DEINIT, self.bus);
    }
}

impl embedded_hal::i2c::Error for I2CError {
    fn kind(&self) -> embedded_hal::i2c::ErrorKind {
        match *self {
            I2CError::Code(code) => match code {
                -1 => embedded_hal::i2c::ErrorKind::NoAcknowledge(NoAcknowledgeSource::Address),
                -2 => embedded_hal::i2c::ErrorKind::NoAcknowledge(NoAcknowledgeSource::Data),
                -3 => embedded_hal::i2c::ErrorKind::Bus,
                -4 => embedded_hal::i2c::ErrorKind::ArbitrationLoss,
                -5 => embedded_hal::i2c::ErrorKind::Overrun,
                _ => embedded_hal::i2c::ErrorKind::Other,
            },
        }
    }
}

impl<const CLK_HZ: u32> embedded_hal::i2c::ErrorType for I2C<CLK_HZ> {
    type Error = I2CError;
}

impl<const CLK_HZ: u32> embedded_hal::i2c::I2c<SevenBitAddress> for I2C<CLK_HZ> {
    fn read(&mut self, address: u8, read: &mut [u8]) -> Result<(), Self::Error> {
        self.blocking_read(I2CAddr::SevenBitAddress(address), read)
    }

    fn write(&mut self, address: u8, write: &[u8]) -> Result<(), Self::Error> {
        self.blocking_write(I2CAddr::SevenBitAddress(address), write)
    }

    fn write_read(
        &mut self,
        address: u8,
        write: &[u8],
        read: &mut [u8],
    ) -> Result<(), Self::Error> {
        self.blocking_write_read(I2CAddr::SevenBitAddress(address), write, read)
    }

    fn transaction(
        &mut self,
        address: u8,
        operations: &mut [embedded_hal::i2c::Operation<'_>],
    ) -> Result<(), Self::Error> {
        for op in operations {
            match op {
                embedded_hal::i2c::Operation::Write(data) => {
                    self.blocking_write(I2CAddr::SevenBitAddress(address), data)?;
                }
                embedded_hal::i2c::Operation::Read(data) => {
                    self.blocking_read(I2CAddr::SevenBitAddress(address), data)?;
                }
            }
        }
        Ok(())
    }
}

impl<const CLK_HZ: u32> embedded_hal::i2c::I2c<TenBitAddress> for I2C<CLK_HZ> {
    fn read(&mut self, address: u16, read: &mut [u8]) -> Result<(), Self::Error> {
        self.blocking_read(I2CAddr::TenBitAddress(address), read)
    }

    fn write(&mut self, address: u16, write: &[u8]) -> Result<(), Self::Error> {
        self.blocking_write(I2CAddr::TenBitAddress(address), write)
    }

    fn write_read(
        &mut self,
        address: u16,
        write: &[u8],
        read: &mut [u8],
    ) -> Result<(), Self::Error> {
        self.blocking_write_read(I2CAddr::TenBitAddress(address), write, read)
    }

    fn transaction(
        &mut self,
        address: u16,
        operations: &mut [embedded_hal::i2c::Operation<'_>],
    ) -> Result<(), Self::Error> {
        for op in operations {
            match op {
                embedded_hal::i2c::Operation::Write(data) => {
                    self.blocking_write(I2CAddr::TenBitAddress(address), data)?;
                }
                embedded_hal::i2c::Operation::Read(data) => {
                    self.blocking_read(I2CAddr::TenBitAddress(address), data)?;
                }
            }
        }
        Ok(())
    }
}
