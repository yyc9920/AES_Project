#!/usr/bin/env bash

FILE_TO_WRITE=u-boot.bin

if [ -z "${1}" -o "x${1}" = "x" ];
then
	echo " [*] ${0} <sdcard_nodename>"
	echo " [*} example : ${0} sdb"
	exit
fi

if [ "$(id -u)" != "0" ];
then
	echo " [!] no root permission"
	echo " [!] use sudo or su"
	exit
fi

TARGET_DEV=/dev/${1}

if [ ! -e ${TARGET_DEV} ];
then
	echo " [!] ${TARGET_DEV} not exists"
	exit
fi

if [ ! -b ${TARGET_DEV} ];
then
	echo " [!] ${TARGET_DEV} is not block device"
	exit
fi

if mount | grep ${TARGET_DEV} > /dev/null;
then
	echo " [!] ${TARGET_DEV} or it's partions are mounted"
	echo " [!] unmount and retry"
	exit
fi

if [ ! -e ${FILE_TO_WRITE} ];
then
	echo " [!] cannot find ${FILE_TO_WRITE}"
	echo " [!] build u-boot or check directory"
	exit
fi

echo " [*] This script will write u-boot.bin to ${TARGET_DEV}"
echo " [*] node info - `ls -l ${TARGET_DEV}`"

DEVICE_INFO=`lsblk -b |grep -w ${1}`
if [ -z "${DEVICE_INFO}" -o "x${DEVICE_INFO}" = "x" ];
then
	echo " [!] cannot query block device infomation"
	exit 1
fi

echo " [*] device info - ${DEVICE_INFO}"

while true;
do
	read -p ' [?] Are you sure? (y/n) ' ynanswer
	case ${ynanswer} in
		[Yy]* )
			break;;
		[Nn]* )
			exit;;
		* )
			echo " [!] Retry, answer with y or n";;
	esac
done

echo " [*] writing ${FILE_TO_WRITE} to ${TARGET_DEV}"

dd if=${FILE_TO_WRITE} of=${TARGET_DEV} bs=1K skip=1 seek=1 1> /dev/null 2> /dev/null
