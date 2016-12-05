// ----------------------------------------------------------------------------
// PowerSwitchController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "PowerSwitchController.h"


using namespace power_switch_controller;

PowerSwitchController::PowerSwitchController()
{
}

void PowerSwitchController::setup()
{
  // Parent Setup
  ModularDevice::setup();

  // Event Controller Setup
  event_controller_.setup();

  // Pin Setup
  for (int channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    pinMode(constants::enable_pins[channel],OUTPUT);
    digitalWrite(constants::enable_pins[channel],LOW);
    pinMode(constants::dir_a_pins[channel],OUTPUT);
    pinMode(constants::dir_b_pins[channel],OUTPUT);
  }

  // Set Device ID
  modular_server_.setDeviceName(constants::device_name);

  // Add Hardware
  modular_server_.addHardware(constants::hardware_info,
                              interrupts_);

  // Interrupts
#if defined(__AVR_ATmega2560__)
  modular_server::Interrupt & bnc_b_interrupt = modular_server_.createInterrupt(constants::bnc_b_interrupt_name,
                                                                                constants::bnc_b_pin);

  modular_server::Interrupt & switch_0_interrupt = modular_server_.createInterrupt(constants::switch_0_interrupt_name,
                                                                                   constants::switch_0_pin);

  modular_server::Interrupt & switch_1_interrupt = modular_server_.createInterrupt(constants::switch_1_interrupt_name,
                                                                                   constants::switch_1_pin);

  modular_server::Interrupt & switch_2_interrupt = modular_server_.createInterrupt(constants::switch_2_interrupt_name,
                                                                                   constants::switch_2_pin);

#endif

  // Add Firmware
  modular_server_.addFirmware(constants::firmware_info,
                              properties_,
                              parameters_,
                              functions_,
                              callbacks_);
  // Properties
  modular_server::Property & polarity_reversed_property = modular_server_.createProperty(constants::polarity_reversed_property_name,constants::polarity_reversed_default);

  modular_server::Property & channels_enabled_property = modular_server_.createProperty(constants::channels_enabled_property_name,constants::channels_enabled_default);
  channels_enabled_property.attachPostSetElementValueFunctor(makeFunctor((Functor1<const size_t> *)0,*this,&PowerSwitchController::setChannelOff));

  // Parameters
  modular_server::Parameter & channel_parameter = modular_server_.createParameter(constants::channel_parameter_name);
  channel_parameter.setRange(0,constants::CHANNEL_COUNT-1);

  modular_server::Parameter & channels_parameter = modular_server_.createParameter(constants::channels_parameter_name);
  channels_parameter.setRange(0,constants::CHANNEL_COUNT-1);
  channels_parameter.setArrayLengthRange(1,constants::CHANNEL_COUNT);

  modular_server::Parameter & polarity_parameter = modular_server_.createParameter(constants::polarity_parameter_name);
  polarity_parameter.setTypeString();
  polarity_parameter.setSubset(constants::polarity_ptr_subset);

  modular_server::Parameter & delay_parameter = modular_server_.createParameter(constants::delay_parameter_name);
  delay_parameter.setRange(constants::delay_min,constants::delay_max);
  delay_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & period_parameter = modular_server_.createParameter(constants::period_parameter_name);
  period_parameter.setRange(constants::period_min,constants::period_max);
  period_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & on_duration_parameter = modular_server_.createParameter(constants::on_duration_parameter_name);
  on_duration_parameter.setRange(constants::on_duration_min,constants::on_duration_max);
  on_duration_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & count_parameter = modular_server_.createParameter(constants::count_parameter_name);
  count_parameter.setRange(constants::count_min,constants::count_max);
  count_parameter.setUnits(constants::ms_unit);

  modular_server::Parameter & pwm_index_parameter = modular_server_.createParameter(constants::pwm_index_parameter_name);
  pwm_index_parameter.setRange(0,constants::INDEXED_PULSES_COUNT_MAX-1);

  // Functions
  modular_server::Function & set_channel_on_function = modular_server_.createFunction(constants::set_channel_on_function_name);
  set_channel_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelOnHandler));
  set_channel_on_function.addParameter(channel_parameter);
  set_channel_on_function.addParameter(polarity_parameter);

  modular_server::Function & set_channel_off_function = modular_server_.createFunction(constants::set_channel_off_function_name);
  set_channel_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelOffHandler));
  set_channel_off_function.addParameter(channel_parameter);

  modular_server::Function & set_channels_on_function = modular_server_.createFunction(constants::set_channels_on_function_name);
  set_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelsOnHandler));
  set_channels_on_function.addParameter(channels_parameter);
  set_channels_on_function.addParameter(polarity_parameter);

  modular_server::Function & set_channels_off_function = modular_server_.createFunction(constants::set_channels_off_function_name);
  set_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setChannelsOffHandler));
  set_channels_off_function.addParameter(channels_parameter);

  modular_server::Function & set_all_channels_on_function = modular_server_.createFunction(constants::set_all_channels_on_function_name);
  set_all_channels_on_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setAllChannelsOnHandler));
  set_all_channels_on_function.addParameter(polarity_parameter);

  modular_server::Function & set_all_channels_off_function = modular_server_.createFunction(constants::set_all_channels_off_function_name);
  set_all_channels_off_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::setAllChannelsOffHandler));

  modular_server::Function & add_pwm_function = modular_server_.createFunction(constants::add_pwm_function_name);
  add_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::addPwmHandler));
  add_pwm_function.addParameter(channels_parameter);
  add_pwm_function.addParameter(polarity_parameter);
  add_pwm_function.addParameter(delay_parameter);
  add_pwm_function.addParameter(period_parameter);
  add_pwm_function.addParameter(on_duration_parameter);
  add_pwm_function.addParameter(count_parameter);
  add_pwm_function.setReturnTypeLong();

  modular_server::Function & start_pwm_function = modular_server_.createFunction(constants::start_pwm_function_name);
  start_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::startPwmHandler));
  start_pwm_function.addParameter(channels_parameter);
  start_pwm_function.addParameter(polarity_parameter);
  start_pwm_function.addParameter(delay_parameter);
  start_pwm_function.addParameter(period_parameter);
  start_pwm_function.addParameter(on_duration_parameter);
  start_pwm_function.setReturnTypeLong();

  modular_server::Function & add_toggle_pwm_function = modular_server_.createFunction(constants::add_toggle_pwm_function_name);
  add_toggle_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::addTogglePwmHandler));
  add_toggle_pwm_function.addParameter(channels_parameter);
  add_toggle_pwm_function.addParameter(polarity_parameter);
  add_toggle_pwm_function.addParameter(delay_parameter);
  add_toggle_pwm_function.addParameter(period_parameter);
  add_toggle_pwm_function.addParameter(on_duration_parameter);
  add_toggle_pwm_function.addParameter(count_parameter);
  add_toggle_pwm_function.setReturnTypeLong();

  modular_server::Function & start_toggle_pwm_function = modular_server_.createFunction(constants::start_toggle_pwm_function_name);
  start_toggle_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::startTogglePwmHandler));
  start_toggle_pwm_function.addParameter(channels_parameter);
  start_toggle_pwm_function.addParameter(polarity_parameter);
  start_toggle_pwm_function.addParameter(delay_parameter);
  start_toggle_pwm_function.addParameter(period_parameter);
  start_toggle_pwm_function.addParameter(on_duration_parameter);
  start_toggle_pwm_function.setReturnTypeLong();

  modular_server::Function & stop_pwm_function = modular_server_.createFunction(constants::stop_pwm_function_name);
  stop_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::stopPwmHandler));
  stop_pwm_function.addParameter(pwm_index_parameter);

  modular_server::Function & stop_all_pwm_function = modular_server_.createFunction(constants::stop_all_pwm_function_name);
  stop_all_pwm_function.attachFunctor(makeFunctor((Functor0 *)0,*this,&PowerSwitchController::stopAllPwmHandler));

  // Callbacks

}

void PowerSwitchController::setChannelOn(const size_t channel, const ConstantString * const polarity_ptr)
{
  bool channel_enabled;
  modular_server_.property(constants::channels_enabled_property_name).getElementValue(channel,
                                                                                channel_enabled);
  if (!channel_enabled)
  {
    return;
  }
  bool channel_polarity_reversed;
  modular_server_.property(constants::polarity_reversed_property_name).getElementValue(channel,
                                                                                 channel_polarity_reversed);
  const ConstantString * polarity_corrected_ptr = polarity_ptr;
  if (channel_polarity_reversed)
  {
    polarity_corrected_ptr = ((polarity_ptr == &constants::polarity_positive) ? &constants::polarity_negative : &constants::polarity_positive);
  }
  if (polarity_corrected_ptr == &constants::polarity_positive)
  {
    digitalWrite(constants::dir_a_pins[channel],HIGH);
    digitalWrite(constants::dir_b_pins[channel],LOW);
  }
  else
  {
    digitalWrite(constants::dir_a_pins[channel],LOW);
    digitalWrite(constants::dir_b_pins[channel],HIGH);
  }
  digitalWrite(constants::enable_pins[channel],HIGH);
}

void PowerSwitchController::setChannelOff(const size_t channel)
{
  digitalWrite(constants::enable_pins[channel],LOW);
}

void PowerSwitchController::setChannelsOn(const uint32_t channels, const ConstantString * const polarity_ptr)
{
  uint32_t bit = 1;
  for (int channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (channels & (bit << channel))
    {
      setChannelOn(channel,polarity_ptr);
    }
  }
}

void PowerSwitchController::setChannelsOff(const uint32_t channels)
{
  uint32_t bit = 1;
  for (int channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    if (channels & (bit << channel))
    {
      setChannelOff(channel);
    }
  }
}

void PowerSwitchController::setAllChannelsOn(const ConstantString * const polarity_ptr)
{
  for (int channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    setChannelOn(channel,polarity_ptr);
  }
}

void PowerSwitchController::setAllChannelsOff()
{
  for (int channel=0; channel<constants::CHANNEL_COUNT; ++channel)
  {
    setChannelOff(channel);
  }
}

int PowerSwitchController::addPwm(const uint32_t channels,
                              const ConstantString * const polarity_ptr,
                              const long delay,
                              const long period,
                              const long on_duration,
                              const long count)
{
  if (indexed_pulses_.full())
  {
    return constants::bad_index;
  }
  power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  pulse_info.polarity_ptr = polarity_ptr;
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

int PowerSwitchController::startPwm(const uint32_t channels,
                                const ConstantString * const polarity_ptr,
                                const long delay,
                                const long period,
                                const long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  pulse_info.polarity_ptr = polarity_ptr;
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

int PowerSwitchController::addTogglePwm(const uint32_t channels,
                                    const ConstantString * const polarity_ptr,
                                    const long delay,
                                    const long period,
                                    const long on_duration,
                                    const long count)
{
  if (indexed_pulses_.full())
  {
    return constants::bad_index;
  }
  power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  pulse_info.polarity_ptr = polarity_ptr;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOnHandler),
                                                                 makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOnReversedHandler),
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

int PowerSwitchController::startTogglePwm(const uint32_t channels,
                                      const ConstantString * const polarity_ptr,
                                      const long delay,
                                      const long period,
                                      const long on_duration)
{
  if (indexed_pulses_.full())
  {
    return -1;
  }
  power_switch_controller::constants::PulseInfo pulse_info;
  pulse_info.channels = channels;
  pulse_info.polarity_ptr = polarity_ptr;
  int index = indexed_pulses_.add(pulse_info);
  EventIdPair event_id_pair = event_controller_.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOnHandler),
                                                                         makeFunctor((Functor1<int> *)0,*this,&PowerSwitchController::setChannelsOnReversedHandler),
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

void PowerSwitchController::stopPwm(const int pwm_index)
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

uint32_t PowerSwitchController::arrayToChannels(ArduinoJson::JsonArray & channels_array)
{
  uint32_t channels = 0;
  uint32_t bit = 1;
  for (ArduinoJson::JsonArray::iterator channels_it=channels_array.begin();
       channels_it != channels_array.end();
       ++channels_it)
  {
    long channel = *channels_it;
    channels |= bit << channel;
  }
  return channels;
}

ConstantString * const PowerSwitchController::stringToPolarityPtr(const char * string)
{
  if (string == constants::polarity_positive)
  {
    return &constants::polarity_positive;
  }
  else
  {
    return &constants::polarity_negative;
  }
}

// Handlers must be non-blocking (avoid 'delay')
//
// modular_server_.parameter(parameter_name).getValue(value) value type must be either:
// fixed-point number (int, long, etc.)
// floating-point number (float, double)
// bool
// const char *
// ArduinoJson::JsonArray *
// ArduinoJson::JsonObject *
//
// For more info read about ArduinoJson parsing https://github.com/janelia-arduino/ArduinoJson
//
// modular_server_.property(property_name).getValue(value) value type must match the property default type
// modular_server_.property(property_name).setValue(value) value type must match the property default type
// modular_server_.property(property_name).getElementValue(value) value type must match the property array element default type
// modular_server_.property(property_name).setElementValue(value) value type must match the property array element default type

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
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  setChannelOn(channel,polarity_ptr);
}

void PowerSwitchController::setChannelOffHandler()
{
  int channel;
  modular_server_.parameter(constants::channel_parameter_name).getValue(channel);
  setChannelOff(channel);
}

void PowerSwitchController::setChannelsOnHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  setChannelsOn(channels,polarity_ptr);
}

void PowerSwitchController::setChannelsOffHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  setChannelsOff(channels);
}

void PowerSwitchController::setAllChannelsOnHandler()
{
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  setAllChannelsOn(polarity_ptr);
}

void PowerSwitchController::setAllChannelsOffHandler()
{
  setAllChannelsOff();
}

void PowerSwitchController::addPwmHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  int index = addPwm(channels,polarity_ptr,delay,period,on_duration,count);
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
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  int index = startPwm(channels,polarity_ptr,delay,period,on_duration);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void PowerSwitchController::addTogglePwmHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  long count;
  modular_server_.parameter(constants::count_parameter_name).getValue(count);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  int index = addTogglePwm(channels,polarity_ptr,delay,period,on_duration,count);
  if (index >= 0)
  {
    modular_server_.response().returnResult(index);
  }
  else
  {
    modular_server_.response().returnError(constants::pwm_error);
  }
}

void PowerSwitchController::startTogglePwmHandler()
{
  ArduinoJson::JsonArray * channels_array_ptr;
  modular_server_.parameter(constants::channels_parameter_name).getValue(channels_array_ptr);
  const char * polarity_string;
  modular_server_.parameter(constants::polarity_parameter_name).getValue(polarity_string);
  long delay;
  modular_server_.parameter(constants::delay_parameter_name).getValue(delay);
  long period;
  modular_server_.parameter(constants::period_parameter_name).getValue(period);
  long on_duration;
  modular_server_.parameter(constants::on_duration_parameter_name).getValue(on_duration);
  const uint32_t channels = arrayToChannels(*channels_array_ptr);
  const ConstantString * const polarity_ptr = stringToPolarityPtr(polarity_string);
  int index = startTogglePwm(channels,polarity_ptr,delay,period,on_duration);
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

void PowerSwitchController::setChannelsOnHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  const ConstantString * const polarity_ptr = indexed_pulses_[index].polarity_ptr;
  setChannelsOn(channels,polarity_ptr);
}

void PowerSwitchController::setChannelsOffHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  setChannelsOff(channels);
}

void PowerSwitchController::setChannelsOnReversedHandler(int index)
{
  uint32_t & channels = indexed_pulses_[index].channels;
  const ConstantString * const polarity_ptr = indexed_pulses_[index].polarity_ptr;
  const ConstantString * polarity_reversed_ptr;
  polarity_reversed_ptr = ((polarity_ptr == &constants::polarity_positive) ? &constants::polarity_negative : &constants::polarity_positive);
  setChannelsOn(channels,polarity_reversed_ptr);
}
