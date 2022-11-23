#pragma once

#if defined(_OPENMP) && !defined(_DISABLE_OMP)

#define PARALLEL_ENABLED
#include <omp.h>

#endif
