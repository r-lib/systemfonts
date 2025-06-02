#include <Foundation/Foundation.h>
#include <CoreText/CoreText.h>
#include <string>
#include <map>
#include "../FontDescriptor.h"

// converts a CoreText weight (-1 to +1) to a standard weight (100 to 900)
// See https://chromium.googlesource.com/chromium/src/+/master/ui/gfx/platform_font_mac.mm
// for conversion
static int convertWeight(float weight) {
  if (weight <= -0.7f)
    return 100;
  else if (weight <= -0.45f)
    return 200;
  else if (weight <= -0.1f)
    return 300;
  else if (weight <= 0.1f)
    return 400;
  else if (weight <= 0.27f)
    return 500;
  else if (weight <= 0.35f)
    return 600;
  else if (weight <= 0.5f)
    return 700;
  else if (weight <= 0.6f)
    return 800;
  else
    return 900;
}

// converts a CoreText width (-1 to +1) to a standard width (1 to 9)
static int convertWidth(float unit) {
  if (unit < 0) {
    return 1 + (1 + unit) * 4;
  } else {
    return 5 + unit * 4;
  }
}

void addFontIndex(FontDescriptor* font) { @autoreleasepool {
  static std::map<std::string, int> font_index;

  if (font->variable) {
    font->index = 0;
    return;
  }

  std::string font_name(font->postscriptName);
  int font_no;
  std::map<std::string, int>::iterator it = font_index.find(font_name);
  if (it == font_index.end()) {
    NSString *font_path = [NSString stringWithUTF8String:font->path];
    NSURL *font_url = [NSURL fileURLWithPath: font_path];
    CFArrayRef font_descriptors = CTFontManagerCreateFontDescriptorsFromURL((CFURLRef) font_url);
    if (font_descriptors == NULL) {
      font_no = 0;
      font_index[font_name] = 0;
    } else {
      int n_fonts = CFArrayGetCount(font_descriptors);
      if (n_fonts == 1) {
        font_no = 0;
        font_index[font_name] = 0;
      } else {
        for (int i = 0; i < n_fonts; i++) {
          CTFontDescriptorRef font_at_i = (CTFontDescriptorRef) CFArrayGetValueAtIndex(font_descriptors, i);
          std::string font_name_at_i = [(__bridge_transfer NSString *) CTFontDescriptorCopyAttribute(font_at_i, kCTFontNameAttribute) UTF8String];
          font_index[font_name_at_i] = i;
          if (font_name.compare(font_name_at_i) == 0) {
            font_no = i;
          }
        }
      }
      CFRelease(font_descriptors);
    }
  } else {
    font_no = (*it).second;
  }
  font->index = font_no;
}}

FontDescriptor *createFontDescriptor(CTFontDescriptorRef descriptor) { @autoreleasepool {
  NSURL *url = (__bridge_transfer NSURL *) CTFontDescriptorCopyAttribute(descriptor, kCTFontURLAttribute);
  NSString *psName = (__bridge_transfer NSString *) CTFontDescriptorCopyAttribute(descriptor, kCTFontNameAttribute);
  NSString *family = (__bridge_transfer NSString *) CTFontDescriptorCopyAttribute(descriptor, kCTFontFamilyNameAttribute);
  NSString *style = (__bridge_transfer NSString *) CTFontDescriptorCopyAttribute(descriptor, kCTFontStyleNameAttribute);

  NSDictionary *traits = (__bridge_transfer NSDictionary *) CTFontDescriptorCopyAttribute(descriptor, kCTFontTraitsAttribute);
  NSNumber *weightVal = traits[(id)kCTFontWeightTrait];
  FontWeight weight = (FontWeight) convertWeight([weightVal floatValue]);

  NSNumber *widthVal = traits[(id)kCTFontWidthTrait];
  FontWidth width = (FontWidth) convertWidth([widthVal floatValue]);

  NSNumber *symbolicTraitsVal = traits[(id)kCTFontSymbolicTrait];
  unsigned int symbolicTraits = [symbolicTraitsVal unsignedIntValue];

  CFArrayRef variations = CTFontCopyVariationAxes(CTFontCreateWithFontDescriptor(descriptor, 0.0, NULL));

  FontDescriptor *res = new FontDescriptor(
    [[url path] UTF8String],
    [psName UTF8String],
    [family UTF8String],
    [style UTF8String],
    weight,
    width,
    (symbolicTraits & kCTFontItalicTrait) != 0,
    (symbolicTraits & kCTFontMonoSpaceTrait) != 0,
    variations != nullptr && CFArrayGetCount(variations) != 0
  );
  addFontIndex(res);
  return res;
}}

static CTFontCollectionRef collection = NULL;
void resetFontCache() {
  if (collection != NULL) {
    CTFontCollectionRef temp = collection;
    collection = NULL;
    CFRelease(temp);
  }
}

ResultSet *getAvailableFonts() { @autoreleasepool {
  // cache font collection for fast use in future calls
  if (collection == NULL)
    collection = CTFontCollectionCreateFromAvailableFonts(NULL);

  NSArray *matches = (__bridge_transfer NSArray *) CTFontCollectionCreateMatchingFontDescriptors(collection);
  ResultSet *results = new ResultSet();

  for (id m in matches) {
    CTFontDescriptorRef match = (__bridge CTFontDescriptorRef) m;
    results->push_back(createFontDescriptor(match));
  }

  return results;
}}

// helper to square a value
static inline int sqr(int value) {
  return value * value;
}

CTFontDescriptorRef getFontDescriptor(FontDescriptor *desc) {
  // build a dictionary of font attributes
  NSMutableDictionary *attrs = [NSMutableDictionary dictionary];
  CTFontSymbolicTraits symbolicTraits = 0;

  if (desc->postscriptName) {
    NSString *postscriptName = [NSString stringWithUTF8String:desc->postscriptName];
    attrs[(id)kCTFontNameAttribute] = postscriptName;
  }

  if (desc->family) {
    NSString *family = [NSString stringWithUTF8String:desc->family];
    attrs[(id)kCTFontFamilyNameAttribute] = family;
  }

  if (desc->style) {
    NSString *style = [NSString stringWithUTF8String:desc->style];
    attrs[(id)kCTFontStyleNameAttribute] = style;
  }

  // build symbolic traits
  if (desc->italic)
    symbolicTraits |= kCTFontItalicTrait;

  if (desc->weight == FontWeightBold)
    symbolicTraits |= kCTFontBoldTrait;

  if (desc->monospace)
    symbolicTraits |= kCTFontMonoSpaceTrait;

  if (desc->width == FontWidthCondensed)
    symbolicTraits |= kCTFontCondensedTrait;

  if (desc->width == FontWidthExpanded)
    symbolicTraits |= kCTFontExpandedTrait;

  if (symbolicTraits) {
    NSDictionary *traits = @{(id)kCTFontSymbolicTrait:[NSNumber numberWithUnsignedInt:symbolicTraits]};
    attrs[(id)kCTFontTraitsAttribute] = traits;
  }

  // create a font descriptor and search for matches
  return CTFontDescriptorCreateWithAttributes((CFDictionaryRef) attrs);
}

int metricForMatch(CTFontDescriptorRef match, FontDescriptor *desc) { @autoreleasepool {
  NSDictionary *dict = (__bridge_transfer NSDictionary *)CTFontDescriptorCopyAttribute(match, kCTFontTraitsAttribute);

  bool italic = ([dict[(id)kCTFontSymbolicTrait] unsignedIntValue] & kCTFontItalicTrait);

  // See if font has variations for certain traits. If so, ignore these in the metric
  bool has_var_weight = false;
  bool has_var_width = false;
  bool has_var_italic = false;
  CFArrayRef variations = CTFontCopyVariationAxes(CTFontCreateWithFontDescriptor(match, 0.0, NULL));
  if (variations != nullptr) {
    long weight_tag = 2003265652;
    long width_tag = 2003072104;
    long italic_tag = 1769234796;
    long tag_val = 0;
    for (CFIndex i = 0; i < CFArrayGetCount(variations); ++i) {
      CFDictionaryRef axis = (CFDictionaryRef)CFArrayGetValueAtIndex(variations, i);
      CFNumberRef tag = (CFNumberRef) CFDictionaryGetValue(axis, kCTFontVariationAxisIdentifierKey);
      CFNumberGetValue(tag, kCFNumberLongType, &tag_val);
      if (tag_val == weight_tag) has_var_weight = true;
      else if (tag_val == width_tag) has_var_width = true;
      else if (tag_val == italic_tag) has_var_italic = true;
    }
  }

  // normalize everything to base-900
  int metric = 0;
  if (!has_var_weight && desc->weight)
    metric += sqr(convertWeight([dict[(id)kCTFontWeightTrait] floatValue]) - desc->weight);

  if (!has_var_width && desc->width)
    metric += sqr((convertWidth([dict[(id)kCTFontWidthTrait] floatValue]) - desc->width) * 100);

  if (!has_var_italic)
    metric += sqr((italic != desc->italic) * 900);

  return metric;
}}

ResultSet *findFonts(FontDescriptor *desc) { @autoreleasepool {
  CTFontDescriptorRef descriptor = getFontDescriptor(desc);
  NSArray *matches = (__bridge_transfer NSArray *) CTFontDescriptorCreateMatchingFontDescriptors(descriptor, NULL);
  ResultSet *results = new ResultSet();

  NSArray *sorted = [matches sortedArrayUsingComparator:^NSComparisonResult(id a, id b) {
    int ma = metricForMatch((__bridge CTFontDescriptorRef) a, desc);
    int mb = metricForMatch((__bridge CTFontDescriptorRef) b, desc);
    return ma < mb ? NSOrderedAscending : ma > mb ? NSOrderedDescending : NSOrderedSame;
  }];

  for (id m in sorted) {
    CTFontDescriptorRef match = (__bridge CTFontDescriptorRef) m;
    int mb = metricForMatch((__bridge CTFontDescriptorRef) m, desc);

    if (mb < 10000) {
      results->push_back(createFontDescriptor(match));
    }
  }

  CFRelease(descriptor);
  return results;
}}

CTFontDescriptorRef findBest(FontDescriptor *desc, NSArray *matches) {
  // find the closest match for width and weight attributes
  CTFontDescriptorRef best = NULL;
  int bestMetric = INT_MAX;

  for (id m in matches) {
    int metric = metricForMatch((__bridge CTFontDescriptorRef) m, desc);

    if (metric < bestMetric) {
      bestMetric = metric;
      best = (__bridge CTFontDescriptorRef) m;
    }

    // break if this is an exact match
    if (metric == 0)
      break;
  }

  return best;
}

FontDescriptor *findFont(FontDescriptor *desc) { @autoreleasepool {
  FontDescriptor *res = NULL;
  CTFontDescriptorRef descriptor = getFontDescriptor(desc);
  NSArray *matches = (__bridge_transfer NSArray *) CTFontDescriptorCreateMatchingFontDescriptors(descriptor, NULL);

  // if there was no match, try again but use family as postscriptName
  if ([matches count] == 0) {
    desc->postscriptName = desc->family;
    desc->family = NULL;
    CTFontDescriptorRef descriptor_ps = getFontDescriptor(desc);
    matches = (__bridge_transfer NSArray *) CTFontDescriptorCreateMatchingFontDescriptors(descriptor_ps, NULL);
    desc->family = desc->postscriptName;
    desc->postscriptName = NULL;
  }
  // if there was no match, try again but only try to match traits
  if ([matches count] == 0) {
    NSSet *set = [NSSet setWithObjects:(id)kCTFontTraitsAttribute, nil];
    matches = (__bridge_transfer NSArray *) CTFontDescriptorCreateMatchingFontDescriptors(descriptor, (CFSetRef) set);
  }

  // find the closest match for width and weight attributes
  CTFontDescriptorRef best = findBest(desc, matches);

  // if we found a match, generate and return a URL for it
  if (best) {
    res = createFontDescriptor(best);
  }

  CFRelease(descriptor);
  return res;
}}

FontDescriptor *substituteFont(char *postscriptName, char *string) { @autoreleasepool {
  FontDescriptor *res = NULL;

  // create a font descriptor to find the font by its postscript name
  // we don't use CTFontCreateWithName because that supports font
  // names other than the postscript name but prints warnings.
  NSString *ps = [NSString stringWithUTF8String:postscriptName];
  NSDictionary *attrs = @{(id)kCTFontNameAttribute: ps};
  CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes((CFDictionaryRef) attrs);
  CTFontRef font = CTFontCreateWithFontDescriptor(descriptor, 12.0, NULL);

  // find a substitute font that support the given characters
  NSString *str = [NSString stringWithUTF8String:string];
  CTFontRef substituteFont = CTFontCreateForString(font, (CFStringRef) str, CFRangeMake(0, [str length]));
  CTFontDescriptorRef substituteDescriptor = CTFontCopyFontDescriptor(substituteFont);

  // finally, create and return a result object for this substitute font
  res = createFontDescriptor(substituteDescriptor);

  CFRelease(descriptor);
  CFRelease(font);
  CFRelease(substituteFont);
  CFRelease(substituteDescriptor);

  return res;
}}
