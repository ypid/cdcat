
## help {{{
.PHONY: list
# http://stackoverflow.com/a/26339924/2239985
list:
	@echo "This Makefile has the following targets:"
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | sed 's/^/    /'
## }}}

lupdate: src/cdcat.pro
	lupdate -locations relative "$<"

enforce-coding-style:
	git ls-files src | egrep '\.(cpp|h)$$' | xargs uncrustify --no-backup -c ./uncrustify.cfg
