# PowerSwitchController

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

## Ancestors

[ModularServer](https://github.com/janelia-arduino/ModularServer)

[ModularDeviceBase](https://github.com/janelia-arduino/ModularDeviceBase)

## Clients

## Devices

[modular_device_base](https://github.com/janelia-modular-devices/modular_device_base.git)

[power_switch_controller](https://github.com/janelia-modular-devices/power_switch_controller.git)

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
      "stopAllPwm",
      "setPower",
      "getPowers"
    ],
    "parameters":[
      "channel",
      "channels",
      "state",
      "delay",
      "period",
      "on_duration",
      "count",
      "pwm_index",
      "channel_group",
      "power"
    ],
    "properties":[
      "states"
    ],
    "callbacks":[]
  }
}
```

## API Verbose

[API Verbose](./api.json)

## More Detailed Modular Device Information

[modular-devices](https://github.com/janelia-modular-devices/modular-devices)

## Install Library Dependencies

[arduino-libraries](https://github.com/janelia-arduino/arduino-libraries)
