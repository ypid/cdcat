
all: lupdate lrelease src/lang.qrc src/img/cdcat.icns
	$(MAKE) --directory src

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

src/img/cdcat.icns: src/img/cdcat_logo_1024x1024.png
	png2icns "$@" "$<"
