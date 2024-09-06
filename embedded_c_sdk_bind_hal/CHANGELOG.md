# Changelog

## 0.7.0 - 2024-09-06

 - add dma mod and example
 - reserve %Y format flag

## 0.6.0 - 2024-09-02

 - add i2c mod and ll_api define
 - impl embedded_hal InputPin trait for OutputPin
 - separate soft-i2c to stand-alone crate
 - print-log-csdk now support print {:?} Debug format
 - make Delay::new() const

## 0.5.0 - 2024-08-31

- add fastpin to speedup pin operation
- add softi2c module to simulate i2c bus
- SpiDevice nss change to <NSS: OutputPin> generic
- add pub fn tick() -> TickType for Tick

## 0.4.0 - 2024-08-26

- replace print-log-ufmt featue by print-log-csdk
- use `embedded-c-sdk-bind-print-macros` crate print/println macro to format print

## 0.3.4 - 2024-08-22

- add print-log-ufmt featue to optimize fmt code size

## 0.3.3 - 2024-08-20

- impl embedded_hal::digital::InputPin for Pin<Alternate>
- fix pwm set/get period fn

## 0.3.2 - 2024-08-19

- make fn Pin::new() const
