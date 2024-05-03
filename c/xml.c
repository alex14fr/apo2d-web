#include "inc.h"
#include <stdlib.h>

#define ERR(msg) errprintf(msg "\n");

void xmlInit(xmldoc_t *x, char *buf, int bufLen, char *stack, int stackSz) {
	x->buf=buf;
	x->posmax=bufLen;
	x->pos=0;
	x->line=1;
#ifndef XML_USE_DEPTH_COUNT
	x->stack=x->stTop=stack;
	x->stackSz=stackSz;
#else
	x->depth=0;
#endif
}

void xmlSubInitBegin(xmldoc_t *xo, xmldoc_t *x) {
	x->buf=xo->buf;
	x->pos=x->posmax=xo->pos;
	x->line=xo->line;
#ifndef XML_USE_DEPTH_COUNT
	x->stack=x->stTop=xo->stTop;
	x->stackSz=xo->stackSz-(xo->stTop-xo->stack);
#else
	x->depth=xo->depth;
#endif
}

void xmlSubInitEnd(xmldoc_t *xo, xmldoc_t *x) {
	x->posmax=xo->pos;
}

static char xmlGetCh(xmldoc_t *x) {
	if(x->pos == x->posmax) return(0);
	else return(x->buf[x->pos++]);
}

static char xmlNextCh(xmldoc_t *x) {
	char c;
	do {
		c=xmlGetCh(x);
		if(c=='\n') x->line++;
	} while(c=='\n' || c=='\r' || c=='\t' || c==' ');
	return(c);
}

TokenType xmlNextTok(xmldoc_t *x, char **v, int *lv) {
	char c=xmlNextCh(x);
	TokenType ret;
	int pBeg, pEnd;

	if(c==0) {
		*lv=0;
		*v=x->buf+x->posmax-1;
		return(TOK_END);
	} else if(c=='<') {
		c=xmlNextCh(x);
		if(c=='/') {
			ret=TOK_CLOSE;
			xmlNextCh(x);
		} else {
			ret=TOK_OPEN;
		}
		pBeg=x->pos-1;
		while((c=xmlNextCh(x))!=0 && c!='>') ;
	} else {
		pBeg=x->pos-1;
		ret=TOK_CONTENT;
		while((c=xmlNextCh(x))!=0 && c!='<') ;
		if(c!=0) x->pos--;
	}
	pEnd=x->pos-1;
	if(ret==TOK_OPEN && x->buf[pEnd-1]=='/') {
		ret=TOK_OPCLO;
		pEnd--;
	}
	*v=x->buf+pBeg;
	*lv=pEnd-pBeg;
	return(ret);
}

TokenType xmlNextTokIgn(xmldoc_t *x, char **v, int *lv) {
	TokenType tt;
	do {
		tt=xmlNextTok(x, v, lv);
	} while(tt==TOK_OPEN && (**v=='?' || **v=='!'));
	return(tt);
}

TokenType xmlNextTokIgnPeek(xmldoc_t *x) {
	int opos=x->pos;
	char *w;
	int lw;
	TokenType tt=xmlNextTokIgn(x, &w, &lw);
	x->pos=opos;
	return(tt);
}

#ifndef XML_USE_DEPTH_COUNT
PushStat xmlPush(xmldoc_t *x, char *v, int lv) {
	if((x->stTop-x->stack)+lv > x->stackSz-1) return(PUSH_ERR);
	memcpy(x->stTop, v, lv);
	x->stTop += lv;
	*(x->stTop++)=(lv & 0xff);
	*(x->stTop++)=((lv >> 8) & 0xff);
	*(x->stTop++)=((lv >> 16) & 0xff);
	*(x->stTop++)=((lv >> 24) & 0xff);
	return(PUSH_OK);
}

PopStat xmlPeek(xmldoc_t *x, char *v, int vcap) {
	if(x->stTop==x->stack) { ERR("Empty stack") return(POP_ERR); }
	int len=(*(x->stTop-1) << 24) | (*(x->stTop-2) << 16) | (*(x->stTop-3) << 8) | (*(x->stTop-4));
	if(len>vcap) { fprintf(stderr, "Peek: destination overflow") return(POP_ERR); }
	memcpy(v, x->stTop-len-4, len);
	return(POP_OK);
}

PopStat xmlPop(xmldoc_t *x, char *v, int vcap) {
	if(x->stTop==x->stack) { ERR("Empty stack") return(POP_ERR); }
	int len=(*(x->stTop-1) << 24) | (*(x->stTop-2) << 16) | (*(x->stTop-3) << 8) | (*(x->stTop-4));
	if(len>vcap) { fprintf(stderr, "Pop: destination overflow") return(POP_ERR); }
	memcpy(v, x->stTop-len-4, len);
	x->stTop-=len+4;
	return(POP_OK);
}
#endif

EntStat xmlEnter(xmldoc_t *x, char *v, int vcap) {
	char *w;
	int wl;
	TokenType tt=xmlNextTokIgn(x, &w, &wl);
	if(tt!=TOK_OPEN) {
		errprintf("xmlEnter: Expected TOK_OPEN, got %d at input line %d\n", tt, x->line);
		return(ENT_ERR);
	}
#ifndef XML_USE_DEPTH_COUNT
	if(xmlPush(x, w, wl)==PUSH_ERR) {
		errprintf("xmlEnter: Push error at input line %d\n", x->line);
		return(ENT_ERR);
	}
#else
	x->depth++;
#endif
	if(wl>vcap-1) {
		errprintf("xmlEnter: destination overflow (got %d, required %d)\n", vcap-1, wl);
		exit(1);
		//return(ENT_ERR);
	}
	memcpy(v, w, wl);
	v[wl]=0;
	return(ENT_OK);
}

VisStat xmlVisit(xmldoc_t *x, xmldoc_t *xsub) {
	char *v;
	int vl;
	xmlSubInitBegin(x, xsub);

	while(1) {
		TokenType tt=xmlNextTokIgn(x, &v, &vl);
		if(tt==TOK_END || tt==TOK_ERR) {
			ERR("xmlVisit: Unexpected end of input or tokenizer error")
			return(VIS_ERR);
		} else if(tt==TOK_OPEN) {
#ifndef XML_USE_DEPTH_COUNT
			if(xmlPush(x, v)==PUSH_ERR) {
				ERR("xmlVisit: Unexpected push error")
				return(VIS_ERR);
			}
#else
			x->depth++;
#endif
		} else if(tt==TOK_CLOSE) {
#ifndef XML_USE_DEPTH_COUNT
			if(xmlPop(x, v2)==POP_ERR || strcasecmp(v, v2)!=0) {
				errprintf("xmlVisit: expected: %s, got %s at input line %d\n", v2, v, x->line);
				return(VIS_ERR);
			}
			if(x->stTop == xsub->stTop) {
				xmlSubInitEnd(x, xsub);
				return(VIS_OK);
			} else if(x->stTop < xsub->stTop) {
				return(VIS_LAST);
			}
#else
			x->depth--;
			if(x->depth == xsub->depth) {
				xmlSubInitEnd(x, xsub);
				return(VIS_OK);
			} else if(x->depth < xsub->depth) {
				return(VIS_LAST);
			}
#endif
		} else if(tt==TOK_CONTENT || tt==TOK_OPCLO) {
#ifndef XML_USE_DEPTH_COUNT
			if(x->stTop == xsub->stTop) 
#else
			if(x->depth == xsub->depth) 
#endif
				{
					xmlSubInitEnd(x, xsub);
					return(VIS_OK);
				}
		}
	}
}

void xmlDumpbuf(xmldoc_t *x) {
	for(int i=0; i<x->posmax-x->pos; i++) 
		putchar((x->buf+x->pos)[i]);
}

void xmlStrcpy(xmldoc_t *x, char *v, int vcap) {
	if(x->posmax-x->pos>vcap-1) {
		memcpy(v, "OFLW\0", 5);
		return;
	}
	memcpy(v, x->buf+x->pos, x->posmax-x->pos);
	v[x->posmax-x->pos]=0;
}

void xmlEntcpy(xmldoc_t *x, char *v, int vcap) {
	char *w=alloca(vcap);
	xmlEnter(x, w, vcap);
	xmldoc_t tmp;
	xmlVisit(x, &tmp);
	xmlStrcpy(&tmp, v, vcap);
}

void xmlEnterCk(xmldoc_t *x, char *expect) {
	int n=strlen(expect);
	char *w=alloca(n+10);
	EntStat es=xmlEnter(x, w, n+10);
	if(es==ENT_ERR) {
		errprintf("xmlEnterck: expected %s, got error, input line %d\n", expect, x->line);
		exit(1);
	}
	if(strcasecmp(w, expect)!=0) {
		errprintf("xmlEnterck: expected %s, got %s\n", expect, w);
		exit(1);
	}
}

void xmlCpynext(xmldoc_t *x, char *v, int vcap) {
	xmldoc_t tmp;
	xmlVisit(x, &tmp);
	if(vcap>0) {
		if(xmlNextTokIgnPeek(&tmp)==TOK_OPEN)
			xmlEntcpy(&tmp, v, vcap);
		else
			v[0]=0;
	}
}

#ifdef TEST
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char **argv) {
	//char buf[1024]="<root>te<!commentaire>xte</root>";
	char buf[1024]="<root>texte<elem1>aaaa</elem1><elem2><nest>kikoo</nest></elem2></root>";
	char xmlstack[512]={0,};
	char *v=alloca(64); 
	xmldoc_t x, xsub;
	xmlInit(&x, buf, strlen(buf), xmlstack, 512);
	EntStat es;
	es=xmlEnter(&x, v, 64);
	printf("xmlEnter --> %d, %s\n", es, v);
	VisStat vs;
	do {
		vs=xmlVisit(&x, &xsub);
		printf("xmlVisit --> %d, ", vs);
		xmlDumpbuf(&xsub);
		putchar('\n');
	} while(vs!=VIS_LAST && vs!=VIS_ERR);


	/*
	nr=read(STDIN_FILENO, buf, 1024);
	if(nr<0) { perror("read"); exit(1); }
	else if(nr==0) { exit(0); }

	TokenType tt;
	do {
		tt=xmlNextTokIgn(&x, v);
		printf("type=%d v=%s\n", tt, v);
	} while(tt!=TOK_ERR && tt!=TOK_END);
	*/
}
#endif


