EXPATVER = 2.6.2
GVZVER = 11.0.0
EMCC = emcc
CFLAGS = -Oz -flto
EMLINKFLAGS = --no-entry -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['$$FS'] -sEXPORTED_FUNCTIONS=_apo_parse_tmp -sENVIRONMENT=web -sALLOW_MEMORY_GROWTH=1 -sINVOKE_RUN=0 -sWASM_BIGINT 
SRCS =  gviz.c ../apo.c ../rgc.c ../xml.c
NJ = $(shell nproc)

.PHONY: clean allclean deploy serve

all: apo.js

dest/.EXPAT_OK:
	test -f expat-$(EXPATVER).tar.xz || wget https://github.com/libexpat/libexpat/releases/download/R_$(subst .,_,$(EXPATVER))/expat-$(EXPATVER).tar.xz
	test -d expat-$(EXPATVER) || tar xf expat-$(EXPATVER).tar.xz
	cd expat-$(EXPATVER) && emconfigure ./configure --prefix=$(shell pwd)/dest --enable-static --disable-shared --without-xmlwf --without-examples --without-tests --without-docbook && make -j $(NJ) install
	touch $@

dest/.GVZ_OK:
	test -f graphviz-$(GVZVER).tar.xz || wget https://gitlab.com/api/v4/projects/4207231/packages/generic/graphviz-releases/$(GVZVER)/graphviz-$(GVZVER).tar.xz
	test -d graphviz-$(GVZVER) || tar xf graphviz-$(GVZVER).tar.xz
	cd graphviz-$(GVZVER) && EXPAT_CFLAGS=-I$(shell pwd)/dest/include EXPAT_LIBS="-L$(shell pwd)/dest/lib -lexpat" emconfigure ./configure --without-x --prefix=$(shell pwd)/dest --disable-ltdl --disable-shared --enable-static --without-swig --enable-go=no  --without-smyrna --without-gtk --without-digcola --without-ipsepcola --without-sfdp --without-gdk --without-freetype --without-perl --without-webp --without-pangocairo --without-ft2 --with-expat && for d in lib plugin; do make -j$(NJ) -C $$d install; done
	touch $@

apo.js: $(SRCS) dest/.EXPAT_OK dest/.GVZ_OK
	$(EMCC) $(CFLAGS) $(EMLINKFLAGS) -Idest/include -Ldest/lib -Ldest/lib/graphviz -o $@ $(SRCS) -lgvplugin_dot_layout -lgvplugin_core -lgvc -lpathplan -lcgraph -lxdot -lcdt -lexpat

clean:
	rm -f apo.js apo.wasm

sclean: clean
	rm -rf EXPAT_OK GVZ_OK dest/

serve: apo.js
	python -mhttp.server

deploy: apo.js
	rsync -avz apo.js apo.wasm index.html script.js sasmath:wwwdoc/apogee2dot/


