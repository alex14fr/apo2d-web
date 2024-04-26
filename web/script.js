"use strict";

var pyo;
var ouvrBtn, fileIn, ifrm;
var nready=0;

function btnOuvrir(ev) {
	fileIn.click();
}

async function loadXml(ev) {
	var file=fileIn.files[0];
	worker.postMessage(await(file.text()));
}

function debloq() {
	if(nready>=2) {
		document.getElementById("chargement").style.display="none";
		document.querySelector("form").style.display="block";
		ifrm.style.display="block";
		wResize();
	}
}

function winMessage(ev) {
	var evd=JSON.parse(ev.data);
	if(evd.event=="init") {
		nready++;
		debloq();
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

function workMsg(ev) {
	if(ev.data=="ready") {
		nready++;
		debloq();
	} else {
		ifrm.contentWindow.postMessage(ev.data, "*");
	}
}

var worker=new Worker("worker.js");
worker.addEventListener("message", workMsg);
ouvrBtn=document.getElementById("ouvrir");
ouvrBtn.addEventListener("click", btnOuvrir);
fileIn=document.querySelector("input[type=file]");
fileIn.addEventListener("change", loadXml);
ifrm=document.querySelector("iframe");
window.addEventListener("message", winMessage);
window.addEventListener("resize", wResize);
