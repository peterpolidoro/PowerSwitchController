// ----------------------------------------------------------------------------
// PowerSwitchController.h
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef POWER_SWITCH_CONTROLLER_H
#define POWER_SWITCH_CONTROLLER_H
#include <Functor.h>

#include <IndexedContainer.h>
#include <EventController.h>

#include <TLE72X.h>

#include "PowerSwitchController/Constants.h"


template <uint8_t EVENT_COUNT_MAX>
class PowerSwitchController : public TLE72X
{
public:
  PowerSwitchController();
  void setup();

  void saveState(size_t state);
  void recallState(size_t state);

  int addPwm(uint32_t channels,
    long delay,
    long period,
    long on_duration,
    long count);
  int startPwm(uint32_t channels,
    long delay,
    long period,
    long on_duration);
  void stopPwm(int pwm_index);
  void stopAllPwm();

  void setPower(size_t channel_group,
    uint8_t power);
  uint8_t getPower(size_t channel_group);

  // Handlers
  virtual void startPwmHandler(int index);
  virtual void stopPwmHandler(int index);

protected:
  EventController<EVENT_COUNT_MAX> * event_controller_ptr_;

  IndexedContainer<power_switch_controller::constants::PulseInfo,
    power_switch_controller::constants::INDEXED_PULSES_COUNT_MAX> indexed_pulses_;

private:
  uint8_t powers_[power_switch_controller::constants::IC_COUNT];

  // Handlers
  void setChannelOnHandler();
  void setChannelOffHandler();
  void setChannelsOnHandler();
  void setChannelsOffHandler();
  void toggleChannelHandler();
  void toggleChannelsHandler();
  void toggleAllChannelsHandler();
  void setAllChannelsOnHandler();
  void setAllChannelsOffHandler();
  void setChannelOnAllOthersOffHandler();
  void setChannelOffAllOthersOnHandler();
  void setChannelsOnAllOthersOffHandler();
  void setChannelsOffAllOthersOnHandler();
  void getChannelsOnHandler();
  void getChannelsOffHandler();
  void getChannelCountHandler();
  void saveStateHandler();
  void recallStateHandler();
  void addPwmHandler();
  void startPwmHandler();
  void stopPwmHandler();
  void stopAllPwmHandler();
  void setPowerHandler();
  void getPowersHandler();
  void setChannelsOnHandler(int index);
  void setChannelsOffHandler(int index);

};

#endif
