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

SFILE="S98ukmtimeup"
KFILE="K02ukmtimeup"

echo "****************************************"
echo "Uninstall UkmTimeUp autorun Start script"
rm -f /etc/rc3.d/$SFILE
result
rm -f /etc/rc5.d/$SFILE
result

echo "Uninstall UkmTimeUp autorun Stop scripts"
rm -f /etc/rc0.d/$KFILE
result
rm -f /etc/rc1.d/$KFILE
result
rm -f /etc/rc2.d/$KFILE
result
rm -f /etc/rc4.d/$KFILE
result

echo "Success"
echo "****************************************"
