#pragma once

#include <cpp11/list.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/integers.hpp>
#include <cpp11/doubles.hpp>
#include <cpp11/logicals.hpp>

[[cpp11::register]]
cpp11::list get_string_shape_c(cpp11::strings string, cpp11::integers id, 
                               cpp11::strings path, cpp11::integers index, 
                               cpp11::doubles size, cpp11::doubles res, 
                               cpp11::doubles lineheight, cpp11::integers align, 
                               cpp11::doubles hjust, cpp11::doubles vjust, 
                               cpp11::doubles width, cpp11::doubles tracking, 
                               cpp11::doubles indent, cpp11::doubles hanging, 
                               cpp11::doubles space_before, cpp11::doubles space_after);

[[cpp11::register]]
cpp11::doubles get_line_width_c(cpp11::strings string, cpp11::strings path, 
                                cpp11::integers index, cpp11::doubles size, 
                                cpp11::doubles res, cpp11::logicals include_bearing);

int string_width(const char* string, const char* fontfile, int index, 
                 double size, double res, int include_bearing, double* width);

int string_shape(const char* string, const char* fontfile, int index, 
                 double size, double res, double* x, double* y, unsigned int max_length);

[[cpp11::init]]
void export_string_metrics(DllInfo* dll);
