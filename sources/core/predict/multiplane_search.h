//==============================================================================
//
//    OPENROX   : File multiplane_search.h
//
//    Contents  : API of multiplane_search module
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#ifndef __OPENROX_MULTIPLANE_SEARCH__
#define __OPENROX_MULTIPLANE_SEARCH__

#include <generated/array2d_uchar.h>
#include <generated/array2d_double.h>
#include <generated/array2d_float.h>
#include <generated/array2d_uint.h>
#include <generated/array2d_point2d_float.h>
#include <baseproc/maths/linalg/matsl3.h>

//! \ingroup Identification
//! \addtogroup Plane_Search
//! @{

//! The Rox_MultiPlane_Search_Struct object 
struct Rox_MultiPlane_Search_Struct
{
   //! The templates to search 
   Rox_DynVec_Array2D_Float model;

   //! The model mask 
   Rox_DynVec_Array2D_Uint model_mask;

   //! The search warped image 
   Rox_Array2D_Float search;

   //! The current warped mask 
   Rox_Imask search_mask;

   //! The grid to warp the current image into the search image
   Rox_Array2D_Point2D_Float map;
   
   //! The homography to warp the reference image into the template image
   Rox_MatSL3 r_G_t;
   
   //! The homography to warp the current image into the template image
   Rox_MatSL3 c_G_t;
   
   //! The homography to warp the current image into the search image
   Rox_MatSL3 c_G_s;
   
   //! The estimated homography to warp the search image into the template
   Rox_MatSL3 s_G_t;

   //! The search radius in pixels 
   Rox_Sint search_radius;

   //! The zncc score for the best estimated position
   Rox_Float score;

   //! The estimated shift along u-axis
   Rox_Double shift_u;
   
   //! The estimated shift along v-axis
   Rox_Double shift_v;
};

//! Define the pointer of the Rox_Plane_Search_Struct 
typedef struct Rox_Plane_Search_Struct * Rox_Plane_Search;

//! Create plane searching object
//! \param  [out]  plane_search      The pointer to the plane searching object
//! \param  [in ]  model_rows        The patch rows in pixels
//! \param  [in ]  model_cols        The patch cols in pixels
//! \param  [in ]  search_radius     The search radius
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_new (
   Rox_Plane_Search * plane_search, 
   const Rox_Sint model_rows, 
   const Rox_Sint model_cols, 
   const Rox_Sint search_radius
);

//! Delete plane searching object
//! \param  [out]  plane_search     The pointer to the plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_del ( 
   Rox_Plane_Search * plane_search
);

//! Set the model image (template) and its mask (the homography r_G_t = I by default)
//! \param  [out]  plane_search      The plane searching object
//! \param  [in ]  model             The model image
//! \param  [in ]  model_mask        The model mask
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_set_model (
   Rox_Plane_Search plane_search, 
   const Rox_Array2D_Float model, 
   const Rox_Imask model_mask
);

//! Set the model image (template) and its mask 
//! \param  [out]  plane_search      The plane searching object
//! \param  [in ]  reference_image   The reference image
//! \param  [in ]  r_G_t             The homography r_G_t to warp the reference image Ic into the search image Is 
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_set_model_warp (
   Rox_Plane_Search plane_search, 
   const Rox_Array2D_Float reference_image, 
   const Rox_MatSL3 r_G_t
);

//! Search the model image in the current image
//! \param  [in ]  plane_search      The plane searching object
//! \param  [in ]  current_image     The image to search into
//! \param  [in ]  c_G_s             The homography c_G_s to warp the curent image Ic into the search image Is 
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_make (
   Rox_Plane_Search plane_search, 
   const Rox_Array2D_Float current_image, 
   const Rox_MatSL3 c_G_s
);

//! Get result shift for the last "make" (do not use if no valid make was done)
//! \param  [out]  shift_u           The shift along the u coordinates in the plane space
//! \param  [out]  shift_v           The shift along the v coordinates in the plane space
//! \param  [in ]  plane_search      The plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_get_shift ( 
   Rox_Double * shift_u, 
   Rox_Double * shift_v, 
   Rox_Plane_Search const plane_search);

//! Get result score for the last "make" (do not use if no valid make was done)
//! \param  [out]  score             The score of the last search
//! \param  [in ]  plane_search      The plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_get_score ( 
   Rox_Double * score, 
   const Rox_Plane_Search plane_search
);

//! Get results for the last "make" (do not use if no valid make was done)
//! \param  [out]  score             The score of the last search
//! \param  [out]  shift_u           The shift along the u coordinates in the plane space
//! \param  [out]  shift_v           The shift along the v coordinates in the plane space
//! \param  [in]   plane_search      The plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_get_results (
   Rox_Double * score, 
   Rox_Double * shift_u, 
   Rox_Double * shift_v, 
   const Rox_Plane_Search 
   plane_search
);

//! Update pose given last make results : c_T_r = c_T_r * [I, [plane_search->shift_u/px; plane_search->shift_v/py; 0]; 0 0 0 1]
//! \param  [out]  pose              The pose to update
//! \param  [in]   calib             The calibration matrix
//! \param  [in]   plane_search      The plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_update_pose_translation (
   Rox_Array2D_Double pose, 
   const Rox_Array2D_Double calib, 
   const Rox_Plane_Search plane_search
   );

//! Update rotation given last make results
//! \param  [out]  rotation       The rotation to update 
//! \param  [in ]  calib          The camera calibration matrix
//! \param  [in ]  plane_search   The plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_update_pose_rotation (
   Rox_Array2D_Double rotation, 
   const Rox_Array2D_Double calib, 
   const Rox_Plane_Search plane_search
);

//! Update homography given last make results
//! \param  [out]  homography     The homography to update
//! \param  [in ]  plane_search   The plane searching object
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_plane_search_update_homography (
   Rox_MatSL3 homography, 
   const Rox_Plane_Search plane_search
);

//! @} 

#endif
