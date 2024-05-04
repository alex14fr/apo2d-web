.PHONY: pkg.tgz deploy serve clean

all: pkg.tgz

ifndef NO_UPDATE
pkg.tgz:
	curl https://gitlab.dsi.universite-paris-saclay.fr/pavel.kalouguine/apogee2drawio/-/archive/main/apogee2drawio-main.tar.gz | tar zx && \
	cd apogee2drawio-main && \
	tar c apogee2drawio/__init__.py apogee2drawio/*er.py | gzip -9c > ../pkg.tgz
else
pkg.tgz:
endif

deploy: all
	rsync -avz index.html script.js worker.js pkg.tgz sasmath:wwwdoc/apogee2drawio/

serve: all
	python -mhttp.server

clean:
	rm -rf pkg.tgz apogee2drawio-main

