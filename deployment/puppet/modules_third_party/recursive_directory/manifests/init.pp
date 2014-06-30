# == Class: recursive_directory
#                                                                                                                      
# Defined type to recursively create files from templates
#
# === Parameters
#
# Document parameters here.
#
# [*source_dir*]
#   The source module and folder within the source module's template dir
#   modulename/config would use modulename/templates/config as the
#   root directory to recursively create files from the templates
#   found in modulename/templates/config
#
# [*dest_dir*]
#   The destination for the interpolated files
#
# [*file_mode*]
#   The file mode for the created file resources
#   Default: '0600'
#
# [*owner*]
#   The file owner for the created files
#   Default: 'nobody'
#
# [*group*]
#   The file group for the created files
#   Default: 'nobody'
#
# === Examples
# class recursive_directory::test {
#     recursive_directory{'recursive_something':
#         source_dir => 'recursive_directory',
#         dest_dir  => '/tmp',
#         owner      => 'root',
#         group      => 'root'
#     }
# }
#
# === Authors
#
# Rob Tucker <rtucker@mozilla.com>
#
# === Copyright
#
# Copyright 2013 Rob Tucker here, unless otherwise noted.
#
define recursive_directory (
    $source_dir = undef,
    $dest_dir = undef,
    $file_mode = undef,
    $owner = 'nobody',
    $group = 'nobody',
    $dir_mode = undef
){
    if $source_dir and $dest_dir {
        $resources_to_create = recurse_directory(
            $source_dir,
            $dest_dir,
            $file_mode,
            $owner,
            $group,
            $dir_mode
            )
        notice($resources_to_create)
        create_resources('file', $resources_to_create)
    } else {
        fail("source_dir and dest_dir are required")
    }

}
