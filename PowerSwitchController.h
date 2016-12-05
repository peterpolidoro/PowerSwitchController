// ----------------------------------------------------------------------------
// PowerSwitchController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef POWER_SWITCH_CONTROLLER_H
#define POWER_SWITCH_CONTROLLER_H
#include "Flash.h"
#include <EEPROM.h>
#include "Streaming.h"
#include "ArduinoJson.h"
#include "JsonSanitizer.h"
#include "JsonStream.h"
#include "Array.h"
#include "Vector.h"
#include "MemoryFree.h"
#include "ConstantVariable.h"
#include "SavedVariable.h"
#include "Functor.h"
#include "IndexedContainer.h"
#include "FunctorCallbacks.h"

#include "EventController.h"

#include "ModularServer.h"
#include "ModularDevice.h"

#include "utility/Constants.h"


class PowerSwitchController : public ModularDevice
{
public:
  PowerSwitchController();
  virtual void setup();
  void setChannelOn(const size_t channel, const ConstantString * const polarity_ptr);
  void setChannelOff(const size_t channel);
  void setChannelsOn(const uint32_t channels, const ConstantString * const polarity_ptr);
  void setChannelsOff(const uint32_t channels);
  void setAllChannelsOn(const ConstantString * const polarity_ptr);
  void setAllChannelsOff();
  int addPwm(const uint32_t channels,
             const ConstantString * const polarity_ptr,
             const long delay,
             const long period,
             const long on_duration,
             const long count);
  int startPwm(const uint32_t channels,
               const ConstantString * const polarity_ptr,
               const long delay,
               const long period,
               const long on_duration);
  int addTogglePwm(const uint32_t channels,
                   const ConstantString * const polarity_ptr,
                   const long delay,
                   const long period,
                   const long on_duration,
                   const long count);
  int startTogglePwm(const uint32_t channels,
                     const ConstantString * const polarity_ptr,
                     const long delay,
                     const long period,
                     const long on_duration);
  void stopPwm(const int pwm_index);
  void stopAllPwm();
  uint32_t arrayToChannels(ArduinoJson::JsonArray & channels_array);
  ConstantString * const stringToPolarityPtr(const char * string);

  // Handlers
  virtual void startPwmHandler(int index);
  virtual void stopPwmHandler(int index);

private:
  modular_server::Interrupt interrupts_[power_switch_controller::constants::INTERRUPT_COUNT_MAX];

  modular_server::Property properties_[power_switch_controller::constants::PROPERTY_COUNT_MAX];
  modular_server::Parameter parameters_[power_switch_controller::constants::PARAMETER_COUNT_MAX];
  modular_server::Function functions_[power_switch_controller::constants::FUNCTION_COUNT_MAX];
  modular_server::Callback callbacks_[power_switch_controller::constants::CALLBACK_COUNT_MAX];

  EventController<power_switch_controller::constants::EVENT_COUNT_MAX> event_controller_;

  IndexedContainer<power_switch_controller::constants::PulseInfo,
                   power_switch_controller::constants::INDEXED_PULSES_COUNT_MAX> indexed_pulses_;

  // Handlers
  void setChannelOnHandler();
  void setChannelOffHandler();
  void setChannelsOnHandler();
  void setChannelsOffHandler();
  void setAllChannelsOnHandler();
  void setAllChannelsOffHandler();
  void addPwmHandler();
  void startPwmHandler();
  void addTogglePwmHandler();
  void startTogglePwmHandler();
  void stopPwmHandler();
  void stopAllPwmHandler();
  void setChannelsOnHandler(int index);
  void setChannelsOffHandler(int index);
  void setChannelsOnReversedHandler(int index);

};

#endif
