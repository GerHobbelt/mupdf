#pragma once

enum
{
	UC_BIDI_L = 0,
	UC_BIDI_R,
	UC_BIDI_AL,
	UC_BIDI_AN,
	UC_BIDI_EN,
	UC_BIDI_NSM,
	UC_BIDI_ES,
	UC_BIDI_CS,
	UC_BIDI_ET,
	UC_BIDI_ON,
	UC_BIDI_BN
};

enum
{
	UC_CCC_VR = 1,
};

enum
{
	UC_JOINING_TYPE_L = 1,
	UC_JOINING_TYPE_D,
	UC_JOINING_TYPE_T,
	UC_JOINING_TYPE_R,
};

typedef uint32_t ucs4_t;

typedef struct
{
	const char* name;
} uc_script_t;

static inline int uc_bidi_category(int c)
{
	return UC_BIDI_L;
}

int uc_combining_class(uint32_t cp);

int uc_joining_type(uint32_t lbl);


