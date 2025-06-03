#include <windows.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../FontDescriptor.h"
#include "../utils.h"
#include "../font_matching.h"
#include "../emoji.h"

// A map for keeping font linking on Windows
typedef std::unordered_map<std::string, std::vector<std::string> > WinLinkMap;

ResultSet& get_font_list();
WinLinkMap& get_win_link_map();

WCHAR *utf8ToUtf16(const char *input) {
  unsigned int len = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
  WCHAR *output = new WCHAR[len];
  MultiByteToWideChar(CP_UTF8, 0, input, -1, output, len);
  return output;
}

char *utf16ToUtf8(const WCHAR *input) {
  unsigned int len = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
  char *output = new char[len];
  WideCharToMultiByte(CP_UTF8, 0, input, -1, output, len, NULL, NULL);
  return output;
}

int scan_font_dir(HKEY which, bool data_is_path, bool last_chance = false) {
  char win_dir[MAX_PATH];
  GetWindowsDirectoryA(win_dir, MAX_PATH);

  std::string font_dir;
  if (last_chance) {
    font_dir = "C:\\WINDOWS";
  } else {
    font_dir += win_dir;
  }
  font_dir += "\\Fonts\\";

  static const LPCSTR font_registry_path = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
  HKEY h_key;
  LONG result;

  result = RegOpenKeyExA(which, font_registry_path, 0, KEY_READ, &h_key);
  if (result != ERROR_SUCCESS) {
    return 1;
  }

  DWORD max_value_name_size, max_value_data_size;
  result = RegQueryInfoKey(h_key, 0, 0, 0, 0, 0, 0, 0, &max_value_name_size, &max_value_data_size, 0, 0);
  if (result != ERROR_SUCCESS) {
    return 1;
  }

  DWORD value_index = 0;
  LPSTR value_name = new CHAR[max_value_name_size];
  LPBYTE value_data = new BYTE[max_value_data_size];
  DWORD value_name_size, value_data_size, value_type;
  std::string font_path;

  ResultSet& font_list = get_font_list();
  FT_Library  library;
  FT_Face     face;
  FT_Error    error;
  error = FT_Init_FreeType(&library);
  if (error) {
    return 1;
  }

  do {
    // Loop over font registry, construct file path and parse with freetype
    value_data_size = max_value_data_size;
    value_name_size = max_value_name_size;

    result = RegEnumValueA(h_key, value_index, value_name, &value_name_size, 0, &value_type, value_data, &value_data_size);

    value_index++;

    if (!(result == ERROR_SUCCESS || result == ERROR_MORE_DATA) || value_type != REG_SZ) {
      continue;
    }
    font_path.clear();
    if (!data_is_path) {
      font_path += font_dir;
    }
    font_path.append((LPSTR) value_data, value_data_size);
    error = FT_New_Face(library,
                        font_path.c_str(),
                        0,
                        &face);
    if (error) {
      continue;
    }
    FT_MM_Var* variations = nullptr;
    int error = FT_Get_MM_Var(face, &variations);
    font_list.push_back(new FontDescriptor(face, font_path.c_str(), 0, error == 0 && variations->num_axis != 0));
    FT_Done_MM_Var(library, variations);
    int n_fonts = face->num_faces;
    FT_Done_Face(face);
    for (int i = 1; i < n_fonts; ++i) {
      error = FT_New_Face(library,
                          font_path.c_str(),
                          i,
                          &face);
      if (error) {
        continue;
      }
      font_list.push_back(new FontDescriptor(face, font_path.c_str(), i));
      FT_Done_Face(face);
    }
  } while (result != ERROR_NO_MORE_ITEMS);

  // Cleanup
  delete[] value_name;
  delete[] value_data;
  FT_Done_FreeType(library);

  return 0;
}
int scan_font_reg() {
  scan_font_dir(HKEY_LOCAL_MACHINE, false);
  scan_font_dir(HKEY_CURRENT_USER, true);

  // Move Arial Regular to front
  ResultSet& font_list = get_font_list();

  if (font_list.n_fonts() == 0) {
    scan_font_dir(HKEY_LOCAL_MACHINE, false, true);
  }

  for (ResultSet::iterator it = font_list.begin(); it != font_list.end(); it++) {
    if (strcmp((*it)->family, "Arial") == 0 && strcmp((*it)->style, "Regular") == 0) {
      FontDescriptor* arial = *it;
      font_list.erase(it);
      font_list.insert(font_list.begin(), arial);
      break;
    }
  }

  return 0;
}

void resetFontCache() {
  ResultSet& font_list = get_font_list();
  font_list.clear();
  WinLinkMap& font_links = get_win_link_map();
  font_links.clear();
}

ResultSet *getAvailableFonts() {
  ResultSet *res = new ResultSet();
  ResultSet& font_list = get_font_list();
  if (font_list.size() == 0) scan_font_reg();
  for (ResultSet::iterator it = font_list.begin(); it != font_list.end(); it++) {
    FontDescriptor* font = new FontDescriptor(*it);
    res->push_back(font);
  }
  return res;
}

bool resultMatches(FontDescriptor *result, FontDescriptor *desc) {
  if (desc->postscriptName && !strcmp_no_case(desc->postscriptName, result->postscriptName))
    return false;

  if (desc->family && !strcmp_no_case(desc->family, result->family))
    return false;

  if (desc->style && !strcmp_no_case(desc->style, result->style))
    return false;

  if (!result->var_wght && desc->weight && desc->weight != result->weight)
    return false;

  if (!result->var_wdth && desc->width && desc->width != result->width)
    return false;

  if (!result->var_ital && desc->italic != result->italic)
    return false;

  return true;
}

ResultSet *findFonts(FontDescriptor *desc) {
  ResultSet *res = new ResultSet();
  ResultSet& font_list = get_font_list();
  if (font_list.size() == 0) scan_font_reg();
  for (ResultSet::iterator it = font_list.begin(); it != font_list.end(); it++) {
    if (!resultMatches(*it, desc)) {
      continue;
    }
    FontDescriptor* font = new FontDescriptor(*it);
    res->push_back(font);
  }
  return res;
}

FontDescriptor *findFont(FontDescriptor *desc) {
  ResultSet *fonts = findFonts(desc);

  // if we didn't find anything, try again with postscriptName as family
  if (fonts->size() == 0) {
    delete fonts;

    desc->postscriptName = desc->family;
    desc->family = NULL;

    fonts = findFonts(desc);

    desc->family = desc->postscriptName;
    desc->postscriptName = NULL;
  }
  // if we didn't find anything, try again with only the font traits, no string names
  if (fonts->size() == 0) {
    delete fonts;

    FontDescriptor *fallback = new FontDescriptor(
      NULL, NULL, NULL, NULL,
      desc->weight, desc->width, desc->italic, false
    );

    fonts = findFonts(fallback);
  }

  // ok, nothing. shouldn't happen often.
  // just return the first available font
  if (fonts->size() == 0) {
    delete fonts;
    fonts = getAvailableFonts();
  }

  // hopefully we found something now.
  // copy and return the first result
  if (fonts->size() > 0) {
    FontDescriptor *res = new FontDescriptor(fonts->front());
    delete fonts;
    return res;
  }

  // whoa, weird. no fonts installed or something went wrong.
  delete fonts;
  return NULL;
}

bool font_has_glyphs(const char * font_path, int index, FT_Library &library, uint32_t * str, int n_chars) {
  FT_Face     face;
  FT_Error    error;
  error = FT_New_Face(library,
                      font_path,
                      index,
                      &face);
  if (error) {
    return false;
  }

  bool has_glyph = false;
  for (int i = 0; i < n_chars; ++i) {
    if (FT_Get_Char_Index( face, str[i])) {
      has_glyph = true;
      break;
    }
  }

  FT_Done_Face(face);
  return has_glyph;
}

int scan_link_reg() {
  WinLinkMap& font_links = get_win_link_map();
  if (font_links.size() != 0) {
    return 0;
  }

  static const LPCSTR link_registry_path = "Software\\Microsoft\\Windows NT\\CurrentVersion\\FontLink\\SystemLink";
  HKEY h_key;
  LONG result;

  result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, link_registry_path, 0, KEY_READ, &h_key);
  if (result != ERROR_SUCCESS) {
    return 1;
  }

  DWORD max_value_name_size, max_value_data_size;
  result = RegQueryInfoKey(h_key, 0, 0, 0, 0, 0, 0, 0, &max_value_name_size, &max_value_data_size, 0, 0);
  if (result != ERROR_SUCCESS) {
    return 1;
  }

  DWORD value_index = 0;
  LPSTR value_name = new CHAR[max_value_name_size];
  LPBYTE value_data = new BYTE[max_value_data_size];
  DWORD value_name_size, value_data_size, value_type;

  do {
    // Loop over font registry, construct file path and parse with freetype
    value_data_size = max_value_data_size;
    value_name_size = max_value_name_size;

    result = RegEnumValueA(h_key, value_index, value_name, &value_name_size, 0, &value_type, value_data, &value_data_size);

    value_index++;

    if (!(result == ERROR_SUCCESS || result == ERROR_MORE_DATA) || value_type != REG_MULTI_SZ) {
      continue;
    }
    std::string name((LPSTR) value_name, value_name_size);
    std::vector<std::string> values;
    unsigned char* value_cast = value_data;
    DWORD value_counter = 0;
    bool at_font_name = false;
    DWORD value_start = 0;
    bool ignore_subvalue = false;
    while (value_counter <= value_data_size) {
      if (value_cast[value_counter] == ',') {
        if (at_font_name) {
          ignore_subvalue = true;
        } else {
          at_font_name = true;
          value_start = value_counter + 1;
        }
      } else if (value_cast[value_counter] == '\0') {
        if (!ignore_subvalue) {
          values.emplace_back(reinterpret_cast<char*>(value_cast + value_start));
        }
        if (value_cast[value_counter + 1] == '\0') {
          break;
        }
        ignore_subvalue = false;
        at_font_name = false;
      }
      ++value_counter;
    }

    if (values.size() != 0) {
      font_links[name] = values;
    }
  } while (result != ERROR_NO_MORE_ITEMS);

  // Cleanup
  delete[] value_name;
  delete[] value_data;

  return 0;
}

FontDescriptor *substituteFont(char *postscriptName, char *string) {
  scan_link_reg();

  FontDescriptor *res = NULL;
  // find the font for the given postscript name
  FontDescriptor *desc = new FontDescriptor();
  desc->postscriptName = postscriptName;
  FontDescriptor *font = findFont(desc);
  desc->postscriptName = NULL;
  if (font == NULL) {
    delete desc;
    return font;
  }

  FT_Library library;
  FT_Error    error;
  error = FT_Init_FreeType( &library );
  if (error) {
    delete desc;
    return font;
  }

  UTF_UCS conv;
  int n_chars = 0;

  uint32_t* str = conv.convert(string, n_chars);

  // Does the provided one work?
  if (font->path != NULL && font_has_glyphs(font->path, font->index, library, str, n_chars)) {
    FT_Done_FreeType(library);
    delete desc;

    return font;
  }

  // Try emoji
  desc->family = EMOJI;
  res = findFont(desc);
  desc->family = NULL;
  if (res != NULL && font_has_glyphs(res->get_path(), res->index, library, str, n_chars)) {
    FT_Done_FreeType(library);
    delete desc;
    delete font;

    return res;
  }
  delete res;

  desc->weight = font->weight;
  desc->italic = font->italic;

  // Look for links
  WinLinkMap& font_links = get_win_link_map();
  std::string family(font->get_family());
  auto link = font_links.find(family);

  // If the font doesn't have links, try the different standard system fonts
  if (link == font_links.end()) {
    link = font_links.find("Segoe UI");
    if (link == font_links.end()) {
      link = font_links.find("Tahoma");
      if (link == font_links.end()) {
        link = font_links.find("Lucida Sans Unicode");
      }
    }
  }

  // hopefully some links were found
  if (link != font_links.end()) {
    for (auto it = link->second.begin(); it != link->second.end(); ++it) {
      desc->family = it->c_str();
      res = findFont(desc);
      desc->family = NULL;
      if (res != NULL && font_has_glyphs(res->get_path(), res->index, library, str, n_chars)) {
        FT_Done_FreeType(library);
        delete desc;
        delete font;

        return res;
      }
      delete res;
    }
  }

  // Still no match -> try some standard unicode fonts
  static std::vector<std::string> fallbacks = {
    "Segoe UI", // Latin, Greek, Cyrillic, Arabic
    "Arial Unicode MS", // Only installed with office AFAIK
    "Tahoma", // Latin, Greek, Cyrillic, Arabic, Hebrew, Thai
    "Meiryo UI", // CJK (Japanese)
    "MS UI Gothic", // CJK (Japanese)
    "Microsoft JhengHei UI", // CJK (Traditional Chinese)
    "Microsoft YaHei UI", // CJK (Simplified Chinese)
    "Microsoft Himalaya", // Tibetan
    "Microsoft New Tai Lue", // New Tai Lue
    "Microsoft PhagsPa", // Phags-Pa
    "Microsoft Sans Serif", // Latin, Greek, Cyrillic, Arabic, Hebrew, Thai, Vietnamese, Baltic, Turkish
    "Microsoft Tai Le", // Tai Le
    "Microsoft Yi Baiti", // Yi
    "Nirmala UI", // Many central asian scripts
    "Malgun Gothic", // CJK (Korean)
    "PMingLiU", // CJK (Traditional Chinese)
    "SimSun", // CJK (Simplified Chinese)
    "Gulim", // CJK (Korean)
    "Yu Gothic", // CJK (Japanese)
    "Leelawadee UI", // Thai
    "Ebrima", // African
    "Gadugi", // Cherokee
    "Javanese Text", // Javanese
    "Mongolian Baiti", // Mongolian
    "Myanmar Text", // Myanmar
    "Segoe UI Symbol"// Symbols
  };
  for (auto it = fallbacks.begin(); it != fallbacks.end(); ++it) {
    desc->family = it->c_str();
    res = findFont(desc);
    desc->family = NULL;
    if (res != NULL && font_has_glyphs(res->get_path(), res->index, library, str, n_chars)) {
      FT_Done_FreeType(library);
      delete desc;
      delete font;

      return res;
    }
    delete res;
  }

  // Really? We just return the input font

  FT_Done_FreeType(library);
  delete desc;
  return font;
}
