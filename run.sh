#!/bin/bash
make clean
make
rmmod getDevice2
insmod getDevice2.ko
