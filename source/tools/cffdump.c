/* CFF font parser.
 * 2006 (C) Tor Andersson.
 */

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "mupdf/mutool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <unistd.h>
#include <errno.h>
#include <math.h> // for sqrt

static int cffpos; /* offset in file of cff (for opentypes) */
static int cfflen;

static int fontcount;

static int gsubrbias;
static int gsubrcount;
static unsigned char **gsubrdata;

static int localbias;

static const char *predefined_strings[] = {
/* 0 */		".notdef",
/* 1 */		"space",
/* 2 */		"exclam",
/* 3 */		"quotedbl",
/* 4 */		"numbersign",
/* 5 */		"dollar",
/* 6 */		"percent",
/* 7 */		"ampersand",
/* 8 */		"quoteright",
/* 9 */		"parenleft",
/* 10 */	"parenright",
/* 11 */	"asterisk",
/* 12 */	"plus",
/* 13 */	"comma",
/* 14 */	"hyphen",
/* 15 */	"period",
/* 16 */	"slash",
/* 17 */	"zero",
/* 18 */	"one",
/* 19 */	"two",
/* 20 */	"three",
/* 21 */	"four",
/* 22 */	"five",
/* 23 */	"six",
/* 24 */	"seven",
/* 25 */	"eight",
/* 26 */	"nine",
/* 27 */	"colon",
/* 28 */	"semicolon",
/* 29 */	"less",
/* 30 */	"equal",
/* 31 */	"greater",
/* 32 */	"question",
/* 33 */	"at",
/* 34 */	"A",
/* 35 */	"B",
/* 36 */	"C",
/* 37 */	"D",
/* 38 */	"E",
/* 39 */	"F",
/* 40 */	"G",
/* 41 */	"H",
/* 42 */	"I",
/* 43 */	"J",
/* 44 */	"K",
/* 45 */	"L",
/* 46 */	"M",
/* 47 */	"N",
/* 48 */	"O",
/* 49 */	"P",
/* 50 */	"Q",
/* 51 */	"R",
/* 52 */	"S",
/* 53 */	"T",
/* 54 */	"U",
/* 55 */	"V",
/* 56 */	"W",
/* 57 */	"X",
/* 58 */	"Y",
/* 59 */	"Z",
/* 60 */	"bracketleft",
/* 61 */	"backslash",
/* 62 */	"bracketright",
/* 63 */	"asciicircum",
/* 64 */	"underscore",
/* 65 */	"quoteleft",
/* 66 */	"a",
/* 67 */	"b",
/* 68 */	"c",
/* 69 */	"d",
/* 70 */	"e",
/* 71 */	"f",
/* 72 */	"g",
/* 73 */	"h",
/* 74 */	"i",
/* 75 */	"j",
/* 76 */	"k",
/* 77 */	"l",
/* 78 */	"m",
/* 79 */	"n",
/* 80 */	"o",
/* 81 */	"p",
/* 82 */	"q",
/* 83 */	"r",
/* 84 */	"s",
/* 85 */	"t",
/* 86 */	"u",
/* 87 */	"v",
/* 88 */	"w",
/* 89 */	"x",
/* 90 */	"y",
/* 91 */	"z",
/* 92 */	"braceleft",
/* 93 */	"bar",
/* 94 */	"braceright",
/* 95 */	"asciitilde",
/* 96 */	"exclamdown",
/* 97 */	"cent",
/* 98 */	"sterling",
/* 99 */	"fraction",
/* 100 */	"yen",
/* 101 */	"florin",
/* 102 */	"section",
/* 103 */	"currency",
/* 104 */	"quotesingle",
/* 105 */	"quotedblleft",
/* 106 */	"guillemotleft",
/* 107 */	"guilsinglleft",
/* 108 */	"guilsinglright",
/* 109 */	"fi",
/* 110 */	"fl",
/* 111 */	"endash",
/* 112 */	"dagger",
/* 113 */	"daggerdbl",
/* 114 */	"periodcentered",
/* 115 */	"paragraph",
/* 116 */	"bullet",
/* 117 */	"quotesinglbase",
/* 118 */	"quotedblbase",
/* 119 */	"quotedblright",
/* 120 */	"guillemotright",
/* 121 */	"ellipsis",
/* 122 */	"perthousand",
/* 123 */	"questiondown",
/* 124 */	"grave",
/* 125 */	"acute",
/* 126 */	"circumflex",
/* 127 */	"tilde",
/* 128 */	"macron",
/* 129 */	"breve",
/* 130 */	"dotaccent",
/* 131 */	"dieresis",
/* 132 */	"ring",
/* 133 */	"cedilla",
/* 134 */	"hungarumlaut",
/* 135 */	"ogonek",
/* 136 */	"caron",
/* 137 */	"emdash",
/* 138 */	"AE",
/* 139 */	"ordfeminine",
/* 140 */	"Lslash",
/* 141 */	"Oslash",
/* 142 */	"OE",
/* 143 */	"ordmasculine",
/* 144 */	"ae",
/* 145 */	"dotlessi",
/* 146 */	"lslash",
/* 147 */	"oslash",
/* 148 */	"oe",
/* 149 */	"germandbls",
/* 150 */	"onesuperior",
/* 151 */	"logicalnot",
/* 152 */	"mu",
/* 153 */	"trademark",
/* 154 */	"Eth",
/* 155 */	"onehalf",
/* 156 */	"plusminus",
/* 157 */	"Thorn",
/* 158 */	"onequarter",
/* 159 */	"divide",
/* 160 */	"brokenbar",
/* 161 */	"degree",
/* 162 */	"thorn",
/* 163 */	"threequarters",
/* 164 */	"twosuperior",
/* 165 */	"registered",
/* 166 */	"minus",
/* 167 */	"eth",
/* 168 */	"multiply",
/* 169 */	"threesuperior",
/* 170 */	"copyright",
/* 171 */	"Aacute",
/* 172 */	"Acircumflex",
/* 173 */	"Adieresis",
/* 174 */	"Agrave",
/* 175 */	"Aring",
/* 176 */	"Atilde",
/* 177 */	"Ccedilla",
/* 178 */	"Eacute",
/* 179 */	"Ecircumflex",
/* 180 */	"Edieresis",
/* 181 */	"Egrave",
/* 182 */	"Iacute",
/* 183 */	"Icircumflex",
/* 184 */	"Idieresis",
/* 185 */	"Igrave",
/* 186 */	"Ntilde",
/* 187 */	"Oacute",
/* 188 */	"Ocircumflex",
/* 189 */	"Odieresis",
/* 190 */	"Ograve",
/* 191 */	"Otilde",
/* 192 */	"Scaron",
/* 193 */	"Uacute",
/* 194 */	"Ucircumflex",
/* 195 */	"Udieresis",
/* 196 */	"Ugrave",
/* 197 */	"Yacute",
/* 198 */	"Ydieresis",
/* 199 */	"Zcaron",
/* 200 */	"aacute",
/* 201 */	"acircumflex",
/* 202 */	"adieresis",
/* 203 */	"agrave",
/* 204 */	"aring",
/* 205 */	"atilde",
/* 206 */	"ccedilla",
/* 207 */	"eacute",
/* 208 */	"ecircumflex",
/* 209 */	"edieresis",
/* 210 */	"egrave",
/* 211 */	"iacute",
/* 212 */	"icircumflex",
/* 213 */	"idieresis",
/* 214 */	"igrave",
/* 215 */	"ntilde",
/* 216 */	"oacute",
/* 217 */	"ocircumflex",
/* 218 */	"odieresis",
/* 219 */	"ograve",
/* 220 */	"otilde",
/* 221 */	"scaron",
/* 222 */	"uacute",
/* 223 */	"ucircumflex",
/* 224 */	"udieresis",
/* 225 */	"ugrave",
/* 226 */	"yacute",
/* 227 */	"ydieresis",
/* 228 */	"zcaron",
/* 229 */	"exclamsmall",
/* 230 */	"Hungarumlautsmall",
/* 231 */	"dollaroldstyle",
/* 232 */	"dollarsuperior",
/* 233 */	"ampersandsmall",
/* 234 */	"Acutesmall",
/* 235 */	"parenleftsuperior",
/* 236 */	"parenrightsuperior",
/* 237 */	"twodotenleader",
/* 238 */	"onedotenleader",
/* 239 */	"zerooldstyle",
/* 240 */	"oneoldstyle",
/* 241 */	"twooldstyle",
/* 242 */	"threeoldstyle",
/* 243 */	"fouroldstyle",
/* 244 */	"fiveoldstyle",
/* 245 */	"sixoldstyle",
/* 246 */	"sevenoldstyle",
/* 247 */	"eightoldstyle",
/* 248 */	"nineoldstyle",
/* 249 */	"commasuperior",
/* 250 */	"threequartersemdash",
/* 251 */	"periodsuperior",
/* 252 */	"questionsmall",
/* 253 */	"asuperior",
/* 254 */	"bsuperior",
/* 255 */	"centsuperior",
/* 256 */	"dsuperior",
/* 257 */	"esuperior",
/* 258 */	"isuperior",
/* 259 */	"lsuperior",
/* 260 */	"msuperior",
/* 261 */	"nsuperior",
/* 262 */	"osuperior",
/* 263 */	"rsuperior",
/* 264 */	"ssuperior",
/* 265 */	"tsuperior",
/* 266 */	"ff",
/* 267 */	"ffi",
/* 268 */	"ffl",
/* 269 */	"parenleftinferior",
/* 270 */	"parenrightinferior",
/* 271 */	"Circumflexsmall",
/* 272 */	"hyphensuperior",
/* 273 */	"Gravesmall",
/* 274 */	"Asmall",
/* 275 */	"Bsmall",
/* 276 */	"Csmall",
/* 277 */	"Dsmall",
/* 278 */	"Esmall",
/* 279 */	"Fsmall",
/* 280 */	"Gsmall",
/* 281 */	"Hsmall",
/* 282 */	"Ismall",
/* 283 */	"Jsmall",
/* 284 */	"Ksmall",
/* 285 */	"Lsmall",
/* 286 */	"Msmall",
/* 287 */	"Nsmall",
/* 288 */	"Osmall",
/* 289 */	"Psmall",
/* 290 */	"Qsmall",
/* 291 */	"Rsmall",
/* 292 */	"Ssmall",
/* 293 */	"Tsmall",
/* 294 */	"Usmall",
/* 295 */	"Vsmall",
/* 296 */	"Wsmall",
/* 297 */	"Xsmall",
/* 298 */	"Ysmall",
/* 299 */	"Zsmall",
/* 300 */	"colonmonetary",
/* 301 */	"onefitted",
/* 302 */	"rupiah",
/* 303 */	"Tildesmall",
/* 304 */	"exclamdownsmall",
/* 305 */	"centoldstyle",
/* 306 */	"Lslashsmall",
/* 307 */	"Scaronsmall",
/* 308 */	"Zcaronsmall",
/* 309 */	"Dieresissmall",
/* 310 */	"Brevesmall",
/* 311 */	"Caronsmall",
/* 312 */	"Dotaccentsmall",
/* 313 */	"Macronsmall",
/* 314 */	"figuredash",
/* 315 */	"hypheninferior",
/* 316 */	"Ogoneksmall",
/* 317 */	"Ringsmall",
/* 318 */	"Cedillasmall",
/* 319 */	"questiondownsmall",
/* 320 */	"oneeighth",
/* 321 */	"threeeighths",
/* 322 */	"fiveeighths",
/* 323 */	"seveneighths",
/* 324 */	"onethird",
/* 325 */	"twothirds",
/* 326 */	"zerosuperior",
/* 327 */	"foursuperior",
/* 328 */	"fivesuperior",
/* 329 */	"sixsuperior",
/* 330 */	"sevensuperior",
/* 331 */	"eightsuperior",
/* 332 */	"ninesuperior",
/* 333 */	"zeroinferior",
/* 334 */	"oneinferior",
/* 335 */	"twoinferior",
/* 336 */	"threeinferior",
/* 337 */	"fourinferior",
/* 338 */	"fiveinferior",
/* 339 */	"sixinferior",
/* 340 */	"seveninferior",
/* 341 */	"eightinferior",
/* 342 */	"nineinferior",
/* 343 */	"centinferior",
/* 344 */	"dollarinferior",
/* 345 */	"periodinferior",
/* 346 */	"commainferior",
/* 347 */	"Agravesmall",
/* 348 */	"Aacutesmall",
/* 349 */	"Acircumflexsmall",
/* 350 */	"Atildesmall",
/* 351 */	"Adieresissmall",
/* 352 */	"Aringsmall",
/* 353 */	"AEsmall",
/* 354 */	"Ccedillasmall",
/* 355 */	"Egravesmall",
/* 356 */	"Eacutesmall",
/* 357 */	"Ecircumflexsmall",
/* 358 */	"Edieresissmall",
/* 359 */	"Igravesmall",
/* 360 */	"Iacutesmall",
/* 361 */	"Icircumflexsmall",
/* 362 */	"Idieresissmall",
/* 363 */	"Ethsmall",
/* 364 */	"Ntildesmall",
/* 365 */	"Ogravesmall",
/* 366 */	"Oacutesmall",
/* 367 */	"Ocircumflexsmall",
/* 368 */	"Otildesmall",
/* 369 */	"Odieresissmall",
/* 370 */	"OEsmall",
/* 371 */	"Oslashsmall",
/* 372 */	"Ugravesmall",
/* 373 */	"Uacutesmall",
/* 374 */	"Ucircumflexsmall",
/* 375 */	"Udieresissmall",
/* 376 */	"Yacutesmall",
/* 377 */	"Thornsmall",
/* 378 */	"Ydieresissmall",
/* 379 */	"001.000",
/* 380 */	"001.001",
/* 381 */	"001.002",
/* 382 */	"001.003",
/* 383 */	"Black",
/* 384 */	"Bold",
/* 385 */	"Book",
/* 386 */	"Light",
/* 387 */	"Medium",
/* 388 */	"Regular",
/* 389 */	"Roman",
/* 390 */	"Semibold"
};

static struct
{
	int charset;
	int encoding;
	int charstrings;
	int charstringtype;
	int privatelen;
	int privateofs;
	int fdarray;
	int fdselect;
	int subrs; /* from private */
	int glyphcount; /* from charstrings */
	int defaultwidth;

	int subrcount; /* from subrs */
	unsigned char **subrdata;
} fontinfo[256];

void die(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	putc('\n', stderr);
	va_end(ap);
	exit(1);
}

typedef unsigned char byte;

enum { INTEGER, REAL };

struct number { int type; int ival; float rval; };

static inline int read8(FILE *f)
{
	return getc(f);
}

static inline int read16(FILE *f)
{
	int a = getc(f);
	int b = getc(f);
	return (a << 8) | b;
}

static inline int read24(FILE *f)
{
	int a = getc(f);
	int b = getc(f);
	int c = getc(f);
	return (a << 16) | (b << 8) | c;
}

static inline int read32(FILE *f)
{
	int a = getc(f);
	int b = getc(f);
	int c = getc(f);
	int d = getc(f);
	return (a << 24) | (b << 16) | (c << 8) | d;
}

static inline int readoffset(FILE *f, int offsize)
{
	if (offsize == 1) return read8(f);
	if (offsize == 2) return read16(f);
	if (offsize == 3) return read24(f);
	if (offsize == 4) return read32(f);
	die("invalid offset size");
	return EOF;
}

static inline void seekpos(FILE *f, int pos)
{
	fseek(f, cffpos + pos, 0);
}

static inline int tellpos(FILE *f)
{
	return ftell(f) - cffpos;
}

/* XXX not tested */
static float readreal(FILE *f)
{
	char buf[64];
	char *p = buf;

	/* b0 was 30 */

	while (p < buf + (sizeof buf) - 3)
	{
		int b, n;

		b = read8(f);

		n = (b >> 4) & 0xf;
		if (n < 0xA) { *p++ = n + '0'; }
		else if (n == 0xA) { *p++ = '.'; }
		else if (n == 0xB) { *p++ = 'E'; }
		else if (n == 0xC) { *p++ = 'E'; *p++ = '-'; }
		else if (n == 0xE) { *p++ = '-'; }
		else if (n == 0xF) { break; }

		n = b & 0xf;
		if (n < 0xA) { *p++ = n + '0'; }
		else if (n == 0xA) { *p++ = '.'; }
		else if (n == 0xB) { *p++ = 'E'; }
		else if (n == 0xC) { *p++ = 'E'; *p++ = '-'; }
		else if (n == 0xE) { *p++ = '-'; }
		else if (n == 0xF) { break; }
	}

	*p = 0;

	return (float)atof(buf);
}

static int readinteger(FILE *f, int b0)
{
	int b1, b2, b3, b4;

	if (b0 == 28)
	{
		b1 = read8(f);
		b2 = read8(f);
		return (short)((b1 << 8) | b2);
	}

	if (b0 == 29)
	{
		b1 = read8(f);
		b2 = read8(f);
		b3 = read8(f);
		b4 = read8(f);
		return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
	}

	if (b0 < 247)
	{
		return b0 - 139;
	}

	if (b0 < 251)
	{
		b1 = read8(f);
		return (b0 - 247) * 256 + b1 + 108;
	}

	b1 = read8(f);
	return -(b0 - 251) * 256 - b1 - 108;
}

static char *operatorname(int op)
{
	static char tmpstr[64];

	switch (op)
	{
		/* topdict */
	case 0:	return "version";
	case 1:	return "Notice";
	case 256|0:	return "Copyright";
	case 2:	return "FullName";
	case 3:	return "FamilyName";
	case 4:	return "Weight";
	case 256|1:	return "isFixedPitch";
	case 256|2:	return "ItalicAngle";
	case 256|3:	return "UnderlinePosition";
	case 256|4:	return "UnderlineThickness";
	case 256|5:	return "PaintType";
	case 256|6:	return "CharstringType";
	case 256|7:	return "FontMatrix";
	case 13:	return "UniqueID";
	case 5:	return "FontBBox";
	case 256|8:	return "StrokeWidth";
	case 14:	return "XUID";
	case 15:	return "charset";
	case 16:	return "Encoding";
	case 17:	return "CharStrings";
	case 18:	return "Private";
	case 256|20: return "SyntheticBase";
	case 256|21: return "PostScript";
	case 256|22: return "BaseFontName";
	case 256|23: return "BaseFontBlend";
	case 256|30: return "ROS";
	case 256|31: return "CIDFontVersion";
	case 256|32: return "CIDFontRevision";
	case 256|33: return "CIDFontType";
	case 256|34: return "CIDCount";
	case 256|35: return "UIDBase";
	case 256|36: return "FDArray";
	case 256|37: return "FDSelect";
	case 256|38: return "FontName";

		/* private */
	case 6: return "BlueValues";
	case 7: return "OtherBlues";
	case 8: return "FamilyBlues";
	case 9: return "FamilyOtherBlues";
	case 256|9: return "BlueScale";
	case 256|10: return "BlueShift";
	case 256|11: return "BlueFuzz";
	case 10: return "StdHW";
	case 11: return "StdVW";
	case 256|12: return "StemSnapH";
	case 256|13: return "StemSnapV";
	case 256|14: return "ForceBold";
	case 256|17: return "LanguageGroup";
	case 256|18: return "ExpansionFactor";
	case 256|19: return "initialRandomSeed";
	case 19: return "Subrs";
	case 20: return "defaultWidthX";
	case 21: return "nominalWidthX";
	}

	if (op >= 256)
		sprintf(tmpstr, "12 %d", op & 0xff);
	else
		sprintf(tmpstr, "%d", op);
	return tmpstr;
}

static int bias_from_count(int count)
{
	if (count == 0)
		return 0;
	if (count < 1240)
		return 107;
	if (count < 33900)
		return 1131;
	return 32768;
}

static void readdict(FILE *f, char *dictname, int idx, int sofs, int eofs, int indent)
{
	struct number x[48];
	int n = 0;
	int i;
	int b0;

	// Set some defaults
	fontinfo[idx].charstringtype = 2;

	if (indent) printf("    ");
	printf("%s %d\n", dictname, idx);

	if (indent) printf("    ");
	printf("{\n");

	seekpos(f, sofs);
	while (tellpos(f) < eofs)
	{
		b0 = read8(f);
		if (b0 < 22)
		{
			if (b0 == 12)
				b0 = 0x100 | read8(f);

			/* extract some offsets that we need to follow later */

			if (b0 == 15)
				fontinfo[idx].charset = x[0].ival;
			if (b0 == 16)
				fontinfo[idx].encoding = x[0].ival;
			if (b0 == 17)
				fontinfo[idx].charstrings = x[0].ival;
			if (b0 == 18)
			{
				fontinfo[idx].privatelen = x[0].ival;
				fontinfo[idx].privateofs = x[1].ival;
			}
			if (b0 == 19)
				fontinfo[idx].subrs = x[0].ival + sofs;
		if (b0 == 20)
				fontinfo[idx].defaultwidth = x[0].ival;
			if (b0 == (256|36))
				fontinfo[idx].fdarray = x[0].ival;
			if (b0 == (256|6))
				fontinfo[idx].charstringtype = x[0].ival;
			if (b0 == (256|37))
				fontinfo[idx].fdselect = x[0].ival;

			/* print the operator name and values */

			if (indent) printf("    ");
			printf("    %-20s = ", operatorname(b0));

			for (i = 0; i < n; i++)
			{
				if (x[i].type == INTEGER)
					printf("%d ", x[i].ival);
				if (x[i].type == REAL)
					printf("%0.9g ", x[i].rval);
			}

			printf("\n");

			n = 0;
		}

		else
		{
			if (b0 == 30)
			{
				x[n].type = REAL;
				x[n].rval = readreal(f);
				x[n].ival = (int)x[n].rval; // just in case
				n++;
			}
			else // if (b0 == 28 || b0 == 29 || (b0 >= 32 && b0 <= 254))
			{
				x[n].type = INTEGER;
				x[n].ival = readinteger(f, b0);
				x[n].rval = (float)x[n].ival; // just in case
				n++;
			}
		}
	}

	if (indent) printf("    ");
	printf("}\n\n");
}

void runcode(unsigned char *code, int len, int depth)
{
	int array[32]; // transient array
	int arg[48];
	unsigned char *pc;
	unsigned char *end;
	int n, nret, v, w;
	int nhints;
	int y, i;

	pc = code;
	end = code + len;
	nhints = 0;
	nret = 0;
	n = 0;

	while (pc < end)
	{
		v = *pc++;
		if (v > 31)
		{
			if (v < 247)
			{
				arg[n++] = v - 139;
			}
			else if (v < 251)
			{
				w = *pc++;
				arg[n++] = (v - 247) * 256 + w + 108;
			}
			else if (v < 255)
			{
				w = *pc++;
				arg[n++] = -(v - 251) * 256 - w - 108;
			}
			else
			{
				unsigned a = *pc++ << 24;
				unsigned b = *pc++ << 16;
				unsigned c = *pc++ << 8;
				unsigned d = *pc++;
				arg[n++] = a | b | c | d;
				arg[n-1] = arg[n-1] >> 16; // fixed point (oops)
			}
		}

		else if (v == 28)
		{
			unsigned a = *pc++ << 8;
			unsigned b = *pc++;
			arg[n++] = (short)(a | b);
		}

		else if (v == 12)
		{
			v = *pc++;
			switch (v)
			{
			case 9: // abs
				arg[n-1] = arg[n-1] < 0 ? -arg[n-1] : arg[n-1];
				break;

			case 10: // add
				arg[n-2] = arg[n-2] + arg[n-1];
				n--;
				break;

			case 11: // sub
				arg[n-2] = arg[n-2] - arg[n-1];
				n--;
				break;

			case 12: // div
				arg[n-2] = arg[n-2] / arg[n-1];
				n--;
				break;

			case 14: // neg
				arg[n-1] = -arg[n-1];
				break;

			case 23: // random
				arg[n++] = 0;
				break;

			case 24: // mul
				arg[n-2] = arg[n-2] * arg[n-1];
				n--;
				break;

			case 26: // sqrt
				arg[n-1] = (int)sqrt(arg[n-1]);
				break;

			case 18: // drop
				n--;
				break;

			case 28: // exch
				{ int t = arg[n-2]; arg[n-2] = arg[n-1]; arg[n-1] = t; }
				break;

			case 29: // index
				i = arg[--n];
				if (i < 0)
					i = 0;
				arg[n] = arg[n-1-i];
				n++;
				break;

			case 30: // roll
				{
					int j = arg[--n];
					int r = arg[--n];
					int tmp[256];
					for (i = 0; i < n; i++)
						tmp[i] = arg[n-r+i];
					for (i = 0; i < n; i++)
						arg[n-r+(i+j)%r] = tmp[i];
				}
				break;

			case 27: // dup
				arg[n] = arg[n-1];
				n++;
				break;


			case 20: // put
				array[arg[n-1]] = arg[n-2];
				n -= 2;
				break;

			case 21: // get
				arg[n-1] = array[arg[n-1]];
				break;


			case 3: // and
				arg[n-2] = arg[n-1] && arg[n-2];
				n--;
				break;

			case 4: // or
				arg[n-2] = arg[n-1] || arg[n-2];
				n--;
				break;

			case 5: // not
				arg[n-1] = !arg[n-1];
				break;

			case 15: // eq
				arg[n-2] = arg[n-2] == arg[n-1];
				break;

			case 22: // ifelse
				{
					int v2 = arg[--n];
					int v1 = arg[--n];
					int s2 = arg[--n];
					int s1 = arg[--n];
					arg[n++] = (v1 > v2) ? s2 : s1;
				}
				break;


			case 35: // flex
			case 34: // hflex
			case 36: // hflex1
			case 37: // flex1
				n = 0;
				break;

			}
		}

		else
		{
			switch (v)
			{
			case 1: // hstem
			case 3: // vstem
			case 18: // hstemhm
			case 23: // vstemhm
				i = 0;

				// thes hints take an even number of args,
				// so if we're odd, the width is first.
				if (n & 1)
				{
					printf("\t%d charwidth\n", arg[0]);
					i = 1;
				}

				y = 0;
				while (i < n)
				{
					printf("\t%d %d hintstem\n", arg[i+0] + y, arg[i+1]);
					y = y + arg[0] + arg[1];
					nhints ++;
					i += 2;
				}
				n = 0;
				break;

			case 19: // hintmask
			case 20: // cntrmask
				if (n == 1)
				{
					printf("\t%d charwidth\n", arg[0]);
				}

				printf("\thintmask (%d hints) ", nhints);
				for (i = 0; i < (nhints + 7) / 8; i++)
				{
					y = *pc++;
					printf("0x%02x ", y);
				}
				printf("\n");
				n = 0;
				break;

			case 11: // return
				printf("\treturn\n");
				break;

			case 14: // endchar
				if (n == 1)
					printf("\t%d charwidth\n", arg[0]);
				printf("\tendchar\n");
				if (pc != end)
					printf("\tENDCHAR NOT AT END!\n");
				n = 0;
				return;

			case 21: // rmoveto
				if (n == 3)
				{
					printf("\t%d charwidth\n", arg[0]);
					printf("\t%d %d rmoveto\n", arg[1], arg[2]);
				}
				else
					printf("\t%d %d rmoveto\n", arg[0], arg[1]);
				n = 0;
				break;

			case 22: // hmoveto
				if (n == 2)
				{
					printf("\t%d charwidth\n", arg[0]);
					printf("\t%d hmoveto\n", arg[1]);
				}
				else
					printf("\t%d hmoveto\n", arg[0]);
				n = 0;
				break;

			case 4: // vmoveto
				if (n == 2)
				{
					printf("\t%d charwidth\n", arg[0]);
					printf("\t%d vmoveto\n", arg[1]);
				}
				else
					printf("\t%d vmoveto\n", arg[0]);
				n = 0;
				break;

			case 5: // rlineto
				for (i = 0; i < n; i += 2)
				{
					printf("\t%d %d rlineto\n", arg[i+0], arg[i+1]);
				}
				n = 0;
				break;

			case 6: // hlineto
				for (i = 0; i < n; i++)
					printf("\t%d %clineto\n", arg[i], i&1 ? 'v' : 'h');
				n = 0;
				break;

			case 7: // vlineto
				for (i = 0; i < n; i++)
					printf("\t%d %clineto\n", arg[i], i&1 ? 'h' : 'v');
				n = 0;
				break;

			case 10:
				printf("\t");
				for (i = 0; i < n; i++)
					printf("%d ", arg[i] + (i == n-1 ? localbias : 0));
				printf("callsubr\n");
				if (depth > 1)
					printf("\tILLEGAL!\n");
				n = 0;
				break;

			case 29:
				printf("\t");
				for (i = 0; i < n; i++)
					printf("%d ", arg[i] + (i == n-1 ? gsubrbias : 0));
				printf("callgsubr\n");
				n = 0;
				break;

			case 8:
			case 24:
			case 25:
			case 26:
			case 27:
			case 31:
			case 30:
				printf("\t");
				for (i = 0; i < n; i++)
					printf("%d ", arg[i]);
				if (v == 8)
					printf("rrcurveto\n");
				else if (v == 24)
					printf("rcurveline\n");
				else if (v == 25)
					printf("rlinecurve\n");
				else if (v == 26)
					printf("vvcurveto\n");
				else if (v == 27)
					printf("hhcurveto\n");
				else if (v == 30)
					printf("vhcurveto\n");
				else if (v == 31)
					printf("hvcurveto\n");
				n = 0;
				break;
			default:
				printf("\tUNKNOWN!\n");
				break;

			}
		}
	}
	if (pc > end)
		printf("\tOVERRUN!\n");
	if (len > 0 && end[-1] != 0xe && end[-1] != 10 && end[-1] != 29 && (depth == 0 || end[-1] != 11))
		printf("\tDID NOT END ON ENDCHAR (OR SUBR CALL)%s!\n", depth == 0 ? "" : " (OR RETURN)");
}

void readcode(FILE *f)
{
	int done = 0;
	int stems = 0;
	int v, w;
	int n = 0;
	int x[48];

	while (!done)
	{
		v = getc(f);
		if (v > 31)
		{
			if (v < 247)
			{
				x[n++] = v - 139;
			}
			else if (v < 251)
			{
				w = getc(f);
				x[n++] = (v - 247) * 256 + w + 108;
			}
			else if (v < 255)
			{
				w = getc(f);
				x[n++] = -(v - 251) * 256 - w - 108;
			}
			else
			{
				unsigned a = getc(f) << 24;
				unsigned b = getc(f) << 16;
				unsigned c = getc(f) << 8;
				unsigned d = getc(f);
				unsigned u = a | b | c | d;
				x[n++] = u;
			}
			printf("%d ", x[n-1]);
		}
		else if (v == 28)
		{
			unsigned a = getc(f) << 8;
			unsigned b = getc(f);
			short s = a | b;
			x[n++] = s; // XXX Fixed?
			printf("%d ", x[n-1]);
		}

		else if (v == 19 || v == 20)
		{
			int i;
			if (v == 19) printf("hintmask");
			if (v == 20) printf("cntrmask");
			for (i = 0; i < (stems + 7) / 8; i++)
				printf(" 0x%02x", getc(f));
			printf("\n");
		}

		/* it's an operator */
		else
		{
			switch (v)
			{
			case 0: puts("-Reserved-"); break;
			case 2: puts("-Reserved-"); break;
			case 9: puts("-Reserved-"); break;
			case 13: puts("-Reserved-"); break;
			case 15: puts("-Reserved-"); break;
			case 16: puts("-Reserved-"); break;
			case 17: puts("-Reserved-"); break;
			case 1: puts("hstem"); stems++; break;
			case 3: puts("vstem"); stems++; break;
			case 4: puts("vmoveto"); break;
			case 5: puts("rlineto"); break;
			case 6: puts("hlineto"); break;
			case 7: puts("vlineto"); break;
			case 8: puts("rrcurveto"); break;
			case 10: puts("callsubr"); break;
			case 11: puts("return"); done = 1; break;
			case 14: puts("endchar"); done = 1; break;
			case 18: puts("hstemhm"); stems++; break;
			case 21: puts("rmoveto"); break;
			case 22: puts("hmoveto"); break;
			case 23: puts("vstemhm"); stems++; break;
			case 24: puts("rcurveline"); break;
			case 25: puts("rlinecurve"); break;
			case 26: puts("vvcurveto"); break;
			case 27: puts("hhcurveto"); break;
			case 29: puts("callgsubr"); break;
			case 30: puts("vhcurveto"); break;
			case 31: puts("hvcurveto"); break;
			}

			if (v == 12)
			{
				w = getc(f);
				switch (w)
				{
				case 0: puts("dotsection"); break; // deprecated noop
				case 1: puts("-Reserved-"); break;
				case 2: puts("-Reserved-"); break;
				case 6: puts("-Reserved-"); break;
				case 7: puts("-Reserved-"); break;
				case 8: puts("-Reserved-"); break;
				case 13: puts("-Reserved-"); break;
				case 16: puts("-Reserved-"); break;
				case 17: puts("-Reserved-"); break;
				case 19: puts("-Reserved-"); break;
				case 3: puts("and"); break;
				case 4: puts("or"); break;
				case 5: puts("not"); break;
				case 9: puts("abs"); break;
				case 10: puts("add"); break;
				case 11: puts("sub"); break;
				case 12: puts("div"); break;
				case 14: puts("neg"); break;
				case 15: puts("eq"); break;
				case 18: puts("drop"); break;
				}
			}

			n = 0;
		}
	}
}

void hexdump(byte *sofs, byte *eofs)
{
	printf("< ");
	while (sofs < eofs)
	printf("%02x ", *sofs++);
	printf("> ");
}

void readheader(FILE *f)
{
	int major = read8(f);
	int minor = read8(f);
	int hdrsize = read8(f);
	int offsize = read8(f);

	printf("version = %d.%d\n", major, minor);

	seekpos(f, hdrsize);
}

void readnameindex(FILE *f)
{
	int i;

	int count = read16(f);
	int offsize = read8(f);
	int *offset = malloc(sizeof(int) * (count+1));
	int baseofs;
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);

	baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
	{
		int n = offset[i+1] - offset[i];
		char buf[256 + 1];
		seekpos(f, baseofs + offset[i]);
		fread(buf, 1, n, f);
		buf[n] = 0;
		printf("name %d = (%s)\n", i, buf);
	}

	fontcount = count;

	printf("\n");
}

void readtopdictindex(FILE *f)
{
	int i;
	int count = read16(f);
	int offsize = read8(f);
	int *offset = malloc(sizeof(int) * (count+1));
	int baseofs;
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);
	baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
		readdict(f, "top dict", i, baseofs + offset[i], baseofs + offset[i+1], 0);
	free(offset);
}

void readstringindex(FILE *f)
{
	int i;
	int count = read16(f);
	if (count == 0)
		return;
	int offsize = read8(f);
	int baseofs;
	int *offset = malloc(sizeof(int)*(count+1));
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);
	baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
	{
		int n = offset[i+1] - offset[i];
		char buf[256 + 1];
		seekpos(f, baseofs + offset[i]);
		fread(buf, 1, n, f);
		buf[n] = 0;
		printf("string %d = (%s)\n", i + 391, buf);
	}
	printf("\n");
	free(offset);
}

void readgsubrindex(FILE *f)
{
	int i;
	int count = read16(f);
	if (count == 0)
		return;

	gsubrcount = count;
	gsubrbias = bias_from_count(count);
	gsubrdata = malloc(sizeof(char*) * gsubrcount);

	int offsize = read8(f);
	int *offset = malloc(sizeof(int) * (count+1));
	int baseofs;
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);
	baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
	{
		gsubrdata[i] = malloc(offset[i+1] - offset[i]);
		seekpos(f, baseofs + offset[i]);
		fread(gsubrdata[i], 1, offset[i+1] - offset[i], f);
	}

	printf("loaded %d gsubrs\n\n", gsubrcount);
	for (i = 0; i < count; i++)
	{
		printf("gsubr %d ", i);
		hexdump(gsubrdata[i],
			gsubrdata[i] + offset[i + 1] - offset[i]);
		printf("\n");
		runcode(gsubrdata[i], offset[i + 1] - offset[i], 2);
	}
	printf("\n");
	free(offset);
}

/*
 *
 */

void readfdarray(FILE *f, int idx)
{
	int fdarray = fontinfo[idx].fdarray;
	if (fdarray == 0)
		return;
	seekpos(f, fdarray);

	int i;
	int count = read16(f);
	if (count == 0)
		return;
	int offsize = read8(f);
	int *offset = malloc(sizeof(int) * (count+1));
	int baseofs;
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);

	baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
	{
		fontinfo[idx].privatelen = 0;
		fontinfo[idx].privateofs = 0;
		readdict(f, "cid font dict", i, baseofs + offset[i], baseofs + offset[i+1], 1);
		readdict(f, "cid font private dict", i,
				fontinfo[i].privateofs,
				fontinfo[i].privateofs + fontinfo[i].privatelen, 1);
	}
	free(offset);
}

void readfdselect(FILE *f, int idx)
{
	int glyphcount = fontinfo[idx].glyphcount;
	int fdselect = fontinfo[idx].fdselect;
	if (fdselect == 0)
		return;
	seekpos(f, fdselect);

	int format = read8(f);
	int i;

	printf("    cid font dict selector format %d\n    {\n", format);

	if (format == 0)
	{
		printf("        ");
		for (i = 0; i < glyphcount; i++)
		{
			int fd = read8(f);
			printf("%-3d ", fd);
			if (i % 15 == 15)
				printf("\n        ");
		}
		printf("\n");
	}

	if (format == 3)
	{
		int nranges = read16(f);
		for (i = 0; i < nranges; i++)
		{
			int first = read16(f);
			int fd = read8(f);
			printf("        range %d -> %d\n", first, fd);
		}
		int sentinel = read16(f);
		printf("        sentinel %d\n", sentinel);
	}

	printf("    }\n\n");
}

void readsubrindex(FILE *f, int idx)
{
	int i;
	int count = read16(f);
	localbias = bias_from_count(count);
	if (count == 0)
		return;
	int offsize = read8(f);
	int *offset = malloc(sizeof(int) * (count+1));
	int baseofs;
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);

	fontinfo[idx].subrcount = count;
	fontinfo[idx].subrdata = malloc(sizeof(char*) * count);

	baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
	{
		seekpos(f, baseofs + offset[i]);
		fontinfo[idx].subrdata[i] = malloc(offset[i+1] - offset[i]);
		fread(fontinfo[idx].subrdata[i], 1, offset[i+1] - offset[i], f);
	}

	printf("    loaded %d local subrs\n", count);
	for (i = 0; i < count; i++)
	{
		printf("    subr %d ", i);
		hexdump(fontinfo[idx].subrdata[i],
			fontinfo[idx].subrdata[i] + offset[i + 1] - offset[i]);
		printf("\n");
		runcode(fontinfo[idx].subrdata[i], offset[i + 1] - offset[i], 1);
	}
	printf("\n");
	free(offset);
}

static const char *cs2_operator[] =
{
	"--Reserved--",	// 0
	"hstem",	// 1
	"--Reserved--",	// 2
	"vstem",	// 3
	"vmoveto",	// 4
	"rlineto",	// 5
	"hlineto",	// 6
	"vlineto",	// 7
	"rcurveto",	// 8
	"--Reserved--",	// 9
	"callsubr",	// 10
	"return",	// 11
	"escape",	// 12
	"--Reserved--",	// 13
	"endchar",	// 14
	"--Reserved--",	// 15
	"--Reserved--",	// 16
	"--Reserved--",	// 17
	"hstemhm",	// 18
	"hintmask",	// 19
	"cntrmask",	// 20
	"rmoveto",	// 21
	"hmoveto",	// 22
	"vstemhm",	// 23
	"rcurveline",	// 24
	"rlinecurve",	// 25
	"vvcurveto",	// 26
	"hhcurveto",	// 27
	"callgsubr",	// 29
	"vhcurveto",	// 30
	"hvcurveto",	// 31
};

static const char *ext_operator[] =
{
	"--Reserved--",	// 0
	"--Reserved--",	// 1
	"--Reserved--",	// 2
	"and",		// 3
	"or",		// 4
	"not",		// 5
	"--Reserved--",	// 6
	"--Reserved--",	// 7
	"--Reserved--",	// 8
	"abs",		// 9
	"add",		// 10
	"sub",		// 11
	"div",		// 12
	"--Reserved--",	// 13
	"neg",		// 14
	"eq",		// 15
	"--Reserved--",	// 16
	"--Reserved--",	// 17
	"drop",		// 18
	"--Reserved--",	// 19
	"put",		// 20
	"get",		// 21
	"ifelse",	// 22
	"random",	// 23
	"mul",		// 24
	"--Reserved--",	// 25
	"sqrt",		// 26
	"dup",		// 27
	"exch",		// 28
	"index",	// 29
	"roll",		// 30
	"--Reserved--",	// 31
	"--Reserved--",	// 32
	"--Reserved--",	// 33
	"hflex",	// 34
	"flex",		// 35
	"hflex1",	// 36
	"flex1",	// 37
	"--Reserved--",	// 38
	"--Reserved--",	// 39
	"--Reserved--",	// 40
	"--Reserved--",	// 41
	"--Reserved--",	// 42
	"--Reserved--",	// 43
	"--Reserved--",	// 44
	"--Reserved--",	// 45
	"--Reserved--",	// 46
	"--Reserved--",	// 47
	"--Reserved--",	// 48
	"--Reserved--",	// 49
	"--Reserved--",	// 50
	"--Reserved--",	// 51
	"--Reserved--",	// 52
	"--Reserved--",	// 53
	"--Reserved--",	// 54
	"--Reserved--",	// 55
	"--Reserved--",	// 56
	"--Reserved--",	// 57
	"--Reserved--",	// 58
	"--Reserved--",	// 59
	"--Reserved--",	// 60
	"--Reserved--",	// 61
	"--Reserved--",	// 62
	"--Reserved--",	// 63
	"--Reserved--",	// 64
	"--Reserved--",	// 65
	"--Reserved--",	// 66
	"--Reserved--",	// 67
	"--Reserved--",	// 68
	"--Reserved--",	// 69
	"--Reserved--",	// 70
	"--Reserved--",	// 71
	"--Reserved--",	// 72
	"--Reserved--",	// 73
	"--Reserved--",	// 74
	"--Reserved--",	// 75
	"--Reserved--",	// 76
	"--Reserved--",	// 77
	"--Reserved--",	// 78
	"--Reserved--",	// 79
	"--Reserved--",	// 80
	"--Reserved--",	// 81
	"--Reserved--",	// 82
	"--Reserved--",	// 83
	"--Reserved--",	// 84
	"--Reserved--",	// 85
	"--Reserved--",	// 86
	"--Reserved--",	// 87
	"--Reserved--",	// 88
	"--Reserved--",	// 89
	"--Reserved--",	// 90
	"--Reserved--",	// 91
	"--Reserved--",	// 92
	"--Reserved--",	// 93
	"--Reserved--",	// 94
	"--Reserved--",	// 95
	"--Reserved--",	// 96
	"--Reserved--",	// 97
	"--Reserved--",	// 98
	"--Reserved--",	// 99
	"--Reserved--",	// 100
	"--Reserved--",	// 101
	"--Reserved--",	// 102
	"--Reserved--",	// 103
	"--Reserved--",	// 104
	"--Reserved--",	// 105
	"--Reserved--",	// 106
	"--Reserved--",	// 107
	"--Reserved--",	// 108
	"--Reserved--",	// 109
	"--Reserved--",	// 110
	"--Reserved--",	// 111
	"--Reserved--",	// 112
	"--Reserved--",	// 113
	"--Reserved--",	// 114
	"--Reserved--",	// 115
	"--Reserved--",	// 116
	"--Reserved--",	// 117
	"--Reserved--",	// 118
	"--Reserved--",	// 119
	"--Reserved--",	// 120
	"--Reserved--",	// 121
	"--Reserved--",	// 122
	"--Reserved--",	// 123
	"--Reserved--",	// 124
	"--Reserved--",	// 125
	"--Reserved--",	// 126
	"--Reserved--",	// 127
	"--Reserved--",	// 128
	"--Reserved--",	// 129
	"--Reserved--",	// 130
	"--Reserved--",	// 131
	"--Reserved--",	// 132
	"--Reserved--",	// 133
	"--Reserved--",	// 134
	"--Reserved--",	// 135
	"--Reserved--",	// 136
	"--Reserved--",	// 137
	"--Reserved--",	// 138
	"--Reserved--",	// 139
	"--Reserved--",	// 140
	"--Reserved--",	// 141
	"--Reserved--",	// 142
	"--Reserved--",	// 143
	"--Reserved--",	// 144
	"--Reserved--",	// 145
	"--Reserved--",	// 146
	"--Reserved--",	// 147
	"--Reserved--",	// 148
	"--Reserved--",	// 149
	"--Reserved--",	// 150
	"--Reserved--",	// 151
	"--Reserved--",	// 152
	"--Reserved--",	// 153
	"--Reserved--",	// 154
	"--Reserved--",	// 155
	"--Reserved--",	// 156
	"--Reserved--",	// 157
	"--Reserved--",	// 158
	"--Reserved--",	// 159
	"--Reserved--",	// 160
	"--Reserved--",	// 161
	"--Reserved--",	// 162
	"--Reserved--",	// 163
	"--Reserved--",	// 164
	"--Reserved--",	// 165
	"--Reserved--",	// 166
	"--Reserved--",	// 167
	"--Reserved--",	// 168
	"--Reserved--",	// 169
	"--Reserved--",	// 170
	"--Reserved--",	// 171
	"--Reserved--",	// 172
	"--Reserved--",	// 173
	"--Reserved--",	// 174
	"--Reserved--",	// 175
	"--Reserved--",	// 176
	"--Reserved--",	// 177
	"--Reserved--",	// 178
	"--Reserved--",	// 179
	"--Reserved--",	// 180
	"--Reserved--",	// 181
	"--Reserved--",	// 182
	"--Reserved--",	// 183
	"--Reserved--",	// 184
	"--Reserved--",	// 185
	"--Reserved--",	// 186
	"--Reserved--",	// 187
	"--Reserved--",	// 188
	"--Reserved--",	// 189
	"--Reserved--",	// 190
	"--Reserved--",	// 191
	"--Reserved--",	// 192
	"--Reserved--",	// 193
	"--Reserved--",	// 194
	"--Reserved--",	// 195
	"--Reserved--",	// 196
	"--Reserved--",	// 197
	"--Reserved--",	// 198
	"--Reserved--",	// 199
	"--Reserved--",	// 200
	"--Reserved--",	// 201
	"--Reserved--",	// 202
	"--Reserved--",	// 203
	"--Reserved--",	// 204
	"--Reserved--",	// 205
	"--Reserved--",	// 206
	"--Reserved--",	// 207
	"--Reserved--",	// 208
	"--Reserved--",	// 209
	"--Reserved--",	// 210
	"--Reserved--",	// 211
	"--Reserved--",	// 212
	"--Reserved--",	// 213
	"--Reserved--",	// 214
	"--Reserved--",	// 215
	"--Reserved--",	// 216
	"--Reserved--",	// 217
	"--Reserved--",	// 218
	"--Reserved--",	// 219
	"--Reserved--",	// 220
	"--Reserved--",	// 221
	"--Reserved--",	// 222
	"--Reserved--",	// 223
	"--Reserved--",	// 224
	"--Reserved--",	// 225
	"--Reserved--",	// 226
	"--Reserved--",	// 227
	"--Reserved--",	// 228
	"--Reserved--",	// 229
	"--Reserved--",	// 230
	"--Reserved--",	// 231
	"--Reserved--",	// 232
	"--Reserved--",	// 233
	"--Reserved--",	// 234
	"--Reserved--",	// 235
	"--Reserved--",	// 236
	"--Reserved--",	// 237
	"--Reserved--",	// 238
	"--Reserved--",	// 239
	"--Reserved--",	// 240
	"--Reserved--",	// 241
	"--Reserved--",	// 242
	"--Reserved--",	// 243
	"--Reserved--",	// 244
	"--Reserved--",	// 245
	"--Reserved--",	// 246
	"--Reserved--",	// 247
	"--Reserved--",	// 248
	"--Reserved--",	// 249
	"--Reserved--",	// 250
	"--Reserved--",	// 251
	"--Reserved--",	// 252
	"--Reserved--",	// 253
	"--Reserved--",	// 254
	"--Reserved--"	// 255
};

void dumpcharstring2(const unsigned char *code, int len)
{
	while (len--)
	{
		unsigned char v = *code++;

		if (v == 12)
		{
			unsigned char w = *code++;
			len--;
			printf("%s\n", ext_operator[w]);
		}
		else if (v == 28)
		{
			v = (*code++)<<8;
			v += (*code++);
			len -= 2;
			printf("%d ", v);
		}
		else if (v < 32)
		{
			printf("%s\n", cs2_operator[v]);
			if (v == 19 || v == 20)
			{
				unsigned char w = *code++;
				len--;
				printf("%02x ", w);
			}
		}
		else if (v <= 246)
		{
			printf("%d ", v - 139);
		}
		else if (v <= 250)
		{
			unsigned char w = *code++;
			len--;
			printf("%d ", (v-247)*256 + w + 108);
		}
		else if (v <= 254)
		{
			unsigned char w = *code++;
			len--;
			printf("%d ", -(v-251)*256 - w - 108);
		}
		else
		{
			v = (*code++)<<24;
			v += (*code++)<<16;
			v += (*code++)<<8;
			v += (*code++);
			len -= 4;
			printf("%d ", v);
		}
	}
	printf("\n");
}

void readcharstrings(FILE *f, int idx)
{
	int charstrings = fontinfo[idx].charstrings;
	if (charstrings == 0)
		die("no charstrings for font");
	seekpos(f, charstrings);

	int i;
	int count = read16(f);
	int offsize = read8(f);
	int *offset = malloc(sizeof(int) * (count+1));
	for (i = 0; i < count + 1; i++)
		offset[i] = readoffset(f, offsize);

	fontinfo[idx].glyphcount = count;

	printf("    loaded %d charstrings\n", count);

	int baseofs = tellpos(f) - 1;
	for (i = 0; i < count; i++)
	{
		int n = offset[i+1] - offset[i];
		unsigned char *code = malloc(n);

		printf("    charstring %d ", i);

		// seekpos(f, baseofs + offset[i]);
		// readcode(f);

		seekpos(f, baseofs + offset[i]);
		fread(code, 1, n, f);
		// printf("    {\n");
		// printf("    }\n");

		hexdump(code, code + n);
		printf("\n");
		if (fontinfo[idx].charstringtype == 2)
			runcode(code, n, 0);
		free(code);
	}

	printf("\n");
}

void readcharset(FILE *f, int idx)
{
	int charset = fontinfo[idx].charset;
	int glyphcount = fontinfo[idx].glyphcount;

	switch (charset)
	{
	case 0: printf("    charset = ISOAdobe\n\n"); return;
	case 1: printf("    charset = Expert\n\n"); return;
	case 2: printf("    charset = ExpertSubset\n\n"); return;
	}

	seekpos(f, charset);

	int format = read8(f);
	int wid = 75;
	int col = 4;
	int i, sid;

	printf("    charset format %d\n    {\n", format);

	if (format == 0)
	{
		printf("        ");
		for (i = 1; i < glyphcount; i++)
		{
			sid = read16(f);
			printf("%-5d ", sid);
			if (i % 15 == 0)
				printf("\n        ");
		}
		printf("\n");
	}

	if (format == 1 || format == 2)
	{
		i = 1;
		while (i < glyphcount)
		{
			int first = read16(f);
			int count;
			if (format == 1)
				count = read8(f) + 1;
			else
				count = read16(f) + 1;
			printf("        range %d + %d\n", first, count);
			i += count;
		}
	}

	printf("    }\n\n");
}

void readencoding(FILE *f, int idx)
{
	int encoding = fontinfo[idx].encoding;

	if (encoding == 0)
	{
		printf("    encoding = Standard\n\n");
		return;
	}

	if (encoding == 1)
	{
		printf("    encoding = Expert\n\n");
		return;
	}

	seekpos(f, encoding);

	int format = read8(f);
	int i;

	printf("    encoding format %d\n    {\n", format);

	if ((format & 0x7f) == 0)
	{
		printf("        ");
		int ncodes = read8(f);
		for (i = 0; i < ncodes; i++)
		{
			int code = read8(f);
			printf("%-3d ", code);
			if ((i & 15) == 0)
				printf("\n        ");
		}
		printf("\n");
	}

	if ((format & 0x7f) == 1)
	{
		int nranges = read8(f);
		for (i = 0; i < nranges; i++)
		{
			int first = read8(f);
			int count = read8(f) + 1;
			printf("        range %d + %d\n", first, count);
		}
	}

	if (format & 0x80)
	{
		int nsups = read8(f);
		for (i = 0; i < nsups; i++)
		{
			int code = read8(f);
			int sid = read16(f);
			printf("        supplement %d = %d\n", code, sid);
		}
	}

	printf("    }\n\n");
}

/*
 *
 */

void readfontset(FILE *f)
{
	int i;
	memset(fontinfo, 0, sizeof fontinfo);

	/* read the fixed-in-place tables */
	readheader(f);
	readnameindex(f);
	readtopdictindex(f);
	readstringindex(f);
	readgsubrindex(f);

	/* load the private dict (we need the subrs offset) */
	for (i = 0; i < fontcount; i++)
	{
		printf("font %d\n{\n", i);

		readdict(f, "private dict", i,
				fontinfo[i].privateofs,
				fontinfo[i].privateofs + fontinfo[i].privatelen, 1);

		if (fontinfo[i].subrs)
		{
			seekpos(f, fontinfo[i].subrs);
			readsubrindex(f, i);
		}

		/* load the char strings (we need the glyphcount) */
		readcharstrings(f, i);

		readcharset(f, i);
		readencoding(f, i);

		/* this will mess with the private and subr fields */
		readfdarray(f, i);

		readfdselect(f, i);

		printf("}\n\n");
	}
}

/*
 *
 */


#if defined(BUILD_MONOLITHIC)
#define main      mupdf_cffdump_main
#endif

int main(int argc, const char **argv)
{
	char magic[4];
	FILE *fp;
	int i;

	if (argc != 2)
		die("usage: cffdump font.cff");

	fp = fopen(argv[1], "rb");
	if (!fp)
		die("cannot open file '%s': %s", argv[1], strerror(errno));

	fread(magic, 1, 4, fp);
	if (!memcmp(magic, "OTTO", 4))
	{
		cffpos = 0;

		printf("opentype\n{\n");

		int numtables = read16(fp);
		read16(fp); // search range
		read16(fp); // entry selector
		read16(fp); // range shift

		for (i = 0; i < numtables; i++)
		{
			char name[5];
			fread(name, 1, 4, fp);
			name[5] = 0;
			int checksum = read32(fp);
			int offset = read32(fp);
			int length = read32(fp);
			printf("    table (%s) at %8d + %d\n", name, offset, length);
			if (!memcmp(name, "CFF ", 4))
			{
				cffpos = offset;
				cfflen = length;
			}
		}

		if (cffpos == 0)
			die("no CFF table found!");

		printf("}\n\n");
	}

	else
	{
		fseek(fp, 0, 2);
		cffpos = 0;
		cfflen = ftell(fp);
	}

	fseek(fp, cffpos, 0);
	readfontset(fp);

	fclose(fp);

	return 0;
}
