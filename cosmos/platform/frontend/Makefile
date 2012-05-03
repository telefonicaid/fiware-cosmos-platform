CONFIG?=buildout.dev.cfg

all:
	@echo To bootstrap the buildout environment use "make env".
	@echo Config file buildout.dev.cfg will be used unless CONFIG environment
	@echo variable is defined.

env: src/cosmos/jobconf/cluster/gen-py bin/django

src/cosmos/jobconf/cluster/gen-py: ../cluster/server/src/main/thrift/cluster.thrift
	thrift -r --gen py:new_style -o src/cosmos/jobconf/cluster/ \
	    ../cluster/server/src/main/thrift/cluster.thrift

bin/django: bin/buildout
	bin/buildout -c $(CONFIG)

bin/buildout: $(CONFIG)
	./bootstrap.py -c $(CONFIG)

distclean:
	rm -rf bin build develop-eggs dist downloads eggs parts src/cosmos.egg-info/ var

.PHONY: dev prod all distclean
