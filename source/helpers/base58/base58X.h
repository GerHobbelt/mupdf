#pragma once

#ifndef __BASE58X_ENC_DEC_H__
#define __BASE58X_ENC_DEC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	// Encode the `src` byte sequence into a Base58X string value stored in the `dst` buffer.
	// Return a reference to the NUL-terminated string buffer when done or NULL on error.
	const char* EncodeBase58X(char* dst, size_t dstsize, uint8_t* src, size_t srcsize);

	// Decode the `src` Base58X string to a byte sequence stored in the `dst` buffer.
	// Return a reference to the `dst` buffer when done or NULL on error.
	//
	// NOTE: `*targetsize` is set to the *decoded* number of bytes produced by this decode.
	//
	// NOTE: The buffer MUST be 8 bytes larger to account for boundary effect during decoding.
	const uint8_t* DecodeBase58X(uint8_t* dst, size_t dstsize, size_t* targetsize_ref, const char* src);

#ifdef __cplusplus
}
#endif

#endif
