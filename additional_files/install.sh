#!/bin/sh

echo "Install UkmTimeUp autorun Start script"
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc.3/S98ukmtimeup

echo "Install UkmTimeUp autorun Stop scripts"
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc.0/K02ukmtimeup
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc.1/K02ukmtimeup
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc.2/K02ukmtimeup
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc.4/K02ukmtimeup
ln -s /usr/local/ukmtimeup/ukmtimeup.sh /etc/rc.5/K02ukmtimeup

echo "Success"
