#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <limits.h>

// The exact location of a single file
struct FontLoc {
  std::string file;
  unsigned int index;
};
// Settings related to a single OpenType feature (all feature ids are 4 char long)
struct FontFeature {
  char feature[4];
  int setting;
};
// A collection of four fonts (plain, bold, italic, bold-italic) along with optional features
struct FontCollection {
  FontLoc fonts[4];
  std::vector<FontFeature> features;
};
// A structure to pass around a single font with features (used by the C interface)
struct FontSettings {
  char file[PATH_MAX + 1];
  unsigned int index;
  const FontFeature* features;
  int n_features;
};
// A collection of registered fonts
typedef std::unordered_map<std::string, FontCollection> FontReg;
// A map of Emoji unicode points
typedef std::unordered_map<uint32_t, uint8_t> EmojiMap;
// A map for keeping font linking on Windows
typedef std::unordered_map<std::string, std::vector<std::string> > WinLinkMap;

// Key for looking up cached font locations
struct FontKey {
  std::string family;
  int bold;
  int italic;
  
  FontKey() : family(""), bold(0), italic(0) {}
  FontKey(std::string _family) : family(_family), bold(0), italic(0) {}
  FontKey(std::string _family, int _bold, int _italic) : family(_family), bold(_bold), italic(_italic) {}
  
  inline bool operator==(const FontKey &other) const { 
    return (bold == other.bold && italic == other.italic && family == other.family);
  }
};
namespace std {
template <> 
struct hash<FontKey> {
  size_t operator()(const FontKey & x) const {
    return std::hash<std::string>()(x.family) ^ std::hash<int>()(x.bold) ^std::hash<int>()(x.italic);
  }
};
}
// Map for keeping already resolved font locations
typedef std::unordered_map<FontKey, FontLoc> FontMap;
