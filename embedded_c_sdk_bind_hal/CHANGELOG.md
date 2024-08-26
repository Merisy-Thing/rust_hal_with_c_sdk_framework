# Changelog

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
