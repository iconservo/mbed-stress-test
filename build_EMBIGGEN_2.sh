#!/bin/sh

mbed compile -t GCC_ARM -m EMBIGGEN_2 --source=. --source=mbed-embiggen-platform
