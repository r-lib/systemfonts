#pragma once

#include <cpp11/R.hpp>

int get_cached_face(const char* file, int index, double size, double res, void * face);

[[cpp11::init]]
void export_cache_store(DllInfo* dll);
