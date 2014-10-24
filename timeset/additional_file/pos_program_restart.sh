#!/bin/sh

#######################################################################################
#
# Script vars
#

DIR_BACKUP="/root/_backup_pre_60000_ukmclient_SF#6495_fix"
DIR_UKMCLIENT="/usr/local/ukmclient"

DIR_INIT_D="/etc/rc.d/init.d"
FILE_INIT_D_UKMCLIENT="$DIR_INIT_D/ukmclient"
FILE_INIT_D_LILLO="$DIR_INIT_D/lillo"
FILE_INIT_D_ZX11="$DIR_INIT_D/zx11"

FILE_UKMCLIENT="/usr/local/ukmclient"
FILE_LILLO="/usr/local/lillo"

#######################################################################################
#
# Script functions
#

result()
{
	if [ $? -ne 0 ]; then
		echo "ERROR"
		exit 1
	fi
}

pos_program_detect()
{
	local pos_program
	echo
	echo "pos_program_detect() begin"
	pos_program="$FILE_UKMCLIENT"
	echo -n "Checking $pos_program ... "
	if [ -x $pos_program ]; then
		echo "found"
		pos_program_cmd_start="$FILE_INIT_D_UKMCLIENT start"
		pos_program_cmd_stop="$FILE_INIT_D_UKMCLIENT stop"
		pos_program_name="ukmclient"
	else
		echo "not found"
		pos_program="$FILE_LILLO"
		echo -n "Checking $pos_program ... "
		if [ -x $pos_program ]; then
			echo "found"
			pos_program_cmd_start="$FILE_INIT_D_ZX11 start"
			pos_program_cmd_stop="$FILE_INIT_D_ZX11 stop"
			pos_program_name="lillo"
		else
			echo "not found"
			pos_program=""
			pos_program_cmd_start=""
			pos_program_cmd_stop=""
			pos_program=""
			pos_program_name=""
			echo "ERROR: POS program not found"
			exit 1
		fi
	fi
	echo "Detected POS program is $pos_program_name"
	echo "pos_program_cmd_start=$pos_program_cmd_start"
	echo "pos_program_cmd_stop=$pos_program_cmd_stop"
	echo "pos_program_name=$pos_program_name"
	echo "pos_program_detect() end"
	echo
}

pos_program_wait_to_finish()
{
	local pos_program=$1
	local pos_program_executing="y"
	
	echo "$(date) Waiting for $pos_program to finish"
	
	while [ "$pos_program_executing" == "y" ]; do
		ps -e | grep $pos_program | grep -v grep > /dev/null
		if [ $? -ne 0 ]; then
			pos_program_executing="n"
		else
			echo "$(date) $pos_program is still executing"
			sleep 5
		fi
	done
	echo "$pos_program is not executing"
}

pos_program_check()
{
	if [ -z "$pos_program_name" ]; then
		echo "ERROR: POS program not detected"
		exit 1
	fi
}


pos_program_stop()
{
	echo
	echo "pos_program_stop() begin"
	pos_program_check
	echo "Stopping POS program"
	$pos_program_cmd_stop
	pos_program_wait_to_finish $pos_program_name
	sleep 5
	echo "pos_program_stop() end"
	echo
}

pos_program_start()
{
	echo
	echo "pos_program_start() begin"
	pos_program_check
	echo "Starting POS program"
	$pos_program_cmd_start
	echo "pos_program_start() end"
	echo
}

pos_program_restart()
{
	pos_program_stop
	pos_program_start
}