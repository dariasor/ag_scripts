#!/bin/bash

head -1 `ls $1/*.csv | head -1` > $2

for filename in $1/*.csv; do
    tail -n +2 < $filename >> $2
done