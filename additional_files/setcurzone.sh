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

echo "****************************************"
echo " 			Start setcurzone.sh 			"

echo " Save current time "
# сохранием текущие дату и время
CURDATE=`date +%Y/%m/%d`
CURTIME=`date +%H:%M:%S`
CURDATETIME="$CURDATE $CURTIME"
echo " Current time is $CURDATETIME "

echo " Save current timezone "
# Сохраняем текущую зону
LSLOCALTIME=`ls -l /etc/localtime`
CURZONE=${LSLOCALTIME#*info/}
echo " Current timezone is $CURZONE "

NEWZONE="$1"
echo " New timezone is $NEWZONE "

if [ "$NEWZONE" == "$CURZONE" ]; then
	echo " Zones does not change. Current zone $CURZONE, installed zone $NEWZONE "
	echo "==== SUCCESS: $0 completed $(date)"
	echo "****************************************"
	exit 0
fi

# Подключаем функции детектирования, остановки и запуска УКМ
. /usr/local/ukmtimeup/detectnreboot_func.sh

echo " Detect runnig ukmclient "
pos_program_detect
result
echo " Detect runnig ukmclient success "

echo " Stop ukmclient "
pos_program_stop
result
echo " Stop ukmclient success "

echo " Backup /etc/localtime "
# На всякий случай делаем резервную копию файла с текущими настройками
mv /etc/localtime  /etc/localtime.2011
result
echo " Backup file is /etc/localtime.2011 "

echo " Create symlink fon nem timezone "
# Создаем символическую ссылку на нужный нам timezone: 
ln -sf /usr/share/zoneinfo/$NEWZONE /etc/localtime
result
echo `ls -l /etc/localtime`

echo " Restore saved time "
# Восстанавливаем сохраненное время
date "+%Y/%m/%d %H:%M:%S" --set="$CURDATETIME"
result
echo " Current time `date` "

# We need to to remove /etc/adjtime to force hwclock set hardware clock
echo " Removing /etc/adjtime "
rm -fv /etc/adjtime
result

echo "hwclock flags=$CLOCKFLAGS"
echo "Writing system time to hw clock"
/sbin/hwclock --systohc
result

echo "System time is set to:  `/sbin/hwclock --show`"

echo " Start ukmclient "
pos_program_start
result
echo " Start ukmclient success "

echo "==== SUCCESS: $0 completed $(date)"
echo "****************************************"

exit 0