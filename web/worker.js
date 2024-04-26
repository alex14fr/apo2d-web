"use strict";
var pyo;
importScripts("https://cdn.jsdelivr.net/pyodide/v0.25.1/full/pyodide.js");

async function loadpy() {
	pyo=await loadPyodide();
	pyo.unpackArchive(await (await fetch("pkg.tgz")).arrayBuffer(), "tgz");
	pyo.runPython("import apogee2drawio");
	self.postMessage("ready");
}

function gotmsg(ev) {
	pyo.FS.writeFile("tmp.xml", ev.data);
	pyo.runPython("apogee2drawio.make_diagram('tmp.xml','tmp2.xml')")
	var xmlstr=pyo.FS.readFile("tmp2.xml", {"encoding":"utf8"});
	var msgstr=JSON.stringify({"action":"load", "xml":xmlstr});
	self.postMessage(msgstr);
}

loadpy();

self.addEventListener('message', gotmsg);

