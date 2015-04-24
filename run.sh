#!/bin/bash
make clean
make
rmmod getDevice2_copy6
insmod getDevice2_copy6.ko
