#PowerSwitchController

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

[Examples](./examples)

##API

```json
{
  "id":"getApi",
  "result":{
    "firmware":["PowerSwitchController"],
    "functions":[
      "setChannelOn",
      "setChannelOff",
      "setChannelsOn",
      "setChannelsOff",
      "setAllChannelsOn",
      "setAllChannelsOff",
      "addPwm",
      "startPwm",
      "addTogglePwm",
      "startTogglePwm",
      "stopPwm",
      "stopAllPwm"
    ],
    "parameters":[
      "channel",
      "channels",
      "polarity",
      "delay",
      "period",
      "on_duration",
      "count",
      "pwm_index"
    ],
    "properties":[
      "polarityReversed",
      "channelsEnabled"
    ],
    "callbacks":[]
  }
}
```

##More Detailed Modular Device Information

[modular-devices](https://github.com/janelia-modular-devices/modular-devices)

##Install Library Dependencies

[arduino-libraries](https://github.com/janelia-arduino/arduino-libraries)
