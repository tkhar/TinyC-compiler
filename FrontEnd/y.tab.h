#define INTEGER 257
#define id 258
#define WHILE 259
#define IF 260
#define EXTERN 261
#define INT 262
#define READ 263
#define PRINT 264
#define VOID 265
#define RETURN 266
#define IFX 267
#define ELSE 268
#define GE 269
#define LE 270
#define EQ 271
#define NE 272
#define UMINUS 273
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
	int iValue;					/* integer value*/
	char *varName; 			/* variable name*/
	vector <astNode *> *stmt_list; /* statement list*/
	astNode*	nodePointer;	/* node pointer*/
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
