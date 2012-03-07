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
all_debug: core_modules_debug extra_modules_debug

core_modules:
	for module in $(CORE_MODULES); do \
		make -C core/$$module install; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

extra_modules:
	for module in $(EXTRA_MODULES); do \
		make -C extra/$$module install; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

core_modules_debug:
	for module in $(CORE_MODULES); do \
		make -C core/$$module install_debug; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

extra_modules_debug:
	for module in $(EXTRA_MODULES); do \
		make -C extra/$$module install_debug; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

#moduletemplate
