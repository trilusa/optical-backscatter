#!/bin/bash

export TARGET="adc.pru0"

echo TARGET=$TARGET

config-pin P9_27 pruout #CS
config-pin P9_41 pruin #SDO
config-pin P9_30 pruout #SCLK

echo out >  /sys/class/gpio/gpio49/direction
echo "P9_23:"; cat /sys/class/gpio/gpio49/direction
echo 0 > /sys/class/gpio/gpio49/value
