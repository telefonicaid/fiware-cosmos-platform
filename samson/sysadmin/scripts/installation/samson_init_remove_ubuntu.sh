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


function script_removal()
{

	if [[ -f ${INITD_FOLDER}/${INIT_FILE} ]] 
	then

		/bin/rm ${INITD_FOLDER}/${INIT_FILE}			
	
	fi

	if [[ -f ${INITD_FOLDER}/${INIT_FILE2} ]] 
	then
			
		/bin/rm ${INITD_FOLDER}/${INIT_FILE2}			
	
	fi

}

function link_removal()
{
	
	cd ${RC0_FOLDER} && [[ -f ${LINK_STOP} ]] && /bin/rm ${LINK_STOP}
	cd ${RC1_FOLDER} && [[ -f ${LINK_STOP} ]] && /bin/rm ${LINK_STOP}
	
	cd ${RC2_FOLDER} && [[ -f ${LINK_START} ]] && [[ -f ${LINK_START2} ]] && /bin/rm ${LINK_START} ${LINK_START2}
	cd ${RC3_FOLDER} && [[ -f ${LINK_START} ]] && [[ -f ${LINK_START2} ]] && /bin/rm ${LINK_START} ${LINK_START2}
	cd ${RC4_FOLDER} && [[ -f ${LINK_START} ]] && [[ -f ${LINK_START2} ]] && /bin/rm ${LINK_START} ${LINK_START2}
	cd ${RC5_FOLDER} && [[ -f ${LINK_START} ]] && [[ -f ${LINK_START2} ]] && /bin/rm ${LINK_START} ${LINK_START2}
	
	cd ${RC6_FOLDER} && [[ -f ${LINK_STOP} ]] && /bin/rm ${LINK_STOP}

}


# ---- Main process --------

if [[ ${LINUX_UBUNTU} -eq 1 ]]
then

	echo "Performing samson init script removal..."
	
	link_removal
	
	script_removal

else

	echo "This is not Ubuntu distribution, exiting..."

fi
