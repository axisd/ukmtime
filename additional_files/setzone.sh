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

echo " Start setzone.sh "

echo " Save current time "
# сохранием текущие дату и время
CURDATE=`date +%Y/%m/%d`
CURTIME=`date +%k:%M:%S`
CURDATETIME="$CURDATE $CURTIME"
echo " Current time is $CURDATETIME "

echo " Save current timezone "
# Сохраняем текущую зону
LSLOCALTIME=`ls -l /etc/localtime`
CURZONE=${LSLOCALTIME#*info/}
echo " Current timezone is $CURZONE "

echo " Backup /etc/localtime "
# На всякий случай делаем резервную копию файла с текущими настройками
mv /etc/localtime  /etc/localtime-old
result
echo " Backup file is /etc/localtime-old "

echo " Remove /usr/share/zoneinfo-posix "
# Перемещаем старую папку zoneinfo-posix
mv /usr/share/zoneinfo-posix /usr/share/zoneinfo-posix-old
result
echo " Remove /usr/share/zoneinfo-posix success "

echo " Create sym-link /usr/share/zoneinfo-posix "
# Вместо неё будет симлинк на паку zoneinfo
ln -s /usr/share/zoneinfo /usr/share/zoneinfo-posix
result
echo " Create sym-link /usr/share/zoneinfo-posix success "

echo " Copy new zoneinfo "
# Копируем новые файлы зон, утилиты, маны и библиотеку
BASE_FOLDER="/usr/local/ukmtimeup"

cp -Rf $BASE_FOLDER/tmpunpack/* /
result
echo " Copy new zoneinfo success"

NEWZONE="$1"
echo " New timezone $NEWZONE "

echo " Create symlink fon nem timezone "
# Создаем символическую ссылку на нужный нам timezone: 
ln -sf /usr/share/zoneinfo/$NEWZONE /etc/localtime
result
echo `ls -l /etc/localtime`

echo " Compare old timezone and new timezone "
# Проверяем, изменилась ли зона
if [[ "$NEWZONE" -ne "$CURZONE" ]]; then
	echo " Timezone was changed "
	# Зона изменилась
	echo " Reset time "
	# Восстанавливаем сохраненное время
	date "+%Y/%m/%d %k:%M:%S" --set="$CURDATETIME"
	result
	echo " Current time `date` "
fi

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

exit 0