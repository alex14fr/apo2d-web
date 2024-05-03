#include <graphviz/gvc.h>
#include <graphviz/gvplugin.h>
#include <stddef.h>
#include <stdio.h>

extern gvplugin_installed_t gvplugin_dot_layout_LTX_library[], gvrender_svg_types[], gvdevice_svg_types[];

static gvplugin_api_t core_apis[] = {
	{ API_render, gvrender_svg_types },
	{ API_device, gvdevice_svg_types },
	{ 0, 0 },
};

static gvplugin_library_t core_lib = { "core", core_apis };

lt_symlist_t lt_preloaded_symbols[]={
  { "gvplugin_core_LTX_library", &core_lib },
  { "gvplugin_dot_layout_LTX_library", &gvplugin_dot_layout_LTX_library },
  { 0, 0 },
};

void gviz_tmp(void) {
  GVC_t *gvc=gvContextPlugins(lt_preloaded_symbols, 0);
  FILE *fpin, *fpout;
  fpin=fopen("tmp.gv", "r");
  fpout=fopen("tmp.svg", "w+");
  if(!fpin || !fpout) { perror("fopen"); return; }
  graph_t *g = agread(fpin, NULL);
  gvLayout(gvc, g, "dot");
  gvRender(gvc, g, "svg", fpout);
  gvFreeLayout(gvc, g);
  agclose(g);
  gvFreeContext(gvc);
}

