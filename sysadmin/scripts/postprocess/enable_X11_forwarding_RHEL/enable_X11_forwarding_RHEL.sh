#! /bin/bash

SSHD_CONFIG_ROUTE="/etc/ssh/sshd_config"

# ---- Be sure that OpenSSH server is there

yum install -y openssh-server

# ---- By default it is not installed and is obligued

yum install -y xorg-x11-xauth

# ---- Moreover, /etc/ssh/sshd.conf needs to be updated as well

cat ${SSHD_CONFIG_ROUTE} | sed -e 's/^X11Forwarding no/X11Forwarding yes/' > ${SSHD_CONFIG_ROUTE}_alt

mv ${SSHD_CONFIG_ROUTE}_alt ${SSHD_CONFIG_ROUTE}

if [[ $( grep "^X11Forwarding yes" ${SSHD_CONFIG_ROUTE} | wc -l ) -eq 0 ]] 
then

	echo "X11Forwarding yes" >> ${SSHD_CONFIG_ROUTE}

fi

# ---- Just restart the daemon and OK

/etc/init.d/sshd restart


