#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <limits.h>
#include <cstring>

// The exact location of a single file
struct FontLoc {
  std::string file;
  unsigned int index;
  std::vector<int> axes;
  std::vector<int> coords;
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

  FontSettings() : index(0), features(nullptr), n_features(0) {
    file[0] = '\0';
  }
};
// A structure to pass around a single font with features and variable axes (used by the C interface)
struct FontSettings2 : public FontSettings {
  const int* axes;
  const int* coords;
  int n_axes;

  FontSettings2() : axes(nullptr), coords(nullptr), n_axes(0) {

  }
  FontSettings2(FontSettings x) : axes(nullptr), coords(nullptr), n_axes(0) {
    strncpy(file, x.file, PATH_MAX + 1);
    index = x.index;
    features = x.features;
    n_features = x.n_features;
  }
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
  int weight;
  int width;
  int italic;

  FontKey() : family(""), weight(400), width(5), italic(0) {}
  FontKey(std::string _family) : family(_family), weight(400), width(5), italic(0) {}
  FontKey(std::string _family, int _weight, int _width, int _italic) : family(_family), weight(_weight), width(_width), italic(_italic) {}

  inline bool operator==(const FontKey &other) const {
    return (weight == other.weight && width == other.width && italic == other.italic && family == other.family);
  }
};
namespace std {
template <>
struct hash<FontKey> {
  size_t operator()(const FontKey & x) const {
    return std::hash<std::string>()(x.family) ^ std::hash<int>()(x.weight) ^ std::hash<int>()(x.width) ^ std::hash<int>()(x.italic);
  }
};
}
// Map for keeping already resolved font locations
typedef std::unordered_map<FontKey, FontLoc> FontMap;
