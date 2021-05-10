#!/bin/bash
#
# singlerun-example.sh
# example file for a local job sumbission

time /usr/bin/mpirun \
  ./directdebye \
  --efield-out=1:50:100:1 \
  --efield-lb=0:-0.1:-0.2:-6.6712819049999997504e-13 \
  --efield-ub=1:0.1:0.2:10 \
  --radius=50.0 \
  --temporal-offset=3.9928428404523743911e-14 \
  --output-file="0000-parabola--6.6712819049999997504e-13.h5" \
  --pulse-function='"sqrt(40e-15*sqrt(pi)/sqrt(ln2))*exp(-(x-2354564459136066.560)*(x-2354564459136066.560)*40e-15*40e-15*0.125/ln2)"' \
  --apodization-function='"0.5*(1+cos(x))"' \
  rays.txt
