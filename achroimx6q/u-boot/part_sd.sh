#!/usr/bin/env bash

# in mibibytes
PART_SIZE_UBOOT=8
PART_SIZE_BOOT=32
PART_SIZE_RECOVERY=32
PART_SIZE_SYSTEM=512
PART_SIZE_CACHE=512
PART_SIZE_DEVICE=8
PART_SIZE_MISC=4

PART_SIZE_EXTEND=`expr ${PART_SIZE_SYSTEM} '+' ${PART_SIZE_CACHE} '+' ${PART_SIZE_DEVICE} '+' ${PART_SIZE_MISC}`
# in sector
PART_EXTEND_PADDING=2048


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

echo " [*] This script will part ${TARGET_DEV} and format partitions"
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

PART_UBOOT_START=2
PART_UBOOT_END=`expr ${PART_UBOOT_START} '+' '(' ${PART_SIZE_UBOOT} '*' '1048576' '/' '512' ')' '-' '1'`

PART_BOOT_START=`expr ${PART_UBOOT_END} '+' '1'`
PART_BOOT_END=`expr ${PART_BOOT_START} '+' '(' ${PART_SIZE_BOOT} '*' '1048576' '/' '512' ')' '-' '1'`

PART_RECOVERY_START=`expr ${PART_BOOT_END} '+' '1'`
PART_RECOVERY_END=`expr ${PART_RECOVERY_START} '+' '(' ${PART_SIZE_RECOVERY} '*' '1048576' '/' '512' ')' '-' '1'`

PART_EXTEND_START=`expr ${PART_RECOVERY_END} '+' '1'`
PART_EXTEND_END=`expr ${PART_EXTEND_START} '+' '(' ${PART_SIZE_EXTEND} '*' '1048576' '/' '512' ')' '-' '1'`
PART_EXTEND_END=`expr ${PART_EXTEND_END} '+' ${PART_EXTEND_PADDING} '*' '4'`

PART_SYSTEM_START=`expr ${PART_EXTEND_START} '+' ${PART_EXTEND_PADDING}`
PART_SYSTEM_END=`expr ${PART_SYSTEM_START} '+' '(' ${PART_SIZE_SYSTEM} '*' '1048576' '/' '512' ')' '-' '1'`

PART_CACHE_START=`expr ${PART_SYSTEM_END} '+' ${PART_EXTEND_PADDING} '+' 1`
PART_CACHE_END=`expr ${PART_CACHE_START} '+' '(' ${PART_SIZE_CACHE} '*' '1048576' '/' '512' ')' '-' '1'`

PART_DEVICE_START=`expr ${PART_CACHE_END} '+' ${PART_EXTEND_PADDING} '+' 1`
PART_DEVICE_END=`expr ${PART_DEVICE_START} '+' '(' ${PART_SIZE_DEVICE} '*' '1048576' '/' '512' ')' '-' '1'`

PART_MISC_START=`expr ${PART_DEVICE_END} '+' ${PART_EXTEND_PADDING} '+' 1`
PART_MISC_END=`expr ${PART_MISC_START} '+' '(' ${PART_SIZE_MISC} '*' '1048576' '/' '512' ')' '-' '1'`

PART_DATA_START=`expr ${PART_MISC_END} '+' '1'`
TARGET_SECTORS=$(expr `fdisk -s ${TARGET_DEV}` '*' '2')
PART_DATA_END=$(expr ${TARGET_SECTORS} '-' 1)

echo " [*] partition map ----------"
echo " [*] ${TARGET_DEV}1 BOOT $(expr 1 '+' ${PART_BOOT_END} '-' ${PART_BOOT_START}) blocks"
echo " [*] ${TARGET_DEV}2 RESERVED $(expr 1 '+' ${PART_RECOVERY_END} '-' ${PART_RECOVERY_START}) blocks"
echo " [*] ${TARGET_DEV}3 EXTENDED $(expr 1 '+' ${PART_EXTEND_END} '-' ${PART_EXTEND_START}) blocks"
echo " [*] ${TARGET_DEV}4 DATA $(expr 1 '+' ${PART_DATA_END} '-' ${PART_DATA_START}) blocks"
echo " [*] extended sub parts -----"
echo " [*] ${TARGET_DEV}5 SYSTEM $(expr 1 '+' ${PART_SYSTEM_END} '-' ${PART_SYSTEM_START}) blocks"
echo " [*] ${TARGET_DEV}6 CACHE $(expr 1 '+' ${PART_CACHE_END} '-' ${PART_CACHE_START}) blocks"
echo " [*] ${TARGET_DEV}7 DEVICE $(expr 1 '+' ${PART_DEVICE_END} '-' ${PART_DEVICE_START}) blocks"
echo " [*] ${TARGET_DEV}8 MISC $(expr 1 '+' ${PART_MISC_END} '-' ${PART_MISC_START}) blocks"

echo " [*] Partitioning ${TARGET_DEV}"

echo -e "o
n\np\n1\n${PART_BOOT_START}\n${PART_BOOT_END}
n\np\n2\n${PART_RECOVERY_START}\n${PART_RECOVERY_END}
n\ne\n3\n${PART_EXTEND_START}\n${PART_EXTEND_END}
n\nl\n${PART_SYSTEM_START}\n${PART_SYSTEM_END}
n\nl\n${PART_CACHE_START}\n${PART_CACHE_END}
n\nl\n${PART_DEVICE_START}\n${PART_DEVICE_END}
n\nl\n${PART_MISC_START}\n${PART_MISC_END}
n\np\n4\n${PART_DATA_START}\n${PART_DATA_END}
w\n" | fdisk ${TARGET_DEV} 1> /dev/null 2> /dev/null


echo " [*] Formatting ${TARGET_DEV}4"
mkfs.ext4 ${TARGET_DEV}4 -L ACHROIMX_DATA  1> /dev/null 2> /dev/null
echo " [*] Formatting ${TARGET_DEV}5"
mkfs.ext4 ${TARGET_DEV}5 -L ACHROIMX_SYSTEM  1> /dev/null 2> /dev/null
echo " [*] Formatting ${TARGET_DEV}6"
mkfs.ext4 ${TARGET_DEV}6 -L ACHROIMX_CACHE  1> /dev/null 2> /dev/null
echo " [*] Formatting ${TARGET_DEV}7"
mkfs.ext4 ${TARGET_DEV}7 -L ACHROIMX_DEVICE  1> /dev/null 2> /dev/null
