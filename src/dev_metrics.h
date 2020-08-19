#pragma once

#include <cpp11/doubles.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/data_frame.hpp>

[[cpp11::register]]
cpp11::doubles dev_string_widths_c(cpp11::strings string, cpp11::strings family, 
                                   cpp11::integers face, cpp11::doubles size, 
                                   cpp11::doubles cex, cpp11::integers unit);

[[cpp11::register]]
cpp11::writable::data_frame dev_string_metrics_c(cpp11::strings string, 
                                                 cpp11::strings family, 
                                                 cpp11::integers face, 
                                                 cpp11::doubles size, 
                                                 cpp11::doubles cex, 
                                                 cpp11::integers unit);
