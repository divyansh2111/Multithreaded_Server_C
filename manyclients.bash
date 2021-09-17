#!/bin/bash

for N in {1..50}
do
    ./client 8000 /lib/x86_64-linux-gnu/libm.so.6 sqrt 25.0 
    echo
done
wait