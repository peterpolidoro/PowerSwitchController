// ----------------------------------------------------------------------------
// PowerSwitchController.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "PowerSwitchController.h"


using namespace power_switch_controller;

PowerSwitchController::PowerSwitchController() :
TLE72X(constants::cs_pin,constants::reset_pin)
{
}

void PowerSwitchController::setup()
{
  // Parent Setup
  ModularDeviceBase::setup();
  TLE72X::setup(constants::IC_COUNT);

  // Reset Watchdog
  resetWatchdog();

  // Event Controller Setup
  event_controller_.setup();

  // Pin Setup
  pinMode(constants::in_pin,OUTPUT);
  digitalWrite(constants::in_pin,HIGH);
  for (size_t i=0; i<constants::IC_COUNT; ++i)
  {
    pinMode(constants::map_pins[i],OUTPUT);
    digitalWrite(constants::map_pins[i],HIGH);
    powers_[i] = constants::power_max;
  }

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware
  modular_server_.addHardware(constants::hardware_info,
    pins_);

  // Pins

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
    properties_,
    parameters_,
    functions_,
    callbacks_);
  // Properties
  modular_server_.createProperty(constants::states_property_name,constants::states_array_default);

  // Parameters
  modular_server::Parameter & channel_parameter = modular_server_.createParameter(constants::channel_parameter_name);
  channel_parameter.setRange(0,constants::CHANNEL_COUNT-1);

  modular_server::Parameter & channels_parameter = modular_server_.createParameter(constants::channels_parameter_name);
  channels_parameter.setRange(0,constants::CHANNEL_COUNT-1);
  channels_parameter.setArrayLengthRange(1,constants::CHANNEL_COUNT);

  modular_server::Parameter & state_parameter = modular_server_.createParameter(constants::state_parameter_name);
  state_parameter.setRange(0,constants::STATE_COUNT-1);

  modular_server::Parameter & delay_parameter = modular_server_.createParameter(constants::delay_parameter_name);
  delay_parameter.setRange(constants::delay_min,constants::delay_max);
  delay_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & period_parameter = modular_server_.createParameter(constants::period_parameter_name);
  period_parameter.setRange(constants::period_min,constants::period_max);
  period_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & on_duration_parameter = modular_server_.createParameter(constants::on_duration_parameter_name);
  on_duration_parameter.setRange(constants::on_duration_min,constants::on_duration_max);
  on_duration_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & count_parameter = modular_server_.createParameter(constants::count_parameter_name);
  count_parameter.setRange(constants::count_min,constants::count_max);
  count_parameter.setUnits(constants::ms_units);

  modular_server::Parameter & pwm_index_parameter = modular_server_.createParameter(constants::pwm_index_parameter_name);
  pwm_index_parameter.setRange(0,constants::INDEXED_PULSES_COUNT_MAX-1);

  modular_server::Parameter & channel_group_parameter = modular_server_.createParameter(constants::channel_group_parameter_name);
  channel_group_parameter.setRange(constants::channel_group_min,(long)constants::IC_COUNT-1);

  modular_server::Parameter & power_parameter = modular_server_.createParameter(constants::power_parameter_name);
  power_parameter.setRange(constants::power_min,constants::power_max);
  power_parameter.setUnits(constants::percent_units);

  // Functions
  modular_server::Function & set_channel_on_function = modular_server_.createFunction(constants::set_channel_on_function_name);
  set_channel_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelOnHandler));
  set_channel_on_function.addParameter(channel_parameter);

  modular_server::Function & set_channel_off_function = modular_server_.createFunction(constants::set_channel_off_function_name);
  set_channel_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelOffHandler));
  set_channel_off_function.addParameter(channel_parameter);

  modular_server::Function & set_channels_on_function = modular_server_.createFunction(constants::set_channels_on_function_name);
  set_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelsOnHandler));
  set_channels_on_function.addParameter(channels_parameter);

  modular_server::Function & set_channels_off_function = modular_server_.createFunction(constants::set_channels_off_function_name);
  set_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelsOffHandler));
  set_channels_off_function.addParameter(channels_parameter);

  modular_server::Function & toggle_channel_function = modular_server_.createFunction(constants::toggle_channel_function_name);
  toggle_channel_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::toggleChannelHandler));
  toggle_channel_function.addParameter(channel_parameter);

  modular_server::Function & toggle_channels_function = modular_server_.createFunction(constants::toggle_channels_function_name);
  toggle_channels_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::toggleChannelsHandler));
  toggle_channels_function.addParameter(channels_parameter);

  modular_server::Function & toggle_all_channels_function = modular_server_.createFunction(constants::toggle_all_channels_function_name);
  toggle_all_channels_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::toggleAllChannelsHandler));

  modular_server::Function & set_all_channels_on_function = modular_server_.createFunction(constants::set_all_channels_on_function_name);
  set_all_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setAllChannelsOnHandler));

  modular_server::Function & set_all_channels_off_function = modular_server_.createFunction(constants::set_all_channels_off_function_name);
  set_all_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setAllChannelsOffHandler));

  modular_server::Function & set_channel_on_all_others_off_function = modular_server_.createFunction(constants::set_channel_on_all_others_off_function_name);
  set_channel_on_all_others_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelOnAllOthersOffHandler));
  set_channel_on_all_others_off_function.addParameter(channel_parameter);

  modular_server::Function & set_channel_off_all_others_on_function = modular_server_.createFunction(constants::set_channel_off_all_others_on_function_name);
  set_channel_off_all_others_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelOffAllOthersOnHandler));
  set_channel_off_all_others_on_function.addParameter(channel_parameter);

  modular_server::Function & set_channels_on_all_others_off_function = modular_server_.createFunction(constants::set_channels_on_all_others_off_function_name);
  set_channels_on_all_others_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelsOnAllOthersOffHandler));
  set_channels_on_all_others_off_function.addParameter(channels_parameter);

  modular_server::Function & set_channels_off_all_others_on_function = modular_server_.createFunction(constants::set_channels_off_all_others_on_function_name);
  set_channels_off_all_others_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelsOffAllOthersOnHandler));
  set_channels_off_all_others_on_function.addParameter(channels_parameter);

  modular_server::Function & get_channels_on_function = modular_server_.createFunction(constants::get_channels_on_function_name);
  get_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::getChannelsOnHandler));
  get_channels_on_function.setResultTypeArray();
  get_channels_on_function.setResultTypeLong();

  modular_server::Function & get_channels_off_function = modular_server_.createFunction(constants::get_channels_off_function_name);
  get_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::getChannelsOffHandler));
  get_channels_off_function.setResultTypeArray();
  get_channels_off_function.setResultTypeLong();

  modular_server::Function & get_channel_count_function = modular_server_.createFunction(constants::get_channel_count_function_name);
  get_channel_count_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::getChannelCountHandler));

  modular_server::Function & save_state_function = modular_server_.createFunction(constants::save_state_function_name);
  save_state_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::saveStateHandler));
  save_state_function.addParameter(state_parameter);

  modular_server::Function & recall_state_function = modular_server_.createFunction(constants::recall_state_function_name);
  recall_state_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::recallStateHandler));
  recall_state_function.addParameter(state_parameter);

  modular_server::Function & add_pwm_function = modular_server_.createFunction(constants::add_pwm_function_name);
  add_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::addPwmHandler));
  add_pwm_function.addParameter(channels_parameter);
  add_pwm_function.addParameter(delay_parameter);
  add_pwm_function.addParameter(period_parameter);
  add_pwm_function.addParameter(on_duration_parameter);
  add_pwm_function.addParameter(count_parameter);
  add_pwm_function.setResultTypeLong();

  modular_server::Function & start_pwm_function = modular_server_.createFunction(constants::start_pwm_function_name);
  start_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::startPwmHandler));
  start_pwm_function.addParameter(channels_parameter);
  start_pwm_function.addParameter(delay_parameter);
  start_pwm_function.addParameter(period_parameter);
  start_pwm_function.addParameter(on_duration_parameter);
  start_pwm_function.setResultTypeLong();

  modular_server::Function & stop_pwm_function = modular_server_.createFunction(constants::stop_pwm_function_name);
  stop_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::stopPwmHandler));
  stop_pwm_function.addParameter(pwm_index_parameter);

  modular_server::Function & stop_all_pwm_function = modular_server_.createFunction(constants::stop_all_pwm_function_name);
  stop_all_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::stopAllPwmHandler));

  modular_server::Function & set_power_function = modular_server_.createFunction(constants::set_power_function_name);
  set_power_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setPowerHandler));
  set_power_function.addParameter(channel_group_parameter);
  set_power_function.addParameter(power_parameter);

  modular_server::Function & get_powers_function = modular_server_.createFunction(constants::get_powers_function_name);
  get_powers_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::getPowersHandler));

  // Callbacks

}

void PowerSwitchController::saveState(size_t state)
{
  if (state >= constants::STATE_COUNT)
  {
    return;
  }
  long channels = getChannelsOn();
  modular_server_.property(constants::states_property_name).setElementValue(state,channels);
}

void PowerSwitchController::recallState(size_t state)
{
  if (state >= constants::STATE_COUNT)
  {
    return;
  }
  long channels;
  modular_server_.property(constants::states_property_name).getElementValue(state,channels);
  setChannels(channels);
}

int PowerSwitchController::addPwm(uint32_t channels,
  long delay,
  long period,
  long on_duration,
  long count)
{
  if (indexed_pulses_.full())
  {
    return constants::bad_index;
  }
  power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOnHandler),
    makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOffHandler),
    delay,
    period,
    on_duration,
    count,
    index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

int PowerSwitchController::startPwm(uint32_t channels,
  long delay,
  long period,
  long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOnHandler),
    makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOffHandler),
    delay,
    period,
    on_duration,
    index);
  event_controller_.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::startPwmHandler));
  event_controller_.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::stopPwmHandler));
  indexed_pulses_[index].event_id_pair = event_id_pair;
  event_controller_.enable(event_id_pair);
  return index;
}

void PowerSwitchController::stopPwm(int pwm_index)
{
  if (pwm_index < 0)
  {
    return;
  }
  if (indexed_pulses_.indexHasValue(pwm_index))
  {
    constants::PulseInfo pulse_info = indexed_pulses_[pwm_index];
    event_controller_.remove(pulse_info.event_id_pair);
  }
}

void PowerSwitchController::stopAllPwm()
{
  for (size_t i=0; i<constants::INDEXED_PULSES_COUNT_MAX; ++i)
  {
    stopPwm(i);
  }
}

uint32_t PowerSwitchController::arrayToChannels(ArduinoJson::JsonArray channels_array)
{
  uint32_t channels = 0;
  uint32_t bit = 1;
  for (long channel : channels_array)
  {
    channels |= bit << channel;
  }
  return channels;
}

void PowerSwitchController::setPower(size_t channel_group,
  uint8_t power)
{
#if !defined(__AVR_ATmega2560__)
  if (channel_group < constants::IC_COUNT)
  {
    uint8_t power_constrained = constrain(power,constants::power_min,
      constants::power_max);
    powers_[channel_group] = power_constrained;
    long pwm_value = map(power_constrained,
      constants::power_min,
      constants::power_max,
      constants::power_pwm_value_min,
      constants::power_pwm_value_max);
    analogWriteFrequency(constants::map_pins[channel_group],
      constants::power_pwm_frequency);
    analogWriteResolution(constants::power_pwm_resolution);
    analogWrite(constants::map_pins[channel_group],
      pwm_value);
  }
#endif
}

uint8_t PowerSwitchController::getPower(size_t channel_group)
{
  uint8_t power = 0;
  if (channel_group < constants::IC_COUNT)
  {
    power = powers_[channel_group];
  }
  return power;
}

void PowerSwitchController::startPwmHandler(int index)
{
}

void PowerSwitchController::stopPwmHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOff(channels);
  indexed_pulses_.remove(index);
}

void PowerSwitchController::setChannelOnHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOn(channel);
}

void PowerSwitchController::setChannelOffHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOff(channel);
}

void PowerSwitchController::setChannelsOnHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  const uint32_t channels = arrayToChannels(channels_array);
  setChannelsOn(channels);
}

void PowerSwitchController::setChannelsOffHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  const uint32_t channels = arrayToChannels(channels_array);
  setChannelsOff(channels);
}

void PowerSwitchController::toggleChannelHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  toggleChannel(channel);
}

void PowerSwitchController::toggleChannelsHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  const uint32_t channels = arrayToChannels(channels_array);
  toggleChannels(channels);
}

void PowerSwitchController::toggleAllChannelsHandler()
{
  toggleAllChannels();
}

void PowerSwitchController::setAllChannelsOnHandler()
{
  setAllChannelsOn();
}

void PowerSwitchController::setAllChannelsOffHandler()
{
  setAllChannelsOff();
}

void PowerSwitchController::setChannelOnAllOthersOffHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOnAllOthersOff(channel);
}

void PowerSwitchController::setChannelOffAllOthersOnHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOffAllOthersOn(channel);
}

void PowerSwitchController::setChannelsOnAllOthersOffHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  const uint32_t channels = arrayToChannels(channels_array);
  setChannelsOnAllOthersOff(channels);
}

void PowerSwitchController::setChannelsOffAllOthersOnHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  const uint32_t channels = arrayToChannels(channels_array);
  setChannelsOffAllOthersOn(channels);
}

void PowerSwitchController::getChannelsOnHandler()
{
  uint32_t channels_on = getChannelsOn();
  uint32_t bit = 1;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (channels_on & (bit << channel))
    {
      modular_server_.response().write(channel);
    }
  }
  modular_server_.response().endArray();
}

void PowerSwitchController::getChannelsOffHandler()
{
  uint32_t channels_on = getChannelsOn();
  uint32_t channels_off = ~channels_on;
  uint32_t bit = 1;
  modular_server_.response().writeResultKey();
  modular_server_.response().beginArray();
  for (size_t channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (channels_off & (bit << channel))
    {
      modular_server_.response().write(channel);
    }
  }
  modular_server_.response().endArray();
}

void PowerSwitchController::getChannelCountHandler()
{
  int channel_count = getChannelCount();
  modular_server_.response().returnResult(channel_count);
}

void PowerSwitchController::saveStateHandler()
{
  int state;
  modular_server_.parameter(constants::state_parameter_name).getValue(state);
  saveState(state);
}

void PowerSwitchController::recallStateHandler()
{
  int state;
  modular_server_.parameter(constants::state_parameter_name).getValue(state);
  recallState(state);
}

void PowerSwitchController::addPwmHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  const uint32_t channels = arrayToChannels(channels_array);
  int index = addPwm(channels,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void PowerSwitchController::startPwmHandler()
{
  ArduinoJson::JsonArray channels_array;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  const uint32_t channels = arrayToChannels(channels_array);
  int index = startPwm(channels,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void PowerSwitchController::stopPwmHandler()
{
  int pwm_index;
  modular_server_.parameter(constants::pwm_index_parameter_name).getValue(pwm_index);
  stopPwm(pwm_index);
}

void PowerSwitchController::stopAllPwmHandler()
{
  stopAllPwm();
}

void PowerSwitchController::setPowerHandler()
{
  size_t channel_group;
  modular_server_.parameter(constants::channel_group_parameter_name).getValue(channel_group);

  size_t power;
  modular_server_.parameter(constants::power_parameter_name).getValue(power);

  setPower(channel_group,power);
}

void PowerSwitchController::getPowersHandler()
{
  modular_server_.response().writeResultKey();

  modular_server_.response().beginArray();

  for (size_t i=0; i<constants::IC_COUNT; ++i)
  {
    modular_server_.response().write(getPower(i));
  }

  modular_server_.response().endArray();
}

void PowerSwitchController::setChannelsOnHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOn(channels);
}

void PowerSwitchController::setChannelsOffHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOff(channels);
}
