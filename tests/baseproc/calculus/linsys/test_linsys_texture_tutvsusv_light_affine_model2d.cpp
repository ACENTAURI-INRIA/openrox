//==============================================================================
//
//    OPENROX   : File test_linsys_tutvsusv_light_affine_texture_model2d.cpp
//
//    Contents  : Tests for linsys_tutvsusv_light_affine_texture_model2d.c
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

//=== INCLUDED HEADERS   =====================================================

#include <openrox_tests.hpp>

extern "C"
{
	#include <baseproc/calculus/linsys/linsys_texture_tutvsusv_light_affine_model2d.h>
}

//=== INTERNAL MACROS    =====================================================

ROX_TEST_SUITE_BEGIN ( linsys_tutvsusv_light_affine_texture_model2d )

//=== INTERNAL TYPESDEFS =====================================================

//=== INTERNAL DATATYPES =====================================================

//=== INTERNAL VARIABLES =====================================================

//=== INTERNAL FUNCTDEFS =====================================================

//=== INTERNAL FUNCTIONS =====================================================

//=== EXPORTED FUNCTIONS =====================================================


ROX_TEST_CASE_DECLARE(rox::OpenROXTest, test_jacobian_tutvsusv_light_affine_premul)
{
	Rox_ErrorCode error = ROX_ERROR_NONE;

	ROX_TEST_MESSAGE ( "This test has not been implemented yet !!! \n" );

   ROX_TEST_CHECK_EQUAL ( error, ROX_ERROR_NONE );
}

ROX_TEST_SUITE_END()
