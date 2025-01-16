#ifndef FONT_DESCRIPTOR_H
#define FONT_DESCRIPTOR_H

#include <vector>
#include <cstring>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#include "utils.h"

enum FontWeight {
  FontWeightUndefined   = 0,
  FontWeightThin        = 100,
  FontWeightUltraLight  = 200,
  FontWeightLight       = 300,
  FontWeightNormal      = 400,
  FontWeightMedium      = 500,
  FontWeightSemiBold    = 600,
  FontWeightBold        = 700,
  FontWeightUltraBold   = 800,
  FontWeightHeavy       = 900
};

enum FontWidth {
  FontWidthUndefined      = 0,
  FontWidthUltraCondensed = 1,
  FontWidthExtraCondensed = 2,
  FontWidthCondensed      = 3,
  FontWidthSemiCondensed  = 4,
  FontWidthNormal         = 5,
  FontWidthSemiExpanded   = 6,
  FontWidthExpanded       = 7,
  FontWidthExtraExpanded  = 8,
  FontWidthUltraExpanded  = 9
};

inline FontWeight get_font_weight(FT_Face face) {
  void* table = FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
  if (table == NULL) {
    return FontWeightUndefined;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return (FontWeight) os2_table->usWeightClass;
}

inline FontWidth get_font_width(FT_Face face) {
  void* table = FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
  if (table == NULL) {
    return FontWidthUndefined;
  }
  TT_OS2* os2_table = (TT_OS2*) table;
  return (FontWidth) os2_table->usWidthClass;
}

struct FontDescriptor {
public:
  const char *path;
  int index;
  const char *postscriptName;
  const char *family;
  const char *style;
  FontWeight weight;
  FontWidth width;
  bool italic;
  bool monospace;

  FontDescriptor() {
    path = NULL;
    index = -1;
    postscriptName = NULL;
    family = NULL;
    style = NULL;
    weight = FontWeightUndefined;
    width = FontWidthUndefined;
    italic = false;
    monospace = false;
  }

  // Constructor added by Thomas Lin Pedersen
  FontDescriptor(const char *family, bool italic, bool bold) {
    this->path = NULL;
    this->index = -1;
    this->postscriptName = NULL;
    this->family = copyString(family);
    this->style = NULL;
    this->weight = bold ? FontWeightBold : FontWeightNormal;
    this->width = FontWidthUndefined;
    this->italic = italic;
    this->monospace = false;
  }

  // Constructor added by Thomas Lin Pedersen
  FontDescriptor(const char *family, bool italic, FontWeight weight, FontWidth width) {
    this->path = NULL;
    this->index = -1;
    this->postscriptName = NULL;
    this->family = copyString(family);
    this->style = NULL;
    this->weight = weight;
    this->width = width;
    this->italic = italic;
    this->monospace = false;
  }

  // Constructor added by Thomas Lin Pedersen
  FontDescriptor(FT_Face face, const char* path, int index) {
    this->path = copyString(path);
    this->index = index;
    this->postscriptName = FT_Get_Postscript_Name(face) == NULL ? "" : FT_Get_Postscript_Name(face);
    this->family = copyString(face->family_name);
    this->style = copyString(face->style_name);
    this->weight = get_font_weight(face);
    this->width = get_font_width(face);
    this->italic = face->style_flags & FT_STYLE_FLAG_ITALIC;
    this->monospace = FT_IS_FIXED_WIDTH(face);
  }

  FontDescriptor(const char *path, const char *postscriptName, const char *family, const char *style,
                 FontWeight weight, FontWidth width, bool italic, bool monospace) {
    this->path = copyString(path);
    this->index = 0;
    this->postscriptName = copyString(postscriptName);
    this->family = copyString(family);
    this->style = copyString(style);
    this->weight = weight;
    this->width = width;
    this->italic = italic;
    this->monospace = monospace;
  }

  FontDescriptor(const char *path, int index, const char *postscriptName, const char *family, const char *style,
                 FontWeight weight, FontWidth width, bool italic, bool monospace) {
    this->path = copyString(path);
    this->index = index;
    this->postscriptName = copyString(postscriptName);
    this->family = copyString(family);
    this->style = copyString(style);
    this->weight = weight;
    this->width = width;
    this->italic = italic;
    this->monospace = monospace;
  }

  FontDescriptor(FontDescriptor *desc) {
    path = copyString(desc->path);
    index = desc->index;
    postscriptName = copyString(desc->postscriptName);
    family = copyString(desc->family);
    style = copyString(desc->style);
    weight = desc->weight;
    width = desc->width;
    italic = desc->italic;
    monospace = desc->monospace;
  }

  const char* get_path() {
    return path == NULL ? "" : path;
  }

  const char* get_psname() {
    return postscriptName == NULL ? "" : postscriptName;
  }

  const char* get_family() {
    return family == NULL ? "" : family;
  }

  const char* get_style() {
    return style == NULL ? "" : style;
  }

  int get_weight() {
    switch (weight) {
    case FontWeightThin: return 1;
    case FontWeightUltraLight: return 2;
    case FontWeightLight: return 3;
    case FontWeightNormal: return 4;
    case FontWeightMedium: return 5;
    case FontWeightSemiBold: return 6;
    case FontWeightBold: return 7;
    case FontWeightUltraBold: return 8;
    case FontWeightHeavy: return 9;

    case FontWeightUndefined: return 0;
    }
    return 0;
  }

  int get_width() {
    switch (width) {
    case FontWidthUltraCondensed: return 1;
    case FontWidthExtraCondensed: return 2;
    case FontWidthCondensed: return 3;
    case FontWidthSemiCondensed: return 4;
    case FontWidthNormal: return 5;
    case FontWidthSemiExpanded: return 6;
    case FontWidthExpanded: return 7;
    case FontWidthExtraExpanded: return 8;
    case FontWidthUltraExpanded: return 9;

    case FontWidthUndefined: return 0;
    }
    return 0;
  }

  ~FontDescriptor() {
    if (path)
      delete[] path;

    if (postscriptName)
      delete[] postscriptName;

    if (family)
      delete[] family;

    if (style)
      delete[] style;

    postscriptName = NULL;
    family = NULL;
    style = NULL;
  }

  bool operator==(FontDescriptor& other) {
    if (postscriptName && !strcmp_no_case(postscriptName, other.postscriptName))
      return false;

    if (family && !strcmp_no_case(family, other.family))
      return false;

    if (style && !strcmp_no_case(style, other.style))
      return false;

    if (weight && weight != other.weight)
      return false;

    if (width && width != other.width)
      return false;

    if (italic != other.italic)
      return false;

    return true;
  }

  bool operator!=(FontDescriptor& other) {
    return !this->operator==(other);
  }

private:
  char *copyString(const char *input) {
    if (input == NULL) {
      return NULL;
    }

    char *str = new char[strlen(input) + 1];
    strcpy(str, input);
    return str;
  }
};

class ResultSet : public std::vector<FontDescriptor *> {
public:
  ~ResultSet() {
    for (ResultSet::iterator it = this->begin(); it != this->end(); it++) {
      delete *it;
    }
  }
  int n_fonts() {
    return size();
  }
};

#endif
