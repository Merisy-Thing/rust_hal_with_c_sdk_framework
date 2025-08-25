#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use cortex_m_rt::pre_init;

pub mod rt;

include!("../bind/bind_ll_drv.rs");

#[pre_init]
unsafe fn pre_init() {
	ll_invoke(INVOKE_ID_SYSTEM_INIT);
}
