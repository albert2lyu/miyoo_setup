#!/bin/sh
mkdir /mnt/.miyoo
echo -n $1 > /mnt/.miyoo/version
if [ $1 -lt 3 ]; then
	touch /mnt/.miyoo/flashlow
	rm /mnt/.miyoo/rumblelow
	rm /mnt/.miyoo/flip
	cp -r bitboy/* /mnt/
elif [ $1 -eq 3 ]; then
	rm /mnt/.miyoo/flip
	rm /mnt/.miyoo/flashlow
	touch /mnt/.miyoo/rumblelow
	cp -r bitboy/* /mnt/
else 
	touch /mnt/.miyoo/flip
	rm /mnt/.miyoo/flashlow
	touch /mnt/.miyoo/rumblelow
	cp -r pocketgo/* /mnt/
fi


