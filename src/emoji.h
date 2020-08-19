#pragma once

#include <unordered_map>
#include <cstdint>

#include <cpp11/integers.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/list.hpp>

using namespace cpp11;

typedef std::unordered_map<uint32_t, uint8_t> EmojiMap;

[[cpp11::register]]
void load_emoji_codes_c(integers all, integers default_text, integers base_mod);

[[cpp11::register]]
list emoji_split_c(strings string, strings path, integers index);
