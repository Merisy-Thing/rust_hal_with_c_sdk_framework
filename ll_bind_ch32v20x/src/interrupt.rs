use critical_section::{set_impl, Impl, RawRestoreState};

struct CriticalSection;
set_impl!(CriticalSection);

unsafe impl Impl for CriticalSection {
    unsafe fn acquire() -> RawRestoreState {
        let mut gintenr: usize;
        core::arch::asm!("csrrci {}, 0x800, 0b1000", out(reg) gintenr);

        gintenr as u8 & 0x08
    }

    unsafe fn release(last_state: RawRestoreState) {
        if last_state > 0 {
            core::arch::asm!("csrsi 0x800, 0b1000");
        }
    }
}

