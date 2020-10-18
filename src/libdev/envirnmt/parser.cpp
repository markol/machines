
//# line 7 "d:\libdev\envirnmt\source\parser.y"

#include "envirnmt/elevclut.hpp"
#include "envirnmt/orbit.hpp"
#include "envirnmt/sky.hpp"
#include "envirnmt/planet.hpp"
#include "envirnmt/internal/plaparse.hpp"
#include "envirnmt/internal/orbparam.hpp"
#include "envirnmt/internal/satparam.hpp"
#include "envirnmt/internal/skydecl.hpp"
#include "render/colour.hpp"
#include "render/stars.hpp"
#include "system/pathname.hpp"
#include <iostream>
#include <fstream>

extern int  yylex();

static MexDegrees elevationRangeCheck(float deg);


//# line 28 "d:\libdev\envirnmt\source\parser.y"
typedef union  {
  int			intVal;
  float			floatVal;
  string*		pString;
  RenColour*	pColour;
  EnvElevationColourTable*	pClut;
  EnvElevationColourTable::Entry* pClutEntry;
} YYSTYPE;
#define FLOAT_CONSTANT 257
#define INTEGER_CONSTANT 258
#define STRING 259
#define UNKNOWN_KEYWORD 260
#define ColourKW 261
#define TableKW 262
#define OrbitKW 263
#define SatelliteKW 264
#define MeshKW 265
#define FileKW 266
#define BeginKW 267
#define RadiusKW 268
#define SetKW 269
#define ElevationKW 270
#define MaxKW 271
#define MinKW 272
#define HeadingKW 273
#define PeriodKW 274
#define StartKW 275
#define DirectionalKW 276
#define TimeKW 277
#define AmbientKW 278
#define GraduatedKW 279
#define SkyKW 280
#define UniformKW 281
#define ControllerKW 282
#define FogKW 283
#define VisibilityKW 284
#define StarsKW 285
#define NumberKW 286
#define ZenithKW 287
#define MultKW 288
#define StaticKW 289
#define DynamicKW 290
#define BackgroundKW 291
#define DayKW 292
#define NightKW 293
#define ThresholdKW 294
#define PhaseKW 295
#define NvgKW 296
#define FilterKW 297
#define LightKW 298
#define ConfigKW 299
#define SkylineKW 300
YYSTYPE yylval, yyval;
#define YYERRCODE 256

//# line 613 "d:\libdev\envirnmt\source\parser.y"


//#include <strstrea.h>
#include <sstream>

void yyerror(char* msg)
{
	extern int yylineno;
	std::ostringstream ostr;
	ostr << msg << " at line " << yylineno << std::endl;
	ASSERT(false, ostr.str().c_str());
}

static MexDegrees elevationRangeCheck(float deg)
{
	if (deg > 90)
		deg = 90;
	else if (deg < -90)
		deg = -90;

	return MexDegrees(deg);
}
const int yyexca[] = {
  -1, 1,
  0, -1,
  -2, 0,
  0,
};

#define YYNPROD 118
#define YYLAST 386

const int yyact[] = {
     242,      12,     179,      19,      18,     178,     214,      62,
      64,      43,      63,      12,      63,      19,      18,      64,
      62,      63,      37,     294,     293,      28,     180,      14,
      45,      64,      62,      64,     269,      29,      30,      62,
      16,      14,     270,     111,      17,     110,      55,      56,
      96,     299,      16,      97,     122,     121,      17,     283,
     316,      43,     199,     105,     137,     261,     105,     102,
     137,      63,      92,     262,     103,     138,     134,      91,
     124,      71,      97,      68,      51,      50,     134,     236,
     103,     138,     262,     103,     139,     138,      69,      91,
      70,      75,      78,      49,      77,      76,     146,      79,
      80,     144,     143,     142,     300,     127,     217,     105,
     169,     159,     126,     102,     102,      96,     150,     203,
     289,     204,     194,     193,     192,     171,     155,      33,
     261,     105,     235,     234,     233,     202,     160,      92,
     129,     128,     120,     259,     275,     260,     207,     208,
     241,     309,     303,     302,     291,     286,     285,     280,
     279,     278,     267,     265,     264,     252,     206,     243,
     229,     218,     195,     123,      83,      82,     321,     256,
     212,     247,     135,     132,     104,     133,     136,     100,
     323,     215,      99,     196,     161,     156,     151,     101,
     112,     109,     190,     147,      86,     231,     232,      72,
      48,      47,      46,      36,      34,      32,      31,      59,
      95,      94,      89,      61,      74,     238,     239,     230,
      90,      67,     319,     308,     307,     305,     304,     301,
     292,     290,      71,     273,      68,     272,     271,     263,
     257,     251,     253,     254,     255,     249,     246,      69,
     245,      70,      60,      75,      78,     244,      77,      76,
     228,      79,      80,     221,     220,     219,     213,     268,
     211,     210,     209,     205,     191,     189,     274,     188,
     187,     173,     172,     170,     116,     118,     154,     115,
     117,     145,     284,     141,     130,     108,     107,     106,
     207,     208,     168,     162,     125,     166,     140,     164,
     167,      87,      85,     163,     165,      81,      65,      40,
      39,      38,      35,     250,     312,     153,     114,     157,
     119,     158,     152,     317,     298,     322,     318,     237,
      41,     198,     197,     201,       3,     266,     200,      21,
      98,      27,      93,     306,      26,     184,     181,     186,
     183,     310,     311,     204,     313,     314,      88,      25,
      24,     315,     277,      23,      22,     258,     216,     320,
     168,     149,     168,     223,     148,      44,     282,     227,
     225,     226,     222,      84,     224,     281,     182,     131,
      66,     185,      13,      73,      15,     177,     176,     295,
     296,     297,     175,     174,     113,      58,      57,      11,
     248,      54,      53,      52,      10,       9,       8,       7,
       5,       4,      42,      20,       2,       1,     240,       6,
       0,     276,       0,       0,       0,       0,       0,       0,
       0,       0,       0,     168,       0,     168,       0,       0,
     288,     287,
};

const int yypact[] = {
    -250,   -4096,    -260,   -4096,   -4096,   -4096,   -4096,   -4096,
   -4096,   -4096,      59,      58,    -151,      57,     216,      56,
    -277,     215,     214,     213,    -276,   -4096,   -4096,   -4096,
   -4096,      55,      54,      53,    -197,    -211,    -212,    -254,
    -271,     212,    -198,      52,    -187,     211,   -4096,    -110,
    -111,   -4096,    -236,     208,      49,     207,    -203,    -225,
    -210,     197,     196,     195,      44,   -4096,   -4096,    -257,
    -259,      43,   -4096,    -273,    -290,    -266,    -139,    -243,
    -244,    -112,     -61,   -4096,    -168,    -140,    -141,     194,
    -209,     -49,   -4096,     193,    -179,    -180,    -184,     191,
    -191,   -4096,   -4096,   -4096,   -4096,      48,    -163,   -4096,
      41,    -142,    -219,     188,    -152,      40,    -248,    -164,
    -169,    -143,      39,    -207,    -166,    -210,    -170,     185,
   -4096,    -153,   -4096,   -4096,   -4096,   -4096,     184,     183,
   -4096,    -278,    -283,    -226,    -283,    -281,    -226,    -281,
     182,     181,     179,      47,   -4096,   -4096,     178,    -154,
    -155,    -156,    -113,      38,    -205,    -201,    -234,    -221,
    -144,      45,     177,   -4096,   -4096,    -131,     176,     175,
     174,    -106,     172,    -293,      36,   -4096,    -172,   -4096,
   -4096,   -4096,    -114,     171,   -4096,   -4096,   -4096,     170,
     169,   -4096,    -148,    -207,    -148,    -166,    -165,    -222,
   -4096,     166,    -115,     127,    -131,    -131,   -4096,   -4096,
   -4096,   -4096,    -145,    -146,    -147,   -4096,   -4096,   -4096,
   -4096,   -4096,   -4096,     247,    -131,    -131,    -131,    -116,
     163,     158,     156,   -4096,   -4096,   -4096,   -4096,     155,
   -4096,   -4096,    -157,     247,     151,    -118,   -4096,   -4096,
   -4096,    -131,    -131,    -131,   -4096,    -107,     150,   -4096,
    -208,     149,   -4096,    -119,    -120,     247,    -148,   -4096,
    -148,   -4096,   -4096,   -4096,    -121,   -4096,    -131,    -265,
    -258,     148,     147,     145,   -4096,    -131,   -4096,   -4096,
      -1,   -4096,     247,   -4096,    -122,    -123,    -124,    -240,
    -240,    -131,   -4096,    -125,   -4096,   -4096,   -4096,   -4096,
   -4096,    -126,   -4096,    -223,    -149,    -158,     143,    -127,
   -4096,   -4096,   -4096,   -4096,     142,    -274,    -275,     247,
     247,     247,     240,   -4096,   -4096,   -4096,   -4096,   -4096,
   -4096,   -4096,   -4096,    -247,   -4096,   -4096,    -176,   -4096,
   -4096,     141,    -128,   -4096,    -129,     140,     139,   -4096,
   -4096,   -4096,    -131,     138,     137,    -130,   -4096,   -4096,
    -131,    -131,     232,    -131,    -131,   -4096,   -4096,   -4096,
    -131,   -4096,    -238,     242,     136,   -4096,    -131,    -108,
     244,      35,   -4096,   -4096,
};

const int yypgo[] = {
       0,      71,     367,     366,     128,       0,     365,     364,
     363,     288,     362,     292,     361,     360,     359,     358,
     357,     356,     355,     354,     353,     351,     350,     349,
     348,     183,     218,     187,     347,     346,     342,     341,
     340,     339,     188,     338,     336,     193,     335,     155,
     157,     153,     154,     158,     333,     326,     325,     324,
     321,     318,     317,     123,     125,     316,     315,     312,
     311,     310,     186,     192,     300,     298,     185,     184,
     297,     296,     162,     159,     167,     156,
};

const int yyr1[] = {
       0,       6,       6,       6,       6,       7,       7,      11,
      11,      11,      11,      11,      11,      12,      17,      18,
      18,      19,      20,      13,      21,      22,      23,      23,
      23,      23,      23,      23,      24,      24,      28,      28,
      28,      25,      26,      27,      30,      29,      31,       2,
       3,       3,       4,      16,      32,      33,      33,      34,
      34,      34,      34,      34,      34,      14,      35,      36,
      36,      37,      37,      37,      37,      37,      15,      38,
      38,      39,      40,      40,      40,      43,      42,      41,
      41,      44,      45,      10,      46,      47,      49,      49,
      50,      50,      48,      52,      51,       8,       8,       8,
      53,      56,      57,      57,      58,      59,      54,      60,
      61,      61,      62,      63,      55,      64,      65,      65,
      65,      65,      65,      65,      66,      67,      68,      68,
      69,       9,       1,       1,       5,       5,
};

const int yyr2[] = {
       0,       2,       3,       3,       4,       1,       2,       1,
       1,       1,       1,       1,       1,       4,       3,       1,
       1,       8,       8,       4,       2,       2,       3,       3,
       3,       3,       3,       3,       0,       2,       1,       1,
       1,       4,       4,       4,       4,       4,       4,       7,
       1,       2,       2,       4,       3,       1,       2,       3,
       4,       4,       4,       3,       4,       4,       3,       1,
       2,       4,       5,       5,       5,       3,       5,       3,
       3,       4,       2,       2,       3,       3,       4,       0,
       2,       1,       4,       4,       2,       2,       0,       2,
       2,       2,       4,       3,       4,       1,       1,       1,
       4,       3,       2,       2,       3,       4,       4,       3,
       2,       2,       4,       4,       4,       3,       3,       3,
       3,       3,       3,       3,       4,       3,       1,       2,
       6,      13,       7,       9,       1,       1,
};

const int yychk[] = {
   -4096,      -6,      -7,     -11,     -12,     -13,      -2,     -14,
     -15,     -16,     -17,     -21,     261,     -35,     283,     -32,
     292,     296,     264,     263,      -8,     -11,     -53,     -54,
     -55,     -56,     -60,     -64,     281,     289,     290,     123,
     123,     262,     123,      58,     123,     295,      58,      58,
      58,      -9,     -10,     285,     -46,     300,     123,     123,
     123,     280,     280,     280,     -18,     -19,     -20,     292,
     293,     -22,     -23,     -25,     -26,     -27,     297,     283,
     298,      58,     -36,     -37,     265,     276,     278,     263,
     123,     -33,     -34,     268,     272,     271,     269,     274,
     275,      58,     259,     259,      -9,      58,     123,      58,
     -57,     -58,     -59,     282,     261,     -61,     -62,     -63,
     265,     291,     -65,     -66,     -67,     -68,     265,     282,
     -69,     261,      58,      58,      58,     125,     294,     294,
     125,     -24,     -26,     -27,     -25,     -27,     -25,     -26,
     261,     288,     288,     259,     125,     -37,     266,     261,
     261,     261,      58,     -38,     -39,     -40,     271,     -42,
     -43,     261,     282,     125,     -34,      58,     270,     270,
     273,      58,     277,     123,     -47,     -48,     265,     125,
     -59,     -58,      58,     262,     125,     -63,     -62,     266,
     261,     125,     -67,     -68,     -66,     -68,     -67,     -66,
     -69,     266,      58,     262,      58,      58,     -28,     -29,
     -30,     -31,     283,     280,     300,     -27,     -26,     -27,
     -25,     -26,     -25,      58,      58,      58,     123,      58,
     262,     262,     262,     259,     125,     -40,     -39,     284,
     -43,     -42,     261,      58,     262,      58,      -5,     257,
     258,      58,      58,      58,     258,      58,     299,     125,
     -49,     266,     259,      58,      58,      58,     -68,     -67,
     -68,     -66,     -66,     -67,      58,     259,      64,      -5,
      -5,     261,     261,     261,      -1,      40,      -5,      -5,
      -3,      -4,      -5,     259,      58,      58,      58,     -41,
     -41,      58,      -1,      58,     259,      -5,      -5,      -5,
     258,      58,     -50,     -51,     -52,     261,     282,      58,
     259,     259,      -1,     259,      -5,     293,     292,      58,
      58,      58,      -5,     125,      -4,      -1,     259,     259,
     259,     -44,     -45,     287,      -5,     259,     259,     -52,
     -51,     262,      58,     259,      58,     294,     294,      -1,
      -1,      -1,      44,     288,     268,      58,     259,     259,
      58,      58,      -5,      58,      58,     259,      -5,      -5,
      44,      -5,      -5,      -5,     286,      41,      44,      58,
      -5,     258,      41,     125,
};

const int yydef[] = {
       0,      -2,       0,       5,       7,       8,       9,      10,
      11,      12,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       1,       6,      85,      86,
      87,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,      20,       0,
       0,       2,       3,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,      15,      16,       0,
       0,       0,      28,       0,       0,       0,       0,       0,
       0,       0,       0,      55,       0,       0,       0,       0,
       0,       0,      45,       0,       0,       0,       0,       0,
       0,      14,      54,      44,       4,       0,       0,      76,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
     110,       0,      89,      95,     101,      13,       0,       0,
      19,      21,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,      53,      56,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,      43,      46,       0,       0,       0,
       0,       0,       0,       0,       0,      78,       0,      88,
      90,      91,       0,       0,      94,      96,      97,       0,
       0,     100,       0,       0,       0,       0,       0,       0,
     111,       0,       0,       0,       0,       0,      29,      30,
      31,      32,       0,       0,       0,      22,      23,      24,
      25,      26,      27,       0,       0,       0,       0,       0,
       0,       0,       0,      61,      62,      71,      71,       0,
      66,      67,       0,       0,       0,       0,      47,     116,
     117,       0,       0,       0,      51,       0,       0,      75,
      77,       0,      92,       0,       0,       0,     102,     103,
     104,     105,     106,     107,       0,     109,       0,       0,
       0,       0,       0,       0,      33,       0,      34,      35,
       0,      40,       0,      57,       0,       0,       0,      63,
      64,       0,      68,       0,      69,      48,      49,      50,
      52,       0,      79,       0,       0,       0,       0,       0,
      93,      98,      99,     108,       0,       0,       0,       0,
       0,       0,       0,      39,      41,      42,      58,      59,
      60,      72,      73,       0,      65,      70,       0,      80,
      81,       0,       0,      82,       0,       0,       0,      37,
      36,      38,       0,       0,       0,       0,      83,     112,
       0,       0,       0,       0,       0,      84,      17,      18,
       0,      74,       0,       0,       0,     114,       0,       0,
       0,       0,     115,     113,
};

/*****************************************************************/
/* PCYACC LALR parser driver routine -- a table driven procedure */
/* for recognizing sentences of a language defined by the        */
/* grammar that PCYACC analyzes. An LALR parsing table is then   */
/* constructed for the grammar and the skeletal parser uses the  */
/* table when performing syntactical analysis on input source    */
/* programs. The actions associated with grammar rules are       */
/* inserted into a switch statement for execution.               */
/*****************************************************************/


#ifndef YYMAXDEPTH
#define YYMAXDEPTH 200
#endif
#ifndef YYREDMAX
#define YYREDMAX 1000
#endif
#define PCYYFLAG -4096
#define WAS0ERR 0
#define WAS1ERR 1
#define WAS2ERR 2
#define WAS3ERR 3
#define yyclearin pcyytoken = -1
#define yyerrok   pcyyerrfl = 0
YYSTYPE yyv[YYMAXDEPTH];     /* value stack */
int pcyyerrct = 0;           /* error count */
int pcyyerrfl = 0;           /* error flag */
int redseq[YYREDMAX];
int redcnt = 0;
int pcyytoken = -1;          /* input token */


int yyparse()
{
#ifdef YYSHORT
  const short *yyxi;
#else
  const int *yyxi;
#endif
#ifdef YYASTFLAG
    int ti; int tj;
#endif

#ifdef YYDEBUG
int tmptoken;
#endif
  int statestack[YYMAXDEPTH]; /* state stack */
  int      j, m;              /* working index */
  YYSTYPE *yypvt;
  int      tmpstate, *yyps, n;
  YYSTYPE *yypv;


  tmpstate = 0;
  pcyytoken = -1;


#ifdef YYDEBUG
  tmptoken = -1;
#endif


  pcyyerrct = 0;
  pcyyerrfl = 0;
  yyps = &statestack[-1];
  yypv = &yyv[-1];


  enstack:    /* push stack */
#ifdef YYDEBUG
    printf("at state %d, next token %d\n", tmpstate, tmptoken);
#endif
    if (++yyps - &statestack[YYMAXDEPTH-1] > 0) {
      yyerror("pcyacc internal stack overflow");
      return(1);
    }
    *yyps = tmpstate;
    ++yypv;
    *yypv = yyval;


  newstate:
    n = yypact[tmpstate];
    if (n <= PCYYFLAG) goto defaultact; /*  a simple state */


    if (pcyytoken < 0) if ((pcyytoken=yylex()) < 0) pcyytoken = 0;
    if ((n += pcyytoken) < 0 || n >= YYLAST) goto defaultact;


    if (yychk[n=yyact[n]] == pcyytoken) { /* a shift */
#ifdef YYDEBUG
      tmptoken  = pcyytoken;
#endif
      pcyytoken = -1;
      yyval = yylval;
      tmpstate = n;
      if (pcyyerrfl > 0) --pcyyerrfl;
      goto enstack;
    }


  defaultact:


    if ((n=yydef[tmpstate]) == -2) {
      if (pcyytoken < 0) if ((pcyytoken=yylex())<0) pcyytoken = 0;
      for (yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=tmpstate); yyxi += 2);
      while (*(yyxi+=2) >= 0) if (*yyxi == pcyytoken) break;
      if ((n=yyxi[1]) < 0) { /* an accept action */
#ifdef YYASTFLAG
          yytfilep = fopen(yytfilen, "w");
          if (yytfilep == NULL) {
            fprintf(stderr, "Can't open t file: %s\n", yytfilen);
            return(0);          }
          for (ti=redcnt-1; ti>=0; ti--) {
            tj = svdprd[redseq[ti]];
            while (strcmp(svdnams[tj], "$EOP"))
              fprintf(yytfilep, "%s ", svdnams[tj++]);
            fprintf(yytfilep, "\n");
          }
          fclose(yytfilep);
#endif

        return (0);
      }
    }


    if (n == 0) {        /* error situation */
      switch (pcyyerrfl) {
        case WAS0ERR:          /* an error just occurred */
          yyerror("syntax error");
            ++pcyyerrct;
        case WAS1ERR:
        case WAS2ERR:           /* try again */
          pcyyerrfl = 3;
	   /* find a state for a legal shift action */
          while (yyps >= statestack) {
	     n = yypact[*yyps] + YYERRCODE;
	     if (n >= 0 && n < YYLAST && yychk[yyact[n]] == YYERRCODE) {
	       tmpstate = yyact[n];  /* simulate a shift of "error" */
	       goto enstack;
            }
	     n = yypact[*yyps];


	     /* the current yyps has no shift on "error", pop stack */
#ifdef YYDEBUG
            printf("error: pop state %d, recover state %d\n", *yyps, yyps[-1]);
#endif
	     --yyps;
	     --yypv;
	   }


	   yyabort:

#ifdef YYASTFLAG
              yytfilep = fopen(yytfilen, "w");
              if (yytfilep == NULL) {
                fprintf(stderr, "Can't open t file: %s\n", yytfilen);
                return(1);              }
              for (ti=1; ti<redcnt; ti++) {
                tj = svdprd[redseq[ti]];
                while (strcmp(svdnams[tj], "$EOP"))
                  fprintf(yytfilep, "%s ", svdnams[tj++]);
                fprintf(yytfilep, "\n");
              }
              fclose(yytfilep);
#endif

	     return(1);


	 case WAS3ERR:  /* clobber input char */
#ifdef YYDEBUG
          printf("error: discard token %d\n", pcyytoken);
#endif
          if (pcyytoken == 0) goto yyabort; /* quit */
	   pcyytoken = -1;
	   goto newstate;      } /* switch */
    } /* if */


    /* reduction, given a production n */
#ifdef YYDEBUG
    printf("reduce with rule %d\n", n);
#endif
#ifdef YYASTFLAG
    if ( redcnt<YYREDMAX ) redseq[redcnt++] = n;
#endif
    yyps -= yyr2[n];
    yypvt = yypv;
    yypv -= yyr2[n];
    yyval = yypv[1];
    m = n;
    /* find next state from goto table */
    n = yyr1[n];
    j = yypgo[n] + *yyps + 1;
    if (j>=YYLAST || yychk[ tmpstate = yyact[j] ] != -n) tmpstate = yyact[yypgo[n]];
    switch (m) { /* actions associated with grammar rules */

      case 17:
//# line 101 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* planet = EnvIPlanetParser::instance().planet();
      		ASSERT(planet, "A planet has not been created.");

		if(yypvt[-4].floatVal < yypvt[-0].floatVal)
      			yyerror("\"the day threshold must be >= the night threshold\"");

		planet->darknessRange(yypvt[-0].floatVal, yypvt[-4].floatVal);
      	} break;
      case 18:
//# line 114 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* planet = EnvIPlanetParser::instance().planet();
      		ASSERT(planet, "A planet has not been created.");

		if(yypvt[-0].floatVal < yypvt[-4].floatVal)
      			yyerror("\"the day threshold must be >= the night threshold\"");

		planet->darknessRange(yypvt[-4].floatVal, yypvt[-0].floatVal);
      	} break;
      case 33:
//# line 148 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* pPlanet = EnvIPlanetParser::instance().planet();
      		ASSERT(pPlanet, "There is no valid planet.");

		pPlanet->nvgFilterColour(*yypvt[-0].pColour);
      		_DELETE(yypvt[-0].pColour);
      	} break;
      case 34:
//# line 157 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* pPlanet = EnvIPlanetParser::instance().planet();
      		ASSERT(pPlanet, "There is no valid planet.");

		pPlanet->nvgFogMultiplier(yypvt[-0].floatVal);
      	} break;
      case 35:
//# line 165 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* pPlanet = EnvIPlanetParser::instance().planet();
      		ASSERT(pPlanet, "There is no valid planet.");

		pPlanet->nvgLightMultiplier(yypvt[-0].floatVal);
      	} break;
      case 36:
//# line 174 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* pPlanet = EnvIPlanetParser::instance().planet();
      		ASSERT(pPlanet, "There is no valid planet.");

		pPlanet->nvgSkyColour(*yypvt[-0].pColour);
      		_DELETE(yypvt[-0].pColour);
      	} break;
      case 37:
//# line 184 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* pPlanet = EnvIPlanetParser::instance().planet();
      		ASSERT(pPlanet, "There is no valid planet.");

		pPlanet->nvgFogColour(*yypvt[-0].pColour);
      		_DELETE(yypvt[-0].pColour);
      	} break;
      case 38:
//# line 194 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* pPlanet = EnvIPlanetParser::instance().planet();
      		ASSERT(pPlanet, "There is no valid planet.");

		pPlanet->nvgSkylineColour(*yypvt[-0].pColour);
      		_DELETE(yypvt[-0].pColour);
      	} break;
      case 39:
//# line 204 "d:\libdev\envirnmt\source\parser.y"
      {
      		yypvt[-1].pClut->name(*yypvt[-3].pString);
      		_DELETE(yypvt[-3].pString);
      		yyval.pClut = yypvt[-1].pClut;

		// We can't add the colour table to the planet parser until it has a name.
      		EnvIPlanetParser::instance().addClut(yyval.pClut);

		IAIN_STREAM(*yyval.pClut << std::endl);
      	} break;
      case 40:
//# line 217 "d:\libdev\envirnmt\source\parser.y"
      {
      		yyval.pClut = _NEW(EnvElevationColourTable());
      		yyval.pClut->addEntry(yypvt[-0].pClutEntry->elevation, yypvt[-0].pClutEntry->colour);
      		_DELETE(yypvt[-0].pClutEntry);
      	} break;
      case 41:
//# line 223 "d:\libdev\envirnmt\source\parser.y"
      {
      		yypvt[-1].pClut->addEntry(yypvt[-0].pClutEntry->elevation, yypvt[-0].pClutEntry->colour);
      		_DELETE(yypvt[-0].pClutEntry);
      		yyval.pClut = yypvt[-1].pClut;
      	} break;
      case 42:
//# line 231 "d:\libdev\envirnmt\source\parser.y"
      {
      		yyval.pClutEntry = _NEW(EnvElevationColourTable::Entry);
      		yyval.pClutEntry->elevation = elevationRangeCheck(yypvt[-1].floatVal);
      		yyval.pClutEntry->colour    = *yypvt[-0].pColour;
      		_DELETE(yypvt[-0].pColour);
      	} break;
      case 43:
//# line 239 "d:\libdev\envirnmt\source\parser.y"
      {
      		// The full orbit definition has now has been seen.
      		EnvIPlanetParser::instance().orbitComplete();
      	} break;
      case 44:
//# line 246 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().startOrbit(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 47:
//# line 257 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIOrbitParams* o = EnvIPlanetParser::instance().orbitInConstruction();
      		ASSERT(o, "Parsing logic error.  Orbit params are not set.");
      		o->radius(yypvt[-0].floatVal);
      	} break;
      case 48:
//# line 263 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIOrbitParams* o = EnvIPlanetParser::instance().orbitInConstruction();
      		ASSERT(o, "Parsing logic error.  Orbit params are not set.");
      		o->minElevation(yypvt[-0].floatVal);
      	} break;
      case 49:
//# line 269 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIOrbitParams* o = EnvIPlanetParser::instance().orbitInConstruction();
      		ASSERT(o, "Parsing logic error.  Orbit params are not set.");
      		o->maxElevation(yypvt[-0].floatVal);
      	} break;
      case 50:
//# line 275 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIOrbitParams* o = EnvIPlanetParser::instance().orbitInConstruction();
      		ASSERT(o, "Parsing logic error.  Orbit params are not set.");
      		o->heading(yypvt[-0].floatVal);
      	} break;
      case 51:
//# line 281 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIOrbitParams* o = EnvIPlanetParser::instance().orbitInConstruction();
      		ASSERT(o, "Parsing logic error.  Orbit params are not set.");
      		o->period(yypvt[-0].intVal);
      	} break;
      case 52:
//# line 287 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIOrbitParams* o = EnvIPlanetParser::instance().orbitInConstruction();
      		ASSERT(o, "Parsing logic error.  Orbit params are not set.");
      		o->startTime(yypvt[-0].intVal);
      	} break;
      case 53:
//# line 294 "d:\libdev\envirnmt\source\parser.y"
      {
      		// The full satellite definition has now has been seen.
      		EnvIPlanetParser::instance().satelliteComplete();
      	} break;
      case 54:
//# line 301 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().startSatellite(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 57:
//# line 312 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvISatelliteParams* s = EnvIPlanetParser::instance().satelliteInConstruction();
      		ASSERT(s, "Parsing logic error.  Satellite params are not set.");
      		s->mesh(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 58:
//# line 319 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvISatelliteParams* s = EnvIPlanetParser::instance().satelliteInConstruction();
      		ASSERT(s, "Parsing logic error.  Satellite params are not set.");
      		s->meshColourClut(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 59:
//# line 326 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvISatelliteParams* s = EnvIPlanetParser::instance().satelliteInConstruction();
      		ASSERT(s, "Parsing logic error.  Satellite params are not set.");
      		s->dirLightClut(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 60:
//# line 333 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvISatelliteParams* s = EnvIPlanetParser::instance().satelliteInConstruction();
      		ASSERT(s, "Parsing logic error.  Satellite params are not set.");
      		s->ambLightClut(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 61:
//# line 340 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvISatelliteParams* s = EnvIPlanetParser::instance().satelliteInConstruction();
      		ASSERT(s, "Parsing logic error.  Satellite params are not set.");
      		s->orbit(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 65:
//# line 355 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* p = EnvIPlanetParser::instance().planet();
      		ASSERT(p, "Parsing logic error.  Planet is not set.");
      		p->fogMaxVisibility(yypvt[-0].floatVal);
      	} break;
      case 68:
//# line 365 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* p = EnvIPlanetParser::instance().planet();
      		ASSERT(p, "Parsing logic error.  Planet is not set.");
      		p->fogColourFixed(*yypvt[-0].pColour);
      		_DELETE(yypvt[-0].pColour);
      	} break;
      case 69:
//# line 374 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* p = EnvIPlanetParser::instance().planet();
      		ASSERT(p, "Parsing logic error.  Planet is not set.");

		EnvSatellite* sat = EnvIPlanetParser::instance().lookUpSatellite(yypvt[-0].pString);
      		p->fogColourController(sat);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 70:
//# line 385 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* p = EnvIPlanetParser::instance().planet();
      		ASSERT(p, "Parsing logic error.  Planet is not set.");

		EnvElevationColourTable* clut = EnvIPlanetParser::instance().lookUpClut(yypvt[-0].pString);
      		p->fogColourTable(clut);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 74:
//# line 402 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvPlanetEnvironment* p = EnvIPlanetParser::instance().planet();
      		ASSERT(p, "Parsing logic error.  Planet is not set.");
      		p->fogZenithScale(yypvt[-0].floatVal);
      	} break;
      case 75:
//# line 409 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().completeSkyline();
      	} break;
      case 76:
//# line 414 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().startSkyline();
      	} break;
      case 82:
//# line 430 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skylineMeshFile(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 83:
//# line 436 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skylineController(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 84:
//# line 442 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skylineColourTable(yypvt[-0].pString);
      		_DELETE(yypvt[-0].pString);
      	} break;
      case 88:
//# line 452 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().uniformSky().completeSky();
      	} break;
      case 89:
//# line 457 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().createUniformSkyDeclaration(
      			EnvIPlanetParser::instance().createUniformSky());
      	} break;
      case 92:
//# line 467 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().uniformSky().controller(*yypvt[-0].pString);

		_DELETE(yypvt[-0].pString);
      	} break;
      case 93:
//# line 474 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().uniformSky().colourTable(*yypvt[-0].pString);

		_DELETE(yypvt[-0].pString);
      	} break;
      case 94:
//# line 482 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().staticSky().completeSky();
      	} break;
      case 95:
//# line 487 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().createStaticSkyDeclaration(
      			EnvIPlanetParser::instance().createStaticSky());
      	} break;
      case 98:
//# line 496 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().staticSky().meshFile(*yypvt[-0].pString);

		_DELETE(yypvt[-0].pString);
      	} break;
      case 99:
//# line 503 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().staticSky().backgroundColour(*yypvt[-0].pColour);

		_DELETE(yypvt[-0].pColour);
      	} break;
      case 100:
//# line 511 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().dynamicSky().completeSky();
      	} break;
      case 101:
//# line 516 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().createDynamicSkyDeclaration(
      			EnvIPlanetParser::instance().createDynamicSky());
      	} break;
      case 108:
//# line 530 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().dynamicSky().meshFile(*yypvt[-0].pString);

		_DELETE(yypvt[-0].pString);
      	} break;
      case 109:
//# line 537 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().dynamicSky().controller(*yypvt[-0].pString);

		_DELETE(yypvt[-0].pString);
      	} break;
      case 112:
//# line 546 "d:\libdev\envirnmt\source\parser.y"
      {
      		EnvIPlanetParser::instance().skyDeclaration().dynamicSky().colourTable(yypvt[-2].floatVal, *yypvt[-0].pString);

		_DELETE(yypvt[-0].pString);
      	} break;
      case 113:
//# line 553 "d:\libdev\envirnmt\source\parser.y"
      {
      		PRE(*yypvt[-7].pString == "spherical" or *yypvt[-7].pString == "hemispherical");

		if(*yypvt[-7].pString == "spherical")
      			EnvIPlanetParser::instance().createStars(RenStars::SPHERICAL, yypvt[-4].floatVal, yypvt[-1].intVal);
      		else
      			EnvIPlanetParser::instance().createStars(RenStars::HEMISPHERICAL, yypvt[-4].floatVal, yypvt[-1].intVal);

		_DELETE(yypvt[-7].pString);
      	} break;
      case 114:
//# line 566 "d:\libdev\envirnmt\source\parser.y"
      {
      		yyval.pColour = _NEW(RenColour(yypvt[-5].floatVal, yypvt[-3].floatVal, yypvt[-1].floatVal));
      		IAIN_STREAM(*yyval.pColour << std::endl);
      	} break;
      case 115:
//# line 571 "d:\libdev\envirnmt\source\parser.y"
      {
      		yyval.pColour = _NEW(RenColour(yypvt[-7].floatVal, yypvt[-5].floatVal, yypvt[-3].floatVal, yypvt[-1].floatVal));
      		IAIN_STREAM(*yyval.pColour << std::endl);
      	} break;
      case 116:
//# line 577 "d:\libdev\envirnmt\source\parser.y"
      {
      		yyval.floatVal = yypvt[-0].floatVal;
      	} break;
      case 117:
//# line 581 "d:\libdev\envirnmt\source\parser.y"
      {
      		yyval.floatVal = yypvt[-0].intVal;
      	} break;    }
    goto enstack;
}
