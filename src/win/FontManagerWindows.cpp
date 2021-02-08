#include <windows.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
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

FontWeight get_font_weight(FT_Face face) {
  void* table = FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
  if (table == NULL) {
    return FontWeightUndefined;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return (FontWeight) os2_table->usWeightClass;
}

FontWidth get_font_width(FT_Face face) {
  void* table = FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
  if (table == NULL) {
    return FontWidthUndefined;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return (FontWidth) os2_table->usWidthClass;
}
FontDescriptor* descriptor_from_face(FT_Face &face, const char* path, int index) {
  FontDescriptor* res = NULL;

  res = new FontDescriptor(
    path,
    index,
    FT_Get_Postscript_Name(face) == NULL ? "" : FT_Get_Postscript_Name(face),
    face->family_name,
    face->style_name,
    get_font_weight(face),
    get_font_width(face),
    face->style_flags & FT_STYLE_FLAG_ITALIC,
    FT_IS_FIXED_WIDTH(face)
  );
  return res;
}

int scan_font_dir(HKEY which, bool data_is_path) {
  char win_dir[MAX_PATH];
  GetWindowsDirectoryA(win_dir, MAX_PATH);

  std::string font_dir;
  font_dir += win_dir;
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
    font_list.push_back(descriptor_from_face(face, font_path.c_str(), 0));
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
      font_list.push_back(descriptor_from_face(face, font_path.c_str(), i));
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

  if (desc->weight && desc->weight != result->weight)
    return false;

  if (desc->width && desc->width != result->width)
    return false;

  if (desc->italic != result->italic)
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
    "Malgun Gothic", // CJK (Korean)
    "PMingLiU", // CJK (Traditional Chinese)
    "SimSun", // CJK (Simplified Chinese)
    "Gulim", // CJK (Korean)
    "Yu Gothic", // CJK (Japanese)
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
