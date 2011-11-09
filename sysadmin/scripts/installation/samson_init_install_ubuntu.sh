#! /bin/bash

LINUX_UBUNTU=$( /bin/uname -a | /bin/grep 'Ubuntu' | /usr/bin/wc -l )

INIT_FILE="samson"
INIT_FILE2="samson_presets"

LINK_START="S99samson"
LINK_STOP="S10samson" # On other distros, it starts with K !!
LINK_START2="S98samson_presets"

INITD_FOLDER="/etc/init.d"

RC0_FOLDER="/etc/rc0.d"
RC1_FOLDER="/etc/rc1.d"
RC2_FOLDER="/etc/rc2.d"
RC3_FOLDER="/etc/rc3.d"
RC4_FOLDER="/etc/rc4.d"
RC5_FOLDER="/etc/rc5.d"
RC6_FOLDER="/etc/rc6.d"

USER="samson"
IS_USER=$( /bin/cat /etc/passwd | grep "^${USER}" | wc -l )

function script_creation()
{

	if [[ ! -f ${INITD_FOLDER}/${INIT_FILE} ]] 
	then
			
		/usr/bin/touch ${INITD_FOLDER}/${INIT_FILE}
		cat - > ${INITD_FOLDER}/${INIT_FILE} <<\EndOfScript
#! /bin/bash

#
# ---- sysadmin : init.d script for SAMSON ( samsonSpawner daemon )
#

PARAM=$1

case $PARAM in

        'start')

                /usr/bin/sudo -u samson -i samsonSpawner
                ;;

        'stop')

                /usr/bin/sudo -u samson -i samsonKiller
                ;;

        'restart')

                /usr/bin/sudo -u samson -i samsonKiller
                /usr/bin/sudo -u samson -i samsonSpawner
                ;;
esac

EndOfScript
	
	fi


	if [[ ! -f ${INITD_FOLDER}/${INIT_FILE2} ]] 
	then
			
		/usr/bin/touch ${INITD_FOLDER}/${INIT_FILE2}
		cat - > ${INITD_FOLDER}/${INIT_FILE2} <<\EndOfScript
#! /bin/bash

#
# ---- sysadmin : init.d script for SAMSON needed PRESETS 
#

/sbin/sysctl -w kernel.shmmax=270000000
EndOfScript
	
	fi

}

function link_creation()
{

	cd ${RC0_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_STOP}

	cd ${RC1_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_STOP}
	
	cd ${RC2_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_START}
	/bin/ln -s ../init.d/${INIT_FILE2} ${LINK_START2}

	cd ${RC3_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_START}
	/bin/ln -s ../init.d/${INIT_FILE2} ${LINK_START2}
	
	cd ${RC4_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_START}
	/bin/ln -s ../init.d/${INIT_FILE2} ${LINK_START2}
	
	cd ${RC5_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_START}
	/bin/ln -s ../init.d/${INIT_FILE2} ${LINK_START2}

	cd ${RC6_FOLDER}
	/bin/ln -s ../init.d/${INIT_FILE} ${LINK_STOP}

}

function user_add()
{


	/usr/sbin/adduser --quiet --home /home/${USER} --shell /bin/bash ${USER} <<EOF
${USER}
${USER}
Samson user
no_room
1
1
no_desc
y
EOF
	

}

function permission_settings()
{

	/bin/chmod 755 ${INITD_FOLDER}/${INIT_FILE}
	/bin/chmod 755 ${INITD_FOLDER}/${INIT_FILE2}

}


# ---- Main process --------

if [[ ${LINUX_UBUNTU} -eq 1 ]]
then

	echo "Performing samson init script install..."
	
	script_creation

	link_creation

	[[ ${IS_USER} -eq 1 ]] || user_add

	permission_settings

else

	echo "This is not Ubuntu distribution, exiting..."

fi
