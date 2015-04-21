#!/bin/bash
make clean
make
rmmod getDevice2_copy
insmod getDevice2_copy.ko
