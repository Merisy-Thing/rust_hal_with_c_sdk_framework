#![no_main]
#![no_std]

use embedded_c_sdk_bind_hal::{
    self as CSDK_HAL,
    gpio::{AltMode, Alternate, AnyPin, Level, Output},
    spi::{Config, SpiBus, SpiBusId},
    tick::Delay,
};

use embedded_graphics::{
    mono_font::{ascii::FONT_6X10, MonoTextStyle},
    pixelcolor::{raw::LittleEndian, Rgb565},
    prelude::*,
    primitives::{Circle, PrimitiveStyle, Rectangle},
    text::{Alignment, Text},
};

use ll_bind_ch32v20x as _;
use panic_halt as _;
use st7735_lcd::Orientation;
const IMAGE_WIDTH: u16 = 86;
//const IMAGE_HEIGHT: u16 = 64;

#[riscv_rt_macros::entry]
fn main() -> ! {
    let p = CSDK_HAL::init();
    let mut delay = Delay::new();

    let _sck = Alternate::new(p.PA5.into::<AnyPin>(), AltMode::AFPP);
    let _mosi = Alternate::new(p.PA7.into::<AnyPin>(), AltMode::AFPP);
    //let mut _miso = Pin::new(PortNum::PA, PinNum::Pin6).into_input(PinModeInput::InPullUp);
    let mut nss = Output::new(p.PA2.into::<AnyPin>(), Level::High);
    nss.set_high();

    let dc = Output::new(p.PA0.into::<AnyPin>(), Level::High);
    let rst = Output::new(p.PA1.into::<AnyPin>(), Level::High);

    let spi_cfg = Config::default();
    let spi_dev = SpiBus::new(SpiBusId::Bus1, &spi_cfg).to_device(nss);

    let mut disp = st7735_lcd::ST7735::new(spi_dev, dc, rst, true, false, 128, 64);

    disp.init(&mut delay).unwrap();
    disp.set_orientation(&Orientation::PortraitSwapped).unwrap();
    let ypos = 67;

    let image: embedded_graphics::image::ImageRaw<Rgb565, LittleEndian> =
        embedded_graphics::image::ImageRaw::new(include_bytes!("./ferris.raw"), IMAGE_WIDTH as u32);

    disp.set_offset(2, ypos);
    disp.clear(Rgb565::WHITE).unwrap();

    image.draw(&mut disp).unwrap();
    let character_style = MonoTextStyle::new(&FONT_6X10, Rgb565::GREEN);

    let yoffset = 10;
    let text = "embedded-graphics";
    Text::with_alignment(
        text,
        disp.bounding_box().center() + Point::new(0, 12),
        character_style,
        Alignment::Center,
    )
    .draw(&mut disp)
    .unwrap();

    let fill = PrimitiveStyle::with_fill(Rgb565::RED);
    // Draw a filled square
    Rectangle::new(Point::new(16, 8), Size::new(16, 16))
        .into_styled(fill)
        .draw(&mut disp)
        .unwrap();

    let thick_stroke = PrimitiveStyle::with_stroke(Rgb565::YELLOW, 2);
    // Draw a circle with a 3px wide stroke.
    Circle::new(Point::new(72, yoffset), 17)
        .into_styled(thick_stroke)
        .draw(&mut disp)
        .unwrap();

    loop {}
}
