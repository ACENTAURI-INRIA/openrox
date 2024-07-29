### NON REGRESSION TESTS ####

MESSAGE(STATUS "\n_____ rox_open/cmake/tests.cmake ____________________________________________\n" )

if (OPENROX_HAS_STD_TESTS_SUPPORT)
   option ( OPENROX_BUILD_TESTS "Build tests" OFF )
else()
   message(STATUS "CURRENT PLATFORM DOES NOT HAVE STD TESTS SUPPORT")
endif ()

unset(OPENROX_TESTS_OK)

if ( OPENROX_BUILD_TESTS )

   # Define folder with data for tests
   # add_definitions( -DROX_DATA_HOME="$ENV{OPENROX_DATA_HOME}" )

   # Add lib for non-windows platform
   if(NOT WIN32)
     set (TESTS_EXTERNAL_LIBS ${TESTS_EXTERNAL_LIBS} m)
   endif()

   set(OPENROX_TESTS_OK 1)

   # Enable CTest
   enable_testing()

   # Enable submission to Dashboard
   include(CTest)

   set ( TESTS_EXTERNAL_LIBS openrox ${OPENROX_PLUGIN_NAME} ${OPENROX_EXTERNAL_LIBS} )
   set ( TESTS_SOURCES_DIR ${OPENROX_SOURCE_DIR}/tests )
   set ( TESTS_EXTERNAL_SOURCES ${OPENROX_SOURCE_DIR}/tests/openrox_tests.cpp )
   include_directories(${OPENROX_SOURCE_DIR}/tests)

   # if(ANDROID)
   #   set(TESTS_EXTERNAL_LIBS ${TESTS_EXTERNAL_LIBS} ${ROX_MODEL_LIBRARIES} ${ROX_MODEL_EXTERNAL_LIBS} ${ROX_VIZ_LIBRARIES})
   # endif()

   # Unitary tests with rox_kernel

   #################################################################################################
   ## Test system layer
   #################################################################################################

   unit_test_macro ( system/memory                          test_array )
   unit_test_macro ( system/memory                          test_array2d )
   unit_test_macro ( system/memory                          test_memory )
   unit_test_macro ( system/version                         test_version )

if (OPENROX_USE_AVX)
   unit_test_macro ( system/vectorisation                   test_avx )
endif ()

if (OPENROX_USE_SSE OR OPENROX_USE_AVX)
   unit_test_macro ( system/vectorisation                   test_sse )
endif ()

   #################################################################################################
   ## Test baseproc layer
   #################################################################################################

   unit_test_macro ( baseproc/array/add                      test_add                                                      )
   unit_test_macro ( baseproc/array/band                     test_band                                                     )
   unit_test_macro ( baseproc/array/bnot                     test_bnot                                                     )
   unit_test_macro ( baseproc/array/conversion               test_conversion_array2d_float_uchar                           )
   unit_test_macro ( baseproc/array/conversion               test_conversion_array2d_uint_uchar                            )
   unit_test_macro ( baseproc/array/crosscor                 test_zncrosscor                                               )
   unit_test_macro ( baseproc/array/crosscor                 test_zncrosscor9x9                                            )
    
   unit_test_macro ( baseproc/array/crossprod                test_crossprod                                                )
   unit_test_macro ( baseproc/array/determinant              test_determinant                                              )
   unit_test_macro ( baseproc/array/decomposition            test_decomposition_cholesky                                   )
   unit_test_macro ( baseproc/array/decomposition            test_decomposition_svd                                        )

   unit_test_macro ( baseproc/array/eigenv                   test_real_eigenvalues_eigenvectors                            )

   unit_test_macro ( baseproc/array/error                    test_centered_error                                           )
   unit_test_macro ( baseproc/array/error                    test_l2_error                                                 )
   unit_test_macro ( baseproc/array/error                    test_ssd                                                      )
   unit_test_macro ( baseproc/array/exponent                 test_expmat                                                   )
   unit_test_macro ( baseproc/array/fill                     test_fill                                                     )
   unit_test_macro ( baseproc/array/flip                     test_flip                                                     )
   unit_test_macro ( baseproc/array/integral                 test_integralsum                                              )
   unit_test_macro ( baseproc/array/integral                 test_integralaccess                                           )
   unit_test_macro ( baseproc/array/inverse                  test_mat3x3inv                                                )
   unit_test_macro ( baseproc/array/inverse                  test_pseudoinverse                                            )
   unit_test_macro ( baseproc/array/inverse                  test_svdinverse                                               )
   unit_test_macro ( baseproc/array/inverse                  test_inverse_lu                                               )
   unit_test_macro ( baseproc/array/inverse                  test_lotinverse                                               )
   unit_test_macro ( baseproc/array/mad                      test_mad                                                      )
   unit_test_macro ( baseproc/array/maxima                   test_maxima                                                   )
   unit_test_macro ( baseproc/array/mean                     test_mean                                                     )
   unit_test_macro ( baseproc/array/meanvar                  test_meanvar                                                  )
   unit_test_macro ( baseproc/array/median                   test_median                                                   )
   unit_test_macro ( baseproc/array/minmax                   test_minmax                                                   )
   unit_test_macro ( baseproc/array/morphological            test_dilate_grayone                                           )
   unit_test_macro ( baseproc/array/multiply                 test_mulmatmat                                                )
   unit_test_macro ( baseproc/array/multiply                 test_mulmatmattrans                                           )
   unit_test_macro ( baseproc/array/multiply                 test_mulmattransmat                                           )
   unit_test_macro ( baseproc/array/nonmax                   test_nonmax                                                   )
   unit_test_macro ( baseproc/array/norm                     test_norm2sq                                                  )
   unit_test_macro ( baseproc/array/robust                   test_huber                                                    )
   unit_test_macro ( baseproc/array/robust                   test_tukey                                                    )
   unit_test_macro ( baseproc/array/rotate                   test_rotate90                                                 )
   unit_test_macro ( baseproc/array/scale                    test_scaleshift                                               )
   unit_test_macro ( baseproc/array/scale                    test_scale                                                    )
   unit_test_macro ( baseproc/array/solve                    test_svd_solve                                                )
   unit_test_macro ( baseproc/array/solve                    test_linsys_solve_cholesky                                    )
   unit_test_macro ( baseproc/array/solve                    test_symm3x3_solve                                            )
   unit_test_macro ( baseproc/array/substract                test_substract                                                )
   unit_test_macro ( baseproc/array/transpose                test_transpose                                                )
   unit_test_macro ( baseproc/array/symmetrise               test_symmetrise                                               )
 
   unit_test_macro ( baseproc/calculus/jacobians             test_interaction_row_point_to_line_matse3                     )

   unit_test_macro ( baseproc/calculus/jacobians             test_interaction_point2d_pix_matut3                           )
   unit_test_macro ( baseproc/calculus/jacobians             test_interaction_point2d_nor_matse3                           )
   unit_test_macro ( baseproc/calculus/jacobians             test_interaction_point2d_nor_matse3_matso3z_r2                )
   unit_test_macro ( baseproc/calculus/jacobians             test_interaction_point2d_pix_matse3                           )
   unit_test_macro ( baseproc/calculus/jacobians             test_jacobian_perspective_stereo_calibration                  )
   unit_test_macro ( baseproc/calculus/jacobians             test_jacobian_row_bundle_matse3_point3d                       )

   unit_test_macro ( baseproc/calculus/linsys                test_linsys_point_to_line_matse3                              )

   unit_test_macro ( baseproc/calculus/linsys                test_linsys_essential_geometric_weighted_premul               )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_generalized_geometric_weighted_premul             )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_se3_light_affine_premul                           )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_se3_z1_light_affine_premul                        )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_se3_z1_light_affine_premul_left                   )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_stereo_point2d_pix_matse3_weighted                )

   unit_test_macro ( baseproc/calculus/linsys                test_linsys_weighted_texture_matse3_light_affine_model3d_zi   )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_weighted_texture_matse3_light_affine_model3d      )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matse3_model3d_zi                         )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matse3_light_affine_model3d_zi            )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matsl3_light_affine                       )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matse3_light_affine_model2d               )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matse3_light_affine_depth_model3d         )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matse2_light_affine_model2d               )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_tutvsr_light_affine_model2d               )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_tutvsusv_light_affine_model2d             )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_matso3_light_affine                       )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_rx_light_affine                           )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_texture_rxry_light_affine                         )

   unit_test_macro ( baseproc/calculus/linsys                test_linsys_point2d_pix_matse3_weighted                       )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_point2d_pix_matsl3                                )
   unit_test_macro ( baseproc/calculus/linsys                test_linsys_point2d_nor_matse3_weighted                       )


   unit_test_macro ( baseproc/geometry/calibration           test_optimalcalib                                             )
   unit_test_macro ( baseproc/geometry/connectivity          test_connectivity                                             )
   unit_test_macro ( baseproc/geometry/disparity             test_depthfromdisparity                                       )
   
   unit_test_macro ( baseproc/geometry/segment               test_segment2d                                                )
   unit_test_macro ( baseproc/geometry/segment               test_segment3d                                                )

   unit_test_macro ( baseproc/geometry/line                  test_line_clip                                                )
   unit_test_macro ( baseproc/geometry/line                  test_line_closestpoint                                        )
   unit_test_macro ( baseproc/geometry/line                  test_line_from_planes                                         )
   unit_test_macro ( baseproc/geometry/line                  test_line_from_points                                         )
   unit_test_macro ( baseproc/geometry/line                  test_line_project                                             )
   unit_test_macro ( baseproc/geometry/line                  test_line_transform                                           )
 
   unit_test_macro ( baseproc/geometry/pixelgrid             test_meshgrid2d                                               )
   unit_test_macro ( baseproc/geometry/pixelgrid             test_warp_grid_matsl3                                         )
   unit_test_macro ( baseproc/geometry/pixelgrid             test_warp_grid_matse3                                         )
   unit_test_macro ( baseproc/geometry/pixelgrid             test_warp_grid_distortion                                     )
   unit_test_macro ( baseproc/geometry/pixelgrid             test_warp_grid_distortion_matsl3                              )
 
   unit_test_macro ( baseproc/geometry/plane                 test_plane_transform                                          )
   unit_test_macro ( baseproc/geometry/plane                 test_plane_3points                             )
   unit_test_macro ( baseproc/geometry/points                test_point3d_from_template                     )
   unit_test_macro ( baseproc/geometry/points                test_point3d_inverse_projection_from_point2d   )
   unit_test_macro ( baseproc/geometry/points                test_point2d_matsl3_transform                  )
   unit_test_macro ( baseproc/geometry/points                test_point3d_sphere                            )
   unit_test_macro ( baseproc/geometry/points                test_point3d_tools                             )
   unit_test_macro ( baseproc/geometry/points                test_point2d_projection_from_point3d           )
   unit_test_macro ( baseproc/geometry/points                test_point3d_matse3_transform                  )
   unit_test_macro ( baseproc/geometry/points                test_point2d_projection_from_point3d_transform )
   unit_test_macro ( baseproc/geometry/ellipse               test_ellipse2d                                 )
   unit_test_macro ( baseproc/geometry/ellipse               test_ellipse3d                                 )
   unit_test_macro ( baseproc/geometry/ellipse               test_ellipse_from_points                       )
   unit_test_macro ( baseproc/geometry/cylinder              test_cylinder2d                                )
   unit_test_macro ( baseproc/geometry/cylinder              test_cylinder3d                                )
   unit_test_macro ( baseproc/geometry/transforms            test_transform_tools                           )
   unit_test_macro ( baseproc/geometry/transforms/distortion test_pointundistort                            )
   unit_test_macro ( baseproc/geometry/transforms/matsl3     test_sl3from4points                            )
   unit_test_macro ( baseproc/geometry/transforms/matsl3     test_sl3fromNpoints                            )
   unit_test_macro ( baseproc/geometry/transforms/matsl3     test_sl3interfrom3dpoints                      )
   unit_test_macro ( baseproc/geometry/transforms/matsl3     test_sl3normalize                              )
   unit_test_macro ( baseproc/geometry/transforms/matsl3     test_sl3virtualview                            )
   unit_test_macro ( baseproc/geometry/transforms/matse3     test_matse3_from_n_points3d_to_points2d        )
   unit_test_macro ( baseproc/geometry/transforms/matse3     test_matse3_from_n_points3d_to_planes3d        )
   unit_test_macro ( baseproc/geometry/transforms/matse3     test_matse3_from_points3d_double_sets          )
   unit_test_macro ( baseproc/geometry/specifics/            test_init_vvs_se3_so3z_so3z                    )
   unit_test_macro ( baseproc/image/convert                  test_roxrgba_to_roxgray                        )
   unit_test_macro ( baseproc/image/convert                  test_roxgray_to_gray                           )
   unit_test_macro ( baseproc/image/convert                  test_roxgray_uchar_to_roxgray_float            )
   unit_test_macro ( baseproc/image/convolve                 test_basic_convolve                            )
   unit_test_macro ( baseproc/image/convolve                 test_sparse_convolve                           )
   unit_test_macro ( baseproc/image/convolve                 test_symm_convolve                             )
   unit_test_macro ( baseproc/image/filter/median            test_medianfilter                              )
   unit_test_macro ( baseproc/image/gradient                 test_basegradient                              )
   unit_test_macro ( baseproc/image/gradient                 test_gradient_anglenorm                        )
   unit_test_macro ( baseproc/image/gradient                 test_gradient_sobel                            )
   unit_test_macro ( baseproc/image/noise                    test_gaussian_noise                            )
   unit_test_macro ( baseproc/image/pyramid                  test_pyramid_float                             )
   unit_test_macro ( baseproc/image/pyramid                  test_pyramid_uchar                             )
   unit_test_macro ( baseproc/image/pyramid                  test_pyramid_uint                              )
   unit_test_macro ( baseproc/image/pyramid                  test_pyramid_npot_uchar                        )
   unit_test_macro ( baseproc/image/pyramid                  test_pyramid_tools                             )
   unit_test_macro ( baseproc/image/remap                    test_remap_bilinear                            )
   unit_test_macro ( baseproc/image/remap                    test_remap_bilinear_nomask                     )
   unit_test_macro ( baseproc/image/remap                    test_remap_bilinear_omo                        )
   unit_test_macro ( baseproc/image/remap                    test_remap_bilinear_onepixel                   )
   unit_test_macro ( baseproc/image/remap                    test_remap_bilinear_trans                      )
   unit_test_macro ( baseproc/image/remap                    test_remap_bilinear_nomask_float_to_float_doubled )
   unit_test_macro ( baseproc/image/remap                    test_remap_ewa_omo                             )
   unit_test_macro ( baseproc/image/remap                    test_remap_box_halved                          )
   unit_test_macro ( baseproc/image/remap                    test_remap_box_mask_halved                     )
   unit_test_macro ( baseproc/image/remap                    test_remap_nn_halved                           )
   unit_test_macro ( baseproc/image/transform                test_distance                                  )
   unit_test_macro ( baseproc/image                          test_image_square_centered                     )
   unit_test_macro ( baseproc/image                          test_image_flip                                )
   unit_test_macro ( baseproc/image                          test_image                                     )
   unit_test_macro ( baseproc/image/warp                     test_image_warp_matsl3                         )
   unit_test_macro ( baseproc/image/draw                     test_draw_circle                               )
   unit_test_macro ( baseproc/image/draw                     test_draw_ellipse                              )
   unit_test_macro ( baseproc/image/draw                     test_color                                     )
   unit_test_macro ( baseproc/image/imask                    test_imask                                     )

   unit_test_macro ( baseproc/maths/base                     test_basemaths                                 )
   unit_test_macro ( baseproc/maths/filter                   test_filter_matse3                             )
   unit_test_macro ( baseproc/maths/kernels                  test_gaussian2d                                )
   unit_test_macro ( baseproc/maths/linalg                   test_matrix                                    )
   unit_test_macro ( baseproc/maths/linalg                   test_matse3                                    )
   unit_test_macro ( baseproc/maths/linalg                   test_matso3                                    )
   unit_test_macro ( baseproc/maths/linalg                   test_matsl3                                    )
   unit_test_macro ( baseproc/maths/linalg                   test_matut3                                    )
   unit_test_macro ( baseproc/maths/linalg                   test_expmat_se3                                )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algse2                                    )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algse3                                    )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algsl3                                    )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algso3                                    )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algtutvsr                                 )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algtutvsusv                               )
   unit_test_macro ( baseproc/maths/linalg/generators        test_algut3                                    )
 
   unit_test_macro ( baseproc/maths/nonlin                   test_polynomials                               )
   unit_test_macro ( baseproc/maths/random                   test_combination                               )
   unit_test_macro ( baseproc/maths/random                   test_random                                    )
 
   unit_test_macro ( baseproc/tools/encoder                  test_bch                                       )
   unit_test_macro ( baseproc/tools/string                   test_filepath                                  )

   #################################################################################################
   ## Test core layer
   #################################################################################################

   unit_test_macro ( core/bundle                            test_bundle                                     )
   unit_test_macro ( core/bundle                            test_bundle_camera                              )
   unit_test_macro ( core/bundle                            test_bundle_frame                               )
   unit_test_macro ( core/bundle                            test_bundle_measure                             )
   unit_test_macro ( core/bundle                            test_bundle_point                               )
   unit_test_macro ( core/calibration/inertial              test_calibration_vision_inertial                )
   unit_test_macro ( core/calibration/mono                  test_calibration_generalized                    )   
   unit_test_macro ( core/calibration/mono                  test_calibration_perspective                    )
   unit_test_macro ( core/calibration/stereo                test_stereo_calibration                         )
   unit_test_macro ( core/calibration/stereo                test_stereo_calibration_se3                     )
   unit_test_macro ( core/calibration/stereo                test_stereo_rectification                       )
   unit_test_macro ( core/features/descriptors/brief        test_brief                                      )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid                                       )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_compiler                              )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_computedescriptor                     )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_database                              )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_dbindex                               )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_matcher                               )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_searchtree                            )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_target                                )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_viewpointbin                          )
   unit_test_macro ( core/features/descriptors/ehid         test_ehid_window                                )
   unit_test_macro ( core/features/descriptors/fpsm         test_fpsm                                       )
   unit_test_macro ( core/features/descriptors/fpsm         test_fpsm_index                                 )
   unit_test_macro ( core/features/descriptors/sraid        test_heap_branch                                )
   unit_test_macro ( core/features/descriptors/sraid        test_kdtree_sraid                               )
   unit_test_macro ( core/features/descriptors/sraid        test_sraid                                      )
   unit_test_macro ( core/features/descriptors/sraid        test_sraiddesc                                  )
   unit_test_macro ( core/features/descriptors/sraid        test_sraiddesc_kmean                            )
   unit_test_macro ( core/features/descriptors/sraid        test_sraid_match                                )
   unit_test_macro ( core/features/descriptors/sraid        test_sraid_matchresultset                       )
   unit_test_macro ( core/features/descriptors/sraid        test_sraid_matchset                             )
   unit_test_macro ( core/features/descriptors/tlid         test_tlid                                       )
   unit_test_macro ( core/features/descriptors/tlid         test_tlid_matcher                               )
   unit_test_macro ( core/features/detectors/checkerboard   test_checkerboard                               )
   unit_test_macro ( core/features/detectors/checkerboard   test_checkerboard_detect                        )
   unit_test_macro ( core/features/detectors/checkerboard   test_checkercorner_detect                       )
   unit_test_macro ( core/features/detectors/corners        test_gftt                                       )
   unit_test_macro ( core/features/detectors/corners        test_harris                                     )
   unit_test_macro ( core/features/detectors/corners        test_shicorner9x9                               )
   unit_test_macro ( core/features/detectors/corners        test_shicorner                                  )
   unit_test_macro ( core/features/detectors/dog            test_dog                                        )
   unit_test_macro ( core/features/detectors/edges          test_canny                                      )
   unit_test_macro ( core/features/detectors/edges          test_edgedraw                                   )
   unit_test_macro ( core/features/detectors/edges          test_edgepostproc_ac                            )
   unit_test_macro ( core/features/detectors/edges          test_edgepostproc_normal                        )
   unit_test_macro ( core/features/detectors/edges          test_edgepostproc_segment                       )
   unit_test_macro ( core/features/detectors/edges          test_edgepreproc_gray                           )
   unit_test_macro ( core/features/detectors/edges          test_edgepreproc_rgb                            )
   unit_test_macro ( core/features/detectors/orientation    test_orimoments )
   unit_test_macro ( core/features/detectors/quad           test_quad_detection )
   unit_test_macro ( core/features/detectors/quad           test_quad_gradientclusterer )
   unit_test_macro ( core/features/detectors/quad           test_quad_segment2d )
   unit_test_macro ( core/features/detectors/segment        test_fastst )
   unit_test_macro ( core/features/detectors/segment        test_fastst_score )
   unit_test_macro ( core/features/detectors/segment        test_segmentpoint_tools )
   unit_test_macro ( core/features/detectors/shape          test_sdwm )
   unit_test_macro ( core/features/detectors/shape          test_sdwm_object )
   unit_test_macro ( core/identification                    test_codedframe )
   unit_test_macro ( core/identification                    test_dbident_se3 )
   unit_test_macro ( core/identification                    test_dbident_sl3 )
   unit_test_macro ( core/identification                    test_multiident )
   unit_test_macro ( core/identification                    test_photoframe )
   unit_test_macro ( core/identification                    test_templateident )
   unit_test_macro ( core/identification                    test_templateident_se3 )
   unit_test_macro ( core/identification                    test_templateident_sl3 )
   unit_test_macro ( core/indirect/essential                test_e5points_nister )
   unit_test_macro ( core/indirect/essential                test_essentialerror )
   unit_test_macro ( core/indirect/essential                test_essentialminimize )
   unit_test_macro ( core/indirect/essential                test_essentialposes )
   unit_test_macro ( core/indirect/essential                test_ransacessentialcommon )
   unit_test_macro ( core/indirect/essential                test_ransacessentialpose )
   unit_test_macro ( core/indirect/euclidean                test_p3points )
   unit_test_macro ( core/indirect/euclidean                test_pointcloud_similarity )
   unit_test_macro ( core/indirect/euclidean                test_ransacse3 )
   unit_test_macro ( core/indirect/euclidean                test_triangulate )
   unit_test_macro ( core/indirect/euclidean                test_matso3_from_vectors )
   unit_test_macro ( core/indirect/euclidean                test_vvspointsse3 )
   unit_test_macro ( core/indirect/euclidean                test_vvspointsse3_stereo )
   unit_test_macro ( core/indirect/euclidean                test_vvs_points_se3_so3z_r2 )
   unit_test_macro ( core/indirect/homography               test_ransacsl3 )
   unit_test_macro ( core/indirect/homography               test_vvspointssl3 )
   unit_test_macro ( core/indirect/multinonoverlap          test_nonoverlaperror )
   unit_test_macro ( core/indirect/multinonoverlap          test_nonoverlapminimize )
   unit_test_macro ( core/indirect/multinonoverlap          test_p7p )
   unit_test_macro ( core/indirect/multinonoverlap          test_ransac_nonoverlap )
   unit_test_macro ( core/inertial/frame                    test_frame )
   unit_test_macro ( core/inertial/measure                  test_inertial_measure_buffer )
   unit_test_macro ( core/inertial/measure                  test_inertial_measure )
   unit_test_macro ( core/inertial/observer                 test_inertial_observer )
   unit_test_macro ( core/inertial/observer                 test_complementary_filter )
   unit_test_macro ( core/inertial/sensor                   test_imu ) 

   unit_test_macro ( core/occupancy                         test_quadtree_ref )
   unit_test_macro ( core/occupancy                         test_quadtree_static )
   unit_test_macro ( core/odometry/affine                   test_odometry_se2 )
   unit_test_macro ( core/odometry/depth                    test_odometry_dense_depthmap )
   unit_test_macro ( core/odometry/essential                test_odometry_essential )
   unit_test_macro ( core/odometry/multiplane               test_odometry_planes )
   unit_test_macro ( core/odometry/multiplane               test_odometry_multi_plane_photoframe )
   unit_test_macro ( core/odometry/plane                    test_odometry_plane )
   unit_test_macro ( core/odometry/plane                    test_odometry_plane_robustlight )
   unit_test_macro ( core/odometry/plane                    test_odometry_plane_photoframe )
   unit_test_macro ( core/odometry/rotation                 test_odometry_so3 )
   unit_test_macro ( core/odometry/edge                     test_odometry_ellipses )
   unit_test_macro ( core/odometry/edge                     test_objset_edge_ellipse )
   unit_test_macro ( core/patch                             test_patchplane )
   unit_test_macro ( core/patch                             test_patchplane_pyramid )
   unit_test_macro ( core/patch                             test_patchplane_robustlight )
   unit_test_macro ( core/patch                             test_patchplane_robustlight_pyramid )
   unit_test_macro ( core/predict                           test_plane_search )
   unit_test_macro ( core/predict                           test_plane_search_uchar )
   unit_test_macro ( core/templatesearch                    test_ocm )
   unit_test_macro ( core/templatesearch                    test_region_zncc_search_mask_template_mask )
   unit_test_macro ( core/tracking/patch                    test_tracking_patch_sl3 )
   unit_test_macro ( core/tracking/patch                    test_tracking_patch_tu_tv_s_r )
   unit_test_macro ( core/tracking/patch                    test_tracking_patch_tu_tv_su_sv )
   unit_test_macro ( core/tracking/point                    test_tracking_point_11x11 )
   unit_test_macro ( core/tracking/point                    test_tracking_point_9x9 )
   unit_test_macro ( core/tracking/point                    test_tracking_point )
   unit_test_macro ( core/tracking/edge                     test_search_edge )
   unit_test_macro ( core/tracking/edge                     test_scan_scale_angle_matrix )
   unit_test_macro ( core/tracking/edge                     test_find_closest_scale_above_threshold_angle_isinrange )
   unit_test_macro ( core/virtualview                       test_planar_view_generator )
   unit_test_macro ( core/virtualview                       test_unwrap_model )
   unit_test_macro ( core/model                             test_model_single_plane )
   unit_test_macro ( core/model                             test_model_multi_plane )
   unit_test_macro ( core/model                             test_model_checkerboard )
   unit_test_macro ( core/model                             test_model_projector_checkerboard )

   #################################################################################################
   ## Test inout layer
   #################################################################################################

   unit_test_macro ( inout/image/ppm                        test_ppm )
   unit_test_macro ( inout/image/pgm                        test_pgm )

   unit_test_macro ( inout/mask/fill                        test_set_polygon )

   unit_test_macro ( inout/system                           test_file )

   #################################################################################################
   ## Test user layer
   #################################################################################################

   unit_test_macro ( user/calibration/camproj               test_calibration_camproj_checkerboard     )

   unit_test_macro ( user/identification/database           test_database                             )
   unit_test_macro ( user/identification/database           test_database_item                        )
   unit_test_macro ( user/identification/database           test_database_features                    )
   unit_test_macro ( user/identification/database           test_ident_database_sl3                   )
   unit_test_macro ( user/identification/database           test_ident_database_se3                   )
   unit_test_macro ( user/identification/database           test_image_quality_score                  )

   unit_test_macro ( user/identification/texture            test_ident_texture_sl3                    )
   unit_test_macro ( user/identification/texture            test_multi_ident_texture_sl3              )

   unit_test_macro ( user/identification/photoframe         test_ident_photoframe_se3                 )
   unit_test_macro ( user/identification/photoframe         test_ident_multi_photoframe_se3           )

   unit_test_macro ( user/tracking                          test_tracking_database                    )

   unit_test_macro ( user/sensor/camera                     test_camera                               )

   unit_test_macro ( user/odometry/plane                    test_odometry_single_plane                )
   unit_test_macro ( user/odometry/plane                    test_odometry_single_plane_sparse         )
   unit_test_macro ( user/odometry/plane                    test_odometry_single_plane_database       )
   unit_test_macro ( user/odometry/plane                    test_odometry_single_plane_photoframe     )
   # unit_test_macro ( user/odometry/multiplane               test_odometry_multi_plane                 )

   #################################################################################################
   ## Test extern layer
   #################################################################################################

endif ()
