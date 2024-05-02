"use strict";

var ouvrBtn, fileIn, gv, svg, gdiv;

function btnOuvrir(ev) {
	fileIn.click();
}

function loadXml(ev) {
	fileIn.files[0].text().then((f)=> { 
		FS.writeFile("tmp.xml", f);
		ccall("apo_parse_tmp");
		var gv=FS.readFile("tmp.gv", {"encoding":"utf8"});
		Viz.instance().then((viz)=> {
			var svg=viz.renderSVGElement(gv);
			wResize();
			gdiv.replaceChildren(svg);
			});
		});
}

function wResize(ev) {
	gdiv.style.width=(window.innerWidth-5)+'px';
}

gdiv=document.getElementById("gdiv");
ouvrBtn=document.getElementById("ouvrir");
ouvrBtn.addEventListener("click", btnOuvrir);
fileIn=document.querySelector("input[type=file]");
fileIn.addEventListener("change", loadXml);
window.addEventListener("resize", wResize);
