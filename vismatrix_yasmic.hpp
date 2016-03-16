#ifndef VISMATRIX_YASMIC
#define VISMATRIX_YASMIC

/**
 * @file vismatrix_yasmic.hpp
 * This file contains all of the yasmic includes for vismatrix.
 * This function simplifies the code somewhat because 
 * then we just have to include this function to get yasmic included.
 */


#undef max
#undef min

#include <yasmic/compressed_row_matrix.hpp>
#include <yasmic/util/load_crm_matrix.hpp>

#include <yasmic/transpose_matrix.hpp>
#include <yasmic/nonzero_union.hpp>

#endif // VISMATRIX_YASMIC


