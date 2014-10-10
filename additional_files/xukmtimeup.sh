#!/bin/bash

time="date +%y%m%d%H%M"	
basedir=/usr/local/ukmtimeup
logger -t xukmtimeup "$0: start() begin"
if [[ $(eval "$time") -ge 1410260200 ]]; then
	logger -t xukmtimeup "$0: Time to update zone"
	logger -t xukmtimeup "$0: Set correct current timezone"
	$basedir/curzone.sh >> /var/log/ukmtimeup.log
	if [ $? -ne 0 ]; then
		logger -t xukmtimeup "$0: Set correct current timezone fail"
	else
		logger -t xukmtimeup "$0: Set correct current timezone success"
		logger -t xukmtimeup "$0: Set new timezone"
		$basedir/newzone.sh >> /var/log/ukmtimeup.log
		if [ $? -ne 0 ]; then
			logger -t xukmtimeup "$0: Update zone fail"
		else
			logger -t xukmtimeup "$0: Update zone success"
			/usr/local/ukmtimeup/xuserdialog >> /var/log/ukmtimeup.log
			$basedir/uninstall.sh >> /var/log/ukmtimeup.log
		fi
	fi
else
	logger -t xukmtimeup "$0: Wrong time to update"
fi
logger -t xukmtimeup "$0: start() end"