// ----------------------------------------------------------------------------
// 5x3.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "5x3.h"


#if defined(__MK64FX512__)

namespace power_switch_controller
{
namespace constants
{
const modular_server::HardwareInfo hardware_info =
{
  .name_ptr=&hardware_name,
  .part_number=1110,
  .version_major=1,
  .version_minor=6,
};

const size_t cs_pin = 46;
const size_t reset_pin = 44;
const size_t in_pin = 45;

const size_t map_pins[IC_COUNT] =
{
  6,
  7,
  8,
  9,
};

// Pins

// Units

// Properties

// Parameters

// Functions

// Callbacks

// Errors
}
}

#elif defined(__AVR_ATmega2560__)

namespace power_switch_controller
{
namespace constants
{
const modular_server::HardwareInfo hardware_info =
{
  .name_ptr=&hardware_name,
  .part_number=1110,
  .version_major=1,
  .version_minor=6,
};

const size_t cs_pin = 46;
const size_t reset_pin = 44;
const size_t in_pin = 45;

const size_t map_pins[IC_COUNT] =
{
  6,
  7,
  8,
  9,
};

// Pins

// Units

// Properties

// Parameters

// Functions

// Callbacks

// Errors
}
}
#endif
