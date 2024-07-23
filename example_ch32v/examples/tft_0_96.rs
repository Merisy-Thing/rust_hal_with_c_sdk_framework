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

use embedded_hal::{self, digital::*};
use ll_bind_ch32v20x as _;
use panic_halt as _;
use embedded_graphics::{
    mono_font::{ascii::FONT_6X10, MonoTextStyle}, pixelcolor::{raw::LittleEndian, Bgr565}, prelude::*, primitives::{Circle, PrimitiveStyle, Rectangle}, text::{Alignment, Text}
};
use st7735_lcd::Orientation;
const IMAGE_WIDTH: u16 = 86;
//const IMAGE_HEIGHT: u16 = 64;

#[riscv_rt_macros::entry]
fn main() -> ! {
    let mut delay = Delay::<1000>::new();

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
    let ypos = 67;

    let image: embedded_graphics::image::ImageRaw<Bgr565, LittleEndian> =
        embedded_graphics::image::ImageRaw::new(
            include_bytes!("./ferris.raw"),
            IMAGE_WIDTH as u32,
        );

    disp.set_offset(2, ypos);
    disp.clear(Bgr565::WHITE).unwrap();

    image.draw(&mut disp).unwrap();
    let character_style = MonoTextStyle::new(&FONT_6X10, Bgr565::GREEN);
    
    let yoffset = 10;
    let text = "embedded-graphics";
    Text::with_alignment(
        text,
        disp.bounding_box().center() + Point::new(0, 12),
        character_style,
        Alignment::Center,
    ).draw(&mut disp).unwrap();

    let fill = PrimitiveStyle::with_fill(Bgr565::RED);
    // Draw a filled square
    Rectangle::new(Point::new(16, 8), Size::new(16, 16))
        .into_styled(fill)
        .draw(&mut disp).unwrap();

    let thick_stroke = PrimitiveStyle::with_stroke(Bgr565::YELLOW, 2);
    // Draw a circle with a 3px wide stroke.
    Circle::new(Point::new(72, yoffset), 17)
        .into_styled(thick_stroke)
        .draw(&mut disp).unwrap();

    loop {
    }
}
