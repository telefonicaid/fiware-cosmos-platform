ifndef SAMSON_HOME
SAMSON_HOME=/opt/samson
endif
MODULES = system txt txt_md example tienda cdr plot mobility mob mob2 passive_location graph sna snalight osn url urlbenchmark sort terasort testtime testcomparefunctions hit gg page_rank wiki simple_mobility web

.PHONY: all release reset clean
all:
	for module in $(MODULES); do \
	   (SAMSON_HOME=$(SAMSON_HOME) make -C $$module); \
	done

release: 
	for module in $(MODULES); do \
	   (SAMSON_HOME=$(SAMSON_HOME) make release -C $$module); \
	done

reset:
	for module in $(MODULES); do \
	   (SAMSON_HOME=$(SAMSON_HOME) make reset -C $$module); \
	done

install:
	for module in $(MODULES); do \
	   (SAMSON_HOME=$(SAMSON_HOME) make install -C $$module); \
	done

test:
	SAMSON_HOME=$(SAMSON_HOME) make test -C txt
	SAMSON_HOME=$(SAMSON_HOME) make test -C sort
	SAMSON_HOME=$(SAMSON_HOME) make test -C urlbenchmark

eclipse:
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C system
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C txt_md
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C passive_location
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C example
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C tienda
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C cdr
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C mob
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C mob2
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C graph
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C sna
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C snalight
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C osn
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C url
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C urlbenchmark
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C sort
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C terasort
	SAMSON_HOME=$(SAMSON_HOME) make eclipse -C testtime
