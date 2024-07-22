#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

mod interrupt;
pub mod rt;

include!("../bind/bind_ll_drv.rs");

#[export_name = "_start_rust"]
pub unsafe extern "C" fn start_rust() -> ! {
    #[rustfmt::skip]
    extern "Rust" {
        // This symbol will be provided by the user via `#[entry]`
        fn main() -> !;
    }

    ll_invoke(INVOKE_ID_LL_DRV_INIT);
    //ll_invoke(INVOKE_ID_LOG_PUTS, "start main\r\n", 12);
    main();
}

