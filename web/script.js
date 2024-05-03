"use strict";

var ouvrBtn, fileIn, fnam, ifrm;

function btnOuvrir(ev) {
	fileIn.click();
}

function loadXml(ev) {
	fnam=fileIn.files[0].name;
	fileIn.files[0].text().then((f)=> { 
		FS.writeFile("tmp.xml", f);
		if(typeof _apo_parse_tmp == "function") {
			_apo_parse_tmp();
		} else {
			ccall("apo_parse_tmp");
		}
		ifrm.contentDocument.documentElement.innerHTML='';
		["svgbtn","gvbtn","htmlbtn"].forEach((x)=>document.getElementById(x).style.display="none");
		var htm;
		if(FS.findObject("tmp.svg")) {
			htm=FS.readFile("tmp.svg", {"encoding":"utf8"});
			["svgbtn","gvbtn"].forEach((x)=>document.getElementById(x).style.display="initial");
		} else {
			htm=FS.readFile("tmp.html", {"encoding":"utf8"});
			document.getElementById("htmlbtn").style.display="initial";
		}
		ifrm.contentDocument.documentElement.innerHTML=htm;
		wResize();
	});
}

function wResize(ev) {
	ifrm.style.marginTop=(document.querySelector('form').clientHeight)+'px';
	ifrm.style.width=(ifrm.parentElement.clientWidth-5)+"px";
	ifrm.style.height=(window.innerHeight-document.querySelector("form").clientHeight-20)+"px";
	ifrm.style.display='initial';
}

function enreg(ext, type, ev) {
	var f=new Blob([FS.readFile("tmp."+ext, {"encoding":"utf8"})], {"type":type});
	lien(f, ext);
}

function lien(f, ext) {
	var a=document.createElement('a');
	var u=URL.createObjectURL(f);
	a.href=u;
	a.download=fnam+"."+ext;
	a.click();
	URL.revokeObjectURL(u);
}

ifrm=document.getElementById("f");
ouvrBtn=document.getElementById("ouvrir");
ouvrBtn.addEventListener("click", btnOuvrir);
fileIn=document.querySelector("input[type=file]");
fileIn.addEventListener("change", loadXml);
window.addEventListener("resize", wResize);
document.getElementById("svgbtn").addEventListener("click", (ev)=>enreg("svg", "image/svg+xml", ev));
document.getElementById("gvbtn").addEventListener("click", (ev)=>enreg("gv", "text/x-graphviz", ev));
document.getElementById("htmlbtn").addEventListener("click", (ev)=>enreg("html", "text/html", ev));

