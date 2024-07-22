//GPIO
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum PortNum {
    PA = 0,
    PB = 1,
    PC = 2,
    PD = 3,
    PE = 4,
    PF = 5,
    PG = 6,
    PH = 7,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum PinNum {
    Pin0 = 0,
    Pin1 = 1,
    Pin2 = 2,
    Pin3 = 3,
    Pin4 = 4,
    Pin5 = 5,
    Pin6 = 6,
    Pin7 = 7,
    Pin8 = 8,
    Pin9 = 9,
    Pin10 = 10,
    Pin11 = 11,
    Pin12 = 12,
    Pin13 = 13,
    Pin14 = 14,
    Pin15 = 15,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
pub(crate) enum GpioInitFlag {
    Analog = 0,     // Analog Input
    OutPP = 1,      // Output Push-Pull
    OutOD = 2,      // Output Open-Drain
    InFloating = 4, // Input Floating
    InPU = 8,       // Input Pull-Up
    InPD = 12,      // Input Pull-Down
    AF0 = 16,       // Alternate Function 0
    AF1 = 32,       // Alternate Function 1
    AF2 = 48,       // Alternate Function 2
    AF3 = 64,       // Alternate Function 3
    AF4 = 80,       // Alternate Function 4
    AF5 = 96,       // Alternate Function 5
    AF6 = 112,      // Alternate Function 6
    AF7 = 128,      // Alternate Function 7
    AFOD = 144,     // Alternate Function Open-Drain
    AFPP = 160,     // Alternate Function Push-Pull
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
pub(crate) enum GpioExtiFlag {
    Rising = 1,
    Falling = 2,
    RisingFalling = 3,
    Enable = 4,
    Disable = 5,
}

//SPI BUS
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum SpiBusId {
    Bus0 = 0,
    Bus1 = 1,
    Bus2 = 2,
    Bus3 = 3,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum SpiBusDataSize {
    DataSize8 = 0,  // Data size is 8 bits
    DataSize16 = 1, // Data size is 16 bits
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum SpiBusBitOrder {
    MsbFirst = 0, // Most Significant Bit first
    LsbFirst = 2, // Least Significant Bit first
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum SpiBusMode {
    Mode0 = 0,
    Mode1 = 1,
    Mode2 = 2,
    Mode3 = 3,
}

//USART
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum UsartId {
    USART0,
    USART1,
    USART2,
    USART3,
    USART4,
    USART5,
    USART6,
    USART7,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
/// Number of data bits
pub enum UsartDataBits {
    /// 8 Data Bits
    DataBits8 = 0,
    /// 9 Data Bits
    DataBits9 = 256,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
/// Parity
pub enum UsartParity {
    /// No parity
    ParityNone = 0,
    /// Even Parity
    ParityEven = 4,
    /// Odd Parity
    ParityOdd = 8,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
/// Number of stop bits
pub enum UsartStopBits {
    STOP0P5 = 0,
    STOP1 = 1,
    STOP1P5 = 2,
    STOP2 = 3,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum UsartMode {
    ///Loop back
    LoopBack = 0,
    ///Read only
    RxOnly = 16,
    ///Write only
    TxOnly = 32,
    ///Read/Write
    TxRx = 16 + 32,
}

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u32)]
pub enum UsartHwFlowCtrl {
    None = 0,
    Cts = 128,
    Rts = 64,
    CtsRts = 192,
}

//PWM
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum PwmChannel {
    CH0,
    CH1,
    CH2,
    CH3,
    CH4,
    CH5,
    CH6,
    CH7,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[repr(u32)]
pub enum PwmCtrl {
    On = 0,
    Off = 1,
    SetDuty = 2,
    GetDuty = 3,
    GetMaxDuty = 4,
    SetPeriod = 5,
    GetPeriod = 6,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[repr(u32)]
pub enum PwmPolarity {
    ActiveHigh = 10,
    ActiveLow = 11,
}

//ADC
#[derive(Clone, Copy, PartialEq, Eq, Debug)]
#[repr(u8)]
pub enum AdcChannel {
    CH0,
    CH1,
    CH2,
    CH3,
    CH4,
    CH5,
    CH6,
    CH7,
}
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[repr(u32)]
pub enum AdcCtrl {
    Start = 0,
    Stop = 1,
    Convert = 2,
}

pub mod ll_cmd {
    //INVOKE
    pub type InvokeParam = ::core::ffi::c_uint;
    pub const INVOKE_ID_SYSTEM_INIT: InvokeParam = 100;
    pub const INVOKE_ID_LL_DRV_INIT: InvokeParam = 200;
    pub const INVOKE_ID_DELAY_NANO: InvokeParam = 201;
    pub const INVOKE_ID_LOG_PUTS: InvokeParam = 202;
    pub const INVOKE_ID_GPIO_INIT: InvokeParam = 300;
    pub const INVOKE_ID_GPIO_SET: InvokeParam = 301;
    pub const INVOKE_ID_GPIO_GET_INPUT: InvokeParam = 302;
    pub const INVOKE_ID_GPIO_GET_OUTPUT: InvokeParam = 303;
    pub const INVOKE_ID_GPIO_GET_PORT_REG: InvokeParam = 304;
    pub const INVOKE_ID_GPIO_EXTI: InvokeParam = 305;
    pub const INVOKE_ID_GPIO_CUSTOM_BASE: InvokeParam = 350;
    pub const INVOKE_ID_SPI_INIT: InvokeParam = 400;
    pub const INVOKE_ID_SPI_DEINIT: InvokeParam = 401;
    pub const INVOKE_ID_SPI_BLOCKING_RW: InvokeParam = 402;
    pub const INVOKE_ID_SPI_CUSTOM_BASE: InvokeParam = 450;
    pub const INVOKE_ID_USART_INIT: InvokeParam = 500;
    pub const INVOKE_ID_USART_DEINIT: InvokeParam = 501;
    pub const INVOKE_ID_USART_WRITE: InvokeParam = 502;
    pub const INVOKE_ID_USART_CUSTOM_BASE: InvokeParam = 550;
    pub const INVOKE_ID_PWM_INIT: InvokeParam = 600;
    pub const INVOKE_ID_PWM_DEINIT: InvokeParam = 601;
    pub const INVOKE_ID_PWM_CTRL: InvokeParam = 602;
    pub const INVOKE_ID_PWM_CUSTOM_BASE: InvokeParam = 650;
    pub const INVOKE_ID_ADC_INIT: InvokeParam = 700;
    pub const INVOKE_ID_ADC_DEINIT: InvokeParam = 701;
    pub const INVOKE_ID_ADC_CTRL: InvokeParam = 702;
    pub const INVOKE_ID_ADC_CUSTOM_BASE: InvokeParam = 750;

    //For user custom
    pub const INVOKE_ID_DEV_CUSTOM_BASE: InvokeParam = 10_000;

    extern "C" {
        pub fn ll_invoke(invoke_id: InvokeParam, ...) -> ::core::ffi::c_int;
    }
}
