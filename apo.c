/*
 *  apo2d-web
    Copyright (C) 2024 <alexandre.janon@universite-paris-saclay.fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "inc.h"

static char *gbuf;
static char* itemMap;
static int itemMapIdx=0;
static int itemMapCap=0;

#define VCAP 128

char *naturesElts[]={"Semestre", "SEM", "U.E.", "UE", "U.F.", "UF", "Rés. étape", "RET", "Stage", "STG",
          "Parcours", "PAR", "Elt à choi", "ELC", "BCC", "BCC", "Année", "AN", "Bloc", "BLC",
          "Certificat", "CRT", "C.M.", "CM", "Compétence", "CMP", "Cursus", "CUR", "ECUE", "ECU",
          "Examen", "EXA", "Filière", "FIL", "Matière", "MAT", "Mémoire", "MEM", "Module", "MOD",
          "Niveau", "NIV", "option", "OPT", "Période", "PER", "Projet", "PRJ", "Section", "SEC",
          "T.D.", "TD", "T.P.", "TP", "UE  nonADD", "UEF", "UE sansnot", "USN", "U.V.", "UV",
			 "Obligatoire", "LO", "Obligatoire à choix", "LOX", "Facultative", "LF"};

#define SUSP "<FONT COLOR=\"gray\">"
#define SUSP2 "</FONT>"

char* natureElt(char *nat) {
	int n=sizeof(naturesElts)/(2*sizeof(char*));
	for(int i=0; i<n; i++) {
		if(strcasecmp(nat, naturesElts[2*i])==0)
			return naturesElts[2*i+1];
	}
	return(nat);
}

void itemMapAdd(char *item, char *nomListe, int pos) {
	char str[128];
	int len=snprintf(str, 128, "%s%c%s:L%d", item, 0, nomListe, pos);
	if(len<0) { errprintf("itemMapAdd: snprintf error\n"); exit(1); }
	while(itemMapIdx+len+2>itemMapCap) { 
		itemMap=realloc(itemMap, itemMapCap+8192);
		if(!itemMap) {
			errprintf("itemMapAdd: realloc failed\n");
			exit(1);
		}
		itemMapCap+=8192;
	}
	itemMap[itemMapIdx++]=len+1;
	memcpy(itemMap+itemMapIdx, str, len+1);
	itemMapIdx+=len+1;
}

void itemMapFind(char *item, char *pos) {
	int i=0;
	int n=strlen(item);
	while(i<itemMapIdx) {
		int len=itemMap[i++];
		if(memcmp(item, itemMap+i, n+1)==0) {
			memcpy(pos, itemMap+i+n+1, len-n-1);
			return;
		} else {
			i+=len;
		}
	}
	sprintf(pos, "UNKN");
}

void parseListGCodElpFils(xmldoc_t *x, char *nomListe, FILE *out) {
	char *descrFils=alloca(VCAP), *natureEl=alloca(VCAP), *suspendu=alloca(VCAP);
	char *codFils=alloca(VCAP);
	xmlEnterCk(x, "LIST_G_COD_ELP_FILS");
	xmldoc_t xGCodElpFils;
	xmldoc_t xCodElpFils, xEtaElpFils, xListGCodElp1;
	xmldoc_t xGCodElp1;
	xmldoc_t jnk, xLicElp1, xLicNel1, xTemSusElp1;
	int pos=0;
	while(1) {
		VisStat vs=xmlVisit(x, &xGCodElpFils);
		if(vs==VIS_LAST) break;
		xmlEnterCk(&xGCodElpFils, "G_COD_ELP_FILS");
		xmlVisit(&xGCodElpFils, &xCodElpFils);
		xmlVisit(&xGCodElpFils, &xEtaElpFils);
		xmlVisit(&xGCodElpFils, &xListGCodElp1);

		xmlEnterCk(&xListGCodElp1, "LIST_G_COD_ELP1");
		xmlVisit(&xListGCodElp1, &xGCodElp1);

		xmlEnterCk(&xGCodElp1, "G_COD_ELP1");
		xmlVisit(&xGCodElp1, &jnk); /* NBR_CRD_ELP */
		xmlVisit(&xGCodElp1, &jnk); /* COD_ELP1 */
		xmlVisit(&xGCodElp1, &xLicElp1);
		xmlVisit(&xGCodElp1, &xLicNel1);
		xmlVisit(&xGCodElp1, &xTemSusElp1);
		xmlEntcpy(&xLicElp1, descrFils, VCAP);
		xmlEntcpy(&xLicNel1, natureEl, VCAP);
		xmlEntcpy(&xTemSusElp1, suspendu, VCAP);

		xmlEntcpy(&xCodElpFils, codFils, VCAP);
		fprintf(out, "\t<TR><TD PORT=\"L%d\">%s%s %s : %s%s</TD></TR>\n", pos, (suspendu[0]=='N' ? "" : SUSP), codFils, natureElt(natureEl), descrFils, (suspendu[0]=='N' ? "" : SUSP2));
		itemMapAdd(codFils, nomListe, pos++);
	}
}

void markAllAsDead(xmldoc_t *x) {
	char *codFils=alloca(VCAP);
	xmlEnterCk(x, "LIST_G_COD_ELP_FILS");
	xmldoc_t xGCodElpFils;
	xmldoc_t xCodElpFils;
	while(1) {
		VisStat vs=xmlVisit(x, &xGCodElpFils);
		if(vs==VIS_LAST) break;
		xmlEnterCk(&xGCodElpFils, "G_COD_ELP_FILS");
		xmlVisit(&xGCodElpFils, &xCodElpFils);
		xmlEntcpy(&xCodElpFils, codFils, VCAP);
		itemMapAdd(codFils, "DEAD", 111);
	}
}

void parseListGCodElpPere1(xmldoc_t *x, int niv, FILE *out) {
	//fprintf(out, "# Niveau %d\n", niv);
	xmlEnterCk(x, "LIST_G_COD_ELP_PERE1");
	xmldoc_t xGCodElpPere1;
	xmldoc_t xCodElpPere1, xCodLse2, xListGCodElpFils, xListGCodLse1;
	xmldoc_t xGCodLse1;
	xmldoc_t xCodLse1, xLicLse1, xTypLse, jnk, xNbreMin, xNbreMax;
	char nomListe[VCAP], itemPere[VCAP];
	char *descrListe=alloca(VCAP), *natureEl=alloca(VCAP);
	char nmin[VCAP], nmax[VCAP];
	char minmaxStr[128];
	char locPere[128];
	while(1) {
		VisStat vs=xmlVisit(x, &xGCodElpPere1);
		if(vs==VIS_LAST) break;
		xmlEnterCk(&xGCodElpPere1, "G_COD_ELP_PERE1");
		xmlVisit(&xGCodElpPere1, &xCodElpPere1);
		xmlVisit(&xGCodElpPere1, &xCodLse2);
		xmlVisit(&xGCodElpPere1, &xListGCodElpFils);
		xmlVisit(&xGCodElpPere1, &xListGCodLse1);
		
		xmlEntcpy(&xCodElpPere1, itemPere, VCAP);
		xmlEntcpy(&xCodLse2, nomListe, VCAP);

		xmlEnterCk(&xListGCodLse1, "LIST_G_COD_LSE1");
		xmlVisit(&xListGCodLse1, &xGCodLse1);
		xmlEnterCk(&xGCodLse1, "G_COD_LSE1");
		xmlVisit(&xGCodLse1, &xCodLse1); /* COD_LSE1 */
		xmlVisit(&xGCodLse1, &xLicLse1);
		xmlVisit(&xGCodLse1, &xTypLse);
		xmlVisit(&xGCodLse1, &jnk); /* ETA_LSE1 */
		xmlVisit(&xGCodLse1, &jnk); /* COD_ELP3 */
		xmlVisit(&xGCodLse1, &xNbreMin);
		xmlVisit(&xGCodLse1, &xNbreMax);
		xmlEntcpy(&xLicLse1, descrListe, VCAP);
		xmlEntcpy(&xTypLse, natureEl, VCAP);
		minmaxStr[0]=0;
		if(xmlNextTokIgnPeek(&xNbreMin)==TOK_OPEN &&
			xmlNextTokIgnPeek(&xNbreMax)==TOK_OPEN) {
			xmlEntcpy(&xNbreMin, nmin, VCAP);
			xmlEntcpy(&xNbreMax, nmax, VCAP);
			if(strlen(nmin)>0 && strlen(nmax)>0)
				snprintf(minmaxStr, 128, " [label=\"%s - %s\"]", nmin, nmax);
		}


		itemMapFind(itemPere, locPere);
		if(memcmp(locPere, "DEAD:L111", 9)!=0) {
			//fprintf(out, "# Liste : %s\n# Item père : %s\n", nomListe, itemPere);
			fprintf(out, "%s [ label=<\n\t<TABLE BORDER=\"0\">\n\t<TR><TD><B>%s %s : %s</B></TD></TR>\n", nomListe, nomListe, natureElt(natureEl), descrListe);
			parseListGCodElpFils(&xListGCodElpFils, nomListe, out);
			fprintf(out, "\t</TABLE>\n\t>\n]\n");
			fprintf(out, "%s -> %s:n%s\n", locPere, nomListe, minmaxStr);
		} else {
			markAllAsDead(&xListGCodElpFils);
		}
	}
}

void parseApobuf(char *buf, int len, FILE *out) {
#ifndef XML_USE_DEPTH_COUNT
	char xmlstack[STSIZE];
#endif
	xmldoc_t x;
	xmldoc_t xListGNiveau, jnk, xListGCodDip;
	xmldoc_t xListGCodLse, xGCodLse, xListGCodElp, xGCodElp, xCodElp, xTemSusElp, xLicNel; 
	char temsus[16], codelp[VCAP], nomlp[VCAP], naturelp[VCAP];
	VisStat vs;

#ifndef XML_USE_DEPTH_COUNT
	xmlInit(&x, buf, len, xmlstack, STSIZE);
#else
	xmlInit(&x, buf, len, NULL, 0);
#endif
	xmlEnterCk(&x, "EEDDDR10");
	xmlVisit(&x, &xListGNiveau);
	xmlVisit(&x, &xListGCodDip); 
	xmlVisit(&x, &jnk); /* G_LIC_VDI1 */
	xmlVisit(&x, &jnk); /* C_NB_ENR */
	xmlVisit(&x, &jnk); /* C_NB_ENR_FILS */

	xmldoc_t xGCodDip, xCodDip, xCodVrsVdi, xLicVdi, xVrsVet, xLicEtp, xLicElp;
	xmlEnterCk(&xListGCodDip, "LIST_G_COD_DIP");
	xmlVisit(&xListGCodDip, &xGCodDip);
	xmlEnterCk(&xGCodDip, "G_COD_DIP");
	xmlVisit(&xGCodDip, &xCodDip);
	xmlVisit(&xGCodDip, &xCodVrsVdi);
	xmlVisit(&xGCodDip, &jnk); /* LIC_DIP */
	xmlVisit(&xGCodDip, &xLicVdi); 
	xmlVisit(&xGCodDip, &xVrsVet); 
	xmlVisit(&xGCodDip, &xLicEtp); /* LIC_ETP */
	xmlVisit(&xGCodDip, &xListGCodLse);

	char *codip=alloca(VCAP), *covdi=alloca(VCAP), *nomdip=alloca(VCAP), *nometp=alloca(VCAP), *covet=alloca(VCAP);
	xmlEntcpy(&xCodDip, codip, VCAP);
	xmlEntcpy(&xCodVrsVdi, covdi, VCAP);
	xmlEntcpy(&xLicVdi, nomdip, VCAP);
	xmlEntcpy(&xLicEtp, nometp, VCAP);
	xmlEntcpy(&xVrsVet, covet, VCAP);

	xmlEnterCk(&xListGCodLse, "LIST_G_COD_LSE");
	xmlVisit(&xListGCodLse, &xGCodLse);
	xmlEnterCk(&xGCodLse, "G_COD_LSE");
	xmldoc_t xCodLse, xLicLse, xTypLse1;
	char lcode[VCAP], lnom[VCAP], ltype[VCAP];
	xmlVisit(&xGCodLse, &xCodLse);
	xmlEntcpy(&xCodLse, lcode, VCAP);
	xmlVisit(&xGCodLse, &xLicLse);
	xmlEntcpy(&xLicLse, lnom, VCAP);
	xmlVisit(&xGCodLse, &xTypLse1);
	xmlEntcpy(&xTypLse1, ltype, VCAP);

	fprintf(out, "digraph { graph[rankdir=\"TB\"]; \nnode[fontname=Courier; fontsize=10; shape=box]; \nedge[fontname=Courier; fontsize=8; ]\n");

	fprintf(out, "root0 [ label=<<TABLE BORDER=\"0\"><TR><TD PORT=\"L0\" BGCOLOR=\"lightgray\"><B>%s DIP V%s : %s</B><BR/><B>ETP V%s : %s</B></TD></TR></TABLE>> ];\nroot0 -> root\n", codip, covdi, nomdip, covet, nometp);
	fprintf(out, "root [ label=<\n\t<TABLE BORDER=\"0\"><TR><TD><B>%s %s : %s</B></TD></TR>\n", lcode, natureElt(ltype), lnom);

	for(int i=0; i<3; i++) {
		xmlVisit(&xGCodLse, &jnk); /* NBR_MIN_ELP, NBR_MAX_ELP... */
	}
	xmlVisit(&xGCodLse, &xListGCodElp); /* LIST_G_COD_ELP */
	xmlEnterCk(&xListGCodElp, "LIST_G_COD_ELP");
	int pos=0;
	do {
		vs=xmlVisit(&xListGCodElp, &xGCodElp);
		if(vs==VIS_LAST) break;
		xmlEnterCk(&xGCodElp, "G_COD_ELP");
		xmlVisit(&xGCodElp, &jnk); /* NBR_CRD_ELP1 */
		xmlVisit(&xGCodElp, &xCodElp);
		xmlEntcpy(&xCodElp, codelp, VCAP);
		xmlVisit(&xGCodElp, &xLicElp); 
		xmlEntcpy(&xLicElp, nomlp, VCAP);
		xmlVisit(&xGCodElp, &xTemSusElp);
		xmlEntcpy(&xTemSusElp, temsus, 16);
		xmlVisit(&xGCodElp, &jnk); /* ETA_ELP */
		xmlVisit(&xGCodElp, &xLicNel);
		xmlEntcpy(&xLicNel, naturelp, VCAP);

		char suspB[32]="", suspE[8]="";
		if(temsus[0]=='O') {
			itemMapAdd(codelp, "DEAD", 111);
			memcpy(suspB, "<FONT COLOR=\"gray\">\0", 20);
			memcpy(suspE, "</FONT>\0", 8);
		} else {
			itemMapAdd(codelp, "root", pos);
		}

		fprintf(out, "\t<TR><TD PORT=\"L%d\">%s%s %s : %s%s</TD></TR>\n", pos, suspB, codelp, natureElt(naturelp), nomlp, suspE);
		pos++;
	} while(1);

	fprintf(out, "</TABLE>>]; \n");

	xmlEnterCk(&xListGNiveau, "LIST_G_NIVEAU");
	int niv=0;
	while(1) {
		xmldoc_t xGNiveau;
		vs=xmlVisit(&xListGNiveau, &xGNiveau);
		if(vs!=VIS_OK)
			break;
		xmlEnterCk(&xGNiveau, "G_NIVEAU");

		xmldoc_t xNiveau, xListGCodElpPere1;
		xmlVisit(&xGNiveau, &xNiveau);
		xmlVisit(&xGNiveau, &xListGCodElpPere1);

		parseListGCodElpPere1(&xListGCodElpPere1, niv++, out);
	} 
	fprintf(out, "}\n");
}

typedef enum {
	TYP_DDD,
	TYP_RGC,
} typeF;

typeF typeFic(char *buf, int len) {
	char *rech="<ECRGCR10>";
	int n=strlen(rech), nf=0;
	for(int i=0; i<128 && nf<n; i++) {
		if(rech[nf]==gbuf[i]) nf++;
		else nf=0;
	}
	if(nf==n) return TYP_RGC;
	else return TYP_DDD;
}

void doFic(char *nomx, FILE *out) {
	char *nom;
	if(!nomx) {
		nom=alloca(8);
		memcpy(nom, "tmp.xml\0", 8);
	} else {
		nom=nomx;
	}
	int fd=open(nom, O_RDONLY);
	if(fd<0) { perror("open"); exit(1); }
	struct stat sb;
	fstat(fd, &sb);
	gbuf=mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(gbuf==MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	typeF tf=typeFic(gbuf, sb.st_size);
	if(tf==TYP_DDD) {
		if(!out) out=fopen("tmp.gv", "w+");
		itemMapCap=16536;
		itemMapIdx=0;
		if(itemMap) free(itemMap);
		itemMap=malloc(itemMapCap);
		if(!itemMap) { perror("malloc itemMap"); exit(1); }
		parseApobuf(gbuf, sb.st_size, out);
		free(itemMap);
		fflush(out);
#ifdef __EMSCRIPTEN__
		void gviz_tmp(void);
		gviz_tmp();
#endif
	} else {
		if(!out) out=fopen("tmp.html", "w+");
		parseRgc(gbuf, sb.st_size, out);
		fflush(out);
	}
	munmap(gbuf, sb.st_size);
}

#if !defined(__EMSCRIPTEN__)
int main(int argc, char **argv) {
	if(argc<2) { printf("Usage: %s <xml-apogee>\n\n<xml-apogee> est un fichier XML exporté depuis Apogée: \n\n - soit dans Structure des enseignements, Documentation/Edition de la \ndécomposition d'un diplôme, auquel cas la sortie est un diagramme GraphViz/dot; \n\n - soit dans Modalités de contrôle, Documentation/Edition du \ndétail des règles de calcul, auquel cas la sortie est un fichier HTML.\n\n ", argv[0]); exit(1); }
	doFic(argv[1], stdout);
	return(0);
}
#else
void apo_parse_tmp(void) {
	unlink("tmp.gv");
	unlink("tmp.html");
	unlink("tmp.svg");
	doFic(NULL, NULL);

}
#endif

