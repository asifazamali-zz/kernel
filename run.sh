#!/bin/bash
make clean
make
rmmod getDevice2_copy4
insmod getDevice2_copy4.ko
