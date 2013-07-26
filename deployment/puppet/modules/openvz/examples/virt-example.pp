class ovz-guest {
	virt { "ovz1":
		os_template => 'ubuntu-10.10-x86_64',
		ensure     => 'running',
		virt_type  => 'openvz',
		autoboot   => 'false',
		configfile => 'basic',
		ipaddr     => ['10.0.0.1'],
		features => ["nfs:on", "sit:off"],
		resources_parameters => ["NUMPTY=20:20", "NUMSIGINFO=255:255"],
	}

	virt { "ovz2":
		ctid       => 101,
		os_template => 'ubuntu-11.04-x86_64',
		ensure     => 'stopped',
		virt_type  => 'openvz',
		ve_root => '/home/a/root/$VEID',
		ve_private => '/home/a/private/$VEID',
		user => 'user:password',
		capability => ["chown:off"],
		devices => ["b:8:19:rw", "b:8:18:rw", "cdrom:rw"]
	}

}
