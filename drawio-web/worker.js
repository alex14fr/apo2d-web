"use strict";
var pyo;
self.importScripts("https://cdn.jsdelivr.net/pyodide/v0.25.1/full/pyodide.js");

async function loadpy() {
	pyo=await loadPyodide();
	pyo.unpackArchive(await (await fetch("pkg.tgz")).arrayBuffer(), "tgz");
	pyo.runPython("import apogee2drawio; from apogee2drawio.diagram_writer import ShowOption");
	self.postMessage("ready");
}

function gotmsg(ev) {
	var doo=JSON.parse(ev.data);
	pyo.FS.writeFile("tmp.xml", doo.f);
	pyo.runPython("apogee2drawio.make_diagram('tmp.xml','tmp2.xml',ShowOption."+doo.sopt+")")
	var xmlstr=pyo.FS.readFile("tmp2.xml", {"encoding":"utf8"});
	var msgstr=JSON.stringify({"action":"load", "xml":xmlstr});
	self.postMessage(msgstr);
}

loadpy();

self.addEventListener('message', gotmsg);

