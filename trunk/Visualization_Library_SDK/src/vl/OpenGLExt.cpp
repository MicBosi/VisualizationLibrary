/**************************************************************************************/
/*          */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*          */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*          */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*          */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*          */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*          */
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
/*          */
/**************************************************************************************/

#include "vl/OpenGL.hpp"
#include <string>

using namespace vl;

namespace vl
{
  class ExtensionInitializer
  {
    std::string mExtensions;
    bool mIsCompatible;

  public:
    bool hasExtension(const char* ext)
    {
      const char* pos = strstr(mExtensions.c_str(), ext);
      return pos != NULL;
    }

    const char* initExtensions()
    {
      int cur_major = -1;
      int cur_minor = -1;
      sscanf((const char*)glGetString(GL_VERSION), "%d.%d", cur_major, cur_minor);

      if (cur_major == -1 || cur_minor == -1)
        return "OpenGL Context Not Present";

      // check if compatible mode is supported

      // clear errors
      glGetError();
      // test if fixed function pipeline is supported.
      glDisable(GL_TEXTURE_2D);
      // check error code
      mIsCompatible = glGetError() == GL_NO_ERROR;

      // check all available opengl versions

      #define IS_COMPATIBLE(major, minor) (mIsCompatible && major > cur_major) || (major == cur_major)

      Has_GL_VERSION_1_0 = IS_COMPATIBLE(1,0);
      Has_GL_VERSION_1_1 = IS_COMPATIBLE(1,1);
      Has_GL_VERSION_1_2 = IS_COMPATIBLE(1,2);
      Has_GL_VERSION_1_3 = IS_COMPATIBLE(1,3);
      Has_GL_VERSION_1_4 = IS_COMPATIBLE(1,4);
      Has_GL_VERSION_1_5 = IS_COMPATIBLE(1,5);
      Has_GL_VERSION_2_0 = IS_COMPATIBLE(2,0);
      Has_GL_VERSION_2_1 = IS_COMPATIBLE(2,1);
      Has_GL_VERSION_3_0 = IS_COMPATIBLE(3,0);
      Has_GL_VERSION_3_1 = IS_COMPATIBLE(3,1);
      Has_GL_VERSION_3_2 = IS_COMPATIBLE(3,2);
      Has_GL_VERSION_3_3 = IS_COMPATIBLE(3,3);
      Has_GL_VERSION_4_0 = IS_COMPATIBLE(4,0);
      Has_GL_VERSION_4_1 = IS_COMPATIBLE(4,1);

      // fill a string with all the extensions
      if (Has_GL_VERSION_3_0)
      {
        int count = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &count);
        for( int i=0; i<count; ++i )
        {
          const char* str = (const char*)glGetStringi(GL_EXTENSIONS,i); VL_CHECK_OGL();
          if (!str)
            break;
          mExtensions += std::string(str) + " ";
        }
      } 
      else
      {
        VL_CHECK(glGetString(GL_EXTENSIONS));
        mExtensions = (const char*)glGetString(GL_EXTENSIONS);
      }
      // insert extra delimiters at the beginning and end of the string list
      // so we can safely check for the extension strings.
      mExtensions = " " + mExtensions + " ";

      Has_GL_ARB_imaging = hasExtension(" GL_ARB_imaging ");
      Has_GL_ARB_multitexture = hasExtension(" GL_ARB_multitexture ");
      Has_GLX_ARB_get_proc_address = hasExtension(" GLX_ARB_get_proc_address ");
      Has_GL_ARB_transpose_matrix = hasExtension(" GL_ARB_transpose_matrix ");
      Has_WGL_ARB_buffer_region = hasExtension(" WGL_ARB_buffer_region ");
      Has_GL_ARB_multisample = hasExtension(" GL_ARB_multisample ");
      Has_GLX_ARB_multisample = hasExtension(" GLX_ARB_multisample ");
      Has_WGL_ARB_multisample = hasExtension(" WGL_ARB_multisample ");
      Has_GL_ARB_texture_env_add = hasExtension(" GL_ARB_texture_env_add ");
      Has_GL_ARB_texture_cube_map = hasExtension(" GL_ARB_texture_cube_map ");
      Has_WGL_ARB_extensions_string = hasExtension(" WGL_ARB_extensions_string ");
      Has_WGL_ARB_pixel_format = hasExtension(" WGL_ARB_pixel_format ");
      Has_WGL_ARB_make_current_read = hasExtension(" WGL_ARB_make_current_read ");
      Has_WGL_ARB_pbuffer = hasExtension(" WGL_ARB_pbuffer ");
      Has_GL_ARB_texture_compression = hasExtension(" GL_ARB_texture_compression ");
      Has_GL_ARB_texture_border_clamp = hasExtension(" GL_ARB_texture_border_clamp ");
      Has_GL_ARB_point_parameters = hasExtension(" GL_ARB_point_parameters ");
      Has_GL_ARB_vertex_blend = hasExtension(" GL_ARB_vertex_blend ");
      Has_GL_ARB_matrix_palette = hasExtension(" GL_ARB_matrix_palette ");
      Has_GL_ARB_texture_env_combine = hasExtension(" GL_ARB_texture_env_combine ");
      Has_GL_ARB_texture_env_crossbar = hasExtension(" GL_ARB_texture_env_crossbar ");
      Has_GL_ARB_texture_env_dot3 = hasExtension(" GL_ARB_texture_env_dot3 ");
      Has_WGL_ARB_render_texture = hasExtension(" WGL_ARB_render_texture ");
      Has_GL_ARB_texture_mirrored_repeat = hasExtension(" GL_ARB_texture_mirrored_repeat ");
      Has_GL_ARB_depth_texture = hasExtension(" GL_ARB_depth_texture ");
      Has_GL_ARB_shadow = hasExtension(" GL_ARB_shadow ");
      Has_GL_ARB_shadow_ambient = hasExtension(" GL_ARB_shadow_ambient ");
      Has_GL_ARB_window_pos = hasExtension(" GL_ARB_window_pos ");
      Has_GL_ARB_vertex_program = hasExtension(" GL_ARB_vertex_program ");
      Has_GL_ARB_fragment_program = hasExtension(" GL_ARB_fragment_program ");
      Has_GL_ARB_vertex_buffer_object = hasExtension(" GL_ARB_vertex_buffer_object ");
      Has_GL_ARB_occlusion_query = hasExtension(" GL_ARB_occlusion_query ");
      Has_GL_ARB_shader_objects = hasExtension(" GL_ARB_shader_objects ");
      Has_GL_ARB_vertex_shader = hasExtension(" GL_ARB_vertex_shader ");
      Has_GL_ARB_fragment_shader = hasExtension(" GL_ARB_fragment_shader ");
      Has_GL_ARB_shading_language_100 = hasExtension(" GL_ARB_shading_language_100 ");
      Has_GL_ARB_texture_non_power_of_two = hasExtension(" GL_ARB_texture_non_power_of_two ");
      Has_GL_ARB_point_sprite = hasExtension(" GL_ARB_point_sprite ");
      Has_GL_ARB_fragment_program_shadow = hasExtension(" GL_ARB_fragment_program_shadow ");
      Has_GL_ARB_draw_buffers = hasExtension(" GL_ARB_draw_buffers ");
      Has_GL_ARB_texture_rectangle = hasExtension(" GL_ARB_texture_rectangle ");
      Has_GL_ARB_color_buffer_float = hasExtension(" GL_ARB_color_buffer_float ");
      Has_WGL_ARB_pixel_format_float = hasExtension(" WGL_ARB_pixel_format_float ");
      Has_GLX_ARB_fbconfig_float = hasExtension(" GLX_ARB_fbconfig_float ");
      Has_GL_ARB_half_float_pixel = hasExtension(" GL_ARB_half_float_pixel ");
      Has_GL_ARB_texture_float = hasExtension(" GL_ARB_texture_float ");
      Has_GL_ARB_pixel_buffer_object = hasExtension(" GL_ARB_pixel_buffer_object ");
      Has_GL_ARB_depth_buffer_float = hasExtension(" GL_ARB_depth_buffer_float ");
      Has_GL_ARB_draw_instanced = hasExtension(" GL_ARB_draw_instanced ");
      Has_GL_ARB_framebuffer_object = hasExtension(" GL_ARB_framebuffer_object ");
      Has_GL_ARB_framebuffer_sRGB = hasExtension(" GL_ARB_framebuffer_sRGB ");
      Has_GLX_ARB_framebuffer_sRGB = hasExtension(" GLX_ARB_framebuffer_sRGB ");
      Has_WGL_ARB_framebuffer_sRGB = hasExtension(" WGL_ARB_framebuffer_sRGB ");
      Has_GL_ARB_geometry_shader4 = hasExtension(" GL_ARB_geometry_shader4 ");
      Has_GL_ARB_half_float_vertex = hasExtension(" GL_ARB_half_float_vertex ");
      Has_GL_ARB_instanced_arrays = hasExtension(" GL_ARB_instanced_arrays ");
      Has_GL_ARB_map_buffer_range = hasExtension(" GL_ARB_map_buffer_range ");
      Has_GL_ARB_texture_buffer_object = hasExtension(" GL_ARB_texture_buffer_object ");
      Has_GL_ARB_texture_compression_rgtc = hasExtension(" GL_ARB_texture_compression_rgtc ");
      Has_GL_ARB_texture_rg = hasExtension(" GL_ARB_texture_rg ");
      Has_GL_ARB_vertex_array_object = hasExtension(" GL_ARB_vertex_array_object ");
      Has_WGL_ARB_create_context = hasExtension(" WGL_ARB_create_context ");
      Has_GLX_ARB_create_context = hasExtension(" GLX_ARB_create_context ");
      Has_GL_ARB_uniform_buffer_object = hasExtension(" GL_ARB_uniform_buffer_object ");
      Has_GL_ARB_compatibility = hasExtension(" GL_ARB_compatibility ");
      Has_GL_ARB_copy_buffer = hasExtension(" GL_ARB_copy_buffer ");
      Has_GL_ARB_shader_texture_lod = hasExtension(" GL_ARB_shader_texture_lod ");
      Has_GL_ARB_depth_clamp = hasExtension(" GL_ARB_depth_clamp ");
      Has_GL_ARB_draw_elements_base_vertex = hasExtension(" GL_ARB_draw_elements_base_vertex ");
      Has_GL_ARB_fragment_coord_conventions = hasExtension(" GL_ARB_fragment_coord_conventions ");
      Has_GL_ARB_provoking_vertex = hasExtension(" GL_ARB_provoking_vertex ");
      Has_GL_ARB_seamless_cube_map = hasExtension(" GL_ARB_seamless_cube_map ");
      Has_GL_ARB_sync = hasExtension(" GL_ARB_sync ");
      Has_GL_ARB_texture_multisample = hasExtension(" GL_ARB_texture_multisample ");
      Has_GL_ARB_vertex_array_bgra = hasExtension(" GL_ARB_vertex_array_bgra ");
      Has_GL_ARB_draw_buffers_blend = hasExtension(" GL_ARB_draw_buffers_blend ");
      Has_GL_ARB_sample_shading = hasExtension(" GL_ARB_sample_shading ");
      Has_GL_ARB_texture_cube_map_array = hasExtension(" GL_ARB_texture_cube_map_array ");
      Has_GL_ARB_texture_gather = hasExtension(" GL_ARB_texture_gather ");
      Has_GL_ARB_texture_query_lod = hasExtension(" GL_ARB_texture_query_lod ");
      Has_WGL_ARB_create_context_profile = hasExtension(" WGL_ARB_create_context_profile ");
      Has_GLX_ARB_create_context_profile = hasExtension(" GLX_ARB_create_context_profile ");
      Has_GL_ARB_shading_language_include = hasExtension(" GL_ARB_shading_language_include ");
      Has_GL_ARB_texture_compression_bptc = hasExtension(" GL_ARB_texture_compression_bptc ");
      Has_GL_ARB_blend_func_extended = hasExtension(" GL_ARB_blend_func_extended ");
      Has_GL_ARB_explicit_attrib_location = hasExtension(" GL_ARB_explicit_attrib_location ");
      Has_GL_ARB_occlusion_query2 = hasExtension(" GL_ARB_occlusion_query2 ");
      Has_GL_ARB_sampler_objects = hasExtension(" GL_ARB_sampler_objects ");
      Has_GL_ARB_shader_bit_encoding = hasExtension(" GL_ARB_shader_bit_encoding ");
      Has_GL_ARB_texture_rgb10_a2ui = hasExtension(" GL_ARB_texture_rgb10_a2ui ");
      Has_GL_ARB_texture_swizzle = hasExtension(" GL_ARB_texture_swizzle ");
      Has_GL_ARB_timer_query = hasExtension(" GL_ARB_timer_query ");
      Has_GL_ARB_vertex_type_2_10_10_10_rev = hasExtension(" GL_ARB_vertex_type_2_10_10_10_rev ");
      Has_GL_ARB_draw_indirect = hasExtension(" GL_ARB_draw_indirect ");
      Has_GL_ARB_gpu_shader5 = hasExtension(" GL_ARB_gpu_shader5 ");
      Has_GL_ARB_gpu_shader_fp64 = hasExtension(" GL_ARB_gpu_shader_fp64 ");
      Has_GL_ARB_shader_subroutine = hasExtension(" GL_ARB_shader_subroutine ");
      Has_GL_ARB_tessellation_shader = hasExtension(" GL_ARB_tessellation_shader ");
      Has_GL_ARB_texture_buffer_object_rgb32 = hasExtension(" GL_ARB_texture_buffer_object_rgb32 ");
      Has_GL_ARB_transform_feedback2 = hasExtension(" GL_ARB_transform_feedback2 ");
      Has_GL_ARB_transform_feedback3 = hasExtension(" GL_ARB_transform_feedback3 ");
      Has_GL_ARB_ES2_compatibility = hasExtension(" GL_ARB_ES2_compatibility ");
      Has_GL_ARB_get_program_binary = hasExtension(" GL_ARB_get_program_binary ");
      Has_GL_ARB_separate_shader_objects = hasExtension(" GL_ARB_separate_shader_objects ");
      Has_GL_ARB_shader_precision = hasExtension(" GL_ARB_shader_precision ");
      Has_GL_ARB_vertex_attrib_64bit = hasExtension(" GL_ARB_vertex_attrib_64bit ");
      Has_GL_ARB_viewport_array = hasExtension(" GL_ARB_viewport_array ");
      Has_GLX_ARB_create_context_robustness = hasExtension(" GLX_ARB_create_context_robustness ");
      Has_WGL_ARB_create_context_robustness = hasExtension(" WGL_ARB_create_context_robustness ");
      Has_GL_ARB_cl_event = hasExtension(" GL_ARB_cl_event ");
      Has_GL_ARB_debug_output = hasExtension(" GL_ARB_debug_output ");
      Has_GL_ARB_robustness = hasExtension(" GL_ARB_robustness ");
      Has_GL_ARB_shader_stencil_export = hasExtension(" GL_ARB_shader_stencil_export ");

      // vendor and EXT extensions

      Has_GL_EXT_abgr = hasExtension(" GL_EXT_abgr ");
      Has_GL_EXT_blend_color = hasExtension(" GL_EXT_blend_color ");
      Has_GL_EXT_polygon_offset = hasExtension(" GL_EXT_polygon_offset ");
      Has_GL_EXT_texture = hasExtension(" GL_EXT_texture ");
      Has_GL_EXT_texture3D = hasExtension(" GL_EXT_texture3D ");
      Has_GL_SGIS_texture_filter4 = hasExtension(" GL_SGIS_texture_filter4 ");
      Has_GL_EXT_subtexture = hasExtension(" GL_EXT_subtexture ");
      Has_GL_EXT_copy_texture = hasExtension(" GL_EXT_copy_texture ");
      Has_GL_EXT_histogram = hasExtension(" GL_EXT_histogram ");
      Has_GL_EXT_convolution = hasExtension(" GL_EXT_convolution ");
      Has_GL_SGI_color_matrix = hasExtension(" GL_SGI_color_matrix ");
      Has_GL_SGI_color_table = hasExtension(" GL_SGI_color_table ");
      Has_GL_SGIS_pixel_texture = hasExtension(" GL_SGIS_pixel_texture ");
      Has_GL_SGIX_pixel_texture = hasExtension(" GL_SGIX_pixel_texture ");
      Has_GL_SGIS_texture4D = hasExtension(" GL_SGIS_texture4D ");
      Has_GL_SGI_texture_color_table = hasExtension(" GL_SGI_texture_color_table ");
      Has_GL_EXT_cmyka = hasExtension(" GL_EXT_cmyka ");
      Has_GL_EXT_texture_object = hasExtension(" GL_EXT_texture_object ");
      Has_GL_SGIS_detail_texture = hasExtension(" GL_SGIS_detail_texture ");
      Has_GL_SGIS_sharpen_texture = hasExtension(" GL_SGIS_sharpen_texture ");
      Has_GL_EXT_packed_pixels = hasExtension(" GL_EXT_packed_pixels ");
      Has_GL_SGIS_texture_lod = hasExtension(" GL_SGIS_texture_lod ");
      Has_GL_SGIS_multisample = hasExtension(" GL_SGIS_multisample ");
      Has_GLX_SGIS_multisample = hasExtension(" GLX_SGIS_multisample ");
      Has_GL_EXT_rescale_normal = hasExtension(" GL_EXT_rescale_normal ");
      Has_GLX_EXT_visual_info = hasExtension(" GLX_EXT_visual_info ");
      Has_GL_EXT_vertex_array = hasExtension(" GL_EXT_vertex_array ");
      Has_GL_EXT_misc_attribute = hasExtension(" GL_EXT_misc_attribute ");
      Has_GL_SGIS_generate_mipmap = hasExtension(" GL_SGIS_generate_mipmap ");
      Has_GL_SGIX_clipmap = hasExtension(" GL_SGIX_clipmap ");
      Has_GL_SGIX_shadow = hasExtension(" GL_SGIX_shadow ");
      Has_GL_SGIS_texture_edge_clamp = hasExtension(" GL_SGIS_texture_edge_clamp ");
      Has_GL_SGIS_texture_border_clamp = hasExtension(" GL_SGIS_texture_border_clamp ");
      Has_GL_EXT_blend_minmax = hasExtension(" GL_EXT_blend_minmax ");
      Has_GL_EXT_blend_subtract = hasExtension(" GL_EXT_blend_subtract ");
      Has_GL_EXT_blend_logic_op = hasExtension(" GL_EXT_blend_logic_op ");
      Has_GLX_SGI_swap_control = hasExtension(" GLX_SGI_swap_control ");
      Has_GLX_SGI_video_sync = hasExtension(" GLX_SGI_video_sync ");
      Has_GLX_SGI_make_current_read = hasExtension(" GLX_SGI_make_current_read ");
      Has_GLX_SGIX_video_source = hasExtension(" GLX_SGIX_video_source ");
      Has_GLX_EXT_visual_rating = hasExtension(" GLX_EXT_visual_rating ");
      Has_GL_SGIX_interlace = hasExtension(" GL_SGIX_interlace ");
      Has_GLX_EXT_import_context = hasExtension(" GLX_EXT_import_context ");
      Has_GLX_SGIX_fbconfig = hasExtension(" GLX_SGIX_fbconfig ");
      Has_GLX_SGIX_pbuffer = hasExtension(" GLX_SGIX_pbuffer ");
      Has_GL_SGIS_texture_select = hasExtension(" GL_SGIS_texture_select ");
      Has_GL_SGIX_sprite = hasExtension(" GL_SGIX_sprite ");
      Has_GL_SGIX_texture_multi_buffer = hasExtension(" GL_SGIX_texture_multi_buffer ");
      Has_GL_EXT_point_parameters = hasExtension(" GL_EXT_point_parameters ");
      Has_GL_SGIX_instruments = hasExtension(" GL_SGIX_instruments ");
      Has_GL_SGIX_texture_scale_bias = hasExtension(" GL_SGIX_texture_scale_bias ");
      Has_GL_SGIX_framezoom = hasExtension(" GL_SGIX_framezoom ");
      Has_GL_SGIX_tag_sample_buffer = hasExtension(" GL_SGIX_tag_sample_buffer ");
      Has_GL_SGIX_reference_plane = hasExtension(" GL_SGIX_reference_plane ");
      Has_GL_SGIX_flush_raster = hasExtension(" GL_SGIX_flush_raster ");
      Has_GLX_SGI_cushion = hasExtension(" GLX_SGI_cushion ");
      Has_GL_SGIX_depth_texture = hasExtension(" GL_SGIX_depth_texture ");
      Has_GL_SGIS_fog_function = hasExtension(" GL_SGIS_fog_function ");
      Has_GL_SGIX_fog_offset = hasExtension(" GL_SGIX_fog_offset ");
      Has_GL_HP_image_transform = hasExtension(" GL_HP_image_transform ");
      Has_GL_HP_convolution_border_modes = hasExtension(" GL_HP_convolution_border_modes ");
      Has_GL_SGIX_texture_add_env = hasExtension(" GL_SGIX_texture_add_env ");
      Has_GL_EXT_color_subtable = hasExtension(" GL_EXT_color_subtable ");
      Has_GLU_EXT_object_space_tess = hasExtension(" GLU_EXT_object_space_tess ");
      Has_GL_PGI_vertex_hints = hasExtension(" GL_PGI_vertex_hints ");
      Has_GL_PGI_misc_hints = hasExtension(" GL_PGI_misc_hints ");
      Has_GL_EXT_paletted_texture = hasExtension(" GL_EXT_paletted_texture ");
      Has_GL_EXT_clip_volume_hint = hasExtension(" GL_EXT_clip_volume_hint ");
      Has_GL_SGIX_list_priority = hasExtension(" GL_SGIX_list_priority ");
      Has_GL_SGIX_ir_instrument1 = hasExtension(" GL_SGIX_ir_instrument1 ");
      Has_GLX_SGIX_video_resize = hasExtension(" GLX_SGIX_video_resize ");
      Has_GL_SGIX_texture_lod_bias = hasExtension(" GL_SGIX_texture_lod_bias ");
      Has_GLU_SGI_filter4_parameters = hasExtension(" GLU_SGI_filter4_parameters ");
      Has_GLX_SGIX_dm_buffer = hasExtension(" GLX_SGIX_dm_buffer ");
      Has_GL_SGIX_shadow_ambient = hasExtension(" GL_SGIX_shadow_ambient ");
      Has_GLX_SGIX_swap_group = hasExtension(" GLX_SGIX_swap_group ");
      Has_GLX_SGIX_swap_barrier = hasExtension(" GLX_SGIX_swap_barrier ");
      Has_GL_EXT_index_texture = hasExtension(" GL_EXT_index_texture ");
      Has_GL_EXT_index_material = hasExtension(" GL_EXT_index_material ");
      Has_GL_EXT_index_func = hasExtension(" GL_EXT_index_func ");
      Has_GL_EXT_index_array_formats = hasExtension(" GL_EXT_index_array_formats ");
      Has_GL_EXT_compiled_vertex_array = hasExtension(" GL_EXT_compiled_vertex_array ");
      Has_GL_EXT_cull_vertex = hasExtension(" GL_EXT_cull_vertex ");
      Has_GLU_EXT_nurbs_tessellator = hasExtension(" GLU_EXT_nurbs_tessellator ");
      Has_GL_SGIX_ycrcb = hasExtension(" GL_SGIX_ycrcb ");
      Has_GL_EXT_fragment_lighting = hasExtension(" GL_EXT_fragment_lighting ");
      Has_GL_IBM_rasterpos_clip = hasExtension(" GL_IBM_rasterpos_clip ");
      Has_GL_HP_texture_lighting = hasExtension(" GL_HP_texture_lighting ");
      Has_GL_EXT_draw_range_elements = hasExtension(" GL_EXT_draw_range_elements ");
      Has_GL_WIN_phong_shading = hasExtension(" GL_WIN_phong_shading ");
      Has_GL_WIN_specular_fog = hasExtension(" GL_WIN_specular_fog ");
      Has_GLX_SGIS_color_range = hasExtension(" GLX_SGIS_color_range ");
      Has_GL_SGIS_color_range = hasExtension(" GL_SGIS_color_range ");
      Has_GL_EXT_light_texture = hasExtension(" GL_EXT_light_texture ");
      Has_GL_SGIX_blend_alpha_minmax = hasExtension(" GL_SGIX_blend_alpha_minmax ");
      Has_GL_EXT_scene_marker = hasExtension(" GL_EXT_scene_marker ");
      Has_GLX_EXT_scene_marker = hasExtension(" GLX_EXT_scene_marker ");
      Has_GL_SGIX_pixel_texture_bits = hasExtension(" GL_SGIX_pixel_texture_bits ");
      Has_GL_EXT_bgra = hasExtension(" GL_EXT_bgra ");
      Has_GL_SGIX_async = hasExtension(" GL_SGIX_async ");
      Has_GL_SGIX_async_pixel = hasExtension(" GL_SGIX_async_pixel ");
      Has_GL_SGIX_async_histogram = hasExtension(" GL_SGIX_async_histogram ");
      Has_GL_INTEL_texture_scissor = hasExtension(" GL_INTEL_texture_scissor ");
      Has_GL_INTEL_parallel_arrays = hasExtension(" GL_INTEL_parallel_arrays ");
      Has_GL_HP_occlusion_test = hasExtension(" GL_HP_occlusion_test ");
      Has_GL_EXT_pixel_transform = hasExtension(" GL_EXT_pixel_transform ");
      Has_GL_EXT_pixel_transform_color_table = hasExtension(" GL_EXT_pixel_transform_color_table ");
      Has_GL_EXT_shared_texture_palette = hasExtension(" GL_EXT_shared_texture_palette ");
      Has_GLX_SGIS_blended_overlay = hasExtension(" GLX_SGIS_blended_overlay ");
      Has_GL_EXT_separate_specular_color = hasExtension(" GL_EXT_separate_specular_color ");
      Has_GL_EXT_secondary_color = hasExtension(" GL_EXT_secondary_color ");
      Has_GL_EXT_texture_env = hasExtension(" GL_EXT_texture_env ");
      Has_GL_EXT_texture_perturb_normal = hasExtension(" GL_EXT_texture_perturb_normal ");
      Has_GL_EXT_multi_draw_arrays = hasExtension(" GL_EXT_multi_draw_arrays ");
      Has_GL_SUN_multi_draw_arrays = hasExtension(" GL_SUN_multi_draw_arrays ");
      Has_GL_EXT_fog_coord = hasExtension(" GL_EXT_fog_coord ");
      Has_GL_REND_screen_coordinates = hasExtension(" GL_REND_screen_coordinates ");
      Has_GL_EXT_coordinate_frame = hasExtension(" GL_EXT_coordinate_frame ");
      Has_GL_EXT_texture_env_combine = hasExtension(" GL_EXT_texture_env_combine ");
      Has_GL_APPLE_specular_vector = hasExtension(" GL_APPLE_specular_vector ");
      Has_GL_APPLE_transform_hint = hasExtension(" GL_APPLE_transform_hint ");
      Has_GL_SUNX_constant_data = hasExtension(" GL_SUNX_constant_data ");
      Has_GL_SUN_global_alpha = hasExtension(" GL_SUN_global_alpha ");
      Has_GL_SUN_triangle_list = hasExtension(" GL_SUN_triangle_list ");
      Has_GL_SUN_vertex = hasExtension(" GL_SUN_vertex ");
      Has_WGL_EXT_display_color_table = hasExtension(" WGL_EXT_display_color_table ");
      Has_WGL_EXT_extensions_string = hasExtension(" WGL_EXT_extensions_string ");
      Has_WGL_EXT_make_current_read = hasExtension(" WGL_EXT_make_current_read ");
      Has_WGL_EXT_pixel_format = hasExtension(" WGL_EXT_pixel_format ");
      Has_WGL_EXT_pbuffer = hasExtension(" WGL_EXT_pbuffer ");
      Has_WGL_EXT_swap_control = hasExtension(" WGL_EXT_swap_control ");
      Has_GL_EXT_blend_func_separate = hasExtension(" GL_EXT_blend_func_separate ");
      Has_GL_INGR_color_clamp = hasExtension(" GL_INGR_color_clamp ");
      Has_GL_INGR_interlace_read = hasExtension(" GL_INGR_interlace_read ");
      Has_GL_EXT_stencil_wrap = hasExtension(" GL_EXT_stencil_wrap ");
      Has_WGL_EXT_depth_float = hasExtension(" WGL_EXT_depth_float ");
      Has_GL_EXT_422_pixels = hasExtension(" GL_EXT_422_pixels ");
      Has_GL_NV_texgen_reflection = hasExtension(" GL_NV_texgen_reflection ");
      Has_GL_SGIX_texture_range = hasExtension(" GL_SGIX_texture_range ");
      Has_GL_SUN_convolution_border_modes = hasExtension(" GL_SUN_convolution_border_modes ");
      Has_GLX_SUN_get_transparent_index = hasExtension(" GLX_SUN_get_transparent_index ");
      Has_GL_EXT_texture_env_add = hasExtension(" GL_EXT_texture_env_add ");
      Has_GL_EXT_texture_lod_bias = hasExtension(" GL_EXT_texture_lod_bias ");
      Has_GL_EXT_texture_filter_anisotropic = hasExtension(" GL_EXT_texture_filter_anisotropic ");
      Has_GL_EXT_vertex_weighting = hasExtension(" GL_EXT_vertex_weighting ");
      Has_GL_NV_light_max_exponent = hasExtension(" GL_NV_light_max_exponent ");
      Has_GL_NV_vertex_array_range = hasExtension(" GL_NV_vertex_array_range ");
      Has_GL_NV_register_combiners = hasExtension(" GL_NV_register_combiners ");
      Has_GL_NV_fog_distance = hasExtension(" GL_NV_fog_distance ");
      Has_GL_NV_texgen_emboss = hasExtension(" GL_NV_texgen_emboss ");
      Has_GL_NV_blend_square = hasExtension(" GL_NV_blend_square ");
      Has_GL_NV_texture_env_combine4 = hasExtension(" GL_NV_texture_env_combine4 ");
      Has_GL_MESA_resize_buffers = hasExtension(" GL_MESA_resize_buffers ");
      Has_GL_MESA_window_pos = hasExtension(" GL_MESA_window_pos ");
      Has_GL_EXT_texture_compression_s3tc = hasExtension(" GL_EXT_texture_compression_s3tc ");
      Has_GL_IBM_cull_vertex = hasExtension(" GL_IBM_cull_vertex ");
      Has_GL_IBM_multimode_draw_arrays = hasExtension(" GL_IBM_multimode_draw_arrays ");
      Has_GL_IBM_vertex_array_lists = hasExtension(" GL_IBM_vertex_array_lists ");
      Has_GL_3DFX_texture_compression_FXT1 = hasExtension(" GL_3DFX_texture_compression_FXT1 ");
      Has_GL_3DFX_multisample = hasExtension(" GL_3DFX_multisample ");
      Has_GL_3DFX_tbuffer = hasExtension(" GL_3DFX_tbuffer ");
      Has_WGL_EXT_multisample = hasExtension(" WGL_EXT_multisample ");
      Has_GL_EXT_multisample = hasExtension(" GL_EXT_multisample ");
      Has_GL_SGIX_vertex_preclip = hasExtension(" GL_SGIX_vertex_preclip ");
      Has_GL_SGIX_vertex_preclip_hint = hasExtension(" GL_SGIX_vertex_preclip_hint ");
      Has_GL_SGIX_convolution_accuracy = hasExtension(" GL_SGIX_convolution_accuracy ");
      Has_GL_SGIX_resample = hasExtension(" GL_SGIX_resample ");
      Has_GL_SGIS_point_line_texgen = hasExtension(" GL_SGIS_point_line_texgen ");
      Has_GL_SGIS_texture_color_mask = hasExtension(" GL_SGIS_texture_color_mask ");
      Has_GLX_MESA_copy_sub_buffer = hasExtension(" GLX_MESA_copy_sub_buffer ");
      Has_GLX_MESA_pixmap_colormap = hasExtension(" GLX_MESA_pixmap_colormap ");
      Has_GLX_MESA_release_buffers = hasExtension(" GLX_MESA_release_buffers ");
      Has_GLX_MESA_set_3dfx_mode = hasExtension(" GLX_MESA_set_3dfx_mode ");
      Has_GL_EXT_texture_env_dot3 = hasExtension(" GL_EXT_texture_env_dot3 ");
      Has_GL_ATI_texture_mirror_once = hasExtension(" GL_ATI_texture_mirror_once ");
      Has_GL_NV_fence = hasExtension(" GL_NV_fence ");
      Has_GL_IBM_static_data = hasExtension(" GL_IBM_static_data ");
      Has_GL_IBM_texture_mirrored_repeat = hasExtension(" GL_IBM_texture_mirrored_repeat ");
      Has_GL_NV_evaluators = hasExtension(" GL_NV_evaluators ");
      Has_GL_NV_packed_depth_stencil = hasExtension(" GL_NV_packed_depth_stencil ");
      Has_GL_NV_register_combiners2 = hasExtension(" GL_NV_register_combiners2 ");
      Has_GL_NV_texture_compression_vtc = hasExtension(" GL_NV_texture_compression_vtc ");
      Has_GL_NV_texture_rectangle = hasExtension(" GL_NV_texture_rectangle ");
      Has_GL_NV_texture_shader = hasExtension(" GL_NV_texture_shader ");
      Has_GL_NV_texture_shader2 = hasExtension(" GL_NV_texture_shader2 ");
      Has_GL_NV_vertex_array_range2 = hasExtension(" GL_NV_vertex_array_range2 ");
      Has_GL_NV_vertex_program = hasExtension(" GL_NV_vertex_program ");
      Has_GLX_SGIX_visual_select_group = hasExtension(" GLX_SGIX_visual_select_group ");
      Has_GL_SGIX_texture_coordinate_clamp = hasExtension(" GL_SGIX_texture_coordinate_clamp ");
      Has_GLX_OML_swap_method = hasExtension(" GLX_OML_swap_method ");
      Has_GLX_OML_sync_control = hasExtension(" GLX_OML_sync_control ");
      Has_GL_OML_interlace = hasExtension(" GL_OML_interlace ");
      Has_GL_OML_subsample = hasExtension(" GL_OML_subsample ");
      Has_GL_OML_resample = hasExtension(" GL_OML_resample ");
      Has_WGL_OML_sync_control = hasExtension(" WGL_OML_sync_control ");
      Has_GL_NV_copy_depth_to_color = hasExtension(" GL_NV_copy_depth_to_color ");
      Has_GL_ATI_envmap_bumpmap = hasExtension(" GL_ATI_envmap_bumpmap ");
      Has_GL_ATI_fragment_shader = hasExtension(" GL_ATI_fragment_shader ");
      Has_GL_ATI_pn_triangles = hasExtension(" GL_ATI_pn_triangles ");
      Has_GL_ATI_vertex_array_object = hasExtension(" GL_ATI_vertex_array_object ");
      Has_GL_EXT_vertex_shader = hasExtension(" GL_EXT_vertex_shader ");
      Has_GL_ATI_vertex_streams = hasExtension(" GL_ATI_vertex_streams ");
      Has_WGL_I3D_digital_video_control = hasExtension(" WGL_I3D_digital_video_control ");
      Has_WGL_I3D_gamma = hasExtension(" WGL_I3D_gamma ");
      Has_WGL_I3D_genlock = hasExtension(" WGL_I3D_genlock ");
      Has_WGL_I3D_image_buffer = hasExtension(" WGL_I3D_image_buffer ");
      Has_WGL_I3D_swap_frame_lock = hasExtension(" WGL_I3D_swap_frame_lock ");
      Has_WGL_I3D_swap_frame_usage = hasExtension(" WGL_I3D_swap_frame_usage ");
      Has_GL_ATI_element_array = hasExtension(" GL_ATI_element_array ");
      Has_GL_SUN_mesh_array = hasExtension(" GL_SUN_mesh_array ");
      Has_GL_SUN_slice_accum = hasExtension(" GL_SUN_slice_accum ");
      Has_GL_NV_multisample_filter_hint = hasExtension(" GL_NV_multisample_filter_hint ");
      Has_GL_NV_depth_clamp = hasExtension(" GL_NV_depth_clamp ");
      Has_GL_NV_occlusion_query = hasExtension(" GL_NV_occlusion_query ");
      Has_GL_NV_point_sprite = hasExtension(" GL_NV_point_sprite ");
      Has_WGL_NV_render_depth_texture = hasExtension(" WGL_NV_render_depth_texture ");
      Has_WGL_NV_render_texture_rectangle = hasExtension(" WGL_NV_render_texture_rectangle ");
      Has_GL_NV_texture_shader3 = hasExtension(" GL_NV_texture_shader3 ");
      Has_GL_NV_vertex_program1_1 = hasExtension(" GL_NV_vertex_program1_1 ");
      Has_GL_EXT_shadow_funcs = hasExtension(" GL_EXT_shadow_funcs ");
      Has_GL_EXT_stencil_two_side = hasExtension(" GL_EXT_stencil_two_side ");
      Has_GL_ATI_text_fragment_shader = hasExtension(" GL_ATI_text_fragment_shader ");
      Has_GL_APPLE_client_storage = hasExtension(" GL_APPLE_client_storage ");
      Has_GL_APPLE_element_array = hasExtension(" GL_APPLE_element_array ");
      Has_GL_APPLE_fence = hasExtension(" GL_APPLE_fence ");
      Has_GL_APPLE_vertex_array_object = hasExtension(" GL_APPLE_vertex_array_object ");
      Has_GL_APPLE_vertex_array_range = hasExtension(" GL_APPLE_vertex_array_range ");
      Has_GL_APPLE_ycbcr_422 = hasExtension(" GL_APPLE_ycbcr_422 ");
      Has_GL_S3_s3tc = hasExtension(" GL_S3_s3tc ");
      Has_GL_ATI_draw_buffers = hasExtension(" GL_ATI_draw_buffers ");
      Has_WGL_ATI_pixel_format_float = hasExtension(" WGL_ATI_pixel_format_float ");
      Has_GL_ATI_texture_env_combine3 = hasExtension(" GL_ATI_texture_env_combine3 ");
      Has_GL_ATI_texture_float = hasExtension(" GL_ATI_texture_float ");
      Has_GL_NV_float_buffer = hasExtension(" GL_NV_float_buffer ");
      Has_WGL_NV_float_buffer = hasExtension(" WGL_NV_float_buffer ");
      Has_GL_NV_fragment_program = hasExtension(" GL_NV_fragment_program ");
      Has_GL_NV_half_float = hasExtension(" GL_NV_half_float ");
      Has_GL_NV_pixel_data_range = hasExtension(" GL_NV_pixel_data_range ");
      Has_GL_NV_primitive_restart = hasExtension(" GL_NV_primitive_restart ");
      Has_GL_NV_texture_expand_normal = hasExtension(" GL_NV_texture_expand_normal ");
      Has_GL_NV_vertex_program2 = hasExtension(" GL_NV_vertex_program2 ");
      Has_GL_ATI_map_object_buffer = hasExtension(" GL_ATI_map_object_buffer ");
      Has_GL_ATI_separate_stencil = hasExtension(" GL_ATI_separate_stencil ");
      Has_GL_ATI_vertex_attrib_array_object = hasExtension(" GL_ATI_vertex_attrib_array_object ");
      Has_GL_OES_byte_coordinates = hasExtension(" GL_OES_byte_coordinates ");
      Has_GL_OES_fixed_point = hasExtension(" GL_OES_fixed_point ");
      Has_GL_OES_single_precision = hasExtension(" GL_OES_single_precision ");
      Has_GL_OES_compressed_paletted_texture = hasExtension(" GL_OES_compressed_paletted_texture ");
      Has_GL_OES_read_format = hasExtension(" GL_OES_read_format ");
      Has_GL_OES_query_matrix = hasExtension(" GL_OES_query_matrix ");
      Has_GL_EXT_depth_bounds_test = hasExtension(" GL_EXT_depth_bounds_test ");
      Has_GL_EXT_texture_mirror_clamp = hasExtension(" GL_EXT_texture_mirror_clamp ");
      Has_GL_EXT_blend_equation_separate = hasExtension(" GL_EXT_blend_equation_separate ");
      Has_GL_MESA_pack_invert = hasExtension(" GL_MESA_pack_invert ");
      Has_GL_MESA_ycbcr_texture = hasExtension(" GL_MESA_ycbcr_texture ");
      Has_GL_EXT_pixel_buffer_object = hasExtension(" GL_EXT_pixel_buffer_object ");
      Has_GL_NV_fragment_program_option = hasExtension(" GL_NV_fragment_program_option ");
      Has_GL_NV_fragment_program2 = hasExtension(" GL_NV_fragment_program2 ");
      Has_GL_NV_vertex_program2_option = hasExtension(" GL_NV_vertex_program2_option ");
      Has_GL_NV_vertex_program3 = hasExtension(" GL_NV_vertex_program3 ");
      Has_GLX_SGIX_hyperpipe = hasExtension(" GLX_SGIX_hyperpipe ");
      Has_GLX_MESA_agp_offset = hasExtension(" GLX_MESA_agp_offset ");
      Has_GL_EXT_texture_compression_dxt1 = hasExtension(" GL_EXT_texture_compression_dxt1 ");
      Has_GL_EXT_framebuffer_object = hasExtension(" GL_EXT_framebuffer_object ");
      Has_GL_GREMEDY_string_marker = hasExtension(" GL_GREMEDY_string_marker ");
      Has_GL_EXT_packed_depth_stencil = hasExtension(" GL_EXT_packed_depth_stencil ");
      Has_WGL_3DL_stereo_control = hasExtension(" WGL_3DL_stereo_control ");
      Has_GL_EXT_stencil_clear_tag = hasExtension(" GL_EXT_stencil_clear_tag ");
      Has_GL_EXT_texture_sRGB = hasExtension(" GL_EXT_texture_sRGB ");
      Has_GL_EXT_framebuffer_blit = hasExtension(" GL_EXT_framebuffer_blit ");
      Has_GL_EXT_framebuffer_multisample = hasExtension(" GL_EXT_framebuffer_multisample ");
      Has_GL_MESAX_texture_stack = hasExtension(" GL_MESAX_texture_stack ");
      Has_GL_EXT_timer_query = hasExtension(" GL_EXT_timer_query ");
      Has_GL_EXT_gpu_program_parameters = hasExtension(" GL_EXT_gpu_program_parameters ");
      Has_GL_APPLE_flush_buffer_range = hasExtension(" GL_APPLE_flush_buffer_range ");
      Has_GL_NV_gpu_program4 = hasExtension(" GL_NV_gpu_program4 ");
      Has_GL_NV_geometry_program4 = hasExtension(" GL_NV_geometry_program4 ");
      Has_GL_EXT_geometry_shader4 = hasExtension(" GL_EXT_geometry_shader4 ");
      Has_GL_NV_vertex_program4 = hasExtension(" GL_NV_vertex_program4 ");
      Has_GL_EXT_gpu_shader4 = hasExtension(" GL_EXT_gpu_shader4 ");
      Has_GL_EXT_draw_instanced = hasExtension(" GL_EXT_draw_instanced ");
      Has_GL_EXT_packed_float = hasExtension(" GL_EXT_packed_float ");
      Has_WGL_EXT_pixel_format_packed_float = hasExtension(" WGL_EXT_pixel_format_packed_float ");
      Has_GLX_EXT_fbconfig_packed_float = hasExtension(" GLX_EXT_fbconfig_packed_float ");
      Has_GL_EXT_texture_array = hasExtension(" GL_EXT_texture_array ");
      Has_GL_EXT_texture_buffer_object = hasExtension(" GL_EXT_texture_buffer_object ");
      Has_GL_EXT_texture_compression_latc = hasExtension(" GL_EXT_texture_compression_latc ");
      Has_GL_EXT_texture_compression_rgtc = hasExtension(" GL_EXT_texture_compression_rgtc ");
      Has_GL_EXT_texture_shared_exponent = hasExtension(" GL_EXT_texture_shared_exponent ");
      Has_GL_NV_depth_buffer_float = hasExtension(" GL_NV_depth_buffer_float ");
      Has_GL_NV_fragment_program4 = hasExtension(" GL_NV_fragment_program4 ");
      Has_GL_NV_framebuffer_multisample_coverage = hasExtension(" GL_NV_framebuffer_multisample_coverage ");
      Has_GL_EXT_framebuffer_sRGB = hasExtension(" GL_EXT_framebuffer_sRGB ");
      Has_GLX_EXT_framebuffer_sRGB = hasExtension(" GLX_EXT_framebuffer_sRGB ");
      Has_WGL_EXT_framebuffer_sRGB = hasExtension(" WGL_EXT_framebuffer_sRGB ");
      Has_GL_NV_geometry_shader4 = hasExtension(" GL_NV_geometry_shader4 ");
      Has_GL_NV_parameter_buffer_object = hasExtension(" GL_NV_parameter_buffer_object ");
      Has_GL_EXT_draw_buffers2 = hasExtension(" GL_EXT_draw_buffers2 ");
      Has_GL_NV_transform_feedback = hasExtension(" GL_NV_transform_feedback ");
      Has_GL_EXT_bindable_uniform = hasExtension(" GL_EXT_bindable_uniform ");
      Has_GL_EXT_texture_integer = hasExtension(" GL_EXT_texture_integer ");
      Has_GLX_EXT_texture_from_pixmap = hasExtension(" GLX_EXT_texture_from_pixmap ");
      Has_GL_GREMEDY_frame_terminator = hasExtension(" GL_GREMEDY_frame_terminator ");
      Has_GL_NV_conditional_render = hasExtension(" GL_NV_conditional_render ");
      Has_GL_NV_present_video = hasExtension(" GL_NV_present_video ");
      Has_GLX_NV_present_video = hasExtension(" GLX_NV_present_video ");
      Has_WGL_NV_present_video = hasExtension(" WGL_NV_present_video ");
      Has_GLX_NV_video_output = hasExtension(" GLX_NV_video_output ");
      Has_WGL_NV_video_output = hasExtension(" WGL_NV_video_output ");
      Has_GLX_NV_swap_group = hasExtension(" GLX_NV_swap_group ");
      Has_WGL_NV_swap_group = hasExtension(" WGL_NV_swap_group ");
      Has_GL_EXT_transform_feedback = hasExtension(" GL_EXT_transform_feedback ");
      Has_GL_EXT_direct_state_access = hasExtension(" GL_EXT_direct_state_access ");
      Has_GL_EXT_vertex_array_bgra = hasExtension(" GL_EXT_vertex_array_bgra ");
      Has_WGL_NV_gpu_affinity = hasExtension(" WGL_NV_gpu_affinity ");
      Has_GL_EXT_texture_swizzle = hasExtension(" GL_EXT_texture_swizzle ");
      Has_GL_NV_explicit_multisample = hasExtension(" GL_NV_explicit_multisample ");
      Has_GL_NV_transform_feedback2 = hasExtension(" GL_NV_transform_feedback2 ");
      Has_GL_ATI_meminfo = hasExtension(" GL_ATI_meminfo ");
      Has_GL_AMD_performance_monitor = hasExtension(" GL_AMD_performance_monitor ");
      Has_WGL_AMD_gpu_association = hasExtension(" WGL_AMD_gpu_association ");
      Has_GL_AMD_texture_texture4 = hasExtension(" GL_AMD_texture_texture4 ");
      Has_GL_AMD_vertex_shader_tessellator = hasExtension(" GL_AMD_vertex_shader_tessellator ");
      Has_GL_EXT_provoking_vertex = hasExtension(" GL_EXT_provoking_vertex ");
      Has_GL_EXT_texture_snorm = hasExtension(" GL_EXT_texture_snorm ");
      Has_GL_AMD_draw_buffers_blend = hasExtension(" GL_AMD_draw_buffers_blend ");
      Has_GL_APPLE_texture_range = hasExtension(" GL_APPLE_texture_range ");
      Has_GL_APPLE_float_pixels = hasExtension(" GL_APPLE_float_pixels ");
      Has_GL_APPLE_vertex_program_evaluators = hasExtension(" GL_APPLE_vertex_program_evaluators ");
      Has_GL_APPLE_aux_depth_stencil = hasExtension(" GL_APPLE_aux_depth_stencil ");
      Has_GL_APPLE_object_purgeable = hasExtension(" GL_APPLE_object_purgeable ");
      Has_GL_APPLE_row_bytes = hasExtension(" GL_APPLE_row_bytes ");
      Has_GL_APPLE_rgb_422 = hasExtension(" GL_APPLE_rgb_422 ");
      Has_GL_NV_video_capture = hasExtension(" GL_NV_video_capture ");
      Has_GLX_NV_video_capture = hasExtension(" GLX_NV_video_capture ");
      Has_WGL_NV_video_capture = hasExtension(" WGL_NV_video_capture ");
      Has_GL_EXT_swap_control = hasExtension(" GL_EXT_swap_control ");
      Has_GL_NV_copy_image = hasExtension(" GL_NV_copy_image ");
      Has_WGL_NV_copy_image = hasExtension(" WGL_NV_copy_image ");
      Has_GLX_NV_copy_image = hasExtension(" GLX_NV_copy_image ");
      Has_GL_EXT_separate_shader_objects = hasExtension(" GL_EXT_separate_shader_objects ");
      Has_GL_NV_parameter_buffer_object2 = hasExtension(" GL_NV_parameter_buffer_object2 ");
      Has_GL_NV_shader_buffer_load = hasExtension(" GL_NV_shader_buffer_load ");
      Has_GL_NV_vertex_buffer_unified_memory = hasExtension(" GL_NV_vertex_buffer_unified_memory ");
      Has_GL_NV_texture_barrier = hasExtension(" GL_NV_texture_barrier ");
      Has_GL_AMD_shader_stencil_export = hasExtension(" GL_AMD_shader_stencil_export ");
      Has_GL_AMD_seamless_cubemap_per_texture = hasExtension(" GL_AMD_seamless_cubemap_per_texture ");
      Has_GLX_INTEL_swap_event = hasExtension(" GLX_INTEL_swap_event ");
      Has_GL_AMD_conservative_depth = hasExtension(" GL_AMD_conservative_depth ");
      Has_GL_EXT_shader_image_load_store = hasExtension(" GL_EXT_shader_image_load_store ");
      Has_GL_EXT_vertex_attrib_64bit = hasExtension(" GL_EXT_vertex_attrib_64bit ");
      Has_GL_NV_gpu_program5 = hasExtension(" GL_NV_gpu_program5 ");
      Has_GL_NV_gpu_shader5 = hasExtension(" GL_NV_gpu_shader5 ");
      Has_GL_NV_shader_buffer_store = hasExtension(" GL_NV_shader_buffer_store ");
      Has_GL_NV_tessellation_program5 = hasExtension(" GL_NV_tessellation_program5 ");
      Has_GL_NV_vertex_attrib_integer_64bit = hasExtension(" GL_NV_vertex_attrib_integer_64bit ");
      Has_GL_NV_multisample_coverage = hasExtension(" GL_NV_multisample_coverage ");
      Has_GL_AMD_name_gen_delete = hasExtension(" GL_AMD_name_gen_delete ");
      Has_GL_AMD_debug_output = hasExtension(" GL_AMD_debug_output ");
      Has_GL_NV_vdpau_interop = hasExtension(" GL_NV_vdpau_interop ");
      Has_GL_AMD_transform_feedback3_lines_triangles = hasExtension(" GL_AMD_transform_feedback3_lines_triangles ");
      Has_GLX_AMD_gpu_association = hasExtension(" GLX_AMD_gpu_association ");
      Has_GLX_EXT_create_context_es2_profile = hasExtension(" GLX_EXT_create_context_es2_profile ");
      Has_WGL_EXT_create_context_es2_profile = hasExtension(" WGL_EXT_create_context_es2_profile ");
      Has_GL_AMD_depth_clamp_separate = hasExtension(" GL_AMD_depth_clamp_separate ");
      Has_GL_EXT_texture_sRGB_decode = hasExtension(" GL_EXT_texture_sRGB_decode ");

      return initFunctions();
    }

    const void* getProcAddress(const char* func)
    {
      return NULL;
    }

    const char* initFunctions()
    {

      return NULL;
    }
  };

  bool Has_GL_VERSION_1_0 = false;
  bool Has_GL_VERSION_1_1 = false;
  bool Has_GL_VERSION_1_2 = false;
  bool Has_GL_VERSION_1_3 = false;
  bool Has_GL_VERSION_1_4 = false;
  bool Has_GL_VERSION_1_5 = false;
  bool Has_GL_VERSION_2_0 = false;
  bool Has_GL_VERSION_2_1 = false;
  bool Has_GL_VERSION_3_0 = false;
  bool Has_GL_VERSION_3_1 = false;
  bool Has_GL_VERSION_3_2 = false;
  bool Has_GL_VERSION_3_3 = false;
  bool Has_GL_VERSION_4_0 = false;
  bool Has_GL_VERSION_4_1 = false;

  bool Has_GL_ARB_imaging = false;
  bool Has_GL_ARB_multitexture = false;
  bool Has_GLX_ARB_get_proc_address = false;
  bool Has_GL_ARB_transpose_matrix = false;
  bool Has_WGL_ARB_buffer_region = false;
  bool Has_GL_ARB_multisample = false;
  bool Has_GLX_ARB_multisample = false;
  bool Has_WGL_ARB_multisample = false;
  bool Has_GL_ARB_texture_env_add = false;
  bool Has_GL_ARB_texture_cube_map = false;
  bool Has_WGL_ARB_extensions_string = false;
  bool Has_WGL_ARB_pixel_format = false;
  bool Has_WGL_ARB_make_current_read = false;
  bool Has_WGL_ARB_pbuffer = false;
  bool Has_GL_ARB_texture_compression = false;
  bool Has_GL_ARB_texture_border_clamp = false;
  bool Has_GL_ARB_point_parameters = false;
  bool Has_GL_ARB_vertex_blend = false;
  bool Has_GL_ARB_matrix_palette = false;
  bool Has_GL_ARB_texture_env_combine = false;
  bool Has_GL_ARB_texture_env_crossbar = false;
  bool Has_GL_ARB_texture_env_dot3 = false;
  bool Has_WGL_ARB_render_texture = false;
  bool Has_GL_ARB_texture_mirrored_repeat = false;
  bool Has_GL_ARB_depth_texture = false;
  bool Has_GL_ARB_shadow = false;
  bool Has_GL_ARB_shadow_ambient = false;
  bool Has_GL_ARB_window_pos = false;
  bool Has_GL_ARB_vertex_program = false;
  bool Has_GL_ARB_fragment_program = false;
  bool Has_GL_ARB_vertex_buffer_object = false;
  bool Has_GL_ARB_occlusion_query = false;
  bool Has_GL_ARB_shader_objects = false;
  bool Has_GL_ARB_vertex_shader = false;
  bool Has_GL_ARB_fragment_shader = false;
  bool Has_GL_ARB_shading_language_100 = false;
  bool Has_GL_ARB_texture_non_power_of_two = false;
  bool Has_GL_ARB_point_sprite = false;
  bool Has_GL_ARB_fragment_program_shadow = false;
  bool Has_GL_ARB_draw_buffers = false;
  bool Has_GL_ARB_texture_rectangle = false;
  bool Has_GL_ARB_color_buffer_float = false;
  bool Has_WGL_ARB_pixel_format_float = false;
  bool Has_GLX_ARB_fbconfig_float = false;
  bool Has_GL_ARB_half_float_pixel = false;
  bool Has_GL_ARB_texture_float = false;
  bool Has_GL_ARB_pixel_buffer_object = false;
  bool Has_GL_ARB_depth_buffer_float = false;
  bool Has_GL_ARB_draw_instanced = false;
  bool Has_GL_ARB_framebuffer_object = false;
  bool Has_GL_ARB_framebuffer_sRGB = false;
  bool Has_GLX_ARB_framebuffer_sRGB = false;
  bool Has_WGL_ARB_framebuffer_sRGB = false;
  bool Has_GL_ARB_geometry_shader4 = false;
  bool Has_GL_ARB_half_float_vertex = false;
  bool Has_GL_ARB_instanced_arrays = false;
  bool Has_GL_ARB_map_buffer_range = false;
  bool Has_GL_ARB_texture_buffer_object = false;
  bool Has_GL_ARB_texture_compression_rgtc = false;
  bool Has_GL_ARB_texture_rg = false;
  bool Has_GL_ARB_vertex_array_object = false;
  bool Has_WGL_ARB_create_context = false;
  bool Has_GLX_ARB_create_context = false;
  bool Has_GL_ARB_uniform_buffer_object = false;
  bool Has_GL_ARB_compatibility = false;
  bool Has_GL_ARB_copy_buffer = false;
  bool Has_GL_ARB_shader_texture_lod = false;
  bool Has_GL_ARB_depth_clamp = false;
  bool Has_GL_ARB_draw_elements_base_vertex = false;
  bool Has_GL_ARB_fragment_coord_conventions = false;
  bool Has_GL_ARB_provoking_vertex = false;
  bool Has_GL_ARB_seamless_cube_map = false;
  bool Has_GL_ARB_sync = false;
  bool Has_GL_ARB_texture_multisample = false;
  bool Has_GL_ARB_vertex_array_bgra = false;
  bool Has_GL_ARB_draw_buffers_blend = false;
  bool Has_GL_ARB_sample_shading = false;
  bool Has_GL_ARB_texture_cube_map_array = false;
  bool Has_GL_ARB_texture_gather = false;
  bool Has_GL_ARB_texture_query_lod = false;
  bool Has_WGL_ARB_create_context_profile = false;
  bool Has_GLX_ARB_create_context_profile = false;
  bool Has_GL_ARB_shading_language_include = false;
  bool Has_GL_ARB_texture_compression_bptc = false;
  bool Has_GL_ARB_blend_func_extended = false;
  bool Has_GL_ARB_explicit_attrib_location = false;
  bool Has_GL_ARB_occlusion_query2 = false;
  bool Has_GL_ARB_sampler_objects = false;
  bool Has_GL_ARB_shader_bit_encoding = false;
  bool Has_GL_ARB_texture_rgb10_a2ui = false;
  bool Has_GL_ARB_texture_swizzle = false;
  bool Has_GL_ARB_timer_query = false;
  bool Has_GL_ARB_vertex_type_2_10_10_10_rev = false;
  bool Has_GL_ARB_draw_indirect = false;
  bool Has_GL_ARB_gpu_shader5 = false;
  bool Has_GL_ARB_gpu_shader_fp64 = false;
  bool Has_GL_ARB_shader_subroutine = false;
  bool Has_GL_ARB_tessellation_shader = false;
  bool Has_GL_ARB_texture_buffer_object_rgb32 = false;
  bool Has_GL_ARB_transform_feedback2 = false;
  bool Has_GL_ARB_transform_feedback3 = false;
  bool Has_GL_ARB_ES2_compatibility = false;
  bool Has_GL_ARB_get_program_binary = false;
  bool Has_GL_ARB_separate_shader_objects = false;
  bool Has_GL_ARB_shader_precision = false;
  bool Has_GL_ARB_vertex_attrib_64bit = false;
  bool Has_GL_ARB_viewport_array = false;
  bool Has_GLX_ARB_create_context_robustness = false;
  bool Has_WGL_ARB_create_context_robustness = false;
  bool Has_GL_ARB_cl_event = false;
  bool Has_GL_ARB_debug_output = false;
  bool Has_GL_ARB_robustness = false;
  bool Has_GL_ARB_shader_stencil_export = false;

  // Vendor and EXT Extensions

  bool Has_GL_EXT_abgr = false;
  bool Has_GL_EXT_blend_color = false;
  bool Has_GL_EXT_polygon_offset = false;
  bool Has_GL_EXT_texture = false;
  bool Has_GL_EXT_texture3D = false;
  bool Has_GL_SGIS_texture_filter4 = false;
  bool Has_GL_EXT_subtexture = false;
  bool Has_GL_EXT_copy_texture = false;
  bool Has_GL_EXT_histogram = false;
  bool Has_GL_EXT_convolution = false;
  bool Has_GL_SGI_color_matrix = false;
  bool Has_GL_SGI_color_table = false;
  bool Has_GL_SGIS_pixel_texture = false;
  bool Has_GL_SGIX_pixel_texture = false;
  bool Has_GL_SGIS_texture4D = false;
  bool Has_GL_SGI_texture_color_table = false;
  bool Has_GL_EXT_cmyka = false;
  bool Has_GL_EXT_texture_object = false;
  bool Has_GL_SGIS_detail_texture = false;
  bool Has_GL_SGIS_sharpen_texture = false;
  bool Has_GL_EXT_packed_pixels = false;
  bool Has_GL_SGIS_texture_lod = false;
  bool Has_GL_SGIS_multisample = false;
  bool Has_GLX_SGIS_multisample = false;
  bool Has_GL_EXT_rescale_normal = false;
  bool Has_GLX_EXT_visual_info = false;
  bool Has_GL_EXT_vertex_array = false;
  bool Has_GL_EXT_misc_attribute = false;
  bool Has_GL_SGIS_generate_mipmap = false;
  bool Has_GL_SGIX_clipmap = false;
  bool Has_GL_SGIX_shadow = false;
  bool Has_GL_SGIS_texture_edge_clamp = false;
  bool Has_GL_SGIS_texture_border_clamp = false;
  bool Has_GL_EXT_blend_minmax = false;
  bool Has_GL_EXT_blend_subtract = false;
  bool Has_GL_EXT_blend_logic_op = false;
  bool Has_GLX_SGI_swap_control = false;
  bool Has_GLX_SGI_video_sync = false;
  bool Has_GLX_SGI_make_current_read = false;
  bool Has_GLX_SGIX_video_source = false;
  bool Has_GLX_EXT_visual_rating = false;
  bool Has_GL_SGIX_interlace = false;
  bool Has_GLX_EXT_import_context = false;
  bool Has_GLX_SGIX_fbconfig = false;
  bool Has_GLX_SGIX_pbuffer = false;
  bool Has_GL_SGIS_texture_select = false;
  bool Has_GL_SGIX_sprite = false;
  bool Has_GL_SGIX_texture_multi_buffer = false;
  bool Has_GL_EXT_point_parameters = false;
  bool Has_GL_SGIX_instruments = false;
  bool Has_GL_SGIX_texture_scale_bias = false;
  bool Has_GL_SGIX_framezoom = false;
  bool Has_GL_SGIX_tag_sample_buffer = false;
  bool Has_GL_SGIX_reference_plane = false;
  bool Has_GL_SGIX_flush_raster = false;
  bool Has_GLX_SGI_cushion = false;
  bool Has_GL_SGIX_depth_texture = false;
  bool Has_GL_SGIS_fog_function = false;
  bool Has_GL_SGIX_fog_offset = false;
  bool Has_GL_HP_image_transform = false;
  bool Has_GL_HP_convolution_border_modes = false;
  bool Has_GL_SGIX_texture_add_env = false;
  bool Has_GL_EXT_color_subtable = false;
  bool Has_GLU_EXT_object_space_tess = false;
  bool Has_GL_PGI_vertex_hints = false;
  bool Has_GL_PGI_misc_hints = false;
  bool Has_GL_EXT_paletted_texture = false;
  bool Has_GL_EXT_clip_volume_hint = false;
  bool Has_GL_SGIX_list_priority = false;
  bool Has_GL_SGIX_ir_instrument1 = false;
  bool Has_GLX_SGIX_video_resize = false;
  bool Has_GL_SGIX_texture_lod_bias = false;
  bool Has_GLU_SGI_filter4_parameters = false;
  bool Has_GLX_SGIX_dm_buffer = false;
  bool Has_GL_SGIX_shadow_ambient = false;
  bool Has_GLX_SGIX_swap_group = false;
  bool Has_GLX_SGIX_swap_barrier = false;
  bool Has_GL_EXT_index_texture = false;
  bool Has_GL_EXT_index_material = false;
  bool Has_GL_EXT_index_func = false;
  bool Has_GL_EXT_index_array_formats = false;
  bool Has_GL_EXT_compiled_vertex_array = false;
  bool Has_GL_EXT_cull_vertex = false;
  bool Has_GLU_EXT_nurbs_tessellator = false;
  bool Has_GL_SGIX_ycrcb = false;
  bool Has_GL_EXT_fragment_lighting = false;
  bool Has_GL_IBM_rasterpos_clip = false;
  bool Has_GL_HP_texture_lighting = false;
  bool Has_GL_EXT_draw_range_elements = false;
  bool Has_GL_WIN_phong_shading = false;
  bool Has_GL_WIN_specular_fog = false;
  bool Has_GLX_SGIS_color_range = false;
  bool Has_GL_SGIS_color_range = false;
  bool Has_GL_EXT_light_texture = false;
  bool Has_GL_SGIX_blend_alpha_minmax = false;
  bool Has_GL_EXT_scene_marker = false;
  bool Has_GLX_EXT_scene_marker = false;
  bool Has_GL_SGIX_pixel_texture_bits = false;
  bool Has_GL_EXT_bgra = false;
  bool Has_GL_SGIX_async = false;
  bool Has_GL_SGIX_async_pixel = false;
  bool Has_GL_SGIX_async_histogram = false;
  bool Has_GL_INTEL_texture_scissor = false;
  bool Has_GL_INTEL_parallel_arrays = false;
  bool Has_GL_HP_occlusion_test = false;
  bool Has_GL_EXT_pixel_transform = false;
  bool Has_GL_EXT_pixel_transform_color_table = false;
  bool Has_GL_EXT_shared_texture_palette = false;
  bool Has_GLX_SGIS_blended_overlay = false;
  bool Has_GL_EXT_separate_specular_color = false;
  bool Has_GL_EXT_secondary_color = false;
  bool Has_GL_EXT_texture_env = false;
  bool Has_GL_EXT_texture_perturb_normal = false;
  bool Has_GL_EXT_multi_draw_arrays = false;
  bool Has_GL_SUN_multi_draw_arrays = false;
  bool Has_GL_EXT_fog_coord = false;
  bool Has_GL_REND_screen_coordinates = false;
  bool Has_GL_EXT_coordinate_frame = false;
  bool Has_GL_EXT_texture_env_combine = false;
  bool Has_GL_APPLE_specular_vector = false;
  bool Has_GL_APPLE_transform_hint = false;
  bool Has_GL_SUNX_constant_data = false;
  bool Has_GL_SUN_global_alpha = false;
  bool Has_GL_SUN_triangle_list = false;
  bool Has_GL_SUN_vertex = false;
  bool Has_WGL_EXT_display_color_table = false;
  bool Has_WGL_EXT_extensions_string = false;
  bool Has_WGL_EXT_make_current_read = false;
  bool Has_WGL_EXT_pixel_format = false;
  bool Has_WGL_EXT_pbuffer = false;
  bool Has_WGL_EXT_swap_control = false;
  bool Has_GL_EXT_blend_func_separate = false;
  bool Has_GL_INGR_color_clamp = false;
  bool Has_GL_INGR_interlace_read = false;
  bool Has_GL_EXT_stencil_wrap = false;
  bool Has_WGL_EXT_depth_float = false;
  bool Has_GL_EXT_422_pixels = false;
  bool Has_GL_NV_texgen_reflection = false;
  bool Has_GL_SGIX_texture_range = false;
  bool Has_GL_SUN_convolution_border_modes = false;
  bool Has_GLX_SUN_get_transparent_index = false;
  bool Has_GL_EXT_texture_env_add = false;
  bool Has_GL_EXT_texture_lod_bias = false;
  bool Has_GL_EXT_texture_filter_anisotropic = false;
  bool Has_GL_EXT_vertex_weighting = false;
  bool Has_GL_NV_light_max_exponent = false;
  bool Has_GL_NV_vertex_array_range = false;
  bool Has_GL_NV_register_combiners = false;
  bool Has_GL_NV_fog_distance = false;
  bool Has_GL_NV_texgen_emboss = false;
  bool Has_GL_NV_blend_square = false;
  bool Has_GL_NV_texture_env_combine4 = false;
  bool Has_GL_MESA_resize_buffers = false;
  bool Has_GL_MESA_window_pos = false;
  bool Has_GL_EXT_texture_compression_s3tc = false;
  bool Has_GL_IBM_cull_vertex = false;
  bool Has_GL_IBM_multimode_draw_arrays = false;
  bool Has_GL_IBM_vertex_array_lists = false;
  bool Has_GL_3DFX_texture_compression_FXT1 = false;
  bool Has_GL_3DFX_multisample = false;
  bool Has_GL_3DFX_tbuffer = false;
  bool Has_WGL_EXT_multisample = false;
  bool Has_GL_EXT_multisample = false;
  bool Has_GL_SGIX_vertex_preclip = false;
  bool Has_GL_SGIX_vertex_preclip_hint = false;
  bool Has_GL_SGIX_convolution_accuracy = false;
  bool Has_GL_SGIX_resample = false;
  bool Has_GL_SGIS_point_line_texgen = false;
  bool Has_GL_SGIS_texture_color_mask = false;
  bool Has_GLX_MESA_copy_sub_buffer = false;
  bool Has_GLX_MESA_pixmap_colormap = false;
  bool Has_GLX_MESA_release_buffers = false;
  bool Has_GLX_MESA_set_3dfx_mode = false;
  bool Has_GL_EXT_texture_env_dot3 = false;
  bool Has_GL_ATI_texture_mirror_once = false;
  bool Has_GL_NV_fence = false;
  bool Has_GL_IBM_static_data = false;
  bool Has_GL_IBM_texture_mirrored_repeat = false;
  bool Has_GL_NV_evaluators = false;
  bool Has_GL_NV_packed_depth_stencil = false;
  bool Has_GL_NV_register_combiners2 = false;
  bool Has_GL_NV_texture_compression_vtc = false;
  bool Has_GL_NV_texture_rectangle = false;
  bool Has_GL_NV_texture_shader = false;
  bool Has_GL_NV_texture_shader2 = false;
  bool Has_GL_NV_vertex_array_range2 = false;
  bool Has_GL_NV_vertex_program = false;
  bool Has_GLX_SGIX_visual_select_group = false;
  bool Has_GL_SGIX_texture_coordinate_clamp = false;
  bool Has_GLX_OML_swap_method = false;
  bool Has_GLX_OML_sync_control = false;
  bool Has_GL_OML_interlace = false;
  bool Has_GL_OML_subsample = false;
  bool Has_GL_OML_resample = false;
  bool Has_WGL_OML_sync_control = false;
  bool Has_GL_NV_copy_depth_to_color = false;
  bool Has_GL_ATI_envmap_bumpmap = false;
  bool Has_GL_ATI_fragment_shader = false;
  bool Has_GL_ATI_pn_triangles = false;
  bool Has_GL_ATI_vertex_array_object = false;
  bool Has_GL_EXT_vertex_shader = false;
  bool Has_GL_ATI_vertex_streams = false;
  bool Has_WGL_I3D_digital_video_control = false;
  bool Has_WGL_I3D_gamma = false;
  bool Has_WGL_I3D_genlock = false;
  bool Has_WGL_I3D_image_buffer = false;
  bool Has_WGL_I3D_swap_frame_lock = false;
  bool Has_WGL_I3D_swap_frame_usage = false;
  bool Has_GL_ATI_element_array = false;
  bool Has_GL_SUN_mesh_array = false;
  bool Has_GL_SUN_slice_accum = false;
  bool Has_GL_NV_multisample_filter_hint = false;
  bool Has_GL_NV_depth_clamp = false;
  bool Has_GL_NV_occlusion_query = false;
  bool Has_GL_NV_point_sprite = false;
  bool Has_WGL_NV_render_depth_texture = false;
  bool Has_WGL_NV_render_texture_rectangle = false;
  bool Has_GL_NV_texture_shader3 = false;
  bool Has_GL_NV_vertex_program1_1 = false;
  bool Has_GL_EXT_shadow_funcs = false;
  bool Has_GL_EXT_stencil_two_side = false;
  bool Has_GL_ATI_text_fragment_shader = false;
  bool Has_GL_APPLE_client_storage = false;
  bool Has_GL_APPLE_element_array = false;
  bool Has_GL_APPLE_fence = false;
  bool Has_GL_APPLE_vertex_array_object = false;
  bool Has_GL_APPLE_vertex_array_range = false;
  bool Has_GL_APPLE_ycbcr_422 = false;
  bool Has_GL_S3_s3tc = false;
  bool Has_GL_ATI_draw_buffers = false;
  bool Has_WGL_ATI_pixel_format_float = false;
  bool Has_GL_ATI_texture_env_combine3 = false;
  bool Has_GL_ATI_texture_float = false;
  bool Has_GL_NV_float_buffer = false;
  bool Has_WGL_NV_float_buffer = false;
  bool Has_GL_NV_fragment_program = false;
  bool Has_GL_NV_half_float = false;
  bool Has_GL_NV_pixel_data_range = false;
  bool Has_GL_NV_primitive_restart = false;
  bool Has_GL_NV_texture_expand_normal = false;
  bool Has_GL_NV_vertex_program2 = false;
  bool Has_GL_ATI_map_object_buffer = false;
  bool Has_GL_ATI_separate_stencil = false;
  bool Has_GL_ATI_vertex_attrib_array_object = false;
  bool Has_GL_OES_byte_coordinates = false;
  bool Has_GL_OES_fixed_point = false;
  bool Has_GL_OES_single_precision = false;
  bool Has_GL_OES_compressed_paletted_texture = false;
  bool Has_GL_OES_read_format = false;
  bool Has_GL_OES_query_matrix = false;
  bool Has_GL_EXT_depth_bounds_test = false;
  bool Has_GL_EXT_texture_mirror_clamp = false;
  bool Has_GL_EXT_blend_equation_separate = false;
  bool Has_GL_MESA_pack_invert = false;
  bool Has_GL_MESA_ycbcr_texture = false;
  bool Has_GL_EXT_pixel_buffer_object = false;
  bool Has_GL_NV_fragment_program_option = false;
  bool Has_GL_NV_fragment_program2 = false;
  bool Has_GL_NV_vertex_program2_option = false;
  bool Has_GL_NV_vertex_program3 = false;
  bool Has_GLX_SGIX_hyperpipe = false;
  bool Has_GLX_MESA_agp_offset = false;
  bool Has_GL_EXT_texture_compression_dxt1 = false;
  bool Has_GL_EXT_framebuffer_object = false;
  bool Has_GL_GREMEDY_string_marker = false;
  bool Has_GL_EXT_packed_depth_stencil = false;
  bool Has_WGL_3DL_stereo_control = false;
  bool Has_GL_EXT_stencil_clear_tag = false;
  bool Has_GL_EXT_texture_sRGB = false;
  bool Has_GL_EXT_framebuffer_blit = false;
  bool Has_GL_EXT_framebuffer_multisample = false;
  bool Has_GL_MESAX_texture_stack = false;
  bool Has_GL_EXT_timer_query = false;
  bool Has_GL_EXT_gpu_program_parameters = false;
  bool Has_GL_APPLE_flush_buffer_range = false;
  bool Has_GL_NV_gpu_program4 = false;
  bool Has_GL_NV_geometry_program4 = false;
  bool Has_GL_EXT_geometry_shader4 = false;
  bool Has_GL_NV_vertex_program4 = false;
  bool Has_GL_EXT_gpu_shader4 = false;
  bool Has_GL_EXT_draw_instanced = false;
  bool Has_GL_EXT_packed_float = false;
  bool Has_WGL_EXT_pixel_format_packed_float = false;
  bool Has_GLX_EXT_fbconfig_packed_float = false;
  bool Has_GL_EXT_texture_array = false;
  bool Has_GL_EXT_texture_buffer_object = false;
  bool Has_GL_EXT_texture_compression_latc = false;
  bool Has_GL_EXT_texture_compression_rgtc = false;
  bool Has_GL_EXT_texture_shared_exponent = false;
  bool Has_GL_NV_depth_buffer_float = false;
  bool Has_GL_NV_fragment_program4 = false;
  bool Has_GL_NV_framebuffer_multisample_coverage = false;
  bool Has_GL_EXT_framebuffer_sRGB = false;
  bool Has_GLX_EXT_framebuffer_sRGB = false;
  bool Has_WGL_EXT_framebuffer_sRGB = false;
  bool Has_GL_NV_geometry_shader4 = false;
  bool Has_GL_NV_parameter_buffer_object = false;
  bool Has_GL_EXT_draw_buffers2 = false;
  bool Has_GL_NV_transform_feedback = false;
  bool Has_GL_EXT_bindable_uniform = false;
  bool Has_GL_EXT_texture_integer = false;
  bool Has_GLX_EXT_texture_from_pixmap = false;
  bool Has_GL_GREMEDY_frame_terminator = false;
  bool Has_GL_NV_conditional_render = false;
  bool Has_GL_NV_present_video = false;
  bool Has_GLX_NV_present_video = false;
  bool Has_WGL_NV_present_video = false;
  bool Has_GLX_NV_video_output = false;
  bool Has_WGL_NV_video_output = false;
  bool Has_GLX_NV_swap_group = false;
  bool Has_WGL_NV_swap_group = false;
  bool Has_GL_EXT_transform_feedback = false;
  bool Has_GL_EXT_direct_state_access = false;
  bool Has_GL_EXT_vertex_array_bgra = false;
  bool Has_WGL_NV_gpu_affinity = false;
  bool Has_GL_EXT_texture_swizzle = false;
  bool Has_GL_NV_explicit_multisample = false;
  bool Has_GL_NV_transform_feedback2 = false;
  bool Has_GL_ATI_meminfo = false;
  bool Has_GL_AMD_performance_monitor = false;
  bool Has_WGL_AMD_gpu_association = false;
  bool Has_GL_AMD_texture_texture4 = false;
  bool Has_GL_AMD_vertex_shader_tessellator = false;
  bool Has_GL_EXT_provoking_vertex = false;
  bool Has_GL_EXT_texture_snorm = false;
  bool Has_GL_AMD_draw_buffers_blend = false;
  bool Has_GL_APPLE_texture_range = false;
  bool Has_GL_APPLE_float_pixels = false;
  bool Has_GL_APPLE_vertex_program_evaluators = false;
  bool Has_GL_APPLE_aux_depth_stencil = false;
  bool Has_GL_APPLE_object_purgeable = false;
  bool Has_GL_APPLE_row_bytes = false;
  bool Has_GL_APPLE_rgb_422 = false;
  bool Has_GL_NV_video_capture = false;
  bool Has_GLX_NV_video_capture = false;
  bool Has_WGL_NV_video_capture = false;
  bool Has_GL_EXT_swap_control = false;
  bool Has_GL_NV_copy_image = false;
  bool Has_WGL_NV_copy_image = false;
  bool Has_GLX_NV_copy_image = false;
  bool Has_GL_EXT_separate_shader_objects = false;
  bool Has_GL_NV_parameter_buffer_object2 = false;
  bool Has_GL_NV_shader_buffer_load = false;
  bool Has_GL_NV_vertex_buffer_unified_memory = false;
  bool Has_GL_NV_texture_barrier = false;
  bool Has_GL_AMD_shader_stencil_export = false;
  bool Has_GL_AMD_seamless_cubemap_per_texture = false;
  bool Has_GLX_INTEL_swap_event = false;
  bool Has_GL_AMD_conservative_depth = false;
  bool Has_GL_EXT_shader_image_load_store = false;
  bool Has_GL_EXT_vertex_attrib_64bit = false;
  bool Has_GL_NV_gpu_program5 = false;
  bool Has_GL_NV_gpu_shader5 = false;
  bool Has_GL_NV_shader_buffer_store = false;
  bool Has_GL_NV_tessellation_program5 = false;
  bool Has_GL_NV_vertex_attrib_integer_64bit = false;
  bool Has_GL_NV_multisample_coverage = false;
  bool Has_GL_AMD_name_gen_delete = false;
  bool Has_GL_AMD_debug_output = false;
  bool Has_GL_NV_vdpau_interop = false;
  bool Has_GL_AMD_transform_feedback3_lines_triangles = false;
  bool Has_GLX_AMD_gpu_association = false;
  bool Has_GLX_EXT_create_context_es2_profile = false;
  bool Has_WGL_EXT_create_context_es2_profile = false;
  bool Has_GL_AMD_depth_clamp_separate = false;
  bool Has_GL_EXT_texture_sRGB_decode = false;

  const char* initOpenGLExtensions()
  {
    ExtensionInitializer init;
    return init.initExtensions();
  }
}

extern "C"
{
  // GL_VERSION_1_2
  PFNGLBLENDCOLORPROC glBlendColor = NULL;
  PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;
  PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = NULL;
  PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;
  PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = NULL;
  PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D = NULL;

  // GL_VERSION_1_2_DEPRECATED
  PFNGLCOLORTABLEPROC glColorTable = NULL;
  PFNGLCOLORTABLEPARAMETERFVPROC glColorTableParameterfv = NULL;
  PFNGLCOLORTABLEPARAMETERIVPROC glColorTableParameteriv = NULL;
  PFNGLCOPYCOLORTABLEPROC glCopyColorTable = NULL;
  PFNGLGETCOLORTABLEPROC glGetColorTable = NULL;
  PFNGLGETCOLORTABLEPARAMETERFVPROC glGetColorTableParameterfv = NULL;
  PFNGLGETCOLORTABLEPARAMETERIVPROC glGetColorTableParameteriv = NULL;
  PFNGLCOLORSUBTABLEPROC glColorSubTable = NULL;
  PFNGLCOPYCOLORSUBTABLEPROC glCopyColorSubTable = NULL;
  PFNGLCONVOLUTIONFILTER1DPROC glConvolutionFilter1D = NULL;
  PFNGLCONVOLUTIONFILTER2DPROC glConvolutionFilter2D = NULL;
  PFNGLCONVOLUTIONPARAMETERFPROC glConvolutionParameterf = NULL;
  PFNGLCONVOLUTIONPARAMETERFVPROC glConvolutionParameterfv = NULL;
  PFNGLCONVOLUTIONPARAMETERIPROC glConvolutionParameteri = NULL;
  PFNGLCONVOLUTIONPARAMETERIVPROC glConvolutionParameteriv = NULL;
  PFNGLCOPYCONVOLUTIONFILTER1DPROC glCopyConvolutionFilter1D = NULL;
  PFNGLCOPYCONVOLUTIONFILTER2DPROC glCopyConvolutionFilter2D = NULL;
  PFNGLGETCONVOLUTIONFILTERPROC glGetConvolutionFilter = NULL;
  PFNGLGETCONVOLUTIONPARAMETERFVPROC glGetConvolutionParameterfv = NULL;
  PFNGLGETCONVOLUTIONPARAMETERIVPROC glGetConvolutionParameteriv = NULL;
  PFNGLGETSEPARABLEFILTERPROC glGetSeparableFilter = NULL;
  PFNGLSEPARABLEFILTER2DPROC glSeparableFilter2D = NULL;
  PFNGLGETHISTOGRAMPROC glGetHistogram = NULL;
  PFNGLGETHISTOGRAMPARAMETERFVPROC glGetHistogramParameterfv = NULL;
  PFNGLGETHISTOGRAMPARAMETERIVPROC glGetHistogramParameteriv = NULL;
  PFNGLGETMINMAXPROC glGetMinmax = NULL;
  PFNGLGETMINMAXPARAMETERFVPROC glGetMinmaxParameterfv = NULL;
  PFNGLGETMINMAXPARAMETERIVPROC glGetMinmaxParameteriv = NULL;
  PFNGLHISTOGRAMPROC glHistogram = NULL;
  PFNGLMINMAXPROC glMinmax = NULL;
  PFNGLRESETHISTOGRAMPROC glResetHistogram = NULL;
  PFNGLRESETMINMAXPROC glResetMinmax = NULL;

  // GL_VERSION_1_3
  PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
  PFNGLSAMPLECOVERAGEPROC glSampleCoverage = NULL;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = NULL;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
  PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D = NULL;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = NULL;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = NULL;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = NULL;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage = NULL;

  // GL_VERSION_1_3_DEPRECATED
  PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = NULL;
  PFNGLMULTITEXCOORD1DPROC glMultiTexCoord1d = NULL;
  PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv = NULL;
  PFNGLMULTITEXCOORD1FPROC glMultiTexCoord1f = NULL;
  PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv = NULL;
  PFNGLMULTITEXCOORD1IPROC glMultiTexCoord1i = NULL;
  PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv = NULL;
  PFNGLMULTITEXCOORD1SPROC glMultiTexCoord1s = NULL;
  PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv = NULL;
  PFNGLMULTITEXCOORD2DPROC glMultiTexCoord2d = NULL;
  PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv = NULL;
  PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f = NULL;
  PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv = NULL;
  PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i = NULL;
  PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv = NULL;
  PFNGLMULTITEXCOORD2SPROC glMultiTexCoord2s = NULL;
  PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv = NULL;
  PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d = NULL;
  PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv = NULL;
  PFNGLMULTITEXCOORD3FPROC glMultiTexCoord3f = NULL;
  PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv = NULL;
  PFNGLMULTITEXCOORD3IPROC glMultiTexCoord3i = NULL;
  PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv = NULL;
  PFNGLMULTITEXCOORD3SPROC glMultiTexCoord3s = NULL;
  PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv = NULL;
  PFNGLMULTITEXCOORD4DPROC glMultiTexCoord4d = NULL;
  PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv = NULL;
  PFNGLMULTITEXCOORD4FPROC glMultiTexCoord4f = NULL;
  PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv = NULL;
  PFNGLMULTITEXCOORD4IPROC glMultiTexCoord4i = NULL;
  PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv = NULL;
  PFNGLMULTITEXCOORD4SPROC glMultiTexCoord4s = NULL;
  PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv = NULL;
  PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf = NULL;
  PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd = NULL;
  PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf = NULL;
  PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd = NULL;

  // GL_VERSION_1_4
  PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;
  PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = NULL;
  PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements = NULL;
  PFNGLPOINTPARAMETERFPROC glPointParameterf = NULL;
  PFNGLPOINTPARAMETERFVPROC glPointParameterfv = NULL;
  PFNGLPOINTPARAMETERIPROC glPointParameteri = NULL;
  PFNGLPOINTPARAMETERIVPROC glPointParameteriv = NULL;

  // GL_VERSION_1_4_DEPRECATED
  PFNGLFOGCOORDFPROC glFogCoordf = NULL;
  PFNGLFOGCOORDFVPROC glFogCoordfv = NULL;
  PFNGLFOGCOORDDPROC glFogCoordd = NULL;
  PFNGLFOGCOORDDVPROC glFogCoorddv = NULL;
  PFNGLFOGCOORDPOINTERPROC glFogCoordPointer = NULL;
  PFNGLSECONDARYCOLOR3BPROC glSecondaryColor3b = NULL;
  PFNGLSECONDARYCOLOR3BVPROC glSecondaryColor3bv = NULL;
  PFNGLSECONDARYCOLOR3DPROC glSecondaryColor3d = NULL;
  PFNGLSECONDARYCOLOR3DVPROC glSecondaryColor3dv = NULL;
  PFNGLSECONDARYCOLOR3FPROC glSecondaryColor3f = NULL;
  PFNGLSECONDARYCOLOR3FVPROC glSecondaryColor3fv = NULL;
  PFNGLSECONDARYCOLOR3IPROC glSecondaryColor3i = NULL;
  PFNGLSECONDARYCOLOR3IVPROC glSecondaryColor3iv = NULL;
  PFNGLSECONDARYCOLOR3SPROC glSecondaryColor3s = NULL;
  PFNGLSECONDARYCOLOR3SVPROC glSecondaryColor3sv = NULL;
  PFNGLSECONDARYCOLOR3UBPROC glSecondaryColor3ub = NULL;
  PFNGLSECONDARYCOLOR3UBVPROC glSecondaryColor3ubv = NULL;
  PFNGLSECONDARYCOLOR3UIPROC glSecondaryColor3ui = NULL;
  PFNGLSECONDARYCOLOR3UIVPROC glSecondaryColor3uiv = NULL;
  PFNGLSECONDARYCOLOR3USPROC glSecondaryColor3us = NULL;
  PFNGLSECONDARYCOLOR3USVPROC glSecondaryColor3usv = NULL;
  PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer = NULL;
  PFNGLWINDOWPOS2DPROC glWindowPos2d = NULL;
  PFNGLWINDOWPOS2DVPROC glWindowPos2dv = NULL;
  PFNGLWINDOWPOS2FPROC glWindowPos2f = NULL;
  PFNGLWINDOWPOS2FVPROC glWindowPos2fv = NULL;
  PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;
  PFNGLWINDOWPOS2IVPROC glWindowPos2iv = NULL;
  PFNGLWINDOWPOS2SPROC glWindowPos2s = NULL;
  PFNGLWINDOWPOS2SVPROC glWindowPos2sv = NULL;
  PFNGLWINDOWPOS3DPROC glWindowPos3d = NULL;
  PFNGLWINDOWPOS3DVPROC glWindowPos3dv = NULL;
  PFNGLWINDOWPOS3FPROC glWindowPos3f = NULL;
  PFNGLWINDOWPOS3FVPROC glWindowPos3fv = NULL;
  PFNGLWINDOWPOS3IPROC glWindowPos3i = NULL;
  PFNGLWINDOWPOS3IVPROC glWindowPos3iv = NULL;
  PFNGLWINDOWPOS3SPROC glWindowPos3s = NULL;
  PFNGLWINDOWPOS3SVPROC glWindowPos3sv = NULL;

  // GL_VERSION_1_5
  PFNGLGENQUERIESPROC glGenQueries = NULL;
  PFNGLDELETEQUERIESPROC glDeleteQueries = NULL;
  PFNGLISQUERYPROC glIsQuery = NULL;
  PFNGLBEGINQUERYPROC glBeginQuery = NULL;
  PFNGLENDQUERYPROC glEndQuery = NULL;
  PFNGLGETQUERYIVPROC glGetQueryiv = NULL;
  PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = NULL;
  PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = NULL;
  PFNGLBINDBUFFERPROC glBindBuffer = NULL;
  PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
  PFNGLGENBUFFERSPROC glGenBuffers = NULL;
  PFNGLISBUFFERPROC glIsBuffer = NULL;
  PFNGLBUFFERDATAPROC glBufferData = NULL;
  PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
  PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = NULL;
  PFNGLMAPBUFFERPROC glMapBuffer = NULL;
  PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;
  PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
  PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv = NULL;

  // GL_VERSION_2_0
  PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = NULL;
  PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;
  PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = NULL;
  PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = NULL;
  PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = NULL;
  PFNGLATTACHSHADERPROC glAttachShader = NULL;
  PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
  PFNGLCOMPILESHADERPROC glCompileShader = NULL;
  PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
  PFNGLCREATESHADERPROC glCreateShader = NULL;
  PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
  PFNGLDELETESHADERPROC glDeleteShader = NULL;
  PFNGLDETACHSHADERPROC glDetachShader = NULL;
  PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
  PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
  PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = NULL;
  PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = NULL;
  PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = NULL;
  PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
  PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
  PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
  PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
  PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
  PFNGLGETSHADERSOURCEPROC glGetShaderSource = NULL;
  PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
  PFNGLGETUNIFORMFVPROC glGetUniformfv = NULL;
  PFNGLGETUNIFORMIVPROC glGetUniformiv = NULL;
  PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = NULL;
  PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = NULL;
  PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = NULL;
  PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = NULL;
  PFNGLISPROGRAMPROC glIsProgram = NULL;
  PFNGLISSHADERPROC glIsShader = NULL;
  PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
  PFNGLSHADERSOURCEPROC glShaderSource = NULL;
  PFNGLUSEPROGRAMPROC glUseProgram = NULL;
  PFNGLUNIFORM1FPROC glUniform1f = NULL;
  PFNGLUNIFORM2FPROC glUniform2f = NULL;
  PFNGLUNIFORM3FPROC glUniform3f = NULL;
  PFNGLUNIFORM4FPROC glUniform4f = NULL;
  PFNGLUNIFORM1IPROC glUniform1i = NULL;
  PFNGLUNIFORM2IPROC glUniform2i = NULL;
  PFNGLUNIFORM3IPROC glUniform3i = NULL;
  PFNGLUNIFORM4IPROC glUniform4i = NULL;
  PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
  PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
  PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
  PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
  PFNGLUNIFORM1IVPROC glUniform1iv = NULL;
  PFNGLUNIFORM2IVPROC glUniform2iv = NULL;
  PFNGLUNIFORM3IVPROC glUniform3iv = NULL;
  PFNGLUNIFORM4IVPROC glUniform4iv = NULL;
  PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = NULL;
  PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = NULL;
  PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
  PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
  PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d = NULL;
  PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv = NULL;
  PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = NULL;
  PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = NULL;
  PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s = NULL;
  PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv = NULL;
  PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d = NULL;
  PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv = NULL;
  PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f = NULL;
  PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = NULL;
  PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s = NULL;
  PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv = NULL;
  PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d = NULL;
  PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv = NULL;
  PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f = NULL;
  PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = NULL;
  PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s = NULL;
  PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv = NULL;
  PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv = NULL;
  PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv = NULL;
  PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv = NULL;
  PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub = NULL;
  PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv = NULL;
  PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv = NULL;
  PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv = NULL;
  PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv = NULL;
  PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d = NULL;
  PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv = NULL;
  PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f = NULL;
  PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = NULL;
  PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv = NULL;
  PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s = NULL;
  PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv = NULL;
  PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv = NULL;
  PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv = NULL;
  PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv = NULL;
  PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;

  // GL_VERSION_2_1
  PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv = NULL;
  PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv = NULL;
  PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv = NULL;
  PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv = NULL;
  PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv = NULL;
  PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv = NULL;

  // GL_VERSION_3_0
  /* OpenGL 3.0 also reuses entry points from these extensions: */
  /* ARB_framebuffer_object */
  /* ARB_map_buffer_range */
  /* ARB_vertex_array_object */
  PFNGLCOLORMASKIPROC glColorMaski = NULL;
  // PFNGLGETBOOLEANINDEXEDVEXTPROC glGetBooleanIndexedvEXT = NULL;
  // PFNGLGETINTEGERINDEXEDVEXTPROC glGetIntegerIndexedvEXT = NULL;
  PFNGLENABLEIPROC glEnablei = NULL;
  PFNGLDISABLEIPROC glDisablei = NULL;
  PFNGLISENABLEDIPROC glIsEnabledi = NULL;
  PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback = NULL;
  PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback = NULL;
  PFNGLBINDBUFFERRANGEPROC glBindBufferRange = NULL;
  PFNGLBINDBUFFERBASEPROC glBindBufferBase = NULL;
  PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings = NULL;
  PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying = NULL;
  PFNGLCLAMPCOLORPROC glClampColor = NULL;
  PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender = NULL;
  PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender = NULL;
  PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = NULL;
  PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv = NULL;
  PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv = NULL;
  PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i = NULL;
  PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i = NULL;
  PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i = NULL;
  PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i = NULL;
  PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui = NULL;
  PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui = NULL;
  PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui = NULL;
  PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui = NULL;
  PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv = NULL;
  PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv = NULL;
  PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv = NULL;
  PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv = NULL;
  PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv = NULL;
  PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv = NULL;
  PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv = NULL;
  PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv = NULL;
  PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv = NULL;
  PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv = NULL;
  PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv = NULL;
  PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv = NULL;
  PFNGLGETUNIFORMUIVPROC glGetUniformuiv = NULL;
  PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation = NULL;
  PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation = NULL;
  PFNGLUNIFORM1UIPROC glUniform1ui = NULL;
  PFNGLUNIFORM2UIPROC glUniform2ui = NULL;
  PFNGLUNIFORM3UIPROC glUniform3ui = NULL;
  PFNGLUNIFORM4UIPROC glUniform4ui = NULL;
  PFNGLUNIFORM1UIVPROC glUniform1uiv = NULL;
  PFNGLUNIFORM2UIVPROC glUniform2uiv = NULL;
  PFNGLUNIFORM3UIVPROC glUniform3uiv = NULL;
  PFNGLUNIFORM4UIVPROC glUniform4uiv = NULL;
  PFNGLTEXPARAMETERIIVPROC glTexParameterIiv = NULL;
  PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv = NULL;
  PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv = NULL;
  PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv = NULL;
  PFNGLCLEARBUFFERIVPROC glClearBufferiv = NULL;
  PFNGLCLEARBUFFERUIVPROC glClearBufferuiv = NULL;
  PFNGLCLEARBUFFERFVPROC glClearBufferfv = NULL;
  PFNGLCLEARBUFFERFIPROC glClearBufferfi = NULL;
  PFNGLGETSTRINGIPROC glGetStringi = NULL;


  // GL_VERSION_3_1
  /* OpenGL 3.1 also reuses entry points from these extensions: */
  /* ARB_copy_buffer */
  /* ARB_uniform_buffer_object */
  PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced = NULL;
  PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced = NULL;
  PFNGLTEXBUFFERPROC glTexBuffer = NULL;
  PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex = NULL;

  // GL_VERSION_3_2
  /* OpenGL 3.2 also reuses entry points from these extensions: */
  /* ARB_draw_elements_base_vertex */
  /* ARB_provoking_vertex */
  /* ARB_sync */
  /* ARB_texture_multisample */
  PFNGLGETINTEGER64I_VPROC  glGetInteger64i_v = NULL;
  PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v = NULL;
  PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = NULL;

  // GL_VERSION_3_3
  /* OpenGL 3.3 also reuses entry points from these extensions: */
  /* ARB_blend_func_extended */
  /* ARB_sampler_objects */
  /* ARB_explicit_attrib_location, but it has none */
  /* ARB_occlusion_query2 (no entry points) */
  /* ARB_shader_bit_encoding (no entry points) */
  /* ARB_texture_rgb10_a2ui (no entry points) */
  /* ARB_texture_swizzle (no entry points) */
  /* ARB_timer_query */
  /* ARB_vertex_type_2_10_10_10_rev */
  PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor = NULL;

  // GL_VERSION_4_0
  /* OpenGL 4.0 also reuses entry points from these extensions: */
  /* ARB_texture_query_lod (no entry points) */
  /* ARB_draw_indirect */
  /* ARB_gpu_shader5 (no entry points) */
  /* ARB_gpu_shader_fp64 */
  /* ARB_shader_subroutine */
  /* ARB_tessellation_shader */
  /* ARB_texture_buffer_object_rgb32 (no entry points) */
  /* ARB_texture_cube_map_array (no entry points) */
  /* ARB_texture_gather (no entry points) */
  /* ARB_transform_feedback2 */
  /* ARB_transform_feedback3 */
  PFNGLMINSAMPLESHADINGPROC glMinSampleShading = NULL;
  PFNGLBLENDEQUATIONIPROC glBlendEquationi = NULL;
  PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei = NULL;
  PFNGLBLENDFUNCIPROC glBlendFunci = NULL;
  PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei = NULL;

  // GL_VERSION_4_1
  /* OpenGL 4.1 also reuses entry points from these extensions: */
  /* ARB_ES2_compatibility */
  /* ARB_get_program_binary */
  /* ARB_separate_shader_objects */
  /* ARB_shader_precision (no entry points) */
  /* ARB_vertex_attrib_64bit */
  /* ARB_viewport_array */
  // NO OTHER PROCEDURES DEFINED

  // GL_ARB_multitexture
  PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
  PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
  PFNGLMULTITEXCOORD1DARBPROC glMultiTexCoord1dARB = NULL;
  PFNGLMULTITEXCOORD1DVARBPROC glMultiTexCoord1dvARB = NULL;
  PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB = NULL;
  PFNGLMULTITEXCOORD1FVARBPROC glMultiTexCoord1fvARB = NULL;
  PFNGLMULTITEXCOORD1IARBPROC glMultiTexCoord1iARB = NULL;
  PFNGLMULTITEXCOORD1IVARBPROC glMultiTexCoord1ivARB = NULL;
  PFNGLMULTITEXCOORD1SARBPROC glMultiTexCoord1sARB = NULL;
  PFNGLMULTITEXCOORD1SVARBPROC glMultiTexCoord1svARB = NULL;
  PFNGLMULTITEXCOORD2DARBPROC glMultiTexCoord2dARB = NULL;
  PFNGLMULTITEXCOORD2DVARBPROC glMultiTexCoord2dvARB = NULL;
  PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
  PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB = NULL;
  PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB = NULL;
  PFNGLMULTITEXCOORD2IVARBPROC glMultiTexCoord2ivARB = NULL;
  PFNGLMULTITEXCOORD2SARBPROC glMultiTexCoord2sARB = NULL;
  PFNGLMULTITEXCOORD2SVARBPROC glMultiTexCoord2svARB = NULL;
  PFNGLMULTITEXCOORD3DARBPROC glMultiTexCoord3dARB = NULL;
  PFNGLMULTITEXCOORD3DVARBPROC glMultiTexCoord3dvARB = NULL;
  PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB = NULL;
  PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB = NULL;
  PFNGLMULTITEXCOORD3IARBPROC glMultiTexCoord3iARB = NULL;
  PFNGLMULTITEXCOORD3IVARBPROC glMultiTexCoord3ivARB = NULL;
  PFNGLMULTITEXCOORD3SARBPROC glMultiTexCoord3sARB = NULL;
  PFNGLMULTITEXCOORD3SVARBPROC glMultiTexCoord3svARB = NULL;
  PFNGLMULTITEXCOORD4DARBPROC glMultiTexCoord4dARB = NULL;
  PFNGLMULTITEXCOORD4DVARBPROC glMultiTexCoord4dvARB = NULL;
  PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB = NULL;
  PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB = NULL;
  PFNGLMULTITEXCOORD4IARBPROC glMultiTexCoord4iARB = NULL;
  PFNGLMULTITEXCOORD4IVARBPROC glMultiTexCoord4ivARB = NULL;
  PFNGLMULTITEXCOORD4SARBPROC glMultiTexCoord4sARB = NULL;
  PFNGLMULTITEXCOORD4SVARBPROC glMultiTexCoord4svARB = NULL;

  // GL_ARB_transpose_matrix
  PFNGLLOADTRANSPOSEMATRIXFARBPROC glLoadTransposeMatrixfARB = NULL;
  PFNGLLOADTRANSPOSEMATRIXDARBPROC glLoadTransposeMatrixdARB = NULL;
  PFNGLMULTTRANSPOSEMATRIXFARBPROC glMultTransposeMatrixfARB = NULL;
  PFNGLMULTTRANSPOSEMATRIXDARBPROC glMultTransposeMatrixdARB = NULL;

  // GL_ARB_multisample
  PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB = NULL;

  // GL_ARB_texture_compression
  PFNGLCOMPRESSEDTEXIMAGE3DARBPROC glCompressedTexImage3DARB = NULL;
  PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = NULL;
  PFNGLCOMPRESSEDTEXIMAGE1DARBPROC glCompressedTexImage1DARB = NULL;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glCompressedTexSubImage3DARB = NULL;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB = NULL;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glCompressedTexSubImage1DARB = NULL;
  PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB = NULL;

  // GL_ARB_point_parameters
  PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB = NULL;
  PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;

  // GL_ARB_vertex_blend
  PFNGLWEIGHTBVARBPROC glWeightbvARB = NULL;
  PFNGLWEIGHTSVARBPROC glWeightsvARB = NULL;
  PFNGLWEIGHTIVARBPROC glWeightivARB = NULL;
  PFNGLWEIGHTFVARBPROC glWeightfvARB = NULL;
  PFNGLWEIGHTDVARBPROC glWeightdvARB = NULL;
  PFNGLWEIGHTUBVARBPROC glWeightubvARB = NULL;
  PFNGLWEIGHTUSVARBPROC glWeightusvARB = NULL;
  PFNGLWEIGHTUIVARBPROC glWeightuivARB = NULL;
  PFNGLWEIGHTPOINTERARBPROC glWeightPointerARB = NULL;
  PFNGLVERTEXBLENDARBPROC glVertexBlendARB = NULL;

  // GL_ARB_matrix_palette
  PFNGLCURRENTPALETTEMATRIXARBPROC glCurrentPaletteMatrixARB = NULL;
  PFNGLMATRIXINDEXUBVARBPROC glMatrixIndexubvARB = NULL;
  PFNGLMATRIXINDEXUSVARBPROC glMatrixIndexusvARB = NULL;
  PFNGLMATRIXINDEXUIVARBPROC glMatrixIndexuivARB = NULL;
  PFNGLMATRIXINDEXPOINTERARBPROC glMatrixIndexPointerARB = NULL;

  // GL_ARB_window_pos
  PFNGLWINDOWPOS2DARBPROC glWindowPos2dARB = NULL;
  PFNGLWINDOWPOS2DVARBPROC glWindowPos2dvARB = NULL;
  PFNGLWINDOWPOS2FARBPROC glWindowPos2fARB = NULL;
  PFNGLWINDOWPOS2FVARBPROC glWindowPos2fvARB = NULL;
  PFNGLWINDOWPOS2IARBPROC glWindowPos2iARB = NULL;
  PFNGLWINDOWPOS2IVARBPROC glWindowPos2ivARB = NULL;
  PFNGLWINDOWPOS2SARBPROC glWindowPos2sARB = NULL;
  PFNGLWINDOWPOS2SVARBPROC glWindowPos2svARB = NULL;
  PFNGLWINDOWPOS3DARBPROC glWindowPos3dARB = NULL;
  PFNGLWINDOWPOS3DVARBPROC glWindowPos3dvARB = NULL;
  PFNGLWINDOWPOS3FARBPROC glWindowPos3fARB = NULL;
  PFNGLWINDOWPOS3FVARBPROC glWindowPos3fvARB = NULL;
  PFNGLWINDOWPOS3IARBPROC glWindowPos3iARB = NULL;
  PFNGLWINDOWPOS3IVARBPROC glWindowPos3ivARB = NULL;
  PFNGLWINDOWPOS3SARBPROC glWindowPos3sARB = NULL;
  PFNGLWINDOWPOS3SVARBPROC glWindowPos3svARB = NULL;

  // GL_ARB_vertex_program
  PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB = NULL;
  PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB = NULL;
  PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB = NULL;
  PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB = NULL;
  PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB = NULL;
  PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB = NULL;
  PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB = NULL;
  PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB = NULL;
  PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB = NULL;
  PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB = NULL;
  PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB = NULL;
  PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB = NULL;
  PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB = NULL;
  PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB = NULL;
  PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB = NULL;
  PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB = NULL;
  PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB = NULL;
  PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB = NULL;
  PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB = NULL;
  PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB = NULL;
  PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB = NULL;
  PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB = NULL;
  PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB = NULL;
  PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB = NULL;
  PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB = NULL;
  PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB = NULL;
  PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB = NULL;
  PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB = NULL;
  PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB = NULL;
  PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB = NULL;
  PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB = NULL;
  PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB = NULL;
  PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB = NULL;
  PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB = NULL;
  PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB = NULL;
  PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB = NULL;
  PFNGLVERTEXATTRIBPOINTERARBPROC glVertexAttribPointerARB = NULL;
  PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB = NULL;
  PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB = NULL;
  PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = NULL;
  PFNGLBINDPROGRAMARBPROC glBindProgramARB = NULL;
  PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB = NULL;
  PFNGLGENPROGRAMSARBPROC glGenProgramsARB = NULL;
  PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB = NULL;
  PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB = NULL;
  PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4fARB = NULL;
  PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB = NULL;
  PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB = NULL;
  PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB = NULL;
  PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB = NULL;
  PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = NULL;
  PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB = NULL;
  PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB = NULL;
  PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB = NULL;
  PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB = NULL;
  PFNGLGETPROGRAMIVARBPROC glGetProgramivARB = NULL;
  PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB = NULL;
  PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB = NULL;
  PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB = NULL;
  PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB = NULL;
  PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB = NULL;
  PFNGLISPROGRAMARBPROC glIsProgramARB = NULL;

  // GL_ARB_vertex_buffer_object
  PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
  PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
  PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
  PFNGLISBUFFERARBPROC glIsBufferARB = NULL;
  PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
  PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
  PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB = NULL;
  PFNGLMAPBUFFERARBPROC glMapBufferARB = NULL;
  PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = NULL;
  PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
  PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB = NULL;

  // GL_ARB_occlusion_query
  PFNGLGENQUERIESARBPROC glGenQueriesARB = NULL;
  PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB = NULL;
  PFNGLISQUERYARBPROC glIsQueryARB = NULL;
  PFNGLBEGINQUERYARBPROC glBeginQueryARB = NULL;
  PFNGLENDQUERYARBPROC glEndQueryARB = NULL;
  PFNGLGETQUERYIVARBPROC glGetQueryivARB = NULL;
  PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB = NULL;
  PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB = NULL;

  // GL_ARB_shader_objects
  PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
  PFNGLGETHANDLEARBPROC glGetHandleARB = NULL;
  PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
  PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
  PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
  PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
  PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
  PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
  PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
  PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
  PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB = NULL;
  PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
  PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
  PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
  PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
  PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
  PFNGLUNIFORM2IARBPROC glUniform2iARB = NULL;
  PFNGLUNIFORM3IARBPROC glUniform3iARB = NULL;
  PFNGLUNIFORM4IARBPROC glUniform4iARB = NULL;
  PFNGLUNIFORM1FVARBPROC glUniform1fvARB = NULL;
  PFNGLUNIFORM2FVARBPROC glUniform2fvARB = NULL;
  PFNGLUNIFORM3FVARBPROC glUniform3fvARB = NULL;
  PFNGLUNIFORM4FVARBPROC glUniform4fvARB = NULL;
  PFNGLUNIFORM1IVARBPROC glUniform1ivARB = NULL;
  PFNGLUNIFORM2IVARBPROC glUniform2ivARB = NULL;
  PFNGLUNIFORM3IVARBPROC glUniform3ivARB = NULL;
  PFNGLUNIFORM4IVARBPROC glUniform4ivARB = NULL;
  PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB = NULL;
  PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB = NULL;
  PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB = NULL;
  PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB = NULL;
  PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
  PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
  PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjectsARB = NULL;
  PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
  PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniformARB = NULL;
  PFNGLGETUNIFORMFVARBPROC glGetUniformfvARB = NULL;
  PFNGLGETUNIFORMIVARBPROC glGetUniformivARB = NULL;
  PFNGLGETSHADERSOURCEARBPROC glGetShaderSourceARB = NULL;

  // GL_ARB_vertex_shader
  PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB = NULL;
  PFNGLGETACTIVEATTRIBARBPROC glGetActiveAttribARB = NULL;
  PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB = NULL;

  // GL_ARB_draw_buffers
  PFNGLDRAWBUFFERSARBPROC glDrawBuffersARB = NULL;

  // GL_ARB_color_buffer_float
  PFNGLCLAMPCOLORARBPROC glClampColorARB = NULL;

  // GL_ARB_draw_instanced
  PFNGLDRAWARRAYSINSTANCEDARBPROC glDrawArraysInstancedARB = NULL;
  PFNGLDRAWELEMENTSINSTANCEDARBPROC glDrawElementsInstancedARB = NULL;

  // GL_ARB_framebuffer_object
  PFNGLISRENDERBUFFERPROC glIsRenderbuffer = NULL;
  PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
  PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
  PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
  PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
  PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv = NULL;
  PFNGLISFRAMEBUFFERPROC glIsFramebuffer = NULL;
  PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
  PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
  PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
  PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
  PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D = NULL;
  PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
  PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = NULL;
  PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = NULL;
  PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;
  PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = NULL;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = NULL;
  PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer = NULL;

  // GL_ARB_geometry_shader4
  PFNGLPROGRAMPARAMETERIARBPROC glProgramParameteriARB = NULL;
  PFNGLFRAMEBUFFERTEXTUREARBPROC glFramebufferTextureARB = NULL;
  PFNGLFRAMEBUFFERTEXTURELAYERARBPROC glFramebufferTextureLayerARB = NULL;
  PFNGLFRAMEBUFFERTEXTUREFACEARBPROC glFramebufferTextureFaceARB = NULL;

  // GL_ARB_instanced_arrays
  PFNGLVERTEXATTRIBDIVISORARBPROC glVertexAttribDivisorARB = NULL;

  // GL_ARB_map_buffer_range
  PFNGLMAPBUFFERRANGEPROC glMapBufferRange = NULL;
  PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange = NULL;

  // GL_ARB_texture_buffer_object
  PFNGLTEXBUFFERARBPROC glTexBufferARB = NULL;

  // GL_ARB_vertex_array_object
  PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
  PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
  PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
  PFNGLISVERTEXARRAYPROC glIsVertexArray = NULL;

  // GL_ARB_uniform_buffer_object
  PFNGLGETUNIFORMINDICESPROC glGetUniformIndices = NULL;
  PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv = NULL;
  PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName = NULL;
  PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex = NULL;
  PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv = NULL;
  PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = NULL;
  PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = NULL;

  // GL_ARB_copy_buffer
  PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData = NULL;

  // GL_ARB_draw_elements_base_vertex
  PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex = NULL;
  PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex = NULL;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = NULL;
  PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex = NULL;

  // GL_ARB_provoking_vertex
  PFNGLPROVOKINGVERTEXPROC glProvokingVertex = NULL;

  // GL_ARB_sync
  PFNGLFENCESYNCPROC glFenceSync = NULL;
  PFNGLISSYNCPROC glIsSync = NULL;
  PFNGLDELETESYNCPROC glDeleteSync = NULL;
  PFNGLCLIENTWAITSYNCPROC glClientWaitSync = NULL;
  PFNGLWAITSYNCPROC glWaitSync = NULL;
  PFNGLGETINTEGER64VPROC glGetInteger64v = NULL;
  PFNGLGETSYNCIVPROC glGetSynciv = NULL;

  // GL_ARB_texture_multisample
  PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = NULL;
  PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample = NULL;
  PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv = NULL;
  PFNGLSAMPLEMASKIPROC glSampleMaski = NULL;

  // GL_ARB_draw_buffers_blend
  PFNGLBLENDEQUATIONIARBPROC glBlendEquationiARB = NULL;
  PFNGLBLENDEQUATIONSEPARATEIARBPROC glBlendEquationSeparateiARB = NULL;
  PFNGLBLENDFUNCIARBPROC glBlendFunciARB = NULL;
  PFNGLBLENDFUNCSEPARATEIARBPROC glBlendFuncSeparateiARB = NULL;

  // GL_ARB_sample_shading
  PFNGLMINSAMPLESHADINGARBPROC glMinSampleShadingARB = NULL;

  // GL_ARB_shading_language_include
  PFNGLNAMEDSTRINGARBPROC glNamedStringARB = NULL;
  PFNGLDELETENAMEDSTRINGARBPROC glDeleteNamedStringARB = NULL;
  PFNGLCOMPILESHADERINCLUDEARBPROC glCompileShaderIncludeARB = NULL;
  PFNGLISNAMEDSTRINGARBPROC glIsNamedStringARB = NULL;
  PFNGLGETNAMEDSTRINGARBPROC glGetNamedStringARB = NULL;
  PFNGLGETNAMEDSTRINGIVARBPROC glGetNamedStringivARB = NULL;

  // GL_ARB_blend_func_extended
  PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed = NULL;
  PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex = NULL;

  // GL_ARB_sampler_objects
  PFNGLGENSAMPLERSPROC glGenSamplers = NULL;
  PFNGLDELETESAMPLERSPROC glDeleteSamplers = NULL;
  PFNGLISSAMPLERPROC glIsSampler = NULL;
  PFNGLBINDSAMPLERPROC glBindSampler = NULL;
  PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = NULL;
  PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv = NULL;
  PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = NULL;
  PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = NULL;
  PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv = NULL;
  PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv = NULL;
  PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv = NULL;
  PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv = NULL;
  PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv = NULL;
  PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv = NULL;

  // GL_ARB_timer_query
  PFNGLQUERYCOUNTERPROC glQueryCounter = NULL;
  PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v = NULL;
  PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v = NULL;

  // GL_ARB_vertex_type_2_10_10_10_rev
  PFNGLVERTEXP2UIPROC glVertexP2ui = NULL;
  PFNGLVERTEXP2UIVPROC glVertexP2uiv = NULL;
  PFNGLVERTEXP3UIPROC glVertexP3ui = NULL;
  PFNGLVERTEXP3UIVPROC glVertexP3uiv = NULL;
  PFNGLVERTEXP4UIPROC glVertexP4ui = NULL;
  PFNGLVERTEXP4UIVPROC glVertexP4uiv = NULL;
  PFNGLTEXCOORDP1UIPROC glTexCoordP1ui = NULL;
  PFNGLTEXCOORDP1UIVPROC glTexCoordP1uiv = NULL;
  PFNGLTEXCOORDP2UIPROC glTexCoordP2ui = NULL;
  PFNGLTEXCOORDP2UIVPROC glTexCoordP2uiv = NULL;
  PFNGLTEXCOORDP3UIPROC glTexCoordP3ui = NULL;
  PFNGLTEXCOORDP3UIVPROC glTexCoordP3uiv = NULL;
  PFNGLTEXCOORDP4UIPROC glTexCoordP4ui = NULL;
  PFNGLTEXCOORDP4UIVPROC glTexCoordP4uiv = NULL;
  PFNGLMULTITEXCOORDP1UIPROC glMultiTexCoordP1ui = NULL;
  PFNGLMULTITEXCOORDP1UIVPROC glMultiTexCoordP1uiv = NULL;
  PFNGLMULTITEXCOORDP2UIPROC glMultiTexCoordP2ui = NULL;
  PFNGLMULTITEXCOORDP2UIVPROC glMultiTexCoordP2uiv = NULL;
  PFNGLMULTITEXCOORDP3UIPROC glMultiTexCoordP3ui = NULL;
  PFNGLMULTITEXCOORDP3UIVPROC glMultiTexCoordP3uiv = NULL;
  PFNGLMULTITEXCOORDP4UIPROC glMultiTexCoordP4ui = NULL;
  PFNGLMULTITEXCOORDP4UIVPROC glMultiTexCoordP4uiv = NULL;
  PFNGLNORMALP3UIPROC glNormalP3ui = NULL;
  PFNGLNORMALP3UIVPROC glNormalP3uiv = NULL;
  PFNGLCOLORP3UIPROC glColorP3ui = NULL;
  PFNGLCOLORP3UIVPROC glColorP3uiv = NULL;
  PFNGLCOLORP4UIPROC glColorP4ui = NULL;
  PFNGLCOLORP4UIVPROC glColorP4uiv = NULL;
  PFNGLSECONDARYCOLORP3UIPROC glSecondaryColorP3ui = NULL;
  PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv = NULL;
  PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui = NULL;
  PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv = NULL;
  PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui = NULL;
  PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv = NULL;
  PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui = NULL;
  PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv = NULL;
  PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui = NULL;
  PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv = NULL;

  // GL_ARB_draw_indirect
  PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect = NULL;
  PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect = NULL;

  // GL_ARB_gpu_shader_fp64
  PFNGLUNIFORM1DPROC glUniform1d = NULL;
  PFNGLUNIFORM2DPROC glUniform2d = NULL;
  PFNGLUNIFORM3DPROC glUniform3d = NULL;
  PFNGLUNIFORM4DPROC glUniform4d = NULL;
  PFNGLUNIFORM1DVPROC glUniform1dv = NULL;
  PFNGLUNIFORM2DVPROC glUniform2dv = NULL;
  PFNGLUNIFORM3DVPROC glUniform3dv = NULL;
  PFNGLUNIFORM4DVPROC glUniform4dv = NULL;
  PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv = NULL;
  PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv = NULL;
  PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv = NULL;
  PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv = NULL;
  PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv = NULL;
  PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv = NULL;
  PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv = NULL;
  PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv = NULL;
  PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv = NULL;
  PFNGLGETUNIFORMDVPROC glGetUniformdv = NULL;

  // GL_ARB_shader_subroutine
  PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation = NULL;
  PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex = NULL;
  PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv = NULL;
  PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName = NULL;
  PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName = NULL;
  PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv = NULL;
  PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv = NULL;
  PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv = NULL;

  // GL_ARB_tessellation_shader
  PFNGLPATCHPARAMETERIPROC glPatchParameteri = NULL;
  PFNGLPATCHPARAMETERFVPROC glPatchParameterfv = NULL;

  // GL_ARB_transform_feedback2
  PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback = NULL;
  PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks = NULL;
  PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks = NULL;
  PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback = NULL;
  PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback = NULL;
  PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback = NULL;
  PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback = NULL;

  // GL_ARB_transform_feedback3
  PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream = NULL;
  PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed = NULL;
  PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed = NULL;
  PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv = NULL;

  // GL_ARB_ES2_compatibility
  PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler = NULL;
  PFNGLSHADERBINARYPROC glShaderBinary = NULL;
  PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat = NULL;
  PFNGLDEPTHRANGEFPROC glDepthRangef = NULL;
  PFNGLCLEARDEPTHFPROC glClearDepthf = NULL;

  // GL_ARB_get_program_binary
  PFNGLGETPROGRAMBINARYPROC glGetProgramBinary = NULL;
  PFNGLPROGRAMBINARYPROC glProgramBinary = NULL;
  PFNGLPROGRAMPARAMETERIPROC glProgramParameteri = NULL;

  // GL_ARB_separate_shader_objects
  PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages = NULL;
  PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram = NULL;
  PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv = NULL;
  PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline = NULL;
  PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines = NULL;
  PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines = NULL;
  PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline = NULL;
  PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv = NULL;
  PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i = NULL;
  PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv = NULL;
  PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f = NULL;
  PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv = NULL;
  PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d = NULL;
  PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv = NULL;
  PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui = NULL;
  PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv = NULL;
  PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i = NULL;
  PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv = NULL;
  PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f = NULL;
  PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv = NULL;
  PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d = NULL;
  PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv = NULL;
  PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui = NULL;
  PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv = NULL;
  PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i = NULL;
  PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv = NULL;
  PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f = NULL;
  PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv = NULL;
  PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d = NULL;
  PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv = NULL;
  PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui = NULL;
  PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv = NULL;
  PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i = NULL;
  PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv = NULL;
  PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f = NULL;
  PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv = NULL;
  PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d = NULL;
  PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv = NULL;
  PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui = NULL;
  PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv = NULL;
  PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline = NULL;
  PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog = NULL;

  // GL_ARB_vertex_attrib_64bit
  PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d = NULL;
  PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d = NULL;
  PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d = NULL;
  PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d = NULL;
  PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv = NULL;
  PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv = NULL;
  PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv = NULL;
  PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv = NULL;
  PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer = NULL;
  PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv = NULL;

  // GL_ARB_viewport_array
  PFNGLVIEWPORTARRAYVPROC glViewportArrayv = NULL;
  PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf = NULL;
  PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv = NULL;
  PFNGLSCISSORARRAYVPROC glScissorArrayv = NULL;
  PFNGLSCISSORINDEXEDPROC glScissorIndexed = NULL;
  PFNGLSCISSORINDEXEDVPROC glScissorIndexedv = NULL;
  PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv = NULL;
  PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed = NULL;
  PFNGLGETFLOATI_VPROC  glGetFloati_v ;
  PFNGLGETDOUBLEI_VPROC  glGetDoublei_v ;

  // GL_ARB_cl_event
  PFNGLCREATESYNCFROMCLEVENTARBPROC glCreateSyncFromCLeventARB = NULL;

  // GL_ARB_debug_output
  PFNGLDEBUGMESSAGECONTROLARBPROC glDebugMessageControlARB = NULL;
  PFNGLDEBUGMESSAGEINSERTARBPROC glDebugMessageInsertARB = NULL;
  PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB = NULL;
  PFNGLGETDEBUGMESSAGELOGARBPROC glGetDebugMessageLogARB = NULL;

  // GL_ARB_robustness
  PFNGLGETGRAPHICSRESETSTATUSARBPROC glGetGraphicsResetStatusARB = NULL;
  PFNGLGETNMAPDVARBPROC glGetnMapdvARB = NULL;
  PFNGLGETNMAPFVARBPROC glGetnMapfvARB = NULL;
  PFNGLGETNMAPIVARBPROC glGetnMapivARB = NULL;
  PFNGLGETNPIXELMAPFVARBPROC glGetnPixelMapfvARB = NULL;
  PFNGLGETNPIXELMAPUIVARBPROC glGetnPixelMapuivARB = NULL;
  PFNGLGETNPIXELMAPUSVARBPROC glGetnPixelMapusvARB = NULL;
  PFNGLGETNPOLYGONSTIPPLEARBPROC glGetnPolygonStippleARB = NULL;
  PFNGLGETNCOLORTABLEARBPROC glGetnColorTableARB = NULL;
  PFNGLGETNCONVOLUTIONFILTERARBPROC glGetnConvolutionFilterARB = NULL;
  PFNGLGETNSEPARABLEFILTERARBPROC glGetnSeparableFilterARB = NULL;
  PFNGLGETNHISTOGRAMARBPROC glGetnHistogramARB = NULL;
  PFNGLGETNMINMAXARBPROC glGetnMinmaxARB = NULL;
  PFNGLGETNTEXIMAGEARBPROC glGetnTexImageARB = NULL;
  PFNGLREADNPIXELSARBPROC glReadnPixelsARB = NULL;
  PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC glGetnCompressedTexImageARB = NULL;
  PFNGLGETNUNIFORMFVARBPROC glGetnUniformfvARB = NULL;
  PFNGLGETNUNIFORMIVARBPROC glGetnUniformivARB = NULL;
  PFNGLGETNUNIFORMUIVARBPROC glGetnUniformuivARB = NULL;
  PFNGLGETNUNIFORMDVARBPROC glGetnUniformdvARB = NULL;

  // GL_EXT_blend_color
  PFNGLBLENDCOLOREXTPROC glBlendColorEXT = NULL;

  // GL_EXT_polygon_offset
  PFNGLPOLYGONOFFSETEXTPROC glPolygonOffsetEXT = NULL;

  // GL_EXT_texture3D
  PFNGLTEXIMAGE3DEXTPROC glTexImage3DEXT = NULL;
  PFNGLTEXSUBIMAGE3DEXTPROC glTexSubImage3DEXT = NULL;

  // GL_SGIS_texture_filter4
  PFNGLGETTEXFILTERFUNCSGISPROC glGetTexFilterFuncSGIS = NULL;
  PFNGLTEXFILTERFUNCSGISPROC glTexFilterFuncSGIS = NULL;

  // GL_EXT_subtexture
  PFNGLTEXSUBIMAGE1DEXTPROC glTexSubImage1DEXT = NULL;
  PFNGLTEXSUBIMAGE2DEXTPROC glTexSubImage2DEXT = NULL;

  // GL_EXT_copy_texture
  PFNGLCOPYTEXIMAGE1DEXTPROC glCopyTexImage1DEXT = NULL;
  PFNGLCOPYTEXIMAGE2DEXTPROC glCopyTexImage2DEXT = NULL;
  PFNGLCOPYTEXSUBIMAGE1DEXTPROC glCopyTexSubImage1DEXT = NULL;
  PFNGLCOPYTEXSUBIMAGE2DEXTPROC glCopyTexSubImage2DEXT = NULL;
  PFNGLCOPYTEXSUBIMAGE3DEXTPROC glCopyTexSubImage3DEXT = NULL;

  // GL_EXT_histogram
  PFNGLGETHISTOGRAMEXTPROC glGetHistogramEXT = NULL;
  PFNGLGETHISTOGRAMPARAMETERFVEXTPROC glGetHistogramParameterfvEXT = NULL;
  PFNGLGETHISTOGRAMPARAMETERIVEXTPROC glGetHistogramParameterivEXT = NULL;
  PFNGLGETMINMAXEXTPROC glGetMinmaxEXT = NULL;
  PFNGLGETMINMAXPARAMETERFVEXTPROC glGetMinmaxParameterfvEXT = NULL;
  PFNGLGETMINMAXPARAMETERIVEXTPROC glGetMinmaxParameterivEXT = NULL;
  PFNGLHISTOGRAMEXTPROC glHistogramEXT = NULL;
  PFNGLMINMAXEXTPROC glMinmaxEXT = NULL;
  PFNGLRESETHISTOGRAMEXTPROC glResetHistogramEXT = NULL;
  PFNGLRESETMINMAXEXTPROC glResetMinmaxEXT = NULL;

  // GL_EXT_convolution
  PFNGLCONVOLUTIONFILTER1DEXTPROC glConvolutionFilter1DEXT = NULL;
  PFNGLCONVOLUTIONFILTER2DEXTPROC glConvolutionFilter2DEXT = NULL;
  PFNGLCONVOLUTIONPARAMETERFEXTPROC glConvolutionParameterfEXT = NULL;
  PFNGLCONVOLUTIONPARAMETERFVEXTPROC glConvolutionParameterfvEXT = NULL;
  PFNGLCONVOLUTIONPARAMETERIEXTPROC glConvolutionParameteriEXT = NULL;
  PFNGLCONVOLUTIONPARAMETERIVEXTPROC glConvolutionParameterivEXT = NULL;
  PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC glCopyConvolutionFilter1DEXT = NULL;
  PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC glCopyConvolutionFilter2DEXT = NULL;
  PFNGLGETCONVOLUTIONFILTEREXTPROC glGetConvolutionFilterEXT = NULL;
  PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC glGetConvolutionParameterfvEXT = NULL;
  PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC glGetConvolutionParameterivEXT = NULL;
  PFNGLGETSEPARABLEFILTEREXTPROC glGetSeparableFilterEXT = NULL;
  PFNGLSEPARABLEFILTER2DEXTPROC glSeparableFilter2DEXT = NULL;

  // GL_SGI_color_table
  PFNGLCOLORTABLESGIPROC glColorTableSGI = NULL;
  PFNGLCOLORTABLEPARAMETERFVSGIPROC glColorTableParameterfvSGI = NULL;
  PFNGLCOLORTABLEPARAMETERIVSGIPROC glColorTableParameterivSGI = NULL;
  PFNGLCOPYCOLORTABLESGIPROC glCopyColorTableSGI = NULL;
  PFNGLGETCOLORTABLESGIPROC glGetColorTableSGI = NULL;
  PFNGLGETCOLORTABLEPARAMETERFVSGIPROC glGetColorTableParameterfvSGI = NULL;
  PFNGLGETCOLORTABLEPARAMETERIVSGIPROC glGetColorTableParameterivSGI = NULL;

  // GL_SGIX_pixel_texture
  PFNGLPIXELTEXGENSGIXPROC glPixelTexGenSGIX = NULL;

  // GL_SGIS_pixel_texture
  PFNGLPIXELTEXGENPARAMETERISGISPROC glPixelTexGenParameteriSGIS = NULL;
  PFNGLPIXELTEXGENPARAMETERIVSGISPROC glPixelTexGenParameterivSGIS = NULL;
  PFNGLPIXELTEXGENPARAMETERFSGISPROC glPixelTexGenParameterfSGIS = NULL;
  PFNGLPIXELTEXGENPARAMETERFVSGISPROC glPixelTexGenParameterfvSGIS = NULL;
  PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC glGetPixelTexGenParameterivSGIS = NULL;
  PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC glGetPixelTexGenParameterfvSGIS = NULL;

  // GL_SGIS_texture4D
  PFNGLTEXIMAGE4DSGISPROC glTexImage4DSGIS = NULL;
  PFNGLTEXSUBIMAGE4DSGISPROC glTexSubImage4DSGIS = NULL;

  // GL_EXT_texture_object
  PFNGLARETEXTURESRESIDENTEXTPROC glAreTexturesResidentEXT = NULL;
  PFNGLBINDTEXTUREEXTPROC glBindTextureEXT = NULL;
  PFNGLDELETETEXTURESEXTPROC glDeleteTexturesEXT = NULL;
  PFNGLGENTEXTURESEXTPROC glGenTexturesEXT = NULL;
  PFNGLISTEXTUREEXTPROC glIsTextureEXT = NULL;
  PFNGLPRIORITIZETEXTURESEXTPROC glPrioritizeTexturesEXT = NULL;

  // GL_SGIS_detail_texture
  PFNGLDETAILTEXFUNCSGISPROC glDetailTexFuncSGIS = NULL;
  PFNGLGETDETAILTEXFUNCSGISPROC glGetDetailTexFuncSGIS = NULL;

  // GL_SGIS_sharpen_texture
  PFNGLSHARPENTEXFUNCSGISPROC glSharpenTexFuncSGIS = NULL;
  PFNGLGETSHARPENTEXFUNCSGISPROC glGetSharpenTexFuncSGIS = NULL;

  // GL_SGIS_multisample
  PFNGLSAMPLEMASKSGISPROC glSampleMaskSGIS = NULL;
  PFNGLSAMPLEPATTERNSGISPROC glSamplePatternSGIS = NULL;

  // GL_EXT_vertex_array
  PFNGLARRAYELEMENTEXTPROC glArrayElementEXT = NULL;
  PFNGLCOLORPOINTEREXTPROC glColorPointerEXT = NULL;
  PFNGLDRAWARRAYSEXTPROC glDrawArraysEXT = NULL;
  PFNGLEDGEFLAGPOINTEREXTPROC glEdgeFlagPointerEXT = NULL;
  PFNGLGETPOINTERVEXTPROC glGetPointervEXT = NULL;
  PFNGLINDEXPOINTEREXTPROC glIndexPointerEXT = NULL;
  PFNGLNORMALPOINTEREXTPROC glNormalPointerEXT = NULL;
  PFNGLTEXCOORDPOINTEREXTPROC glTexCoordPointerEXT = NULL;
  PFNGLVERTEXPOINTEREXTPROC glVertexPointerEXT = NULL;

  // GL_EXT_blend_minmax
  PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT = NULL;

  // GL_SGIX_sprite
  PFNGLSPRITEPARAMETERFSGIXPROC glSpriteParameterfSGIX = NULL;
  PFNGLSPRITEPARAMETERFVSGIXPROC glSpriteParameterfvSGIX = NULL;
  PFNGLSPRITEPARAMETERISGIXPROC glSpriteParameteriSGIX = NULL;
  PFNGLSPRITEPARAMETERIVSGIXPROC glSpriteParameterivSGIX = NULL;

  // GL_EXT_point_parameters
  PFNGLPOINTPARAMETERFEXTPROC glPointParameterfEXT = NULL;
  PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT = NULL;

  // GL_SGIS_point_parameters
  PFNGLPOINTPARAMETERFVSGISPROC glPointParameterfvSGIS = NULL;

  // GL_SGIX_instruments
  PFNGLGETINSTRUMENTSSGIXPROC glGetInstrumentsSGIX = NULL;
  PFNGLINSTRUMENTSBUFFERSGIXPROC glInstrumentsBufferSGIX = NULL;
  PFNGLPOLLINSTRUMENTSSGIXPROC glPollInstrumentsSGIX = NULL;
  PFNGLREADINSTRUMENTSSGIXPROC glReadInstrumentsSGIX = NULL;
  PFNGLSTARTINSTRUMENTSSGIXPROC glStartInstrumentsSGIX = NULL;
  PFNGLSTOPINSTRUMENTSSGIXPROC glStopInstrumentsSGIX = NULL;

  // GL_SGIX_framezoom
  PFNGLFRAMEZOOMSGIXPROC glFrameZoomSGIX = NULL;

  // GL_SGIX_tag_sample_buffer
  PFNGLTAGSAMPLEBUFFERSGIXPROC glTagSampleBufferSGIX = NULL;

  // GL_SGIX_polynomial_ffd
  PFNGLDEFORMATIONMAP3DSGIXPROC glDeformationMap3dSGIX = NULL;
  PFNGLDEFORMATIONMAP3FSGIXPROC glDeformationMap3fSGIX = NULL;
  PFNGLDEFORMSGIXPROC glDeformSGIX = NULL;
  PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC glLoadIdentityDeformationMapSGIX = NULL;

  // GL_SGIX_reference_plane
  PFNGLREFERENCEPLANESGIXPROC glReferencePlaneSGIX = NULL;

  // GL_SGIX_flush_raster
  PFNGLFLUSHRASTERSGIXPROC glFlushRasterSGIX = NULL;

  // GL_SGIS_fog_function
  PFNGLFOGFUNCSGISPROC glFogFuncSGIS = NULL;
  PFNGLGETFOGFUNCSGISPROC glGetFogFuncSGIS = NULL;

  // GL_HP_image_transform
  PFNGLIMAGETRANSFORMPARAMETERIHPPROC glImageTransformParameteriHP = NULL;
  PFNGLIMAGETRANSFORMPARAMETERFHPPROC glImageTransformParameterfHP = NULL;
  PFNGLIMAGETRANSFORMPARAMETERIVHPPROC glImageTransformParameterivHP = NULL;
  PFNGLIMAGETRANSFORMPARAMETERFVHPPROC glImageTransformParameterfvHP = NULL;
  PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC glGetImageTransformParameterivHP = NULL;
  PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC glGetImageTransformParameterfvHP = NULL;

  // GL_EXT_color_subtable
  PFNGLCOLORSUBTABLEEXTPROC glColorSubTableEXT = NULL;
  PFNGLCOPYCOLORSUBTABLEEXTPROC glCopyColorSubTableEXT = NULL;

  // GL_PGI_misc_hints
  PFNGLHINTPGIPROC glHintPGI = NULL;

  // GL_EXT_paletted_texture
  PFNGLCOLORTABLEEXTPROC glColorTableEXT = NULL;
  PFNGLGETCOLORTABLEEXTPROC glGetColorTableEXT = NULL;
  PFNGLGETCOLORTABLEPARAMETERIVEXTPROC glGetColorTableParameterivEXT = NULL;
  PFNGLGETCOLORTABLEPARAMETERFVEXTPROC glGetColorTableParameterfvEXT = NULL;

  // GL_SGIX_list_priority
  PFNGLGETLISTPARAMETERFVSGIXPROC glGetListParameterfvSGIX = NULL;
  PFNGLGETLISTPARAMETERIVSGIXPROC glGetListParameterivSGIX = NULL;
  PFNGLLISTPARAMETERFSGIXPROC glListParameterfSGIX = NULL;
  PFNGLLISTPARAMETERFVSGIXPROC glListParameterfvSGIX = NULL;
  PFNGLLISTPARAMETERISGIXPROC glListParameteriSGIX = NULL;
  PFNGLLISTPARAMETERIVSGIXPROC glListParameterivSGIX = NULL;

  // GL_EXT_index_material
  PFNGLINDEXMATERIALEXTPROC glIndexMaterialEXT = NULL;

  // GL_EXT_index_func
  PFNGLINDEXFUNCEXTPROC glIndexFuncEXT = NULL;

  // GL_EXT_compiled_vertex_array
  PFNGLLOCKARRAYSEXTPROC glLockArraysEXT = NULL;
  PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT = NULL;

  // GL_EXT_cull_vertex
  PFNGLCULLPARAMETERDVEXTPROC glCullParameterdvEXT = NULL;
  PFNGLCULLPARAMETERFVEXTPROC glCullParameterfvEXT = NULL;

  // GL_SGIX_fragment_lighting
  PFNGLFRAGMENTCOLORMATERIALSGIXPROC glFragmentColorMaterialSGIX = NULL;
  PFNGLFRAGMENTLIGHTFSGIXPROC glFragmentLightfSGIX = NULL;
  PFNGLFRAGMENTLIGHTFVSGIXPROC glFragmentLightfvSGIX = NULL;
  PFNGLFRAGMENTLIGHTISGIXPROC glFragmentLightiSGIX = NULL;
  PFNGLFRAGMENTLIGHTIVSGIXPROC glFragmentLightivSGIX = NULL;
  PFNGLFRAGMENTLIGHTMODELFSGIXPROC glFragmentLightModelfSGIX = NULL;
  PFNGLFRAGMENTLIGHTMODELFVSGIXPROC glFragmentLightModelfvSGIX = NULL;
  PFNGLFRAGMENTLIGHTMODELISGIXPROC glFragmentLightModeliSGIX = NULL;
  PFNGLFRAGMENTLIGHTMODELIVSGIXPROC glFragmentLightModelivSGIX = NULL;
  PFNGLFRAGMENTMATERIALFSGIXPROC glFragmentMaterialfSGIX = NULL;
  PFNGLFRAGMENTMATERIALFVSGIXPROC glFragmentMaterialfvSGIX = NULL;
  PFNGLFRAGMENTMATERIALISGIXPROC glFragmentMaterialiSGIX = NULL;
  PFNGLFRAGMENTMATERIALIVSGIXPROC glFragmentMaterialivSGIX = NULL;
  PFNGLGETFRAGMENTLIGHTFVSGIXPROC glGetFragmentLightfvSGIX = NULL;
  PFNGLGETFRAGMENTLIGHTIVSGIXPROC glGetFragmentLightivSGIX = NULL;
  PFNGLGETFRAGMENTMATERIALFVSGIXPROC glGetFragmentMaterialfvSGIX = NULL;
  PFNGLGETFRAGMENTMATERIALIVSGIXPROC glGetFragmentMaterialivSGIX = NULL;
  PFNGLLIGHTENVISGIXPROC glLightEnviSGIX = NULL;

  // GL_EXT_draw_range_elements
  PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT = NULL;

  // GL_EXT_light_texture
  PFNGLAPPLYTEXTUREEXTPROC glApplyTextureEXT = NULL;
  PFNGLTEXTURELIGHTEXTPROC glTextureLightEXT = NULL;
  PFNGLTEXTUREMATERIALEXTPROC glTextureMaterialEXT = NULL;

  // GL_SGIX_async
  PFNGLASYNCMARKERSGIXPROC glAsyncMarkerSGIX = NULL;
  PFNGLFINISHASYNCSGIXPROC glFinishAsyncSGIX = NULL;
  PFNGLPOLLASYNCSGIXPROC glPollAsyncSGIX = NULL;
  PFNGLGENASYNCMARKERSSGIXPROC glGenAsyncMarkersSGIX = NULL;
  PFNGLDELETEASYNCMARKERSSGIXPROC glDeleteAsyncMarkersSGIX = NULL;
  PFNGLISASYNCMARKERSGIXPROC glIsAsyncMarkerSGIX = NULL;

  // GL_INTEL_parallel_arrays
  PFNGLVERTEXPOINTERVINTELPROC glVertexPointervINTEL = NULL;
  PFNGLNORMALPOINTERVINTELPROC glNormalPointervINTEL = NULL;
  PFNGLCOLORPOINTERVINTELPROC glColorPointervINTEL = NULL;
  PFNGLTEXCOORDPOINTERVINTELPROC glTexCoordPointervINTEL = NULL;

  // GL_EXT_pixel_transform
  PFNGLPIXELTRANSFORMPARAMETERIEXTPROC glPixelTransformParameteriEXT = NULL;
  PFNGLPIXELTRANSFORMPARAMETERFEXTPROC glPixelTransformParameterfEXT = NULL;
  PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC glPixelTransformParameterivEXT = NULL;
  PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC glPixelTransformParameterfvEXT = NULL;

  // GL_EXT_secondary_color
  PFNGLSECONDARYCOLOR3BEXTPROC glSecondaryColor3bEXT = NULL;
  PFNGLSECONDARYCOLOR3BVEXTPROC glSecondaryColor3bvEXT = NULL;
  PFNGLSECONDARYCOLOR3DEXTPROC glSecondaryColor3dEXT = NULL;
  PFNGLSECONDARYCOLOR3DVEXTPROC glSecondaryColor3dvEXT = NULL;
  PFNGLSECONDARYCOLOR3FEXTPROC glSecondaryColor3fEXT = NULL;
  PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT = NULL;
  PFNGLSECONDARYCOLOR3IEXTPROC glSecondaryColor3iEXT = NULL;
  PFNGLSECONDARYCOLOR3IVEXTPROC glSecondaryColor3ivEXT = NULL;
  PFNGLSECONDARYCOLOR3SEXTPROC glSecondaryColor3sEXT = NULL;
  PFNGLSECONDARYCOLOR3SVEXTPROC glSecondaryColor3svEXT = NULL;
  PFNGLSECONDARYCOLOR3UBEXTPROC glSecondaryColor3ubEXT = NULL;
  PFNGLSECONDARYCOLOR3UBVEXTPROC glSecondaryColor3ubvEXT = NULL;
  PFNGLSECONDARYCOLOR3UIEXTPROC glSecondaryColor3uiEXT = NULL;
  PFNGLSECONDARYCOLOR3UIVEXTPROC glSecondaryColor3uivEXT = NULL;
  PFNGLSECONDARYCOLOR3USEXTPROC glSecondaryColor3usEXT = NULL;
  PFNGLSECONDARYCOLOR3USVEXTPROC glSecondaryColor3usvEXT = NULL;
  PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT = NULL;

  // GL_EXT_texture_perturb_normal
  PFNGLTEXTURENORMALEXTPROC glTextureNormalEXT = NULL;

  // GL_EXT_multi_draw_arrays
  PFNGLMULTIDRAWARRAYSEXTPROC glMultiDrawArraysEXT = NULL;
  PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT = NULL;

  // GL_EXT_fog_coord
  PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;
  PFNGLFOGCOORDFVEXTPROC glFogCoordfvEXT = NULL;
  PFNGLFOGCOORDDEXTPROC glFogCoorddEXT = NULL;
  PFNGLFOGCOORDDVEXTPROC glFogCoorddvEXT = NULL;
  PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT = NULL;

  // GL_EXT_coordinate_frame
  PFNGLTANGENT3BEXTPROC glTangent3bEXT = NULL;
  PFNGLTANGENT3BVEXTPROC glTangent3bvEXT = NULL;
  PFNGLTANGENT3DEXTPROC glTangent3dEXT = NULL;
  PFNGLTANGENT3DVEXTPROC glTangent3dvEXT = NULL;
  PFNGLTANGENT3FEXTPROC glTangent3fEXT = NULL;
  PFNGLTANGENT3FVEXTPROC glTangent3fvEXT = NULL;
  PFNGLTANGENT3IEXTPROC glTangent3iEXT = NULL;
  PFNGLTANGENT3IVEXTPROC glTangent3ivEXT = NULL;
  PFNGLTANGENT3SEXTPROC glTangent3sEXT = NULL;
  PFNGLTANGENT3SVEXTPROC glTangent3svEXT = NULL;
  PFNGLBINORMAL3BEXTPROC glBinormal3bEXT = NULL;
  PFNGLBINORMAL3BVEXTPROC glBinormal3bvEXT = NULL;
  PFNGLBINORMAL3DEXTPROC glBinormal3dEXT = NULL;
  PFNGLBINORMAL3DVEXTPROC glBinormal3dvEXT = NULL;
  PFNGLBINORMAL3FEXTPROC glBinormal3fEXT = NULL;
  PFNGLBINORMAL3FVEXTPROC glBinormal3fvEXT = NULL;
  PFNGLBINORMAL3IEXTPROC glBinormal3iEXT = NULL;
  PFNGLBINORMAL3IVEXTPROC glBinormal3ivEXT = NULL;
  PFNGLBINORMAL3SEXTPROC glBinormal3sEXT = NULL;
  PFNGLBINORMAL3SVEXTPROC glBinormal3svEXT = NULL;
  PFNGLTANGENTPOINTEREXTPROC glTangentPointerEXT = NULL;
  PFNGLBINORMALPOINTEREXTPROC glBinormalPointerEXT = NULL;

  // GL_SUNX_constant_data
  PFNGLFINISHTEXTURESUNXPROC glFinishTextureSUNX = NULL;

  // GL_SUN_global_alpha
  PFNGLGLOBALALPHAFACTORBSUNPROC glGlobalAlphaFactorbSUN = NULL;
  PFNGLGLOBALALPHAFACTORSSUNPROC glGlobalAlphaFactorsSUN = NULL;
  PFNGLGLOBALALPHAFACTORISUNPROC glGlobalAlphaFactoriSUN = NULL;
  PFNGLGLOBALALPHAFACTORFSUNPROC glGlobalAlphaFactorfSUN = NULL;
  PFNGLGLOBALALPHAFACTORDSUNPROC glGlobalAlphaFactordSUN = NULL;
  PFNGLGLOBALALPHAFACTORUBSUNPROC glGlobalAlphaFactorubSUN = NULL;
  PFNGLGLOBALALPHAFACTORUSSUNPROC glGlobalAlphaFactorusSUN = NULL;
  PFNGLGLOBALALPHAFACTORUISUNPROC glGlobalAlphaFactoruiSUN = NULL;

  // GL_SUN_triangle_list
  PFNGLREPLACEMENTCODEUISUNPROC glReplacementCodeuiSUN = NULL;
  PFNGLREPLACEMENTCODEUSSUNPROC glReplacementCodeusSUN = NULL;
  PFNGLREPLACEMENTCODEUBSUNPROC glReplacementCodeubSUN = NULL;
  PFNGLREPLACEMENTCODEUIVSUNPROC glReplacementCodeuivSUN = NULL;
  PFNGLREPLACEMENTCODEUSVSUNPROC glReplacementCodeusvSUN = NULL;
  PFNGLREPLACEMENTCODEUBVSUNPROC glReplacementCodeubvSUN = NULL;
  PFNGLREPLACEMENTCODEPOINTERSUNPROC glReplacementCodePointerSUN = NULL;

  // GL_SUN_vertex
  PFNGLCOLOR4UBVERTEX2FSUNPROC glColor4ubVertex2fSUN = NULL;
  PFNGLCOLOR4UBVERTEX2FVSUNPROC glColor4ubVertex2fvSUN = NULL;
  PFNGLCOLOR4UBVERTEX3FSUNPROC glColor4ubVertex3fSUN = NULL;
  PFNGLCOLOR4UBVERTEX3FVSUNPROC glColor4ubVertex3fvSUN = NULL;
  PFNGLCOLOR3FVERTEX3FSUNPROC glColor3fVertex3fSUN = NULL;
  PFNGLCOLOR3FVERTEX3FVSUNPROC glColor3fVertex3fvSUN = NULL;
  PFNGLNORMAL3FVERTEX3FSUNPROC glNormal3fVertex3fSUN = NULL;
  PFNGLNORMAL3FVERTEX3FVSUNPROC glNormal3fVertex3fvSUN = NULL;
  PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC glColor4fNormal3fVertex3fSUN = NULL;
  PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC glColor4fNormal3fVertex3fvSUN = NULL;
  PFNGLTEXCOORD2FVERTEX3FSUNPROC glTexCoord2fVertex3fSUN = NULL;
  PFNGLTEXCOORD2FVERTEX3FVSUNPROC glTexCoord2fVertex3fvSUN = NULL;
  PFNGLTEXCOORD4FVERTEX4FSUNPROC glTexCoord4fVertex4fSUN = NULL;
  PFNGLTEXCOORD4FVERTEX4FVSUNPROC glTexCoord4fVertex4fvSUN = NULL;
  PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC glTexCoord2fColor4ubVertex3fSUN = NULL;
  PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC glTexCoord2fColor4ubVertex3fvSUN = NULL;
  PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC glTexCoord2fColor3fVertex3fSUN = NULL;
  PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC glTexCoord2fColor3fVertex3fvSUN = NULL;
  PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC glTexCoord2fNormal3fVertex3fSUN = NULL;
  PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC glTexCoord2fNormal3fVertex3fvSUN = NULL;
  PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC glTexCoord2fColor4fNormal3fVertex3fSUN = NULL;
  PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC glTexCoord2fColor4fNormal3fVertex3fvSUN = NULL;
  PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC glTexCoord4fColor4fNormal3fVertex4fSUN = NULL;
  PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC glTexCoord4fColor4fNormal3fVertex4fvSUN = NULL;
  PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC glReplacementCodeuiVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC glReplacementCodeuiVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC glReplacementCodeuiColor4ubVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC glReplacementCodeuiColor4ubVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC glReplacementCodeuiColor3fVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC glReplacementCodeuiColor3fVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC glReplacementCodeuiNormal3fVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiNormal3fVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC glReplacementCodeuiColor4fNormal3fVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiColor4fNormal3fVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC glReplacementCodeuiTexCoord2fVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN = NULL;
  PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN = NULL;
  PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN = NULL;

  // GL_EXT_blend_func_separate
  PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT = NULL;

  // GL_INGR_blend_func_separate
  PFNGLBLENDFUNCSEPARATEINGRPROC glBlendFuncSeparateINGR = NULL;

  // GL_EXT_vertex_weighting
  PFNGLVERTEXWEIGHTFEXTPROC glVertexWeightfEXT = NULL;
  PFNGLVERTEXWEIGHTFVEXTPROC glVertexWeightfvEXT = NULL;
  PFNGLVERTEXWEIGHTPOINTEREXTPROC glVertexWeightPointerEXT = NULL;

  // GL_NV_vertex_array_range
  PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV = NULL;
  PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV = NULL;

  // GL_NV_register_combiners
  PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV = NULL;
  PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV = NULL;
  PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV = NULL;
  PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV = NULL;
  PFNGLCOMBINERINPUTNVPROC glCombinerInputNV = NULL;
  PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV = NULL;
  PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV = NULL;
  PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV = NULL;
  PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV = NULL;
  PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV = NULL;
  PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV = NULL;
  PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV = NULL;
  PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV = NULL;

  // GL_MESA_resize_buffers
  PFNGLRESIZEBUFFERSMESAPROC glResizeBuffersMESA = NULL;

  // GL_MESA_window_pos
  PFNGLWINDOWPOS2DMESAPROC glWindowPos2dMESA = NULL;
  PFNGLWINDOWPOS2DVMESAPROC glWindowPos2dvMESA = NULL;
  PFNGLWINDOWPOS2FMESAPROC glWindowPos2fMESA = NULL;
  PFNGLWINDOWPOS2FVMESAPROC glWindowPos2fvMESA = NULL;
  PFNGLWINDOWPOS2IMESAPROC glWindowPos2iMESA = NULL;
  PFNGLWINDOWPOS2IVMESAPROC glWindowPos2ivMESA = NULL;
  PFNGLWINDOWPOS2SMESAPROC glWindowPos2sMESA = NULL;
  PFNGLWINDOWPOS2SVMESAPROC glWindowPos2svMESA = NULL;
  PFNGLWINDOWPOS3DMESAPROC glWindowPos3dMESA = NULL;
  PFNGLWINDOWPOS3DVMESAPROC glWindowPos3dvMESA = NULL;
  PFNGLWINDOWPOS3FMESAPROC glWindowPos3fMESA = NULL;
  PFNGLWINDOWPOS3FVMESAPROC glWindowPos3fvMESA = NULL;
  PFNGLWINDOWPOS3IMESAPROC glWindowPos3iMESA = NULL;
  PFNGLWINDOWPOS3IVMESAPROC glWindowPos3ivMESA = NULL;
  PFNGLWINDOWPOS3SMESAPROC glWindowPos3sMESA = NULL;
  PFNGLWINDOWPOS3SVMESAPROC glWindowPos3svMESA = NULL;
  PFNGLWINDOWPOS4DMESAPROC glWindowPos4dMESA = NULL;
  PFNGLWINDOWPOS4DVMESAPROC glWindowPos4dvMESA = NULL;
  PFNGLWINDOWPOS4FMESAPROC glWindowPos4fMESA = NULL;
  PFNGLWINDOWPOS4FVMESAPROC glWindowPos4fvMESA = NULL;
  PFNGLWINDOWPOS4IMESAPROC glWindowPos4iMESA = NULL;
  PFNGLWINDOWPOS4IVMESAPROC glWindowPos4ivMESA = NULL;
  PFNGLWINDOWPOS4SMESAPROC glWindowPos4sMESA = NULL;
  PFNGLWINDOWPOS4SVMESAPROC glWindowPos4svMESA = NULL;

  // GL_IBM_multimode_draw_arrays
  PFNGLMULTIMODEDRAWARRAYSIBMPROC glMultiModeDrawArraysIBM = NULL;
  PFNGLMULTIMODEDRAWELEMENTSIBMPROC glMultiModeDrawElementsIBM = NULL;

  // GL_IBM_vertex_array_lists
  PFNGLCOLORPOINTERLISTIBMPROC glColorPointerListIBM = NULL;
  PFNGLSECONDARYCOLORPOINTERLISTIBMPROC glSecondaryColorPointerListIBM = NULL;
  PFNGLEDGEFLAGPOINTERLISTIBMPROC glEdgeFlagPointerListIBM = NULL;
  PFNGLFOGCOORDPOINTERLISTIBMPROC glFogCoordPointerListIBM = NULL;
  PFNGLINDEXPOINTERLISTIBMPROC glIndexPointerListIBM = NULL;
  PFNGLNORMALPOINTERLISTIBMPROC glNormalPointerListIBM = NULL;
  PFNGLTEXCOORDPOINTERLISTIBMPROC glTexCoordPointerListIBM = NULL;
  PFNGLVERTEXPOINTERLISTIBMPROC glVertexPointerListIBM = NULL;

  // GL_3DFX_tbuffer
  PFNGLTBUFFERMASK3DFXPROC glTbufferMask3DFX = NULL;

  // GL_EXT_multisample
  PFNGLSAMPLEMASKEXTPROC glSampleMaskEXT = NULL;
  PFNGLSAMPLEPATTERNEXTPROC glSamplePatternEXT = NULL;

  // GL_SGIS_texture_color_mask
  PFNGLTEXTURECOLORMASKSGISPROC glTextureColorMaskSGIS = NULL;

  // GL_SGIX_igloo_interface
  PFNGLIGLOOINTERFACESGIXPROC glIglooInterfaceSGIX = NULL;

  // GL_NV_fence
  PFNGLDELETEFENCESNVPROC glDeleteFencesNV = NULL;
  PFNGLGENFENCESNVPROC glGenFencesNV = NULL;
  PFNGLISFENCENVPROC glIsFenceNV = NULL;
  PFNGLTESTFENCENVPROC glTestFenceNV = NULL;
  PFNGLGETFENCEIVNVPROC glGetFenceivNV = NULL;
  PFNGLFINISHFENCENVPROC glFinishFenceNV = NULL;
  PFNGLSETFENCENVPROC glSetFenceNV = NULL;

  // GL_NV_evaluators
  PFNGLMAPCONTROLPOINTSNVPROC glMapControlPointsNV = NULL;
  PFNGLMAPPARAMETERIVNVPROC glMapParameterivNV = NULL;
  PFNGLMAPPARAMETERFVNVPROC glMapParameterfvNV = NULL;
  PFNGLGETMAPCONTROLPOINTSNVPROC glGetMapControlPointsNV = NULL;
  PFNGLGETMAPPARAMETERIVNVPROC glGetMapParameterivNV = NULL;
  PFNGLGETMAPPARAMETERFVNVPROC glGetMapParameterfvNV = NULL;
  PFNGLGETMAPATTRIBPARAMETERIVNVPROC glGetMapAttribParameterivNV = NULL;
  PFNGLGETMAPATTRIBPARAMETERFVNVPROC glGetMapAttribParameterfvNV = NULL;
  PFNGLEVALMAPSNVPROC glEvalMapsNV = NULL;

  // GL_NV_register_combiners2
  PFNGLCOMBINERSTAGEPARAMETERFVNVPROC glCombinerStageParameterfvNV = NULL;
  PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC glGetCombinerStageParameterfvNV = NULL;

  // GL_NV_vertex_program
  PFNGLAREPROGRAMSRESIDENTNVPROC glAreProgramsResidentNV = NULL;
  PFNGLBINDPROGRAMNVPROC glBindProgramNV = NULL;
  PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV = NULL;
  PFNGLEXECUTEPROGRAMNVPROC glExecuteProgramNV = NULL;
  PFNGLGENPROGRAMSNVPROC glGenProgramsNV = NULL;
  PFNGLGETPROGRAMPARAMETERDVNVPROC glGetProgramParameterdvNV = NULL;
  PFNGLGETPROGRAMPARAMETERFVNVPROC glGetProgramParameterfvNV = NULL;
  PFNGLGETPROGRAMIVNVPROC glGetProgramivNV = NULL;
  PFNGLGETPROGRAMSTRINGNVPROC glGetProgramStringNV = NULL;
  PFNGLGETTRACKMATRIXIVNVPROC glGetTrackMatrixivNV = NULL;
  PFNGLGETVERTEXATTRIBDVNVPROC glGetVertexAttribdvNV = NULL;
  PFNGLGETVERTEXATTRIBFVNVPROC glGetVertexAttribfvNV = NULL;
  PFNGLGETVERTEXATTRIBIVNVPROC glGetVertexAttribivNV = NULL;
  PFNGLGETVERTEXATTRIBPOINTERVNVPROC glGetVertexAttribPointervNV = NULL;
  PFNGLISPROGRAMNVPROC glIsProgramNV = NULL;
  PFNGLLOADPROGRAMNVPROC glLoadProgramNV = NULL;
  PFNGLPROGRAMPARAMETER4DNVPROC glProgramParameter4dNV = NULL;
  PFNGLPROGRAMPARAMETER4DVNVPROC glProgramParameter4dvNV = NULL;
  PFNGLPROGRAMPARAMETER4FNVPROC glProgramParameter4fNV = NULL;
  PFNGLPROGRAMPARAMETER4FVNVPROC glProgramParameter4fvNV = NULL;
  PFNGLPROGRAMPARAMETERS4DVNVPROC glProgramParameters4dvNV = NULL;
  PFNGLPROGRAMPARAMETERS4FVNVPROC glProgramParameters4fvNV = NULL;
  PFNGLREQUESTRESIDENTPROGRAMSNVPROC glRequestResidentProgramsNV = NULL;
  PFNGLTRACKMATRIXNVPROC glTrackMatrixNV = NULL;
  PFNGLVERTEXATTRIBPOINTERNVPROC glVertexAttribPointerNV = NULL;
  PFNGLVERTEXATTRIB1DNVPROC glVertexAttrib1dNV = NULL;
  PFNGLVERTEXATTRIB1DVNVPROC glVertexAttrib1dvNV = NULL;
  PFNGLVERTEXATTRIB1FNVPROC glVertexAttrib1fNV = NULL;
  PFNGLVERTEXATTRIB1FVNVPROC glVertexAttrib1fvNV = NULL;
  PFNGLVERTEXATTRIB1SNVPROC glVertexAttrib1sNV = NULL;
  PFNGLVERTEXATTRIB1SVNVPROC glVertexAttrib1svNV = NULL;
  PFNGLVERTEXATTRIB2DNVPROC glVertexAttrib2dNV = NULL;
  PFNGLVERTEXATTRIB2DVNVPROC glVertexAttrib2dvNV = NULL;
  PFNGLVERTEXATTRIB2FNVPROC glVertexAttrib2fNV = NULL;
  PFNGLVERTEXATTRIB2FVNVPROC glVertexAttrib2fvNV = NULL;
  PFNGLVERTEXATTRIB2SNVPROC glVertexAttrib2sNV = NULL;
  PFNGLVERTEXATTRIB2SVNVPROC glVertexAttrib2svNV = NULL;
  PFNGLVERTEXATTRIB3DNVPROC glVertexAttrib3dNV = NULL;
  PFNGLVERTEXATTRIB3DVNVPROC glVertexAttrib3dvNV = NULL;
  PFNGLVERTEXATTRIB3FNVPROC glVertexAttrib3fNV = NULL;
  PFNGLVERTEXATTRIB3FVNVPROC glVertexAttrib3fvNV = NULL;
  PFNGLVERTEXATTRIB3SNVPROC glVertexAttrib3sNV = NULL;
  PFNGLVERTEXATTRIB3SVNVPROC glVertexAttrib3svNV = NULL;
  PFNGLVERTEXATTRIB4DNVPROC glVertexAttrib4dNV = NULL;
  PFNGLVERTEXATTRIB4DVNVPROC glVertexAttrib4dvNV = NULL;
  PFNGLVERTEXATTRIB4FNVPROC glVertexAttrib4fNV = NULL;
  PFNGLVERTEXATTRIB4FVNVPROC glVertexAttrib4fvNV = NULL;
  PFNGLVERTEXATTRIB4SNVPROC glVertexAttrib4sNV = NULL;
  PFNGLVERTEXATTRIB4SVNVPROC glVertexAttrib4svNV = NULL;
  PFNGLVERTEXATTRIB4UBNVPROC glVertexAttrib4ubNV = NULL;
  PFNGLVERTEXATTRIB4UBVNVPROC glVertexAttrib4ubvNV = NULL;
  PFNGLVERTEXATTRIBS1DVNVPROC glVertexAttribs1dvNV = NULL;
  PFNGLVERTEXATTRIBS1FVNVPROC glVertexAttribs1fvNV = NULL;
  PFNGLVERTEXATTRIBS1SVNVPROC glVertexAttribs1svNV = NULL;
  PFNGLVERTEXATTRIBS2DVNVPROC glVertexAttribs2dvNV = NULL;
  PFNGLVERTEXATTRIBS2FVNVPROC glVertexAttribs2fvNV = NULL;
  PFNGLVERTEXATTRIBS2SVNVPROC glVertexAttribs2svNV = NULL;
  PFNGLVERTEXATTRIBS3DVNVPROC glVertexAttribs3dvNV = NULL;
  PFNGLVERTEXATTRIBS3FVNVPROC glVertexAttribs3fvNV = NULL;
  PFNGLVERTEXATTRIBS3SVNVPROC glVertexAttribs3svNV = NULL;
  PFNGLVERTEXATTRIBS4DVNVPROC glVertexAttribs4dvNV = NULL;
  PFNGLVERTEXATTRIBS4FVNVPROC glVertexAttribs4fvNV = NULL;
  PFNGLVERTEXATTRIBS4SVNVPROC glVertexAttribs4svNV = NULL;
  PFNGLVERTEXATTRIBS4UBVNVPROC glVertexAttribs4ubvNV = NULL;

  // GL_ATI_envmap_bumpmap
  PFNGLTEXBUMPPARAMETERIVATIPROC glTexBumpParameterivATI = NULL;
  PFNGLTEXBUMPPARAMETERFVATIPROC glTexBumpParameterfvATI = NULL;
  PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI = NULL;
  PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI = NULL;

  // GL_ATI_fragment_shader
  PFNGLGENFRAGMENTSHADERSATIPROC glGenFragmentShadersATI = NULL;
  PFNGLBINDFRAGMENTSHADERATIPROC glBindFragmentShaderATI = NULL;
  PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI = NULL;
  PFNGLBEGINFRAGMENTSHADERATIPROC glBeginFragmentShaderATI = NULL;
  PFNGLENDFRAGMENTSHADERATIPROC glEndFragmentShaderATI = NULL;
  PFNGLPASSTEXCOORDATIPROC glPassTexCoordATI = NULL;
  PFNGLSAMPLEMAPATIPROC glSampleMapATI = NULL;
  PFNGLCOLORFRAGMENTOP1ATIPROC glColorFragmentOp1ATI = NULL;
  PFNGLCOLORFRAGMENTOP2ATIPROC glColorFragmentOp2ATI = NULL;
  PFNGLCOLORFRAGMENTOP3ATIPROC glColorFragmentOp3ATI = NULL;
  PFNGLALPHAFRAGMENTOP1ATIPROC glAlphaFragmentOp1ATI = NULL;
  PFNGLALPHAFRAGMENTOP2ATIPROC glAlphaFragmentOp2ATI = NULL;
  PFNGLALPHAFRAGMENTOP3ATIPROC glAlphaFragmentOp3ATI = NULL;
  PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI = NULL;

  // GL_ATI_pn_triangles
  PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI = NULL;
  PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI = NULL;

  // GL_ATI_vertex_array_object
  PFNGLNEWOBJECTBUFFERATIPROC glNewObjectBufferATI = NULL;
  PFNGLISOBJECTBUFFERATIPROC glIsObjectBufferATI = NULL;
  PFNGLUPDATEOBJECTBUFFERATIPROC glUpdateObjectBufferATI = NULL;
  PFNGLGETOBJECTBUFFERFVATIPROC glGetObjectBufferfvATI = NULL;
  PFNGLGETOBJECTBUFFERIVATIPROC glGetObjectBufferivATI = NULL;
  PFNGLFREEOBJECTBUFFERATIPROC glFreeObjectBufferATI = NULL;
  PFNGLARRAYOBJECTATIPROC glArrayObjectATI = NULL;
  PFNGLGETARRAYOBJECTFVATIPROC glGetArrayObjectfvATI = NULL;
  PFNGLGETARRAYOBJECTIVATIPROC glGetArrayObjectivATI = NULL;
  PFNGLVARIANTARRAYOBJECTATIPROC glVariantArrayObjectATI = NULL;
  PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI = NULL;
  PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI = NULL;

  // GL_EXT_vertex_shader
  PFNGLBEGINVERTEXSHADEREXTPROC glBeginVertexShaderEXT = NULL;
  PFNGLENDVERTEXSHADEREXTPROC glEndVertexShaderEXT = NULL;
  PFNGLBINDVERTEXSHADEREXTPROC glBindVertexShaderEXT = NULL;
  PFNGLGENVERTEXSHADERSEXTPROC glGenVertexShadersEXT = NULL;
  PFNGLDELETEVERTEXSHADEREXTPROC glDeleteVertexShaderEXT = NULL;
  PFNGLSHADEROP1EXTPROC glShaderOp1EXT = NULL;
  PFNGLSHADEROP2EXTPROC glShaderOp2EXT = NULL;
  PFNGLSHADEROP3EXTPROC glShaderOp3EXT = NULL;
  PFNGLSWIZZLEEXTPROC glSwizzleEXT = NULL;
  PFNGLWRITEMASKEXTPROC glWriteMaskEXT = NULL;
  PFNGLINSERTCOMPONENTEXTPROC glInsertComponentEXT = NULL;
  PFNGLEXTRACTCOMPONENTEXTPROC glExtractComponentEXT = NULL;
  PFNGLGENSYMBOLSEXTPROC glGenSymbolsEXT = NULL;
  PFNGLSETINVARIANTEXTPROC glSetInvariantEXT = NULL;
  PFNGLSETLOCALCONSTANTEXTPROC glSetLocalConstantEXT = NULL;
  PFNGLVARIANTBVEXTPROC glVariantbvEXT = NULL;
  PFNGLVARIANTSVEXTPROC glVariantsvEXT = NULL;
  PFNGLVARIANTIVEXTPROC glVariantivEXT = NULL;
  PFNGLVARIANTFVEXTPROC glVariantfvEXT = NULL;
  PFNGLVARIANTDVEXTPROC glVariantdvEXT = NULL;
  PFNGLVARIANTUBVEXTPROC glVariantubvEXT = NULL;
  PFNGLVARIANTUSVEXTPROC glVariantusvEXT = NULL;
  PFNGLVARIANTUIVEXTPROC glVariantuivEXT = NULL;
  PFNGLVARIANTPOINTEREXTPROC glVariantPointerEXT = NULL;
  PFNGLENABLEVARIANTCLIENTSTATEEXTPROC glEnableVariantClientStateEXT = NULL;
  PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT = NULL;
  PFNGLBINDLIGHTPARAMETEREXTPROC glBindLightParameterEXT = NULL;
  PFNGLBINDMATERIALPARAMETEREXTPROC glBindMaterialParameterEXT = NULL;
  PFNGLBINDTEXGENPARAMETEREXTPROC glBindTexGenParameterEXT = NULL;
  PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT = NULL;
  PFNGLBINDPARAMETEREXTPROC glBindParameterEXT = NULL;
  PFNGLISVARIANTENABLEDEXTPROC glIsVariantEnabledEXT = NULL;
  PFNGLGETVARIANTBOOLEANVEXTPROC glGetVariantBooleanvEXT = NULL;
  PFNGLGETVARIANTINTEGERVEXTPROC glGetVariantIntegervEXT = NULL;
  PFNGLGETVARIANTFLOATVEXTPROC glGetVariantFloatvEXT = NULL;
  PFNGLGETVARIANTPOINTERVEXTPROC glGetVariantPointervEXT = NULL;
  PFNGLGETINVARIANTBOOLEANVEXTPROC glGetInvariantBooleanvEXT = NULL;
  PFNGLGETINVARIANTINTEGERVEXTPROC glGetInvariantIntegervEXT = NULL;
  PFNGLGETINVARIANTFLOATVEXTPROC glGetInvariantFloatvEXT = NULL;
  PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT = NULL;
  PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT = NULL;
  PFNGLGETLOCALCONSTANTFLOATVEXTPROC glGetLocalConstantFloatvEXT = NULL;

  // GL_ATI_vertex_streams
  PFNGLVERTEXSTREAM1SATIPROC glVertexStream1sATI = NULL;
  PFNGLVERTEXSTREAM1SVATIPROC glVertexStream1svATI = NULL;
  PFNGLVERTEXSTREAM1IATIPROC glVertexStream1iATI = NULL;
  PFNGLVERTEXSTREAM1IVATIPROC glVertexStream1ivATI = NULL;
  PFNGLVERTEXSTREAM1FATIPROC glVertexStream1fATI = NULL;
  PFNGLVERTEXSTREAM1FVATIPROC glVertexStream1fvATI = NULL;
  PFNGLVERTEXSTREAM1DATIPROC glVertexStream1dATI = NULL;
  PFNGLVERTEXSTREAM1DVATIPROC glVertexStream1dvATI = NULL;
  PFNGLVERTEXSTREAM2SATIPROC glVertexStream2sATI = NULL;
  PFNGLVERTEXSTREAM2SVATIPROC glVertexStream2svATI = NULL;
  PFNGLVERTEXSTREAM2IATIPROC glVertexStream2iATI = NULL;
  PFNGLVERTEXSTREAM2IVATIPROC glVertexStream2ivATI = NULL;
  PFNGLVERTEXSTREAM2FATIPROC glVertexStream2fATI = NULL;
  PFNGLVERTEXSTREAM2FVATIPROC glVertexStream2fvATI = NULL;
  PFNGLVERTEXSTREAM2DATIPROC glVertexStream2dATI = NULL;
  PFNGLVERTEXSTREAM2DVATIPROC glVertexStream2dvATI = NULL;
  PFNGLVERTEXSTREAM3SATIPROC glVertexStream3sATI = NULL;
  PFNGLVERTEXSTREAM3SVATIPROC glVertexStream3svATI = NULL;
  PFNGLVERTEXSTREAM3IATIPROC glVertexStream3iATI = NULL;
  PFNGLVERTEXSTREAM3IVATIPROC glVertexStream3ivATI = NULL;
  PFNGLVERTEXSTREAM3FATIPROC glVertexStream3fATI = NULL;
  PFNGLVERTEXSTREAM3FVATIPROC glVertexStream3fvATI = NULL;
  PFNGLVERTEXSTREAM3DATIPROC glVertexStream3dATI = NULL;
  PFNGLVERTEXSTREAM3DVATIPROC glVertexStream3dvATI = NULL;
  PFNGLVERTEXSTREAM4SATIPROC glVertexStream4sATI = NULL;
  PFNGLVERTEXSTREAM4SVATIPROC glVertexStream4svATI = NULL;
  PFNGLVERTEXSTREAM4IATIPROC glVertexStream4iATI = NULL;
  PFNGLVERTEXSTREAM4IVATIPROC glVertexStream4ivATI = NULL;
  PFNGLVERTEXSTREAM4FATIPROC glVertexStream4fATI = NULL;
  PFNGLVERTEXSTREAM4FVATIPROC glVertexStream4fvATI = NULL;
  PFNGLVERTEXSTREAM4DATIPROC glVertexStream4dATI = NULL;
  PFNGLVERTEXSTREAM4DVATIPROC glVertexStream4dvATI = NULL;
  PFNGLNORMALSTREAM3BATIPROC glNormalStream3bATI = NULL;
  PFNGLNORMALSTREAM3BVATIPROC glNormalStream3bvATI = NULL;
  PFNGLNORMALSTREAM3SATIPROC glNormalStream3sATI = NULL;
  PFNGLNORMALSTREAM3SVATIPROC glNormalStream3svATI = NULL;
  PFNGLNORMALSTREAM3IATIPROC glNormalStream3iATI = NULL;
  PFNGLNORMALSTREAM3IVATIPROC glNormalStream3ivATI = NULL;
  PFNGLNORMALSTREAM3FATIPROC glNormalStream3fATI = NULL;
  PFNGLNORMALSTREAM3FVATIPROC glNormalStream3fvATI = NULL;
  PFNGLNORMALSTREAM3DATIPROC glNormalStream3dATI = NULL;
  PFNGLNORMALSTREAM3DVATIPROC glNormalStream3dvATI = NULL;
  PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC glClientActiveVertexStreamATI = NULL;
  PFNGLVERTEXBLENDENVIATIPROC glVertexBlendEnviATI = NULL;
  PFNGLVERTEXBLENDENVFATIPROC glVertexBlendEnvfATI = NULL;

  // GL_ATI_element_array
  PFNGLELEMENTPOINTERATIPROC glElementPointerATI = NULL;
  PFNGLDRAWELEMENTARRAYATIPROC glDrawElementArrayATI = NULL;
  PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI = NULL;

  // GL_SUN_mesh_array
  PFNGLDRAWMESHARRAYSSUNPROC glDrawMeshArraysSUN = NULL;

  // GL_NV_occlusion_query
  PFNGLGENOCCLUSIONQUERIESNVPROC glGenOcclusionQueriesNV = NULL;
  PFNGLDELETEOCCLUSIONQUERIESNVPROC glDeleteOcclusionQueriesNV = NULL;
  PFNGLISOCCLUSIONQUERYNVPROC glIsOcclusionQueryNV = NULL;
  PFNGLBEGINOCCLUSIONQUERYNVPROC glBeginOcclusionQueryNV = NULL;
  PFNGLENDOCCLUSIONQUERYNVPROC glEndOcclusionQueryNV = NULL;
  PFNGLGETOCCLUSIONQUERYIVNVPROC glGetOcclusionQueryivNV = NULL;
  PFNGLGETOCCLUSIONQUERYUIVNVPROC glGetOcclusionQueryuivNV = NULL;

  // GL_NV_point_sprite
  PFNGLPOINTPARAMETERINVPROC glPointParameteriNV = NULL;
  PFNGLPOINTPARAMETERIVNVPROC glPointParameterivNV = NULL;

  // GL_EXT_stencil_two_side
  PFNGLACTIVESTENCILFACEEXTPROC glActiveStencilFaceEXT = NULL;

  // GL_APPLE_element_array
  PFNGLELEMENTPOINTERAPPLEPROC glElementPointerAPPLE = NULL;
  PFNGLDRAWELEMENTARRAYAPPLEPROC glDrawElementArrayAPPLE = NULL;
  PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC glDrawRangeElementArrayAPPLE = NULL;
  PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC glMultiDrawElementArrayAPPLE = NULL;
  PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC glMultiDrawRangeElementArrayAPPLE = NULL;

  // GL_APPLE_fence
  PFNGLGENFENCESAPPLEPROC glGenFencesAPPLE = NULL;
  PFNGLDELETEFENCESAPPLEPROC glDeleteFencesAPPLE = NULL;
  PFNGLSETFENCEAPPLEPROC glSetFenceAPPLE = NULL;
  PFNGLISFENCEAPPLEPROC glIsFenceAPPLE = NULL;
  PFNGLTESTFENCEAPPLEPROC glTestFenceAPPLE = NULL;
  PFNGLFINISHFENCEAPPLEPROC glFinishFenceAPPLE = NULL;
  PFNGLTESTOBJECTAPPLEPROC glTestObjectAPPLE = NULL;
  PFNGLFINISHOBJECTAPPLEPROC glFinishObjectAPPLE = NULL;

  // GL_APPLE_vertex_array_object
  PFNGLBINDVERTEXARRAYAPPLEPROC glBindVertexArrayAPPLE = NULL;
  PFNGLDELETEVERTEXARRAYSAPPLEPROC glDeleteVertexArraysAPPLE = NULL;
  PFNGLGENVERTEXARRAYSAPPLEPROC glGenVertexArraysAPPLE = NULL;
  PFNGLISVERTEXARRAYAPPLEPROC glIsVertexArrayAPPLE = NULL;

  // GL_APPLE_vertex_array_range
  PFNGLVERTEXARRAYRANGEAPPLEPROC glVertexArrayRangeAPPLE = NULL;
  PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC glFlushVertexArrayRangeAPPLE = NULL;
  PFNGLVERTEXARRAYPARAMETERIAPPLEPROC glVertexArrayParameteriAPPLE = NULL;

  // GL_ATI_draw_buffers
  PFNGLDRAWBUFFERSATIPROC glDrawBuffersATI = NULL;

  // GL_NV_fragment_program
  /* Some NV_fragment_program entry points are shared with ARB_vertex_program. */
  PFNGLPROGRAMNAMEDPARAMETER4FNVPROC glProgramNamedParameter4fNV = NULL;
  PFNGLPROGRAMNAMEDPARAMETER4DNVPROC glProgramNamedParameter4dNV = NULL;
  PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC glProgramNamedParameter4fvNV = NULL;
  PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC glProgramNamedParameter4dvNV = NULL;
  PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC glGetProgramNamedParameterfvNV = NULL;
  PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC glGetProgramNamedParameterdvNV = NULL;

  // GL_NV_half_float
  PFNGLVERTEX2HNVPROC glVertex2hNV = NULL;
  PFNGLVERTEX2HVNVPROC glVertex2hvNV = NULL;
  PFNGLVERTEX3HNVPROC glVertex3hNV = NULL;
  PFNGLVERTEX3HVNVPROC glVertex3hvNV = NULL;
  PFNGLVERTEX4HNVPROC glVertex4hNV = NULL;
  PFNGLVERTEX4HVNVPROC glVertex4hvNV = NULL;
  PFNGLNORMAL3HNVPROC glNormal3hNV = NULL;
  PFNGLNORMAL3HVNVPROC glNormal3hvNV = NULL;
  PFNGLCOLOR3HNVPROC glColor3hNV = NULL;
  PFNGLCOLOR3HVNVPROC glColor3hvNV = NULL;
  PFNGLCOLOR4HNVPROC glColor4hNV = NULL;
  PFNGLCOLOR4HVNVPROC glColor4hvNV = NULL;
  PFNGLTEXCOORD1HNVPROC glTexCoord1hNV = NULL;
  PFNGLTEXCOORD1HVNVPROC glTexCoord1hvNV = NULL;
  PFNGLTEXCOORD2HNVPROC glTexCoord2hNV = NULL;
  PFNGLTEXCOORD2HVNVPROC glTexCoord2hvNV = NULL;
  PFNGLTEXCOORD3HNVPROC glTexCoord3hNV = NULL;
  PFNGLTEXCOORD3HVNVPROC glTexCoord3hvNV = NULL;
  PFNGLTEXCOORD4HNVPROC glTexCoord4hNV = NULL;
  PFNGLTEXCOORD4HVNVPROC glTexCoord4hvNV = NULL;
  PFNGLMULTITEXCOORD1HNVPROC glMultiTexCoord1hNV = NULL;
  PFNGLMULTITEXCOORD1HVNVPROC glMultiTexCoord1hvNV = NULL;
  PFNGLMULTITEXCOORD2HNVPROC glMultiTexCoord2hNV = NULL;
  PFNGLMULTITEXCOORD2HVNVPROC glMultiTexCoord2hvNV = NULL;
  PFNGLMULTITEXCOORD3HNVPROC glMultiTexCoord3hNV = NULL;
  PFNGLMULTITEXCOORD3HVNVPROC glMultiTexCoord3hvNV = NULL;
  PFNGLMULTITEXCOORD4HNVPROC glMultiTexCoord4hNV = NULL;
  PFNGLMULTITEXCOORD4HVNVPROC glMultiTexCoord4hvNV = NULL;
  PFNGLFOGCOORDHNVPROC glFogCoordhNV = NULL;
  PFNGLFOGCOORDHVNVPROC glFogCoordhvNV = NULL;
  PFNGLSECONDARYCOLOR3HNVPROC glSecondaryColor3hNV = NULL;
  PFNGLSECONDARYCOLOR3HVNVPROC glSecondaryColor3hvNV = NULL;
  PFNGLVERTEXWEIGHTHNVPROC glVertexWeighthNV = NULL;
  PFNGLVERTEXWEIGHTHVNVPROC glVertexWeighthvNV = NULL;
  PFNGLVERTEXATTRIB1HNVPROC glVertexAttrib1hNV = NULL;
  PFNGLVERTEXATTRIB1HVNVPROC glVertexAttrib1hvNV = NULL;
  PFNGLVERTEXATTRIB2HNVPROC glVertexAttrib2hNV = NULL;
  PFNGLVERTEXATTRIB2HVNVPROC glVertexAttrib2hvNV = NULL;
  PFNGLVERTEXATTRIB3HNVPROC glVertexAttrib3hNV = NULL;
  PFNGLVERTEXATTRIB3HVNVPROC glVertexAttrib3hvNV = NULL;
  PFNGLVERTEXATTRIB4HNVPROC glVertexAttrib4hNV = NULL;
  PFNGLVERTEXATTRIB4HVNVPROC glVertexAttrib4hvNV = NULL;
  PFNGLVERTEXATTRIBS1HVNVPROC glVertexAttribs1hvNV = NULL;
  PFNGLVERTEXATTRIBS2HVNVPROC glVertexAttribs2hvNV = NULL;
  PFNGLVERTEXATTRIBS3HVNVPROC glVertexAttribs3hvNV = NULL;
  PFNGLVERTEXATTRIBS4HVNVPROC glVertexAttribs4hvNV = NULL;

  // GL_NV_pixel_data_range
  PFNGLPIXELDATARANGENVPROC glPixelDataRangeNV = NULL;
  PFNGLFLUSHPIXELDATARANGENVPROC glFlushPixelDataRangeNV = NULL;

  // GL_NV_primitive_restart
  PFNGLPRIMITIVERESTARTNVPROC glPrimitiveRestartNV = NULL;
  PFNGLPRIMITIVERESTARTINDEXNVPROC glPrimitiveRestartIndexNV = NULL;

  // GL_ATI_map_object_buffer
  PFNGLMAPOBJECTBUFFERATIPROC glMapObjectBufferATI = NULL;
  PFNGLUNMAPOBJECTBUFFERATIPROC glUnmapObjectBufferATI = NULL;

  // GL_ATI_separate_stencil
  PFNGLSTENCILOPSEPARATEATIPROC glStencilOpSeparateATI = NULL;
  PFNGLSTENCILFUNCSEPARATEATIPROC glStencilFuncSeparateATI = NULL;

  // GL_ATI_vertex_attrib_array_object
  PFNGLVERTEXATTRIBARRAYOBJECTATIPROC glVertexAttribArrayObjectATI = NULL;
  PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC glGetVertexAttribArrayObjectfvATI = NULL;
  PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC glGetVertexAttribArrayObjectivATI = NULL;

  // GL_EXT_depth_bounds_test
  PFNGLDEPTHBOUNDSEXTPROC glDepthBoundsEXT = NULL;

  // GL_EXT_blend_equation_separate
  PFNGLBLENDEQUATIONSEPARATEEXTPROC glBlendEquationSeparateEXT = NULL;

  // GL_EXT_framebuffer_object
  PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT = NULL;
  PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
  PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
  PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
  PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
  PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
  PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
  PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
  PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
  PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
  PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
  PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
  PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
  PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
  PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
  PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

  // GL_GREMEDY_string_marker
  PFNGLSTRINGMARKERGREMEDYPROC glStringMarkerGREMEDY = NULL;

  // GL_EXT_stencil_clear_tag
  PFNGLSTENCILCLEARTAGEXTPROC glStencilClearTagEXT = NULL;

  // GL_EXT_framebuffer_blit
  PFNGLBLITFRAMEBUFFEREXTPROC glBlitFramebufferEXT = NULL;

  // GL_EXT_framebuffer_multisample
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT = NULL;

  // GL_EXT_timer_query
  PFNGLGETQUERYOBJECTI64VEXTPROC glGetQueryObjecti64vEXT = NULL;
  PFNGLGETQUERYOBJECTUI64VEXTPROC glGetQueryObjectui64vEXT = NULL;

  // GL_EXT_gpu_program_parameters
  PFNGLPROGRAMENVPARAMETERS4FVEXTPROC glProgramEnvParameters4fvEXT = NULL;
  PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC glProgramLocalParameters4fvEXT = NULL;

  // GL_APPLE_flush_buffer_range
  PFNGLBUFFERPARAMETERIAPPLEPROC glBufferParameteriAPPLE = NULL;
  PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC glFlushMappedBufferRangeAPPLE = NULL;

  // GL_NV_gpu_program4
  PFNGLPROGRAMLOCALPARAMETERI4INVPROC glProgramLocalParameterI4iNV = NULL;
  PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC glProgramLocalParameterI4ivNV = NULL;
  PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC glProgramLocalParametersI4ivNV = NULL;
  PFNGLPROGRAMLOCALPARAMETERI4UINVPROC glProgramLocalParameterI4uiNV = NULL;
  PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC glProgramLocalParameterI4uivNV = NULL;
  PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC glProgramLocalParametersI4uivNV = NULL;
  PFNGLPROGRAMENVPARAMETERI4INVPROC glProgramEnvParameterI4iNV = NULL;
  PFNGLPROGRAMENVPARAMETERI4IVNVPROC glProgramEnvParameterI4ivNV = NULL;
  PFNGLPROGRAMENVPARAMETERSI4IVNVPROC glProgramEnvParametersI4ivNV = NULL;
  PFNGLPROGRAMENVPARAMETERI4UINVPROC glProgramEnvParameterI4uiNV = NULL;
  PFNGLPROGRAMENVPARAMETERI4UIVNVPROC glProgramEnvParameterI4uivNV = NULL;
  PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC glProgramEnvParametersI4uivNV = NULL;
  PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC glGetProgramLocalParameterIivNV = NULL;
  PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC glGetProgramLocalParameterIuivNV = NULL;
  PFNGLGETPROGRAMENVPARAMETERIIVNVPROC glGetProgramEnvParameterIivNV = NULL;
  PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC glGetProgramEnvParameterIuivNV = NULL;

  // GL_NV_geometry_program4
  PFNGLPROGRAMVERTEXLIMITNVPROC glProgramVertexLimitNV = NULL;
  PFNGLFRAMEBUFFERTEXTUREEXTPROC glFramebufferTextureEXT = NULL;
  PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC glFramebufferTextureLayerEXT = NULL;
  PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC glFramebufferTextureFaceEXT = NULL;

  // GL_EXT_geometry_shader4
  PFNGLPROGRAMPARAMETERIEXTPROC glProgramParameteriEXT = NULL;

  // GL_NV_vertex_program4
  PFNGLVERTEXATTRIBI1IEXTPROC glVertexAttribI1iEXT = NULL;
  PFNGLVERTEXATTRIBI2IEXTPROC glVertexAttribI2iEXT = NULL;
  PFNGLVERTEXATTRIBI3IEXTPROC glVertexAttribI3iEXT = NULL;
  PFNGLVERTEXATTRIBI4IEXTPROC glVertexAttribI4iEXT = NULL;
  PFNGLVERTEXATTRIBI1UIEXTPROC glVertexAttribI1uiEXT = NULL;
  PFNGLVERTEXATTRIBI2UIEXTPROC glVertexAttribI2uiEXT = NULL;
  PFNGLVERTEXATTRIBI3UIEXTPROC glVertexAttribI3uiEXT = NULL;
  PFNGLVERTEXATTRIBI4UIEXTPROC glVertexAttribI4uiEXT = NULL;
  PFNGLVERTEXATTRIBI1IVEXTPROC glVertexAttribI1ivEXT = NULL;
  PFNGLVERTEXATTRIBI2IVEXTPROC glVertexAttribI2ivEXT = NULL;
  PFNGLVERTEXATTRIBI3IVEXTPROC glVertexAttribI3ivEXT = NULL;
  PFNGLVERTEXATTRIBI4IVEXTPROC glVertexAttribI4ivEXT = NULL;
  PFNGLVERTEXATTRIBI1UIVEXTPROC glVertexAttribI1uivEXT = NULL;
  PFNGLVERTEXATTRIBI2UIVEXTPROC glVertexAttribI2uivEXT = NULL;
  PFNGLVERTEXATTRIBI3UIVEXTPROC glVertexAttribI3uivEXT = NULL;
  PFNGLVERTEXATTRIBI4UIVEXTPROC glVertexAttribI4uivEXT = NULL;
  PFNGLVERTEXATTRIBI4BVEXTPROC glVertexAttribI4bvEXT = NULL;
  PFNGLVERTEXATTRIBI4SVEXTPROC glVertexAttribI4svEXT = NULL;
  PFNGLVERTEXATTRIBI4UBVEXTPROC glVertexAttribI4ubvEXT = NULL;
  PFNGLVERTEXATTRIBI4USVEXTPROC glVertexAttribI4usvEXT = NULL;
  PFNGLVERTEXATTRIBIPOINTEREXTPROC glVertexAttribIPointerEXT = NULL;
  PFNGLGETVERTEXATTRIBIIVEXTPROC glGetVertexAttribIivEXT = NULL;
  PFNGLGETVERTEXATTRIBIUIVEXTPROC glGetVertexAttribIuivEXT = NULL;

  // GL_EXT_gpu_shader4
  PFNGLGETUNIFORMUIVEXTPROC glGetUniformuivEXT = NULL;
  PFNGLBINDFRAGDATALOCATIONEXTPROC glBindFragDataLocationEXT = NULL;
  PFNGLGETFRAGDATALOCATIONEXTPROC glGetFragDataLocationEXT = NULL;
  PFNGLUNIFORM1UIEXTPROC glUniform1uiEXT = NULL;
  PFNGLUNIFORM2UIEXTPROC glUniform2uiEXT = NULL;
  PFNGLUNIFORM3UIEXTPROC glUniform3uiEXT = NULL;
  PFNGLUNIFORM4UIEXTPROC glUniform4uiEXT = NULL;
  PFNGLUNIFORM1UIVEXTPROC glUniform1uivEXT = NULL;
  PFNGLUNIFORM2UIVEXTPROC glUniform2uivEXT = NULL;
  PFNGLUNIFORM3UIVEXTPROC glUniform3uivEXT = NULL;
  PFNGLUNIFORM4UIVEXTPROC glUniform4uivEXT = NULL;

  // GL_EXT_draw_instanced
  PFNGLDRAWARRAYSINSTANCEDEXTPROC glDrawArraysInstancedEXT = NULL;
  PFNGLDRAWELEMENTSINSTANCEDEXTPROC glDrawElementsInstancedEXT = NULL;

  // GL_EXT_texture_buffer_object
  PFNGLTEXBUFFEREXTPROC glTexBufferEXT = NULL;

  // GL_NV_depth_buffer_float
  PFNGLDEPTHRANGEDNVPROC glDepthRangedNV = NULL;
  PFNGLCLEARDEPTHDNVPROC glClearDepthdNV = NULL;
  PFNGLDEPTHBOUNDSDNVPROC glDepthBoundsdNV = NULL;

  // GL_NV_framebuffer_multisample_coverage
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC glRenderbufferStorageMultisampleCoverageNV = NULL;

  // GL_NV_parameter_buffer_object
  PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC glProgramBufferParametersfvNV = NULL;
  PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC glProgramBufferParametersIivNV = NULL;
  PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC glProgramBufferParametersIuivNV = NULL;

  // GL_EXT_draw_buffers2
  PFNGLCOLORMASKINDEXEDEXTPROC glColorMaskIndexedEXT = NULL;
  PFNGLGETBOOLEANINDEXEDVEXTPROC glGetBooleanIndexedvEXT = NULL;
  PFNGLGETINTEGERINDEXEDVEXTPROC glGetIntegerIndexedvEXT = NULL;
  PFNGLENABLEINDEXEDEXTPROC glEnableIndexedEXT = NULL;
  PFNGLDISABLEINDEXEDEXTPROC glDisableIndexedEXT = NULL;
  PFNGLISENABLEDINDEXEDEXTPROC glIsEnabledIndexedEXT = NULL;

  // GL_NV_transform_feedback
  PFNGLBEGINTRANSFORMFEEDBACKNVPROC glBeginTransformFeedbackNV = NULL;
  PFNGLENDTRANSFORMFEEDBACKNVPROC glEndTransformFeedbackNV = NULL;
  PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC glTransformFeedbackAttribsNV = NULL;
  PFNGLBINDBUFFERRANGENVPROC glBindBufferRangeNV = NULL;
  PFNGLBINDBUFFEROFFSETNVPROC glBindBufferOffsetNV = NULL;
  PFNGLBINDBUFFERBASENVPROC glBindBufferBaseNV = NULL;
  PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC glTransformFeedbackVaryingsNV = NULL;
  PFNGLACTIVEVARYINGNVPROC glActiveVaryingNV = NULL;
  PFNGLGETVARYINGLOCATIONNVPROC glGetVaryingLocationNV = NULL;
  PFNGLGETACTIVEVARYINGNVPROC glGetActiveVaryingNV = NULL;
  PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC glGetTransformFeedbackVaryingNV = NULL;
  PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC glTransformFeedbackStreamAttribsNV = NULL;

  // GL_EXT_bindable_uniform
  PFNGLUNIFORMBUFFEREXTPROC glUniformBufferEXT = NULL;
  PFNGLGETUNIFORMBUFFERSIZEEXTPROC glGetUniformBufferSizeEXT = NULL;
  PFNGLGETUNIFORMOFFSETEXTPROC glGetUniformOffsetEXT = NULL;

  // GL_EXT_texture_integer
  PFNGLTEXPARAMETERIIVEXTPROC glTexParameterIivEXT = NULL;
  PFNGLTEXPARAMETERIUIVEXTPROC glTexParameterIuivEXT = NULL;
  PFNGLGETTEXPARAMETERIIVEXTPROC glGetTexParameterIivEXT = NULL;
  PFNGLGETTEXPARAMETERIUIVEXTPROC glGetTexParameterIuivEXT = NULL;
  PFNGLCLEARCOLORIIEXTPROC glClearColorIiEXT = NULL;
  PFNGLCLEARCOLORIUIEXTPROC glClearColorIuiEXT = NULL;

  // GL_GREMEDY_frame_terminator
  PFNGLFRAMETERMINATORGREMEDYPROC glFrameTerminatorGREMEDY = NULL;

  // GL_NV_conditional_render
  PFNGLBEGINCONDITIONALRENDERNVPROC glBeginConditionalRenderNV = NULL;
  PFNGLENDCONDITIONALRENDERNVPROC glEndConditionalRenderNV = NULL;

  // GL_NV_present_video
  PFNGLPRESENTFRAMEKEYEDNVPROC glPresentFrameKeyedNV = NULL;
  PFNGLPRESENTFRAMEDUALFILLNVPROC glPresentFrameDualFillNV = NULL;
  PFNGLGETVIDEOIVNVPROC glGetVideoivNV = NULL;
  PFNGLGETVIDEOUIVNVPROC glGetVideouivNV = NULL;
  PFNGLGETVIDEOI64VNVPROC glGetVideoi64vNV = NULL;
  PFNGLGETVIDEOUI64VNVPROC glGetVideoui64vNV = NULL;

  // GL_EXT_transform_feedback
  PFNGLBEGINTRANSFORMFEEDBACKEXTPROC glBeginTransformFeedbackEXT = NULL;
  PFNGLENDTRANSFORMFEEDBACKEXTPROC glEndTransformFeedbackEXT = NULL;
  PFNGLBINDBUFFERRANGEEXTPROC glBindBufferRangeEXT = NULL;
  PFNGLBINDBUFFEROFFSETEXTPROC glBindBufferOffsetEXT = NULL;
  PFNGLBINDBUFFERBASEEXTPROC glBindBufferBaseEXT = NULL;
  PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC glTransformFeedbackVaryingsEXT = NULL;
  PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC glGetTransformFeedbackVaryingEXT = NULL;

  // GL_EXT_direct_state_access
  PFNGLCLIENTATTRIBDEFAULTEXTPROC glClientAttribDefaultEXT = NULL;
  PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC glPushClientAttribDefaultEXT = NULL;
  PFNGLMATRIXLOADFEXTPROC glMatrixLoadfEXT = NULL;
  PFNGLMATRIXLOADDEXTPROC glMatrixLoaddEXT = NULL;
  PFNGLMATRIXMULTFEXTPROC glMatrixMultfEXT = NULL;
  PFNGLMATRIXMULTDEXTPROC glMatrixMultdEXT = NULL;
  PFNGLMATRIXLOADIDENTITYEXTPROC glMatrixLoadIdentityEXT = NULL;
  PFNGLMATRIXROTATEFEXTPROC glMatrixRotatefEXT = NULL;
  PFNGLMATRIXROTATEDEXTPROC glMatrixRotatedEXT = NULL;
  PFNGLMATRIXSCALEFEXTPROC glMatrixScalefEXT = NULL;
  PFNGLMATRIXSCALEDEXTPROC glMatrixScaledEXT = NULL;
  PFNGLMATRIXTRANSLATEFEXTPROC glMatrixTranslatefEXT = NULL;
  PFNGLMATRIXTRANSLATEDEXTPROC glMatrixTranslatedEXT = NULL;
  PFNGLMATRIXFRUSTUMEXTPROC glMatrixFrustumEXT = NULL;
  PFNGLMATRIXORTHOEXTPROC glMatrixOrthoEXT = NULL;
  PFNGLMATRIXPOPEXTPROC glMatrixPopEXT = NULL;
  PFNGLMATRIXPUSHEXTPROC glMatrixPushEXT = NULL;
  PFNGLMATRIXLOADTRANSPOSEFEXTPROC glMatrixLoadTransposefEXT = NULL;
  PFNGLMATRIXLOADTRANSPOSEDEXTPROC glMatrixLoadTransposedEXT = NULL;
  PFNGLMATRIXMULTTRANSPOSEFEXTPROC glMatrixMultTransposefEXT = NULL;
  PFNGLMATRIXMULTTRANSPOSEDEXTPROC glMatrixMultTransposedEXT = NULL;
  PFNGLTEXTUREPARAMETERFEXTPROC glTextureParameterfEXT = NULL;
  PFNGLTEXTUREPARAMETERFVEXTPROC glTextureParameterfvEXT = NULL;
  PFNGLTEXTUREPARAMETERIEXTPROC glTextureParameteriEXT = NULL;
  PFNGLTEXTUREPARAMETERIVEXTPROC glTextureParameterivEXT = NULL;
  PFNGLTEXTUREIMAGE1DEXTPROC glTextureImage1DEXT = NULL;
  PFNGLTEXTUREIMAGE2DEXTPROC glTextureImage2DEXT = NULL;
  PFNGLTEXTURESUBIMAGE1DEXTPROC glTextureSubImage1DEXT = NULL;
  PFNGLTEXTURESUBIMAGE2DEXTPROC glTextureSubImage2DEXT = NULL;
  PFNGLCOPYTEXTUREIMAGE1DEXTPROC glCopyTextureImage1DEXT = NULL;
  PFNGLCOPYTEXTUREIMAGE2DEXTPROC glCopyTextureImage2DEXT = NULL;
  PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC glCopyTextureSubImage1DEXT = NULL;
  PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC glCopyTextureSubImage2DEXT = NULL;
  PFNGLGETTEXTUREIMAGEEXTPROC glGetTextureImageEXT = NULL;
  PFNGLGETTEXTUREPARAMETERFVEXTPROC glGetTextureParameterfvEXT = NULL;
  PFNGLGETTEXTUREPARAMETERIVEXTPROC glGetTextureParameterivEXT = NULL;
  PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC glGetTextureLevelParameterfvEXT = NULL;
  PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC glGetTextureLevelParameterivEXT = NULL;
  PFNGLTEXTUREIMAGE3DEXTPROC glTextureImage3DEXT = NULL;
  PFNGLTEXTURESUBIMAGE3DEXTPROC glTextureSubImage3DEXT = NULL;
  PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC glCopyTextureSubImage3DEXT = NULL;
  PFNGLMULTITEXPARAMETERFEXTPROC glMultiTexParameterfEXT = NULL;
  PFNGLMULTITEXPARAMETERFVEXTPROC glMultiTexParameterfvEXT = NULL;
  PFNGLMULTITEXPARAMETERIEXTPROC glMultiTexParameteriEXT = NULL;
  PFNGLMULTITEXPARAMETERIVEXTPROC glMultiTexParameterivEXT = NULL;
  PFNGLMULTITEXIMAGE1DEXTPROC glMultiTexImage1DEXT = NULL;
  PFNGLMULTITEXIMAGE2DEXTPROC glMultiTexImage2DEXT = NULL;
  PFNGLMULTITEXSUBIMAGE1DEXTPROC glMultiTexSubImage1DEXT = NULL;
  PFNGLMULTITEXSUBIMAGE2DEXTPROC glMultiTexSubImage2DEXT = NULL;
  PFNGLCOPYMULTITEXIMAGE1DEXTPROC glCopyMultiTexImage1DEXT = NULL;
  PFNGLCOPYMULTITEXIMAGE2DEXTPROC glCopyMultiTexImage2DEXT = NULL;
  PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC glCopyMultiTexSubImage1DEXT = NULL;
  PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC glCopyMultiTexSubImage2DEXT = NULL;
  PFNGLGETMULTITEXIMAGEEXTPROC glGetMultiTexImageEXT = NULL;
  PFNGLGETMULTITEXPARAMETERFVEXTPROC glGetMultiTexParameterfvEXT = NULL;
  PFNGLGETMULTITEXPARAMETERIVEXTPROC glGetMultiTexParameterivEXT = NULL;
  PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC glGetMultiTexLevelParameterfvEXT = NULL;
  PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC glGetMultiTexLevelParameterivEXT = NULL;
  PFNGLMULTITEXIMAGE3DEXTPROC glMultiTexImage3DEXT = NULL;
  PFNGLMULTITEXSUBIMAGE3DEXTPROC glMultiTexSubImage3DEXT = NULL;
  PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC glCopyMultiTexSubImage3DEXT = NULL;
  PFNGLBINDMULTITEXTUREEXTPROC glBindMultiTextureEXT = NULL;
  PFNGLENABLECLIENTSTATEINDEXEDEXTPROC glEnableClientStateIndexedEXT = NULL;
  PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC glDisableClientStateIndexedEXT = NULL;
  PFNGLMULTITEXCOORDPOINTEREXTPROC glMultiTexCoordPointerEXT = NULL;
  PFNGLMULTITEXENVFEXTPROC glMultiTexEnvfEXT = NULL;
  PFNGLMULTITEXENVFVEXTPROC glMultiTexEnvfvEXT = NULL;
  PFNGLMULTITEXENVIEXTPROC glMultiTexEnviEXT = NULL;
  PFNGLMULTITEXENVIVEXTPROC glMultiTexEnvivEXT = NULL;
  PFNGLMULTITEXGENDEXTPROC glMultiTexGendEXT = NULL;
  PFNGLMULTITEXGENDVEXTPROC glMultiTexGendvEXT = NULL;
  PFNGLMULTITEXGENFEXTPROC glMultiTexGenfEXT = NULL;
  PFNGLMULTITEXGENFVEXTPROC glMultiTexGenfvEXT = NULL;
  PFNGLMULTITEXGENIEXTPROC glMultiTexGeniEXT = NULL;
  PFNGLMULTITEXGENIVEXTPROC glMultiTexGenivEXT = NULL;
  PFNGLGETMULTITEXENVFVEXTPROC glGetMultiTexEnvfvEXT = NULL;
  PFNGLGETMULTITEXENVIVEXTPROC glGetMultiTexEnvivEXT = NULL;
  PFNGLGETMULTITEXGENDVEXTPROC glGetMultiTexGendvEXT = NULL;
  PFNGLGETMULTITEXGENFVEXTPROC glGetMultiTexGenfvEXT = NULL;
  PFNGLGETMULTITEXGENIVEXTPROC glGetMultiTexGenivEXT = NULL;
  PFNGLGETFLOATINDEXEDVEXTPROC glGetFloatIndexedvEXT = NULL;
  PFNGLGETDOUBLEINDEXEDVEXTPROC glGetDoubleIndexedvEXT = NULL;
  PFNGLGETPOINTERINDEXEDVEXTPROC glGetPointerIndexedvEXT = NULL;
  PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC glCompressedTextureImage3DEXT = NULL;
  PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC glCompressedTextureImage2DEXT = NULL;
  PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC glCompressedTextureImage1DEXT = NULL;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC glCompressedTextureSubImage3DEXT = NULL;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC glCompressedTextureSubImage2DEXT = NULL;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC glCompressedTextureSubImage1DEXT = NULL;
  PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC glGetCompressedTextureImageEXT = NULL;
  PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC glCompressedMultiTexImage3DEXT = NULL;
  PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC glCompressedMultiTexImage2DEXT = NULL;
  PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC glCompressedMultiTexImage1DEXT = NULL;
  PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC glCompressedMultiTexSubImage3DEXT = NULL;
  PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC glCompressedMultiTexSubImage2DEXT = NULL;
  PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC glCompressedMultiTexSubImage1DEXT = NULL;
  PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC glGetCompressedMultiTexImageEXT = NULL;
  PFNGLNAMEDPROGRAMSTRINGEXTPROC glNamedProgramStringEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC glNamedProgramLocalParameter4dEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC glNamedProgramLocalParameter4dvEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC glNamedProgramLocalParameter4fEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC glNamedProgramLocalParameter4fvEXT = NULL;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC glGetNamedProgramLocalParameterdvEXT = NULL;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC glGetNamedProgramLocalParameterfvEXT = NULL;
  PFNGLGETNAMEDPROGRAMIVEXTPROC glGetNamedProgramivEXT = NULL;
  PFNGLGETNAMEDPROGRAMSTRINGEXTPROC glGetNamedProgramStringEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC glNamedProgramLocalParameters4fvEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC glNamedProgramLocalParameterI4iEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC glNamedProgramLocalParameterI4ivEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC glNamedProgramLocalParametersI4ivEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC glNamedProgramLocalParameterI4uiEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC glNamedProgramLocalParameterI4uivEXT = NULL;
  PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC glNamedProgramLocalParametersI4uivEXT = NULL;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC glGetNamedProgramLocalParameterIivEXT = NULL;
  PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC glGetNamedProgramLocalParameterIuivEXT = NULL;
  PFNGLTEXTUREPARAMETERIIVEXTPROC glTextureParameterIivEXT = NULL;
  PFNGLTEXTUREPARAMETERIUIVEXTPROC glTextureParameterIuivEXT = NULL;
  PFNGLGETTEXTUREPARAMETERIIVEXTPROC glGetTextureParameterIivEXT = NULL;
  PFNGLGETTEXTUREPARAMETERIUIVEXTPROC glGetTextureParameterIuivEXT = NULL;
  PFNGLMULTITEXPARAMETERIIVEXTPROC glMultiTexParameterIivEXT = NULL;
  PFNGLMULTITEXPARAMETERIUIVEXTPROC glMultiTexParameterIuivEXT = NULL;
  PFNGLGETMULTITEXPARAMETERIIVEXTPROC glGetMultiTexParameterIivEXT = NULL;
  PFNGLGETMULTITEXPARAMETERIUIVEXTPROC glGetMultiTexParameterIuivEXT = NULL;
  PFNGLPROGRAMUNIFORM1FEXTPROC glProgramUniform1fEXT = NULL;
  PFNGLPROGRAMUNIFORM2FEXTPROC glProgramUniform2fEXT = NULL;
  PFNGLPROGRAMUNIFORM3FEXTPROC glProgramUniform3fEXT = NULL;
  PFNGLPROGRAMUNIFORM4FEXTPROC glProgramUniform4fEXT = NULL;
  PFNGLPROGRAMUNIFORM1IEXTPROC glProgramUniform1iEXT = NULL;
  PFNGLPROGRAMUNIFORM2IEXTPROC glProgramUniform2iEXT = NULL;
  PFNGLPROGRAMUNIFORM3IEXTPROC glProgramUniform3iEXT = NULL;
  PFNGLPROGRAMUNIFORM4IEXTPROC glProgramUniform4iEXT = NULL;
  PFNGLPROGRAMUNIFORM1FVEXTPROC glProgramUniform1fvEXT = NULL;
  PFNGLPROGRAMUNIFORM2FVEXTPROC glProgramUniform2fvEXT = NULL;
  PFNGLPROGRAMUNIFORM3FVEXTPROC glProgramUniform3fvEXT = NULL;
  PFNGLPROGRAMUNIFORM4FVEXTPROC glProgramUniform4fvEXT = NULL;
  PFNGLPROGRAMUNIFORM1IVEXTPROC glProgramUniform1ivEXT = NULL;
  PFNGLPROGRAMUNIFORM2IVEXTPROC glProgramUniform2ivEXT = NULL;
  PFNGLPROGRAMUNIFORM3IVEXTPROC glProgramUniform3ivEXT = NULL;
  PFNGLPROGRAMUNIFORM4IVEXTPROC glProgramUniform4ivEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC glProgramUniformMatrix2fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC glProgramUniformMatrix3fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC glProgramUniformMatrix4fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC glProgramUniformMatrix2x3fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC glProgramUniformMatrix3x2fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC glProgramUniformMatrix2x4fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC glProgramUniformMatrix4x2fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC glProgramUniformMatrix3x4fvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC glProgramUniformMatrix4x3fvEXT = NULL;
  PFNGLPROGRAMUNIFORM1UIEXTPROC glProgramUniform1uiEXT = NULL;
  PFNGLPROGRAMUNIFORM2UIEXTPROC glProgramUniform2uiEXT = NULL;
  PFNGLPROGRAMUNIFORM3UIEXTPROC glProgramUniform3uiEXT = NULL;
  PFNGLPROGRAMUNIFORM4UIEXTPROC glProgramUniform4uiEXT = NULL;
  PFNGLPROGRAMUNIFORM1UIVEXTPROC glProgramUniform1uivEXT = NULL;
  PFNGLPROGRAMUNIFORM2UIVEXTPROC glProgramUniform2uivEXT = NULL;
  PFNGLPROGRAMUNIFORM3UIVEXTPROC glProgramUniform3uivEXT = NULL;
  PFNGLPROGRAMUNIFORM4UIVEXTPROC glProgramUniform4uivEXT = NULL;
  PFNGLNAMEDBUFFERDATAEXTPROC glNamedBufferDataEXT = NULL;
  PFNGLNAMEDBUFFERSUBDATAEXTPROC glNamedBufferSubDataEXT = NULL;
  PFNGLMAPNAMEDBUFFEREXTPROC glMapNamedBufferEXT = NULL;
  PFNGLUNMAPNAMEDBUFFEREXTPROC glUnmapNamedBufferEXT = NULL;
  PFNGLMAPNAMEDBUFFERRANGEEXTPROC glMapNamedBufferRangeEXT = NULL;
  PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC glFlushMappedNamedBufferRangeEXT = NULL;
  PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC glNamedCopyBufferSubDataEXT = NULL;
  PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC glGetNamedBufferParameterivEXT = NULL;
  PFNGLGETNAMEDBUFFERPOINTERVEXTPROC glGetNamedBufferPointervEXT = NULL;
  PFNGLGETNAMEDBUFFERSUBDATAEXTPROC glGetNamedBufferSubDataEXT = NULL;
  PFNGLTEXTUREBUFFEREXTPROC glTextureBufferEXT = NULL;
  PFNGLMULTITEXBUFFEREXTPROC glMultiTexBufferEXT = NULL;
  PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC glNamedRenderbufferStorageEXT = NULL;
  PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC glGetNamedRenderbufferParameterivEXT = NULL;
  PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC glCheckNamedFramebufferStatusEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC glNamedFramebufferTexture1DEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC glNamedFramebufferTexture2DEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC glNamedFramebufferTexture3DEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC glNamedFramebufferRenderbufferEXT = NULL;
  PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetNamedFramebufferAttachmentParameterivEXT = NULL;
  PFNGLGENERATETEXTUREMIPMAPEXTPROC glGenerateTextureMipmapEXT = NULL;
  PFNGLGENERATEMULTITEXMIPMAPEXTPROC glGenerateMultiTexMipmapEXT = NULL;
  PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC glFramebufferDrawBufferEXT = NULL;
  PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC glFramebufferDrawBuffersEXT = NULL;
  PFNGLFRAMEBUFFERREADBUFFEREXTPROC glFramebufferReadBufferEXT = NULL;
  PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC glGetFramebufferParameterivEXT = NULL;
  PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glNamedRenderbufferStorageMultisampleEXT = NULL;
  PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC glNamedRenderbufferStorageMultisampleCoverageEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC glNamedFramebufferTextureEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC glNamedFramebufferTextureLayerEXT = NULL;
  PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC glNamedFramebufferTextureFaceEXT = NULL;
  PFNGLTEXTURERENDERBUFFEREXTPROC glTextureRenderbufferEXT = NULL;
  PFNGLMULTITEXRENDERBUFFEREXTPROC glMultiTexRenderbufferEXT = NULL;
  PFNGLPROGRAMUNIFORM1DEXTPROC glProgramUniform1dEXT = NULL;
  PFNGLPROGRAMUNIFORM2DEXTPROC glProgramUniform2dEXT = NULL;
  PFNGLPROGRAMUNIFORM3DEXTPROC glProgramUniform3dEXT = NULL;
  PFNGLPROGRAMUNIFORM4DEXTPROC glProgramUniform4dEXT = NULL;
  PFNGLPROGRAMUNIFORM1DVEXTPROC glProgramUniform1dvEXT = NULL;
  PFNGLPROGRAMUNIFORM2DVEXTPROC glProgramUniform2dvEXT = NULL;
  PFNGLPROGRAMUNIFORM3DVEXTPROC glProgramUniform3dvEXT = NULL;
  PFNGLPROGRAMUNIFORM4DVEXTPROC glProgramUniform4dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC glProgramUniformMatrix2dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC glProgramUniformMatrix3dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC glProgramUniformMatrix4dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC glProgramUniformMatrix2x3dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC glProgramUniformMatrix2x4dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC glProgramUniformMatrix3x2dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC glProgramUniformMatrix3x4dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC glProgramUniformMatrix4x2dvEXT = NULL;
  PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC glProgramUniformMatrix4x3dvEXT = NULL;

  // GL_NV_explicit_multisample
  PFNGLGETMULTISAMPLEFVNVPROC glGetMultisamplefvNV = NULL;
  PFNGLSAMPLEMASKINDEXEDNVPROC glSampleMaskIndexedNV = NULL;
  PFNGLTEXRENDERBUFFERNVPROC glTexRenderbufferNV = NULL;

  // GL_NV_transform_feedback2
  PFNGLBINDTRANSFORMFEEDBACKNVPROC glBindTransformFeedbackNV = NULL;
  PFNGLDELETETRANSFORMFEEDBACKSNVPROC glDeleteTransformFeedbacksNV = NULL;
  PFNGLGENTRANSFORMFEEDBACKSNVPROC glGenTransformFeedbacksNV = NULL;
  PFNGLISTRANSFORMFEEDBACKNVPROC glIsTransformFeedbackNV = NULL;
  PFNGLPAUSETRANSFORMFEEDBACKNVPROC glPauseTransformFeedbackNV = NULL;
  PFNGLRESUMETRANSFORMFEEDBACKNVPROC glResumeTransformFeedbackNV = NULL;
  PFNGLDRAWTRANSFORMFEEDBACKNVPROC glDrawTransformFeedbackNV = NULL;

  // GL_AMD_performance_monitor
  PFNGLGETPERFMONITORGROUPSAMDPROC glGetPerfMonitorGroupsAMD = NULL;
  PFNGLGETPERFMONITORCOUNTERSAMDPROC glGetPerfMonitorCountersAMD = NULL;
  PFNGLGETPERFMONITORGROUPSTRINGAMDPROC glGetPerfMonitorGroupStringAMD = NULL;
  PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC glGetPerfMonitorCounterStringAMD = NULL;
  PFNGLGETPERFMONITORCOUNTERINFOAMDPROC glGetPerfMonitorCounterInfoAMD = NULL;
  PFNGLGENPERFMONITORSAMDPROC glGenPerfMonitorsAMD = NULL;
  PFNGLDELETEPERFMONITORSAMDPROC glDeletePerfMonitorsAMD = NULL;
  PFNGLSELECTPERFMONITORCOUNTERSAMDPROC glSelectPerfMonitorCountersAMD = NULL;
  PFNGLBEGINPERFMONITORAMDPROC glBeginPerfMonitorAMD = NULL;
  PFNGLENDPERFMONITORAMDPROC glEndPerfMonitorAMD = NULL;
  PFNGLGETPERFMONITORCOUNTERDATAAMDPROC glGetPerfMonitorCounterDataAMD = NULL;

  // GL_AMD_vertex_shader_tesselator
  PFNGLTESSELLATIONFACTORAMDPROC glTessellationFactorAMD = NULL;
  PFNGLTESSELLATIONMODEAMDPROC glTessellationModeAMD = NULL;

  // GL_EXT_provoking_vertex
  PFNGLPROVOKINGVERTEXEXTPROC glProvokingVertexEXT = NULL;

  // GL_AMD_draw_buffers_blend
  PFNGLBLENDFUNCINDEXEDAMDPROC glBlendFuncIndexedAMD = NULL;
  PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC glBlendFuncSeparateIndexedAMD = NULL;
  PFNGLBLENDEQUATIONINDEXEDAMDPROC glBlendEquationIndexedAMD = NULL;
  PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC glBlendEquationSeparateIndexedAMD = NULL;

  // GL_APPLE_texture_range
  PFNGLTEXTURERANGEAPPLEPROC glTextureRangeAPPLE = NULL;
  PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC glGetTexParameterPointervAPPLE = NULL;

  // GL_APPLE_vertex_program_evaluators
  PFNGLENABLEVERTEXATTRIBAPPLEPROC glEnableVertexAttribAPPLE = NULL;
  PFNGLDISABLEVERTEXATTRIBAPPLEPROC glDisableVertexAttribAPPLE = NULL;
  PFNGLISVERTEXATTRIBENABLEDAPPLEPROC glIsVertexAttribEnabledAPPLE = NULL;
  PFNGLMAPVERTEXATTRIB1DAPPLEPROC glMapVertexAttrib1dAPPLE = NULL;
  PFNGLMAPVERTEXATTRIB1FAPPLEPROC glMapVertexAttrib1fAPPLE = NULL;
  PFNGLMAPVERTEXATTRIB2DAPPLEPROC glMapVertexAttrib2dAPPLE = NULL;
  PFNGLMAPVERTEXATTRIB2FAPPLEPROC glMapVertexAttrib2fAPPLE = NULL;

  // GL_APPLE_object_purgeable
  PFNGLOBJECTPURGEABLEAPPLEPROC glObjectPurgeableAPPLE = NULL;
  PFNGLOBJECTUNPURGEABLEAPPLEPROC glObjectUnpurgeableAPPLE = NULL;
  PFNGLGETOBJECTPARAMETERIVAPPLEPROC glGetObjectParameterivAPPLE = NULL;

  // GL_NV_video_capture
  PFNGLBEGINVIDEOCAPTURENVPROC glBeginVideoCaptureNV = NULL;
  PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC glBindVideoCaptureStreamBufferNV = NULL;
  PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC glBindVideoCaptureStreamTextureNV = NULL;
  PFNGLENDVIDEOCAPTURENVPROC glEndVideoCaptureNV = NULL;
  PFNGLGETVIDEOCAPTUREIVNVPROC glGetVideoCaptureivNV = NULL;
  PFNGLGETVIDEOCAPTURESTREAMIVNVPROC glGetVideoCaptureStreamivNV = NULL;
  PFNGLGETVIDEOCAPTURESTREAMFVNVPROC glGetVideoCaptureStreamfvNV = NULL;
  PFNGLGETVIDEOCAPTURESTREAMDVNVPROC glGetVideoCaptureStreamdvNV = NULL;
  PFNGLVIDEOCAPTURENVPROC glVideoCaptureNV = NULL;
  PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC glVideoCaptureStreamParameterivNV = NULL;
  PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC glVideoCaptureStreamParameterfvNV = NULL;
  PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC glVideoCaptureStreamParameterdvNV = NULL;

  // GL_NV_copy_image
  PFNGLCOPYIMAGESUBDATANVPROC glCopyImageSubDataNV = NULL;

  // GL_EXT_separate_shader_objects
  PFNGLUSESHADERPROGRAMEXTPROC glUseShaderProgramEXT = NULL;
  PFNGLACTIVEPROGRAMEXTPROC glActiveProgramEXT = NULL;
  PFNGLCREATESHADERPROGRAMEXTPROC glCreateShaderProgramEXT = NULL;

  // GL_NV_shader_buffer_load
  PFNGLMAKEBUFFERRESIDENTNVPROC glMakeBufferResidentNV = NULL;
  PFNGLMAKEBUFFERNONRESIDENTNVPROC glMakeBufferNonResidentNV = NULL;
  PFNGLISBUFFERRESIDENTNVPROC glIsBufferResidentNV = NULL;
  PFNGLMAKENAMEDBUFFERRESIDENTNVPROC glMakeNamedBufferResidentNV = NULL;
  PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC glMakeNamedBufferNonResidentNV = NULL;
  PFNGLISNAMEDBUFFERRESIDENTNVPROC glIsNamedBufferResidentNV = NULL;
  PFNGLGETBUFFERPARAMETERUI64VNVPROC glGetBufferParameterui64vNV = NULL;
  PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC glGetNamedBufferParameterui64vNV = NULL;
  PFNGLGETINTEGERUI64VNVPROC glGetIntegerui64vNV = NULL;
  PFNGLUNIFORMUI64NVPROC glUniformui64NV = NULL;
  PFNGLUNIFORMUI64VNVPROC glUniformui64vNV = NULL;
  PFNGLGETUNIFORMUI64VNVPROC glGetUniformui64vNV = NULL;
  PFNGLPROGRAMUNIFORMUI64NVPROC glProgramUniformui64NV = NULL;
  PFNGLPROGRAMUNIFORMUI64VNVPROC glProgramUniformui64vNV = NULL;

  // GL_NV_vertex_buffer_unified_memory
  PFNGLBUFFERADDRESSRANGENVPROC glBufferAddressRangeNV = NULL;
  PFNGLVERTEXFORMATNVPROC glVertexFormatNV = NULL;
  PFNGLNORMALFORMATNVPROC glNormalFormatNV = NULL;
  PFNGLCOLORFORMATNVPROC glColorFormatNV = NULL;
  PFNGLINDEXFORMATNVPROC glIndexFormatNV = NULL;
  PFNGLTEXCOORDFORMATNVPROC glTexCoordFormatNV = NULL;
  PFNGLEDGEFLAGFORMATNVPROC glEdgeFlagFormatNV = NULL;
  PFNGLSECONDARYCOLORFORMATNVPROC glSecondaryColorFormatNV = NULL;
  PFNGLFOGCOORDFORMATNVPROC glFogCoordFormatNV = NULL;
  PFNGLVERTEXATTRIBFORMATNVPROC glVertexAttribFormatNV = NULL;
  PFNGLVERTEXATTRIBIFORMATNVPROC glVertexAttribIFormatNV = NULL;
  PFNGLGETINTEGERUI64I_VNVPROC glGetIntegerui64i_vNV = NULL;

  // GL_NV_texture_barrier
  PFNGLTEXTUREBARRIERNVPROC glTextureBarrierNV = NULL;

  // GL_EXT_shader_image_load_store
  PFNGLBINDIMAGETEXTUREEXTPROC glBindImageTextureEXT = NULL;
  PFNGLMEMORYBARRIEREXTPROC glMemoryBarrierEXT = NULL;

  // GL_EXT_vertex_attrib_64bit
  PFNGLVERTEXATTRIBL1DEXTPROC glVertexAttribL1dEXT = NULL;
  PFNGLVERTEXATTRIBL2DEXTPROC glVertexAttribL2dEXT = NULL;
  PFNGLVERTEXATTRIBL3DEXTPROC glVertexAttribL3dEXT = NULL;
  PFNGLVERTEXATTRIBL4DEXTPROC glVertexAttribL4dEXT = NULL;
  PFNGLVERTEXATTRIBL1DVEXTPROC glVertexAttribL1dvEXT = NULL;
  PFNGLVERTEXATTRIBL2DVEXTPROC glVertexAttribL2dvEXT = NULL;
  PFNGLVERTEXATTRIBL3DVEXTPROC glVertexAttribL3dvEXT = NULL;
  PFNGLVERTEXATTRIBL4DVEXTPROC glVertexAttribL4dvEXT = NULL;
  PFNGLVERTEXATTRIBLPOINTEREXTPROC glVertexAttribLPointerEXT = NULL;
  PFNGLGETVERTEXATTRIBLDVEXTPROC glGetVertexAttribLdvEXT = NULL;
  PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glVertexArrayVertexAttribLOffsetEXT = NULL;

  // GL_NV_gpu_program5
  PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC glProgramSubroutineParametersuivNV = NULL;
  PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC glGetProgramSubroutineParameteruivNV = NULL;

  // GL_NV_gpu_shader5
  PFNGLUNIFORM1I64NVPROC glUniform1i64NV = NULL;
  PFNGLUNIFORM2I64NVPROC glUniform2i64NV = NULL;
  PFNGLUNIFORM3I64NVPROC glUniform3i64NV = NULL;
  PFNGLUNIFORM4I64NVPROC glUniform4i64NV = NULL;
  PFNGLUNIFORM1I64VNVPROC glUniform1i64vNV = NULL;
  PFNGLUNIFORM2I64VNVPROC glUniform2i64vNV = NULL;
  PFNGLUNIFORM3I64VNVPROC glUniform3i64vNV = NULL;
  PFNGLUNIFORM4I64VNVPROC glUniform4i64vNV = NULL;
  PFNGLUNIFORM1UI64NVPROC glUniform1ui64NV = NULL;
  PFNGLUNIFORM2UI64NVPROC glUniform2ui64NV = NULL;
  PFNGLUNIFORM3UI64NVPROC glUniform3ui64NV = NULL;
  PFNGLUNIFORM4UI64NVPROC glUniform4ui64NV = NULL;
  PFNGLUNIFORM1UI64VNVPROC glUniform1ui64vNV = NULL;
  PFNGLUNIFORM2UI64VNVPROC glUniform2ui64vNV = NULL;
  PFNGLUNIFORM3UI64VNVPROC glUniform3ui64vNV = NULL;
  PFNGLUNIFORM4UI64VNVPROC glUniform4ui64vNV = NULL;
  PFNGLGETUNIFORMI64VNVPROC glGetUniformi64vNV = NULL;
  PFNGLPROGRAMUNIFORM1I64NVPROC glProgramUniform1i64NV = NULL;
  PFNGLPROGRAMUNIFORM2I64NVPROC glProgramUniform2i64NV = NULL;
  PFNGLPROGRAMUNIFORM3I64NVPROC glProgramUniform3i64NV = NULL;
  PFNGLPROGRAMUNIFORM4I64NVPROC glProgramUniform4i64NV = NULL;
  PFNGLPROGRAMUNIFORM1I64VNVPROC glProgramUniform1i64vNV = NULL;
  PFNGLPROGRAMUNIFORM2I64VNVPROC glProgramUniform2i64vNV = NULL;
  PFNGLPROGRAMUNIFORM3I64VNVPROC glProgramUniform3i64vNV = NULL;
  PFNGLPROGRAMUNIFORM4I64VNVPROC glProgramUniform4i64vNV = NULL;
  PFNGLPROGRAMUNIFORM1UI64NVPROC glProgramUniform1ui64NV = NULL;
  PFNGLPROGRAMUNIFORM2UI64NVPROC glProgramUniform2ui64NV = NULL;
  PFNGLPROGRAMUNIFORM3UI64NVPROC glProgramUniform3ui64NV = NULL;
  PFNGLPROGRAMUNIFORM4UI64NVPROC glProgramUniform4ui64NV = NULL;
  PFNGLPROGRAMUNIFORM1UI64VNVPROC glProgramUniform1ui64vNV = NULL;
  PFNGLPROGRAMUNIFORM2UI64VNVPROC glProgramUniform2ui64vNV = NULL;
  PFNGLPROGRAMUNIFORM3UI64VNVPROC glProgramUniform3ui64vNV = NULL;
  PFNGLPROGRAMUNIFORM4UI64VNVPROC glProgramUniform4ui64vNV = NULL;

  // GL_NV_vertex_attrib_integer_64bit
  PFNGLVERTEXATTRIBL1I64NVPROC glVertexAttribL1i64NV = NULL;
  PFNGLVERTEXATTRIBL2I64NVPROC glVertexAttribL2i64NV = NULL;
  PFNGLVERTEXATTRIBL3I64NVPROC glVertexAttribL3i64NV = NULL;
  PFNGLVERTEXATTRIBL4I64NVPROC glVertexAttribL4i64NV = NULL;
  PFNGLVERTEXATTRIBL1I64VNVPROC glVertexAttribL1i64vNV = NULL;
  PFNGLVERTEXATTRIBL2I64VNVPROC glVertexAttribL2i64vNV = NULL;
  PFNGLVERTEXATTRIBL3I64VNVPROC glVertexAttribL3i64vNV = NULL;
  PFNGLVERTEXATTRIBL4I64VNVPROC glVertexAttribL4i64vNV = NULL;
  PFNGLVERTEXATTRIBL1UI64NVPROC glVertexAttribL1ui64NV = NULL;
  PFNGLVERTEXATTRIBL2UI64NVPROC glVertexAttribL2ui64NV = NULL;
  PFNGLVERTEXATTRIBL3UI64NVPROC glVertexAttribL3ui64NV = NULL;
  PFNGLVERTEXATTRIBL4UI64NVPROC glVertexAttribL4ui64NV = NULL;
  PFNGLVERTEXATTRIBL1UI64VNVPROC glVertexAttribL1ui64vNV = NULL;
  PFNGLVERTEXATTRIBL2UI64VNVPROC glVertexAttribL2ui64vNV = NULL;
  PFNGLVERTEXATTRIBL3UI64VNVPROC glVertexAttribL3ui64vNV = NULL;
  PFNGLVERTEXATTRIBL4UI64VNVPROC glVertexAttribL4ui64vNV = NULL;
  PFNGLGETVERTEXATTRIBLI64VNVPROC glGetVertexAttribLi64vNV = NULL;
  PFNGLGETVERTEXATTRIBLUI64VNVPROC glGetVertexAttribLui64vNV = NULL;
  PFNGLVERTEXATTRIBLFORMATNVPROC glVertexAttribLFormatNV = NULL;

  // GL_AMD_name_gen_delete
  PFNGLGENNAMESAMDPROC glGenNamesAMD = NULL;
  PFNGLDELETENAMESAMDPROC glDeleteNamesAMD = NULL;
  PFNGLISNAMEAMDPROC glIsNameAMD = NULL;

  // GL_AMD_debug_output
  PFNGLDEBUGMESSAGEENABLEAMDPROC glDebugMessageEnableAMD = NULL;
  PFNGLDEBUGMESSAGEINSERTAMDPROC glDebugMessageInsertAMD = NULL;
  PFNGLDEBUGMESSAGECALLBACKAMDPROC glDebugMessageCallbackAMD = NULL;
  PFNGLGETDEBUGMESSAGELOGAMDPROC glGetDebugMessageLogAMD = NULL;

  // GL_NV_vdpau_interop
  PFNGLVDPAUINITNVPROC glVDPAUInitNV = NULL;
  PFNGLVDPAUFININVPROC glVDPAUFiniNV = NULL;
  PFNGLVDPAUREGISTERVIDEOSURFACENVPROC glVDPAURegisterVideoSurfaceNV = NULL;
  PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC glVDPAURegisterOutputSurfaceNV = NULL;
  PFNGLVDPAUISSURFACENVPROC glVDPAUIsSurfaceNV = NULL;
  PFNGLVDPAUUNREGISTERSURFACENVPROC glVDPAUUnregisterSurfaceNV = NULL;
  PFNGLVDPAUGETSURFACEIVNVPROC glVDPAUGetSurfaceivNV = NULL;
  PFNGLVDPAUSURFACEACCESSNVPROC glVDPAUSurfaceAccessNV = NULL;
  PFNGLVDPAUMAPSURFACESNVPROC glVDPAUMapSurfacesNV = NULL;
  PFNGLVDPAUUNMAPSURFACESNVPROC glVDPAUUnmapSurfacesNV = NULL;
}
