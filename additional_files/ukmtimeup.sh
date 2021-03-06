#!/bin/sh

start()
{
	local time="date +%y%m%d%H%M"	
	local basedir=/usr/local/ukmtimeup
	logger -t ukmtimeup "$0: start() begin"
	if [[ $(eval "$time") -ge 1410260200 ]]; then
		logger -t ukmtimeup "$0: Time to update zone"
		logger -t ukmtimeup "$0: Set correct current timezone"
		$basedir/curzone.sh >> /var/log/ukmtimeup.log
		if [ $? -ne 0 ]; then
			logger -t ukmtimeup "$0: Set correct current timezone fail"
		else
			logger -t ukmtimeup "$0: Set correct current timezone success"
			logger -t ukmtimeup "$0: Set new timezone"
			$basedir/newzone.sh >> /var/log/ukmtimeup.log
			if [ $? -ne 0 ]; then
				logger -t ukmtimeup "$0: Update zone fail"
			else
				logger -t ukmtimeup "$0: Update zone success"
				openvt -s -c1 -f -w $basedir/userdialog.sh
				$basedir/uninstall.sh >> /var/log/ukmtimeup.log
			fi
		fi
	else
		logger -t ukmtimeup "$0: Wrong time to update"
	fi
	logger -t ukmtimeup "$0: start() end"
}


stop()
{
	logger -t ukmtimeup "$0: stop() begin"
	
	local pid_to_kill=`pidof -x -o $$ "ukmtimeup.sh"`
	kill $pid_to_kill > /dev/null 2>&1
	usleep 200000
	
	logger -t ukmtimeup "$0: stop() end"
}


restart()
{
	stop
	sleep 2
	start
}


case "$1" in
  start)
  	start
	;;
  stop)
  	stop
	;;
  restart)
  	restart
	;;
  *)
	echo $"Usage: $0 {start|stop|restart}"
	exit 1
esac

exit 0
