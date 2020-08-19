#pragma once

#include <cpp11/doubles.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/data_frame.hpp>

using namespace cpp11;

[[cpp11::register]]
doubles dev_string_widths_c(strings string, strings family, integers face, doubles size, doubles cex, integers unit);

[[cpp11::register]]
writable::data_frame dev_string_metrics_c(strings string, strings family, integers face, doubles size, doubles cex, integers unit);
