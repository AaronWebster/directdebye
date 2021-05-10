#!/bin/bash
#
# getrays.sh
# gets rays from the raytrace directory and appends some extra ones on to
# the end if need be

# get rays from the raytrace directory
cat rays.txt | tr -d '' | grep -v "	2	" > rays2.txt

# add an extra set of ones on the end
sed -i -e "s/$/\t1/" rays2.txt

wc -l rays2.txt
mv rays2.txt rays.txt
