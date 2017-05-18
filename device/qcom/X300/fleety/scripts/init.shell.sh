#! /system/bin/sh
echo "shell script called"
busybox mke2fs -T ext4 /dev/block/ram0
mount -t ext4 -o rw /dev/block/ram0  /mnt/ramdisk
sleep 2
chmod -R 777 /mnt/ramdisk
sleep 10

#sh /system/scripts/init.iptables.sh
sh /system/scripts/init.event.sh
