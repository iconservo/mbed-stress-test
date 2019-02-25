#!/bin/sh

nrfjprog --family NRF52 --program BUILD/EMBIGGEN_2/GCC_ARM/mbed-stress-test.hex --chiperase --verify --reset
