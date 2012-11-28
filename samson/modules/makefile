CORE_MODULES=system \
			 txt \
			 example \
			 shop

EXTRA_MODULES= webp \
		fiware_pos \
		wiki \
		level \
		hit \
		web \
		iwebp \
		gg \
		mobility \
		simple_mobility \
		passive_location \
		OTTstream 
#		pilotoOTT \

all: core_modules extra_modules
all_debug: core_modules_debug extra_modules_debug
reset: core_reset extra_reset

core_modules:
	for module in $(CORE_MODULES); do \
		make -C core/$$module release ; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
		make -C core/$$module install; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

extra_modules:
	for module in $(EXTRA_MODULES); do \
		make -C extra/$$module release ; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
		make -C extra/$$module install; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

core_modules_debug:
	for module in $(CORE_MODULES); do \
		make -C core/$$module debug ; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
		make -C core/$$module install_debug; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

extra_modules_debug:
	for module in $(EXTRA_MODULES); do \
		make -C extra/$$module debug ; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
		make -C extra/$$module install_debug; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

core_reset:
	for module in $(CORE_MODULES); do \
		make -C core/$$module reset; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

extra_reset:
	for module in $(EXTRA_MODULES); do \
		make -C extra/$$module reset; \
		if [ "$$?" -ne 0 ]; then \
			break; \
		fi; \
	done

#moduletemplate
