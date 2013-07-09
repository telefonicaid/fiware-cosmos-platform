class openvz::sysctl {
  # On Hardware Node we generally need
  # packet forwarding enabled and proxy arp disabled
  sysctl { 'net.ipv4.ip_forward': val => '1' }
  sysctl { 'net.ipv6.conf.default.forwarding': val => '1' }
  sysctl { 'net.ipv6.conf.all.forwarding': val => '1' }
  sysctl { 'net.ipv4.conf.default.proxy_arp': val => '0' }

  # Enables source route verification
  sysctl { 'net.ipv4.conf.all.rp_filter': val => '1' }

  # Enables the magic-sysrq key
  sysctl { 'kernel.sysrq': val => '1' }

  # We do not want all our interfaces to send redirects
  sysctl { 'net.ipv4.conf.default.send_redirects': val => '1' }
  sysctl { 'net.ipv4.conf.all.send_redirects': val => '0' }
}
