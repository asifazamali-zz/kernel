#!/bin/bash
make clean
make
rmmod getDevice2_copy2
insmod getDevice2_copy2.ko
