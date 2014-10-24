#!/bin/sh

result()
{
	if [ $? -ne 0 ]; then
		echo "==============================="
		echo "$(date)"
		echo "ERROR"
		echo "==============================="
		exit 1
	fi
}



echo "==== $0 started $(date)"

. /tmp/pos_program_restart.sh

pos_program_detect
pos_program_stop

. /tmp/datetime.sh

echo "Current system time is: $(date)"

echo "Setting new system time to \"$NEWTIME\""
date  "+%Y/%m/%d %k:%M:%S" --set="$NEWTIME"
result


# Sync the system clock.
UTC=0

echo "Checking /etc/localtime"
ls -l /etc/localtime
echo

echo -n "Checking /etc/sysconfig/clock ... "
if [ -f /etc/sysconfig/clock ]; then
	echo "found"
	echo "=== /etc/sysconfig/clock ==== content ===="
	cat /etc/sysconfig/clock
	echo "=== /etc/sysconfig/clock ==== content ===="

  . /etc/sysconfig/clock

  # convert old style clock config to new values
  if [ "${CLOCKMODE}" = "GMT" ]; then
    UTC=true
  elif [ "${CLOCKMODE}" = "ARC" ]; then
     ARC=true
  fi
else
	echo "not found"
fi

CLOCKFLAGS="$CLOCKFLAGS --systohc"

case "$UTC" in
   yes|true)
	CLOCKFLAGS="$CLOCKFLAGS -u";
	;;
   no|false)
	CLOCKFLAGS="$CLOCKFLAGS --localtime";
	;;
esac

# ATTENTION! We need to to remove /etc/adjtime to force hwclock set hardware clock
echo "Removing /etc/adjtime"
rm -fv /etc/adjtime
result

echo "hwclock flags=$CLOCKFLAGS"
echo "Writing system time to hw clock"
/sbin/hwclock $CLOCKFLAGS
result

echo "System time is set to:  $(date)"

pos_program_start

echo "==== SUCCESS: $0 completed $(date)"

exit 0
