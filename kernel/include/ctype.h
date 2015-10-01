#define _U	0x01	/* upper case */
#define _L	0x02	/* lower case */
#define _D	0x04	/* digit */
#define _C	0x08	/* control */
#define _P	0x10	/* punctuation */
#define _S	0x20	/* white space (space/cr/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

#define isalnum(c)	((_ctype)[(unsigned char)(c)] & (_U | _L | _D))
#define isalpha(c)	((_ctype)[(unsigned char)(c)] & (_U | _L))
#define iscntrl(c)	((_ctype)[(unsigned char)(c)] & (_C))
#define isdigit(c)	((_ctype)[(unsigned char)(c)] & (_D))
#define isgraph(c)	((_ctype)[(unsigned char)(c)] & (_P | _U | _L | _D))
#define islower(c)	((_ctype)[(unsigned char)(c)] & (_L))
#define isprint(c)	((_ctype)[(unsigned char)(c)] & (_P | _U | _L | _D | _SP))
#define ispunct(c)	((_ctype)[(unsigned char)(c)] & (_P))
#define isspace(c)	((_ctype)[(unsigned char)(c)] & (_S))
#define isupper(c)	((_ctype)[(unsigned char)(c)] & (_U))
#define isxdigit(c)	((_ctype)[(unsigned char)(c)] & (_D | _X))
#define isascii(c)	((unsigned)(c) <= 0x7F)
#define toascii(c)	((unsigned)(c) & 0x7F)
#define tolower(c)	(isupper(c) ? c + 'a' - 'A' : c)
#define toupper(c)	(islower(c) ? c + 'A' - 'a' : c)

const unsigned char _ctype[] = {
    _C,_C,_C,_C,_C,_C,_C,_C,				/* 0-7 */
    _C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,			/* 8-15 */
    _C,_C,_C,_C,_C,_C,_C,_C,				/* 16-23 */
    _C,_C,_C,_C,_C,_C,_C,_C,				/* 24-31 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,				/* 32-39 */
    _P,_P,_P,_P,_P,_P,_P,_P,				/* 40-47 */
    _D,_D,_D,_D,_D,_D,_D,_D,				/* 48-55 */
    _D,_D,_P,_P,_P,_P,_P,_P,				/* 56-63 */
    _P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,		/* 64-71 */
    _U,_U,_U,_U,_U,_U,_U,_U,				/* 72-79 */
    _U,_U,_U,_U,_U,_U,_U,_U,				/* 80-87 */
    _U,_U,_U,_P,_P,_P,_P,_P,				/* 88-95 */
    _P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,		/* 96-103 */
    _L,_L,_L,_L,_L,_L,_L,_L,				/* 104-111 */
    _L,_L,_L,_L,_L,_L,_L,_L,				/* 112-119 */
    _L,_L,_L,_P,_P,_P,_P,_C,				/* 120-127 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 128-143 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 144-159 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 160-175 */
    _P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 176-191 */
    _U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,	/* 192-207 */
    _U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,	/* 208-223 */
    _L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,	/* 224-239 */
    _L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L
};	/* 240-255 */
