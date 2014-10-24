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

XSFILE="998-xukmtimeup.sh"

FILE_UKMCLIENT="/usr/local/ukmclient"
FILE_LILLO="/usr/local/lillo"

OLD_XORG_PATH="/etc/X11/xinit/xinitrc.d"
NEW_XORG_PATH="/usr/local/xorg/xorg-main/etc/X11/xinit/xinitrc.d"
XORG_PATH=""

if [ -x $FILE_UKMCLIENT ]; then
	echo "****************************************"
	echo "Find UKMCLIENT"
	echo "Install UkmTimeUp autorun Start script"
	if [ -f "/etc/rc3.d/$SFILE" ]; then
		echo " /etc/rc3.d/$SFILE already exist "
	else
		ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc3.d/$SFILE
		result
	fi

	echo "Install UkmTimeUp autorun Stop scripts"
	if [ -f "/etc/rc0.d/$KFILE" ]; then
		echo " /etc/rc0.d/$KFILE already exist "
	else
		ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc0.d/$KFILE
		result
	fi

	if [ -f "/etc/rc1.d/$KFILE" ]; then
		echo " /etc/rc1.d/$KFILE already exist "
	else
		ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc1.d/$KFILE
		result
	fi

	if [ -f "/etc/rc2.d/$KFILE" ]; then
		echo " /etc/rc2.d/$KFILE already exist "
	else
		ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc2.d/$KFILE
		result
	fi

	if [ -f "/etc/rc4.d/$KFILE" ]; then
		echo " /etc/rc4.d/$KFILE already exist "
	else
		ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc4.d/$KFILE
		result
	fi

	if [ -f "/etc/rc5.d/$KFILE" ]; then
		echo " /etc/rc5.d/$KFILE already exist "
	else
		ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc5.d/$KFILE
		result
	fi
	echo "****************************************"
	echo
else
	if [ -x $FILE_LILLO ]; then
		echo "****************************************"
		echo "Find LILLO"
		echo "Install XUkmTimeUp autorun Start script"
		echo "Check XORG"
		if [ -d $OLD_XORG_PATH ]; then
			echo "Find XORG at $OLD_XORG_PATH"
			XORG_PATH=$OLD_XORG_PATH
		else
			if [ -d $NEW_XORG_PATH ]; then
				echo "Find XORG at $NEW_XORG_PATH"
				XORG_PATH=$NEW_XORG_PATH
			else
				echo "==============================="
				echo "$(date)"
				echo "ERROR: XORG startup dir not found"
				echo "Find path: $OLD_XORG_PATH"
				echo "Find path: $NEW_XORG_PATH"
				echo "==============================="
				exit 1
			fi
		fi
		
		if [ -f "$XORG_PATH/$XSFILE" ]; then
			echo " $XORG_PATH/$XSFILE already exist "
		else
			ln -s /usr/local/ukmtimeup/xukmtimeup.sh $XORG_PATH/$XSFILE
			result
		fi
		echo "****************************************"
		echo
	else
		echo "==============================="
		echo "$(date)"
		echo "ERROR: POS program not found"
		echo "==============================="
		exit 1
	fi
fi

BASE_FOLDER="/usr/local/ukmtimeup"
ZONE_ARCH="zoneinfo.tar.gz"
TMP_DIR="tmpunpack"

echo " UnTar $ZONE_ARCH "
mkdir $BASE_FOLDER/$TMP_DIR
tar -xzf $BASE_FOLDER/$ZONE_ARCH -C $BASE_FOLDER/$TMP_DIR
result

echo "Success"
echo "****************************************"
