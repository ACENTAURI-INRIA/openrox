//==============================================================================
//
//    OPENROX   : File tracking_point_9x9.h
//
//    Contents  : API of tracking_point_9x9 module
//
//    Author(s) : R&D department directed by Ezio MALIS
//
//    Copyright : 2022 Robocortex S.A.S.
//
//    License   : LGPL v3 or commercial license
//
//==============================================================================

#ifndef __OPENROX_TRACKING_POINT_9X9__
#define __OPENROX_TRACKING_POINT_9X9__

#include <generated/array2d_float.h>
#include <baseproc/image/image.h>
#include <baseproc/maths/linalg/matut3.h>
#include <baseproc/maths/linalg/matse3.h>
#include <baseproc/image/imask/imask.h>

//! \ingroup Tracking
//! \addtogroup Point
//!   @{

//! object 
typedef struct Rox_Tracking_Point_9x9_Struct * Rox_Tracking_Point_9x9;

//! Create tracking point object
//! \param  []  obj the pointer to the tracking object
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_new(Rox_Tracking_Point_9x9 * obj);

//! Delete tracking point object
//! \param  []  obj the pointer to the tracking object
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_del(Rox_Tracking_Point_9x9 * obj);

//! Set reference patch for point
//! \param  []  obj the tracking object
//! \param  []   []  source the reference image to use
//! \param  []  pos_u the coordinates of the point (center of patch)
//! \param  []  pos_v the coordinates of the point (center of patch)
//! \param  []  Z the depth of the point in the reference frame
//! \param  []  pose the pose of the reference frame in the current frame
//! \param  []  calibration_ref the reference camera calibration
//! \param  []  calibration_cur the current camera calibration
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_set_warped_reference(
   Rox_Tracking_Point_9x9 obj, Rox_Image source, Rox_Double pos_u, Rox_Double pos_v, Rox_Double Z, 
   Rox_MatSE3 pose, Rox_MatUT3 calibration_ref, Rox_MatUT3 calibration_cur);

//! Set reference patch for point using a refernce image, a transformation and information about point normal
//! \param  []  obj the tracking object
//! \param  []  source the reference image to use
//! \param  []  pos_u the coordinates of the point (top-left of patch)
//! \param  []  pos_v the coordinates of the point (top-left of patch)
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_set_reference(Rox_Tracking_Point_9x9 obj, Rox_Image source, Rox_Double pos_u, Rox_Double pos_v);

//! Set reference patch for point
//! \param  []  obj the tracking object
//! \param  []  source the reference image to use
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_set_referencepatch(Rox_Tracking_Point_9x9 obj, Rox_Array2D_Float source);

//! Set current patch predicted coordinates
//! \param  []  obj the tracking object
//! \param  []  pos_x the coordinates of the point (top-left of patch)
//! \param  []  pos_y the coordinates of the point (top-left of patch)
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_set_predicted_position(Rox_Tracking_Point_9x9 obj, Rox_Double pos_x, Rox_Double pos_y);

//! Perform tracking of the stored reference point
//! \param  []  obj the tracking object
//! \param  []  current the current image where the reference patch has to be found
//! \param  []  minscore the minimum score zncc after minimization
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_track(Rox_Tracking_Point_9x9 obj, Rox_Image current, Rox_Double minscore);

//! Perform tracking of the stored reference point on x
//! \param  []  obj               the tracking object
//! \param  []  current           the current image where the reference patch has to be found
//! \param  []  minscore          the minimum score zncc after minimization
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_track_x(Rox_Tracking_Point_9x9 obj, Rox_Image current, Rox_Double minscore);

//! Retrieve estimated point coordinates
//! \param  [out]  current_x      pointer to the current estimated x
//! \param  [out]  current_y      pointer to the current estimated y
//! \param  [in ]  obj            the tracking object
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_get_current(Rox_Double * current_x, Rox_Double * current_y, Rox_Tracking_Point_9x9 obj);

//! Get a copy of the reference patch
//! \param  [out]  ref         the reference patch to set
//! \param  [in ]  obj         the tracking point object
//! \return An error code
ROX_API Rox_ErrorCode rox_tracking_point_9x9_get_reference_patch(Rox_Image ref, Rox_Tracking_Point_9x9 obj);

//! @} 

#endif
