/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#ifndef OpenGLExt_INCLUDE_ONCE
#define OpenGLExt_INCLUDE_ONCE

namespace vl
{
  const void* getProcAddress(const char* func);

  const char* initOpenGLExtensions();

  extern bool Has_GL_VERSION_1_0;
  extern bool Has_GL_VERSION_1_1;
  extern bool Has_GL_VERSION_1_2;
  extern bool Has_GL_VERSION_1_3;
  extern bool Has_GL_VERSION_1_4;
  extern bool Has_GL_VERSION_1_5;
  extern bool Has_GL_VERSION_2_0;
  extern bool Has_GL_VERSION_2_1;
  extern bool Has_GL_VERSION_3_0;
  extern bool Has_GL_VERSION_3_1;
  extern bool Has_GL_VERSION_3_2;
  extern bool Has_GL_VERSION_3_3;
  extern bool Has_GL_VERSION_4_0;
  extern bool Has_GL_VERSION_4_1;

  // ARB Extensions

  extern bool Has_GL_ARB_imaging;
  extern bool Has_GL_ARB_multitexture;
  extern bool Has_GLX_ARB_get_proc_address;
  extern bool Has_GL_ARB_transpose_matrix;
  extern bool Has_WGL_ARB_buffer_region;
  extern bool Has_GL_ARB_multisample;
  extern bool Has_GLX_ARB_multisample;
  extern bool Has_WGL_ARB_multisample;
  extern bool Has_GL_ARB_texture_env_add;
  extern bool Has_GL_ARB_texture_cube_map;
  extern bool Has_WGL_ARB_extensions_string;
  extern bool Has_WGL_ARB_pixel_format;
  extern bool Has_WGL_ARB_make_current_read;
  extern bool Has_WGL_ARB_pbuffer;
  extern bool Has_GL_ARB_texture_compression;
  extern bool Has_GL_ARB_texture_border_clamp;
  extern bool Has_GL_ARB_point_parameters;
  extern bool Has_GL_ARB_vertex_blend;
  extern bool Has_GL_ARB_matrix_palette;
  extern bool Has_GL_ARB_texture_env_combine;
  extern bool Has_GL_ARB_texture_env_crossbar;
  extern bool Has_GL_ARB_texture_env_dot3;
  extern bool Has_WGL_ARB_render_texture;
  extern bool Has_GL_ARB_texture_mirrored_repeat;
  extern bool Has_GL_ARB_depth_texture;
  extern bool Has_GL_ARB_shadow;
  extern bool Has_GL_ARB_shadow_ambient;
  extern bool Has_GL_ARB_window_pos;
  extern bool Has_GL_ARB_vertex_program;
  extern bool Has_GL_ARB_fragment_program;
  extern bool Has_GL_ARB_vertex_buffer_object;
  extern bool Has_GL_ARB_occlusion_query;
  extern bool Has_GL_ARB_shader_objects;
  extern bool Has_GL_ARB_vertex_shader;
  extern bool Has_GL_ARB_fragment_shader;
  extern bool Has_GL_ARB_shading_language_100;
  extern bool Has_GL_ARB_texture_non_power_of_two;
  extern bool Has_GL_ARB_point_sprite;
  extern bool Has_GL_ARB_fragment_program_shadow;
  extern bool Has_GL_ARB_draw_buffers;
  extern bool Has_GL_ARB_texture_rectangle;
  extern bool Has_GL_ARB_color_buffer_float;
  extern bool Has_WGL_ARB_pixel_format_float;
  extern bool Has_GLX_ARB_fbconfig_float;
  extern bool Has_GL_ARB_half_float_pixel;
  extern bool Has_GL_ARB_texture_float;
  extern bool Has_GL_ARB_pixel_buffer_object;
  extern bool Has_GL_ARB_depth_buffer_float;
  extern bool Has_GL_ARB_draw_instanced;
  extern bool Has_GL_ARB_framebuffer_object;
  extern bool Has_GL_ARB_framebuffer_sRGB;
  extern bool Has_GLX_ARB_framebuffer_sRGB;
  extern bool Has_WGL_ARB_framebuffer_sRGB;
  extern bool Has_GL_ARB_geometry_shader4;
  extern bool Has_GL_ARB_half_float_vertex;
  extern bool Has_GL_ARB_instanced_arrays;
  extern bool Has_GL_ARB_map_buffer_range;
  extern bool Has_GL_ARB_texture_buffer_object;
  extern bool Has_GL_ARB_texture_compression_rgtc;
  extern bool Has_GL_ARB_texture_rg;
  extern bool Has_GL_ARB_vertex_array_object;
  extern bool Has_WGL_ARB_create_context;
  extern bool Has_GLX_ARB_create_context;
  extern bool Has_GL_ARB_uniform_buffer_object;
  extern bool Has_GL_ARB_compatibility;
  extern bool Has_GL_ARB_copy_buffer;
  extern bool Has_GL_ARB_shader_texture_lod;
  extern bool Has_GL_ARB_depth_clamp;
  extern bool Has_GL_ARB_draw_elements_base_vertex;
  extern bool Has_GL_ARB_fragment_coord_conventions;
  extern bool Has_GL_ARB_provoking_vertex;
  extern bool Has_GL_ARB_seamless_cube_map;
  extern bool Has_GL_ARB_sync;
  extern bool Has_GL_ARB_texture_multisample;
  extern bool Has_GL_ARB_vertex_array_bgra;
  extern bool Has_GL_ARB_draw_buffers_blend;
  extern bool Has_GL_ARB_sample_shading;
  extern bool Has_GL_ARB_texture_cube_map_array;
  extern bool Has_GL_ARB_texture_gather;
  extern bool Has_GL_ARB_texture_query_lod;
  extern bool Has_WGL_ARB_create_context_profile;
  extern bool Has_GLX_ARB_create_context_profile;
  extern bool Has_GL_ARB_shading_language_include;
  extern bool Has_GL_ARB_texture_compression_bptc;
  extern bool Has_GL_ARB_blend_func_extended;
  extern bool Has_GL_ARB_explicit_attrib_location;
  extern bool Has_GL_ARB_occlusion_query2;
  extern bool Has_GL_ARB_sampler_objects;
  extern bool Has_GL_ARB_shader_bit_encoding;
  extern bool Has_GL_ARB_texture_rgb10_a2ui;
  extern bool Has_GL_ARB_texture_swizzle;
  extern bool Has_GL_ARB_timer_query;
  extern bool Has_GL_ARB_vertex_type_2_10_10_10_rev;
  extern bool Has_GL_ARB_draw_indirect;
  extern bool Has_GL_ARB_gpu_shader5;
  extern bool Has_GL_ARB_gpu_shader_fp64;
  extern bool Has_GL_ARB_shader_subroutine;
  extern bool Has_GL_ARB_tessellation_shader;
  extern bool Has_GL_ARB_texture_buffer_object_rgb32;
  extern bool Has_GL_ARB_transform_feedback2;
  extern bool Has_GL_ARB_transform_feedback3;
  extern bool Has_GL_ARB_ES2_compatibility;
  extern bool Has_GL_ARB_get_program_binary;
  extern bool Has_GL_ARB_separate_shader_objects;
  extern bool Has_GL_ARB_shader_precision;
  extern bool Has_GL_ARB_vertex_attrib_64bit;
  extern bool Has_GL_ARB_viewport_array;
  extern bool Has_GLX_ARB_create_context_robustness;
  extern bool Has_WGL_ARB_create_context_robustness;
  extern bool Has_GL_ARB_cl_event;
  extern bool Has_GL_ARB_debug_output;
  extern bool Has_GL_ARB_robustness;
  extern bool Has_GL_ARB_shader_stencil_export;

  // Vendor and EXT Extensions

  extern bool Has_GL_EXT_abgr;
  extern bool Has_GL_EXT_blend_color;
  extern bool Has_GL_EXT_polygon_offset;
  extern bool Has_GL_EXT_texture;
  extern bool Has_GL_EXT_texture3D;
  extern bool Has_GL_SGIS_texture_filter4;
  extern bool Has_GL_EXT_subtexture;
  extern bool Has_GL_EXT_copy_texture;
  extern bool Has_GL_EXT_histogram;
  extern bool Has_GL_EXT_convolution;
  extern bool Has_GL_SGI_color_matrix;
  extern bool Has_GL_SGI_color_table;
  extern bool Has_GL_SGIS_pixel_texture;
  extern bool Has_GL_SGIX_pixel_texture;
  extern bool Has_GL_SGIS_texture4D;
  extern bool Has_GL_SGI_texture_color_table;
  extern bool Has_GL_EXT_cmyka;
  extern bool Has_GL_EXT_texture_object;
  extern bool Has_GL_SGIS_detail_texture;
  extern bool Has_GL_SGIS_sharpen_texture;
  extern bool Has_GL_EXT_packed_pixels;
  extern bool Has_GL_SGIS_texture_lod;
  extern bool Has_GL_SGIS_multisample;
  extern bool Has_GLX_SGIS_multisample;
  extern bool Has_GL_EXT_rescale_normal;
  extern bool Has_GLX_EXT_visual_info;
  extern bool Has_GL_EXT_vertex_array;
  extern bool Has_GL_EXT_misc_attribute;
  extern bool Has_GL_SGIS_generate_mipmap;
  extern bool Has_GL_SGIX_clipmap;
  extern bool Has_GL_SGIX_shadow;
  extern bool Has_GL_SGIS_texture_edge_clamp;
  extern bool Has_GL_SGIS_texture_border_clamp;
  extern bool Has_GL_EXT_blend_minmax;
  extern bool Has_GL_EXT_blend_subtract;
  extern bool Has_GL_EXT_blend_logic_op;
  extern bool Has_GLX_SGI_swap_control;
  extern bool Has_GLX_SGI_video_sync;
  extern bool Has_GLX_SGI_make_current_read;
  extern bool Has_GLX_SGIX_video_source;
  extern bool Has_GLX_EXT_visual_rating;
  extern bool Has_GL_SGIX_interlace;
  extern bool Has_GLX_EXT_import_context;
  extern bool Has_GLX_SGIX_fbconfig;
  extern bool Has_GLX_SGIX_pbuffer;
  extern bool Has_GL_SGIS_texture_select;
  extern bool Has_GL_SGIX_sprite;
  extern bool Has_GL_SGIX_texture_multi_buffer;
  extern bool Has_GL_EXT_point_parameters;
  extern bool Has_GL_SGIX_instruments;
  extern bool Has_GL_SGIX_texture_scale_bias;
  extern bool Has_GL_SGIX_framezoom;
  extern bool Has_GL_SGIX_tag_sample_buffer;
  extern bool Has_GL_SGIX_reference_plane;
  extern bool Has_GL_SGIX_flush_raster;
  extern bool Has_GLX_SGI_cushion;
  extern bool Has_GL_SGIX_depth_texture;
  extern bool Has_GL_SGIS_fog_function;
  extern bool Has_GL_SGIX_fog_offset;
  extern bool Has_GL_HP_image_transform;
  extern bool Has_GL_HP_convolution_border_modes;
  extern bool Has_GL_SGIX_texture_add_env;
  extern bool Has_GL_EXT_color_subtable;
  extern bool Has_GLU_EXT_object_space_tess;
  extern bool Has_GL_PGI_vertex_hints;
  extern bool Has_GL_PGI_misc_hints;
  extern bool Has_GL_EXT_paletted_texture;
  extern bool Has_GL_EXT_clip_volume_hint;
  extern bool Has_GL_SGIX_list_priority;
  extern bool Has_GL_SGIX_ir_instrument1;
  extern bool Has_GLX_SGIX_video_resize;
  extern bool Has_GL_SGIX_texture_lod_bias;
  extern bool Has_GLU_SGI_filter4_parameters;
  extern bool Has_GLX_SGIX_dm_buffer;
  extern bool Has_GL_SGIX_shadow_ambient;
  extern bool Has_GLX_SGIX_swap_group;
  extern bool Has_GLX_SGIX_swap_barrier;
  extern bool Has_GL_EXT_index_texture;
  extern bool Has_GL_EXT_index_material;
  extern bool Has_GL_EXT_index_func;
  extern bool Has_GL_EXT_index_array_formats;
  extern bool Has_GL_EXT_compiled_vertex_array;
  extern bool Has_GL_EXT_cull_vertex;
  extern bool Has_GLU_EXT_nurbs_tessellator;
  extern bool Has_GL_SGIX_ycrcb;
  extern bool Has_GL_EXT_fragment_lighting;
  extern bool Has_GL_IBM_rasterpos_clip;
  extern bool Has_GL_HP_texture_lighting;
  extern bool Has_GL_EXT_draw_range_elements;
  extern bool Has_GL_WIN_phong_shading;
  extern bool Has_GL_WIN_specular_fog;
  extern bool Has_GLX_SGIS_color_range;
  extern bool Has_GL_SGIS_color_range;
  extern bool Has_GL_EXT_light_texture;
  extern bool Has_GL_SGIX_blend_alpha_minmax;
  extern bool Has_GL_EXT_scene_marker;
  extern bool Has_GLX_EXT_scene_marker;
  extern bool Has_GL_SGIX_pixel_texture_bits;
  extern bool Has_GL_EXT_bgra;
  extern bool Has_GL_SGIX_async;
  extern bool Has_GL_SGIX_async_pixel;
  extern bool Has_GL_SGIX_async_histogram;
  extern bool Has_GL_INTEL_texture_scissor;
  extern bool Has_GL_INTEL_parallel_arrays;
  extern bool Has_GL_HP_occlusion_test;
  extern bool Has_GL_EXT_pixel_transform;
  extern bool Has_GL_EXT_pixel_transform_color_table;
  extern bool Has_GL_EXT_shared_texture_palette;
  extern bool Has_GLX_SGIS_blended_overlay;
  extern bool Has_GL_EXT_separate_specular_color;
  extern bool Has_GL_EXT_secondary_color;
  extern bool Has_GL_EXT_texture_env;
  extern bool Has_GL_EXT_texture_perturb_normal;
  extern bool Has_GL_EXT_multi_draw_arrays;
  extern bool Has_GL_SUN_multi_draw_arrays;
  extern bool Has_GL_EXT_fog_coord;
  extern bool Has_GL_REND_screen_coordinates;
  extern bool Has_GL_EXT_coordinate_frame;
  extern bool Has_GL_EXT_texture_env_combine;
  extern bool Has_GL_APPLE_specular_vector;
  extern bool Has_GL_APPLE_transform_hint;
  extern bool Has_GL_SUNX_constant_data;
  extern bool Has_GL_SUN_global_alpha;
  extern bool Has_GL_SUN_triangle_list;
  extern bool Has_GL_SUN_vertex;
  extern bool Has_WGL_EXT_display_color_table;
  extern bool Has_WGL_EXT_extensions_string;
  extern bool Has_WGL_EXT_make_current_read;
  extern bool Has_WGL_EXT_pixel_format;
  extern bool Has_WGL_EXT_pbuffer;
  extern bool Has_WGL_EXT_swap_control;
  extern bool Has_GL_EXT_blend_func_separate;
  extern bool Has_GL_INGR_color_clamp;
  extern bool Has_GL_INGR_interlace_read;
  extern bool Has_GL_EXT_stencil_wrap;
  extern bool Has_WGL_EXT_depth_float;
  extern bool Has_GL_EXT_422_pixels;
  extern bool Has_GL_NV_texgen_reflection;
  extern bool Has_GL_SGIX_texture_range;
  extern bool Has_GL_SUN_convolution_border_modes;
  extern bool Has_GLX_SUN_get_transparent_index;
  extern bool Has_GL_EXT_texture_env_add;
  extern bool Has_GL_EXT_texture_lod_bias;
  extern bool Has_GL_EXT_texture_filter_anisotropic;
  extern bool Has_GL_EXT_vertex_weighting;
  extern bool Has_GL_NV_light_max_exponent;
  extern bool Has_GL_NV_vertex_array_range;
  extern bool Has_GL_NV_register_combiners;
  extern bool Has_GL_NV_fog_distance;
  extern bool Has_GL_NV_texgen_emboss;
  extern bool Has_GL_NV_blend_square;
  extern bool Has_GL_NV_texture_env_combine4;
  extern bool Has_GL_MESA_resize_buffers;
  extern bool Has_GL_MESA_window_pos;
  extern bool Has_GL_EXT_texture_compression_s3tc;
  extern bool Has_GL_IBM_cull_vertex;
  extern bool Has_GL_IBM_multimode_draw_arrays;
  extern bool Has_GL_IBM_vertex_array_lists;
  extern bool Has_GL_3DFX_texture_compression_FXT1;
  extern bool Has_GL_3DFX_multisample;
  extern bool Has_GL_3DFX_tbuffer;
  extern bool Has_WGL_EXT_multisample;
  extern bool Has_GL_EXT_multisample;
  extern bool Has_GL_SGIX_vertex_preclip;
  extern bool Has_GL_SGIX_vertex_preclip_hint;
  extern bool Has_GL_SGIX_convolution_accuracy;
  extern bool Has_GL_SGIX_resample;
  extern bool Has_GL_SGIS_point_line_texgen;
  extern bool Has_GL_SGIS_texture_color_mask;
  extern bool Has_GLX_MESA_copy_sub_buffer;
  extern bool Has_GLX_MESA_pixmap_colormap;
  extern bool Has_GLX_MESA_release_buffers;
  extern bool Has_GLX_MESA_set_3dfx_mode;
  extern bool Has_GL_EXT_texture_env_dot3;
  extern bool Has_GL_ATI_texture_mirror_once;
  extern bool Has_GL_NV_fence;
  extern bool Has_GL_IBM_static_data;
  extern bool Has_GL_IBM_texture_mirrored_repeat;
  extern bool Has_GL_NV_evaluators;
  extern bool Has_GL_NV_packed_depth_stencil;
  extern bool Has_GL_NV_register_combiners2;
  extern bool Has_GL_NV_texture_compression_vtc;
  extern bool Has_GL_NV_texture_rectangle;
  extern bool Has_GL_NV_texture_shader;
  extern bool Has_GL_NV_texture_shader2;
  extern bool Has_GL_NV_vertex_array_range2;
  extern bool Has_GL_NV_vertex_program;
  extern bool Has_GLX_SGIX_visual_select_group;
  extern bool Has_GL_SGIX_texture_coordinate_clamp;
  extern bool Has_GLX_OML_swap_method;
  extern bool Has_GLX_OML_sync_control;
  extern bool Has_GL_OML_interlace;
  extern bool Has_GL_OML_subsample;
  extern bool Has_GL_OML_resample;
  extern bool Has_WGL_OML_sync_control;
  extern bool Has_GL_NV_copy_depth_to_color;
  extern bool Has_GL_ATI_envmap_bumpmap;
  extern bool Has_GL_ATI_fragment_shader;
  extern bool Has_GL_ATI_pn_triangles;
  extern bool Has_GL_ATI_vertex_array_object;
  extern bool Has_GL_EXT_vertex_shader;
  extern bool Has_GL_ATI_vertex_streams;
  extern bool Has_WGL_I3D_digital_video_control;
  extern bool Has_WGL_I3D_gamma;
  extern bool Has_WGL_I3D_genlock;
  extern bool Has_WGL_I3D_image_buffer;
  extern bool Has_WGL_I3D_swap_frame_lock;
  extern bool Has_WGL_I3D_swap_frame_usage;
  extern bool Has_GL_ATI_element_array;
  extern bool Has_GL_SUN_mesh_array;
  extern bool Has_GL_SUN_slice_accum;
  extern bool Has_GL_NV_multisample_filter_hint;
  extern bool Has_GL_NV_depth_clamp;
  extern bool Has_GL_NV_occlusion_query;
  extern bool Has_GL_NV_point_sprite;
  extern bool Has_WGL_NV_render_depth_texture;
  extern bool Has_WGL_NV_render_texture_rectangle;
  extern bool Has_GL_NV_texture_shader3;
  extern bool Has_GL_NV_vertex_program1_1;
  extern bool Has_GL_EXT_shadow_funcs;
  extern bool Has_GL_EXT_stencil_two_side;
  extern bool Has_GL_ATI_text_fragment_shader;
  extern bool Has_GL_APPLE_client_storage;
  extern bool Has_GL_APPLE_element_array;
  extern bool Has_GL_APPLE_fence;
  extern bool Has_GL_APPLE_vertex_array_object;
  extern bool Has_GL_APPLE_vertex_array_range;
  extern bool Has_GL_APPLE_ycbcr_422;
  extern bool Has_GL_S3_s3tc;
  extern bool Has_GL_ATI_draw_buffers;
  extern bool Has_WGL_ATI_pixel_format_float;
  extern bool Has_GL_ATI_texture_env_combine3;
  extern bool Has_GL_ATI_texture_float;
  extern bool Has_GL_NV_float_buffer;
  extern bool Has_WGL_NV_float_buffer;
  extern bool Has_GL_NV_fragment_program;
  extern bool Has_GL_NV_half_float;
  extern bool Has_GL_NV_pixel_data_range;
  extern bool Has_GL_NV_primitive_restart;
  extern bool Has_GL_NV_texture_expand_normal;
  extern bool Has_GL_NV_vertex_program2;
  extern bool Has_GL_ATI_map_object_buffer;
  extern bool Has_GL_ATI_separate_stencil;
  extern bool Has_GL_ATI_vertex_attrib_array_object;
  extern bool Has_GL_OES_byte_coordinates;
  extern bool Has_GL_OES_fixed_point;
  extern bool Has_GL_OES_single_precision;
  extern bool Has_GL_OES_compressed_paletted_texture;
  extern bool Has_GL_OES_read_format;
  extern bool Has_GL_OES_query_matrix;
  extern bool Has_GL_EXT_depth_bounds_test;
  extern bool Has_GL_EXT_texture_mirror_clamp;
  extern bool Has_GL_EXT_blend_equation_separate;
  extern bool Has_GL_MESA_pack_invert;
  extern bool Has_GL_MESA_ycbcr_texture;
  extern bool Has_GL_EXT_pixel_buffer_object;
  extern bool Has_GL_NV_fragment_program_option;
  extern bool Has_GL_NV_fragment_program2;
  extern bool Has_GL_NV_vertex_program2_option;
  extern bool Has_GL_NV_vertex_program3;
  extern bool Has_GLX_SGIX_hyperpipe;
  extern bool Has_GLX_MESA_agp_offset;
  extern bool Has_GL_EXT_texture_compression_dxt1;
  extern bool Has_GL_EXT_framebuffer_object;
  extern bool Has_GL_GREMEDY_string_marker;
  extern bool Has_GL_EXT_packed_depth_stencil;
  extern bool Has_WGL_3DL_stereo_control;
  extern bool Has_GL_EXT_stencil_clear_tag;
  extern bool Has_GL_EXT_texture_sRGB;
  extern bool Has_GL_EXT_framebuffer_blit;
  extern bool Has_GL_EXT_framebuffer_multisample;
  extern bool Has_GL_MESAX_texture_stack;
  extern bool Has_GL_EXT_timer_query;
  extern bool Has_GL_EXT_gpu_program_parameters;
  extern bool Has_GL_APPLE_flush_buffer_range;
  extern bool Has_GL_NV_gpu_program4;
  extern bool Has_GL_NV_geometry_program4;
  extern bool Has_GL_EXT_geometry_shader4;
  extern bool Has_GL_NV_vertex_program4;
  extern bool Has_GL_EXT_gpu_shader4;
  extern bool Has_GL_EXT_draw_instanced;
  extern bool Has_GL_EXT_packed_float;
  extern bool Has_WGL_EXT_pixel_format_packed_float;
  extern bool Has_GLX_EXT_fbconfig_packed_float;
  extern bool Has_GL_EXT_texture_array;
  extern bool Has_GL_EXT_texture_buffer_object;
  extern bool Has_GL_EXT_texture_compression_latc;
  extern bool Has_GL_EXT_texture_compression_rgtc;
  extern bool Has_GL_EXT_texture_shared_exponent;
  extern bool Has_GL_NV_depth_buffer_float;
  extern bool Has_GL_NV_fragment_program4;
  extern bool Has_GL_NV_framebuffer_multisample_coverage;
  extern bool Has_GL_EXT_framebuffer_sRGB;
  extern bool Has_GLX_EXT_framebuffer_sRGB;
  extern bool Has_WGL_EXT_framebuffer_sRGB;
  extern bool Has_GL_NV_geometry_shader4;
  extern bool Has_GL_NV_parameter_buffer_object;
  extern bool Has_GL_EXT_draw_buffers2;
  extern bool Has_GL_NV_transform_feedback;
  extern bool Has_GL_EXT_bindable_uniform;
  extern bool Has_GL_EXT_texture_integer;
  extern bool Has_GLX_EXT_texture_from_pixmap;
  extern bool Has_GL_GREMEDY_frame_terminator;
  extern bool Has_GL_NV_conditional_render;
  extern bool Has_GL_NV_present_video;
  extern bool Has_GLX_NV_present_video;
  extern bool Has_WGL_NV_present_video;
  extern bool Has_GLX_NV_video_output;
  extern bool Has_WGL_NV_video_output;
  extern bool Has_GLX_NV_swap_group;
  extern bool Has_WGL_NV_swap_group;
  extern bool Has_GL_EXT_transform_feedback;
  extern bool Has_GL_EXT_direct_state_access;
  extern bool Has_GL_EXT_vertex_array_bgra;
  extern bool Has_WGL_NV_gpu_affinity;
  extern bool Has_GL_EXT_texture_swizzle;
  extern bool Has_GL_NV_explicit_multisample;
  extern bool Has_GL_NV_transform_feedback2;
  extern bool Has_GL_ATI_meminfo;
  extern bool Has_GL_AMD_performance_monitor;
  extern bool Has_WGL_AMD_gpu_association;
  extern bool Has_GL_AMD_texture_texture4;
  extern bool Has_GL_AMD_vertex_shader_tessellator;
  extern bool Has_GL_EXT_provoking_vertex;
  extern bool Has_GL_EXT_texture_snorm;
  extern bool Has_GL_AMD_draw_buffers_blend;
  extern bool Has_GL_APPLE_texture_range;
  extern bool Has_GL_APPLE_float_pixels;
  extern bool Has_GL_APPLE_vertex_program_evaluators;
  extern bool Has_GL_APPLE_aux_depth_stencil;
  extern bool Has_GL_APPLE_object_purgeable;
  extern bool Has_GL_APPLE_row_bytes;
  extern bool Has_GL_APPLE_rgb_422;
  extern bool Has_GL_NV_video_capture;
  extern bool Has_GLX_NV_video_capture;
  extern bool Has_WGL_NV_video_capture;
  extern bool Has_GL_EXT_swap_control;
  extern bool Has_GL_NV_copy_image;
  extern bool Has_WGL_NV_copy_image;
  extern bool Has_GLX_NV_copy_image;
  extern bool Has_GL_EXT_separate_shader_objects;
  extern bool Has_GL_NV_parameter_buffer_object2;
  extern bool Has_GL_NV_shader_buffer_load;
  extern bool Has_GL_NV_vertex_buffer_unified_memory;
  extern bool Has_GL_NV_texture_barrier;
  extern bool Has_GL_AMD_shader_stencil_export;
  extern bool Has_GL_AMD_seamless_cubemap_per_texture;
  extern bool Has_GLX_INTEL_swap_event;
  extern bool Has_GL_AMD_conservative_depth;
  extern bool Has_GL_EXT_shader_image_load_store;
  extern bool Has_GL_EXT_vertex_attrib_64bit;
  extern bool Has_GL_NV_gpu_program5;
  extern bool Has_GL_NV_gpu_shader5;
  extern bool Has_GL_NV_shader_buffer_store;
  extern bool Has_GL_NV_tessellation_program5;
  extern bool Has_GL_NV_vertex_attrib_integer_64bit;
  extern bool Has_GL_NV_multisample_coverage;
  extern bool Has_GL_AMD_name_gen_delete;
  extern bool Has_GL_AMD_debug_output;
  extern bool Has_GL_NV_vdpau_interop;
  extern bool Has_GL_AMD_transform_feedback3_lines_triangles;
  extern bool Has_GLX_AMD_gpu_association;
  extern bool Has_GLX_EXT_create_context_es2_profile;
  extern bool Has_WGL_EXT_create_context_es2_profile;
  extern bool Has_GL_AMD_depth_clamp_separate;
  extern bool Has_GL_EXT_texture_sRGB_decode;
}

extern "C"
{
  // GL_VERSION_1_2
  extern PFNGLBLENDCOLORPROC glBlendColor;
  extern PFNGLBLENDEQUATIONPROC glBlendEquation;
  extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
  extern PFNGLTEXIMAGE3DPROC glTexImage3D;
  extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
  extern PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

  // GL_VERSION_1_2_DEPRECATED
  extern PFNGLCOLORTABLEPROC glColorTable;
  extern PFNGLCOLORTABLEPARAMETERFVPROC glColorTableParameterfv;
  extern PFNGLCOLORTABLEPARAMETERIVPROC glColorTableParameteriv;
  extern PFNGLCOPYCOLORTABLEPROC glCopyColorTable;
  extern PFNGLGETCOLORTABLEPROC glGetColorTable;
  extern PFNGLGETCOLORTABLEPARAMETERFVPROC glGetColorTableParameterfv;
  extern PFNGLGETCOLORTABLEPARAMETERIVPROC glGetColorTableParameteriv;
  extern PFNGLCOLORSUBTABLEPROC glColorSubTable;
  extern PFNGLCOPYCOLORSUBTABLEPROC glCopyColorSubTable;
  extern PFNGLCONVOLUTIONFILTER1DPROC glConvolutionFilter1D;
  extern PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D;
  extern PFNGLCONVOLUTIONPARAMETERFPROC glConvolutionParameterf;
  extern PFNGLCONVOLUTIONPARAMETERFVPROC glConvolutionParameterfv;
  extern PFNGLCONVOLUTIONPARAMETERIPROC glConvolutionParameteri;
  extern PFNGLCONVOLUTIONPARAMETERIVPROC glConvolutionParameteriv;
  extern PFNGLCOPYCONVOLUTIONFILTER1DPROC glCopyConvolutionFilter1D;
  extern PFNGLCOPYCONVOLUTIONFILTER2DPROC glCopyConvolutionFilter2D;
  extern PFNGLGETCONVOLUTIONFILTERPROC glGetConvolutionFilter;
  extern PFNGLGETCONVOLUTIONPARAMETERFVPROC glGetConvolutionParameterfv;
  extern PFNGLGETCONVOLUTIONPARAMETERIVPROC glGetConvolutionParameteriv;
  extern PFNGLGETSEPARABLEFILTERPROC glGetSeparableFilter;
  extern PFNGLSEPARABLEFILTER2DPROC glSeparableFilter2D;
  extern PFNGLGETHISTOGRAMPROC glGetHistogram;
  extern PFNGLGETHISTOGRAMPARAMETERFVPROC glGetHistogramParameterfv;
  extern PFNGLGETHISTOGRAMPARAMETERIVPROC glGetHistogramParameteriv;
  extern PFNGLGETMINMAXPROC glGetMinmax;
  extern PFNGLGETMINMAXPARAMETERFVPROC glGetMinmaxParameterfv;
  extern PFNGLGETMINMAXPARAMETERIVPROC glGetMinmaxParameteriv;
  extern PFNGLHISTOGRAMPROC glHistogram;
  extern PFNGLMINMAXPROC glMinmax;
  extern PFNGLRESETHISTOGRAMPROC glResetHistogram;
  extern PFNGLRESETMINMAXPROC glResetMinmax;

  // GL_VERSION_1_3
  extern PFNGLACTIVETEXTUREPROC glActiveTexture;
  extern PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
  extern PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
  extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
  extern PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
  extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
  extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
  extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
  extern PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;

  // GL_VERSION_1_3_DEPRECATED
  extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
  extern PFNGLMULTITEXCOORD1DPROC glMultiTexCoord1d;
  extern PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv;
  extern PFNGLMULTITEXCOORD1FPROC glMultiTexCoord1f;
  extern PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv;
  extern PFNGLMULTITEXCOORD1IPROC glMultiTexCoord1i;
  extern PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv;
  extern PFNGLMULTITEXCOORD1SPROC glMultiTexCoord1s;
  extern PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv;
  extern PFNGLMULTITEXCOORD2DPROC glMultiTexCoord2d;
  extern PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv;
  extern PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
  extern PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv;
  extern PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i;
  extern PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv;
  extern PFNGLMULTITEXCOORD2SPROC glMultiTexCoord2s;
  extern PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv;
  extern PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d;
  extern PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv;
  extern PFNGLMULTITEXCOORD3FPROC glMultiTexCoord3f;
  extern PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv;
  extern PFNGLMULTITEXCOORD3IPROC glMultiTexCoord3i;
  extern PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv;
  extern PFNGLMULTITEXCOORD3SPROC glMultiTexCoord3s;
  extern PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv;
  extern PFNGLMULTITEXCOORD4DPROC glMultiTexCoord4d;
  extern PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv;
  extern PFNGLMULTITEXCOORD4FPROC glMultiTexCoord4f;
  extern PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv;
  extern PFNGLMULTITEXCOORD4IPROC glMultiTexCoord4i;
  extern PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv;
  extern PFNGLMULTITEXCOORD4SPROC glMultiTexCoord4s;
  extern PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv;
  extern PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;
  extern PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;
  extern PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;
  extern PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;

  // GL_VERSION_1_4
  extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
  extern PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
  extern PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
  extern PFNGLPOINTPARAMETERFPROC glPointParameterf;
  extern PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
  extern PFNGLPOINTPARAMETERIPROC glPointParameteri;
  extern PFNGLPOINTPARAMETERIVPROC glPointParameteriv;

  // GL_VERSION_1_4_DEPRECATED
  extern PFNGLFOGCOORDFPROC glFogCoordf;
  extern PFNGLFOGCOORDFVPROC glFogCoordfv;
  extern PFNGLFOGCOORDDPROC glFogCoordd;
  extern PFNGLFOGCOORDDVPROC glFogCoorddv;
  extern PFNGLFOGCOORDPOINTERPROC glFogCoordPointer;
  extern PFNGLSECONDARYCOLOR3BPROC glSecondaryColor3b;
  extern PFNGLSECONDARYCOLOR3BVPROC glSecondaryColor3bv;
  extern PFNGLSECONDARYCOLOR3DPROC glSecondaryColor3d;
  extern PFNGLSECONDARYCOLOR3DVPROC glSecondaryColor3dv;
  extern PFNGLSECONDARYCOLOR3FPROC glSecondaryColor3f;
  extern PFNGLSECONDARYCOLOR3FVPROC glSecondaryColor3fv;
  extern PFNGLSECONDARYCOLOR3IPROC glSecondaryColor3i;
  extern PFNGLSECONDARYCOLOR3IVPROC glSecondaryColor3iv;
  extern PFNGLSECONDARYCOLOR3SPROC glSecondaryColor3s;
  extern PFNGLSECONDARYCOLOR3SVPROC glSecondaryColor3sv;
  extern PFNGLSECONDARYCOLOR3UBPROC glSecondaryColor3ub;
  extern PFNGLSECONDARYCOLOR3UBVPROC glSecondaryColor3ubv;
  extern PFNGLSECONDARYCOLOR3UIPROC glSecondaryColor3ui;
  extern PFNGLSECONDARYCOLOR3UIVPROC glSecondaryColor3uiv;
  extern PFNGLSECONDARYCOLOR3USPROC glSecondaryColor3us;
  extern PFNGLSECONDARYCOLOR3USVPROC glSecondaryColor3usv;
  extern PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer;
  extern PFNGLWINDOWPOS2DPROC glWindowPos2d;
  extern PFNGLWINDOWPOS2DVPROC glWindowPos2dv;
  extern PFNGLWINDOWPOS2FPROC glWindowPos2f;
  extern PFNGLWINDOWPOS2FVPROC glWindowPos2fv;
  extern PFNGLWINDOWPOS2IPROC glWindowPos2i;
  extern PFNGLWINDOWPOS2IVPROC glWindowPos2iv;
  extern PFNGLWINDOWPOS2SPROC glWindowPos2s;
  extern PFNGLWINDOWPOS2SVPROC glWindowPos2sv;
  extern PFNGLWINDOWPOS3DPROC glWindowPos3d;
  extern PFNGLWINDOWPOS3DVPROC glWindowPos3dv;
  extern PFNGLWINDOWPOS3FPROC glWindowPos3f;
  extern PFNGLWINDOWPOS3FVPROC glWindowPos3fv;
  extern PFNGLWINDOWPOS3IPROC glWindowPos3i;
  extern PFNGLWINDOWPOS3IVPROC glWindowPos3iv;
  extern PFNGLWINDOWPOS3SPROC glWindowPos3s;
  extern PFNGLWINDOWPOS3SVPROC glWindowPos3sv;

  // GL_VERSION_1_5
  extern PFNGLGENQUERIESPROC glGenQueries;
  extern PFNGLDELETEQUERIESPROC glDeleteQueries;
  extern PFNGLISQUERYPROC glIsQuery;
  extern PFNGLBEGINQUERYPROC glBeginQuery;
  extern PFNGLENDQUERYPROC glEndQuery;
  extern PFNGLGETQUERYIVPROC glGetQueryiv;
  extern PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
  extern PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
  extern PFNGLBINDBUFFERPROC glBindBuffer;
  extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
  extern PFNGLGENBUFFERSPROC glGenBuffers;
  extern PFNGLISBUFFERPROC glIsBuffer;
  extern PFNGLBUFFERDATAPROC glBufferData;
  extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
  extern PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
  extern PFNGLMAPBUFFERPROC glMapBuffer;
  extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;
  extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
  extern PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;

  // GL_VERSION_2_0
  extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
  extern PFNGLDRAWBUFFERSPROC glDrawBuffers;
  extern PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
  extern PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
  extern PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
  extern PFNGLATTACHSHADERPROC glAttachShader;
  extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
  extern PFNGLCOMPILESHADERPROC glCompileShader;
  extern PFNGLCREATEPROGRAMPROC glCreateProgram;
  extern PFNGLCREATESHADERPROC glCreateShader;
  extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
  extern PFNGLDELETESHADERPROC glDeleteShader;
  extern PFNGLDETACHSHADERPROC glDetachShader;
  extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
  extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
  extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
  extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
  extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
  extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
  extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
  extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
  extern PFNGLGETSHADERIVPROC glGetShaderiv;
  extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
  extern PFNGLGETSHADERSOURCEPROC glGetShaderSource;
  extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
  extern PFNGLGETUNIFORMFVPROC glGetUniformfv;
  extern PFNGLGETUNIFORMIVPROC glGetUniformiv;
  extern PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
  extern PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
  extern PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
  extern PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
  extern PFNGLISPROGRAMPROC glIsProgram;
  extern PFNGLISSHADERPROC glIsShader;
  extern PFNGLLINKPROGRAMPROC glLinkProgram;
  extern PFNGLSHADERSOURCEPROC glShaderSource;
  extern PFNGLUSEPROGRAMPROC glUseProgram;
  extern PFNGLUNIFORM1FPROC glUniform1f;
  extern PFNGLUNIFORM2FPROC glUniform2f;
  extern PFNGLUNIFORM3FPROC glUniform3f;
  extern PFNGLUNIFORM4FPROC glUniform4f;
  extern PFNGLUNIFORM1IPROC glUniform1i;
  extern PFNGLUNIFORM2IPROC glUniform2i;
  extern PFNGLUNIFORM3IPROC glUniform3i;
  extern PFNGLUNIFORM4IPROC glUniform4i;
  extern PFNGLUNIFORM1FVPROC glUniform1fv;
  extern PFNGLUNIFORM2FVPROC glUniform2fv;
  extern PFNGLUNIFORM3FVPROC glUniform3fv;
  extern PFNGLUNIFORM4FVPROC glUniform4fv;
  extern PFNGLUNIFORM1IVPROC glUniform1iv;
  extern PFNGLUNIFORM2IVPROC glUniform2iv;
  extern PFNGLUNIFORM3IVPROC glUniform3iv;
  extern PFNGLUNIFORM4IVPROC glUniform4iv;
  extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
  extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
  extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
  extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
  extern PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
  extern PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
  extern PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
  extern PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
  extern PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
  extern PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
  extern PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
  extern PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
  extern PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
  extern PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
  extern PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
  extern PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
  extern PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
  extern PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
  extern PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
  extern PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
  extern PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
  extern PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
  extern PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
  extern PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
  extern PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
  extern PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
  extern PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
  extern PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
  extern PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
  extern PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
  extern PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
  extern PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
  extern PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
  extern PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
  extern PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
  extern PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
  extern PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
  extern PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
  extern PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
  extern PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
  extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

  // GL_VERSION_2_1
  extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
  extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
  extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
  extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
  extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
  extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

  // GL_VERSION_3_0
  /* OpenGL 3.0 also reuses entry points from these extensions: */
  /* ARB_framebuffer_object */
  /* ARB_map_buffer_range */
  /* ARB_vertex_array_object */
  extern PFNGLCOLORMASKIPROC glColorMaski;
  // PFNGLGETBOOLEANINDEXEDVEXTPROC glGetBooleanIndexedvEXT;
  // PFNGLGETINTEGERINDEXEDVEXTPROC glGetIntegerIndexedvEXT;
  extern PFNGLENABLEIPROC glEnablei;
  extern PFNGLDISABLEIPROC glDisablei;
  extern PFNGLISENABLEDIPROC glIsEnabledi;
  extern PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
  extern PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
  extern PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
  extern PFNGLBINDBUFFERBASEPROC glBindBufferBase;
  extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
  extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
  extern PFNGLCLAMPCOLORPROC glClampColor;
  extern PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
  extern PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
  extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
  extern PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
  extern PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
  extern PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
  extern PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
  extern PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
  extern PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
  extern PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
  extern PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
  extern PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
  extern PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
  extern PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
  extern PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
  extern PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
  extern PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
  extern PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
  extern PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
  extern PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
  extern PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
  extern PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
  extern PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
  extern PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
  extern PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
  extern PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
  extern PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
  extern PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
  extern PFNGLUNIFORM1UIPROC glUniform1ui;
  extern PFNGLUNIFORM2UIPROC glUniform2ui;
  extern PFNGLUNIFORM3UIPROC glUniform3ui;
  extern PFNGLUNIFORM4UIPROC glUniform4ui;
  extern PFNGLUNIFORM1UIVPROC glUniform1uiv;
  extern PFNGLUNIFORM2UIVPROC glUniform2uiv;
  extern PFNGLUNIFORM3UIVPROC glUniform3uiv;
  extern PFNGLUNIFORM4UIVPROC glUniform4uiv;
  extern PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
  extern PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
  extern PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
  extern PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
  extern PFNGLCLEARBUFFERIVPROC glClearBufferiv;
  extern PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
  extern PFNGLCLEARBUFFERFVPROC glClearBufferfv;
  extern PFNGLCLEARBUFFERFIPROC glClearBufferfi;
  extern PFNGLGETSTRINGIPROC glGetStringi;


  // GL_VERSION_3_1
  /* OpenGL 3.1 also reuses entry points from these extensions: */
  /* ARB_copy_buffer */
  /* ARB_uniform_buffer_object */
  extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
  extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
  extern PFNGLTEXBUFFERPROC glTexBuffer;
  extern PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;

  // GL_VERSION_3_2
  /* OpenGL 3.2 also reuses entry points from these extensions: */
  /* ARB_draw_elements_base_vertex */
  /* ARB_provoking_vertex */
  /* ARB_sync */
  /* ARB_texture_multisample */
  extern PFNGLGETINTEGER64I_VPROC  glGetInteger64i_v;
  extern PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
  extern PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;

  // GL_VERSION_3_3
  /* OpenGL 3.3 also reuses entry points from these extensions: */
  /* ARB_blend_func_extended */
  /* ARB_sampler_objects */
  /* ARB_explicit_attrib_location, but it has none */
  /* ARB_occlusion_query2  */
  /* ARB_shader_bit_encoding  */
  /* ARB_texture_rgb10_a2ui  */
  /* ARB_texture_swizzle  */
  /* ARB_timer_query */
  /* ARB_vertex_type_2_10_10_10_rev */
  extern PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;

  // GL_VERSION_4_0
  /* OpenGL 4.0 also reuses entry points from these extensions: */
  /* ARB_texture_query_lod  */
  /* ARB_draw_indirect */
  /* ARB_gpu_shader5  */
  /* ARB_gpu_shader_fp64 */
  /* ARB_shader_subroutine */
  /* ARB_tessellation_shader */
  /* ARB_texture_buffer_object_rgb32  */
  /* ARB_texture_cube_map_array  */
  /* ARB_texture_gather  */
  /* ARB_transform_feedback2 */
  /* ARB_transform_feedback3 */
  extern PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
  extern PFNGLBLENDEQUATIONIPROC glBlendEquationi;
  extern PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
  extern PFNGLBLENDFUNCIPROC glBlendFunci;
  extern PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;

  // GL_VERSION_4_1
  /* OpenGL 4.1 also reuses entry points from these extensions: */
  /* ARB_ES2_compatibility */
  /* ARB_get_program_binary */
  /* ARB_separate_shader_objects */
  /* ARB_shader_precision  */
  /* ARB_vertex_attrib_64bit */
  /* ARB_viewport_array */
  // NO OTHER PROCEDURES DEFINED

  // GL_ARB_multitexture
  extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
  extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
  extern PFNGLMULTITEXCOORD1DARBPROC glMultiTexCoord1dARB;
  extern PFNGLMULTITEXCOORD1DVARBPROC glMultiTexCoord1dvARB;
  extern PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
  extern PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fvARB;
  extern PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB;
  extern PFNGLMULTITEXCOORD1IVARBPROC glMultiTexCoord1ivARB;
  extern PFNGLMULTITEXCOORD1SARBPROC glMultiTexCoord1sARB;
  extern PFNGLMULTITEXCOORD1SVARBPROC glMultiTexCoord1svARB;
  extern PFNGLMULTITEXCOORD2DARBPROC glMultiTexCoord2dARB;
  extern PFNGLMULTITEXCOORD2DVARBPROC glMultiTexCoord2dvARB;
  extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
  extern PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;
  extern PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB;
  extern PFNGLMULTITEXCOORD2IVARBPROC glMultiTexCoord2ivARB;
  extern PFNGLMULTITEXCOORD2SARBPROC glMultiTexCoord2sARB;
  extern PFNGLMULTITEXCOORD2SVARBPROC glMultiTexCoord2svARB;
  extern PFNGLMULTITEXCOORD3DARBPROC glMultiTexCoord3dARB;
  extern PFNGLMULTITEXCOORD3DVARBPROC glMultiTexCoord3dvARB;
  extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
  extern PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB;
  extern PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB;
  extern PFNGLMULTITEXCOORD3IVARBPROC glMultiTexCoord3ivARB;
  extern PFNGLMULTITEXCOORD3SARBPROC glMultiTexCoord3sARB;
  extern PFNGLMULTITEXCOORD3SVARBPROC glMultiTexCoord3svARB;
  extern PFNGLMULTITEXCOORD4DARBPROC glMultiTexCoord4dARB;
  extern PFNGLMULTITEXCOORD4DVARBPROC glMultiTexCoord4dvARB;
  extern PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
  extern PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB;
  extern PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB;
  extern PFNGLMULTITEXCOORD4IVARBPROC glMultiTexCoord4ivARB;
  extern PFNGLMULTITEXCOORD4SARBPROC glMultiTexCoord4sARB;
  extern PFNGLMULTITEXCOORD4SVARBPROC glMultiTexCoord4svARB;

  // GL_ARB_transpose_matrix
  extern PFNGLLOADTRANSPOSEMATRIXFARBPROC glLoadTransposeMatrixfARB;
  extern PFNGLLOADTRANSPOSEMATRIXDARBPROC glLoadTransposeMatrixdARB;
  extern PFNGLMULTTRANSPOSEMATRIXFARBPROC glMultTransposeMatrixfARB;
  extern PFNGLMULTTRANSPOSEMATRIXDARBPROC glMultTransposeMatrixdARB;

  // GL_ARB_multisample
  extern PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB;

  // GL_ARB_texture_compression
  extern PFNGLCOMPRESSEDTEXIMAGE3DARBPROC glCompressedTexImage3DARB;
  extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;
  extern PFNGLCOMPRESSEDTEXIMAGE1DARBPROC glCompressedTexImage1DARB;
  extern PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glCompressedTexSubImage3DARB;
  extern PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB;
  extern PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glCompressedTexSubImage1DARB;
  extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB;

  // GL_ARB_point_parameters
  extern PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB;
  extern PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB;

  // GL_ARB_vertex_blend
  extern PFNGLWEIGHTBVARBPROC glWeightbvARB;
  extern PFNGLWEIGHTSVARBPROC glWeightsvARB;
  extern PFNGLWEIGHTIVARBPROC glWeightivARB;
  extern PFNGLWEIGHTFVARBPROC glWeightfvARB;
  extern PFNGLWEIGHTDVARBPROC glWeightdvARB;
  extern PFNGLWEIGHTUBVARBPROC glWeightubvARB;
  extern PFNGLWEIGHTUSVARBPROC glWeightusvARB;
  extern PFNGLWEIGHTUIVARBPROC glWeightuivARB;
  extern PFNGLWEIGHTPOINTERARBPROC glWeightPointerARB;
  extern PFNGLVERTEXBLENDARBPROC glVertexBlendARB;

  // GL_ARB_matrix_palette
  extern PFNGLCURRENTPALETTEMATRIXARBPROC glCurrentPaletteMatrixARB;
  extern PFNGLMATRIXINDEXUBVARBPROC glMatrixIndexubvARB;
  extern PFNGLMATRIXINDEXUSVARBPROC glMatrixIndexusvARB;
  extern PFNGLMATRIXINDEXUIVARBPROC glMatrixIndexuivARB;
  extern PFNGLMATRIXINDEXPOINTERARBPROC glMatrixIndexPointerARB;

  // GL_ARB_window_pos
  extern PFNGLWINDOWPOS2DARBPROC glWindowPos2dARB;
  extern PFNGLWINDOWPOS2DVARBPROC glWindowPos2dvARB;
  extern PFNGLWINDOWPOS2FARBPROC glWindowPos2fARB;
  extern PFNGLWINDOWPOS2FVARBPROC glWindowPos2fvARB;
  extern PFNGLWINDOWPOS2IARBPROC glWindowPos2iARB;
  extern PFNGLWINDOWPOS2IVARBPROC glWindowPos2ivARB;
  extern PFNGLWINDOWPOS2SARBPROC glWindowPos2sARB;
  extern PFNGLWINDOWPOS2SVARBPROC glWindowPos2svARB;
  extern PFNGLWINDOWPOS3DARBPROC glWindowPos3dARB;
  extern PFNGLWINDOWPOS3DVARBPROC glWindowPos3dvARB;
  extern PFNGLWINDOWPOS3FARBPROC glWindowPos3fARB;
  extern PFNGLWINDOWPOS3FVARBPROC glWindowPos3fvARB;
  extern PFNGLWINDOWPOS3IARBPROC glWindowPos3iARB;
  extern PFNGLWINDOWPOS3IVARBPROC glWindowPos3ivARB;
  extern PFNGLWINDOWPOS3SARBPROC glWindowPos3sARB;
  extern PFNGLWINDOWPOS3SVARBPROC glWindowPos3svARB;

  // GL_ARB_vertex_program
  extern PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB;
  extern PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB;
  extern PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
  extern PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
  extern PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB;
  extern PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB;
  extern PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB;
  extern PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB;
  extern PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
  extern PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
  extern PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB;
  extern PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB;
  extern PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB;
  extern PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB;
  extern PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
  extern PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
  extern PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB;
  extern PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB;
  extern PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB;
  extern PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB;
  extern PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB;
  extern PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB;
  extern PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB;
  extern PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB;
  extern PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB;
  extern PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB;
  extern PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB;
  extern PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB;
  extern PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
  extern PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
  extern PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
  extern PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB;
  extern PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB;
  extern PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB;
  extern PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB;
  extern PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB;
  extern PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB;
  extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
  extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;
  extern PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
  extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
  extern PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
  extern PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
  extern PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB;
  extern PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB;
  extern PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB;
  extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
  extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB;
  extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB;
  extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
  extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
  extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB;
  extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB;
  extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
  extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
  extern PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
  extern PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB;
  extern PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB;
  extern PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB;
  extern PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB;
  extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB;
  extern PFNGLISPROGRAMARBPROC glIsProgramARB;

  // GL_ARB_vertex_buffer_object
  extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
  extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
  extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
  extern PFNGLISBUFFERARBPROC glIsBufferARB;
  extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
  extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
  extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
  extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
  extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
  extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
  extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;

  // GL_ARB_occlusion_query
  extern PFNGLGENQUERIESARBPROC glGenQueriesARB;
  extern PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB;
  extern PFNGLISQUERYARBPROC glIsQueryARB;
  extern PFNGLBEGINQUERYARBPROC glBeginQueryARB;
  extern PFNGLENDQUERYARBPROC glEndQueryARB;
  extern PFNGLGETQUERYIVARBPROC glGetQueryivARB;
  extern PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB;
  extern PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB;

  // GL_ARB_shader_objects
  extern PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
  extern PFNGLGETHANDLEARBPROC glGetHandleARB;
  extern PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
  extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
  extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
  extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
  extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
  extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
  extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
  extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
  extern PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB;
  extern PFNGLUNIFORM1FARBPROC glUniform1fARB;
  extern PFNGLUNIFORM2FARBPROC glUniform2fARB;
  extern PFNGLUNIFORM3FARBPROC glUniform3fARB;
  extern PFNGLUNIFORM4FARBPROC glUniform4fARB;
  extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
  extern PFNGLUNIFORM2IARBPROC glUniform2iARB;
  extern PFNGLUNIFORM3IARBPROC glUniform3iARB;
  extern PFNGLUNIFORM4IARBPROC glUniform4iARB;
  extern PFNGLUNIFORM1FVARBPROC glUniform1fvARB;
  extern PFNGLUNIFORM2FVARBPROC glUniform2fvARB;
  extern PFNGLUNIFORM3FVARBPROC glUniform3fvARB;
  extern PFNGLUNIFORM4FVARBPROC glUniform4fvARB;
  extern PFNGLUNIFORM1IVARBPROC glUniform1ivARB;
  extern PFNGLUNIFORM2IVARBPROC glUniform2ivARB;
  extern PFNGLUNIFORM3IVARBPROC glUniform3ivARB;
  extern PFNGLUNIFORM4IVARBPROC glUniform4ivARB;
  extern PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB;
  extern PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB;
  extern PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
  extern PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB;
  extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
  extern PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
  extern PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjectsARB;
  extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
  extern PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniformARB;
  extern PFNGLGETUNIFORMFVARBPROC glGetUniformfvARB;
  extern PFNGLGETUNIFORMIVARBPROC glGetUniformivARB;
  extern PFNGLGETSHADERSOURCEARBPROC glGetShaderSourceARB;

  // GL_ARB_vertex_shader
  extern PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB;
  extern PFNGLGETACTIVEATTRIBARBPROC glGetActiveAttribARB;
  extern PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB;

  // GL_ARB_draw_buffers
  extern PFNGLDRAWBUFFERSARBPROC glDrawBuffersARB;

  // GL_ARB_color_buffer_float
  extern PFNGLCLAMPCOLORARBPROC glClampColorARB;

  // GL_ARB_draw_instanced
  extern PFNGLDRAWARRAYSINSTANCEDARBPROC glDrawArraysInstancedARB;
  extern PFNGLDRAWELEMENTSINSTANCEDARBPROC glDrawElementsInstancedARB;

  // GL_ARB_framebuffer_object
  extern PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
  extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
  extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
  extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
  extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
  extern PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
  extern PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
  extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
  extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
  extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
  extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
  extern PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
  extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
  extern PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
  extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
  extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
  extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
  extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
  extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
  extern PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;

  // GL_ARB_geometry_shader4
  extern PFNGLPROGRAMPARAMETERIARBPROC glProgramParameteriARB;
  extern PFNGLFRAMEBUFFERTEXTUREARBPROC glFramebufferTextureARB;
  extern PFNGLFRAMEBUFFERTEXTURELAYERARBPROC glFramebufferTextureLayerARB;
  extern PFNGLFRAMEBUFFERTEXTUREFACEARBPROC glFramebufferTextureFaceARB;

  // GL_ARB_instanced_arrays
  extern PFNGLVERTEXATTRIBDIVISORARBPROC glVertexAttribDivisorARB;

  // GL_ARB_map_buffer_range
  extern PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
  extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;

  // GL_ARB_texture_buffer_object
  extern PFNGLTEXBUFFERARBPROC glTexBufferARB;

  // GL_ARB_vertex_array_object
  extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
  extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
  extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
  extern PFNGLISVERTEXARRAYPROC glIsVertexArray;

  // GL_ARB_uniform_buffer_object
  extern PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
  extern PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
  extern PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
  extern PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
  extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
  extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
  extern PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;

  // GL_ARB_copy_buffer
  extern PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;

  // GL_ARB_draw_elements_base_vertex
  extern PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
  extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
  extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
  extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;

  // GL_ARB_provoking_vertex
  extern PFNGLPROVOKINGVERTEXPROC glProvokingVertex;

  // GL_ARB_sync
  extern PFNGLFENCESYNCPROC glFenceSync;
  extern PFNGLISSYNCPROC glIsSync;
  extern PFNGLDELETESYNCPROC glDeleteSync;
  extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
  extern PFNGLWAITSYNCPROC glWaitSync;
  extern PFNGLGETINTEGER64VPROC glGetInteger64v;
  extern PFNGLGETSYNCIVPROC glGetSynciv;

  // GL_ARB_texture_multisample
  extern PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
  extern PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
  extern PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
  extern PFNGLSAMPLEMASKIPROC glSampleMaski;

  // GL_ARB_draw_buffers_blend
  extern PFNGLBLENDEQUATIONIARBPROC glBlendEquationiARB;
  extern PFNGLBLENDEQUATIONSEPARATEIARBPROC glBlendEquationSeparateiARB;
  extern PFNGLBLENDFUNCIARBPROC glBlendFunciARB;
  extern PFNGLBLENDFUNCSEPARATEIARBPROC glBlendFuncSeparateiARB;

  // GL_ARB_sample_shading
  extern PFNGLMINSAMPLESHADINGARBPROC glMinSampleShadingARB;

  // GL_ARB_shading_language_include
  extern PFNGLNAMEDSTRINGARBPROC glNamedStringARB;
  extern PFNGLDELETENAMEDSTRINGARBPROC glDeleteNamedStringARB;
  extern PFNGLCOMPILESHADERINCLUDEARBPROC glCompileShaderIncludeARB;
  extern PFNGLISNAMEDSTRINGARBPROC glIsNamedStringARB;
  extern PFNGLGETNAMEDSTRINGARBPROC glGetNamedStringARB;
  extern PFNGLGETNAMEDSTRINGIVARBPROC glGetNamedStringivARB;

  // GL_ARB_blend_func_extended
  extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
  extern PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex;

  // GL_ARB_sampler_objects
  extern PFNGLGENSAMPLERSPROC glGenSamplers;
  extern PFNGLDELETESAMPLERSPROC glDeleteSamplers;
  extern PFNGLISSAMPLERPROC glIsSampler;
  extern PFNGLBINDSAMPLERPROC glBindSampler;
  extern PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
  extern PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
  extern PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
  extern PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
  extern PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
  extern PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
  extern PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
  extern PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
  extern PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
  extern PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;

  // GL_ARB_timer_query
  extern PFNGLQUERYCOUNTERPROC glQueryCounter;
  extern PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
  extern PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;

  // GL_ARB_vertex_type_2_10_10_10_rev
  extern PFNGLVERTEXP2UIPROC glVertexP2ui;
  extern PFNGLVERTEXP2UIVPROC glVertexP2uiv;
  extern PFNGLVERTEXP3UIPROC glVertexP3ui;
  extern PFNGLVERTEXP3UIVPROC glVertexP3uiv;
  extern PFNGLVERTEXP4UIPROC glVertexP4ui;
  extern PFNGLVERTEXP4UIVPROC glVertexP4uiv;
  extern PFNGLTEXCOORDP1UIPROC glTexCoordP1ui;
  extern PFNGLTEXCOORDP1UIVPROC glTexCoordP1uiv;
  extern PFNGLTEXCOORDP2UIPROC glTexCoordP2ui;
  extern PFNGLTEXCOORDP2UIVPROC glTexCoordP2uiv;
  extern PFNGLTEXCOORDP3UIPROC glTexCoordP3ui;
  extern PFNGLTEXCOORDP3UIVPROC glTexCoordP3uiv;
  extern PFNGLTEXCOORDP4UIPROC glTexCoordP4ui;
  extern PFNGLTEXCOORDP4UIVPROC glTexCoordP4uiv;
  extern PFNGLMULTITEXCOORDP1UIPROC glMultiTexCoordP1ui;
  extern PFNGLMULTITEXCOORDP1UIVPROC glMultiTexCoordP1uiv;
  extern PFNGLMULTITEXCOORDP2UIPROC glMultiTexCoordP2ui;
  extern PFNGLMULTITEXCOORDP2UIVPROC glMultiTexCoordP2uiv;
  extern PFNGLMULTITEXCOORDP3UIPROC glMultiTexCoordP3ui;
  extern PFNGLMULTITEXCOORDP3UIVPROC glMultiTexCoordP3uiv;
  extern PFNGLMULTITEXCOORDP4UIPROC glMultiTexCoordP4ui;
  extern PFNGLMULTITEXCOORDP4UIVPROC glMultiTexCoordP4uiv;
  extern PFNGLNORMALP3UIPROC glNormalP3ui;
  extern PFNGLNORMALP3UIVPROC glNormalP3uiv;
  extern PFNGLCOLORP3UIPROC glColorP3ui;
  extern PFNGLCOLORP3UIVPROC glColorP3uiv;
  extern PFNGLCOLORP4UIPROC glColorP4ui;
  extern PFNGLCOLORP4UIVPROC glColorP4uiv;
  extern PFNGLSECONDARYCOLORP3UIPROC glSecondaryColorP3ui;
  extern PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv;
  extern PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui;
  extern PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
  extern PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui;
  extern PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
  extern PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui;
  extern PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
  extern PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui;
  extern PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;

  // GL_ARB_draw_indirect
  extern PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
  extern PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;

  // GL_ARB_gpu_shader_fp64
  extern PFNGLUNIFORM1DPROC glUniform1d;
  extern PFNGLUNIFORM2DPROC glUniform2d;
  extern PFNGLUNIFORM3DPROC glUniform3d;
  extern PFNGLUNIFORM4DPROC glUniform4d;
  extern PFNGLUNIFORM1DVPROC glUniform1dv;
  extern PFNGLUNIFORM2DVPROC glUniform2dv;
  extern PFNGLUNIFORM3DVPROC glUniform3dv;
  extern PFNGLUNIFORM4DVPROC glUniform4dv;
  extern PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv;
  extern PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv;
  extern PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv;
  extern PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
  extern PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
  extern PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
  extern PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
  extern PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
  extern PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
  extern PFNGLGETUNIFORMDVPROC glGetUniformdv;

  // GL_ARB_shader_subroutine
  extern PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation;
  extern PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex;
  extern PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv;
  extern PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
  extern PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName;
  extern PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv;
  extern PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv;
  extern PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv;

  // GL_ARB_tessellation_shader
  extern PFNGLPATCHPARAMETERIPROC glPatchParameteri;
  extern PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;

  // GL_ARB_transform_feedback2
  extern PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback;
  extern PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
  extern PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks;
  extern PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback;
  extern PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback;
  extern PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback;
  extern PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback;

  // GL_ARB_transform_feedback3
  extern PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
  extern PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed;
  extern PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed;
  extern PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv;

  // GL_ARB_ES2_compatibility
  extern PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler;
  extern PFNGLSHADERBINARYPROC glShaderBinary;
  extern PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
  extern PFNGLDEPTHRANGEFPROC glDepthRangef;
  extern PFNGLCLEARDEPTHFPROC glClearDepthf;

  // GL_ARB_get_program_binary
  extern PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
  extern PFNGLPROGRAMBINARYPROC glProgramBinary;
  extern PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;

  // GL_ARB_separate_shader_objects
  extern PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
  extern PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram;
  extern PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
  extern PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
  extern PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
  extern PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
  extern PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline;
  extern PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv;
  extern PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i;
  extern PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv;
  extern PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f;
  extern PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv;
  extern PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d;
  extern PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv;
  extern PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui;
  extern PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv;
  extern PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i;
  extern PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv;
  extern PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f;
  extern PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv;
  extern PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d;
  extern PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv;
  extern PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui;
  extern PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv;
  extern PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i;
  extern PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv;
  extern PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f;
  extern PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv;
  extern PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d;
  extern PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv;
  extern PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui;
  extern PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv;
  extern PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i;
  extern PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv;
  extern PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f;
  extern PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv;
  extern PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d;
  extern PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv;
  extern PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui;
  extern PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv;
  extern PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv;
  extern PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv;
  extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv;
  extern PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv;
  extern PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv;
  extern PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv;
  extern PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
  extern PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
  extern PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
  extern PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
  extern PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
  extern PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
  extern PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
  extern PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
  extern PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
  extern PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
  extern PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
  extern PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
  extern PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline;
  extern PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;

  // GL_ARB_vertex_attrib_64bit
  extern PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d;
  extern PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d;
  extern PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d;
  extern PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d;
  extern PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv;
  extern PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv;
  extern PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv;
  extern PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv;
  extern PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
  extern PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv;

  // GL_ARB_viewport_array
  extern PFNGLVIEWPORTARRAYVPROC glViewportArrayv;
  extern PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf;
  extern PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
  extern PFNGLSCISSORARRAYVPROC glScissorArrayv;
  extern PFNGLSCISSORINDEXEDPROC glScissorIndexed;
  extern PFNGLSCISSORINDEXEDVPROC glScissorIndexedv;
  extern PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv;
  extern PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
  extern PFNGLGETFLOATI_VPROC  glGetFloati_v;
  extern PFNGLGETDOUBLEI_VPROC  glGetDoublei_v;

  // GL_ARB_cl_event
  extern PFNGLCREATESYNCFROMCLEVENTARBPROC glCreateSyncFromCLeventARB;

  // GL_ARB_debug_output
  extern PFNGLDEBUGMESSAGECONTROLARBPROC glDebugMessageControlARB;
  extern PFNGLDEBUGMESSAGEINSERTARBPROC glDebugMessageInsertARB;
  extern PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB;
  extern PFNGLGETDEBUGMESSAGELOGARBPROC glGetDebugMessageLogARB;

  // GL_ARB_robustness
  extern PFNGLGETGRAPHICSRESETSTATUSARBPROC glGetGraphicsResetStatusARB;
  extern PFNGLGETNMAPDVARBPROC glGetnMapdvARB;
  extern PFNGLGETNMAPFVARBPROC glGetnMapfvARB;
  extern PFNGLGETNMAPIVARBPROC glGetnMapivARB;
  extern PFNGLGETNPIXELMAPFVARBPROC glGetnPixelMapfvARB;
  extern PFNGLGETNPIXELMAPUIVARBPROC glGetnPixelMapuivARB;
  extern PFNGLGETNPIXELMAPUSVARBPROC glGetnPixelMapusvARB;
  extern PFNGLGETNPOLYGONSTIPPLEARBPROC glGetnPolygonStippleARB;
  extern PFNGLGETNCOLORTABLEARBPROC glGetnColorTableARB;
  extern PFNGLGETNCONVOLUTIONFILTERARBPROC glGetnConvolutionFilterARB;
  extern PFNGLGETNSEPARABLEFILTERARBPROC glGetnSeparableFilterARB;
  extern PFNGLGETNHISTOGRAMARBPROC glGetnHistogramARB;
  extern PFNGLGETNMINMAXARBPROC glGetnMinmaxARB;
  extern PFNGLGETNTEXIMAGEARBPROC glGetnTexImageARB;
  extern PFNGLREADNPIXELSARBPROC glReadnPixelsARB;
  extern PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC glGetnCompressedTexImageARB;
  extern PFNGLGETNUNIFORMFVARBPROC glGetnUniformfvARB;
  extern PFNGLGETNUNIFORMIVARBPROC glGetnUniformivARB;
  extern PFNGLGETNUNIFORMUIVARBPROC glGetnUniformuivARB;
  extern PFNGLGETNUNIFORMDVARBPROC glGetnUniformdvARB;

  // GL_EXT_blend_color
  extern PFNGLBLENDCOLOREXTPROC glBlendColorEXT;

  // GL_EXT_polygon_offset
  extern PFNGLPOLYGONOFFSETEXTPROC glPolygonOffsetEXT;

  // GL_EXT_texture3D
  extern PFNGLTEXIMAGE3DEXTPROC glTexImage3DEXT;
  extern PFNGLTEXSUBIMAGE3DEXTPROC glTexSubImage3DEXT;

  // GL_SGIS_texture_filter4
  extern PFNGLGETTEXFILTERFUNCSGISPROC glGetTexFilterFuncSGIS;
  extern PFNGLTEXFILTERFUNCSGISPROC glTexFilterFuncSGIS;

  // GL_EXT_subtexture
  extern PFNGLTEXSUBIMAGE1DEXTPROC glTexSubImage1DEXT;
  extern PFNGLTEXSUBIMAGE2DEXTPROC glTexSubImage2DEXT;

  // GL_EXT_copy_texture
  extern PFNGLCOPYTEXIMAGE1DEXTPROC glCopyTexImage1DEXT;
  extern PFNGLCOPYTEXIMAGE2DEXTPROC glCopyTexImage2DEXT;
  extern PFNGLCOPYTEXSUBIMAGE1DEXTPROC glCopyTexSubImage1DEXT;
  extern PFNGLCOPYTEXSUBIMAGE2DEXTPROC glCopyTexSubImage2DEXT;
  extern PFNGLCOPYTEXSUBIMAGE3DEXTPROC glCopyTexSubImage3DEXT;

  // GL_EXT_histogram
  extern PFNGLGETHISTOGRAMEXTPROC glGetHistogramEXT;
  extern PFNGLGETHISTOGRAMPARAMETERFVEXTPROC glGetHistogramParameterfvEXT;
  extern PFNGLGETHISTOGRAMPARAMETERIVEXTPROC glGetHistogramParameterivEXT;
  extern PFNGLGETMINMAXEXTPROC glGetMinmaxEXT;
  extern PFNGLGETMINMAXPARAMETERFVEXTPROC glGetMinmaxParameterfvEXT;
  extern PFNGLGETMINMAXPARAMETERIVEXTPROC glGetMinmaxParameterivEXT;
  extern PFNGLHISTOGRAMEXTPROC glHistogramEXT;
  extern PFNGLMINMAXEXTPROC glMinmaxEXT;
  extern PFNGLRESETHISTOGRAMEXTPROC glResetHistogramEXT;
  extern PFNGLRESETMINMAXEXTPROC glResetMinmaxEXT;

  // GL_EXT_convolution
  extern PFNGLCONVOLUTIONFILTER1DEXTPROC glConvolutionFilter1DEXT;
  extern PFNGLCONVOLUTIONFILTER2DEXTPROC glConvolutionFilter2DEXT;
  extern PFNGLCONVOLUTIONPARAMETERFEXTPROC glConvolutionParameterfEXT;
  extern PFNGLCONVOLUTIONPARAMETERFVEXTPROC glConvolutionParameterfvEXT;
  extern PFNGLCONVOLUTIONPARAMETERIEXTPROC glConvolutionParameteriEXT;
  extern PFNGLCONVOLUTIONPARAMETERIVEXTPROC glConvolutionParameterivEXT;
  extern PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC glCopyConvolutionFilter1DEXT;
  extern PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC glCopyConvolutionFilter2DEXT;
  extern PFNGLGETCONVOLUTIONFILTEREXTPROC glGetConvolutionFilterEXT;
  extern PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC glGetConvolutionParameterfvEXT;
  extern PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC glGetConvolutionParameterivEXT;
  extern PFNGLGETSEPARABLEFILTEREXTPROC glGetSeparableFilterEXT;
  extern PFNGLSEPARABLEFILTER2DEXTPROC glSeparableFilter2DEXT;

  // GL_SGI_color_table
  extern PFNGLCOLORTABLESGIPROC glColorTableSGI;
  extern PFNGLCOLORTABLEPARAMETERFVSGIPROC glColorTableParameterfvSGI;
  extern PFNGLCOLORTABLEPARAMETERIVSGIPROC glColorTableParameterivSGI;
  extern PFNGLCOPYCOLORTABLESGIPROC glCopyColorTableSGI;
  extern PFNGLGETCOLORTABLESGIPROC glGetColorTableSGI;
  extern PFNGLGETCOLORTABLEPARAMETERFVSGIPROC glGetColorTableParameterfvSGI;
  extern PFNGLGETCOLORTABLEPARAMETERIVSGIPROC glGetColorTableParameterivSGI;

  // GL_SGIX_pixel_texture
  extern PFNGLPIXELTEXGENSGIXPROC glPixelTexGenSGIX;

  // GL_SGIS_pixel_texture
  extern PFNGLPIXELTEXGENPARAMETERISGISPROC glPixelTexGenParameteriSGIS;
  extern PFNGLPIXELTEXGENPARAMETERIVSGISPROC glPixelTexGenParameterivSGIS;
  extern PFNGLPIXELTEXGENPARAMETERFSGISPROC glPixelTexGenParameterfSGIS;
  extern PFNGLPIXELTEXGENPARAMETERFVSGISPROC glPixelTexGenParameterfvSGIS;
  extern PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC glGetPixelTexGenParameterivSGIS;
  extern PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC glGetPixelTexGenParameterfvSGIS;

  // GL_SGIS_texture4D
  extern PFNGLTEXIMAGE4DSGISPROC glTexImage4DSGIS;
  extern PFNGLTEXSUBIMAGE4DSGISPROC glTexSubImage4DSGIS;

  // GL_EXT_texture_object
  extern PFNGLARETEXTURESRESIDENTEXTPROC glAreTexturesResidentEXT;
  extern PFNGLBINDTEXTUREEXTPROC glBindTextureEXT;
  extern PFNGLDELETETEXTURESEXTPROC glDeleteTexturesEXT;
  extern PFNGLGENTEXTURESEXTPROC glGenTexturesEXT;
  extern PFNGLISTEXTUREEXTPROC glIsTextureEXT;
  extern PFNGLPRIORITIZETEXTURESEXTPROC glPrioritizeTexturesEXT;

  // GL_SGIS_detail_texture
  extern PFNGLDETAILTEXFUNCSGISPROC glDetailTexFuncSGIS;
  extern PFNGLGETDETAILTEXFUNCSGISPROC glGetDetailTexFuncSGIS;

  // GL_SGIS_sharpen_texture
  extern PFNGLSHARPENTEXFUNCSGISPROC glSharpenTexFuncSGIS;
  extern PFNGLGETSHARPENTEXFUNCSGISPROC glGetSharpenTexFuncSGIS;

  // GL_SGIS_multisample
  extern PFNGLSAMPLEMASKSGISPROC glSampleMaskSGIS;
  extern PFNGLSAMPLEPATTERNSGISPROC glSamplePatternSGIS;

  // GL_EXT_vertex_array
  extern PFNGLARRAYELEMENTEXTPROC glArrayElementEXT;
  extern PFNGLCOLORPOINTEREXTPROC glColorPointerEXT;
  extern PFNGLDRAWARRAYSEXTPROC glDrawArraysEXT;
  extern PFNGLEDGEFLAGPOINTEREXTPROC glEdgeFlagPointerEXT;
  extern PFNGLGETPOINTERVEXTPROC glGetPointervEXT;
  extern PFNGLINDEXPOINTEREXTPROC glIndexPointerEXT;
  extern PFNGLNORMALPOINTEREXTPROC glNormalPointerEXT;
  extern PFNGLTEXCOORDPOINTEREXTPROC glTexCoordPointerEXT;
  extern PFNGLVERTEXPOINTEREXTPROC glVertexPointerEXT;

  // GL_EXT_blend_minmax
  extern PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT;

  // GL_SGIX_sprite
  extern PFNGLSPRITEPARAMETERFSGIXPROC glSpriteParameterfSGIX;
  extern PFNGLSPRITEPARAMETERFVSGIXPROC glSpriteParameterfvSGIX;
  extern PFNGLSPRITEPARAMETERISGIXPROC glSpriteParameteriSGIX;
  extern PFNGLSPRITEPARAMETERIVSGIXPROC glSpriteParameterivSGIX;

  // GL_EXT_point_parameters
  extern PFNGLPOINTPARAMETERFEXTPROC glPointParameterfEXT;
  extern PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT;

  // GL_SGIS_point_parameters
  extern PFNGLPOINTPARAMETERFVSGISPROC glPointParameterfvSGIS;

  // GL_SGIX_instruments
  extern PFNGLGETINSTRUMENTSSGIXPROC glGetInstrumentsSGIX;
  extern PFNGLINSTRUMENTSBUFFERSGIXPROC glInstrumentsBufferSGIX;
  extern PFNGLPOLLINSTRUMENTSSGIXPROC glPollInstrumentsSGIX;
  extern PFNGLREADINSTRUMENTSSGIXPROC glReadInstrumentsSGIX;
  extern PFNGLSTARTINSTRUMENTSSGIXPROC glStartInstrumentsSGIX;
  extern PFNGLSTOPINSTRUMENTSSGIXPROC glStopInstrumentsSGIX;

  // GL_SGIX_framezoom
  extern PFNGLFRAMEZOOMSGIXPROC glFrameZoomSGIX;

  // GL_SGIX_tag_sample_buffer
  extern PFNGLTAGSAMPLEBUFFERSGIXPROC glTagSampleBufferSGIX;

  // GL_SGIX_polynomial_ffd
  extern PFNGLDEFORMATIONMAP3DSGIXPROC glDeformationMap3dSGIX;
  extern PFNGLDEFORMATIONMAP3FSGIXPROC glDeformationMap3fSGIX;
  extern PFNGLDEFORMSGIXPROC glDeformSGIX;
  extern PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC glLoadIdentityDeformationMapSGIX;

  // GL_SGIX_reference_plane
  extern PFNGLREFERENCEPLANESGIXPROC glReferencePlaneSGIX;

  // GL_SGIX_flush_raster
  extern PFNGLFLUSHRASTERSGIXPROC glFlushRasterSGIX;

  // GL_SGIS_fog_function
  extern PFNGLFOGFUNCSGISPROC glFogFuncSGIS;
  extern PFNGLGETFOGFUNCSGISPROC glGetFogFuncSGIS;

  // GL_HP_image_transform
  extern PFNGLIMAGETRANSFORMPARAMETERIHPPROC glImageTransformParameteriHP;
  extern PFNGLIMAGETRANSFORMPARAMETERFHPPROC glImageTransformParameterfHP;
  extern PFNGLIMAGETRANSFORMPARAMETERIVHPPROC glImageTransformParameterivHP;
  extern PFNGLIMAGETRANSFORMPARAMETERFVHPPROC glImageTransformParameterfvHP;
  extern PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC glGetImageTransformParameterivHP;
  extern PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC glGetImageTransformParameterfvHP;

  // GL_EXT_color_subtable
  extern PFNGLCOLORSUBTABLEEXTPROC glColorSubTableEXT;
  extern PFNGLCOPYCOLORSUBTABLEEXTPROC glCopyColorSubTableEXT;

  // GL_PGI_misc_hints
  extern PFNGLHINTPGIPROC glHintPGI;

  // GL_EXT_paletted_texture
  extern PFNGLCOLORTABLEEXTPROC glColorTableEXT;
  extern PFNGLGETCOLORTABLEEXTPROC glGetColorTableEXT;
  extern PFNGLGETCOLORTABLEPARAMETERIVEXTPROC glGetColorTableParameterivEXT;
  extern PFNGLGETCOLORTABLEPARAMETERFVEXTPROC glGetColorTableParameterfvEXT;

  // GL_SGIX_list_priority
  extern PFNGLGETLISTPARAMETERFVSGIXPROC glGetListParameterfvSGIX;
  extern PFNGLGETLISTPARAMETERIVSGIXPROC glGetListParameterivSGIX;
  extern PFNGLLISTPARAMETERFSGIXPROC glListParameterfSGIX;
  extern PFNGLLISTPARAMETERFVSGIXPROC glListParameterfvSGIX;
  extern PFNGLLISTPARAMETERISGIXPROC glListParameteriSGIX;
  extern PFNGLLISTPARAMETERIVSGIXPROC glListParameterivSGIX;

  // GL_EXT_index_material
  extern PFNGLINDEXMATERIALEXTPROC glIndexMaterialEXT;

  // GL_EXT_index_func
  extern PFNGLINDEXFUNCEXTPROC glIndexFuncEXT;

  // GL_EXT_compiled_vertex_array
  extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
  extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

  // GL_EXT_cull_vertex
  extern PFNGLCULLPARAMETERDVEXTPROC glCullParameterdvEXT;
  extern PFNGLCULLPARAMETERFVEXTPROC glCullParameterfvEXT;

  // GL_SGIX_fragment_lighting
  extern PFNGLFRAGMENTCOLORMATERIALSGIXPROC glFragmentColorMaterialSGIX;
  extern PFNGLFRAGMENTLIGHTFSGIXPROC glFragmentLightfSGIX;
  extern PFNGLFRAGMENTLIGHTFVSGIXPROC glFragmentLightfvSGIX;
  extern PFNGLFRAGMENTLIGHTISGIXPROC glFragmentLightiSGIX;
  extern PFNGLFRAGMENTLIGHTIVSGIXPROC glFragmentLightivSGIX;
  extern PFNGLFRAGMENTLIGHTMODELFSGIXPROC glFragmentLightModelfSGIX;
  extern PFNGLFRAGMENTLIGHTMODELFVSGIXPROC glFragmentLightModelfvSGIX;
  extern PFNGLFRAGMENTLIGHTMODELISGIXPROC glFragmentLightModeliSGIX;
  extern PFNGLFRAGMENTLIGHTMODELIVSGIXPROC glFragmentLightModelivSGIX;
  extern PFNGLFRAGMENTMATERIALFSGIXPROC glFragmentMaterialfSGIX;
  extern PFNGLFRAGMENTMATERIALFVSGIXPROC glFragmentMaterialfvSGIX;
  extern PFNGLFRAGMENTMATERIALISGIXPROC glFragmentMaterialiSGIX;
  extern PFNGLFRAGMENTMATERIALIVSGIXPROC glFragmentMaterialivSGIX;
  extern PFNGLGETFRAGMENTLIGHTFVSGIXPROC glGetFragmentLightfvSGIX;
  extern PFNGLGETFRAGMENTLIGHTIVSGIXPROC glGetFragmentLightivSGIX;
  extern PFNGLGETFRAGMENTMATERIALFVSGIXPROC glGetFragmentMaterialfvSGIX;
  extern PFNGLGETFRAGMENTMATERIALIVSGIXPROC glGetFragmentMaterialivSGIX;
  extern PFNGLLIGHTENVISGIXPROC glLightEnviSGIX;

  // GL_EXT_draw_range_elements
  extern PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;

  // GL_EXT_light_texture
  extern PFNGLAPPLYTEXTUREEXTPROC glApplyTextureEXT;
  extern PFNGLTEXTURELIGHTEXTPROC glTextureLightEXT;
  extern PFNGLTEXTUREMATERIALEXTPROC glTextureMaterialEXT;

  // GL_SGIX_async
  extern PFNGLASYNCMARKERSGIXPROC glAsyncMarkerSGIX;
  extern PFNGLFINISHASYNCSGIXPROC glFinishAsyncSGIX;
  extern PFNGLPOLLASYNCSGIXPROC glPollAsyncSGIX;
  extern PFNGLGENASYNCMARKERSSGIXPROC glGenAsyncMarkersSGIX;
  extern PFNGLDELETEASYNCMARKERSSGIXPROC glDeleteAsyncMarkersSGIX;
  extern PFNGLISASYNCMARKERSGIXPROC glIsAsyncMarkerSGIX;

  // GL_INTEL_parallel_arrays
  extern PFNGLVERTEXPOINTERVINTELPROC glVertexPointervINTEL;
  extern PFNGLNORMALPOINTERVINTELPROC glNormalPointervINTEL;
  extern PFNGLCOLORPOINTERVINTELPROC glColorPointervINTEL;
  extern PFNGLTEXCOORDPOINTERVINTELPROC glTexCoordPointervINTEL;

  // GL_EXT_pixel_transform
  extern PFNGLPIXELTRANSFORMPARAMETERIEXTPROC glPixelTransformParameteriEXT;
  extern PFNGLPIXELTRANSFORMPARAMETERFEXTPROC glPixelTransformParameterfEXT;
  extern PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC glPixelTransformParameterivEXT;
  extern PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC glPixelTransformParameterfvEXT;

  // GL_EXT_secondary_color
  extern PFNGLSECONDARYCOLOR3BEXTPROC glSecondaryColor3bEXT;
  extern PFNGLSECONDARYCOLOR3BVEXTPROC glSecondaryColor3bvEXT;
  extern PFNGLSECONDARYCOLOR3DEXTPROC glSecondaryColor3dEXT;
  extern PFNGLSECONDARYCOLOR3DVEXTPROC glSecondaryColor3dvEXT;
  extern PFNGLSECONDARYCOLOR3FEXTPROC glSecondaryColor3fEXT;
  extern PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT;
  extern PFNGLSECONDARYCOLOR3IEXTPROC glSecondaryColor3iEXT;
  extern PFNGLSECONDARYCOLOR3IVEXTPROC glSecondaryColor3ivEXT;
  extern PFNGLSECONDARYCOLOR3SEXTPROC glSecondaryColor3sEXT;
  extern PFNGLSECONDARYCOLOR3SVEXTPROC glSecondaryColor3svEXT;
  extern PFNGLSECONDARYCOLOR3UBEXTPROC glSecondaryColor3ubEXT;
  extern PFNGLSECONDARYCOLOR3UBVEXTPROC glSecondaryColor3ubvEXT;
  extern PFNGLSECONDARYCOLOR3UIEXTPROC glSecondaryColor3uiEXT;
  extern PFNGLSECONDARYCOLOR3UIVEXTPROC glSecondaryColor3uivEXT;
  extern PFNGLSECONDARYCOLOR3USEXTPROC glSecondaryColor3usEXT;
  extern PFNGLSECONDARYCOLOR3USVEXTPROC glSecondaryColor3usvEXT;
  extern PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT;

  // GL_EXT_texture_perturb_normal
  extern PFNGLTEXTURENORMALEXTPROC glTextureNormalEXT;

  // GL_EXT_multi_draw_arrays
  extern PFNGLMULTIDRAWARRAYSEXTPROC glMultiDrawArraysEXT;
  extern PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;

  // GL_EXT_fog_coord
  extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;
  extern PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT;
  extern PFNGLFOGCOORDDEXTPROC glFogCoorddEXT;
  extern PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT;
  extern PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;

  // GL_EXT_coordinate_frame
  extern PFNGLTANGENT3BEXTPROC glTangent3bEXT;
  extern PFNGLTANGENT3BVEXTPROC glTangent3bvEXT;
  extern PFNGLTANGENT3DEXTPROC glTangent3dEXT;
  extern PFNGLTANGENT3DVEXTPROC glTangent3dvEXT;
  extern PFNGLTANGENT3FEXTPROC glTangent3fEXT;
  extern PFNGLTANGENT3FVEXTPROC glTangent3fvEXT;
  extern PFNGLTANGENT3IEXTPROC glTangent3iEXT;
  extern PFNGLTANGENT3IVEXTPROC glTangent3ivEXT;
  extern PFNGLTANGENT3SEXTPROC glTangent3sEXT;
  extern PFNGLTANGENT3SVEXTPROC glTangent3svEXT;
  extern PFNGLBINORMAL3BEXTPROC glBinormal3bEXT;
  extern PFNGLBINORMAL3BVEXTPROC glBinormal3bvEXT;
  extern PFNGLBINORMAL3DEXTPROC glBinormal3dEXT;
  extern PFNGLBINORMAL3DVEXTPROC glBinormal3dvEXT;
  extern PFNGLBINORMAL3FEXTPROC glBinormal3fEXT;
  extern PFNGLBINORMAL3FVEXTPROC glBinormal3fvEXT;
  extern PFNGLBINORMAL3IEXTPROC glBinormal3iEXT;
  extern PFNGLBINORMAL3IVEXTPROC glBinormal3ivEXT;
  extern PFNGLBINORMAL3SEXTPROC glBinormal3sEXT;
  extern PFNGLBINORMAL3SVEXTPROC glBinormal3svEXT;
  extern PFNGLTANGENTPOINTEREXTPROC glTangentPointerEXT;
  extern PFNGLBINORMALPOINTEREXTPROC glBinormalPointerEXT;

  // GL_SUNX_constant_data
  extern PFNGLFINISHTEXTURESUNXPROC glFinishTextureSUNX;

  // GL_SUN_global_alpha
  extern PFNGLGLOBALALPHAFACTORBSUNPROC glGlobalAlphaFactorbSUN;
  extern PFNGLGLOBALALPHAFACTORSSUNPROC glGlobalAlphaFactorsSUN;
  extern PFNGLGLOBALALPHAFACTORISUNPROC glGlobalAlphaFactoriSUN;
  extern PFNGLGLOBALALPHAFACTORFSUNPROC glGlobalAlphaFactorfSUN;
  extern PFNGLGLOBALALPHAFACTORDSUNPROC glGlobalAlphaFactordSUN;
  extern PFNGLGLOBALALPHAFACTORUBSUNPROC glGlobalAlphaFactorubSUN;
  extern PFNGLGLOBALALPHAFACTORUSSUNPROC glGlobalAlphaFactorusSUN;
  extern PFNGLGLOBALALPHAFACTORUISUNPROC glGlobalAlphaFactoruiSUN;

  // GL_SUN_triangle_list
  extern PFNGLREPLACEMENTCODEUISUNPROC glReplacementCodeuiSUN;
  extern PFNGLREPLACEMENTCODEUSSUNPROC glReplacementCodeusSUN;
  extern PFNGLREPLACEMENTCODEUBSUNPROC glReplacementCodeubSUN;
  extern PFNGLREPLACEMENTCODEUIVSUNPROC glReplacementCodeuivSUN;
  extern PFNGLREPLACEMENTCODEUSVSUNPROC glReplacementCodeusvSUN;
  extern PFNGLREPLACEMENTCODEUBVSUNPROC glReplacementCodeubvSUN;
  extern PFNGLREPLACEMENTCODEPOINTERSUNPROC glReplacementCodePointerSUN;

  // GL_SUN_vertex
  extern PFNGLCOLOR4UBVERTEX2FSUNPROC glColor4ubVertex2fSUN;
  extern PFNGLCOLOR4UBVERTEX2FVSUNPROC glColor4ubVertex2fvSUN;
  extern PFNGLCOLOR4UBVERTEX3FSUNPROC glColor4ubVertex3fSUN;
  extern PFNGLCOLOR4UBVERTEX3FVSUNPROC glColor4ubVertex3fvSUN;
  extern PFNGLCOLOR3FVERTEX3FSUNPROC glColor3fVertex3fSUN;
  extern PFNGLCOLOR3FVERTEX3FVSUNPROC glColor3fVertex3fvSUN;
  extern PFNGLNORMAL3FVERTEX3FSUNPROC glNormal3fVertex3fSUN;
  extern PFNGLNORMAL3FVERTEX3FVSUNPROC glNormal3fVertex3fvSUN;
  extern PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC glColor4fNormal3fVertex3fSUN;
  extern PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC glColor4fNormal3fVertex3fvSUN;
  extern PFNGLTEXCOORD2FVERTEX3FSUNPROC glTexCoord2fVertex3fSUN;
  extern PFNGLTEXCOORD2FVERTEX3FVSUNPROC glTexCoord2fVertex3fvSUN;
  extern PFNGLTEXCOORD4FVERTEX4FSUNPROC glTexCoord4fVertex4fSUN;
  extern PFNGLTEXCOORD4FVERTEX4FVSUNPROC glTexCoord4fVertex4fvSUN;
  extern PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC glTexCoord2fColor4ubVertex3fSUN;
  extern PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC glTexCoord2fColor4ubVertex3fvSUN;
  extern PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC glTexCoord2fColor3fVertex3fSUN;
  extern PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC glTexCoord2fColor3fVertex3fvSUN;
  extern PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC glTexCoord2fNormal3fVertex3fSUN;
  extern PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC glTexCoord2fNormal3fVertex3fvSUN;
  extern PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC glTexCoord2fColor4fNormal3fVertex3fSUN;
  extern PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC glTexCoord2fColor4fNormal3fVertex3fvSUN;
  extern PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC glTexCoord4fColor4fNormal3fVertex4fSUN;
  extern PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC glTexCoord4fColor4fNormal3fVertex4fvSUN;
  extern PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC glReplacementCodeuiVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC glReplacementCodeuiVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC glReplacementCodeuiColor4ubVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC glReplacementCodeuiColor4ubVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC glReplacementCodeuiColor3fVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC glReplacementCodeuiColor3fVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC glReplacementCodeuiNormal3fVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiNormal3fVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC glReplacementCodeuiColor4fNormal3fVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiColor4fNormal3fVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC glReplacementCodeuiTexCoord2fVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN;
  extern PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN;
  extern PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN;

  // GL_EXT_blend_func_separate
  extern PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT;

  // GL_INGR_blend_func_separate
  extern PFNGLBLENDFUNCSEPARATEINGRPROC glBlendFuncSeparateINGR;

  // GL_EXT_vertex_weighting
  extern PFNGLVERTEXWEIGHTFEXTPROC glVertexWeightfEXT;
  extern PFNGLVERTEXWEIGHTFVEXTPROC glVertexWeightfvEXT;
  extern PFNGLVERTEXWEIGHTPOINTEREXTPROC glVertexWeightPointerEXT;

  // GL_NV_vertex_array_range
  extern PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
  extern PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;

  // GL_NV_register_combiners
  extern PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
  extern PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
  extern PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
  extern PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;
  extern PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;
  extern PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;
  extern PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;
  extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
  extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
  extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
  extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
  extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
  extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;

  // GL_MESA_resize_buffers
  extern PFNGLRESIZEBUFFERSMESAPROC glResizeBuffersMESA;

  // GL_MESA_window_pos
  extern PFNGLWINDOWPOS2DMESAPROC glWindowPos2dMESA;
  extern PFNGLWINDOWPOS2DVMESAPROC glWindowPos2dvMESA;
  extern PFNGLWINDOWPOS2FMESAPROC glWindowPos2fMESA;
  extern PFNGLWINDOWPOS2FVMESAPROC glWindowPos2fvMESA;
  extern PFNGLWINDOWPOS2IMESAPROC glWindowPos2iMESA;
  extern PFNGLWINDOWPOS2IVMESAPROC glWindowPos2ivMESA;
  extern PFNGLWINDOWPOS2SMESAPROC glWindowPos2sMESA;
  extern PFNGLWINDOWPOS2SVMESAPROC glWindowPos2svMESA;
  extern PFNGLWINDOWPOS3DMESAPROC glWindowPos3dMESA;
  extern PFNGLWINDOWPOS3DVMESAPROC glWindowPos3dvMESA;
  extern PFNGLWINDOWPOS3FMESAPROC glWindowPos3fMESA;
  extern PFNGLWINDOWPOS3FVMESAPROC glWindowPos3fvMESA;
  extern PFNGLWINDOWPOS3IMESAPROC glWindowPos3iMESA;
  extern PFNGLWINDOWPOS3IVMESAPROC glWindowPos3ivMESA;
  extern PFNGLWINDOWPOS3SMESAPROC glWindowPos3sMESA;
  extern PFNGLWINDOWPOS3SVMESAPROC glWindowPos3svMESA;
  extern PFNGLWINDOWPOS4DMESAPROC glWindowPos4dMESA;
  extern PFNGLWINDOWPOS4DVMESAPROC glWindowPos4dvMESA;
  extern PFNGLWINDOWPOS4FMESAPROC glWindowPos4fMESA;
  extern PFNGLWINDOWPOS4FVMESAPROC glWindowPos4fvMESA;
  extern PFNGLWINDOWPOS4IMESAPROC glWindowPos4iMESA;
  extern PFNGLWINDOWPOS4IVMESAPROC glWindowPos4ivMESA;
  extern PFNGLWINDOWPOS4SMESAPROC glWindowPos4sMESA;
  extern PFNGLWINDOWPOS4SVMESAPROC glWindowPos4svMESA;

  // GL_IBM_multimode_draw_arrays
  extern PFNGLMULTIMODEDRAWARRAYSIBMPROC glMultiModeDrawArraysIBM;
  extern PFNGLMULTIMODEDRAWELEMENTSIBMPROC glMultiModeDrawElementsIBM;

  // GL_IBM_vertex_array_lists
  extern PFNGLCOLORPOINTERLISTIBMPROC glColorPointerListIBM;
  extern PFNGLSECONDARYCOLORPOINTERLISTIBMPROC glSecondaryColorPointerListIBM;
  extern PFNGLEDGEFLAGPOINTERLISTIBMPROC glEdgeFlagPointerListIBM;
  extern PFNGLFOGCOORDPOINTERLISTIBMPROC glFogCoordPointerListIBM;
  extern PFNGLINDEXPOINTERLISTIBMPROC glIndexPointerListIBM;
  extern PFNGLNORMALPOINTERLISTIBMPROC glNormalPointerListIBM;
  extern PFNGLTEXCOORDPOINTERLISTIBMPROC glTexCoordPointerListIBM;
  extern PFNGLVERTEXPOINTERLISTIBMPROC glVertexPointerListIBM;

  // GL_3DFX_tbuffer
  extern PFNGLTBUFFERMASK3DFXPROC glTbufferMask3DFX;

  // GL_EXT_multisample
  extern PFNGLSAMPLEMASKEXTPROC glSampleMaskEXT;
  extern PFNGLSAMPLEPATTERNEXTPROC glSamplePatternEXT;

  // GL_SGIS_texture_color_mask
  extern PFNGLTEXTURECOLORMASKSGISPROC glTextureColorMaskSGIS;

  // GL_SGIX_igloo_interface
  extern PFNGLIGLOOINTERFACESGIXPROC glIglooInterfaceSGIX;

  // GL_NV_fence
  extern PFNGLDELETEFENCESNVPROC glDeleteFencesNV;
  extern PFNGLGENFENCESNVPROC glGenFencesNV;
  extern PFNGLISFENCENVPROC glIsFenceNV;
  extern PFNGLTESTFENCENVPROC glTestFenceNV;
  extern PFNGLGETFENCEIVNVPROC glGetFenceivNV;
  extern PFNGLFINISHFENCENVPROC glFinishFenceNV;
  extern PFNGLSETFENCENVPROC glSetFenceNV;

  // GL_NV_evaluators
  extern PFNGLMAPCONTROLPOINTSNVPROC glMapControlPointsNV;
  extern PFNGLMAPPARAMETERIVNVPROC glMapParameterivNV;
  extern PFNGLMAPPARAMETERFVNVPROC glMapParameterfvNV;
  extern PFNGLGETMAPCONTROLPOINTSNVPROC glGetMapControlPointsNV;
  extern PFNGLGETMAPPARAMETERIVNVPROC glGetMapParameterivNV;
  extern PFNGLGETMAPPARAMETERFVNVPROC glGetMapParameterfvNV;
  extern PFNGLGETMAPATTRIBPARAMETERIVNVPROC glGetMapAttribParameterivNV;
  extern PFNGLGETMAPATTRIBPARAMETERFVNVPROC glGetMapAttribParameterfvNV;
  extern PFNGLEVALMAPSNVPROC glEvalMapsNV;

  // GL_NV_register_combiners2
  extern PFNGLCOMBINERSTAGEPARAMETERFVNVPROC glCombinerStageParameterfvNV;
  extern PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC glGetCombinerStageParameterfvNV;

  // GL_NV_vertex_program
  extern PFNGLAREPROGRAMSRESIDENTNVPROC glAreProgramsResidentNV;
  extern PFNGLBINDPROGRAMNVPROC glBindProgramNV;
  extern PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV;
  extern PFNGLEXECUTEPROGRAMNVPROC glExecuteProgramNV;
  extern PFNGLGENPROGRAMSNVPROC glGenProgramsNV;
  extern PFNGLGETPROGRAMPARAMETERDVNVPROC glGetProgramParameterdvNV;
  extern PFNGLGETPROGRAMPARAMETERFVNVPROC glGetProgramParameterfvNV;
  extern PFNGLGETPROGRAMIVNVPROC glGetProgramivNV;
  extern PFNGLGETPROGRAMSTRINGNVPROC glGetProgramStringNV;
  extern PFNGLGETTRACKMATRIXIVNVPROC glGetTrackMatrixivNV;
  extern PFNGLGETVERTEXATTRIBDVNVPROC glGetVertexAttribdvNV;
  extern PFNGLGETVERTEXATTRIBFVNVPROC glGetVertexAttribfvNV;
  extern PFNGLGETVERTEXATTRIBIVNVPROC glGetVertexAttribivNV;
  extern PFNGLGETVERTEXATTRIBPOINTERVNVPROC glGetVertexAttribPointervNV;
  extern PFNGLISPROGRAMNVPROC glIsProgramNV;
  extern PFNGLLOADPROGRAMNVPROC glLoadProgramNV;
  extern PFNGLPROGRAMPARAMETER4DNVPROC glProgramParameter4dNV;
  extern PFNGLPROGRAMPARAMETER4DVNVPROC glProgramParameter4dvNV;
  extern PFNGLPROGRAMPARAMETER4FNVPROC glProgramParameter4fNV;
  extern PFNGLPROGRAMPARAMETER4FVNVPROC glProgramParameter4fvNV;
  extern PFNGLPROGRAMPARAMETERS4DVNVPROC glProgramParameters4dvNV;
  extern PFNGLPROGRAMPARAMETERS4FVNVPROC glProgramParameters4fvNV;
  extern PFNGLREQUESTRESIDENTPROGRAMSNVPROC glRequestResidentProgramsNV;
  extern PFNGLTRACKMATRIXNVPROC glTrackMatrixNV;
  extern PFNGLVERTEXATTRIBPOINTERNVPROC glVertexAttribPointerNV;
  extern PFNGLVERTEXATTRIB1DNVPROC glVertexAttrib1dNV;
  extern PFNGLVERTEXATTRIB1DVNVPROC glVertexAttrib1dvNV;
  extern PFNGLVERTEXATTRIB1FNVPROC glVertexAttrib1fNV;
  extern PFNGLVERTEXATTRIB1FVNVPROC glVertexAttrib1fvNV;
  extern PFNGLVERTEXATTRIB1SNVPROC glVertexAttrib1sNV;
  extern PFNGLVERTEXATTRIB1SVNVPROC glVertexAttrib1svNV;
  extern PFNGLVERTEXATTRIB2DNVPROC glVertexAttrib2dNV;
  extern PFNGLVERTEXATTRIB2DVNVPROC glVertexAttrib2dvNV;
  extern PFNGLVERTEXATTRIB2FNVPROC glVertexAttrib2fNV;
  extern PFNGLVERTEXATTRIB2FVNVPROC glVertexAttrib2fvNV;
  extern PFNGLVERTEXATTRIB2SNVPROC glVertexAttrib2sNV;
  extern PFNGLVERTEXATTRIB2SVNVPROC glVertexAttrib2svNV;
  extern PFNGLVERTEXATTRIB3DNVPROC glVertexAttrib3dNV;
  extern PFNGLVERTEXATTRIB3DVNVPROC glVertexAttrib3dvNV;
  extern PFNGLVERTEXATTRIB3FNVPROC glVertexAttrib3fNV;
  extern PFNGLVERTEXATTRIB3FVNVPROC glVertexAttrib3fvNV;
  extern PFNGLVERTEXATTRIB3SNVPROC glVertexAttrib3sNV;
  extern PFNGLVERTEXATTRIB3SVNVPROC glVertexAttrib3svNV;
  extern PFNGLVERTEXATTRIB4DNVPROC glVertexAttrib4dNV;
  extern PFNGLVERTEXATTRIB4DVNVPROC glVertexAttrib4dvNV;
  extern PFNGLVERTEXATTRIB4FNVPROC glVertexAttrib4fNV;
  extern PFNGLVERTEXATTRIB4FVNVPROC glVertexAttrib4fvNV;
  extern PFNGLVERTEXATTRIB4SNVPROC glVertexAttrib4sNV;
  extern PFNGLVERTEXATTRIB4SVNVPROC glVertexAttrib4svNV;
  extern PFNGLVERTEXATTRIB4UBNVPROC glVertexAttrib4ubNV;
  extern PFNGLVERTEXATTRIB4UBVNVPROC glVertexAttrib4ubvNV;
  extern PFNGLVERTEXATTRIBS1DVNVPROC glVertexAttribs1dvNV;
  extern PFNGLVERTEXATTRIBS1FVNVPROC glVertexAttribs1fvNV;
  extern PFNGLVERTEXATTRIBS1SVNVPROC glVertexAttribs1svNV;
  extern PFNGLVERTEXATTRIBS2DVNVPROC glVertexAttribs2dvNV;
  extern PFNGLVERTEXATTRIBS2FVNVPROC glVertexAttribs2fvNV;
  extern PFNGLVERTEXATTRIBS2SVNVPROC glVertexAttribs2svNV;
  extern PFNGLVERTEXATTRIBS3DVNVPROC glVertexAttribs3dvNV;
  extern PFNGLVERTEXATTRIBS3FVNVPROC glVertexAttribs3fvNV;
  extern PFNGLVERTEXATTRIBS3SVNVPROC glVertexAttribs3svNV;
  extern PFNGLVERTEXATTRIBS4DVNVPROC glVertexAttribs4dvNV;
  extern PFNGLVERTEXATTRIBS4FVNVPROC glVertexAttribs4fvNV;
  extern PFNGLVERTEXATTRIBS4SVNVPROC glVertexAttribs4svNV;
  extern PFNGLVERTEXATTRIBS4UBVNVPROC glVertexAttribs4ubvNV;

  // GL_ATI_envmap_bumpmap
  extern PFNGLTEXBUMPPARAMETERIVATIPROC glTexBumpParameterivATI;
  extern PFNGLTEXBUMPPARAMETERFVATIPROC glTexBumpParameterfvATI;
  extern PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI;
  extern PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI;

  // GL_ATI_fragment_shader
  extern PFNGLGENFRAGMENTSHADERSATIPROC glGenFragmentShadersATI;
  extern PFNGLBINDFRAGMENTSHADERATIPROC glBindFragmentShaderATI;
  extern PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI;
  extern PFNGLBEGINFRAGMENTSHADERATIPROC glBeginFragmentShaderATI;
  extern PFNGLENDFRAGMENTSHADERATIPROC glEndFragmentShaderATI;
  extern PFNGLPASSTEXCOORDATIPROC glPassTexCoordATI;
  extern PFNGLSAMPLEMAPATIPROC glSampleMapATI;
  extern PFNGLCOLORFRAGMENTOP1ATIPROC glColorFragmentOp1ATI;
  extern PFNGLCOLORFRAGMENTOP2ATIPROC glColorFragmentOp2ATI;
  extern PFNGLCOLORFRAGMENTOP3ATIPROC glColorFragmentOp3ATI;
  extern PFNGLALPHAFRAGMENTOP1ATIPROC glAlphaFragmentOp1ATI;
  extern PFNGLALPHAFRAGMENTOP2ATIPROC glAlphaFragmentOp2ATI;
  extern PFNGLALPHAFRAGMENTOP3ATIPROC glAlphaFragmentOp3ATI;
  extern PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI;

  // GL_ATI_pn_triangles
  extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
  extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;

  // GL_ATI_vertex_array_object
  extern PFNGLNEWOBJECTBUFFERATIPROC glNewObjectBufferATI;
  extern PFNGLISOBJECTBUFFERATIPROC glIsObjectBufferATI;
  extern PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI;
  extern PFNGLGETOBJECTBUFFERFVATIPROC glGetObjectBufferfvATI;
  extern PFNGLGETOBJECTBUFFERIVATIPROC glGetObjectBufferivATI;
  extern PFNGLFREEOBJECTBUFFERATIPROC glFreeObjectBufferATI;
  extern PFNGLARRAYOBJECTATIPROC glArrayObjectATI;
  extern PFNGLGETARRAYOBJECTFVATIPROC glGetArrayObjectfvATI;
  extern PFNGLGETARRAYOBJECTIVATIPROC glGetArrayObjectivATI;
  extern PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI;
  extern PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI;
  extern PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI;

  // GL_EXT_vertex_shader
  extern PFNGLBEGINVERTEXSHADEREXTPROC glBeginVertexShaderEXT;
  extern PFNGLENDVERTEXSHADEREXTPROC glEndVertexShaderEXT;
  extern PFNGLBINDVERTEXSHADEREXTPROC glBindVertexShaderEXT;
  extern PFNGLGENVERTEXSHADERSEXTPROC glGenVertexShadersEXT;
  extern PFNGLDELETEVERTEXSHADEREXTPROC glDeleteVertexShaderEXT;
  extern PFNGLSHADEROP1EXTPROC glShaderOp1EXT;
  extern PFNGLSHADEROP2EXTPROC glShaderOp2EXT;
  extern PFNGLSHADEROP3EXTPROC glShaderOp3EXT;
  extern PFNGLSWIZZLEEXTPROC glSwizzleEXT;
  extern PFNGLWRITEMASKEXTPROC glWriteMaskEXT;
  extern PFNGLINSERTCOMPONENTEXTPROC glInsertComponentEXT;
  extern PFNGLEXTRACTCOMPONENTEXTPROC glExtractComponentEXT;
  extern PFNGLGENSYMBOLSEXTPROC glGenSymbolsEXT;
  extern PFNGLSETINVARIANTEXTPROC glSetInvariantEXT;
  extern PFNGLSETLOCALCONSTANTEXTPROC glSetLocalConstantEXT;
  extern PFNGLVARIANTBVEXTPROC glVariantbvEXT;
  extern PFNGLVARIANTSVEXTPROC glVariantsvEXT;
  extern PFNGLVARIANTIVEXTPROC glVariantivEXT;
  extern PFNGLVARIANTFVEXTPROC glVariantfvEXT;
  extern PFNGLVARIANTDVEXTPROC glVariantdvEXT;
  extern PFNGLVARIANTUBVEXTPROC glVariantubvEXT;
  extern PFNGLVARIANTUSVEXTPROC glVariantusvEXT;
  extern PFNGLVARIANTUIVEXTPROC glVariantuivEXT;
  extern PFNGLVARIANTPOINTEREXTPROC glVariantPointerEXT;
  extern PFNGLENABLEVARIANTCLIENTSTATEEXTPROC glEnableVariantClientStateEXT;
  extern PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT;
  extern PFNGLBINDLIGHTPARAMETEREXTPROC glBindLightParameterEXT;
  extern PFNGLBINDMATERIALPARAMETEREXTPROC glBindMaterialParameterEXT;
  extern PFNGLBINDTEXGENPARAMETEREXTPROC glBindTexGenParameterEXT;
  extern PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT;
  extern PFNGLBINDPARAMETEREXTPROC glBindParameterEXT;
  extern PFNGLISVARIANTENABLEDEXTPROC glIsVariantEnabledEXT;
  extern PFNGLGETVARIANTBOOLEANVEXTPROC glGetVariantBooleanvEXT;
  extern PFNGLGETVARIANTINTEGERVEXTPROC glGetVariantIntegervEXT;
  extern PFNGLGETVARIANTFLOATVEXTPROC glGetVariantFloatvEXT;
  extern PFNGLGETVARIANTPOINTERVEXTPROC glGetVariantPointervEXT;
  extern PFNGLGETINVARIANTBOOLEANVEXTPROC glGetInvariantBooleanvEXT;
  extern PFNGLGETINVARIANTINTEGERVEXTPROC glGetInvariantIntegervEXT;
  extern PFNGLGETINVARIANTFLOATVEXTPROC glGetInvariantFloatvEXT;
  extern PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT;
  extern PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT;
  extern PFNGLGETLOCALCONSTANTFLOATVEXTPROC glGetLocalConstantFloatvEXT;

  // GL_ATI_vertex_streams
  extern PFNGLVERTEXSTREAM1SATIPROC glVertexStream1sATI;
  extern PFNGLVERTEXSTREAM1SVATIPROC glVertexStream1svATI;
  extern PFNGLVERTEXSTREAM1IATIPROC glVertexStream1iATI;
  extern PFNGLVERTEXSTREAM1IVATIPROC glVertexStream1ivATI;
  extern PFNGLVERTEXSTREAM1FATIPROC glVertexStream1fATI;
  extern PFNGLVERTEXSTREAM1FVATIPROC glVertexStream1fvATI;
  extern PFNGLVERTEXSTREAM1DATIPROC glVertexStream1dATI;
  extern PFNGLVERTEXSTREAM1DVATIPROC glVertexStream1dvATI;
  extern PFNGLVERTEXSTREAM2SATIPROC glVertexStream2sATI;
  extern PFNGLVERTEXSTREAM2SVATIPROC glVertexStream2svATI;
  extern PFNGLVERTEXSTREAM2IATIPROC glVertexStream2iATI;
  extern PFNGLVERTEXSTREAM2IVATIPROC glVertexStream2ivATI;
  extern PFNGLVERTEXSTREAM2FATIPROC glVertexStream2fATI;
  extern PFNGLVERTEXSTREAM2FVATIPROC glVertexStream2fvATI;
  extern PFNGLVERTEXSTREAM2DATIPROC glVertexStream2dATI;
  extern PFNGLVERTEXSTREAM2DVATIPROC glVertexStream2dvATI;
  extern PFNGLVERTEXSTREAM3SATIPROC glVertexStream3sATI;
  extern PFNGLVERTEXSTREAM3SVATIPROC glVertexStream3svATI;
  extern PFNGLVERTEXSTREAM3IATIPROC glVertexStream3iATI;
  extern PFNGLVERTEXSTREAM3IVATIPROC glVertexStream3ivATI;
  extern PFNGLVERTEXSTREAM3FATIPROC glVertexStream3fATI;
  extern PFNGLVERTEXSTREAM3FVATIPROC glVertexStream3fvATI;
  extern PFNGLVERTEXSTREAM3DATIPROC glVertexStream3dATI;
  extern PFNGLVERTEXSTREAM3DVATIPROC glVertexStream3dvATI;
  extern PFNGLVERTEXSTREAM4SATIPROC glVertexStream4sATI;
  extern PFNGLVERTEXSTREAM4SVATIPROC glVertexStream4svATI;
  extern PFNGLVERTEXSTREAM4IATIPROC glVertexStream4iATI;
  extern PFNGLVERTEXSTREAM4IVATIPROC glVertexStream4ivATI;
  extern PFNGLVERTEXSTREAM4FATIPROC glVertexStream4fATI;
  extern PFNGLVERTEXSTREAM4FVATIPROC glVertexStream4fvATI;
  extern PFNGLVERTEXSTREAM4DATIPROC glVertexStream4dATI;
  extern PFNGLVERTEXSTREAM4DVATIPROC glVertexStream4dvATI;
  extern PFNGLNORMALSTREAM3BATIPROC glNormalStream3bATI;
  extern PFNGLNORMALSTREAM3BVATIPROC glNormalStream3bvATI;
  extern PFNGLNORMALSTREAM3SATIPROC glNormalStream3sATI;
  extern PFNGLNORMALSTREAM3SVATIPROC glNormalStream3svATI;
  extern PFNGLNORMALSTREAM3IATIPROC glNormalStream3iATI;
  extern PFNGLNORMALSTREAM3IVATIPROC glNormalStream3ivATI;
  extern PFNGLNORMALSTREAM3FATIPROC glNormalStream3fATI;
  extern PFNGLNORMALSTREAM3FVATIPROC glNormalStream3fvATI;
  extern PFNGLNORMALSTREAM3DATIPROC glNormalStream3dATI;
  extern PFNGLNORMALSTREAM3DVATIPROC glNormalStream3dvATI;
  extern PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC glClientActiveVertexStreamATI;
  extern PFNGLVERTEXBLENDENVIATIPROC glVertexBlendEnviATI;
  extern PFNGLVERTEXBLENDENVFATIPROC glVertexBlendEnvfATI;

  // GL_ATI_element_array
  extern PFNGLELEMENTPOINTERATIPROC glElementPointerATI;
  extern PFNGLDRAWELEMENTARRAYATIPROC glDrawElementArrayATI;
  extern PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI;

  // GL_SUN_mesh_array
  extern PFNGLDRAWMESHARRAYSSUNPROC glDrawMeshArraysSUN;

  // GL_NV_occlusion_query
  extern PFNGLGENOCCLUSIONQUERIESNVPROC glGenOcclusionQueriesNV;
  extern PFNGLDELETEOCCLUSIONQUERIESNVPROC glDeleteOcclusionQueriesNV;
  extern PFNGLISOCCLUSIONQUERYNVPROC glIsOcclusionQueryNV;
  extern PFNGLBEGINOCCLUSIONQUERYNVPROC glBeginOcclusionQueryNV;
  extern PFNGLENDOCCLUSIONQUERYNVPROC glEndOcclusionQueryNV;
  extern PFNGLGETOCCLUSIONQUERYIVNVPROC glGetOcclusionQueryivNV;
  extern PFNGLGETOCCLUSIONQUERYUIVNVPROC glGetOcclusionQueryuivNV;

  // GL_NV_point_sprite
  extern PFNGLPOINTPARAMETERINVPROC glPointParameteriNV;
  extern PFNGLPOINTPARAMETERIVNVPROC glPointParameterivNV;

  // GL_EXT_stencil_two_side
  extern PFNGLACTIVESTENCILFACEEXTPROC glActiveStencilFaceEXT;

  // GL_APPLE_element_array
  extern PFNGLELEMENTPOINTERAPPLEPROC glElementPointerAPPLE;
  extern PFNGLDRAWELEMENTARRAYAPPLEPROC glDrawElementArrayAPPLE;
  extern PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC glDrawRangeElementArrayAPPLE;
  extern PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC glMultiDrawElementArrayAPPLE;
  extern PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC glMultiDrawRangeElementArrayAPPLE;

  // GL_APPLE_fence
  extern PFNGLGENFENCESAPPLEPROC glGenFencesAPPLE;
  extern PFNGLDELETEFENCESAPPLEPROC glDeleteFencesAPPLE;
  extern PFNGLSETFENCEAPPLEPROC glSetFenceAPPLE;
  extern PFNGLISFENCEAPPLEPROC glIsFenceAPPLE;
  extern PFNGLTESTFENCEAPPLEPROC glTestFenceAPPLE;
  extern PFNGLFINISHFENCEAPPLEPROC glFinishFenceAPPLE;
  extern PFNGLTESTOBJECTAPPLEPROC glTestObjectAPPLE;
  extern PFNGLFINISHOBJECTAPPLEPROC glFinishObjectAPPLE;

  // GL_APPLE_vertex_array_object
  extern PFNGLBINDVERTEXARRAYAPPLEPROC glBindVertexArrayAPPLE;
  extern PFNGLDELETEVERTEXARRAYSAPPLEPROC glDeleteVertexArraysAPPLE;
  extern PFNGLGENVERTEXARRAYSAPPLEPROC glGenVertexArraysAPPLE;
  extern PFNGLISVERTEXARRAYAPPLEPROC glIsVertexArrayAPPLE;

  // GL_APPLE_vertex_array_range
  extern PFNGLVERTEXARRAYRANGEAPPLEPROC glVertexArrayRangeAPPLE;
  extern PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC glFlushVertexArrayRangeAPPLE;
  extern PFNGLVERTEXARRAYPARAMETERIAPPLEPROC glVertexArrayParameteriAPPLE;

  // GL_ATI_draw_buffers
  extern PFNGLDRAWBUFFERSATIPROC glDrawBuffersATI;

  // GL_NV_fragment_program
  /* Some NV_fragment_program entry points are shared with ARB_vertex_program. */
  extern PFNGLPROGRAMNAMEDPARAMETER4FNVPROC glProgramNamedParameter4fNV;
  extern PFNGLPROGRAMNAMEDPARAMETER4DNVPROC glProgramNamedParameter4dNV;
  extern PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC glProgramNamedParameter4fvNV;
  extern PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC glProgramNamedParameter4dvNV;
  extern PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC glGetProgramNamedParameterfvNV;
  extern PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC glGetProgramNamedParameterdvNV;

  // GL_NV_half_float
  extern PFNGLVERTEX2HNVPROC glVertex2hNV;
  extern PFNGLVERTEX2HVNVPROC glVertex2hvNV;
  extern PFNGLVERTEX3HNVPROC glVertex3hNV;
  extern PFNGLVERTEX3HVNVPROC glVertex3hvNV;
  extern PFNGLVERTEX4HNVPROC glVertex4hNV;
  extern PFNGLVERTEX4HVNVPROC glVertex4hvNV;
  extern PFNGLNORMAL3HNVPROC glNormal3hNV;
  extern PFNGLNORMAL3HVNVPROC glNormal3hvNV;
  extern PFNGLCOLOR3HNVPROC glColor3hNV;
  extern PFNGLCOLOR3HVNVPROC glColor3hvNV;
  extern PFNGLCOLOR4HNVPROC glColor4hNV;
  extern PFNGLCOLOR4HVNVPROC glColor4hvNV;
  extern PFNGLTEXCOORD1HNVPROC glTexCoord1hNV;
  extern PFNGLTEXCOORD1HVNVPROC glTexCoord1hvNV;
  extern PFNGLTEXCOORD2HNVPROC glTexCoord2hNV;
  extern PFNGLTEXCOORD2HVNVPROC glTexCoord2hvNV;
  extern PFNGLTEXCOORD3HNVPROC glTexCoord3hNV;
  extern PFNGLTEXCOORD3HVNVPROC glTexCoord3hvNV;
  extern PFNGLTEXCOORD4HNVPROC glTexCoord4hNV;
  extern PFNGLTEXCOORD4HVNVPROC glTexCoord4hvNV;
  extern PFNGLMULTITEXCOORD1HNVPROC glMultiTexCoord1hNV;
  extern PFNGLMULTITEXCOORD1HVNVPROC glMultiTexCoord1hvNV;
  extern PFNGLMULTITEXCOORD2HNVPROC glMultiTexCoord2hNV;
  extern PFNGLMULTITEXCOORD2HVNVPROC glMultiTexCoord2hvNV;
  extern PFNGLMULTITEXCOORD3HNVPROC glMultiTexCoord3hNV;
  extern PFNGLMULTITEXCOORD3HVNVPROC glMultiTexCoord3hvNV;
  extern PFNGLMULTITEXCOORD4HNVPROC glMultiTexCoord4hNV;
  extern PFNGLMULTITEXCOORD4HVNVPROC glMultiTexCoord4hvNV;
  extern PFNGLFOGCOORDHNVPROC glFogCoordhNV;
  extern PFNGLFOGCOORDHVNVPROC glFogCoordhvNV;
  extern PFNGLSECONDARYCOLOR3HNVPROC glSecondaryColor3hNV;
  extern PFNGLSECONDARYCOLOR3HVNVPROC glSecondaryColor3hvNV;
  extern PFNGLVERTEXWEIGHTHNVPROC glVertexWeighthNV;
  extern PFNGLVERTEXWEIGHTHVNVPROC glVertexWeighthvNV;
  extern PFNGLVERTEXATTRIB1HNVPROC glVertexAttrib1hNV;
  extern PFNGLVERTEXATTRIB1HVNVPROC glVertexAttrib1hvNV;
  extern PFNGLVERTEXATTRIB2HNVPROC glVertexAttrib2hNV;
  extern PFNGLVERTEXATTRIB2HVNVPROC glVertexAttrib2hvNV;
  extern PFNGLVERTEXATTRIB3HNVPROC glVertexAttrib3hNV;
  extern PFNGLVERTEXATTRIB3HVNVPROC glVertexAttrib3hvNV;
  extern PFNGLVERTEXATTRIB4HNVPROC glVertexAttrib4hNV;
  extern PFNGLVERTEXATTRIB4HVNVPROC glVertexAttrib4hvNV;
  extern PFNGLVERTEXATTRIBS1HVNVPROC glVertexAttribs1hvNV;
  extern PFNGLVERTEXATTRIBS2HVNVPROC glVertexAttribs2hvNV;
  extern PFNGLVERTEXATTRIBS3HVNVPROC glVertexAttribs3hvNV;
  extern PFNGLVERTEXATTRIBS4HVNVPROC glVertexAttribs4hvNV;

  // GL_NV_pixel_data_range
  extern PFNGLPIXELDATARANGENVPROC glPixelDataRangeNV;
  extern PFNGLFLUSHPIXELDATARANGENVPROC glFlushPixelDataRangeNV;

  // GL_NV_primitive_restart
  extern PFNGLPRIMITIVERESTARTNVPROC glPrimitiveRestartNV;
  extern PFNGLPRIMITIVERESTARTINDEXNVPROC glPrimitiveRestartIndexNV;

  // GL_ATI_map_object_buffer
  extern PFNGLMAPOBJECTBUFFERATIPROC glMapObjectBufferATI;
  extern PFNGLUNMAPOBJECTBUFFERATIPROC glUnmapObjectBufferATI;

  // GL_ATI_separate_stencil
  extern PFNGLSTENCILOPSEPARATEATIPROC glStencilOpSeparateATI;
  extern PFNGLSTENCILFUNCSEPARATEATIPROC glStencilFuncSeparateATI;

  // GL_ATI_vertex_attrib_array_object
  extern PFNGLVERTEXATTRIBARRAYOBJECTATIPROC glVertexAttribArrayObjectATI;
  extern PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC glGetVertexAttribArrayObjectfvATI;
  extern PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC glGetVertexAttribArrayObjectivATI;

  // GL_EXT_depth_bounds_test
  extern PFNGLDEPTHBOUNDSEXTPROC glDepthBoundsEXT;

  // GL_EXT_blend_equation_separate
  extern PFNGLBLENDEQUATIONSEPARATEEXTPROC glBlendEquationSeparateEXT;

  // GL_EXT_framebuffer_object
  extern PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT;
  extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
  extern PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
  extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
  extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
  extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT;
  extern PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT;
  extern PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
  extern PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;
  extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
  extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
  extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT;
  extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
  extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT;
  extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
  extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
  extern PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT;

  // GL_GREMEDY_string_marker
  extern PFNGLSTRINGMARKERGREMEDYPROC glStringMarkerGREMEDY;

  // GL_EXT_stencil_clear_tag
  extern PFNGLSTENCILCLEARTAGEXTPROC glStencilClearTagEXT;

  // GL_EXT_framebuffer_blit
  extern PFNGLBLITFRAMEBUFFEREXTPROC glBlitFramebufferEXT;

  // GL_EXT_framebuffer_multisample
  extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT;

  // GL_EXT_timer_query
  extern PFNGLGETQUERYOBJECTI64VEXTPROC glGetQueryObjecti64vEXT;
  extern PFNGLGETQUERYOBJECTUI64VEXTPROC glGetQueryObjectui64vEXT;

  // GL_EXT_gpu_program_parameters
  extern PFNGLPROGRAMENVPARAMETERS4FVEXTPROC glProgramEnvParameters4fvEXT;
  extern PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC glProgramLocalParameters4fvEXT;

  // GL_APPLE_flush_buffer_range
  extern PFNGLBUFFERPARAMETERIAPPLEPROC glBufferParameteriAPPLE;
  extern PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC glFlushMappedBufferRangeAPPLE;

  // GL_NV_gpu_program4
  extern PFNGLPROGRAMLOCALPARAMETERI4INVPROC glProgramLocalParameterI4iNV;
  extern PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC glProgramLocalParameterI4ivNV;
  extern PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC glProgramLocalParametersI4ivNV;
  extern PFNGLPROGRAMLOCALPARAMETERI4UINVPROC glProgramLocalParameterI4uiNV;
  extern PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC glProgramLocalParameterI4uivNV;
  extern PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC glProgramLocalParametersI4uivNV;
  extern PFNGLPROGRAMENVPARAMETERI4INVPROC glProgramEnvParameterI4iNV;
  extern PFNGLPROGRAMENVPARAMETERI4IVNVPROC glProgramEnvParameterI4ivNV;
  extern PFNGLPROGRAMENVPARAMETERSI4IVNVPROC glProgramEnvParametersI4ivNV;
  extern PFNGLPROGRAMENVPARAMETERI4UINVPROC glProgramEnvParameterI4uiNV;
  extern PFNGLPROGRAMENVPARAMETERI4UIVNVPROC glProgramEnvParameterI4uivNV;
  extern PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC glProgramEnvParametersI4uivNV;
  extern PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC glGetProgramLocalParameterIivNV;
  extern PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC glGetProgramLocalParameterIuivNV;
  extern PFNGLGETPROGRAMENVPARAMETERIIVNVPROC glGetProgramEnvParameterIivNV;
  extern PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC glGetProgramEnvParameterIuivNV;

  // GL_NV_geometry_program4
  extern PFNGLPROGRAMVERTEXLIMITNVPROC glProgramVertexLimitNV;
  extern PFNGLFRAMEBUFFERTEXTUREEXTPROC glFramebufferTextureEXT;
  extern PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC glFramebufferTextureLayerEXT;
  extern PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC glFramebufferTextureFaceEXT;

  // GL_EXT_geometry_shader4
  extern PFNGLPROGRAMPARAMETERIEXTPROC glProgramParameteriEXT;

  // GL_NV_vertex_program4
  extern PFNGLVERTEXATTRIBI1IEXTPROC glVertexAttribI1iEXT;
  extern PFNGLVERTEXATTRIBI2IEXTPROC glVertexAttribI2iEXT;
  extern PFNGLVERTEXATTRIBI3IEXTPROC glVertexAttribI3iEXT;
  extern PFNGLVERTEXATTRIBI4IEXTPROC glVertexAttribI4iEXT;
  extern PFNGLVERTEXATTRIBI1UIEXTPROC glVertexAttribI1uiEXT;
  extern PFNGLVERTEXATTRIBI2UIEXTPROC glVertexAttribI2uiEXT;
  extern PFNGLVERTEXATTRIBI3UIEXTPROC glVertexAttribI3uiEXT;
  extern PFNGLVERTEXATTRIBI4UIEXTPROC glVertexAttribI4uiEXT;
  extern PFNGLVERTEXATTRIBI1IVEXTPROC glVertexAttribI1ivEXT;
  extern PFNGLVERTEXATTRIBI2IVEXTPROC glVertexAttribI2ivEXT;
  extern PFNGLVERTEXATTRIBI3IVEXTPROC glVertexAttribI3ivEXT;
  extern PFNGLVERTEXATTRIBI4IVEXTPROC glVertexAttribI4ivEXT;
  extern PFNGLVERTEXATTRIBI1UIVEXTPROC glVertexAttribI1uivEXT;
  extern PFNGLVERTEXATTRIBI2UIVEXTPROC glVertexAttribI2uivEXT;
  extern PFNGLVERTEXATTRIBI3UIVEXTPROC glVertexAttribI3uivEXT;
  extern PFNGLVERTEXATTRIBI4UIVEXTPROC glVertexAttribI4uivEXT;
  extern PFNGLVERTEXATTRIBI4BVEXTPROC glVertexAttribI4bvEXT;
  extern PFNGLVERTEXATTRIBI4SVEXTPROC glVertexAttribI4svEXT;
  extern PFNGLVERTEXATTRIBI4UBVEXTPROC glVertexAttribI4ubvEXT;
  extern PFNGLVERTEXATTRIBI4USVEXTPROC glVertexAttribI4usvEXT;
  extern PFNGLVERTEXATTRIBIPOINTEREXTPROC glVertexAttribIPointerEXT;
  extern PFNGLGETVERTEXATTRIBIIVEXTPROC glGetVertexAttribIivEXT;
  extern PFNGLGETVERTEXATTRIBIUIVEXTPROC glGetVertexAttribIuivEXT;

  // GL_EXT_gpu_shader4
  extern PFNGLGETUNIFORMUIVEXTPROC glGetUniformuivEXT;
  extern PFNGLBINDFRAGDATALOCATIONEXTPROC glBindFragDataLocationEXT;
  extern PFNGLGETFRAGDATALOCATIONEXTPROC glGetFragDataLocationEXT;
  extern PFNGLUNIFORM1UIEXTPROC glUniform1uiEXT;
  extern PFNGLUNIFORM2UIEXTPROC glUniform2uiEXT;
  extern PFNGLUNIFORM3UIEXTPROC glUniform3uiEXT;
  extern PFNGLUNIFORM4UIEXTPROC glUniform4uiEXT;
  extern PFNGLUNIFORM1UIVEXTPROC glUniform1uivEXT;
  extern PFNGLUNIFORM2UIVEXTPROC glUniform2uivEXT;
  extern PFNGLUNIFORM3UIVEXTPROC glUniform3uivEXT;
  extern PFNGLUNIFORM4UIVEXTPROC glUniform4uivEXT;

  // GL_EXT_draw_instanced
  extern PFNGLDRAWARRAYSINSTANCEDEXTPROC glDrawArraysInstancedEXT;
  extern PFNGLDRAWELEMENTSINSTANCEDEXTPROC glDrawElementsInstancedEXT;

  // GL_EXT_texture_buffer_object
  extern PFNGLTEXBUFFEREXTPROC glTexBufferEXT;

  // GL_NV_depth_buffer_float
  extern PFNGLDEPTHRANGEDNVPROC glDepthRangedNV;
  extern PFNGLCLEARDEPTHDNVPROC glClearDepthdNV;
  extern PFNGLDEPTHBOUNDSDNVPROC glDepthBoundsdNV;

  // GL_NV_framebuffer_multisample_coverage
  extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC glRenderbufferStorageMultisampleCoverageNV;

  // GL_NV_parameter_buffer_object
  extern PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC glProgramBufferParametersfvNV;
  extern PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC glProgramBufferParametersIivNV;
  extern PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC glProgramBufferParametersIuivNV;

  // GL_EXT_draw_buffers2
  extern PFNGLCOLORMASKINDEXEDEXTPROC glColorMaskIndexedEXT;
  extern PFNGLGETBOOLEANINDEXEDVEXTPROC glGetBooleanIndexedvEXT;
  extern PFNGLGETINTEGERINDEXEDVEXTPROC glGetIntegerIndexedvEXT;
  extern PFNGLENABLEINDEXEDEXTPROC glEnableIndexedEXT;
  extern PFNGLDISABLEINDEXEDEXTPROC glDisableIndexedEXT;
  extern PFNGLISENABLEDINDEXEDEXTPROC glIsEnabledIndexedEXT;

  // GL_NV_transform_feedback
  extern PFNGLBEGINTRANSFORMFEEDBACKNVPROC glBeginTransformFeedbackNV;
  extern PFNGLENDTRANSFORMFEEDBACKNVPROC glEndTransformFeedbackNV;
  extern PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC glTransformFeedbackAttribsNV;
  extern PFNGLBINDBUFFERRANGENVPROC glBindBufferRangeNV;
  extern PFNGLBINDBUFFEROFFSETNVPROC glBindBufferOffsetNV;
  extern PFNGLBINDBUFFERBASENVPROC glBindBufferBaseNV;
  extern PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC glTransformFeedbackVaryingsNV;
  extern PFNGLACTIVEVARYINGNVPROC glActiveVaryingNV;
  extern PFNGLGETVARYINGLOCATIONNVPROC glGetVaryingLocationNV;
  extern PFNGLGETACTIVEVARYINGNVPROC glGetActiveVaryingNV;
  extern PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC glGetTransformFeedbackVaryingNV;
  extern PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC glTransformFeedbackStreamAttribsNV;

  // GL_EXT_bindable_uniform
  extern PFNGLUNIFORMBUFFEREXTPROC glUniformBufferEXT;
  extern PFNGLGETUNIFORMBUFFERSIZEEXTPROC glGetUniformBufferSizeEXT;
  extern PFNGLGETUNIFORMOFFSETEXTPROC glGetUniformOffsetEXT;

  // GL_EXT_texture_integer
  extern PFNGLTEXPARAMETERIIVEXTPROC glTexParameterIivEXT;
  extern PFNGLTEXPARAMETERIUIVEXTPROC glTexParameterIuivEXT;
  extern PFNGLGETTEXPARAMETERIIVEXTPROC glGetTexParameterIivEXT;
  extern PFNGLGETTEXPARAMETERIUIVEXTPROC glGetTexParameterIuivEXT;
  extern PFNGLCLEARCOLORIIEXTPROC glClearColorIiEXT;
  extern PFNGLCLEARCOLORIUIEXTPROC glClearColorIuiEXT;

  // GL_GREMEDY_frame_terminator
  extern PFNGLFRAMETERMINATORGREMEDYPROC glFrameTerminatorGREMEDY;

  // GL_NV_conditional_render
  extern PFNGLBEGINCONDITIONALRENDERNVPROC glBeginConditionalRenderNV;
  extern PFNGLENDCONDITIONALRENDERNVPROC glEndConditionalRenderNV;

  // GL_NV_present_video
  extern PFNGLPRESENTFRAMEKEYEDNVPROC glPresentFrameKeyedNV;
  extern PFNGLPRESENTFRAMEDUALFILLNVPROC glPresentFrameDualFillNV;
  extern PFNGLGETVIDEOIVNVPROC glGetVideoivNV;
  extern PFNGLGETVIDEOUIVNVPROC glGetVideouivNV;
  extern PFNGLGETVIDEOI64VNVPROC glGetVideoi64vNV;
  extern PFNGLGETVIDEOUI64VNVPROC glGetVideoui64vNV;

  // GL_EXT_transform_feedback
  extern PFNGLBEGINTRANSFORMFEEDBACKEXTPROC glBeginTransformFeedbackEXT;
  extern PFNGLENDTRANSFORMFEEDBACKEXTPROC glEndTransformFeedbackEXT;
  extern PFNGLBINDBUFFERRANGEEXTPROC glBindBufferRangeEXT;
  extern PFNGLBINDBUFFEROFFSETEXTPROC glBindBufferOffsetEXT;
  extern PFNGLBINDBUFFERBASEEXTPROC glBindBufferBaseEXT;
  extern PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC glTransformFeedbackVaryingsEXT;
  extern PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC glGetTransformFeedbackVaryingEXT;

  // GL_EXT_direct_state_access
  extern PFNGLCLIENTATTRIBDEFAULTEXTPROC glClientAttribDefaultEXT;
  extern PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC glPushClientAttribDefaultEXT;
  extern PFNGLMATRIXLOADFEXTPROC glMatrixLoadfEXT;
  extern PFNGLMATRIXLOADDEXTPROC glMatrixLoaddEXT;
  extern PFNGLMATRIXMULTFEXTPROC glMatrixMultfEXT;
  extern PFNGLMATRIXMULTDEXTPROC glMatrixMultdEXT;
  extern PFNGLMATRIXLOADIDENTITYEXTPROC glMatrixLoadIdentityEXT;
  extern PFNGLMATRIXROTATEFEXTPROC glMatrixRotatefEXT;
  extern PFNGLMATRIXROTATEDEXTPROC glMatrixRotatedEXT;
  extern PFNGLMATRIXSCALEFEXTPROC glMatrixScalefEXT;
  extern PFNGLMATRIXSCALEDEXTPROC glMatrixScaledEXT;
  extern PFNGLMATRIXTRANSLATEFEXTPROC glMatrixTranslatefEXT;
  extern PFNGLMATRIXTRANSLATEDEXTPROC glMatrixTranslatedEXT;
  extern PFNGLMATRIXFRUSTUMEXTPROC glMatrixFrustumEXT;
  extern PFNGLMATRIXORTHOEXTPROC glMatrixOrthoEXT;
  extern PFNGLMATRIXPOPEXTPROC glMatrixPopEXT;
  extern PFNGLMATRIXPUSHEXTPROC glMatrixPushEXT;
  extern PFNGLMATRIXLOADTRANSPOSEFEXTPROC glMatrixLoadTransposefEXT;
  extern PFNGLMATRIXLOADTRANSPOSEDEXTPROC glMatrixLoadTransposedEXT;
  extern PFNGLMATRIXMULTTRANSPOSEFEXTPROC glMatrixMultTransposefEXT;
  extern PFNGLMATRIXMULTTRANSPOSEDEXTPROC glMatrixMultTransposedEXT;
  extern PFNGLTEXTUREPARAMETERFEXTPROC glTextureParameterfEXT;
  extern PFNGLTEXTUREPARAMETERFVEXTPROC glTextureParameterfvEXT;
  extern PFNGLTEXTUREPARAMETERIEXTPROC glTextureParameteriEXT;
  extern PFNGLTEXTUREPARAMETERIVEXTPROC glTextureParameterivEXT;
  extern PFNGLTEXTUREIMAGE1DEXTPROC glTextureImage1DEXT;
  extern PFNGLTEXTUREIMAGE2DEXTPROC glTextureImage2DEXT;
  extern PFNGLTEXTURESUBIMAGE1DEXTPROC glTextureSubImage1DEXT;
  extern PFNGLTEXTURESUBIMAGE2DEXTPROC glTextureSubImage2DEXT;
  extern PFNGLCOPYTEXTUREIMAGE1DEXTPROC glCopyTextureImage1DEXT;
  extern PFNGLCOPYTEXTUREIMAGE2DEXTPROC glCopyTextureImage2DEXT;
  extern PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC glCopyTextureSubImage1DEXT;
  extern PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC glCopyTextureSubImage2DEXT;
  extern PFNGLGETTEXTUREIMAGEEXTPROC glGetTextureImageEXT;
  extern PFNGLGETTEXTUREPARAMETERFVEXTPROC glGetTextureParameterfvEXT;
  extern PFNGLGETTEXTUREPARAMETERIVEXTPROC glGetTextureParameterivEXT;
  extern PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC glGetTextureLevelParameterfvEXT;
  extern PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC glGetTextureLevelParameterivEXT;
  extern PFNGLTEXTUREIMAGE3DEXTPROC glTextureImage3DEXT;
  extern PFNGLTEXTURESUBIMAGE3DEXTPROC glTextureSubImage3DEXT;
  extern PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC glCopyTextureSubImage3DEXT;
  extern PFNGLMULTITEXPARAMETERFEXTPROC glMultiTexParameterfEXT;
  extern PFNGLMULTITEXPARAMETERFVEXTPROC glMultiTexParameterfvEXT;
  extern PFNGLMULTITEXPARAMETERIEXTPROC glMultiTexParameteriEXT;
  extern PFNGLMULTITEXPARAMETERIVEXTPROC glMultiTexParameterivEXT;
  extern PFNGLMULTITEXIMAGE1DEXTPROC glMultiTexImage1DEXT;
  extern PFNGLMULTITEXIMAGE2DEXTPROC glMultiTexImage2DEXT;
  extern PFNGLMULTITEXSUBIMAGE1DEXTPROC glMultiTexSubImage1DEXT;
  extern PFNGLMULTITEXSUBIMAGE2DEXTPROC glMultiTexSubImage2DEXT;
  extern PFNGLCOPYMULTITEXIMAGE1DEXTPROC glCopyMultiTexImage1DEXT;
  extern PFNGLCOPYMULTITEXIMAGE2DEXTPROC glCopyMultiTexImage2DEXT;
  extern PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC glCopyMultiTexSubImage1DEXT;
  extern PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC glCopyMultiTexSubImage2DEXT;
  extern PFNGLGETMULTITEXIMAGEEXTPROC glGetMultiTexImageEXT;
  extern PFNGLGETMULTITEXPARAMETERFVEXTPROC glGetMultiTexParameterfvEXT;
  extern PFNGLGETMULTITEXPARAMETERIVEXTPROC glGetMultiTexParameterivEXT;
  extern PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC glGetMultiTexLevelParameterfvEXT;
  extern PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC glGetMultiTexLevelParameterivEXT;
  extern PFNGLMULTITEXIMAGE3DEXTPROC glMultiTexImage3DEXT;
  extern PFNGLMULTITEXSUBIMAGE3DEXTPROC glMultiTexSubImage3DEXT;
  extern PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC glCopyMultiTexSubImage3DEXT;
  extern PFNGLBINDMULTITEXTUREEXTPROC glBindMultiTextureEXT;
  extern PFNGLENABLECLIENTSTATEINDEXEDEXTPROC glEnableClientStateIndexedEXT;
  extern PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC glDisableClientStateIndexedEXT;
  extern PFNGLMULTITEXCOORDPOINTEREXTPROC glMultiTexCoordPointerEXT;
  extern PFNGLMULTITEXENVFEXTPROC glMultiTexEnvfEXT;
  extern PFNGLMULTITEXENVFVEXTPROC glMultiTexEnvfvEXT;
  extern PFNGLMULTITEXENVIEXTPROC glMultiTexEnviEXT;
  extern PFNGLMULTITEXENVIVEXTPROC glMultiTexEnvivEXT;
  extern PFNGLMULTITEXGENDEXTPROC glMultiTexGendEXT;
  extern PFNGLMULTITEXGENDVEXTPROC glMultiTexGendvEXT;
  extern PFNGLMULTITEXGENFEXTPROC glMultiTexGenfEXT;
  extern PFNGLMULTITEXGENFVEXTPROC glMultiTexGenfvEXT;
  extern PFNGLMULTITEXGENIEXTPROC glMultiTexGeniEXT;
  extern PFNGLMULTITEXGENIVEXTPROC glMultiTexGenivEXT;
  extern PFNGLGETMULTITEXENVFVEXTPROC glGetMultiTexEnvfvEXT;
  extern PFNGLGETMULTITEXENVIVEXTPROC glGetMultiTexEnvivEXT;
  extern PFNGLGETMULTITEXGENDVEXTPROC glGetMultiTexGendvEXT;
  extern PFNGLGETMULTITEXGENFVEXTPROC glGetMultiTexGenfvEXT;
  extern PFNGLGETMULTITEXGENIVEXTPROC glGetMultiTexGenivEXT;
  extern PFNGLGETFLOATINDEXEDVEXTPROC glGetFloatIndexedvEXT;
  extern PFNGLGETDOUBLEINDEXEDVEXTPROC glGetDoubleIndexedvEXT;
  extern PFNGLGETPOINTERINDEXEDVEXTPROC glGetPointerIndexedvEXT;
  extern PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC glCompressedTextureImage3DEXT;
  extern PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC glCompressedTextureImage2DEXT;
  extern PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC glCompressedTextureImage1DEXT;
  extern PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC glCompressedTextureSubImage3DEXT;
  extern PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC glCompressedTextureSubImage2DEXT;
  extern PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC glCompressedTextureSubImage1DEXT;
  extern PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC glGetCompressedTextureImageEXT;
  extern PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC glCompressedMultiTexImage3DEXT;
  extern PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC glCompressedMultiTexImage2DEXT;
  extern PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC glCompressedMultiTexImage1DEXT;
  extern PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC glCompressedMultiTexSubImage3DEXT;
  extern PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC glCompressedMultiTexSubImage2DEXT;
  extern PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC glCompressedMultiTexSubImage1DEXT;
  extern PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC glGetCompressedMultiTexImageEXT;
  extern PFNGLNAMEDPROGRAMSTRINGEXTPROC glNamedProgramStringEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC glNamedProgramLocalParameter4dEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC glNamedProgramLocalParameter4dvEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC glNamedProgramLocalParameter4fEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC glNamedProgramLocalParameter4fvEXT;
  extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC glGetNamedProgramLocalParameterdvEXT;
  extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC glGetNamedProgramLocalParameterfvEXT;
  extern PFNGLGETNAMEDPROGRAMIVEXTPROC glGetNamedProgramivEXT;
  extern PFNGLGETNAMEDPROGRAMSTRINGEXTPROC glGetNamedProgramStringEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC glNamedProgramLocalParameters4fvEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC glNamedProgramLocalParameterI4iEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC glNamedProgramLocalParameterI4ivEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC glNamedProgramLocalParametersI4ivEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC glNamedProgramLocalParameterI4uiEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC glNamedProgramLocalParameterI4uivEXT;
  extern PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC glNamedProgramLocalParametersI4uivEXT;
  extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC glGetNamedProgramLocalParameterIivEXT;
  extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC glGetNamedProgramLocalParameterIuivEXT;
  extern PFNGLTEXTUREPARAMETERIIVEXTPROC glTextureParameterIivEXT;
  extern PFNGLTEXTUREPARAMETERIUIVEXTPROC glTextureParameterIuivEXT;
  extern PFNGLGETTEXTUREPARAMETERIIVEXTPROC glGetTextureParameterIivEXT;
  extern PFNGLGETTEXTUREPARAMETERIUIVEXTPROC glGetTextureParameterIuivEXT;
  extern PFNGLMULTITEXPARAMETERIIVEXTPROC glMultiTexParameterIivEXT;
  extern PFNGLMULTITEXPARAMETERIUIVEXTPROC glMultiTexParameterIuivEXT;
  extern PFNGLGETMULTITEXPARAMETERIIVEXTPROC glGetMultiTexParameterIivEXT;
  extern PFNGLGETMULTITEXPARAMETERIUIVEXTPROC glGetMultiTexParameterIuivEXT;
  extern PFNGLPROGRAMUNIFORM1FEXTPROC glProgramUniform1fEXT;
  extern PFNGLPROGRAMUNIFORM2FEXTPROC glProgramUniform2fEXT;
  extern PFNGLPROGRAMUNIFORM3FEXTPROC glProgramUniform3fEXT;
  extern PFNGLPROGRAMUNIFORM4FEXTPROC glProgramUniform4fEXT;
  extern PFNGLPROGRAMUNIFORM1IEXTPROC glProgramUniform1iEXT;
  extern PFNGLPROGRAMUNIFORM2IEXTPROC glProgramUniform2iEXT;
  extern PFNGLPROGRAMUNIFORM3IEXTPROC glProgramUniform3iEXT;
  extern PFNGLPROGRAMUNIFORM4IEXTPROC glProgramUniform4iEXT;
  extern PFNGLPROGRAMUNIFORM1FVEXTPROC glProgramUniform1fvEXT;
  extern PFNGLPROGRAMUNIFORM2FVEXTPROC glProgramUniform2fvEXT;
  extern PFNGLPROGRAMUNIFORM3FVEXTPROC glProgramUniform3fvEXT;
  extern PFNGLPROGRAMUNIFORM4FVEXTPROC glProgramUniform4fvEXT;
  extern PFNGLPROGRAMUNIFORM1IVEXTPROC glProgramUniform1ivEXT;
  extern PFNGLPROGRAMUNIFORM2IVEXTPROC glProgramUniform2ivEXT;
  extern PFNGLPROGRAMUNIFORM3IVEXTPROC glProgramUniform3ivEXT;
  extern PFNGLPROGRAMUNIFORM4IVEXTPROC glProgramUniform4ivEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC glProgramUniformMatrix2fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC glProgramUniformMatrix3fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC glProgramUniformMatrix4fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC glProgramUniformMatrix2x3fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC glProgramUniformMatrix3x2fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC glProgramUniformMatrix2x4fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC glProgramUniformMatrix4x2fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC glProgramUniformMatrix3x4fvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC glProgramUniformMatrix4x3fvEXT;
  extern PFNGLPROGRAMUNIFORM1UIEXTPROC glProgramUniform1uiEXT;
  extern PFNGLPROGRAMUNIFORM2UIEXTPROC glProgramUniform2uiEXT;
  extern PFNGLPROGRAMUNIFORM3UIEXTPROC glProgramUniform3uiEXT;
  extern PFNGLPROGRAMUNIFORM4UIEXTPROC glProgramUniform4uiEXT;
  extern PFNGLPROGRAMUNIFORM1UIVEXTPROC glProgramUniform1uivEXT;
  extern PFNGLPROGRAMUNIFORM2UIVEXTPROC glProgramUniform2uivEXT;
  extern PFNGLPROGRAMUNIFORM3UIVEXTPROC glProgramUniform3uivEXT;
  extern PFNGLPROGRAMUNIFORM4UIVEXTPROC glProgramUniform4uivEXT;
  extern PFNGLNAMEDBUFFERDATAEXTPROC glNamedBufferDataEXT;
  extern PFNGLNAMEDBUFFERSUBDATAEXTPROC glNamedBufferSubDataEXT;
  extern PFNGLMAPNAMEDBUFFEREXTPROC glMapNamedBufferEXT;
  extern PFNGLUNMAPNAMEDBUFFEREXTPROC glUnmapNamedBufferEXT;
  extern PFNGLMAPNAMEDBUFFERRANGEEXTPROC glMapNamedBufferRangeEXT;
  extern PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC glFlushMappedNamedBufferRangeEXT;
  extern PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC glNamedCopyBufferSubDataEXT;
  extern PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC glGetNamedBufferParameterivEXT;
  extern PFNGLGETNAMEDBUFFERPOINTERVEXTPROC glGetNamedBufferPointervEXT;
  extern PFNGLGETNAMEDBUFFERSUBDATAEXTPROC glGetNamedBufferSubDataEXT;
  extern PFNGLTEXTUREBUFFEREXTPROC glTextureBufferEXT;
  extern PFNGLMULTITEXBUFFEREXTPROC glMultiTexBufferEXT;
  extern PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC glNamedRenderbufferStorageEXT;
  extern PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC glGetNamedRenderbufferParameterivEXT;
  extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC glCheckNamedFramebufferStatusEXT;
  extern PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC glNamedFramebufferTexture1DEXT;
  extern PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC glNamedFramebufferTexture2DEXT;
  extern PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC glNamedFramebufferTexture3DEXT;
  extern PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC glNamedFramebufferRenderbufferEXT;
  extern PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetNamedFramebufferAttachmentParameterivEXT;
  extern PFNGLGENERATETEXTUREMIPMAPEXTPROC glGenerateTextureMipmapEXT;
  extern PFNGLGENERATEMULTITEXMIPMAPEXTPROC glGenerateMultiTexMipmapEXT;
  extern PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC glFramebufferDrawBufferEXT;
  extern PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC glFramebufferDrawBuffersEXT;
  extern PFNGLFRAMEBUFFERREADBUFFEREXTPROC glFramebufferReadBufferEXT;
  extern PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC glGetFramebufferParameterivEXT;
  extern PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glNamedRenderbufferStorageMultisampleEXT;
  extern PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC glNamedRenderbufferStorageMultisampleCoverageEXT;
  extern PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC glNamedFramebufferTextureEXT;
  extern PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC glNamedFramebufferTextureLayerEXT;
  extern PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC glNamedFramebufferTextureFaceEXT;
  extern PFNGLTEXTURERENDERBUFFEREXTPROC glTextureRenderbufferEXT;
  extern PFNGLMULTITEXRENDERBUFFEREXTPROC glMultiTexRenderbufferEXT;
  extern PFNGLPROGRAMUNIFORM1DEXTPROC glProgramUniform1dEXT;
  extern PFNGLPROGRAMUNIFORM2DEXTPROC glProgramUniform2dEXT;
  extern PFNGLPROGRAMUNIFORM3DEXTPROC glProgramUniform3dEXT;
  extern PFNGLPROGRAMUNIFORM4DEXTPROC glProgramUniform4dEXT;
  extern PFNGLPROGRAMUNIFORM1DVEXTPROC glProgramUniform1dvEXT;
  extern PFNGLPROGRAMUNIFORM2DVEXTPROC glProgramUniform2dvEXT;
  extern PFNGLPROGRAMUNIFORM3DVEXTPROC glProgramUniform3dvEXT;
  extern PFNGLPROGRAMUNIFORM4DVEXTPROC glProgramUniform4dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC glProgramUniformMatrix2dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC glProgramUniformMatrix3dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC glProgramUniformMatrix4dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC glProgramUniformMatrix2x3dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC glProgramUniformMatrix2x4dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC glProgramUniformMatrix3x2dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC glProgramUniformMatrix3x4dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC glProgramUniformMatrix4x2dvEXT;
  extern PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC glProgramUniformMatrix4x3dvEXT;

  // GL_NV_explicit_multisample
  extern PFNGLGETMULTISAMPLEFVNVPROC glGetMultisamplefvNV;
  extern PFNGLSAMPLEMASKINDEXEDNVPROC glSampleMaskIndexedNV;
  extern PFNGLTEXRENDERBUFFERNVPROC glTexRenderbufferNV;

  // GL_NV_transform_feedback2
  extern PFNGLBINDTRANSFORMFEEDBACKNVPROC glBindTransformFeedbackNV;
  extern PFNGLDELETETRANSFORMFEEDBACKSNVPROC glDeleteTransformFeedbacksNV;
  extern PFNGLGENTRANSFORMFEEDBACKSNVPROC glGenTransformFeedbacksNV;
  extern PFNGLISTRANSFORMFEEDBACKNVPROC glIsTransformFeedbackNV;
  extern PFNGLPAUSETRANSFORMFEEDBACKNVPROC glPauseTransformFeedbackNV;
  extern PFNGLRESUMETRANSFORMFEEDBACKNVPROC glResumeTransformFeedbackNV;
  extern PFNGLDRAWTRANSFORMFEEDBACKNVPROC glDrawTransformFeedbackNV;

  // GL_AMD_performance_monitor
  extern PFNGLGETPERFMONITORGROUPSAMDPROC glGetPerfMonitorGroupsAMD;
  extern PFNGLGETPERFMONITORCOUNTERSAMDPROC glGetPerfMonitorCountersAMD;
  extern PFNGLGETPERFMONITORGROUPSTRINGAMDPROC glGetPerfMonitorGroupStringAMD;
  extern PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC glGetPerfMonitorCounterStringAMD;
  extern PFNGLGETPERFMONITORCOUNTERINFOAMDPROC glGetPerfMonitorCounterInfoAMD;
  extern PFNGLGENPERFMONITORSAMDPROC glGenPerfMonitorsAMD;
  extern PFNGLDELETEPERFMONITORSAMDPROC glDeletePerfMonitorsAMD;
  extern PFNGLSELECTPERFMONITORCOUNTERSAMDPROC glSelectPerfMonitorCountersAMD;
  extern PFNGLBEGINPERFMONITORAMDPROC glBeginPerfMonitorAMD;
  extern PFNGLENDPERFMONITORAMDPROC glEndPerfMonitorAMD;
  extern PFNGLGETPERFMONITORCOUNTERDATAAMDPROC glGetPerfMonitorCounterDataAMD;

  // GL_AMD_vertex_shader_tesselator
  extern PFNGLTESSELLATIONFACTORAMDPROC glTessellationFactorAMD;
  extern PFNGLTESSELLATIONMODEAMDPROC glTessellationModeAMD;

  // GL_EXT_provoking_vertex
  extern PFNGLPROVOKINGVERTEXEXTPROC glProvokingVertexEXT;

  // GL_AMD_draw_buffers_blend
  extern PFNGLBLENDFUNCINDEXEDAMDPROC glBlendFuncIndexedAMD;
  extern PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC glBlendFuncSeparateIndexedAMD;
  extern PFNGLBLENDEQUATIONINDEXEDAMDPROC glBlendEquationIndexedAMD;
  extern PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC glBlendEquationSeparateIndexedAMD;

  // GL_APPLE_texture_range
  extern PFNGLTEXTURERANGEAPPLEPROC glTextureRangeAPPLE;
  extern PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC glGetTexParameterPointervAPPLE;

  // GL_APPLE_vertex_program_evaluators
  extern PFNGLENABLEVERTEXATTRIBAPPLEPROC glEnableVertexAttribAPPLE;
  extern PFNGLDISABLEVERTEXATTRIBAPPLEPROC glDisableVertexAttribAPPLE;
  extern PFNGLISVERTEXATTRIBENABLEDAPPLEPROC glIsVertexAttribEnabledAPPLE;
  extern PFNGLMAPVERTEXATTRIB1DAPPLEPROC glMapVertexAttrib1dAPPLE;
  extern PFNGLMAPVERTEXATTRIB1FAPPLEPROC glMapVertexAttrib1fAPPLE;
  extern PFNGLMAPVERTEXATTRIB2DAPPLEPROC glMapVertexAttrib2dAPPLE;
  extern PFNGLMAPVERTEXATTRIB2FAPPLEPROC glMapVertexAttrib2fAPPLE;

  // GL_APPLE_object_purgeable
  extern PFNGLOBJECTPURGEABLEAPPLEPROC glObjectPurgeableAPPLE;
  extern PFNGLOBJECTUNPURGEABLEAPPLEPROC glObjectUnpurgeableAPPLE;
  extern PFNGLGETOBJECTPARAMETERIVAPPLEPROC glGetObjectParameterivAPPLE;

  // GL_NV_video_capture
  extern PFNGLBEGINVIDEOCAPTURENVPROC glBeginVideoCaptureNV;
  extern PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC glBindVideoCaptureStreamBufferNV;
  extern PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC glBindVideoCaptureStreamTextureNV;
  extern PFNGLENDVIDEOCAPTURENVPROC glEndVideoCaptureNV;
  extern PFNGLGETVIDEOCAPTUREIVNVPROC glGetVideoCaptureivNV;
  extern PFNGLGETVIDEOCAPTURESTREAMIVNVPROC glGetVideoCaptureStreamivNV;
  extern PFNGLGETVIDEOCAPTURESTREAMFVNVPROC glGetVideoCaptureStreamfvNV;
  extern PFNGLGETVIDEOCAPTURESTREAMDVNVPROC glGetVideoCaptureStreamdvNV;
  extern PFNGLVIDEOCAPTURENVPROC glVideoCaptureNV;
  extern PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC glVideoCaptureStreamParameterivNV;
  extern PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC glVideoCaptureStreamParameterfvNV;
  extern PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC glVideoCaptureStreamParameterdvNV;

  // GL_NV_copy_image
  extern PFNGLCOPYIMAGESUBDATANVPROC glCopyImageSubDataNV;

  // GL_EXT_separate_shader_objects
  extern PFNGLUSESHADERPROGRAMEXTPROC glUseShaderProgramEXT;
  extern PFNGLACTIVEPROGRAMEXTPROC glActiveProgramEXT;
  extern PFNGLCREATESHADERPROGRAMEXTPROC glCreateShaderProgramEXT;

  // GL_NV_shader_buffer_load
  extern PFNGLMAKEBUFFERRESIDENTNVPROC glMakeBufferResidentNV;
  extern PFNGLMAKEBUFFERNONRESIDENTNVPROC glMakeBufferNonResidentNV;
  extern PFNGLISBUFFERRESIDENTNVPROC glIsBufferResidentNV;
  extern PFNGLMAKENAMEDBUFFERRESIDENTNVPROC glMakeNamedBufferResidentNV;
  extern PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC glMakeNamedBufferNonResidentNV;
  extern PFNGLISNAMEDBUFFERRESIDENTNVPROC glIsNamedBufferResidentNV;
  extern PFNGLGETBUFFERPARAMETERUI64VNVPROC glGetBufferParameterui64vNV;
  extern PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC glGetNamedBufferParameterui64vNV;
  extern PFNGLGETINTEGERUI64VNVPROC glGetIntegerui64vNV;
  extern PFNGLUNIFORMUI64NVPROC glUniformui64NV;
  extern PFNGLUNIFORMUI64VNVPROC glUniformui64vNV;
  extern PFNGLGETUNIFORMUI64VNVPROC glGetUniformui64vNV;
  extern PFNGLPROGRAMUNIFORMUI64NVPROC glProgramUniformui64NV;
  extern PFNGLPROGRAMUNIFORMUI64VNVPROC glProgramUniformui64vNV;

  // GL_NV_vertex_buffer_unified_memory
  extern PFNGLBUFFERADDRESSRANGENVPROC glBufferAddressRangeNV;
  extern PFNGLVERTEXFORMATNVPROC glVertexFormatNV;
  extern PFNGLNORMALFORMATNVPROC glNormalFormatNV;
  extern PFNGLCOLORFORMATNVPROC glColorFormatNV;
  extern PFNGLINDEXFORMATNVPROC glIndexFormatNV;
  extern PFNGLTEXCOORDFORMATNVPROC glTexCoordFormatNV;
  extern PFNGLEDGEFLAGFORMATNVPROC glEdgeFlagFormatNV;
  extern PFNGLSECONDARYCOLORFORMATNVPROC glSecondaryColorFormatNV;
  extern PFNGLFOGCOORDFORMATNVPROC glFogCoordFormatNV;
  extern PFNGLVERTEXATTRIBFORMATNVPROC glVertexAttribFormatNV;
  extern PFNGLVERTEXATTRIBIFORMATNVPROC glVertexAttribIFormatNV;
  extern PFNGLGETINTEGERUI64I_VNVPROC glGetIntegerui64i_vNV;

  // GL_NV_texture_barrier
  extern PFNGLTEXTUREBARRIERNVPROC glTextureBarrierNV;

  // GL_EXT_shader_image_load_store
  extern PFNGLBINDIMAGETEXTUREEXTPROC glBindImageTextureEXT;
  extern PFNGLMEMORYBARRIEREXTPROC glMemoryBarrierEXT;

  // GL_EXT_vertex_attrib_64bit
  extern PFNGLVERTEXATTRIBL1DEXTPROC glVertexAttribL1dEXT;
  extern PFNGLVERTEXATTRIBL2DEXTPROC glVertexAttribL2dEXT;
  extern PFNGLVERTEXATTRIBL3DEXTPROC glVertexAttribL3dEXT;
  extern PFNGLVERTEXATTRIBL4DEXTPROC glVertexAttribL4dEXT;
  extern PFNGLVERTEXATTRIBL1DVEXTPROC glVertexAttribL1dvEXT;
  extern PFNGLVERTEXATTRIBL2DVEXTPROC glVertexAttribL2dvEXT;
  extern PFNGLVERTEXATTRIBL3DVEXTPROC glVertexAttribL3dvEXT;
  extern PFNGLVERTEXATTRIBL4DVEXTPROC glVertexAttribL4dvEXT;
  extern PFNGLVERTEXATTRIBLPOINTEREXTPROC glVertexAttribLPointerEXT;
  extern PFNGLGETVERTEXATTRIBLDVEXTPROC glGetVertexAttribLdvEXT;
  extern PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glVertexArrayVertexAttribLOffsetEXT;

  // GL_NV_gpu_program5
  extern PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC glProgramSubroutineParametersuivNV;
  extern PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC glGetProgramSubroutineParameteruivNV;

  // GL_NV_gpu_shader5
  extern PFNGLUNIFORM1I64NVPROC glUniform1i64NV;
  extern PFNGLUNIFORM2I64NVPROC glUniform2i64NV;
  extern PFNGLUNIFORM3I64NVPROC glUniform3i64NV;
  extern PFNGLUNIFORM4I64NVPROC glUniform4i64NV;
  extern PFNGLUNIFORM1I64VNVPROC glUniform1i64vNV;
  extern PFNGLUNIFORM2I64VNVPROC glUniform2i64vNV;
  extern PFNGLUNIFORM3I64VNVPROC glUniform3i64vNV;
  extern PFNGLUNIFORM4I64VNVPROC glUniform4i64vNV;
  extern PFNGLUNIFORM1UI64NVPROC glUniform1ui64NV;
  extern PFNGLUNIFORM2UI64NVPROC glUniform2ui64NV;
  extern PFNGLUNIFORM3UI64NVPROC glUniform3ui64NV;
  extern PFNGLUNIFORM4UI64NVPROC glUniform4ui64NV;
  extern PFNGLUNIFORM1UI64VNVPROC glUniform1ui64vNV;
  extern PFNGLUNIFORM2UI64VNVPROC glUniform2ui64vNV;
  extern PFNGLUNIFORM3UI64VNVPROC glUniform3ui64vNV;
  extern PFNGLUNIFORM4UI64VNVPROC glUniform4ui64vNV;
  extern PFNGLGETUNIFORMI64VNVPROC glGetUniformi64vNV;
  extern PFNGLPROGRAMUNIFORM1I64NVPROC glProgramUniform1i64NV;
  extern PFNGLPROGRAMUNIFORM2I64NVPROC glProgramUniform2i64NV;
  extern PFNGLPROGRAMUNIFORM3I64NVPROC glProgramUniform3i64NV;
  extern PFNGLPROGRAMUNIFORM4I64NVPROC glProgramUniform4i64NV;
  extern PFNGLPROGRAMUNIFORM1I64VNVPROC glProgramUniform1i64vNV;
  extern PFNGLPROGRAMUNIFORM2I64VNVPROC glProgramUniform2i64vNV;
  extern PFNGLPROGRAMUNIFORM3I64VNVPROC glProgramUniform3i64vNV;
  extern PFNGLPROGRAMUNIFORM4I64VNVPROC glProgramUniform4i64vNV;
  extern PFNGLPROGRAMUNIFORM1UI64NVPROC glProgramUniform1ui64NV;
  extern PFNGLPROGRAMUNIFORM2UI64NVPROC glProgramUniform2ui64NV;
  extern PFNGLPROGRAMUNIFORM3UI64NVPROC glProgramUniform3ui64NV;
  extern PFNGLPROGRAMUNIFORM4UI64NVPROC glProgramUniform4ui64NV;
  extern PFNGLPROGRAMUNIFORM1UI64VNVPROC glProgramUniform1ui64vNV;
  extern PFNGLPROGRAMUNIFORM2UI64VNVPROC glProgramUniform2ui64vNV;
  extern PFNGLPROGRAMUNIFORM3UI64VNVPROC glProgramUniform3ui64vNV;
  extern PFNGLPROGRAMUNIFORM4UI64VNVPROC glProgramUniform4ui64vNV;

  // GL_NV_vertex_attrib_integer_64bit
  extern PFNGLVERTEXATTRIBL1I64NVPROC glVertexAttribL1i64NV;
  extern PFNGLVERTEXATTRIBL2I64NVPROC glVertexAttribL2i64NV;
  extern PFNGLVERTEXATTRIBL3I64NVPROC glVertexAttribL3i64NV;
  extern PFNGLVERTEXATTRIBL4I64NVPROC glVertexAttribL4i64NV;
  extern PFNGLVERTEXATTRIBL1I64VNVPROC glVertexAttribL1i64vNV;
  extern PFNGLVERTEXATTRIBL2I64VNVPROC glVertexAttribL2i64vNV;
  extern PFNGLVERTEXATTRIBL3I64VNVPROC glVertexAttribL3i64vNV;
  extern PFNGLVERTEXATTRIBL4I64VNVPROC glVertexAttribL4i64vNV;
  extern PFNGLVERTEXATTRIBL1UI64NVPROC glVertexAttribL1ui64NV;
  extern PFNGLVERTEXATTRIBL2UI64NVPROC glVertexAttribL2ui64NV;
  extern PFNGLVERTEXATTRIBL3UI64NVPROC glVertexAttribL3ui64NV;
  extern PFNGLVERTEXATTRIBL4UI64NVPROC glVertexAttribL4ui64NV;
  extern PFNGLVERTEXATTRIBL1UI64VNVPROC glVertexAttribL1ui64vNV;
  extern PFNGLVERTEXATTRIBL2UI64VNVPROC glVertexAttribL2ui64vNV;
  extern PFNGLVERTEXATTRIBL3UI64VNVPROC glVertexAttribL3ui64vNV;
  extern PFNGLVERTEXATTRIBL4UI64VNVPROC glVertexAttribL4ui64vNV;
  extern PFNGLGETVERTEXATTRIBLI64VNVPROC glGetVertexAttribLi64vNV;
  extern PFNGLGETVERTEXATTRIBLUI64VNVPROC glGetVertexAttribLui64vNV;
  extern PFNGLVERTEXATTRIBLFORMATNVPROC glVertexAttribLFormatNV;

  // GL_AMD_name_gen_delete
  extern PFNGLGENNAMESAMDPROC glGenNamesAMD;
  extern PFNGLDELETENAMESAMDPROC glDeleteNamesAMD;
  extern PFNGLISNAMEAMDPROC glIsNameAMD;

  // GL_AMD_debug_output
  extern PFNGLDEBUGMESSAGEENABLEAMDPROC glDebugMessageEnableAMD;
  extern PFNGLDEBUGMESSAGEINSERTAMDPROC glDebugMessageInsertAMD;
  extern PFNGLDEBUGMESSAGECALLBACKAMDPROC glDebugMessageCallbackAMD;
  extern PFNGLGETDEBUGMESSAGELOGAMDPROC glGetDebugMessageLogAMD;

  // GL_NV_vdpau_interop
  extern PFNGLVDPAUINITNVPROC glVDPAUInitNV;
  extern PFNGLVDPAUFININVPROC glVDPAUFiniNV;
  extern PFNGLVDPAUREGISTERVIDEOSURFACENVPROC glVDPAURegisterVideoSurfaceNV;
  extern PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC glVDPAURegisterOutputSurfaceNV;
  extern PFNGLVDPAUISSURFACENVPROC glVDPAUIsSurfaceNV;
  extern PFNGLVDPAUUNREGISTERSURFACENVPROC glVDPAUUnregisterSurfaceNV;
  extern PFNGLVDPAUGETSURFACEIVNVPROC glVDPAUGetSurfaceivNV;
  extern PFNGLVDPAUSURFACEACCESSNVPROC glVDPAUSurfaceAccessNV;
  extern PFNGLVDPAUMAPSURFACESNVPROC glVDPAUMapSurfacesNV;
  extern PFNGLVDPAUUNMAPSURFACESNVPROC glVDPAUUnmapSurfacesNV;

  // --- WGL ------------------------------------------------------------------

#if _WIN32
  // WGL_ARB_buffer_region
  extern PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegionARB;
  extern PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegionARB;
  extern PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegionARB;
  extern PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB;

  // WGL_ARB_extensions_string
  extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

  // WGL_ARB_pixel_format
  extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
  extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
  extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

  // WGL_ARB_make_current_read
  extern PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
  extern PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB;

  // WGL_ARB_pbuffer
  extern PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
  extern PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
  extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
  extern PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
  extern PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;

  // WGL_ARB_render_texture
  extern PFNWGLBINDTEXIMAGEARBPROC wglBindTexImageARB;
  extern PFNWGLRELEASETEXIMAGEARBPROC wglReleaseTexImageARB;
  extern PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB;

  // WGL_ARB_create_context
  extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

  // WGL_EXT_display_color_table
  extern PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC wglCreateDisplayColorTableEXT;
  extern PFNWGLLOADDISPLAYCOLORTABLEEXTPROC wglLoadDisplayColorTableEXT;
  extern PFNWGLBINDDISPLAYCOLORTABLEEXTPROC wglBindDisplayColorTableEXT;
  extern PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC wglDestroyDisplayColorTableEXT;

  // WGL_EXT_extensions_string
  extern PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;

  // WGL_EXT_make_current_read
  extern PFNWGLMAKECONTEXTCURRENTEXTPROC wglMakeContextCurrentEXT;
  extern PFNWGLGETCURRENTREADDCEXTPROC wglGetCurrentReadDCEXT;

  // WGL_EXT_pbuffer
  extern PFNWGLCREATEPBUFFEREXTPROC wglCreatePbufferEXT;
  extern PFNWGLGETPBUFFERDCEXTPROC wglGetPbufferDCEXT;
  extern PFNWGLRELEASEPBUFFERDCEXTPROC wglReleasePbufferDCEXT;
  extern PFNWGLDESTROYPBUFFEREXTPROC wglDestroyPbufferEXT;
  extern PFNWGLQUERYPBUFFEREXTPROC wglQueryPbufferEXT;

  // WGL_EXT_pixel_format
  extern PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivEXT;
  extern PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvEXT;
  extern PFNWGLCHOOSEPIXELFORMATEXTPROC wglChoosePixelFormatEXT;

  // WGL_EXT_swap_control
  extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
  extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;

  // WGL_NV_vertex_array_range
  extern PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
  extern PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;

  // WGL_OML_sync_control
  extern PFNWGLGETSYNCVALUESOMLPROC wglGetSyncValuesOML;
  extern PFNWGLGETMSCRATEOMLPROC wglGetMscRateOML;
  extern PFNWGLSWAPBUFFERSMSCOMLPROC wglSwapBuffersMscOML;
  extern PFNWGLSWAPLAYERBUFFERSMSCOMLPROC wglSwapLayerBuffersMscOML;
  extern PFNWGLWAITFORMSCOMLPROC wglWaitForMscOML;
  extern PFNWGLWAITFORSBCOMLPROC wglWaitForSbcOML;

  // WGL_I3D_digital_video_control
  extern PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC wglGetDigitalVideoParametersI3D;
  extern PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC wglSetDigitalVideoParametersI3D;

  // WGL_I3D_gamma
  extern PFNWGLGETGAMMATABLEPARAMETERSI3DPROC wglGetGammaTableParametersI3D;
  extern PFNWGLSETGAMMATABLEPARAMETERSI3DPROC wglSetGammaTableParametersI3D;
  extern PFNWGLGETGAMMATABLEI3DPROC wglGetGammaTableI3D;
  extern PFNWGLSETGAMMATABLEI3DPROC wglSetGammaTableI3D;

  // WGL_I3D_genlock
  extern PFNWGLENABLEGENLOCKI3DPROC wglEnableGenlockI3D;
  extern PFNWGLDISABLEGENLOCKI3DPROC wglDisableGenlockI3D;
  extern PFNWGLISENABLEDGENLOCKI3DPROC wglIsEnabledGenlockI3D;
  extern PFNWGLGENLOCKSOURCEI3DPROC wglGenlockSourceI3D;
  extern PFNWGLGETGENLOCKSOURCEI3DPROC wglGetGenlockSourceI3D;
  extern PFNWGLGENLOCKSOURCEEDGEI3DPROC wglGenlockSourceEdgeI3D;
  extern PFNWGLGETGENLOCKSOURCEEDGEI3DPROC wglGetGenlockSourceEdgeI3D;
  extern PFNWGLGENLOCKSAMPLERATEI3DPROC wglGenlockSampleRateI3D;
  extern PFNWGLGETGENLOCKSAMPLERATEI3DPROC wglGetGenlockSampleRateI3D;
  extern PFNWGLGENLOCKSOURCEDELAYI3DPROC wglGenlockSourceDelayI3D;
  extern PFNWGLGETGENLOCKSOURCEDELAYI3DPROC wglGetGenlockSourceDelayI3D;
  extern PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC wglQueryGenlockMaxSourceDelayI3D;

  // WGL_I3D_image_buffer
  extern PFNWGLCREATEIMAGEBUFFERI3DPROC wglCreateImageBufferI3D;
  extern PFNWGLDESTROYIMAGEBUFFERI3DPROC wglDestroyImageBufferI3D;
  extern PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC wglAssociateImageBufferEventsI3D;
  extern PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC wglReleaseImageBufferEventsI3D;

  // WGL_I3D_swap_frame_lock
  extern PFNWGLENABLEFRAMELOCKI3DPROC wglEnableFrameLockI3D;
  extern PFNWGLDISABLEFRAMELOCKI3DPROC wglDisableFrameLockI3D;
  extern PFNWGLISENABLEDFRAMELOCKI3DPROC wglIsEnabledFrameLockI3D;
  extern PFNWGLQUERYFRAMELOCKMASTERI3DPROC wglQueryFrameLockMasterI3D;

  // WGL_I3D_swap_frame_usage
  extern PFNWGLGETFRAMEUSAGEI3DPROC wglGetFrameUsageI3D;
  extern PFNWGLBEGINFRAMETRACKINGI3DPROC wglBeginFrameTrackingI3D;
  extern PFNWGLENDFRAMETRACKINGI3DPROC wglEndFrameTrackingI3D;
  extern PFNWGLQUERYFRAMETRACKINGI3DPROC wglQueryFrameTrackingI3D;

  // WGL_3DL_stereo_control
  extern PFNWGLSETSTEREOEMITTERSTATE3DLPROC wglSetStereoEmitterState3DL;

  // WGL_NV_present_video
  extern PFNWGLENUMERATEVIDEODEVICESNVPROC wglEnumerateVideoDevicesNV;
  extern PFNWGLBINDVIDEODEVICENVPROC wglBindVideoDeviceNV;
  extern PFNWGLQUERYCURRENTCONTEXTNVPROC wglQueryCurrentContextNV;

  // WGL_NV_video_output
  extern PFNWGLGETVIDEODEVICENVPROC wglGetVideoDeviceNV;
  extern PFNWGLRELEASEVIDEODEVICENVPROC wglReleaseVideoDeviceNV;
  extern PFNWGLBINDVIDEOIMAGENVPROC wglBindVideoImageNV;
  extern PFNWGLRELEASEVIDEOIMAGENVPROC wglReleaseVideoImageNV;
  extern PFNWGLSENDPBUFFERTOVIDEONVPROC wglSendPbufferToVideoNV;
  extern PFNWGLGETVIDEOINFONVPROC wglGetVideoInfoNV;

  // WGL_NV_swap_group
  extern PFNWGLJOINSWAPGROUPNVPROC wglJoinSwapGroupNV;
  extern PFNWGLBINDSWAPBARRIERNVPROC wglBindSwapBarrierNV;
  extern PFNWGLQUERYSWAPGROUPNVPROC wglQuerySwapGroupNV;
  extern PFNWGLQUERYMAXSWAPGROUPSNVPROC wglQueryMaxSwapGroupsNV;
  extern PFNWGLQUERYFRAMECOUNTNVPROC wglQueryFrameCountNV;
  extern PFNWGLRESETFRAMECOUNTNVPROC wglResetFrameCountNV;

  // WGL_NV_gpu_affinity
  extern PFNWGLENUMGPUSNVPROC wglEnumGpusNV;
  extern PFNWGLENUMGPUDEVICESNVPROC wglEnumGpuDevicesNV;
  extern PFNWGLCREATEAFFINITYDCNVPROC wglCreateAffinityDCNV;
  extern PFNWGLENUMGPUSFROMAFFINITYDCNVPROC wglEnumGpusFromAffinityDCNV;
  extern PFNWGLDELETEDCNVPROC wglDeleteDCNV;

  // WGL_AMD_gpu_association
  extern PFNWGLGETGPUIDSAMDPROC wglGetGPUIDsAMD;
  extern PFNWGLGETGPUINFOAMDPROC wglGetGPUInfoAMD;
  extern PFNWGLGETCONTEXTGPUIDAMDPROC wglGetContextGPUIDAMD;
  extern PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC wglCreateAssociatedContextAMD;
  extern PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC wglCreateAssociatedContextAttribsAMD;
  extern PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC wglDeleteAssociatedContextAMD;
  extern PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC wglMakeAssociatedContextCurrentAMD;
  extern PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC wglGetCurrentAssociatedContextAMD;
  extern PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC wglBlitContextFramebufferAMD;

  // WGL_NV_video_capture
  extern PFNWGLBINDVIDEOCAPTUREDEVICENVPROC wglBindVideoCaptureDeviceNV;
  extern PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC wglEnumerateVideoCaptureDevicesNV;
  extern PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC wglLockVideoCaptureDeviceNV;
  extern PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC wglQueryVideoCaptureDeviceNV;
  extern PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC wglReleaseVideoCaptureDeviceNV;

  // WGL_NV_copy_image
  extern PFNWGLCOPYIMAGESUBDATANVPROC wglCopyImageSubDataNV;
#endif

  // --- GLX ------------------------------------------------------------------

#ifdef _UNIX
  // GLX_EXT_import_context
  extern PFNGLXGETCURRENTDISPLAYEXTPROC glXGetCurrentDisplayEXT;
  extern PFNGLXQUERYCONTEXTINFOEXTPROC glXQueryContextInfoEXT;
  extern PFNGLXGETCONTEXTIDEXTPROC glXGetContextIDEXT;
  extern PFNGLXIMPORTCONTEXTEXTPROC glXImportContextEXT;
  extern PFNGLXFREECONTEXTEXTPROC glXFreeContextEXT;

  // GLX_SGIX_fbconfig
  extern PFNGLXGETFBCONFIGATTRIBSGIXPROC glXGetFBConfigAttribSGIX;
  extern PFNGLXCHOOSEFBCONFIGSGIXPROC glXChooseFBConfigSGIX;
  extern PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC glXCreateGLXPixmapWithConfigSGIX;
  extern PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC glXCreateContextWithConfigSGIX;
  extern PFNGLXGETVISUALFROMFBCONFIGSGIXPROC glXGetVisualFromFBConfigSGIX;
  extern PFNGLXGETFBCONFIGFROMVISUALSGIXPROC glXGetFBConfigFromVisualSGIX;

  // GLX_SGIX_pbuffer
  extern PFNGLXCREATEGLXPBUFFERSGIXPROC glXCreateGLXPbufferSGIX;
  extern PFNGLXDESTROYGLXPBUFFERSGIXPROC glXDestroyGLXPbufferSGIX;
  extern PFNGLXQUERYGLXPBUFFERSGIXPROC glXQueryGLXPbufferSGIX;
  extern PFNGLXSELECTEVENTSGIXPROC glXSelectEventSGIX;
  extern PFNGLXGETSELECTEDEVENTSGIXPROC glXGetSelectedEventSGIX;

  // GLX_SGI_cushion
  extern PFNGLXCUSHIONSGIPROC glXCushionSGI;

  // GLX_SGIX_video_resize
  extern PFNGLXBINDCHANNELTOWINDOWSGIXPROC glXBindChannelToWindowSGIX;
  extern PFNGLXCHANNELRECTSGIXPROC glXChannelRectSGIX;
  extern PFNGLXQUERYCHANNELRECTSGIXPROC glXQueryChannelRectSGIX;
  extern PFNGLXQUERYCHANNELDELTASSGIXPROC glXQueryChannelDeltasSGIX;
  extern PFNGLXCHANNELRECTSYNCSGIXPROC glXChannelRectSyncSGIX;

  // GLX_SGIX_dmbuffer
  extern PFNGLXASSOCIATEDMPBUFFERSGIXPROC glXAssociateDMPbufferSGIX;

  // GLX_SGIX_swap_group
  extern PFNGLXJOINSWAPGROUPSGIXPROC glXJoinSwapGroupSGIX;

  // GLX_SGIX_swap_barrier
  extern PFNGLXBINDSWAPBARRIERSGIXPROC glXBindSwapBarrierSGIX;
  extern PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC glXQueryMaxSwapBarriersSGIX;

  // GLX_SUN_get_transparent_index
  extern PFNGLXGETTRANSPARENTINDEXSUNPROC glXGetTransparentIndexSUN;

  // GLX_MESA_copy_sub_buffer
  extern PFNGLXCOPYSUBBUFFERMESAPROC glXCopySubBufferMESA;

  // GLX_MESA_pixmap_colormap
  extern PFNGLXCREATEGLXPIXMAPMESAPROC glXCreateGLXPixmapMESA;

  // GLX_MESA_release_buffers
  extern PFNGLXRELEASEBUFFERSMESAPROC glXReleaseBuffersMESA;

  // GLX_MESA_set_3dfx_mode
  extern PFNGLXSET3DFXMODEMESAPROC glXSet3DfxModeMESA;

  // GLX_OML_sync_control
  extern PFNGLXGETSYNCVALUESOMLPROC glXGetSyncValuesOML;
  extern PFNGLXGETMSCRATEOMLPROC glXGetMscRateOML;
  extern PFNGLXSWAPBUFFERSMSCOMLPROC glXSwapBuffersMscOML;
  extern PFNGLXWAITFORMSCOMLPROC glXWaitForMscOML;
  extern PFNGLXWAITFORSBCOMLPROC glXWaitForSbcOML;

  // GLX_SGIX_hyperpipe
  extern PFNGLXQUERYHYPERPIPENETWORKSGIXPROC glXQueryHyperpipeNetworkSGIX;
  extern PFNGLXHYPERPIPECONFIGSGIXPROC glXHyperpipeConfigSGIX;
  extern PFNGLXQUERYHYPERPIPECONFIGSGIXPROC glXQueryHyperpipeConfigSGIX;
  extern PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC glXDestroyHyperpipeConfigSGIX;
  extern PFNGLXBINDHYPERPIPESGIXPROC glXBindHyperpipeSGIX;
  extern PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC glXQueryHyperpipeBestAttribSGIX;
  extern PFNGLXHYPERPIPEATTRIBSGIXPROC glXHyperpipeAttribSGIX;
  extern PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC glXQueryHyperpipeAttribSGIX;

  // GLX_MESA_agp_offset
  extern PFNGLXGETAGPOFFSETMESAPROC glXGetAGPOffsetMESA;

  // GLX_EXT_texture_from_pixmap
  extern PFNGLXBINDTEXIMAGEEXTPROC glXBindTexImageEXT;
  extern PFNGLXRELEASETEXIMAGEEXTPROC glXReleaseTexImageEXT;

  // GLX_NV_present_video
  extern PFNGLXENUMERATEVIDEODEVICESNVPROC glXEnumerateVideoDevicesNV;
  extern PFNGLXBINDVIDEODEVICENVPROC glXBindVideoDeviceNV;

  // GLX_NV_video_output
  extern PFNGLXGETVIDEODEVICENVPROC glXGetVideoDeviceNV;
  extern PFNGLXRELEASEVIDEODEVICENVPROC glXReleaseVideoDeviceNV;
  extern PFNGLXBINDVIDEOIMAGENVPROC glXBindVideoImageNV;
  extern PFNGLXRELEASEVIDEOIMAGENVPROC glXReleaseVideoImageNV;
  extern PFNGLXSENDPBUFFERTOVIDEONVPROC glXSendPbufferToVideoNV;
  extern PFNGLXGETVIDEOINFONVPROC glXGetVideoInfoNV;

    // GLX_NV_swap_group
  extern PFNGLXJOINSWAPGROUPNVPROC glXJoinSwapGroupNV;
  extern PFNGLXBINDSWAPBARRIERNVPROC glXBindSwapBarrierNV;
  extern PFNGLXQUERYSWAPGROUPNVPROC glXQuerySwapGroupNV;
  extern PFNGLXQUERYMAXSWAPGROUPSNVPROC glXQueryMaxSwapGroupsNV;
  extern PFNGLXQUERYFRAMECOUNTNVPROC glXQueryFrameCountNV;
  extern PFNGLXRESETFRAMECOUNTNVPROC glXResetFrameCountNV;

    // GLX_NV_video_capture
  extern PFNGLXBINDVIDEOCAPTUREDEVICENVPROC glXBindVideoCaptureDeviceNV;
  extern PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC glXEnumerateVideoCaptureDevicesNV;
  extern PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC glXLockVideoCaptureDeviceNV;
  extern PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC glXQueryVideoCaptureDeviceNV;
  extern PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC glXReleaseVideoCaptureDeviceNV;

    // GLX_EXT_swap_control
  extern PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;

    // GLX_NV_copy_image
  extern PFNGLXCOPYIMAGESUBDATANVPROC glXCopyImageSubDataNV;
#endif
}

#endif

