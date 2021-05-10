#!/bin/bash
#
# creates data suitable for findmax, and then runs findmax to find the
# maximum of the dataset

OFFSETFILE=findoffset.h5
h5totxt -x 0:0 $OFFSETFILE > /tmp/findoffset-one
linspace \
  $(h5totxt -x 0:0 -d efield_lower_bounds $OFFSETFILE) \
  $(h5totxt -x 1:1 -d efield_lower_bounds $OFFSETFILE) \
  $(h5ls *.h5 | awk '{ print $5 }' | sed -e s/,// | head -n1) > /tmp/findoffset-two
paste findoffset-one findoffset-two > /tmp/findoffset-out
findoffset /tmp/findoffset-out
