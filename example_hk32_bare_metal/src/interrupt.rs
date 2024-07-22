extern "C" {
    pub fn sys_tick_inc();
}

#[allow(non_snake_case)]
#[no_mangle]
unsafe extern "C" fn SysTick() {
    sys_tick_inc();
}
