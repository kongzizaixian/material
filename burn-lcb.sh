#!/bin/bash

##sudo python ./hisi-idt.py -d /dev/ttyUSB0 --img1 fastboot1.img --img2 fastboot2.img

#RAMDISK=./initrd.img-3.18.0-linaro-hikey
#RAMDISK=./rootfs.cpio.lzo
#KERN_IMG=./Image
#DTB_IMG=./hi6220-hikey.dtb
#dd if=/dev/zero of=./boot_fat.img bs=512 count=131072
#sudo mkfs.fat -n "BOOT IMG" ./boot_fat.img
#sudo mkdir -p ./file && sudo mount -o loop,rw,sync ./boot_fat.img ./file
#sudo cp $KERN_IMG ./file/
#sudo cp $DTB_IMG ./file/lcb.dtb
#sudo cp $RAMDISK ./file/ramdisk.img
#sudo cp cmdline ./file/cmdline
#sync
#sudo umount ./boot_fat.img

sudo mount -o loop,rw,sync boot-fat.emmc.img boot-fat && \
sudo cp hi6220-hikey.dtb boot-fat/lcb.dtb && \
sudo cp Image boot-fat && \
sudo sync && \
sudo umount boot-fat && \



#sudo fastboot flash ptable ptable.img
#sudo fastboot flash boot boot_fat.img
sudo fastboot flash boot boot-fat.emmc.img
#sudo fastboot flash fastboot1 fastboot1.img
#sudo fastboot flash fastboot fastboot2.img
#sudo fastboot flash nvme nvme.img
#sudo fastboot flash mcuimage mcuimage.bin
#sudo fastboot flash system hikey-jessie_developer_20150303-124.emmc.img
#sudo fastboot flash system hikey-jessie_developer_20150409-254.emmc.img
#sudo fastboot flash system hikey-jessie_developer_20150526-305.emmc.img
#sudo fastboot flash system kong.img
sudo fastboot reboot
