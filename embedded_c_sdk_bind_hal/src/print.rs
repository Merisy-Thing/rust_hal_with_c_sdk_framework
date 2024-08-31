use crate::ll_api::ll_cmd::*;

#[cfg(all(feature = "print-log", not(feature = "print-log-csdk")))]
#[macro_export]
macro_rules! println {
    ($($arg:tt)*) => {{
        {
            use core::fmt::Write;
            writeln!($crate::print::Printer, $($arg)*).ok();
        }
    }};
}

#[cfg(all(feature = "print-log", not(feature = "print-log-csdk")))]
#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{
        {
            use core::fmt::Write;
            write!($crate::print::Printer, $($arg)*).ok();
        }
    }};
}

#[cfg(not(feature = "print-log"))]
#[macro_export]
macro_rules! println {
    ($($arg:tt)*) => {{}};
}

#[cfg(not(feature = "print-log"))]
#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{}};
}

pub struct Printer;

impl core::fmt::Write for Printer {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        let bytes = s.as_bytes();
        unsafe { ll_invoke(INVOKE_ID_LOG_PUTS, bytes.as_ptr(), bytes.len()) };

        Ok(())
    }
}
