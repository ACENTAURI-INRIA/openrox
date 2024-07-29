//==============================================================================
//
//    OPENROX   : File remap_bilinear_uchar_to_uchar.h
//
//    Contents  : API of remap_bilinear_uchar_to_uchar module
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#ifndef __OPENROX_REMAP_BILINEAR_UCHAR_TO_UCHAR__
#define __OPENROX_REMAP_BILINEAR_UCHAR_TO_UCHAR__

#include <generated/array2d_float.h>
#include <generated/array2d_double.h>
#include <baseproc/geometry/pixelgrid/meshgrid2d.h>

#include <baseproc/image/image.h>
#include <baseproc/image/imask/imask.h>

//! \ingroup Image
//! \addtogroup Remap
//! @{

//! Given an input array, remap it using bilinear interpolation : result(i,j) = source(i+v,j+u)
//! \param  [out]  output         The result array
//! \param  [out]  mask_out       The result array validity mask
//! \param  [in ]  mask_out_ini   The input validity mask in destination space
//! \param  [in ]  mask_inp       The input validity mask in source space
//! \param  [in ]  input          The input array
//! \param  [in ]  map            The map containing the (u,v) shift. It's width is twice the width of the input, u,v being interleaved.
//! \return An error code
//! \todo   To be tested
ROX_API Rox_ErrorCode rox_remap_bilinear_uchar_to_uchar (
   Rox_Image output, 
   Rox_Imask mask_out, 
   const Rox_Imask mask_out_ini, 
   const Rox_Image input, 
   const Rox_Imask mask_inp, 
   const Rox_MeshGrid2D_Float grid
);

//! @} 

#endif
