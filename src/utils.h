#ifndef SFUTILS_H
#define SFUTILS_H

#include <cstring>
#include <cctype>

static bool strcmp_no_case(const char * A, const char * B) {
  unsigned int a_len = strlen(A);
  if (strlen(B) != a_len)
    return false;
  for (unsigned int i = 0; i < a_len; ++i)
    if (tolower(A[i]) != tolower(B[i]))
      return false;
    return true;
}

#endif
