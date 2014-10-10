#!/bin/bash

clear

NEWDATE=`date +%Y/%m/%d`
NEWTIME=`date +%H:%M:%S`
LSLOCALTIME=`ls -l /etc/localtime`
CURZONE=${LSLOCALTIME#*info/}

echo "*********************" >> /var/log/ukmtimeup.log
echo " userdialog started " >> /var/log/ukmtimeup.log

setsysfont
loadkeys -c -s /usr/local/ukmtimeup/ukm-sbrf.map
setleds -D +num

echo
echo "-----------------------------------------"
echo "Внимание!"
echo "Обновлена информация о часовых поясах."
echo "Установлено время: $NEWDATE $NEWTIME"
echo "Установленный часовой пояс: $CURZONE"
echo "Если время верное, нажмите клавишу 1."
echo
echo "Если время неверное, нажмите клавишу 2"
echo "и вызовите системного администратора."
echo "-----------------------------------------"
echo
echo
echo
echo "-----------------------------------------"
echo "Внимание!"
echo "Обновлена информация о часовых поясах."
echo "Установлено время: $NEWDATE $NEWTIME"
echo "Установленный часовой пояс: $CURZONE"
echo "Если время верное, нажмите клавишу 1."
echo
echo "Если время неверное, нажмите клавишу 2"
echo "и вызовите системного администратора."
echo "-----------------------------------------"
echo

read -n1 answer

clear

if [[ $answer -eq 1 ]]; then
	echo " Pressed \"Correct time\" ($answer)" >> /var/log/ukmtimeup.log
	ukmon
	echo
	echo "-----------------------------------------"
	echo "Внимание!"
	echo "При запуске кассовой программы проверьте"
	echo "время на экране и на фискальном устройстве"
	echo "(снимите X отчёт)."
	echo "В случае несоответствия времени вызовите"
	echo "системного администратора."
	echo "Нажмите любую клавишу для продолжения."
	echo "-----------------------------------------"
	echo
	echo
	echo
	echo
	echo "-----------------------------------------"
	echo "Внимание!"
	echo "При запуске кассовой программы проверьте"
	echo "время на экране и на фискальном устройстве"
	echo "(снимите X отчёт)."
	echo "В случае несоответствия времени вызовите"
	echo "системного администратора."
	echo "Нажмите любую клавишу для продолжения."
	echo "-----------------------------------------"
	echo
	echo
	
	read -n1
else
	echo " Pressed \"Wrong time\" ($answer)" >> /var/log/ukmtimeup.log
	ukmoff
	echo
	echo "-----------------------------------------"
	echo "Внимание!"
	echo "После обновления информации о часовых поясах"
	echo "установлено неверное время."
	echo "Работа кассовой программы заблокирована."
	echo "Необходимо установить верное время."
	echo "Вызовите системного администратора."
	echo "Нажмите любую клавишу для продолжения."
	echo "-----------------------------------------"
	echo
	echo
	echo
	echo
	echo "-----------------------------------------"
	echo "Внимание!"
	echo "После обновления информации о часовых поясах"
	echo "установлено неверное время."
	echo "Работа кассовой программы заблокирована."
	echo "Необходимо установить верное время."
	echo "Вызовите системного администратора."
	echo "Нажмите любую клавишу для продолжения."
	echo "-----------------------------------------"
	echo
	echo
	
	read -n1
fi

. /etc/sysconfig/keyboard
loadkeys $KEYTABLE
setleds -D +num

echo " userdialog finished " >> /var/log/ukmtimeup.log
echo "*********************" >> /var/log/ukmtimeup.log