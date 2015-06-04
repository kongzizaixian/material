ST上午10时53分56秒] … 编译modetest方法:
[CST上午10时53分57秒] Xinliang Liu: git clone git://anongit.freedesktop.org/mesa/drm

cd drm

apt-get install autoconf automake libtool pkg-config libpthread-stubs0-dev

./autogen.sh

make
[CST上午10时54分43秒] Xinliang Liu: 连wifi:
[CST上午10时54分44秒] Xinliang Liu: wpa_passphrase hisi hellohisihelloworld > /etc/wpa_supplicant.conf

ifconfig wlan0 up; wpa_supplicant -B -iwlan0 -c/etc/wpa_supplicant.conf -Dnl80211; dhclient wlan0
[CST上午10时56分22秒] Xinliang Liu: drm/tests/modetest
[CST上午11时05分35秒] Xinliang Liu: https://builds.96boards.org/snapshots/hikey
[CST上午11时11分34秒] Xinliang Liu: hikey-jessie_alip_20150409-254.emmc.img
[CST上午11时32分50秒] Xinliang Liu: # CONFIG_DRM_HISI_FBDEV is not set
[CST上午11时41分42秒] Xinliang Liu: sudo mount -o loop,rw,sync boot-fat.emmc.img boot-fat && \

sudo cp hi6220-hikey.dtb boot-fat/lcb.dtb && \

sudo cp Image boot-fat && \

sudo sync && \

sudo umount boot-fat && \

fastboot flash boot boot-fat.emmc.img && fastboot reboot
[CST下午2时37分56秒] Xinliang Liu: hikey-jessie_developer_20150409-254.emmc.img
[CST下午2时38分11秒] Xinliang Liu: /home/liuxl/work/img/debian
[CST下午4时52分18秒] Xinliang Liu: $ modetest -M hisi -s 8:1024x768-60

modetest -M hisi -s 8:1600x900

modetest -M hisi -s 8:1280x720

modetest -M hisi -s 8:640x480
[CST下午4时52分23秒] Xinliang Liu: modetest -M hisi
[CST下午4时52分35秒] Xinliang Liu: modetest -h

[CST上午9时10分20秒] Xinliang Liu: simg2img $DEBIAN_DIR/$EMMC_IMAGE raw.img

sudo mount raw.img raw-files
[CST上午9时10分21秒] Xinliang Liu: sudo make_ext4fs -L rootfs -l 1500M -s $EMMC_IMAGE raw-files

sudo umount raw-files

