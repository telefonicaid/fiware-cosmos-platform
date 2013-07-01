node default {
  include pdi_base
}
notice("The environment: ${environment}")

node 'cosmos-master' inherits default {
  include cosmos::master
}

node 'cosmos-slave1' inherits default {
  include cosmos::slave
}

node 'cosmos-slave2' inherits default {
  include cosmos::slave
}
