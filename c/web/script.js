"use strict";

var ouvrBtn, fileIn, gv, gdiv, iviz, fnam;

function btnOuvrir(ev) {
	fileIn.click();
}

function loadXml(ev) {
	fnam=fileIn.files[0].name;
	fileIn.files[0].text().then((f)=> { 
		FS.writeFile("tmp.xml", f);
		ccall("apo_parse_tmp");
		var gv=FS.readFile("tmp.gv", {"encoding":"utf8"});
		iviz.then((viz)=> {
			var svg=viz.renderSVGElement(gv);
			wResize();
			gdiv.replaceChildren(svg);
			document.getElementById("svgbtn").style.display="inline";
			document.getElementById("gvbtn").style.display="inline";
//			svgPanZoom(svg, {fit:1,center:1});
			});
		});
}

function wResize(ev) {
	gdiv.style.width=(window.innerWidth-5)+'px';
	gdiv.style.marginTop=(document.querySelector('form').clientHeight)+'px';
}

function enregGV(ev) {
	var f=new Blob([FS.readFile("tmp.gv", {"encoding":"utf8"})], {"type":"text/x-graphviz"});
	lien(f, "gv");
}

function enregSVG(ev) {
	var f=new Blob([gdiv.innerHTML], {"type": "image/svg"});
	lien(f, "svg");
}

function lien(f, ext) {
	var a=document.createElement('a');
	var u=URL.createObjectURL(f);
	a.href=u;
	a.download=fnam+"."+ext;
	a.click();
	URL.revokeObjectURL(u);
}

iviz=Viz.instance();
gdiv=document.getElementById("gdiv");
ouvrBtn=document.getElementById("ouvrir");
ouvrBtn.addEventListener("click", btnOuvrir);
fileIn=document.querySelector("input[type=file]");
fileIn.addEventListener("change", loadXml);
window.addEventListener("resize", wResize);
document.getElementById("svgbtn").addEventListener("click", enregSVG);
document.getElementById("gvbtn").addEventListener("click", enregGV);

