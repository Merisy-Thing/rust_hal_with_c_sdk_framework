#![no_std]
#![doc = core::include_str!("../README.md")]
#![cfg_attr(docsrs, feature(doc_cfg))]

use core::cell::UnsafeCell;
use core::mem::MaybeUninit;
use core::marker::{Send, Sync};

/// A struct for storing local variables in static storage.
///
/// `LocalStatic` provides a way to store local variables in static storage,
/// allowing them to be initialized and accessed at runtime.
/// This type is useful when you need to store mutable state in a static context,
/// such as in embedded systems.
pub struct LocalStatic<T> {
    val: UnsafeCell<MaybeUninit<T>>,
}

unsafe impl<T> Send for LocalStatic<T> {}
unsafe impl<T> Sync for LocalStatic<T> {}

impl<T> LocalStatic<T> {
    /// Creates a new `LocalStatic` instance, with the memory being filled with `0` bytes.
    ///
    /// # Returns
    ///
    /// A new `LocalStatic` instance containing an uninitialized value of type `T`.
    #[inline]
    pub const fn new() -> Self {
        Self {
            val: UnsafeCell::new(MaybeUninit::zeroed()),
        }
    }

    /// Creates a new `LocalStatic` instance, initialized with the given value.
    ///
    /// # Parameters
    ///
    /// - `val`: The value to initialize the `LocalStatic` with.
    ///
    /// # Returns
    ///
    /// A new `LocalStatic` instance containing the given `val`.
    #[inline]
    pub const fn with(val: T) -> Self {
        Self {
            val: UnsafeCell::new(MaybeUninit::new(val)),
        }
    }

    /// Gets an immutable reference to the value stored in `LocalStatic`.
    ///
    /// # Returns
    ///
    /// An immutable reference to the value stored in `LocalStatic`.
    pub fn get(&'static self) -> &'static T {
        unsafe { (&mut *self.val.get()).assume_init_ref() }
    }

    /// Gets a mutable reference to the value stored in `LocalStatic`.
    ///
    /// # Returns
    ///
    /// A mutable reference to the value stored in `LocalStatic`.
    pub fn get_mut(&'static self) -> &'static mut T {
        unsafe { (&mut *self.val.get()).assume_init_mut() }
    }

    /// Sets the value stored in `LocalStatic` to the given value.
    ///
    /// # Parameters
    /// - `val`: The value to set the `LocalStatic` to.
    pub fn set(&'static self, val: T) {
        let r = unsafe { &mut *self.val.get() };

        r.write(val);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new() {
        static LOCAL_STATIC:LocalStatic::<i32> = LocalStatic::<i32>::new();
        assert_eq!(*LOCAL_STATIC.get(), 0);
    }

    #[test]
    fn test_with() {
        static LOCAL_STATIC:LocalStatic::<i32> = LocalStatic::<i32>::with(42);
        assert_eq!(*LOCAL_STATIC.get(), 42);
    }

    #[test]
    fn test_get() {
        static LOCAL_STATIC:LocalStatic::<i32> = LocalStatic::<i32>::with(42);
        let value = LOCAL_STATIC.get();
        assert_eq!(*value, 42);
    }

    #[test]
    fn test_get_mut() {
        static LOCAL_STATIC:LocalStatic::<i32> = LocalStatic::<i32>::with(42);
        let mut_value = LOCAL_STATIC.get_mut();
        *mut_value = 100;
        assert_eq!(*LOCAL_STATIC.get(), 100);
    }

    #[test]
    fn test_set() {
        static LOCAL_STATIC:LocalStatic::<i32> = LocalStatic::<i32>::new();
        LOCAL_STATIC.set(100);
        assert_eq!(*LOCAL_STATIC.get(), 100);
    }
}
