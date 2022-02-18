#include <PowerSwitchApparatus.h>
#include <EventController.h>


const long BAUD = 115200;
const int DELAY = 2000;
const uint16_t FREQUENCY_MIN = 1000;
const uint16_t FREQUENCY_MAX = 10000;
const uint16_t FREQUENCY_INC = 1000;
const uint8_t EVENT_COUNT_MAX = 8;

uint16_t frequency = FREQUENCY_MIN;
EventController<EVENT_COUNT_MAX> event_controller;

PowerSwitchApparatus<EVENT_COUNT_MAX> psa;

void setup()
{
  event_controller.setup();
  psa.setup(event_controller);
}

void loop()
{
  Serial.print("psa.playToneAt(");
  Serial.print(frequency);
  Serial.println(")");
  psa.playToneAt(frequency,audio_apparatus::constants::SPEAKER_ALL,25);
  delay(DELAY);
  Serial.println("");

  if (frequency == FREQUENCY_MAX)
  {
    frequency = FREQUENCY_MIN;
  }
  frequency += FREQUENCY_INC;
}
