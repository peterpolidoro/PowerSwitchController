// ----------------------------------------------------------------------------
// TEENSY41.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "TEENSY41.h"


#if defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41)
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
