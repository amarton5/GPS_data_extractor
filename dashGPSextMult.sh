#!/bin/bash

for i in *.MP4
do
  ./maigps -c $i
done

cat *.CSV > ALL_CSV.txt
mv ALL_CSV.txt ALL_CSV.CSV
