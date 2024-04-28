<?php
$f=tempnam("/tmp", "drawio").".html";
file_put_contents($f, "<!doctype html><style>*{border:0;padding:0;margin:0}</style><iframe src='https://embed.diagrams.net/?embed=1&proto=json&spin=1&noSaveBtn=0&noExitBtn=1'></iframe><script>\"use strict\";\nvar xmlstr=atob('".base64_encode(file_get_contents($argv[1]))."'); var ifrm=document.querySelector('iframe'); window.onmessage=function(ev) { ifrm.contentWindow.postMessage(JSON.stringify({'action':'load','xml':xmlstr}), '*'); window.onmessage=null; }; ifrm.style.width=(ifrm.parentElement.clientWidth-5)+'px'; ifrm.style.height=(window.innerHeight-5)+'px'; </script>");
system("/opt/firefox/bin/firefox $f");

