"use strict";

var pyo;
var ouvrBtn, fileIn, ifrm;

function btnOuvrir(ev) {
	fileIn.click();
}

async function loadXml(ev) {
	var file=fileIn.files[0];
	pyo.FS.writeFile("tmp.xml", await(file.text()));
	pyo.runPython("apogee2drawio.make_diagram('tmp.xml','tmp2.xml')")
	var xmlstr=pyo.FS.readFile("tmp2.xml", {"encoding":"utf8"});
	var msgstr=JSON.stringify({"action":"load", "xml":xmlstr});
	ifrm.contentWindow.postMessage(msgstr, "*");
}
async function loadpy() {
	pyo=await loadPyodide();
	pyo.unpackArchive(await (await fetch("pkg.tgz")).arrayBuffer(), "tgz");
	pyo.runPython("import apogee2drawio");
}

function wMessage(ev) {
	var evd=JSON.parse(ev.data);
	if(evd.event=="init") {
		document.getElementById("chargement").style.display="none";
		document.querySelector("form").style.display="block";
		ifrm.style.display="block";
		wResize();
	} else if(evd.event=="save") {
		var tmp=URL.createObjectURL(new Blob([evd.xml]));
		var a=document.createElement("a");
		a.href=tmp;
		a.download=fileIn.files[0].name+".drawio";
		a.click();
		URL.revokeObjectURL(tmp);
	}
}

function wResize(ev) {
	ifrm.style.width=(ifrm.parentElement.clientWidth-5)+"px";
	ifrm.style.height=(window.innerHeight-document.querySelector("form").clientHeight-5)+"px";
}

ouvrBtn=document.getElementById("ouvrir");
ouvrBtn.addEventListener("click", btnOuvrir);
fileIn=document.querySelector("input[type=file]");
fileIn.addEventListener("change", loadXml);
ifrm=document.querySelector("iframe");
window.addEventListener("message", wMessage);
window.addEventListener("resize", wResize);
loadpy();
