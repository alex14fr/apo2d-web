#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "inc.h"

#define VCAP 256


void parseListeManip(FILE *out, xmldoc_t *x) {
	fprintf(out, "<h5>Objets manipulés: </h5>\n<table border=1>\n<tr><td>Num.<td>Objet<td>Coefficient<td>Seuil\n");
	xmlEnterCk(x, "LIST_G_RGC_RGM");
	xmldoc_t xx;
	while(xmlVisit(x, &xx)==VIS_OK) {
		xmlEnterCk(&xx, "G_RGC_RGM");
		char num[VCAP], cod[VCAP], seuil[VCAP], coef[VCAP], libel[VCAP];
		xmlCpynext(&xx, num, VCAP);
		for(int i=0; i<2; i++) xmlCpynext(&xx, NULL, 0);
		xmlCpynext(&xx, cod, VCAP);
		for(int i=0; i<4; i++) xmlCpynext(&xx, NULL, 0);
		xmlCpynext(&xx, coef, VCAP);
		for(int i=0; i<2; i++) xmlCpynext(&xx, NULL, 0);
		xmlCpynext(&xx, seuil, VCAP);
		xmlCpynext(&xx, NULL, 0);
		xmlCpynext(&xx, libel, VCAP);
		fprintf(out, "<tr><td>%s<td>%s : %s<td align=center>%s<td>%s\n", num, cod, libel, coef, seuil);
	}
	fprintf(out, "</table>\n");
}

void parseGTRR(FILE *out, xmldoc_t *x) {
	char res[VCAP], nrosi[VCAP], cond[VCAP];
	xmlEnterCk(x, "G_TRR");
	xmlCpynext(x, NULL, 0);
	xmlCpynext(x, res, VCAP);
	xmlCpynext(x, nrosi, VCAP);
	xmlCpynext(x, cond, VCAP);
	fprintf(out, "<li> <tt><b>%s</b></tt> si <tt>%s</tt>\n", res, cond);
}

void parseRgc(char *buf, int len, FILE *out) {
	fprintf(out, "<!doctype html><html><style>table { border-collapse: collapse; width: 700px } tt { font-size: 140%c } td:nth-child(1), td:nth-child(3), td:nth-child(4) { width: 60px; text-align: center }</style>\n", '%');

	xmldoc_t x, xlistrgc, xrgc;

	xmlInit(&x, buf, len, NULL, 0);
	xmlEnterCk(&x, "ECRGCR10");
	xmlVisit(&x, &xlistrgc);
	xmlEnterCk(&xlistrgc, "LIST_G_RGC");
	while(xmlVisit(&xlistrgc, &xrgc)==VIS_OK) {
		char codObj[VCAP], vrsObj[VCAP], typCalcul[VCAP], anneeDebutS[VCAP], anneeFinS[VCAP], *libObj=alloca(VCAP), typObj[VCAP], numRegle[VCAP], resDef[VCAP], formuleNote[VCAP];
		xmlEnterCk(&xrgc, "G_RGC");
		xmlCpynext(&xrgc, NULL, 0);
		xmlCpynext(&xrgc, codObj, VCAP);
		xmlCpynext(&xrgc, vrsObj, VCAP);
		xmlCpynext(&xrgc, numRegle, VCAP);
		xmlCpynext(&xrgc, typCalcul, VCAP);
		xmlCpynext(&xrgc, NULL, 0);
		for(int i=0; i<3; i++) xmlCpynext(&xrgc, NULL, 0);
		xmlCpynext(&xrgc, anneeDebutS, VCAP);
		xmlCpynext(&xrgc, anneeFinS, VCAP);
		xmlCpynext(&xrgc, NULL, 0);
		xmlCpynext(&xrgc, resDef, VCAP);
		xmlCpynext(&xrgc, libObj, VCAP);
		xmlCpynext(&xrgc, NULL, 0); /* COD_ELP_RGC */
		xmlCpynext(&xrgc, typObj, VCAP);
		for(int i=0; i<3; i++) xmlCpynext(&xrgc, NULL, 0); /* LIC_NUM_OCC .. LON_FML */
		xmlCpynext(&xrgc, formuleNote, VCAP);
		for(int i=0; i<2; i++) xmlCpynext(&xrgc, NULL, 0); 

		while(*libObj && (*libObj!=':')) libObj++; 
		if(*libObj) libObj++;
		if(typCalcul[0]=='R') memcpy(typCalcul, "Résultat\0", 10);
		else memcpy(typCalcul, "Note\0", 5);
		if(strlen(vrsObj)>0)
			fprintf(out, "<h2>%s %s V%s %s - Règle %s</h2>\n", typCalcul, codObj, vrsObj, libObj, numRegle);
		else
			fprintf(out, "<h2>%s %s %s - Règle %s</h2>\n", typCalcul, codObj, libObj, numRegle);

		xmldoc_t xlistgrgcrgm;
		xmlVisit(&xrgc, &xlistgrgcrgm);
		parseListeManip(out, &xlistgrgcrgm);

		if(typCalcul[0]=='R') {
			xmldoc_t xlistgtrr, gtrr;
			xmlVisit(&xrgc, &xlistgtrr);
			xmlEnterCk(&xlistgtrr, "LIST_G_TRR");
			fprintf(out, "<h5>Résultat: </h5>\n<ul>\n");
			while(xmlVisit(&xlistgtrr, &gtrr)==VIS_OK) {
				parseGTRR(out, &gtrr);
			}
			fprintf(out, "<li>Sinon: <tt><b>%s</b></tt>\n</ul>\n", resDef);
		} else {
			fprintf(out, "<h5>Formule: </h5>\n<tt>%s</tt>", formuleNote);
		}
	}
}

