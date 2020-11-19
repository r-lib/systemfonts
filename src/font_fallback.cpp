#include <vector>
#include <cpp11/named_arg.hpp>

#include "font_fallback.h"
#include "FontDescriptor.h"
#include "ft_cache.h"
#include "caches.h"

using namespace cpp11::literals;

// these functions are implemented by the platform
FontDescriptor *substituteFont(char *, char *);

FontDescriptor *fallback_font(const char* file, int index, const char* string) {
  FreetypeCache& cache = get_font_cache();
  if (!cache.load_font(file, index)) {
    return NULL;
  }
  std::string font_name = cache.cur_name();
  std::vector<char> writable_name(font_name.begin(), font_name.end());
  writable_name.push_back('\0');
  std::vector<char> writable_string(string, string + std::strlen(string));
  writable_string.push_back('\0');
  return substituteFont(writable_name.data(), writable_string.data());
}

cpp11::writable::data_frame get_fallback_c(cpp11::strings path, cpp11::integers index, cpp11::strings string) {
  bool one_path = path.size() == 1;
  const char* first_path = Rf_translateCharUTF8(path[0]);
  int first_index = index[0];
  bool one_string = string.size() == 1;
  const char* first_string = Rf_translateCharUTF8(string[0]);
  int full_length = 1;
  if (!one_path) full_length = path.size();
  else if (!one_string) full_length = string.size();
  
  cpp11::writable::strings paths;
  paths.reserve(full_length);
  cpp11::writable::integers indices;
  indices.reserve(full_length);
  
  for (int i = 0; i < full_length; ++i) {
    FontDescriptor* fallback = fallback_font(
      one_path ? first_path : Rf_translateCharUTF8(path[i]),
      one_path ? first_index : index[i],
      one_string ? first_string : Rf_translateCharUTF8(string[i])
    );
    if (fallback == NULL) {
      paths.push_back(R_NaString);
      indices.push_back(R_NaInt);
    } else {
      paths.push_back(fallback->path);
      indices.push_back(fallback->index);
    }
    delete fallback;
  }
  return cpp11::writable::data_frame({
    "path"_nm = paths,
    "index"_nm = indices
  });
}

FontSettings request_fallback(const char *string, const char *path, int index) {
  FontDescriptor *fallback = fallback_font(path, index, string);
  FontSettings result = {};
  if (fallback == NULL) {
    std::strncpy(result.file, path, PATH_MAX);
    result.index = index;
  } else {
    std::strncpy(result.file, fallback->path, PATH_MAX);
    result.index = fallback->index;
  }
  delete fallback;
  return result;
}

void export_font_fallback(DllInfo* dll) {
  R_RegisterCCallable("systemfonts", "get_fallback", (DL_FUNC)request_fallback);
}
