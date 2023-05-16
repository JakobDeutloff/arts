#pragma once

#include "matpack_view.h"

#include "rtepack_multitype.h"
#include "rtepack_propagation_matrix.h"
#include "rtepack_source.h"

#include <concepts>
#include <type_traits>
#include <vector>

namespace rtepack {
void two_level_exp(muelmat &t,
                   muelmat_vector_view &dt1,
                   muelmat_vector_view &dt2,
                   const propmat &k1,
                   const propmat &k2,
                   const propmat_vector_const_view &dk1,
                   const propmat_vector_const_view &dk2,
                   const Numeric r,
                   const ExhaustiveConstVectorView &dr1,
                   const ExhaustiveConstVectorView &dr2);
} // namespace rtepack
