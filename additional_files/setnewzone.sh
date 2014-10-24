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
echo " 			Start setzone.sh 			"

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

echo " Backup /etc/localtime "
# На всякий случай делаем резервную копию файла с текущими настройками
if [ -f "/etc/localtime.2011_2" ]; then
	echo " /etc/localtime already backup. Remove old backup "
	rm -Rf /etc/localtime.2011_2
	result
	echo " Remove old backup success "
fi

mv /etc/localtime  /etc/localtime.2011_2
result
echo " Backup file is /etc/localtime.2011_2 "

echo " Backup zoneinfo files "
if [ -d "/usr/share/zoneinfo-posix.2011" ]; then
	echo " /usr/share/zoneinfo-posix already backup. Remove old backup "
	rm -Rf /usr/share/zoneinfo-posix.2011
	if [ $? -ne 0 ]; then
		echo " Remove old backup /usr/share/zoneinfo-posix fail "
	else
		echo " Remove old backup /usr/share/zoneinfo-posix success "
	fi
fi

echo " Move /usr/share/zoneinfo-posix to zoneinfo-posix.2011 "
# Перемещаем старую папку zoneinfo-posix
mv /usr/share/zoneinfo-posix /usr/share/zoneinfo-posix.2011
if [ $? -ne 0 ]; then
	echo " Move /usr/share/zoneinfo-posix fail "
else
	echo " Move /usr/share/zoneinfo-posix success "
fi


if [ -d "/usr/share/zoneinfo.2011" ]; then
	echo " /usr/share/zoneinfo already backup. Remove old backup "
	rm -Rf /usr/share/zoneinfo.2011
	if [ $? -ne 0 ]; then
		echo " Remove old backup /usr/share/zoneinfo fail "
	else
		echo " Remove old backup /usr/share/zoneinfo success "
	fi	
fi

echo " Move /usr/share/zoneinfo to zoneinfo.2011 "
# Перемещаем старую папку zoneinfo
mv /usr/share/zoneinfo /usr/share/zoneinfo.2011
if [ $? -ne 0 ]; then
	echo " Move /usr/share/zoneinfo fail "
else
	echo " Move /usr/share/zoneinfo success "
fi

if [ -d "/usr/share/zoneinfo-leaps.2011" ]; then
	echo " /usr/share/zoneinfo-leaps already backup. Remove old backup "
	rm -Rf /usr/share/zoneinfo-leaps.2011
	if [ $? -ne 0 ]; then
		echo " Remove old backup /usr/share/zoneinfo-leaps fail "
	else
		echo " Remove old backup /usr/share/zoneinfo-leaps success "
	fi	
fi

echo " Move /usr/share/zoneinfo-leaps to zoneinfo-leaps.2011 "
# Перемещаем старую папку zoneinfo-leaps
mv /usr/share/zoneinfo-leaps /usr/share/zoneinfo-leaps.2011
if [ $? -ne 0 ]; then
	echo " Move /usr/share/zoneinfo-leaps fail "
else
	echo " Move /usr/share/zoneinfo-leaps success "
fi

echo " Copy new zoneinfo "
# Копируем новые файлы зон, утилиты, маны и библиотеку
BASE_FOLDER="/usr/local/ukmtimeup"

cp -Rf $BASE_FOLDER/tmpunpack/* /
result
echo " Copy new zoneinfo success"

echo " Create sym-link /usr/share/zoneinfo-posix "
# Вместо неё будет симлинк на папку zoneinfo
ln -sf /usr/share/zoneinfo /usr/share/zoneinfo-posix
result
echo " Create sym-link /usr/share/zoneinfo-posix success "

NEWZONE="$1"
echo " New timezone $NEWZONE "

echo " Create symlink fon nem timezone "
# Создаем символическую ссылку на нужный нам timezone: 
ln -sf /usr/share/zoneinfo/$NEWZONE /etc/localtime
result
echo `ls -l /etc/localtime`

# We need to to remove /etc/adjtime to force hwclock set hardware clock
echo " Removing /etc/adjtime "
rm -fv /etc/adjtime
result

echo "hwclock flags=$CLOCKFLAGS"
echo "Writing system time to hw clock"
/sbin/hwclock --systohc
result

echo "System time is set to:  `/sbin/hwclock --show`"

echo "==== SUCCESS: $0 completed $(date)"
echo "****************************************"

exit 0