#!/bin/bash
make clean
make
rmmod getDevice2_copy3
insmod getDevice2_copy3.ko
