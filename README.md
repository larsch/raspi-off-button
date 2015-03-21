# raspi-off-button
Daemon that powers off an Raspberry PI power off on GPIO signal

## Overview

Daemon that polls a GPIO pin on the Raspberry Pi, waiting for it to be signalled. When the pin is detected to be high (or low if the `--invert` option is given), raspi-off-button will launch a command. The default command is simply `/sbin/poweroff`, but it can be changed using the `--command` option).

By default, `raspi-off-button` will enable the pull-down resistor on GPIO 4 (Pin number 7) and wait for it to go high for at least 1 second before running the `/sbin/poweroff` command.

## Usage

    Usage: raspi-off-button [OPTIONS]
      -t, --time     Time the input must be active (milliseconds, default 1000)
      -f, --freq     Polling frequency (milliseconds, default 25)
      -c, --command  Command to run (default "/sbin/poweroff")
      -g, --gpio     GPIO number (default 4)
      -i, --invert   Invert input, i.e. pull up and wait for it to go low
      -p, --pull     Pull up/down mode ('up', 'down', 'off', default is 'down' or 'up' if invert is enabled)
      -v, --verbose  Verbose output
