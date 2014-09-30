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

echo "Uninstall UkmTimeUp autorun Start script"
rm -f /etc/rc3.d/S98ukmtimeup
result

echo "Uninstall UkmTimeUp autorun Stop scripts"
rm -f /etc/rc0.d/K02ukmtimeup
result
rm -f /etc/rc1.d/K02ukmtimeup
result
rm -f /etc/rc2.d/K02ukmtimeup
result
rm -f /etc/rc4.d/K02ukmtimeup
result
rm -f /etc/rc5.d/K02ukmtimeup
result

echo "Success"
