"use strict";

var ouvrBtn, fileIn, gv, gdiv, iviz, fnam, ifrm, mode;

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
		ifrm.contentDocument.documentElement.innerHTML=gdiv.innerHTML='';
		document.getElementById("svgbtn").style.display="none";
		document.getElementById("gvbtn").style.display="none";
		document.getElementById("htmlbtn").style.display="none";
		try {
			gv=FS.readFile("tmp.gv", {"encoding":"utf8"});
			mode=1;
			iviz.then((viz)=> {
					var svg=viz.renderSVGElement(gv);
					gdiv.replaceChildren(svg);
					wResize();
					document.getElementById("svgbtn").style.display="inline";
					document.getElementById("gvbtn").style.display="inline";
					//			svgPanZoom(svg, {fit:1,center:1});
				});
		} catch(e) {
			mode=2;
			var htm=FS.readFile("tmp.html", {"encoding":"utf8"});
			ifrm.contentDocument.documentElement.innerHTML=htm;
			document.getElementById("htmlbtn").style.display="inline";
			wResize();
		}
	});
}

function wResize(ev) {
	if(mode==1) {
		gdiv.style.width=(window.innerWidth-5)+'px';
		gdiv.style.marginTop=(document.querySelector('form').clientHeight)+'px';
	} else {
		ifrm.style.marginTop=(document.querySelector('form').clientHeight)+'px';
		ifrm.style.width=(ifrm.parentElement.clientWidth-5)+"px";
		ifrm.style.height=(window.innerHeight-document.querySelector("form").clientHeight-20)+"px";
	}
}

function enregGV(ev) {
	var f=new Blob([FS.readFile("tmp.gv", {"encoding":"utf8"})], {"type":"text/x-graphviz"});
	lien(f, "gv");
}

function enregSVG(ev) {
	var f=new Blob([gdiv.innerHTML], {"type": "image/svg"});
	lien(f, "svg");
}

function enregHTML(ev) {
	var f=new Blob([ifrm.contentDocument.documentElement.innerHTML], {"type": "text/html"});
	lien(f, "html");
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
iviz=Viz.instance();
gdiv=document.getElementById("gdiv");
ouvrBtn=document.getElementById("ouvrir");
ouvrBtn.addEventListener("click", btnOuvrir);
fileIn=document.querySelector("input[type=file]");
fileIn.addEventListener("change", loadXml);
window.addEventListener("resize", wResize);
document.getElementById("svgbtn").addEventListener("click", enregSVG);
document.getElementById("gvbtn").addEventListener("click", enregGV);
document.getElementById("htmlbtn").addEventListener("click", enregHTML);

