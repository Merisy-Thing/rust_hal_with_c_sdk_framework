# Rust soft i2c impl
Soft I2C for any open drain pin

## Usage
 - impl OpenDrainPin for SDA pin
 ```
use soft_i2c::{OpenDrainPin, SoftI2C, FREQ_I2C_SCL_100K};

struct SDA(FastPin::<PB, FastPin14, OutputFastPin>);

impl OpenDrainPin for SDA {
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

let mut delay = Delay::new();
let sda = FastPin::<PB, FastPin14, OutputFastPin>::new().into_output(FastPinModeOutput::OutOD);
let scl = FastPin::<PB, FastPin13, OutputFastPin>::new().into_output(FastPinModeOutput::OutOD);
let soft_i2c = SoftI2C::<_, _, _, FREQ_I2C_SCL_100K>::new(scl, SDA(sda), delay);

```

