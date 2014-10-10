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

XSFILE="998-xukmtimeup"

echo "****************************************"
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

echo "****************************************"
echo "Install XUkmTimeUp autorun Start script"
if [ -f "/usr/local/xorg/xorg-main/etc/X11/xinit/xinitrc.d/$XSFILE" ]; then
	echo " /usr/local/xorg/xorg-main/etc/X11/xinit/xinitrc.d/$XSFILE already exist "
else
	ln -s /usr/local/ukmtimeup/xukmtimeup.sh /usr/local/xorg/xorg-main/etc/X11/xinit/xinitrc.d/$XSFILE
	result
fi
echo "****************************************"
echo

BASE_FOLDER="/usr/local/ukmtimeup"
ZONE_ARCH="zoneinfo.tar.gz"
TMP_DIR="tmpunpack"

echo " UnTar $ZONE_ARCH "
mkdir $BASE_FOLDER/$TMP_DIR
tar -xzf $BASE_FOLDER/$ZONE_ARCH -C $BASE_FOLDER/$TMP_DIR
result

echo "Success"
echo "****************************************"
