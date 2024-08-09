use crate::common::atomic_ring_buffer::RingBuffer;
use crate::ll_api::{ll_cmd::*, AdcCtrl};
#[cfg(feature = "_adc-buffered")]
use paste::paste;

pub use crate::ll_api::AdcChannel;

#[cfg(not(feature = "adc-data-type-u8"))]
pub type AdcDataType = u16;

#[cfg(feature = "adc-data-type-u8")]
pub type AdcDataType = u8;

pub struct Adc {
    ch: AdcChannel,
}

impl Adc {
    /// Creates a new instance of the ADC with the specified channel and optional configuration flags.
    ///
    /// # Arguments
    /// * `ch` - The ADC channel to initialize.
    /// * `flags` - Optional configuration flags for the ADC initialization.
    ///
    /// # Returns
    /// A new `Adc` instance ready for conversion operations.
    pub fn new(ch: AdcChannel, flags: Option<u32>) -> Self {
        if let Some(val) = flags {
            ll_invoke_inner!(INVOKE_ID_ADC_INIT, ch, val);
        } else {
            ll_invoke_inner!(INVOKE_ID_ADC_INIT, ch);
        }

        Adc { ch }
    }

    /// Performs a single conversion on the ADC.
    ///
    /// # Returns
    /// The result of the conversion as an `AdcDataType`.
    pub fn single_convert(&self) -> AdcDataType {
        let mut buf = [0; 1];
        let result = ll_invoke_inner!(
            INVOKE_ID_ADC_CTRL,
            self.ch,
            AdcCtrl::Convert,
            buf.as_mut_ptr(),
            1
        );
        if result < 0 {
            return 0;
        }

        buf[0]
    }

    /// Performs multiple conversions on the ADC into the provided buffer.
    ///
    /// # Arguments
    /// * `buf` - The buffer to store the conversion results.
    ///
    /// # Returns
    /// True if the operation was successful, false otherwise.
    pub fn multiple_convert(&self, buf: &mut [AdcDataType]) -> bool {
        let result = ll_invoke_inner!(
            INVOKE_ID_ADC_CTRL,
            AdcCtrl::Convert,
            self.ch,
            buf.as_ptr(),
            buf.len()
        );
        result >= 0
    }
}

impl Drop for Adc {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_ADC_DEINIT, self.ch);
    }
}

pub struct AdcChData(RingBuffer<AdcDataType>);

pub struct AdcBuffered<'a> {
    ch: AdcChannel,
    data: &'a AdcChData,
}

/// Creates a new instance of `AdcBuffered` with the specified channel and optional configuration flags.
///
/// # Arguments
/// * `ch` - The ADC channel to initialize.
/// * `data` - A reference to the ADC channel data.
/// * `flags` - Optional configuration flags for the ADC initialization.
///
/// # Returns
/// A new `AdcBuffered` instance ready for buffered conversion operations.
impl<'a> AdcBuffered<'a> {
    pub fn new(ch: AdcChannel, data: &'a AdcChData, flags: Option<u32>) -> Self {
        if let Some(val) = flags {
            ll_invoke_inner!(INVOKE_ID_ADC_INIT, ch, val);
        } else {
            ll_invoke_inner!(INVOKE_ID_ADC_INIT, ch);
        }

        AdcBuffered { ch, data }
    }

    /// Sets the buffer for storing conversion results.
    ///
    /// # Arguments
    /// * `buffer` - The mutable buffer to be used for storing conversion results.
    pub fn set_buf(&self, buffer: &mut [u16]) {
        let len = buffer.len();
        unsafe { self.data.0.init(buffer.as_mut_ptr(), len) };
    }

    /// Starts the ADC conversion process.
    pub fn start(&self) {
        ll_invoke_inner!(INVOKE_ID_ADC_CTRL, self.ch, AdcCtrl::Start);
    }

    /// Stops the ADC conversion process.
    pub fn stop(&self) {
        ll_invoke_inner!(INVOKE_ID_ADC_CTRL, self.ch, AdcCtrl::Stop);
    }

    /// Reads a single conversion result from the ADC data buffer.
    ///
    /// # Returns
    /// An `Option<AdcDataType>` containing the conversion result if available, or `None` otherwise.
    pub fn read(&self) -> Option<AdcDataType> {
        let mut result = None;
        critical_section::with(|_| result = unsafe { self.data.0.reader().pop_one() });
        return result;
    }

    /// Reads multiple conversion results from the ADC data buffer into the provided buffer.
    ///
    /// # Arguments
    /// * `buf` - The buffer to store the conversion results.
    ///
    /// # Returns
    /// The number of conversion results read.
    pub fn read_multiple(&self, buf: &mut [AdcDataType]) -> usize {
        let mut result = 0;
        critical_section::with(|_| {
            let mut reader = unsafe { self.data.0.reader() };
            for b in buf {
                if let Some(data) = reader.pop_one() {
                    *b = data;
                    result += 1;
                }
            }
        });

        return result;
    }
}

impl<'a> Drop for AdcBuffered<'a> {
    fn drop(&mut self) {
        ll_invoke_inner!(INVOKE_ID_ADC_DEINIT, self.ch);
    }
}

#[cfg(feature = "_adc-buffered")]
macro_rules! impl_adc_ch_data {
    ($adc_ch:expr) => {
        paste! {
            pub static [<ADC_CH $adc_ch _DATA>]: AdcChData = AdcChData ( RingBuffer::new() );
        }

        paste! {
            #[allow(non_snake_case)]
            #[no_mangle]
            #[inline]
            unsafe extern "C" fn [<ADC_CH $adc_ch _EOC_hook_rs>] (val: AdcDataType) {//fn: ADC_CH{ch}_EOC_hook_rs(val)
                paste! {
                    let ch_data = &[<ADC_CH $adc_ch _DATA>].0;
                    if ch_data.is_full() {
                        ch_data.reader().pop_one();
                    }
                    ch_data.writer().push_one(val);
                }
            }
        }
    };
}

#[cfg(feature = "adc-buffered-ch0")]
impl_adc_ch_data!(0);

#[cfg(feature = "adc-buffered-ch1")]
impl_adc_ch_data!(1);

#[cfg(feature = "adc-buffered-ch2")]
impl_adc_ch_data!(2);

#[cfg(feature = "adc-buffered-ch3")]
impl_adc_ch_data!(3);

#[cfg(feature = "adc-buffered-ch4")]
impl_adc_ch_data!(4);

#[cfg(feature = "adc-buffered-ch5")]
impl_adc_ch_data!(5);

#[cfg(feature = "adc-buffered-ch6")]
impl_adc_ch_data!(6);

#[cfg(feature = "adc-buffered-ch7")]
impl_adc_ch_data!(7);
