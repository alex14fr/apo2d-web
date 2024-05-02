#ifndef _INC_H
#define _INC_H
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <alloca.h>

#define errprintf(...) fprintf(stderr, __VA_ARGS__);

#define XML_USE_DEPTH_COUNT

typedef struct {
	char *buf;
	int pos;
	int posmax;
	int line;
#ifndef XML_USE_DEPTH_COUNT
	char *stack;
	int stackSz;
	char *stTop;
#else
	int depth;
#endif
} xmldoc_t;

typedef enum {
	POP_OK,
	POP_ERR,
} PopStat;

typedef enum {
	PUSH_OK,
	PUSH_ERR,
} PushStat;

typedef enum {
	CAT_OK,
	CAT_ERR,
} CatStat;

typedef enum {
	ENT_OK,
	ENT_ERR,
} EntStat;

typedef enum {
	VIS_OK,
	VIS_LAST,
	VIS_ERR,
} VisStat;

typedef enum {
	TOK_CONTENT,
	TOK_OPEN,
	TOK_CLOSE,
	TOK_OPCLO,
	TOK_END,
	TOK_ERR,
} TokenType;

void xmlInit(xmldoc_t *x, char *buf, int bufLen, char *stack, int stackSz);
void xmlSubInitBegin(xmldoc_t *xo, xmldoc_t *x);
void xmlSubInitEnd(xmldoc_t *xo, xmldoc_t *x);
TokenType xmlNextTok(xmldoc_t *x, char **v, int *lv);
TokenType xmlNextTokIgn(xmldoc_t *x, char **v, int *lv);
TokenType xmlNextTokIgnPeek(xmldoc_t *x);
#ifndef XML_USE_DEPTH_COUNT
PushStat xmlPush(xmldoc_t *x, char *v, int lv);
PopStat xmlPop(xmldoc_t *x, char *v, int vcap);
PopStat xmlPeek(xmldoc_t *x, char *v);
#endif
EntStat xmlEnter(xmldoc_t *x, char *v, int vcap);
VisStat xmlVisit(xmldoc_t *x, xmldoc_t *xsub);
void xmlStrcpy(xmldoc_t *x, char *v, int vcap);
void xmlEntcpy(xmldoc_t *x, char *v, int vcap);
void xmlEnterCk(xmldoc_t *x, char *expect);
void xmlCpynext(xmldoc_t *x, char *v, int vcap);
#endif

