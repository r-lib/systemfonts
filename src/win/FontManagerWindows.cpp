#define WINVER 0x0600

#include <windows.h>
#include <sstream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include "../FontDescriptor.h"

ResultSet* get_font_list();

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
FontDescriptor* descriptor_from_face(FT_Face &face, const char* path) {
  FontDescriptor* res = NULL;

  res = new FontDescriptor(
    path,
    FT_Get_Postscript_Name(face),
    face->family_name,
    face->style_name,
    get_font_weight(face),
    get_font_width(face),
    face->style_flags & FT_STYLE_FLAG_ITALIC,
    FT_IS_FIXED_WIDTH(face)
  );
  return res;
}

int scan_font_dir() {
  char win_dir[MAX_PATH];
  GetWindowsDirectory(win_dir, MAX_PATH);

  std::stringstream font_dir;
  font_dir << win_dir << "\\Fonts\\";

  WIN32_FIND_DATA FindFileData;
  HANDLE hFind = FindFirstFile(font_dir.str().c_str(), &FindFileData);
  if (hFind == INVALID_HANDLE_VALUE) {
    return 1;
  }

  ResultSet* font_list = get_font_list();
  FT_Library  library;
  FT_Face     face;
  FT_Error    error;
  error = FT_Init_FreeType( &library );
  if (error) {
    return 1;
  }

  do {
    if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
    std::stringstream font_path;
    font_path << font_dir << FindFileData.cFileName;
    error = FT_New_Face(library,
                        font_path.str().c_str(),
                        0,
                        &face);
    if ( error ) {
      continue;
    }
    font_list->push_back(descriptor_from_face(face, font_path.str().c_str()));
  } while (FindNextFile(hFind, &FindFileData) != 0);

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  FindClose(hFind);

  // Move Helvetica to front (could be better probably)
  for (ResultSet::iterator it = font_list->begin(); it != font_list->end(); it++) {
    if (strcmp((*it)->family, "Helvetica")) {
      FontDescriptor* p_helvetica = *it;
      font_list->erase(it);
      font_list->insert(font_list->begin(), p_helvetica);
    }
  }

  return 0;
}

ResultSet *getAvailableFonts() {
  ResultSet *res = new ResultSet();
  ResultSet* font_list = get_font_list();
  if (font_list->size() == 0) scan_font_dir();
  for (ResultSet::iterator it = font_list->begin(); it != font_list->end(); it++) {
    FontDescriptor* font = new FontDescriptor(*it);
    res->push_back(font);
  }
  return res;
}
bool strcmp_no_case(const char * A, const char * B) {
  unsigned int a_len = strlen(A);
  if (strlen(B) != a_len)
    return false;
  for (unsigned int i = 0; i < a_len; ++i)
    if (tolower(A[i]) != tolower(B[i]))
      return false;
    return true;
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

  if (desc->monospace != result->monospace)
    return false;

  return true;
}

ResultSet *findFonts(FontDescriptor *desc) {
  ResultSet *res = new ResultSet();
  ResultSet* font_list = get_font_list();
  if (font_list->size() == 0) scan_font_dir();
  for (ResultSet::iterator it = font_list->begin(); it != font_list->end(); it++) {
    if (!resultMatches(*it, desc)) continue;
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

bool font_has_glyphs(const char * font_path, FT_Library library, WCHAR * str) {
  FT_Face     face;
  FT_Error    error;
  error = FT_New_Face(library,
                      font_path,
                      0,
                      &face);
  if (error) {
    return false;
  }
  int i = 0;
  while (str[i]) {
    if (FT_Get_Char_Index( face, str[i])) {
      return false;
    }
    i++;
  }
  FT_Done_Face(face);
  return true;
}

FontDescriptor *substituteFont(char *postscriptName, char *string) {
  FontDescriptor *res = NULL;
  // find the font for the given postscript name
  FontDescriptor *desc = new FontDescriptor();
  desc->postscriptName = postscriptName;
  FontDescriptor *font = findFont(desc);
  desc->postscriptName = NULL;
  desc->weight = font->weight;
  desc->width = font->width;
  desc->italic = font->italic;
  desc->monospace = font->monospace;
  ResultSet* style_matches = findFonts(desc);

  WCHAR *str = utf8ToUtf16(string);

  FT_Library library;
  FT_Error    error;
  error = FT_Init_FreeType( &library );
  if (error) {
    return res;
  }

  for (ResultSet::iterator it = style_matches->begin(); it != style_matches->end(); it++) {
    if (font_has_glyphs((*it)->path, library, str)) {
      res = new FontDescriptor(*it);
      break;
    }
  }

  FT_Done_FreeType(library);
  delete str;
  delete desc;
  delete font;

  return res;
}
