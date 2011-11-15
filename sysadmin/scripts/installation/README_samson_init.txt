Sysadmin NOTES FOR system init scripts installing/removal
=========================================================

	There are two pair of scripts for each linux distribution, the first one will
	create and put the init scripts necessary to get samson running at the startup
	sequence. Also to have available the possibility of starting/stopping samson
	daemons at will ( /etc/init.d/samson ).

	The first one will be run at finishing samson installation stage.

	The second one, at the end of samson uninstall.

	ALSO MORE IMPORTANT: they're useful for putting into package generation ( deb, rpm ) or some kind of
	install script.

	Scripts available at the moment for:

		Ubuntu server :

			samson_init_install_ubuntu.sh
			samson_init_remove_ubuntu.sh

