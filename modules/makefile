CORE_MODULES=system \
			 txt \
			 example

EXTRA_MODULES=tienda \
		txt_md \
		snalight \
		urlbenchmark \
		webp \
		graph \
		terasort \
		wiki \
		url \
		hit \
		osn \
		prime \
		web \
		shop \
		gg \
		testcomparefunctions \
		mob2 \
		sna \
		cdr \
		passive_location \
		sort \
		twitter \
		testtime \
		mobility \
		page_rank \
		mob \
		level \
		simple_mobility

all: core_modules extra_modules

core_modules:
	for module in $(CORE_MODULES); do \
		make -C core/$$module install; \
	done

extra_modules:
	for module in $(EXTRA_MODULES); do \
		make -C extra/$$module install; \
	done

#moduletemplate
