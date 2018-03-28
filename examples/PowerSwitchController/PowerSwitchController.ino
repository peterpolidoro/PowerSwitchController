#include <PowerSwitchController.h>


PowerSwitchController dev;

void setup()
{
  dev.setup();
  dev.startServer();
}

void loop()
{
  dev.update();
}
