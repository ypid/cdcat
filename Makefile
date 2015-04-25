
## help {{{
.PHONY: list
# http://stackoverflow.com/a/26339924/2239985
list:
	@echo "This Makefile has the following targets:"
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | sed 's/^/    /'
## }}}

lupdate: src/cdcat.pro
	lupdate -locations relative "$<"

lrelease: src/cdcat.pro
	lrelease "$<"

enforce-coding-style:
	git ls-files src | egrep '\.(cpp|h)$$' | xargs uncrustify --no-backup -c ./uncrustify.cfg

src/lang.qrc: src/cdcat.pro
	@(echo '<!DOCTYPE RCC><RCC version="1.0">'; \
	echo "<qresource>"; \
	cd src; \
	for file in lang/*.qm; do \
		echo "    <file>$$file</file>"; \
	done; \
	echo "</qresource>"; \
	echo "</RCC>"; \
	) > "$@"
