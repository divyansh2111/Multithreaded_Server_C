#!/bin/bash

for N in {1..50}
do
    ./client ./cli /lib/x86_64-linux-gnu/libm.so.6 cos 5.0 6.0 7.0 8.8 hello
done
wait