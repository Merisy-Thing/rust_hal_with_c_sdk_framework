//
//The original code is from format_n_osd crate: https://crates.io/crates/format_no_std
//

use core::cmp::min;
use core::fmt;
use core::str::from_utf8;

/// A struct representing a writer that appends formatted data to a byte buffer.
pub struct WriteTo<'a> {
    buf: &'a mut [u8],
    len: usize,
}

impl<'a> WriteTo<'a> {
    /// Constructs a new `WriteTo` instance wrapping the provided byte buffer.
    pub fn new(buf: &'a mut [u8]) -> Self {
        WriteTo { buf, len: 0 }
    }

    /// Converts the written portion of the buffer into a string slice, if possible.
    pub fn as_str(self) -> Option<&'a str> {
        let min_len;
        if self.len > self.buf.len() {
            min_len = self.buf.len();
            let tail = &mut self.buf[min_len - 3..];
            tail.fill(b'.');
        } else {
            min_len = min(self.len, self.buf.len())
        };
        from_utf8(&self.buf[..min_len]).ok()
    }

    /// Get the number of bytes written to buffer.
    pub fn len(&self) -> usize {
        min(self.len, self.buf.len())
    }

    /// Returns true if self has a length of zero bytes
    pub fn is_empty(&self) -> bool {
        self.len == 0
    }
}

impl<'a> fmt::Write for WriteTo<'a> {
    /// Writes a string slice into the buffer, updating the length accordingly.
    fn write_str(&mut self, s: &str) -> fmt::Result {
        if self.len >= self.buf.len() {
            return Err(fmt::Error);
        }

        let rem = &mut self.buf[self.len..];
        let raw_s = s.as_bytes();
        let num = min(raw_s.len(), rem.len());

        rem[..num].copy_from_slice(&raw_s[..num]);
        self.len += raw_s.len();

        Ok(())
    }
}

/// Formats data using `format_args!` (`arg` argument) and writes it to a byte buffer `buf`.
pub fn fmt_to_buf<'a>(buf: &'a mut [u8], arg: fmt::Arguments) -> Result<&'a str, fmt::Error> {
    let mut w = WriteTo::new(buf);
    if let Err(e) = fmt::write(&mut w, arg) {
        if w.len() == 0 {
            return Err(e);
        }
    }
    w.as_str().ok_or(fmt::Error)
}
