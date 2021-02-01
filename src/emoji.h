#pragma once

#include <unordered_map>
#include <cstdint>

#include <cpp11/integers.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/list.hpp>
#include <R_ext/Rdynload.h>

typedef std::unordered_map<uint32_t, uint8_t> EmojiMap;

bool has_emoji(const char* string);

void detect_emoji_embedding(const uint32_t* codepoints, int n, int* embedding, const char* fontpath, int index);

[[cpp11::register]]
void load_emoji_codes_c(cpp11::integers all, cpp11::integers default_text, cpp11::integers base_mod);

[[cpp11::register]]
cpp11::list emoji_split_c(cpp11::strings string, cpp11::strings path, cpp11::integers index);

[[cpp11::init]]
void export_emoji_detection(DllInfo* dll);
