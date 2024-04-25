<?php
function convpad($in,$len=11) {
	$s=base_convert($in,10,8);
	$n=strlen($s);
	for($i=0;$i<$len-$n;$i++)
		$s="0".$s;
	return $s;
}
function tarhdr($name,$size,$mtime,$linkto=false,$mode="0000644",$uid="0000000",$gid="0000000") {
	$hdr=$name;
	for($i=strlen($hdr);$i<100;$i++)
		$hdr[$i]="\0";
	$hdr.=$mode."\0".$uid."\0".$gid."\0".convpad($size)."\0".convpad($mtime)."\0"."        ";
	for($i=strlen($hdr);$i<512;$i++)
		$hdr[$i]="\0";
	if($linkto) {
		$hdr=substr_replace($hdr,"2$linkto",156,1+strlen($linkto));
	}
	$cksum=0;
	for($i=0;$i<strlen($hdr);$i++)
		$cksum+=ord($hdr[$i]);
	$hdr=substr_replace($hdr,convpad($cksum,7)."\0",148,8);
	for($i=strlen($hdr);$i<512;$i++)
		$hdr.="\0";
	return $hdr;
}
function tarpad($nwritten) {
	if($nwritten===0) return;
	$pad="";
	for($i=0;$i<512-($nwritten % 512);$i++)
		$pad.="\0";
	return($pad);
}

function addF($source, $linkto=false, $mode="0000644") {
	$prefixdir="./";
	if(!$linkto) {
		$n=filesize($prefixdir.$source);
		print tarhdr($source, $n, 0, false, $mode);
		print file_get_contents($prefixdir.$source);
		print tarpad($n);
	} else {
		print tarhdr($source, 0, 0, $linkto, $mode);
	}
}

function addD($name, $mode="0000755") {
	print tarhdr($name, 0, 0, false, $mode);
}

function addStr($name, $content, $mode="0000644") {
	$n=strlen($content);
	print tarhdr($name, $n, 0, false, $mode);
	print $content;
	print tarpad($n);
}

foreach(["__init__", "apogee_parser", "data_filter", "diagram_maker", "diagram_writer"] as $f) {
	print addF("apogee2drawio/".$f.".py");
}

