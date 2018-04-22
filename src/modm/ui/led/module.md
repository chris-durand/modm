# LED User Interface Abstractions

This module provides abstractions for animating LEDs by wrapping the 
[modm:ui:animation][] module and providing look-up tables for performing
gamma correction of LED brightness.

The main functionality is part of the [`modm::ui::Led`][] class, which provides
a basic interface to fade an LED with an 8-bit value.
Note that this class does *not* do any gamma correction on it's own, it just
wraps an 8-bit [`modm::ui::Animation`][] and a 8-bit value.

Depending on how smooth you require your animation to be, you may call the
[`modm::ui::Led::update()`][] function regularly, at 1ms or less intervals, but 
at most in 255ms intervals.
You must provide a handler which gets called whenever the LED value needs
updating, at most every 1ms, but only when the value has actually changed.
You need to map the 8-bit value to whatever hardware is actually driving the 
LED PWM generator.

```cpp
#include <modm/ui/led.hpp>

modm::ui::Led led([](uint8_t brightness)
{
	// set LED brightness value here
});

main() 
{
	led.on();
	while(1) {
		led.update();
	}
}
```

## Using Gamma Correction LUTs

In order to

```xml

``` 

```cpp
#include <modm/ui/led.hpp>

modm::ui::Led led1([](uint8_t brightness)
{
	Timer4::setCompareValue(1, modm::ui::table22_16_256[brightness]);
});