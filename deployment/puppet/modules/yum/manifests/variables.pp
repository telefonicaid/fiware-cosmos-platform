class yum::variables {
  # Older versions of facter ( <1.7.1 ) don't come with $::operatingsystemmajrelease...
  $os_release_parts = split($operatingsystemrelease, '[.]')
  $operatingsystemmajrelease = $os_release_parts[0]
}
