use embedded_hal::{
    delay::DelayNs,
    digital::OutputPin,
    i2c::{NoAcknowledgeSource, SevenBitAddress, TenBitAddress},
};

use super::OpenDrainPin;

/// Represents errors that can occur during I2C communication.
#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum SoftI2CError {
    /// Error when addressing the device.
    Address,
    /// Error when writing data.
    WriteData,
}

/// Represents I2C addresses.
#[derive(Debug, Clone, Copy)]
pub enum SoftI2CAddr {
    /// A 7-bit address.
    SevenBitAddress(u8),
    /// A 10-bit address.
    TenBitAddress(u16),
}

/// A software implementation of the I2C interface.
///
/// let scl = /* Initialize SCL pin as open drain mode*/;
/// let sda = /* Initialize SDA pin as open drain mode*/;
/// let delay = Delay::new(); /*Delay need impl DelayNs trait*/
/// let i2c = SoftI2C::new(scl, sda, delay);
/// ```
#[derive(Debug)]
pub struct SoftI2C<'a, C, D, Delay, const CLK_HZ: u32> {
    /// SCL pin.
    scl: &'a mut C,
    /// SDA pin.
    sda: &'a mut D,
    /// Delay utility for timing.
    delay: &'a mut Delay,
}

impl<'a, C, D, Delay, const CLK_HZ: u32> SoftI2C<'a, C, D, Delay, CLK_HZ>
where
    C: OutputPin,
    D: OpenDrainPin,
    Delay: DelayNs,
{
    /// Creates a new `SoftI2C` instance with the given SCL and SDA pins.
    pub fn new(scl: &'a mut C, sda: &'a mut D, delay: &'a mut Delay) -> Self {
        scl.set_high().ok();
        sda.set(true);

        SoftI2C { scl, sda, delay }
    }

    /// Delays for a short period.
    #[inline]
    fn i2c_delay(&mut self) {
        self.delay.delay_ns(500_000_000 / CLK_HZ);
    }

    /// Sets the SCL line high.
    fn scl_hi(&mut self) {
        self.scl.set_high().ok();
        self.i2c_delay();
    }

    /// Sets the SCL line low.
    fn scl_lo(&mut self) {
        self.scl.set_low().ok();
        self.i2c_delay();
    }

    /// Sets the SDA line high.
    fn sda_hi(&mut self) {
        self.sda.set(true);
        self.i2c_delay();
    }

    /// Sets the SDA line low.
    fn sda_lo(&mut self) {
        self.sda.set(false);
        self.i2c_delay();
    }

    /// Set the SDA pin high to free pull low ctrl.
    #[inline]
    fn sda_free(&mut self) {
        self.sda.set(true);
    }
    /// Generates a START condition.
    fn start(&mut self) {
        self.sda.set(true);
        self.scl_hi();
        self.sda_lo();
        self.scl_lo();
    }

    /// Generates a STOP condition.
    fn stop(&mut self) {
        self.sda_lo();
        self.scl_hi();
        self.sda.set(true);
    }

    /// Generates a clock pulse.
    fn clock_pulse(&mut self) {
        self.scl_hi();
        self.scl_lo();
    }

    /// Writes a byte to the I2C bus.
    fn write_byte(&mut self, mut data_byte: u8) {
        for _ in 0..8 {
            self.sda.set(data_byte & 0x80 != 0);
            data_byte <<= 1;
            self.clock_pulse();
        }
    }

    /// Reads a single bit from the I2C bus.
    fn read_bit(&mut self) -> bool {
        self.scl_hi();
        let sda_bit = self.sda.get();
        self.scl_lo();

        sda_bit
    }

    /// Reads a byte from the I2C bus.
    fn read_byte(&mut self) -> u8 {
        let mut out_byte = 0;

        self.sda_free();
        for _ in 0..8 {
            out_byte <<= 1;
            out_byte |= (self.read_bit() as u8) & 1;
        }

        out_byte
    }

    /// Reads an acknowledge bit.
    fn read_ack(&mut self, err: SoftI2CError) -> Result<(), SoftI2CError> {
        self.sda_free();
        if self.read_bit() {
            self.stop();
            Err(err)
        } else {
            Ok(())
        }
    }

    /// Generates a repeated start condition.
    fn mak(&mut self) {
        self.sda_lo();
        self.clock_pulse();
    }

    /// Generates a repeated stop condition.
    fn nmak(&mut self) {
        self.sda_hi();
        self.clock_pulse();
    }

    /// Sends the device address over the I2C bus.
    fn address(&mut self, address: SoftI2CAddr, is_read: bool) -> Result<(), SoftI2CError> {
        let r_w_bit = if is_read { 1 } else { 0 };

        match address {
            SoftI2CAddr::SevenBitAddress(addr) => {
                self.write_byte((addr << 1) | r_w_bit);
                self.read_ack(SoftI2CError::Address)?
            }
            SoftI2CAddr::TenBitAddress(addr) => {
                let h_addr = ((((addr >> 7) as u8 & 0xFE) | r_w_bit) & 0x07) | 0xF0;
                let l_addr = (addr & 0xFF) as u8;
                for byte in [h_addr, l_addr] {
                    self.write_byte(byte);
                    self.read_ack(SoftI2CError::Address)?
                }
            }
        }
        Ok(())
    }

    /// Writes bytes and reads bytes over the I2C bus.
    ///
    /// # Examples
    /// ```
    /// let mut buffer = [0; 4];
    /// i2c.soft_i2c_write_read(SoftI2CAddr::SevenBitAddress(0x48), &[0x00, 0x01], &mut buffer)?;
    /// assert_eq!(buffer, [0x02, 0x03, 0x04, 0x05]);
    /// ```
    /// ``` text
    /// Master: ST SAD+W     O0     O1     ... OM     SR SAD+R        MAK    MAK ...    NMAK SP
    /// Slave:           SAK    SAK    SAK ...    SAK          SAK I0     I1     ... IN
    /// ```
    pub fn soft_i2c_write_read(
        &mut self,
        address: SoftI2CAddr,
        write: &[u8],
        read: &mut [u8],
    ) -> Result<(), SoftI2CError> {
        self.start();
        self.address(address, false)?;

        for i in 0..write.len() {
            self.write_byte(write[i]);
            self.read_ack(SoftI2CError::WriteData)?;
        }

        self.soft_i2c_read(address, read)
    }

    /// Writes bytes over the I2C bus.
    ///
    /// # Examples
    /// ```
    /// i2c.soft_i2c_write(SoftI2CAddr::SevenBitAddress(0x48), &[0x00, 0x01])?;
    /// ```
    ///  ``` text
    /// Master: ST SAD+W     B0     B1     ... BN     SP
    /// Slave:           SAK    SAK    SAK ...    SAK
    /// ```
    pub fn soft_i2c_write(
        &mut self,
        address: SoftI2CAddr,
        write: &[u8],
    ) -> Result<(), SoftI2CError> {
        self.start();
        self.address(address, false)?;

        for i in 0..write.len() {
            self.write_byte(write[i]);
            self.read_ack(SoftI2CError::WriteData)?;
        }
        self.stop();

        Ok(())
    }

    /// Reads bytes over the I2C bus.
    ///
    /// # Examples
    /// ```
    /// let mut buffer = [0; 4];
    /// i2c.soft_i2c_read(SoftI2CAddr::SevenBitAddress(0x48), &mut buffer)?;
    /// assert_eq!(buffer, [0x02, 0x03, 0x04, 0x05]);
    /// ```
    /// ``` text
    /// Master: ST SAD+R        MAK    MAK ...    NMAK SP
    /// Slave:           SAK B0     B1     ... BN
    /// ```
    pub fn soft_i2c_read(
        &mut self,
        address: SoftI2CAddr,
        read: &mut [u8],
    ) -> Result<(), SoftI2CError> {
        self.start();
        self.address(address, true)?;

        for i in 0..read.len() {
            read[i] = self.read_byte();
            if i < read.len() - 1 {
                self.mak();
            } else {
                self.nmak();
            }
        }
        self.stop();

        Ok(())
    }
}

impl embedded_hal::i2c::Error for SoftI2CError {
    fn kind(&self) -> embedded_hal::i2c::ErrorKind {
        match *self {
            SoftI2CError::Address => {
                embedded_hal::i2c::ErrorKind::NoAcknowledge(NoAcknowledgeSource::Address)
            }
            SoftI2CError::WriteData => {
                embedded_hal::i2c::ErrorKind::NoAcknowledge(NoAcknowledgeSource::Data)
            }
        }
    }
}

impl<'a, C: OutputPin, D: OpenDrainPin, Delay: DelayNs, const CLK_HZ: u32>
    embedded_hal::i2c::ErrorType for SoftI2C<'a, C, D, Delay, CLK_HZ>
{
    type Error = SoftI2CError;
}

impl<'a, C: OutputPin, D: OpenDrainPin, Delay: DelayNs, const CLK_HZ: u32>
    embedded_hal::i2c::I2c<SevenBitAddress> for SoftI2C<'a, C, D, Delay, CLK_HZ>
{
    fn read(&mut self, address: u8, read: &mut [u8]) -> Result<(), Self::Error> {
        self.soft_i2c_read(SoftI2CAddr::SevenBitAddress(address), read)
    }

    fn write(&mut self, address: u8, write: &[u8]) -> Result<(), Self::Error> {
        self.soft_i2c_write(SoftI2CAddr::SevenBitAddress(address), write)
    }

    fn write_read(
        &mut self,
        address: u8,
        write: &[u8],
        read: &mut [u8],
    ) -> Result<(), Self::Error> {
        self.soft_i2c_write_read(SoftI2CAddr::SevenBitAddress(address), write, read)
    }

    fn transaction(
        &mut self,
        address: u8,
        operations: &mut [embedded_hal::i2c::Operation<'_>],
    ) -> Result<(), Self::Error> {
        for op in operations {
            match op {
                embedded_hal::i2c::Operation::Write(data) => {
                    self.soft_i2c_write(SoftI2CAddr::SevenBitAddress(address), data)?;
                }
                embedded_hal::i2c::Operation::Read(data) => {
                    self.soft_i2c_read(SoftI2CAddr::SevenBitAddress(address), data)?;
                }
            }
        }
        Ok(())
    }
}

impl<'a, C: OutputPin, D: OpenDrainPin, Delay: DelayNs, const CLK_HZ: u32>
    embedded_hal::i2c::I2c<TenBitAddress> for SoftI2C<'a, C, D, Delay, CLK_HZ>
{
    fn read(&mut self, address: u16, read: &mut [u8]) -> Result<(), Self::Error> {
        self.soft_i2c_read(SoftI2CAddr::TenBitAddress(address), read)
    }

    fn write(&mut self, address: u16, write: &[u8]) -> Result<(), Self::Error> {
        self.soft_i2c_write(SoftI2CAddr::TenBitAddress(address), write)
    }

    fn write_read(
        &mut self,
        address: u16,
        write: &[u8],
        read: &mut [u8],
    ) -> Result<(), Self::Error> {
        self.soft_i2c_write_read(SoftI2CAddr::TenBitAddress(address), write, read)
    }

    fn transaction(
        &mut self,
        address: u16,
        operations: &mut [embedded_hal::i2c::Operation<'_>],
    ) -> Result<(), Self::Error> {
        for op in operations {
            match op {
                embedded_hal::i2c::Operation::Write(data) => {
                    self.soft_i2c_write(SoftI2CAddr::TenBitAddress(address), data)?;
                }
                embedded_hal::i2c::Operation::Read(data) => {
                    self.soft_i2c_read(SoftI2CAddr::TenBitAddress(address), data)?;
                }
            }
        }
        Ok(())
    }
}
