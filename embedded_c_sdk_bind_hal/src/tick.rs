use crate::ll_api::ll_cmd::*;
use embedded_hal::delay::DelayNs;
use fugit::{Duration, TimerDurationU32};
use portable_atomic::{AtomicU32, Ordering};

pub const TICK_FREQ_HZ: u32 = crate::tick_freq_hz::TICK_FREQ_HZ;

#[cfg(not(feature = "tick-size-64bit"))]
pub type TickType = u32;
#[cfg(feature = "tick-size-64bit")]
pub type TickType = u64;

static SYS_TICK_0: AtomicU32 = AtomicU32::new(0);
#[cfg(feature = "tick-size-64bit")]
static SYS_TICK_1: AtomicU32 = AtomicU32::new(0);

#[no_mangle]
#[inline]
/// An unsafe external C function that increments the system tick counter.
///
/// This function is called by a hardware interrupt to update the system's tick count.
/// Depending on the build features, it supports either 32-bit or 64-bit tick counters.
///
unsafe extern "C" fn sys_tick_inc() {
    // Increment the low-order 32-bit tick counter atomically.
    #[cfg(any(feature = "tick-size-64bit", feature = "embassy"))]
    let sys_tick = SYS_TICK_0.fetch_add(1, Ordering::Relaxed);

    // Increment the low-order 32-bit tick counter using a method compatible with non-64bit environments.
    #[cfg(not(any(feature = "tick-size-64bit", feature = "embassy")))]
    SYS_TICK_0.add(1, Ordering::Relaxed);

    // Handle 64-bit tick overflow and update the high-order 32-bit tick counter.
    #[cfg(feature = "tick-size-64bit")]
    let sys_tick = if sys_tick == u32::MAX {
        let tick_1 = SYS_TICK_1.fetch_add(1, Ordering::Release);
        ((tick_1 as u64) << 32) | (sys_tick as u64)
    } else {
        let tick_1 = SYS_TICK_1.load(Ordering::Relaxed);
        ((tick_1 as u64) << 32) | (sys_tick as u64)
    };

    // If the 'embassy' feature is enabled, check for any alarms that need to be triggered.
    #[cfg(feature = "embassy")]
    tick_time_driver::check_alarm(sys_tick);
}

/// Represents a point in time as measured by the system tick counter.
///
/// This struct is used to keep track of time in terms of ticks, which can be compared or used to measure elapsed time.
#[derive(Copy, Clone, Debug, Eq, PartialEq, Ord, PartialOrd, Default)]
pub struct Tick(TickType);

impl Tick {
    /// Returns the current system tick value.
    ///
    /// # Examples
    /// ```
    /// let current_tick = Tick::now();
    /// ```
    pub fn now() -> Self {
        // For 64-bit ticks, ensure we get a consistent snapshot of both high and low parts.
        #[cfg(feature = "tick-size-64bit")]
        loop {
            let t0 = SYS_TICK_1.load(Ordering::SeqCst);
            let t = SYS_TICK_0.load(Ordering::SeqCst);
            let t1 = SYS_TICK_1.load(Ordering::SeqCst);
            if t0 == t1 {
                break Tick(((t0 as u64) << 32) | (t as u64));
            }
        }
        // For 32-bit ticks, simply load the tick counter.
        #[cfg(not(feature = "tick-size-64bit"))]
        Tick(SYS_TICK_0.load(Ordering::Relaxed))
    }

    /// Computes the number of ticks elapsed since the creation of this `Tick` instance.
    ///
    /// # Examples
    /// ```
    /// let start_tick = Tick::now();
    /// let elapsed_ticks = start_tick.elapsed();
    /// ```
    pub fn elapsed(self) -> TickType {
        if let Some(tick) = Self::now().0.checked_sub(self.0) {
            tick
        } else {
            TickType::MAX
        }
    }

    /// Creates a new `Tick` instance with the given value.
    ///
    /// # Arguments
    /// * `value` - The value to initialize the `Tick` with.
    ///
    /// # Returns
    /// * A new `Tick` instance.
    #[inline]
    pub const fn with_value(value: TickType) -> Self {
        Tick(value)
    }

    /// Retrieves the current tick value.
    ///
    /// # Returns
    /// * The current tick value.
    #[inline]
    pub fn tick() -> TickType {
        Self::now().0
    }

    /// Calculates the elapsed time since the creation of the `Tick` instance.
    ///
    /// # Returns
    /// * A `Duration` representing the elapsed time in ticks.
    pub fn elapsed_time(self) -> Duration<TickType, 1, TICK_FREQ_HZ> {
        let tick = if let Some(res) = Self::now().0.checked_sub(self.0) {
            res
        } else {
            TickType::MAX
        };

        Duration::<TickType, 1, TICK_FREQ_HZ>::from_ticks(tick)
    }
}

impl core::ops::Add for Tick {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        Tick(self.0 + rhs.0)
    }
}

impl core::ops::Sub for Tick {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self::Output {
        Tick(self.0 - rhs.0)
    }
}

/// A delay utility that allows for precise timing delays based on a specified tick frequency.
///
/// This struct provides methods to create a delay object and then use it to delay for a specific amount of time.
/// The delay is implemented using the system's tick counter and can be configured for different frequencies.
///
/// # Examples
/// ```
/// let mut delay = Delay::new(); // Create a delay object for a 1000 Hz tick frequency.
/// delay.delay_ms(500); // Delay for 500 milliseconds.
/// ```
#[derive(Debug, Clone, Copy)]
pub struct Delay;

impl Delay {
    /// Creates a new `Delay` instance for the specified tick frequency.
    ///
    /// # Examples
    /// ```
    /// let mut delay = Delay::new(); // Create a delay object for a 1000 Hz tick frequency.
    /// ```
    pub const fn new() -> Self {
        Delay
    }
}

/// Trait implementation for delaying in nanoseconds.
impl DelayNs for Delay {
    /// Delays for a specified number of nanoseconds.
    ///
    /// # Examples
    /// ```
    /// let mut delay = Delay::new();
    /// delay.delay_ns(1_000_000); // Delay for 1 millisecond (1,000,000 nanoseconds).
    /// ```
    #[inline]
    fn delay_ns(&mut self, ns: u32) {
        ll_invoke_inner!(INVOKE_ID_DELAY_NANO, ns);
    }

    /// Delays for a specified number of milliseconds.
    ///
    /// # Examples
    /// ```
    /// let mut delay = Delay::new();
    /// delay.delay_ms(500); // Delay for 500 milliseconds.
    /// ```
    #[inline]
    fn delay_ms(&mut self, ms: u32) {
        let ms_tick = TimerDurationU32::<TICK_FREQ_HZ>::millis(ms).ticks();
        let start = Tick::now();
        loop {
            unsafe {
                core::arch::asm!("wfi");
            }
            if (start.elapsed() as u32) >= ms_tick {
                break;
            }
        }
    }
}

#[cfg(feature = "embassy")]
mod tick_time_driver {
    use core::cell::Cell;
    use critical_section::Mutex;
    use embassy_time_driver::{AlarmHandle, Driver};
    use portable_atomic::{AtomicBool, Ordering};

    struct AlarmState {
        timestamp: Cell<super::TickType>,
        callback: Cell<Option<(fn(*mut ()), *mut ())>>,
    }
    unsafe impl Send for AlarmState {}

    const DUMMY_ALARM: AlarmState = AlarmState {
        timestamp: Cell::new(0),
        callback: Cell::new(None),
    };

    struct TimerDriver {
        alarms: Mutex<AlarmState>,
        allocated: AtomicBool,
    }

    embassy_time_driver::time_driver_impl!(static DRIVER: TimerDriver = TimerDriver{
        alarms:  Mutex::new(DUMMY_ALARM),
        allocated: AtomicBool::new(false),
    });

    #[inline(always)]
    pub fn check_alarm(curr_tick: super::TickType) {
        DRIVER.check_alarm(curr_tick);
    }

    impl TimerDriver {
        /// Internal function to check and trigger alarms.
        ///
        /// This function enters a critical section to safely access shared resources and checks if there is an alarm
        /// set to go off at the current tick. If an alarm is due, it resets the alarm timestamp and invokes the callback.
        fn check_alarm(&self, curr_tick: super::TickType) {
            critical_section::with(|cs| {
                let allocated = self.allocated.load(Ordering::Relaxed);
                if !allocated {
                    return;
                }
                let alarm = &self.alarms.borrow(cs);

                let timestamp = alarm.timestamp.get();

                if timestamp <= curr_tick {
                    alarm.timestamp.set(super::TickType::MAX);

                    if let Some((f, ctx)) = alarm.callback.get() {
                        f(ctx);
                    }
                }
            });
        }
    }

    impl Driver for TimerDriver {
        fn now(&self) -> u64 {
            super::Tick::now().0 as u64
        }

        /// Allocates an alarm resource.
        ///
        /// This function checks if an alarm is already allocated. If not, it allocates one and returns an `AlarmHandle`.
        /// Note: This function is marked as unsafe because it may lead to undefined behavior if called incorrectly.
        unsafe fn allocate_alarm(&self) -> Option<AlarmHandle> {
            let allocated = self.allocated.load(Ordering::Relaxed);
            if allocated {
                return None;
            }

            self.allocated.store(true, Ordering::Relaxed);
            Some(AlarmHandle::new(0))
        }

        /// Sets the callback for an alarm.
        ///
        /// This function sets the callback function and context for a given alarm handle.
        fn set_alarm_callback(&self, _alarm: AlarmHandle, callback: fn(*mut ()), ctx: *mut ()) {
            critical_section::with(|cs| {
                let alarm = &self.alarms.borrow(cs);
                alarm.callback.set(Some((callback, ctx)));
            })
        }

        /// Sets an alarm to trigger at a specific timestamp.
        ///
        /// This function sets the alarm to trigger at the provided timestamp. It returns `true` if the alarm was set,
        /// and `false` if the timestamp is in the past.
        fn set_alarm(&self, _alarm: AlarmHandle, timestamp: u64) -> bool {
            critical_section::with(|cs| {
                let alarm = &self.alarms.borrow(cs);

                let now = self.now();
                if timestamp <= now {
                    alarm.timestamp.set(super::TickType::MAX);
                    false
                } else {
                    alarm.timestamp.set(timestamp as super::TickType);
                    true
                }
            })
        }
    }
}
