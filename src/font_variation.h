#pragma once

#include <cpp11/data_frame.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/list_of.hpp>

[[cpp11::register]]
cpp11::writable::integers axes_to_tags(cpp11::strings axes);

[[cpp11::register]]
cpp11::writable::strings tags_to_axes(cpp11::integers tags);

[[cpp11::register]]
cpp11::writable::integers values_to_fixed(cpp11::doubles values);

[[cpp11::register]]
cpp11::writable::doubles fixed_to_values(cpp11::integers fixed);
