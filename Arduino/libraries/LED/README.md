# LED

A simple LED class for hardware debugging.

## Defines

`DEFAULT_PIN` - builtin LED pin (13)

## Functions

`LED()` - default constructor, sets LED pin to `DEFAULT_PIN`

`LED(pin)` - secondary constructor, sets LED pin to `pin`

`setPin(pin)` - sets LED pin to `pin`

`on()` - turns LED on

`off()` - turns LED off

`toggle()` - toggles LED state

`toggle(rate)` - toggles LED state if last call to this function was greater than `rate` milliseconds ago
