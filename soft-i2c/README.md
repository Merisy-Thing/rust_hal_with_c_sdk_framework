# Rust soft i2c impl
Soft I2C for any open drain pin

## Usage
 - impl OpenDrainPin for SDA pin
 ```
use soft_i2c::{OpenDrainPin, SoftI2C, FREQ_I2C_SCL_100K};

struct SDA<'a>(&'a mut FastPin<PB, FastPin14, OutputFastPin>);

impl<'a> OpenDrainPin for SDA<'a> {
    fn set(&mut self, level: bool) {
        if level {
            self.0.output_high();
        } else {
            self.0.output_low();
        }
    }

    fn get(&mut self) -> bool {
        self.0.is_input_high()
    }
}

...

let mut sda_pin = FastPin::<PB, FastPin14, OutputFastPin>::new().into_output(FastPinModeOutput::OutOD);
let mut sda = SDA(&mut sda_pin);
let mut scl = FastPin::<PB, FastPin13, OutputFastPin>::new().into_output(FastPinModeOutput::OutOD);
let mut i2c_delay = Delay::new();
let soft_i2c = SoftI2C::<_, _, _, FREQ_I2C_SCL_100K>::new(&mut scl, &mut sda, &mut i2c_delay);

```

