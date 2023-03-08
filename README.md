[![Build Status](https://github.com/pqrs-org/cpp-osx-iokit_hid_device_report_monitor/workflows/CI/badge.svg)](https://github.com/pqrs-org/cpp-osx-iokit_hid_device_report_monitor/actions)
[![License](https://img.shields.io/badge/license-Boost%20Software%20License-blue.svg)](https://github.com/pqrs-org/cpp-osx-iokit_hid_device_report_monitor/blob/main/LICENSE.md)

# cpp-osx-iokit_hid_device_report_monitor

A wrapper of `IOHIDDeviceRegisterInputReportCallback`.

## Requirements

cpp-osx-iokit_hid_device_report_monitor depends the following classes.

- [Nod](https://github.com/fr00b0/nod)
- [pqrs::cf::run_loop_thread](https://github.com/pqrs-org/cpp-cf-run_loop_thread)
- [pqrs::dispatcher](https://github.com/pqrs-org/cpp-dispatcher)
- [pqrs::osx::iokit_hid_device](https://github.com/pqrs-org/cpp-osx-iokit_hid_device)
- [pqrs::osx::iokit_return](https://github.com/pqrs-org/cpp-osx-iokit_return)

## Install

### Using package manager

You can install `include/pqrs` by using [cget](https://github.com/pfultz2/cget).

```shell
cget install pqrs-org/cget-recipes
cget install pqrs-org/cpp-osx-iokit_hid_device_report_monitor
```

### Manual install

Copy `include/pqrs` directory into your include directory.
