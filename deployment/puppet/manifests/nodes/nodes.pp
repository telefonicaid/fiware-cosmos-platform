node default {
  include pdi_base
}

node 'cosmos-master' inherits default {
  include master
}

node 'cosmos-slave1' inherits default {
  include slave
}

node 'cosmos-slave2' inherits default {
  include slave
}
