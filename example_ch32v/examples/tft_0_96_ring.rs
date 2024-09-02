#![no_main]
#![no_std]

#[allow(unused_imports)]
#[rustfmt::skip]
use embedded_c_sdk_bind_hal::{
    self, ll_invoke, print, println,
    adc::{self, Adc, AdcBuffered, AdcChannel}, 
    gpio::{ Pin, PinNum, PinModeOutput, PinModeInput, PinModeAlternate, PortNum, ExtiMode, Port, PortModeOutput, }, 
    pwm::{Pwm, PwmChannel, PwmPolarity}, 
    tick::{Tick, Delay},
    usart::{self, Usart},
	spi::{SpiBusId, SpiBus, Config},
};

use embedded_graphics::{
    mono_font::{ascii::FONT_8X13, MonoTextStyle},
    pixelcolor::Bgr565,
    prelude::*,
    primitives::{Arc, PrimitiveStyle},
    text::{Alignment, Text},
};
use embedded_hal::{self, delay::DelayNs, digital::*};
use ll_bind_ch32v20x as _;
use panic_halt as _;
use st7735_lcd::Orientation;
const LCD_OFFSET_X: u16 = 2;
const LCD_OFFSET_Y: u16 = 67;

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut delay = Delay::new();

    let _sck = Pin::new(PortNum::PA, PinNum::Pin5).into_alternate(PinModeAlternate::AFPP);
    let _mosi = Pin::new(PortNum::PA, PinNum::Pin7).into_alternate(PinModeAlternate::AFPP);
    //let mut _miso = Pin::new(PortNum::PA, PinNum::Pin6).into_input(PinModeInput::InPullUp);
    let mut nss = Pin::new(PortNum::PA, PinNum::Pin2).into_output(PinModeOutput::OutPP);
    nss.set_high().ok();

    let dc = Pin::new(PortNum::PA, PinNum::Pin0).into_output(PinModeOutput::OutPP);
    let rst = Pin::new(PortNum::PA, PinNum::Pin1).into_output(PinModeOutput::OutPP);

    let spi_cfg = Config::default();
    let spi_dev = SpiBus::new(SpiBusId::Bus1, &spi_cfg).to_device(nss);

    let mut disp = st7735_lcd::ST7735::new(spi_dev, dc, rst, true, false, 128, 64);

    disp.init(&mut delay).unwrap();
    disp.set_orientation(&Orientation::PortraitSwapped).unwrap();

    disp.set_offset(LCD_OFFSET_X, LCD_OFFSET_Y);
    disp.clear(Bgr565::BLACK).unwrap();

    //let yoffset = 10;

    // let fill = PrimitiveStyle::with_fill(Bgr565::RED);
    // // Draw a filled square
    // Rectangle::new(Point::new(1, 1), Size::new(64, 32))
    //     .into_styled(fill)
    //     .draw(&mut disp).unwrap();

    // let thick_stroke = PrimitiveStyle::with_stroke(Bgr565::GREEN, 2);
    // // Draw a circle with a 3px wide stroke.
    // Circle::new(Point::new(72, yoffset), 17)
    //     .into_styled(thick_stroke)
    //     .draw(&mut disp).unwrap();

    let character_style = MonoTextStyle::new(&FONT_8X13, Bgr565::WHITE);
    let text = "Hello\nWorld";
    Text::with_alignment(
        text,
        disp.bounding_box().center(),
        character_style,
        Alignment::Center,
    )
    .draw(&mut disp)
    .unwrap();

    const ANGLE_SWEEP: f32 = 10.0;
    const ANGLE_STEP: f32 = 10.0;

    let short_arc_style = PrimitiveStyle::with_stroke(Bgr565::RED, 4);
    let mut short_arc = Arc::new(
        Point::new(36, 4),
        56,
        Angle::from_degrees(90.0),
        Angle::from_degrees(ANGLE_SWEEP),
    );

    let long_arc_style = PrimitiveStyle::with_stroke(Bgr565::GREEN, 4);
    let mut long_arc = Arc::new(
        Point::new(36, 4),
        56,
        Angle::from_degrees(0.0),
        Angle::from_degrees(360.0), //(360.0 - ANGLE_SWEEP)
    );
    long_arc
        .into_styled(long_arc_style)
        .draw(&mut disp)
        .unwrap();
    long_arc.angle_sweep = Angle::from_degrees(ANGLE_SWEEP);

    let mut delay_1 = Delay::new();

    loop {
        short_arc.angle_start += Angle::from_degrees(ANGLE_STEP);
        long_arc.angle_start += Angle::from_degrees(ANGLE_STEP);

        let start = Tick::now();
        short_arc
            .into_styled(short_arc_style)
            .draw(&mut disp)
            .unwrap();
        long_arc
            .into_styled(long_arc_style)
            .draw(&mut disp)
            .unwrap();

        println!("time:{}us", start.elapsed_time().to_micros());

        delay_1.delay_ms(50);
    }
}
