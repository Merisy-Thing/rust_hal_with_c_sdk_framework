#[allow(non_snake_case)]
#[no_mangle]
unsafe extern "C" fn SysTick() {
    embedded_c_sdk_bind_hal::sys_tick_handler!();
}
