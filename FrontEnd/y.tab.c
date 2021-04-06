/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20190617

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 19 "minic.y"
	#include <stdio.h>
	#include <stdlib.h>
	#include <vector>
	#include <stack>
	#include "ast.h"

	#define ANSI_COLOR_RESET   "\x1b[0m"

	using namespace std;

	/* We are using a stack of symbol tables which we populate*/
	/* as we encounter the declaration of variables and which*/
	/* we use for the semantic analysis we perform.*/
	vector< vector<char *> > symbol_table_stack; 
	bool compilation_success = true;

	void yyerror (char *s); /* declaration of yyerror*/
	void astToLLVMIR (astNode *root); /* declaration of IR building function*/
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 39 "minic.y"
typedef union {
	int iValue;					/* integer value*/
	char *varName; 			/* variable name*/
	vector <astNode *> *stmt_list; /* statement list*/
	astNode*	nodePointer;	/* node pointer*/
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 54 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

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
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    1,    1,    2,    3,    3,    4,    4,    4,    4,
    4,    4,    4,    4,    4,   14,   15,   13,   13,   12,
    8,    8,    8,    9,    9,    9,    9,   10,   11,   11,
   11,   11,   11,   11,    7,    7,    5,    6,
};
static const YYINT yylen[] = {                            2,
    3,    7,    7,    6,    2,    0,    3,    4,    4,    5,
    7,    5,    3,    5,    5,    1,    1,    2,    1,    3,
    1,    1,    1,    3,    3,    3,    3,    2,    3,    3,
    3,    3,    3,    3,    1,    1,    1,    1,
};
static const YYINT yydefred[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    3,    2,    5,    0,   37,    0,    0,    0,    0,    0,
   16,    4,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   38,    0,   35,   36,    0,    0,   22,   23,
    0,    0,    7,    0,    0,    0,    0,    0,   18,   17,
   13,   28,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    8,    9,   24,
   25,   26,   27,   33,   32,   31,   34,   29,   30,   12,
    0,   15,   14,   20,    0,   11,
};
static const YYINT yydgoto[] = {                          2,
    3,   10,   20,   41,   45,   46,   47,   48,   49,   50,
   51,   58,   42,   34,   61,
};
static const YYINT yysindex[] = {                      -250,
 -252,    0, -250, -248, -247, -242,  -14,  -13, -236,    0,
 -232, -230,   -6,    2,    4, -215,  -11,  -10, -208,   10,
    0,    0,    0, -120,    0,   12,   17, -200,   19,   20,
    0,    0,    3, -120,  -40,  -40,    6, -197,  -40,  -43,
 -120,  -63,    0, -233,    0,    0,   -1,  -48,    0,    0,
   22,   25,    0,   26,   27,   29,   11,   13,    0,    0,
    0,    0, -233, -233, -233, -233,  -40,  -40,  -40,  -40,
  -40,  -40, -120, -120,   14,   15,   30,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -192,    0,    0,    0, -120,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   37,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -46,    0,    0,    0,    0,    0,  -41,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    1,    0,    0,    0,    0,    0,
};
static const YYINT yygindex[] = {                         0,
   77,    0,    0,  -20,  -18,    0,  -35,  -32,    0,    0,
   45,    0,   41,    0,    0,
};
#define YYTABLESIZE 267
static const YYINT yytable[] = {                         21,
   10,   44,   31,   32,   44,   33,   55,   57,   62,    4,
    1,   72,    5,   71,    7,   33,    8,   21,   21,    9,
   21,   13,   33,   43,   25,   11,   12,   80,   81,   82,
   83,   15,   14,   16,   84,   85,   86,   87,   88,   89,
   65,   63,   17,   64,   18,   66,   19,   21,   22,   23,
   24,   35,   90,   91,   33,   33,   36,   37,   38,   39,
   54,   60,   73,   40,   53,   74,   75,   76,   77,   78,
   94,   79,   92,   93,   96,   95,   33,    6,   19,    6,
   52,   59,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   10,    0,   10,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   25,   26,   27,
    0,   28,    0,   29,    0,   30,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   43,   25,    0,   43,   25,    0,   56,
   67,   68,   69,   70,    0,    0,    0,   21,   21,   21,
   21,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   10,   10,
   10,    0,   10,    0,   10,    0,   10,
};
static const YYINT yycheck[] = {                         41,
    0,   45,  123,   24,   45,   24,   39,   40,   44,  262,
  261,   60,  265,   62,  263,   34,  264,   59,   60,  262,
   62,  258,   41,  257,  258,   40,   40,   63,   64,   65,
   66,  262,  265,   40,   67,   68,   69,   70,   71,   72,
   42,   43,   41,   45,   41,   47,  262,   59,   59,  258,
   41,   40,   73,   74,   73,   74,   40,  258,   40,   40,
  258,  125,   41,   61,   59,   41,   41,   41,   40,   59,
   41,   59,   59,   59,   95,  268,   95,   41,  125,    3,
   36,   41,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  123,   -1,  125,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,  259,  260,
   -1,  262,   -1,  264,   -1,  266,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  257,  258,   -1,  257,  258,   -1,  263,
  269,  270,  271,  272,   -1,   -1,   -1,  269,  270,  271,
  272,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,  259,
  260,   -1,  262,   -1,  264,   -1,  266,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#define YYMAXTOKEN 273
#define YYUNDFTOKEN 291
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,"';'",
"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,"INTEGER","id","WHILE","IF","EXTERN","INT","READ","PRINT","VOID",
"RETURN","IFX","ELSE","GE","LE","EQ","NE","UMINUS",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : program",
"program : extern extern function",
"extern : EXTERN VOID PRINT '(' INT ')' ';'",
"extern : EXTERN INT READ '(' VOID ')' ';'",
"function : INT id '(' arg ')' stmt",
"arg : INT id",
"arg :",
"stmt : INT id ';'",
"stmt : var '=' expr ';'",
"stmt : var '=' read_call ';'",
"stmt : IF '(' r_expr ')' stmt",
"stmt : IF '(' r_expr ')' stmt ELSE stmt",
"stmt : WHILE '(' r_expr ')' stmt",
"stmt : block_begin block_stmt block_end",
"stmt : RETURN '(' expr ')' ';'",
"stmt : PRINT '(' id ')' ';'",
"block_begin : '{'",
"block_end : '}'",
"block_stmt : stmt block_stmt",
"block_stmt : stmt",
"read_call : READ '(' ')'",
"expr : term",
"expr : b_expr",
"expr : u_expr",
"b_expr : term '+' term",
"b_expr : term '-' term",
"b_expr : term '*' term",
"b_expr : term '/' term",
"u_expr : '-' term",
"r_expr : expr '>' expr",
"r_expr : expr '<' expr",
"r_expr : expr EQ expr",
"r_expr : expr LE expr",
"r_expr : expr GE expr",
"r_expr : expr NE expr",
"term : var",
"term : const",
"var : id",
"const : INTEGER",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 417 "minic.y"

/* prints out an error message to the console */
// TOOD: Add more functionality
void yyerror(char *s) {
	fprintf(stdout,"\nCompilation Error: %s\n",s);
}

int yylex (void);

int main (void) {
	yyparse();
	return 0;
}
#line 339 "y.tab.c"

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yym = 0;
    yyn = 0;
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        yychar = YYLEX;
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);

    switch (yyn)
    {
case 1:
#line 62 "minic.y"
	{ 
																		/* create program node by putting in the external function nodes
																		 * and the main function node */
																		yyval.nodePointer = createProg (yystack.l_mark[-2].nodePointer, yystack.l_mark[-1].nodePointer, yystack.l_mark[0].nodePointer);

																		if (compilation_success) {
																			printf ("\nPrinting Abstract Syntax Tree:\n");
																			printNode (yyval.nodePointer);
																			printf ("Compilation successful!\n");
																		}
																		else {
																			printf ("Compilation failed!\n");
																		}

																		/* time to build the LLVM IR!*/
																		astToLLVMIR (yyval.nodePointer);

																		/* Free the tree!*/
																		freeNode(yyval.nodePointer);
																		exit(0); 
														    		}
break;
case 2:
#line 86 "minic.y"
	{
																		/* create extern node.*/
																		yyval.nodePointer = createExtern ("print");
																	}
break;
case 3:
#line 90 "minic.y"
	{
																		/* create extern node.*/
																		yyval.nodePointer = createExtern ("read");
																	}
break;
case 4:
#line 97 "minic.y"
	{	 
																		/* create function node, providing the name, the parameter, if one
																		 * exists and the body */
																		yyval.nodePointer = createFunc (yystack.l_mark[-4].varName, yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer);
																	}
break;
case 5:
#line 105 "minic.y"
	{	
																		/* create a declaration node, just with the name of the variable
																		 * and start making the stack of symbol tables*/
																		/* Also, make the first symbol table and push the argument in it*/
																		vector<char *> first_symbol_table;
																		first_symbol_table.push_back(yystack.l_mark[0].varName);
																		symbol_table_stack.push_back(first_symbol_table);
																		yyval.nodePointer = createVar (yystack.l_mark[0].varName);
																	}
break;
case 6:
#line 114 "minic.y"
	{
																		/* There are no arguments to our function */
																		yyval.nodePointer = NULL;
																	}
break;
case 7:
#line 121 "minic.y"
	{
																		/* create declaration node with just the name of the variable
																			and continue with the stack of symbol tables */
																		/* Add $2 to the symbol table that is on top of our stack.*/
																		/* While doing that, detect if we have duplicate*/
																		/* declarations!*/
																		printf ("\nDeclaration: %s\n", yystack.l_mark[-1].varName);
																		vector<char *> &current_symbol_table = symbol_table_stack.back();
																		printf ("\nChecking if variable has been declared again\n");
																		for (vector<char *>::iterator it = current_symbol_table.begin();
																				it != current_symbol_table.end(); ++it) {
																				if (strcmp((*it), yystack.l_mark[-1].varName) == 0) {
																					printf ("\nSEMANTIC ERROR: Variables can only be declared once!\n");
																					/* freeing cannot happen here! We must return to the top*/
																					/* of the tree!*/
																					compilation_success = false;
																				}
																		}
																		printf ("Checking concluded\n");
																		current_symbol_table.push_back (yystack.l_mark[-1].varName);
																		printf("\n--------------\n");
																		printf("Now the symbol table stack looks like: \n");
																		for (int i=0; i<symbol_table_stack.size(); i++){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				printf("%s ",symbol_table_stack[i][j]);
																			}
																			printf("\n");
																		}
																		printf("------------\n");

																		yyval.nodePointer = createDecl (yystack.l_mark[-1].varName);
																	}
break;
case 8:
#line 153 "minic.y"
	{ 
																		/* create assignment node with the variable and the expression */
																		yyval.nodePointer = createAsgn (yystack.l_mark[-3].nodePointer, yystack.l_mark[-1].nodePointer);
																	}
break;
case 9:
#line 157 "minic.y"
	{
																		/* create assignment node with the variable and the call-node
																		 * created from the READ function. */
																		yyval.nodePointer = createAsgn (yystack.l_mark[-3].nodePointer, yystack.l_mark[-1].nodePointer);
																	}
break;
case 10:
#line 162 "minic.y"
	{
																		/* create an if-node pointing to the condition and the body with
																		 * else-body = NULL (happens by default) */
																		yyval.nodePointer = createIf (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer);
																	}
break;
case 11:
#line 167 "minic.y"
	{
																		/* create an if-node pointing to the condition, the main body and
																		 * the else-body */
																		yyval.nodePointer = createIf (yystack.l_mark[-4].nodePointer, yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer);
																	}
break;
case 12:
#line 172 "minic.y"
	{
																		/* create a while-node pointing to the condition and body */
																		yyval.nodePointer = createWhile (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer);
																	}
break;
case 13:
#line 176 "minic.y"
	{
																		/* create a block-statement node pointing to a vector of
																		 * statements that is returned by block_stmt ($2) */
																		/* Remove the last layer of the stack of symbol tables.*/
																		yyval.nodePointer = createBlock (yystack.l_mark[-1].stmt_list);
																	}
break;
case 14:
#line 182 "minic.y"
	{
																		/* create a return-statement node pointing to the expression being
																		 * returned. */
																		yyval.nodePointer = createRet (yystack.l_mark[-2].nodePointer);
																	}
break;
case 15:
#line 187 "minic.y"
	{
																		/* create a call-statement node pointing to the function name
																		 * (PRINT) and
																		 * to the VARIABLE being used as a parameter. */
																		/* We must also check if that variable has been declared!*/
																		astNode *printDecl = createDecl (yystack.l_mark[-2].varName);
																		bool hasBeenDeclared = false, stop = false;
																		printf("\nChecking if variable %s has been declared\n", yystack.l_mark[-2].varName);
																		for (int i=symbol_table_stack.size()-1; i>=0; i--){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				if (strcmp(symbol_table_stack[i][j], yystack.l_mark[-2].varName) == 0) {
																					hasBeenDeclared = true;
																					stop = true;
																					break;
																				}
																			}
																			if (stop) {
																				break;
																			}
																		}
																		printf("------- Check concluded -------\n");
																		if (!hasBeenDeclared) {
																			printf("\nSEMANTIC ERROR: variable %s has not been declared!\n",yystack.l_mark[-2].varName);
																			/* free-ing the memory cannot happen here! It must happen at*/
																			/* the end!*/
																			compilation_success = false;
																		}
																		if (printDecl == NULL)
																			yyerror ("\nError in the print() statement!\n");
																		yyval.nodePointer = createCall ("print", printDecl);
																	}
break;
case 16:
#line 221 "minic.y"
	{
																		/*
																		*	When a block statement begins, we must make a new symbol table
																		*	and populate it with the newly declared variables.
																		*/
																		vector <char *> new_symbol_table;
																		symbol_table_stack.push_back(new_symbol_table);
																		
																		/* print the stack of symbol tables*/
																		printf("\n--------------\n");
																		printf("Now the symbol table stack looks like: \n");
																		for (int i=0; i<symbol_table_stack.size(); i++){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				printf("%s ",symbol_table_stack[i][j]);
																			}
																			if (symbol_table_stack[i].size() == 0) {
																				printf("[]");
																			}
																			printf("\n");
																		}
																		printf("------------\n");
																	}
break;
case 17:
#line 246 "minic.y"
	{
																		/* when a block statement ends, we have to pop its respective*/
																		/* symbol table from our stack so that we can correctly keep track*/
																		/* of the scopes of our variables.*/
																		printf ("\nPopping from symbol table stack.\n");
																		symbol_table_stack.pop_back();

																		printf("--------------\n");
																		printf("Now the symbol table stack looks like: \n");
																		for (int i=0; i<symbol_table_stack.size(); i++){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				printf("%s ",symbol_table_stack[i][j]);
																			}
																			if (symbol_table_stack[i].size() == 0) {
																				printf("[]");
																			}
																			printf("\n");
																		}
																		printf("------------\n");
																	}
break;
case 18:
#line 269 "minic.y"
	{
																		/* populate the vector with the statement nodes */
																		(yystack.l_mark[0].stmt_list) -> push_back(yystack.l_mark[-1].nodePointer);
																		yyval.stmt_list = yystack.l_mark[0].stmt_list;
																	}
break;
case 19:
#line 274 "minic.y"
	{
																		/* initialize a vector of ast-nodes and put the first statement
																		 *	in it */
																		vector<astNode *> *list_of_statements = new vector<astNode *> ();
																		list_of_statements -> push_back(yystack.l_mark[0].nodePointer);
																		yyval.stmt_list = list_of_statements;
																	}
break;
case 20:
#line 284 "minic.y"
	{
																		/* create a call-statement node pointing to the function name
																		 * (READ) with a NULL parameter. */
																		yyval.nodePointer = createCall("read");
																	}
break;
case 21:
#line 292 "minic.y"
	{
																		/* just pass the node already created on */
																		yyval.nodePointer = yystack.l_mark[0].nodePointer;
																	}
break;
case 22:
#line 296 "minic.y"
	{
																		/* just pass on the node already created */
																		yyval.nodePointer = yystack.l_mark[0].nodePointer;
																	}
break;
case 23:
#line 300 "minic.y"
	{
																		/* just pass on the node already created */
																		yyval.nodePointer = yystack.l_mark[0].nodePointer;
																	}
break;
case 24:
#line 307 "minic.y"
	{	
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 yyval.nodePointer = createBExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, add);
																	}
break;
case 25:
#line 312 "minic.y"
	{	
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 yyval.nodePointer = createBExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, sub);
																	}
break;
case 26:
#line 317 "minic.y"
	{
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 yyval.nodePointer = createBExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, mul);
																	}
break;
case 27:
#line 322 "minic.y"
	{
																		/* create binary expression-node with the two terms and the
																		 * operator. */
																		 yyval.nodePointer = createBExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, divide);
																	}
break;
case 28:
#line 330 "minic.y"
	{
																		/* create unary expression-node with the operator and the term */
																		yyval.nodePointer = createUExpr (yystack.l_mark[0].nodePointer, uminus); 
																	}
break;
case 29:
#line 337 "minic.y"
	{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 yyval.nodePointer = createRExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, gt); 
																	}
break;
case 30:
#line 342 "minic.y"
	{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 yyval.nodePointer = createRExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, lt);
																	}
break;
case 31:
#line 347 "minic.y"
	{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 yyval.nodePointer = createRExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, eq);
																	}
break;
case 32:
#line 352 "minic.y"
	{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 yyval.nodePointer = createRExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, le);
																	}
break;
case 33:
#line 357 "minic.y"
	{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 yyval.nodePointer = createRExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, ge);
																	}
break;
case 34:
#line 362 "minic.y"
	{
																		/* create relational expression-node by using the two expressions
																		 * and the operator */
																		 yyval.nodePointer = createRExpr (yystack.l_mark[-2].nodePointer, yystack.l_mark[0].nodePointer, neq);
																	}
break;
case 35:
#line 370 "minic.y"
	{ 
																		/* just pass on the node already created */ 
																		yyval.nodePointer = yystack.l_mark[0].nodePointer; 
																	}
break;
case 36:
#line 374 "minic.y"
	{
																		/* just pass on the node already created */
																		yyval.nodePointer = yystack.l_mark[0].nodePointer; 
																	}
break;
case 37:
#line 381 "minic.y"
	{
																		/* create variable node with just the name of the variable. Check
																		 * if the variable has been declared by using the symbol table
																		 * stack. */
																		 bool hasBeenDeclared = false, stop = false;
																		 printf("\nChecking if variable %s has been declared\n", yystack.l_mark[0].varName);
																		 for (int i=symbol_table_stack.size()-1; i>=0; i--){
																			for(int j=0; j<symbol_table_stack[i].size(); j++) {
																				if (strcmp(symbol_table_stack[i][j], yystack.l_mark[0].varName) == 0) {
																					hasBeenDeclared = true;
																					stop = true;
																					break;
																				}
																			}
																			if (stop) {
																				break;
																			}
																		 }
																		 printf("------- Check concluded -------\n");
																		 if (!hasBeenDeclared) {
																			printf("\nSEMANTIC ERROR: variable %s has not been declared!\n",yystack.l_mark[0].varName);
																			/* free-ing the memory cannot happen here! It must happen at*/
																			/* the end!*/
																			compilation_success = false;
																		 }

																		 yyval.nodePointer = createVar(yystack.l_mark[0].varName);
																	}
break;
case 38:
#line 411 "minic.y"
	{
																		/* create a constant node with the value of the integer */
																		yyval.nodePointer = createCnst(yystack.l_mark[0].iValue);
																	}
break;
#line 961 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            yychar = YYLEX;
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
