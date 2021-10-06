/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SOAPCPP_YACC_H_INCLUDED
# define YY_YY_SOAPCPP_YACC_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PRAGMA = 258,
    AUTO = 259,
    DOUBLE = 260,
    INT = 261,
    STRUCT = 262,
    BREAK = 263,
    ELSE = 264,
    LONG = 265,
    SWITCH = 266,
    CASE = 267,
    ENUM = 268,
    REGISTER = 269,
    TYPEDEF = 270,
    CHAR = 271,
    EXTERN = 272,
    RETURN = 273,
    UNION = 274,
    CONST = 275,
    FLOAT = 276,
    SHORT = 277,
    UNSIGNED = 278,
    CONTINUE = 279,
    FOR = 280,
    SIGNED = 281,
    VOID = 282,
    DEFAULT = 283,
    GOTO = 284,
    SIZEOF = 285,
    VOLATILE = 286,
    DO = 287,
    IF = 288,
    STATIC = 289,
    WHILE = 290,
    CLASS = 291,
    PRIVATE = 292,
    PROTECTED = 293,
    PUBLIC = 294,
    VIRTUAL = 295,
    INLINE = 296,
    OPERATOR = 297,
    LLONG = 298,
    BOOL = 299,
    CFALSE = 300,
    CTRUE = 301,
    WCHAR = 302,
    TIME = 303,
    USING = 304,
    NAMESPACE = 305,
    ULLONG = 306,
    MUSTUNDERSTAND = 307,
    SIZE = 308,
    FRIEND = 309,
    TEMPLATE = 310,
    EXPLICIT = 311,
    TYPENAME = 312,
    MUTABLE = 313,
    null = 314,
    RESTRICT = 315,
    FINAL = 316,
    OVERRIDE = 317,
    UCHAR = 318,
    USHORT = 319,
    UINT = 320,
    ULONG = 321,
    NONE = 322,
    ID = 323,
    LAB = 324,
    TYPE = 325,
    LNG = 326,
    DBL = 327,
    CHR = 328,
    TAG = 329,
    STR = 330,
    PA = 331,
    NA = 332,
    TA = 333,
    DA = 334,
    MA = 335,
    AA = 336,
    XA = 337,
    OA = 338,
    LA = 339,
    RA = 340,
    OR = 341,
    AN = 342,
    EQ = 343,
    NE = 344,
    LE = 345,
    GE = 346,
    LS = 347,
    RS = 348,
    AR = 349,
    PP = 350,
    NN = 351
  };
#endif
/* Tokens.  */
#define PRAGMA 258
#define AUTO 259
#define DOUBLE 260
#define INT 261
#define STRUCT 262
#define BREAK 263
#define ELSE 264
#define LONG 265
#define SWITCH 266
#define CASE 267
#define ENUM 268
#define REGISTER 269
#define TYPEDEF 270
#define CHAR 271
#define EXTERN 272
#define RETURN 273
#define UNION 274
#define CONST 275
#define FLOAT 276
#define SHORT 277
#define UNSIGNED 278
#define CONTINUE 279
#define FOR 280
#define SIGNED 281
#define VOID 282
#define DEFAULT 283
#define GOTO 284
#define SIZEOF 285
#define VOLATILE 286
#define DO 287
#define IF 288
#define STATIC 289
#define WHILE 290
#define CLASS 291
#define PRIVATE 292
#define PROTECTED 293
#define PUBLIC 294
#define VIRTUAL 295
#define INLINE 296
#define OPERATOR 297
#define LLONG 298
#define BOOL 299
#define CFALSE 300
#define CTRUE 301
#define WCHAR 302
#define TIME 303
#define USING 304
#define NAMESPACE 305
#define ULLONG 306
#define MUSTUNDERSTAND 307
#define SIZE 308
#define FRIEND 309
#define TEMPLATE 310
#define EXPLICIT 311
#define TYPENAME 312
#define MUTABLE 313
#define null 314
#define RESTRICT 315
#define FINAL 316
#define OVERRIDE 317
#define UCHAR 318
#define USHORT 319
#define UINT 320
#define ULONG 321
#define NONE 322
#define ID 323
#define LAB 324
#define TYPE 325
#define LNG 326
#define DBL 327
#define CHR 328
#define TAG 329
#define STR 330
#define PA 331
#define NA 332
#define TA 333
#define DA 334
#define MA 335
#define AA 336
#define XA 337
#define OA 338
#define LA 339
#define RA 340
#define OR 341
#define AN 342
#define EQ 343
#define NE 344
#define LE 345
#define GE 346
#define LS 347
#define RS 348
#define AR 349
#define PP 350
#define NN 351

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 146 "soapcpp2_yacc.y" /* yacc.c:1909  */

  Symbol  *sym;
  LONG64  i;
  double  r;
  char    c;
  char    *s;
  Tnode   *typ;
  Storage sto;
  Node    rec;
  Entry   *e;
  IR      ir;

#line 259 "soapcpp2_yacc.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SOAPCPP_YACC_H_INCLUDED  */
