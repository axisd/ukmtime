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

echo "Install UkmTimeUp autorun Start script"
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc3.d/S98ukmtimeup
result

echo "Install UkmTimeUp autorun Stop scripts"
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc0.d/K02ukmtimeup
result
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc1.d/K02ukmtimeup
result
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc2.d/K02ukmtimeup
result
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc4.d/K02ukmtimeup
result
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc5.d/K02ukmtimeup
result

BASE_FOLDER="/usr/local/ukmtimeup"
ZONE_ARCH="zoneinfo.tar.gz"
TMP_DIR="tmpunpack"

mkdir $BASE_FOLDER/$TMP_DIR
tar -xzf $BASE_FOLDER/$ZONE_ARCH -C $BASE_FOLDER/$TMP_DIR
result

echo "Success"
