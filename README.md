# PowerSwitchController

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

[Examples](./examples)

## API

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
      "toggleChannel",
      "toggleChannels",
      "toggleAllChannels",
      "setAllChannelsOn",
      "setAllChannelsOff",
      "setChannelOnAllOthersOff",
      "setChannelOffAllOthersOn",
      "setChannelsOnAllOthersOff",
      "setChannelsOffAllOthersOn",
      "getChannelsOn",
      "getChannelsOff",
      "getChannelCount",
      "saveState",
      "recallState",
      "addPwm",
      "startPwm",
      "stopPwm",
      "stopAllPwm"
    ],
    "parameters":[
      "channel",
      "channels",
      "state",
      "delay",
      "period",
      "on_duration",
      "count",
      "pwm_index"
    ],
    "properties":[
      "states"
    ],
    "callbacks":[]
  }
}
```

## More Detailed Modular Device Information

[modular-devices](https://github.com/janelia-modular-devices/modular-devices)

## Install Library Dependencies

[arduino-libraries](https://github.com/janelia-arduino/arduino-libraries)
