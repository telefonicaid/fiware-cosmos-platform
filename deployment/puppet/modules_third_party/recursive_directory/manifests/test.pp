class recursive_directory::test {
    $dest_dir = '/tmp/blah'
    file {$dest_dir:
        ensure => directory,
        recurse => true,
        purge => true,
    }
    recursive_directory{'recursive_something':
        source_dir => 'recursive_directory',
        dest_dir  => $dest_dir,
        owner      => 'root',
        group      => 'root',
        require    => File[$dest_dir]
    }
}
