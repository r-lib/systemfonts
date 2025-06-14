#pragma once

#include <cstring>
#include <cctype>
#include <cstdint>
#include <vector>
#include <exception>
#include <algorithm>

#include <cpp11/protect.hpp>

#define BEGIN_CPP                 \
SEXP err = R_NilValue;            \
const size_t ERROR_SIZE = 8192;   \
char buf[ERROR_SIZE] = "";        \
try {
#define END_CPP                                                \
}                                                              \
catch (cpp11::unwind_exception & e) {                          \
  err = e.token;                                               \
}                                                              \
catch (std::exception & e) {                                   \
  strncpy(buf, e.what(), ERROR_SIZE - 1);                      \
}                                                              \
catch (...) {                                                  \
  strncpy(buf, "C++ error (unknown cause)", ERROR_SIZE - 1);   \
}                                                              \
if (buf[0] != '\0') {                                          \
  Rf_error("%s", buf);                                         \
} else if (err != R_NilValue) {                                \
  R_ContinueUnwind(err);                                                 \
}

const static long WGHT_TAG = 2003265652;
const static long WDTH_TAG = 2003072104;
const static long ITAL_TAG = 1769234796;
const static double FIXED_MOD = 65536.0;

inline bool strcmp_no_case(const char * A, const char * B) {
  if (A == NULL && B == NULL) return true;
  if (A == NULL || B == NULL) return false;
  unsigned int a_len = strlen(A);
  if (strlen(B) != a_len)
    return false;
  for (unsigned int i = 0; i < a_len; ++i)
    if (tolower(A[i]) != tolower(B[i]))
      return false;
  return true;
}



/*
 Basic UTF-8 manipulation routines
 by Jeff Bezanson
 placed in the public domain Fall 2005

 This code is designed to provide the utilities you need to manipulate
 UTF-8 as an internal string encoding. These functions do not perform the
 error checking normally needed when handling UTF-8 data, so if you happen
 to be from the Unicode Consortium you will want to flay me alive.
 I do this because error checking can be performed at the boundaries (I/O),
 with these routines reserved for higher performance on data known to be
 valid.

 Source: https://www.cprogramming.com/tutorial/utf8.c

 Modified 2019 by Thomas Lin Pedersen to work with const char*
 */

static const uint32_t offsetsFromUTF8[6] = {
  0x00000000UL, 0x00003080UL, 0x000E2080UL,
  0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static const char trailingBytesForUTF8[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/* conversions without error checking
 only works for valid UTF-8, i.e. no 5- or 6-byte sequences
 srcsz = source size in bytes, or -1 if 0-terminated
 sz = dest size in # of wide characters

 returns # characters converted
 dest will always be L'\0'-terminated, even if there isn't enough room
 for all the characters.
 if sz = srcsz+1 (i.e. 4*srcsz+4 bytes), there will always be enough space.
 */
static int u8_toucs(uint32_t *dest, int sz, const char *src, int srcsz)
{
  uint32_t ch;
  const char *src_end = src + srcsz;
  int nb;
  int i=0;

  while (i < sz-1) {
    nb = trailingBytesForUTF8[(unsigned char)*src];
    if (srcsz == -1) {
      if (*src == 0)
        goto done_toucs;
    }
    else {
      if (src + nb >= src_end)
        goto done_toucs;
    }
    ch = 0;
    switch (nb) {
    /* these fall through deliberately */
    case 5: ch += (unsigned char)*src++; ch <<= 6;
    case 4: ch += (unsigned char)*src++; ch <<= 6;
    case 3: ch += (unsigned char)*src++; ch <<= 6;
    case 2: ch += (unsigned char)*src++; ch <<= 6;
    case 1: ch += (unsigned char)*src++; ch <<= 6;
    case 0: ch += (unsigned char)*src++;
    }
    ch -= offsetsFromUTF8[nb];
    dest[i++] = ch;
  }
  done_toucs:
    dest[i] = 0;
  return i;
}

/*
 End of Basic UTF-8 manipulation routines
 by Jeff Bezanson
 */

class UTF_UCS {
  std::vector<uint32_t> buffer;

public:
  UTF_UCS() {
    // Allocate space in buffer
    buffer.resize(1024);
  }
  ~UTF_UCS() {
  }
  uint32_t * convert(const char * string, int &n_conv) {
    if (string == NULL) {
      n_conv = 0;
      return buffer.data();
    }
    int n_bytes = strlen(string) + 1;
    unsigned int max_size = n_bytes * 4;
    if (buffer.size() < max_size) {
      buffer.resize(max_size);
    }

    n_conv = u8_toucs(buffer.data(), max_size, string, -1);

    return buffer.data();
  }
};

inline int axis_to_tag(std::string axis) {
  std::reverse(axis.begin(), axis.end());
  const int *tag = reinterpret_cast<const int *>(axis.c_str());
  return *tag;
}

inline std::string tag_to_axis(int tag) {
  const char *axis = reinterpret_cast<const char *>(&tag);
  std::string axis2(axis, 4);
  std::reverse(axis2.begin(), axis2.end());
  return axis2;
}
