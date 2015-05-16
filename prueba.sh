#! /bin/bash

for i in {0..3} ; do
    for j in {0..9} ; do
        ./client 127.0.0.1 -f $j -c $i -p 7557
    done
done
