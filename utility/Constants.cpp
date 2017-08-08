// ----------------------------------------------------------------------------
// Constants.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Constants.h"


namespace power_switch_controller
{
namespace constants
{
CONSTANT_STRING(device_name,"power_switch_controller");

CONSTANT_STRING(firmware_name,"PowerSwitchController");
// Use semantic versioning http://semver.org/
const modular_server::FirmwareInfo firmware_info =
  {
    .name_ptr=&firmware_name,
    .version_major=2,
    .version_minor=1,
    .version_patch=0,
  };

CONSTANT_STRING(hardware_name,"power_switch_controller");

const int bad_index = -1;

// 15us ON + 15us OFF (1/30us) = 33333Hz = 333Hz @ 100 resolution
const size_t power_pwm_frequency = 333;
const size_t power_pwm_resolution = 8;
const size_t power_pwm_value_min = 0;
const size_t power_pwm_value_max = 255;

// Interrupts

// Units
CONSTANT_STRING(ms_units,"ms");
CONSTANT_STRING(percent_units,"%");

// Properties
CONSTANT_STRING(states_property_name,"states");
const long states_array_default[STATE_COUNT] = {0};

// Parameters
CONSTANT_STRING(channel_parameter_name,"channel");

CONSTANT_STRING(channels_parameter_name,"channels");

CONSTANT_STRING(state_parameter_name,"state");

CONSTANT_STRING(delay_parameter_name,"delay");
const long delay_min = 0;
const long delay_max = 2000000000;

CONSTANT_STRING(period_parameter_name,"period");
const long period_min = 2;
const long period_max = 2000000000;

CONSTANT_STRING(on_duration_parameter_name,"on_duration");
const long on_duration_min = 1;
const long on_duration_max = 2000000000;

CONSTANT_STRING(count_parameter_name,"count");
const long count_min = 1;
const long count_max = 2000000000;

CONSTANT_STRING(pwm_index_parameter_name,"pwm_index");

CONSTANT_STRING(channel_group_parameter_name,"channel_group");
const long channel_group_min = 0;

CONSTANT_STRING(power_parameter_name,"power");
const long power_min = 0;
const long power_max = 100;

// Functions
CONSTANT_STRING(set_channel_on_function_name,"setChannelOn");
CONSTANT_STRING(set_channel_off_function_name,"setChannelOff");
CONSTANT_STRING(set_channels_on_function_name,"setChannelsOn");
CONSTANT_STRING(set_channels_off_function_name,"setChannelsOff");
CONSTANT_STRING(toggle_channel_function_name,"toggleChannel");
CONSTANT_STRING(toggle_channels_function_name,"toggleChannels");
CONSTANT_STRING(toggle_all_channels_function_name,"toggleAllChannels");
CONSTANT_STRING(set_all_channels_on_function_name,"setAllChannelsOn");
CONSTANT_STRING(set_all_channels_off_function_name,"setAllChannelsOff");
CONSTANT_STRING(set_channel_on_all_others_off_function_name,"setChannelOnAllOthersOff");
CONSTANT_STRING(set_channel_off_all_others_on_function_name,"setChannelOffAllOthersOn");
CONSTANT_STRING(set_channels_on_all_others_off_function_name,"setChannelsOnAllOthersOff");
CONSTANT_STRING(set_channels_off_all_others_on_function_name,"setChannelsOffAllOthersOn");
CONSTANT_STRING(get_channels_on_function_name,"getChannelsOn");
CONSTANT_STRING(get_channels_off_function_name,"getChannelsOff");
CONSTANT_STRING(get_channel_count_function_name,"getChannelCount");
CONSTANT_STRING(save_state_function_name,"saveState");
CONSTANT_STRING(recall_state_function_name,"recallState");
CONSTANT_STRING(add_pwm_function_name,"addPwm");
CONSTANT_STRING(start_pwm_function_name,"startPwm");
CONSTANT_STRING(stop_pwm_function_name,"stopPwm");
CONSTANT_STRING(stop_all_pwm_function_name,"stopAllPwm");
CONSTANT_STRING(set_power_function_name,"setPower");
CONSTANT_STRING(get_powers_function_name,"getPowers");

// Callbacks

// Errors
CONSTANT_STRING(pwm_error,"Maximum number of pwm already set, must stop one to add another.");
}
}
