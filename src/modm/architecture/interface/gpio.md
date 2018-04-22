# GPIO Interface

These classes are used to create architecture independent
definitions for hardware pins which then can be used as template
parameters for miscellaneous device drivers.

Example of a platform-independent blinking light:

```cpp
#include <modm/platform/platform.hpp>

using Led = GpioOutputB0;

int main()
{
    Led::setOutput();
    Led::set();

    while (1)
    {
        Led::toggle();
        modm::delayMilliseconds(500);
    }
}
```

You can use the common definitions to express your intention for setting a pin.

```cpp
Led::set(modm::Gpio::High); // Turns Led on
Led::set(modm::Gpio::Low); // Turns Led off
```

!!! warning "Initialize your GPIOs"
    Call `Gpio::setInput()` or `Gpio::setOutput()` before the first use, 
    otherwise the GPIO state may be undefined!
