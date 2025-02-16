# LocalStatic

`LocalStatic` is a interior mutable library that provides a way to store variables in static storage, allowing initialization at compile time and being unsafe-friendly for embedded programs.

Using `LocalStatic` in a multi-threaded or multi-core environment is extremely dangerous. Please use other thread-safe libraries such as [lazy_static](https://docs.rs/lazy_static/latest/lazy_static/), [once_cell](https://docs.rs/once_cell/latest/once_cell/), etc.

## Usage

```rust
use local_static::LocalStatic;

{
	static TICK: LocalStatic<Tick> = LocalStatic::new();

	let tick = TICK.get_mut();
	if tick.elapsed_time().to_millis() >= 500 {
		*tick = Tick::now();

		//do something
	}
}
```