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

#if defined(__APPLE__)
#include <stdlib.h>
#include <string.h>
#include <AvailabilityMacros.h>

#ifdef MAC_OS_X_VERSION_10_3

#include <dlfcn.h>

void* NSGLGetProcAddress (const GLubyte *name)
{
  static void* image = NULL;
  if (NULL == image) 
  {
    image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
  }
  return image ? dlsym(image, (const char*)name) : NULL;
}
#else

#include <mach-o/dyld.h>

void* NSGLGetProcAddress (const GLubyte *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
   symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : NULL;
}
#endif /* MAC_OS_X_VERSION_10_3 */
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void* dlGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

namespace vl
{
  const void* getProcAddress(const char* func)
  {
    #if defined(_WIN32)
      return wglGetProcAddress((LPCSTR)func);
    #else
      #if defined(__APPLE__)
  	    return NSGLGetProcAddress((GLubyte)func);
      #else
  	    #if defined(__sgi) || defined(__sun)
          return dlGetProcAddress((GLubyte)func);
  	    #else /* __linux */ 
          return (*glXGetProcAddressARB)((GLubyte)func);
  	    #endif
      #endif
    #endif
  }

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
	  initFunctions();

      int cur_major = -1;
      int cur_minor = -1;
      sscanf((const char*)glGetString(GL_VERSION), "%d.%d", &cur_major, &cur_minor);

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

      #define IS_COMPATIBLE(major, minor) (mIsCompatible && cur_major > major ) || (cur_major == major && cur_minor >= minor)

      vl::Has_GL_VERSION_1_0 = IS_COMPATIBLE(1,0);
      vl::Has_GL_VERSION_1_1 = IS_COMPATIBLE(1,1);
      vl::Has_GL_VERSION_1_2 = IS_COMPATIBLE(1,2);
      vl::Has_GL_VERSION_1_3 = IS_COMPATIBLE(1,3);
      vl::Has_GL_VERSION_1_4 = IS_COMPATIBLE(1,4);
      vl::Has_GL_VERSION_1_5 = IS_COMPATIBLE(1,5);
      vl::Has_GL_VERSION_2_0 = IS_COMPATIBLE(2,0);
      vl::Has_GL_VERSION_2_1 = IS_COMPATIBLE(2,1);
      vl::Has_GL_VERSION_3_0 = IS_COMPATIBLE(3,0);
      vl::Has_GL_VERSION_3_1 = IS_COMPATIBLE(3,1);
      vl::Has_GL_VERSION_3_2 = IS_COMPATIBLE(3,2);
      vl::Has_GL_VERSION_3_3 = IS_COMPATIBLE(3,3);
      vl::Has_GL_VERSION_4_0 = IS_COMPATIBLE(4,0);
      vl::Has_GL_VERSION_4_1 = IS_COMPATIBLE(4,1);

      // fill a string with all the extensions
      if (Has_GL_VERSION_3_0 || Has_GL_VERSION_4_0)
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

      return NULL;
    }

	  void initFunctions()
	  {
		// GL_VERSION_1_2
		glBlendColor = (PFNGLBLENDCOLORPROC)getProcAddress("glBlendColor");
		glBlendEquation = (PFNGLBLENDEQUATIONPROC)getProcAddress("glBlendEquation");
		glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)getProcAddress("glDrawRangeElements");
		glTexImage3D = (PFNGLTEXIMAGE3DPROC)getProcAddress("glTexImage3D");
		glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)getProcAddress("glTexSubImage3D");
		glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)getProcAddress("glCopyTexSubImage3D");

		// GL_VERSION_1_2_DEPRECATED
		glColorTable = (PFNGLCOLORTABLEPROC)getProcAddress("glColorTable");
		glColorTableParameterfv = (PFNGLCOLORTABLEPARAMETERFVPROC)getProcAddress("glColorTableParameterfv");
		glColorTableParameteriv = (PFNGLCOLORTABLEPARAMETERIVPROC)getProcAddress("glColorTableParameteriv");
		glCopyColorTable = (PFNGLCOPYCOLORTABLEPROC)getProcAddress("glCopyColorTable");
		glGetColorTable = (PFNGLGETCOLORTABLEPROC)getProcAddress("glGetColorTable");
		glGetColorTableParameterfv = (PFNGLGETCOLORTABLEPARAMETERFVPROC)getProcAddress("glGetColorTableParameterfv");
		glGetColorTableParameteriv = (PFNGLGETCOLORTABLEPARAMETERIVPROC)getProcAddress("glGetColorTableParameteriv");
		glColorSubTable = (PFNGLCOLORSUBTABLEPROC)getProcAddress("glColorSubTable");
		glCopyColorSubTable = (PFNGLCOPYCOLORSUBTABLEPROC)getProcAddress("glCopyColorSubTable");
		glConvolutionFilter1D = (PFNGLCONVOLUTIONFILTER1DPROC)getProcAddress("glConvolutionFilter1D");
		glConvolutionFilter2D = (PFNGLCONVOLUTIONFILTER2DPROC)getProcAddress("glConvolutionFilter2D");
		glConvolutionParameterf = (PFNGLCONVOLUTIONPARAMETERFPROC)getProcAddress("glConvolutionParameterf");
		glConvolutionParameterfv = (PFNGLCONVOLUTIONPARAMETERFVPROC)getProcAddress("glConvolutionParameterfv");
		glConvolutionParameteri = (PFNGLCONVOLUTIONPARAMETERIPROC)getProcAddress("glConvolutionParameteri");
		glConvolutionParameteriv = (PFNGLCONVOLUTIONPARAMETERIVPROC)getProcAddress("glConvolutionParameteriv");
		glCopyConvolutionFilter1D = (PFNGLCOPYCONVOLUTIONFILTER1DPROC)getProcAddress("glCopyConvolutionFilter1D");
		glCopyConvolutionFilter2D = (PFNGLCOPYCONVOLUTIONFILTER2DPROC)getProcAddress("glCopyConvolutionFilter2D");
		glGetConvolutionFilter = (PFNGLGETCONVOLUTIONFILTERPROC)getProcAddress("glGetConvolutionFilter");
		glGetConvolutionParameterfv = (PFNGLGETCONVOLUTIONPARAMETERFVPROC)getProcAddress("glGetConvolutionParameterfv");
		glGetConvolutionParameteriv = (PFNGLGETCONVOLUTIONPARAMETERIVPROC)getProcAddress("glGetConvolutionParameteriv");
		glGetSeparableFilter = (PFNGLGETSEPARABLEFILTERPROC)getProcAddress("glGetSeparableFilter");
		glSeparableFilter2D = (PFNGLSEPARABLEFILTER2DPROC)getProcAddress("glSeparableFilter2D");
		glGetHistogram = (PFNGLGETHISTOGRAMPROC)getProcAddress("glGetHistogram");
		glGetHistogramParameterfv = (PFNGLGETHISTOGRAMPARAMETERFVPROC)getProcAddress("glGetHistogramParameterfv");
		glGetHistogramParameteriv = (PFNGLGETHISTOGRAMPARAMETERIVPROC)getProcAddress("glGetHistogramParameteriv");
		glGetMinmax = (PFNGLGETMINMAXPROC)getProcAddress("glGetMinmax");
		glGetMinmaxParameterfv = (PFNGLGETMINMAXPARAMETERFVPROC)getProcAddress("glGetMinmaxParameterfv");
		glGetMinmaxParameteriv = (PFNGLGETMINMAXPARAMETERIVPROC)getProcAddress("glGetMinmaxParameteriv");
		glHistogram = (PFNGLHISTOGRAMPROC)getProcAddress("glHistogram");
		glMinmax = (PFNGLMINMAXPROC)getProcAddress("glMinmax");
		glResetHistogram = (PFNGLRESETHISTOGRAMPROC)getProcAddress("glResetHistogram");
		glResetMinmax = (PFNGLRESETMINMAXPROC)getProcAddress("glResetMinmax");

		// GL_VERSION_1_3
		glActiveTexture = (PFNGLACTIVETEXTUREPROC)getProcAddress("glActiveTexture");
		glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)getProcAddress("glSampleCoverage");
		glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)getProcAddress("glCompressedTexImage3D");
		glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)getProcAddress("glCompressedTexImage2D");
		glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)getProcAddress("glCompressedTexImage1D");
		glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)getProcAddress("glCompressedTexSubImage3D");
		glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)getProcAddress("glCompressedTexSubImage2D");
		glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)getProcAddress("glCompressedTexSubImage1D");
		glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)getProcAddress("glGetCompressedTexImage");

		// GL_VERSION_1_3_DEPRECATED
		glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)getProcAddress("glClientActiveTexture");
		glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC)getProcAddress("glMultiTexCoord1d");
		glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC)getProcAddress("glMultiTexCoord1dv");
		glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC)getProcAddress("glMultiTexCoord1f");
		glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC)getProcAddress("glMultiTexCoord1fv");
		glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC)getProcAddress("glMultiTexCoord1i");
		glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC)getProcAddress("glMultiTexCoord1iv");
		glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC)getProcAddress("glMultiTexCoord1s");
		glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC)getProcAddress("glMultiTexCoord1sv");
		glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC)getProcAddress("glMultiTexCoord2d");
		glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC)getProcAddress("glMultiTexCoord2dv");
		glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)getProcAddress("glMultiTexCoord2f");
		glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)getProcAddress("glMultiTexCoord2fv");
		glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC)getProcAddress("glMultiTexCoord2i");
		glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC)getProcAddress("glMultiTexCoord2iv");
		glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC)getProcAddress("glMultiTexCoord2s");
		glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC)getProcAddress("glMultiTexCoord2sv");
		glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC)getProcAddress("glMultiTexCoord3d");
		glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC)getProcAddress("glMultiTexCoord3dv");
		glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)getProcAddress("glMultiTexCoord3f");
		glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC)getProcAddress("glMultiTexCoord3fv");
		glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC)getProcAddress("glMultiTexCoord3i");
		glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC)getProcAddress("glMultiTexCoord3iv");
		glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC)getProcAddress("glMultiTexCoord3s");
		glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC)getProcAddress("glMultiTexCoord3sv");
		glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC)getProcAddress("glMultiTexCoord4d");
		glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC)getProcAddress("glMultiTexCoord4dv");
		glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)getProcAddress("glMultiTexCoord4f");
		glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC)getProcAddress("glMultiTexCoord4fv");
		glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC)getProcAddress("glMultiTexCoord4i");
		glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC)getProcAddress("glMultiTexCoord4iv");
		glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC)getProcAddress("glMultiTexCoord4s");
		glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC)getProcAddress("glMultiTexCoord4sv");
		glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)getProcAddress("glLoadTransposeMatrixf");
		glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)getProcAddress("glLoadTransposeMatrixd");
		glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)getProcAddress("glMultTransposeMatrixf");
		glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)getProcAddress("glMultTransposeMatrixd");

		// GL_VERSION_1_4
		glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)getProcAddress("glBlendFuncSeparate");
		glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)getProcAddress("glMultiDrawArrays");
		glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)getProcAddress("glMultiDrawElements");
		glPointParameterf = (PFNGLPOINTPARAMETERFPROC)getProcAddress("glPointParameterf");
		glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)getProcAddress("glPointParameterfv");
		glPointParameteri = (PFNGLPOINTPARAMETERIPROC)getProcAddress("glPointParameteri");
		glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)getProcAddress("glPointParameteriv");

		// GL_VERSION_1_4_DEPRECATED
		glFogCoordf = (PFNGLFOGCOORDFPROC)getProcAddress("glFogCoordf");
		glFogCoordfv = (PFNGLFOGCOORDFVPROC)getProcAddress("glFogCoordfv");
		glFogCoordd = (PFNGLFOGCOORDDPROC)getProcAddress("glFogCoordd");
		glFogCoorddv = (PFNGLFOGCOORDDVPROC)getProcAddress("glFogCoorddv");
		glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC)getProcAddress("glFogCoordPointer");
		glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC)getProcAddress("glSecondaryColor3b");
		glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC)getProcAddress("glSecondaryColor3bv");
		glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC)getProcAddress("glSecondaryColor3d");
		glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC)getProcAddress("glSecondaryColor3dv");
		glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC)getProcAddress("glSecondaryColor3f");
		glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC)getProcAddress("glSecondaryColor3fv");
		glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC)getProcAddress("glSecondaryColor3i");
		glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC)getProcAddress("glSecondaryColor3iv");
		glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC)getProcAddress("glSecondaryColor3s");
		glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC)getProcAddress("glSecondaryColor3sv");
		glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC)getProcAddress("glSecondaryColor3ub");
		glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC)getProcAddress("glSecondaryColor3ubv");
		glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC)getProcAddress("glSecondaryColor3ui");
		glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC)getProcAddress("glSecondaryColor3uiv");
		glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC)getProcAddress("glSecondaryColor3us");
		glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC)getProcAddress("glSecondaryColor3usv");
		glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC)getProcAddress("glSecondaryColorPointer");
		glWindowPos2d = (PFNGLWINDOWPOS2DPROC)getProcAddress("glWindowPos2d");
		glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC)getProcAddress("glWindowPos2dv");
		glWindowPos2f = (PFNGLWINDOWPOS2FPROC)getProcAddress("glWindowPos2f");
		glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC)getProcAddress("glWindowPos2fv");
		glWindowPos2i = (PFNGLWINDOWPOS2IPROC)getProcAddress("glWindowPos2i");
		glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC)getProcAddress("glWindowPos2iv");
		glWindowPos2s = (PFNGLWINDOWPOS2SPROC)getProcAddress("glWindowPos2s");
		glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC)getProcAddress("glWindowPos2sv");
		glWindowPos3d = (PFNGLWINDOWPOS3DPROC)getProcAddress("glWindowPos3d");
		glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC)getProcAddress("glWindowPos3dv");
		glWindowPos3f = (PFNGLWINDOWPOS3FPROC)getProcAddress("glWindowPos3f");
		glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC)getProcAddress("glWindowPos3fv");
		glWindowPos3i = (PFNGLWINDOWPOS3IPROC)getProcAddress("glWindowPos3i");
		glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC)getProcAddress("glWindowPos3iv");
		glWindowPos3s = (PFNGLWINDOWPOS3SPROC)getProcAddress("glWindowPos3s");
		glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC)getProcAddress("glWindowPos3sv");

		// GL_VERSION_1_5
		glGenQueries = (PFNGLGENQUERIESPROC)getProcAddress("glGenQueries");
		glDeleteQueries = (PFNGLDELETEQUERIESPROC)getProcAddress("glDeleteQueries");
		glIsQuery = (PFNGLISQUERYPROC)getProcAddress("glIsQuery");
		glBeginQuery = (PFNGLBEGINQUERYPROC)getProcAddress("glBeginQuery");
		glEndQuery = (PFNGLENDQUERYPROC)getProcAddress("glEndQuery");
		glGetQueryiv = (PFNGLGETQUERYIVPROC)getProcAddress("glGetQueryiv");
		glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)getProcAddress("glGetQueryObjectiv");
		glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)getProcAddress("glGetQueryObjectuiv");
		glBindBuffer = (PFNGLBINDBUFFERPROC)getProcAddress("glBindBuffer");
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)getProcAddress("glDeleteBuffers");
		glGenBuffers = (PFNGLGENBUFFERSPROC)getProcAddress("glGenBuffers");
		glIsBuffer = (PFNGLISBUFFERPROC)getProcAddress("glIsBuffer");
		glBufferData = (PFNGLBUFFERDATAPROC)getProcAddress("glBufferData");
		glBufferSubData = (PFNGLBUFFERSUBDATAPROC)getProcAddress("glBufferSubData");
		glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)getProcAddress("glGetBufferSubData");
		glMapBuffer = (PFNGLMAPBUFFERPROC)getProcAddress("glMapBuffer");
		glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)getProcAddress("glUnmapBuffer");
		glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)getProcAddress("glGetBufferParameteriv");
		glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)getProcAddress("glGetBufferPointerv");

		// GL_VERSION_2_0
		glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)getProcAddress("glBlendEquationSeparate");
		glDrawBuffers = (PFNGLDRAWBUFFERSPROC)getProcAddress("glDrawBuffers");
		glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)getProcAddress("glStencilOpSeparate");
		glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)getProcAddress("glStencilFuncSeparate");
		glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)getProcAddress("glStencilMaskSeparate");
		glAttachShader = (PFNGLATTACHSHADERPROC)getProcAddress("glAttachShader");
		glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)getProcAddress("glBindAttribLocation");
		glCompileShader = (PFNGLCOMPILESHADERPROC)getProcAddress("glCompileShader");
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProcAddress("glCreateProgram");
		glCreateShader = (PFNGLCREATESHADERPROC)getProcAddress("glCreateShader");
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)getProcAddress("glDeleteProgram");
		glDeleteShader = (PFNGLDELETESHADERPROC)getProcAddress("glDeleteShader");
		glDetachShader = (PFNGLDETACHSHADERPROC)getProcAddress("glDetachShader");
		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)getProcAddress("glDisableVertexAttribArray");
		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getProcAddress("glEnableVertexAttribArray");
		glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)getProcAddress("glGetActiveAttrib");
		glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)getProcAddress("glGetActiveUniform");
		glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)getProcAddress("glGetAttachedShaders");
		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)getProcAddress("glGetAttribLocation");
		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getProcAddress("glGetProgramiv");
		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProcAddress("glGetProgramInfoLog");
		glGetShaderiv = (PFNGLGETSHADERIVPROC)getProcAddress("glGetShaderiv");
		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProcAddress("glGetShaderInfoLog");
		glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)getProcAddress("glGetShaderSource");
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getProcAddress("glGetUniformLocation");
		glGetUniformfv = (PFNGLGETUNIFORMFVPROC)getProcAddress("glGetUniformfv");
		glGetUniformiv = (PFNGLGETUNIFORMIVPROC)getProcAddress("glGetUniformiv");
		glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)getProcAddress("glGetVertexAttribdv");
		glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)getProcAddress("glGetVertexAttribfv");
		glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)getProcAddress("glGetVertexAttribiv");
		glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)getProcAddress("glGetVertexAttribPointerv");
		glIsProgram = (PFNGLISPROGRAMPROC)getProcAddress("glIsProgram");
		glIsShader = (PFNGLISSHADERPROC)getProcAddress("glIsShader");
		glLinkProgram = (PFNGLLINKPROGRAMPROC)getProcAddress("glLinkProgram");
		glShaderSource = (PFNGLSHADERSOURCEPROC)getProcAddress("glShaderSource");
		glUseProgram = (PFNGLUSEPROGRAMPROC)getProcAddress("glUseProgram");
		glUniform1f = (PFNGLUNIFORM1FPROC)getProcAddress("glUniform1f");
		glUniform2f = (PFNGLUNIFORM2FPROC)getProcAddress("glUniform2f");
		glUniform3f = (PFNGLUNIFORM3FPROC)getProcAddress("glUniform3f");
		glUniform4f = (PFNGLUNIFORM4FPROC)getProcAddress("glUniform4f");
		glUniform1i = (PFNGLUNIFORM1IPROC)getProcAddress("glUniform1i");
		glUniform2i = (PFNGLUNIFORM2IPROC)getProcAddress("glUniform2i");
		glUniform3i = (PFNGLUNIFORM3IPROC)getProcAddress("glUniform3i");
		glUniform4i = (PFNGLUNIFORM4IPROC)getProcAddress("glUniform4i");
		glUniform1fv = (PFNGLUNIFORM1FVPROC)getProcAddress("glUniform1fv");
		glUniform2fv = (PFNGLUNIFORM2FVPROC)getProcAddress("glUniform2fv");
		glUniform3fv = (PFNGLUNIFORM3FVPROC)getProcAddress("glUniform3fv");
		glUniform4fv = (PFNGLUNIFORM4FVPROC)getProcAddress("glUniform4fv");
		glUniform1iv = (PFNGLUNIFORM1IVPROC)getProcAddress("glUniform1iv");
		glUniform2iv = (PFNGLUNIFORM2IVPROC)getProcAddress("glUniform2iv");
		glUniform3iv = (PFNGLUNIFORM3IVPROC)getProcAddress("glUniform3iv");
		glUniform4iv = (PFNGLUNIFORM4IVPROC)getProcAddress("glUniform4iv");
		glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)getProcAddress("glUniformMatrix2fv");
		glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)getProcAddress("glUniformMatrix3fv");
		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)getProcAddress("glUniformMatrix4fv");
		glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)getProcAddress("glValidateProgram");
		glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)getProcAddress("glVertexAttrib1d");
		glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)getProcAddress("glVertexAttrib1dv");
		glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)getProcAddress("glVertexAttrib1f");
		glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)getProcAddress("glVertexAttrib1fv");
		glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)getProcAddress("glVertexAttrib1s");
		glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)getProcAddress("glVertexAttrib1sv");
		glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)getProcAddress("glVertexAttrib2d");
		glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)getProcAddress("glVertexAttrib2dv");
		glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)getProcAddress("glVertexAttrib2f");
		glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)getProcAddress("glVertexAttrib2fv");
		glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)getProcAddress("glVertexAttrib2s");
		glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)getProcAddress("glVertexAttrib2sv");
		glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)getProcAddress("glVertexAttrib3d");
		glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)getProcAddress("glVertexAttrib3dv");
		glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)getProcAddress("glVertexAttrib3f");
		glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)getProcAddress("glVertexAttrib3fv");
		glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)getProcAddress("glVertexAttrib3s");
		glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)getProcAddress("glVertexAttrib3sv");
		glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)getProcAddress("glVertexAttrib4Nbv");
		glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)getProcAddress("glVertexAttrib4Niv");
		glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)getProcAddress("glVertexAttrib4Nsv");
		glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)getProcAddress("glVertexAttrib4Nub");
		glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)getProcAddress("glVertexAttrib4Nubv");
		glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)getProcAddress("glVertexAttrib4Nuiv");
		glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)getProcAddress("glVertexAttrib4Nusv");
		glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)getProcAddress("glVertexAttrib4bv");
		glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)getProcAddress("glVertexAttrib4d");
		glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)getProcAddress("glVertexAttrib4dv");
		glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)getProcAddress("glVertexAttrib4f");
		glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)getProcAddress("glVertexAttrib4fv");
		glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)getProcAddress("glVertexAttrib4iv");
		glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)getProcAddress("glVertexAttrib4s");
		glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)getProcAddress("glVertexAttrib4sv");
		glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)getProcAddress("glVertexAttrib4ubv");
		glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)getProcAddress("glVertexAttrib4uiv");
		glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)getProcAddress("glVertexAttrib4usv");
		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getProcAddress("glVertexAttribPointer");

		// GL_VERSION_2_1
		glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)getProcAddress("glUniformMatrix2x3fv");
		glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)getProcAddress("glUniformMatrix3x2fv");
		glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)getProcAddress("glUniformMatrix2x4fv");
		glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)getProcAddress("glUniformMatrix4x2fv");
		glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)getProcAddress("glUniformMatrix3x4fv");
		glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)getProcAddress("glUniformMatrix4x3fv");

		// GL_VERSION_3_0
		/* OpenGL 3.0 also reuses entry points from these extensions: */
		/* ARB_framebuffer_object */
		/* ARB_map_buffer_range */
		/* ARB_vertex_array_object */
		glColorMaski = (PFNGLCOLORMASKIPROC)getProcAddress("glColorMaski");
		// glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)getProcAddress("glGetBooleanIndexedvEXT");
		// glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)getProcAddress("glGetIntegerIndexedvEXT");
		glEnablei = (PFNGLENABLEIPROC)getProcAddress("glEnablei");
		glDisablei = (PFNGLDISABLEIPROC)getProcAddress("glDisablei");
		glIsEnabledi = (PFNGLISENABLEDIPROC)getProcAddress("glIsEnabledi");
		glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)getProcAddress("glBeginTransformFeedback");
		glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)getProcAddress("glEndTransformFeedback");
		glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)getProcAddress("glBindBufferRange");
		glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)getProcAddress("glBindBufferBase");
		glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)getProcAddress("glTransformFeedbackVaryings");
		glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)getProcAddress("glGetTransformFeedbackVarying");
		glClampColor = (PFNGLCLAMPCOLORPROC)getProcAddress("glClampColor");
		glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)getProcAddress("glBeginConditionalRender");
		glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)getProcAddress("glEndConditionalRender");
		glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)getProcAddress("glVertexAttribIPointer");
		glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)getProcAddress("glGetVertexAttribIiv");
		glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)getProcAddress("glGetVertexAttribIuiv");
		glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)getProcAddress("glVertexAttribI1i");
		glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)getProcAddress("glVertexAttribI2i");
		glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)getProcAddress("glVertexAttribI3i");
		glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)getProcAddress("glVertexAttribI4i");
		glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)getProcAddress("glVertexAttribI1ui");
		glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)getProcAddress("glVertexAttribI2ui");
		glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)getProcAddress("glVertexAttribI3ui");
		glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)getProcAddress("glVertexAttribI4ui");
		glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)getProcAddress("glVertexAttribI1iv");
		glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)getProcAddress("glVertexAttribI2iv");
		glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)getProcAddress("glVertexAttribI3iv");
		glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)getProcAddress("glVertexAttribI4iv");
		glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)getProcAddress("glVertexAttribI1uiv");
		glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)getProcAddress("glVertexAttribI2uiv");
		glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)getProcAddress("glVertexAttribI3uiv");
		glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)getProcAddress("glVertexAttribI4uiv");
		glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)getProcAddress("glVertexAttribI4bv");
		glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)getProcAddress("glVertexAttribI4sv");
		glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)getProcAddress("glVertexAttribI4ubv");
		glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)getProcAddress("glVertexAttribI4usv");
		glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)getProcAddress("glGetUniformuiv");
		glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)getProcAddress("glBindFragDataLocation");
		glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)getProcAddress("glGetFragDataLocation");
		glUniform1ui = (PFNGLUNIFORM1UIPROC)getProcAddress("glUniform1ui");
		glUniform2ui = (PFNGLUNIFORM2UIPROC)getProcAddress("glUniform2ui");
		glUniform3ui = (PFNGLUNIFORM3UIPROC)getProcAddress("glUniform3ui");
		glUniform4ui = (PFNGLUNIFORM4UIPROC)getProcAddress("glUniform4ui");
		glUniform1uiv = (PFNGLUNIFORM1UIVPROC)getProcAddress("glUniform1uiv");
		glUniform2uiv = (PFNGLUNIFORM2UIVPROC)getProcAddress("glUniform2uiv");
		glUniform3uiv = (PFNGLUNIFORM3UIVPROC)getProcAddress("glUniform3uiv");
		glUniform4uiv = (PFNGLUNIFORM4UIVPROC)getProcAddress("glUniform4uiv");
		glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)getProcAddress("glTexParameterIiv");
		glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)getProcAddress("glTexParameterIuiv");
		glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)getProcAddress("glGetTexParameterIiv");
		glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)getProcAddress("glGetTexParameterIuiv");
		glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)getProcAddress("glClearBufferiv");
		glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)getProcAddress("glClearBufferuiv");
		glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)getProcAddress("glClearBufferfv");
		glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)getProcAddress("glClearBufferfi");
		glGetStringi = (PFNGLGETSTRINGIPROC)getProcAddress("glGetStringi");


		// GL_VERSION_3_1
		/* OpenGL 3.1 also reuses entry points from these extensions: */
		/* ARB_copy_buffer */
		/* ARB_uniform_buffer_object */
		glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)getProcAddress("glDrawArraysInstanced");
		glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)getProcAddress("glDrawElementsInstanced");
		glTexBuffer = (PFNGLTEXBUFFERPROC)getProcAddress("glTexBuffer");
		glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)getProcAddress("glPrimitiveRestartIndex");

		// GL_VERSION_3_2
		/* OpenGL 3.2 also reuses entry points from these extensions: */
		/* ARB_draw_elements_base_vertex */
		/* ARB_provoking_vertex */
		/* ARB_sync */
		/* ARB_texture_multisample */
		glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)getProcAddress("PFNGLGETINTEGER64I_VPROC");
		glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)getProcAddress("glGetBufferParameteri64v");
		glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)getProcAddress("glFramebufferTexture");

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
		glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)getProcAddress("glVertexAttribDivisor");

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
		glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)getProcAddress("glMinSampleShading");
		glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)getProcAddress("glBlendEquationi");
		glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)getProcAddress("glBlendEquationSeparatei");
		glBlendFunci = (PFNGLBLENDFUNCIPROC)getProcAddress("glBlendFunci");
		glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)getProcAddress("glBlendFuncSeparatei");

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
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)getProcAddress("glActiveTextureARB");
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)getProcAddress("glClientActiveTextureARB");
		glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)getProcAddress("glMultiTexCoord1dARB");
		glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)getProcAddress("glMultiTexCoord1dvARB");
		glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)getProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)getProcAddress("glMultiTexCoord1fvARB");
		glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)getProcAddress("glMultiTexCoord1iARB");
		glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)getProcAddress("glMultiTexCoord1ivARB");
		glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)getProcAddress("glMultiTexCoord1sARB");
		glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)getProcAddress("glMultiTexCoord1svARB");
		glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)getProcAddress("glMultiTexCoord2dARB");
		glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)getProcAddress("glMultiTexCoord2dvARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)getProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)getProcAddress("glMultiTexCoord2fvARB");
		glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)getProcAddress("glMultiTexCoord2iARB");
		glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)getProcAddress("glMultiTexCoord2ivARB");
		glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)getProcAddress("glMultiTexCoord2sARB");
		glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)getProcAddress("glMultiTexCoord2svARB");
		glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)getProcAddress("glMultiTexCoord3dARB");
		glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)getProcAddress("glMultiTexCoord3dvARB");
		glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)getProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)getProcAddress("glMultiTexCoord3fvARB");
		glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)getProcAddress("glMultiTexCoord3iARB");
		glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)getProcAddress("glMultiTexCoord3ivARB");
		glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)getProcAddress("glMultiTexCoord3sARB");
		glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)getProcAddress("glMultiTexCoord3svARB");
		glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)getProcAddress("glMultiTexCoord4dARB");
		glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)getProcAddress("glMultiTexCoord4dvARB");
		glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)getProcAddress("glMultiTexCoord4fARB");
		glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)getProcAddress("glMultiTexCoord4fvARB");
		glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)getProcAddress("glMultiTexCoord4iARB");
		glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)getProcAddress("glMultiTexCoord4ivARB");
		glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)getProcAddress("glMultiTexCoord4sARB");
		glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)getProcAddress("glMultiTexCoord4svARB");

		// GL_ARB_transpose_matrix
		glLoadTransposeMatrixfARB = (PFNGLLOADTRANSPOSEMATRIXFARBPROC)getProcAddress("glLoadTransposeMatrixfARB");
		glLoadTransposeMatrixdARB = (PFNGLLOADTRANSPOSEMATRIXDARBPROC)getProcAddress("glLoadTransposeMatrixdARB");
		glMultTransposeMatrixfARB = (PFNGLMULTTRANSPOSEMATRIXFARBPROC)getProcAddress("glMultTransposeMatrixfARB");
		glMultTransposeMatrixdARB = (PFNGLMULTTRANSPOSEMATRIXDARBPROC)getProcAddress("glMultTransposeMatrixdARB");

		// GL_ARB_multisample
		glSampleCoverageARB = (PFNGLSAMPLECOVERAGEARBPROC)getProcAddress("glSampleCoverageARB");

		// GL_ARB_texture_compression
		glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)getProcAddress("glCompressedTexImage3DARB");
		glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)getProcAddress("glCompressedTexImage2DARB");
		glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)getProcAddress("glCompressedTexImage1DARB");
		glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)getProcAddress("glCompressedTexSubImage3DARB");
		glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)getProcAddress("glCompressedTexSubImage2DARB");
		glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)getProcAddress("glCompressedTexSubImage1DARB");
		glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)getProcAddress("glGetCompressedTexImageARB");

		// GL_ARB_point_parameters
		glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)getProcAddress("glPointParameterfARB");
		glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)getProcAddress("glPointParameterfvARB");

		// GL_ARB_vertex_blend
		glWeightbvARB = (PFNGLWEIGHTBVARBPROC)getProcAddress("glWeightbvARB");
		glWeightsvARB = (PFNGLWEIGHTSVARBPROC)getProcAddress("glWeightsvARB");
		glWeightivARB = (PFNGLWEIGHTIVARBPROC)getProcAddress("glWeightivARB");
		glWeightfvARB = (PFNGLWEIGHTFVARBPROC)getProcAddress("glWeightfvARB");
		glWeightdvARB = (PFNGLWEIGHTDVARBPROC)getProcAddress("glWeightdvARB");
		glWeightubvARB = (PFNGLWEIGHTUBVARBPROC)getProcAddress("glWeightubvARB");
		glWeightusvARB = (PFNGLWEIGHTUSVARBPROC)getProcAddress("glWeightusvARB");
		glWeightuivARB = (PFNGLWEIGHTUIVARBPROC)getProcAddress("glWeightuivARB");
		glWeightPointerARB = (PFNGLWEIGHTPOINTERARBPROC)getProcAddress("glWeightPointerARB");
		glVertexBlendARB = (PFNGLVERTEXBLENDARBPROC)getProcAddress("glVertexBlendARB");

		// GL_ARB_matrix_palette
		glCurrentPaletteMatrixARB = (PFNGLCURRENTPALETTEMATRIXARBPROC)getProcAddress("glCurrentPaletteMatrixARB");
		glMatrixIndexubvARB = (PFNGLMATRIXINDEXUBVARBPROC)getProcAddress("glMatrixIndexubvARB");
		glMatrixIndexusvARB = (PFNGLMATRIXINDEXUSVARBPROC)getProcAddress("glMatrixIndexusvARB");
		glMatrixIndexuivARB = (PFNGLMATRIXINDEXUIVARBPROC)getProcAddress("glMatrixIndexuivARB");
		glMatrixIndexPointerARB = (PFNGLMATRIXINDEXPOINTERARBPROC)getProcAddress("glMatrixIndexPointerARB");

		// GL_ARB_window_pos
		glWindowPos2dARB = (PFNGLWINDOWPOS2DARBPROC)getProcAddress("glWindowPos2dARB");
		glWindowPos2dvARB = (PFNGLWINDOWPOS2DVARBPROC)getProcAddress("glWindowPos2dvARB");
		glWindowPos2fARB = (PFNGLWINDOWPOS2FARBPROC)getProcAddress("glWindowPos2fARB");
		glWindowPos2fvARB = (PFNGLWINDOWPOS2FVARBPROC)getProcAddress("glWindowPos2fvARB");
		glWindowPos2iARB = (PFNGLWINDOWPOS2IARBPROC)getProcAddress("glWindowPos2iARB");
		glWindowPos2ivARB = (PFNGLWINDOWPOS2IVARBPROC)getProcAddress("glWindowPos2ivARB");
		glWindowPos2sARB = (PFNGLWINDOWPOS2SARBPROC)getProcAddress("glWindowPos2sARB");
		glWindowPos2svARB = (PFNGLWINDOWPOS2SVARBPROC)getProcAddress("glWindowPos2svARB");
		glWindowPos3dARB = (PFNGLWINDOWPOS3DARBPROC)getProcAddress("glWindowPos3dARB");
		glWindowPos3dvARB = (PFNGLWINDOWPOS3DVARBPROC)getProcAddress("glWindowPos3dvARB");
		glWindowPos3fARB = (PFNGLWINDOWPOS3FARBPROC)getProcAddress("glWindowPos3fARB");
		glWindowPos3fvARB = (PFNGLWINDOWPOS3FVARBPROC)getProcAddress("glWindowPos3fvARB");
		glWindowPos3iARB = (PFNGLWINDOWPOS3IARBPROC)getProcAddress("glWindowPos3iARB");
		glWindowPos3ivARB = (PFNGLWINDOWPOS3IVARBPROC)getProcAddress("glWindowPos3ivARB");
		glWindowPos3sARB = (PFNGLWINDOWPOS3SARBPROC)getProcAddress("glWindowPos3sARB");
		glWindowPos3svARB = (PFNGLWINDOWPOS3SVARBPROC)getProcAddress("glWindowPos3svARB");

		// GL_ARB_vertex_program
		glVertexAttrib1dARB = (PFNGLVERTEXATTRIB1DARBPROC)getProcAddress("glVertexAttrib1dARB");
		glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)getProcAddress("glVertexAttrib1dvARB");
		glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)getProcAddress("glVertexAttrib1fARB");
		glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)getProcAddress("glVertexAttrib1fvARB");
		glVertexAttrib1sARB = (PFNGLVERTEXATTRIB1SARBPROC)getProcAddress("glVertexAttrib1sARB");
		glVertexAttrib1svARB = (PFNGLVERTEXATTRIB1SVARBPROC)getProcAddress("glVertexAttrib1svARB");
		glVertexAttrib2dARB = (PFNGLVERTEXATTRIB2DARBPROC)getProcAddress("glVertexAttrib2dARB");
		glVertexAttrib2dvARB = (PFNGLVERTEXATTRIB2DVARBPROC)getProcAddress("glVertexAttrib2dvARB");
		glVertexAttrib2fARB = (PFNGLVERTEXATTRIB2FARBPROC)getProcAddress("glVertexAttrib2fARB");
		glVertexAttrib2fvARB = (PFNGLVERTEXATTRIB2FVARBPROC)getProcAddress("glVertexAttrib2fvARB");
		glVertexAttrib2sARB = (PFNGLVERTEXATTRIB2SARBPROC)getProcAddress("glVertexAttrib2sARB");
		glVertexAttrib2svARB = (PFNGLVERTEXATTRIB2SVARBPROC)getProcAddress("glVertexAttrib2svARB");
		glVertexAttrib3dARB = (PFNGLVERTEXATTRIB3DARBPROC)getProcAddress("glVertexAttrib3dARB");
		glVertexAttrib3dvARB = (PFNGLVERTEXATTRIB3DVARBPROC)getProcAddress("glVertexAttrib3dvARB");
		glVertexAttrib3fARB = (PFNGLVERTEXATTRIB3FARBPROC)getProcAddress("glVertexAttrib3fARB");
		glVertexAttrib3fvARB = (PFNGLVERTEXATTRIB3FVARBPROC)getProcAddress("glVertexAttrib3fvARB");
		glVertexAttrib3sARB = (PFNGLVERTEXATTRIB3SARBPROC)getProcAddress("glVertexAttrib3sARB");
		glVertexAttrib3svARB = (PFNGLVERTEXATTRIB3SVARBPROC)getProcAddress("glVertexAttrib3svARB");
		glVertexAttrib4NbvARB = (PFNGLVERTEXATTRIB4NBVARBPROC)getProcAddress("glVertexAttrib4NbvARB");
		glVertexAttrib4NivARB = (PFNGLVERTEXATTRIB4NIVARBPROC)getProcAddress("glVertexAttrib4NivARB");
		glVertexAttrib4NsvARB = (PFNGLVERTEXATTRIB4NSVARBPROC)getProcAddress("glVertexAttrib4NsvARB");
		glVertexAttrib4NubARB = (PFNGLVERTEXATTRIB4NUBARBPROC)getProcAddress("glVertexAttrib4NubARB");
		glVertexAttrib4NubvARB = (PFNGLVERTEXATTRIB4NUBVARBPROC)getProcAddress("glVertexAttrib4NubvARB");
		glVertexAttrib4NuivARB = (PFNGLVERTEXATTRIB4NUIVARBPROC)getProcAddress("glVertexAttrib4NuivARB");
		glVertexAttrib4NusvARB = (PFNGLVERTEXATTRIB4NUSVARBPROC)getProcAddress("glVertexAttrib4NusvARB");
		glVertexAttrib4bvARB = (PFNGLVERTEXATTRIB4BVARBPROC)getProcAddress("glVertexAttrib4bvARB");
		glVertexAttrib4dARB = (PFNGLVERTEXATTRIB4DARBPROC)getProcAddress("glVertexAttrib4dARB");
		glVertexAttrib4dvARB = (PFNGLVERTEXATTRIB4DVARBPROC)getProcAddress("glVertexAttrib4dvARB");
		glVertexAttrib4fARB = (PFNGLVERTEXATTRIB4FARBPROC)getProcAddress("glVertexAttrib4fARB");
		glVertexAttrib4fvARB = (PFNGLVERTEXATTRIB4FVARBPROC)getProcAddress("glVertexAttrib4fvARB");
		glVertexAttrib4ivARB = (PFNGLVERTEXATTRIB4IVARBPROC)getProcAddress("glVertexAttrib4ivARB");
		glVertexAttrib4sARB = (PFNGLVERTEXATTRIB4SARBPROC)getProcAddress("glVertexAttrib4sARB");
		glVertexAttrib4svARB = (PFNGLVERTEXATTRIB4SVARBPROC)getProcAddress("glVertexAttrib4svARB");
		glVertexAttrib4ubvARB = (PFNGLVERTEXATTRIB4UBVARBPROC)getProcAddress("glVertexAttrib4ubvARB");
		glVertexAttrib4uivARB = (PFNGLVERTEXATTRIB4UIVARBPROC)getProcAddress("glVertexAttrib4uivARB");
		glVertexAttrib4usvARB = (PFNGLVERTEXATTRIB4USVARBPROC)getProcAddress("glVertexAttrib4usvARB");
		glVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)getProcAddress("glVertexAttribPointerARB");
		glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)getProcAddress("glEnableVertexAttribArrayARB");
		glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)getProcAddress("glDisableVertexAttribArrayARB");
		glProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)getProcAddress("glProgramStringARB");
		glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)getProcAddress("glBindProgramARB");
		glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)getProcAddress("glDeleteProgramsARB");
		glGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)getProcAddress("glGenProgramsARB");
		glProgramEnvParameter4dARB = (PFNGLPROGRAMENVPARAMETER4DARBPROC)getProcAddress("glProgramEnvParameter4dARB");
		glProgramEnvParameter4dvARB = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)getProcAddress("glProgramEnvParameter4dvARB");
		glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)getProcAddress("glProgramEnvParameter4fARB");
		glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)getProcAddress("glProgramEnvParameter4fvARB");
		glProgramLocalParameter4dARB = (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)getProcAddress("glProgramLocalParameter4dARB");
		glProgramLocalParameter4dvARB = (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)getProcAddress("glProgramLocalParameter4dvARB");
		glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)getProcAddress("glProgramLocalParameter4fARB");
		glProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)getProcAddress("glProgramLocalParameter4fvARB");
		glGetProgramEnvParameterdvARB = (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)getProcAddress("glGetProgramEnvParameterdvARB");
		glGetProgramEnvParameterfvARB = (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)getProcAddress("glGetProgramEnvParameterfvARB");
		glGetProgramLocalParameterdvARB = (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)getProcAddress("glGetProgramLocalParameterdvARB");
		glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)getProcAddress("glGetProgramLocalParameterfvARB");
		glGetProgramivARB = (PFNGLGETPROGRAMIVARBPROC)getProcAddress("glGetProgramivARB");
		glGetProgramStringARB = (PFNGLGETPROGRAMSTRINGARBPROC)getProcAddress("glGetProgramStringARB");
		glGetVertexAttribdvARB = (PFNGLGETVERTEXATTRIBDVARBPROC)getProcAddress("glGetVertexAttribdvARB");
		glGetVertexAttribfvARB = (PFNGLGETVERTEXATTRIBFVARBPROC)getProcAddress("glGetVertexAttribfvARB");
		glGetVertexAttribivARB = (PFNGLGETVERTEXATTRIBIVARBPROC)getProcAddress("glGetVertexAttribivARB");
		glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)getProcAddress("glGetVertexAttribPointervARB");
		glIsProgramARB = (PFNGLISPROGRAMARBPROC)getProcAddress("glIsProgramARB");

		// GL_ARB_vertex_buffer_object
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)getProcAddress("glBindBufferARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)getProcAddress("glDeleteBuffersARB");
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)getProcAddress("glGenBuffersARB");
		glIsBufferARB = (PFNGLISBUFFERARBPROC)getProcAddress("glIsBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)getProcAddress("glBufferDataARB");
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)getProcAddress("glBufferSubDataARB");
		glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)getProcAddress("glGetBufferSubDataARB");
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC)getProcAddress("glMapBufferARB");
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)getProcAddress("glUnmapBufferARB");
		glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)getProcAddress("glGetBufferParameterivARB");
		glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)getProcAddress("glGetBufferPointervARB");

		// GL_ARB_occlusion_query
		glGenQueriesARB = (PFNGLGENQUERIESARBPROC)getProcAddress("glGenQueriesARB");
		glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)getProcAddress("glDeleteQueriesARB");
		glIsQueryARB = (PFNGLISQUERYARBPROC)getProcAddress("glIsQueryARB");
		glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)getProcAddress("glBeginQueryARB");
		glEndQueryARB = (PFNGLENDQUERYARBPROC)getProcAddress("glEndQueryARB");
		glGetQueryivARB = (PFNGLGETQUERYIVARBPROC)getProcAddress("glGetQueryivARB");
		glGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)getProcAddress("glGetQueryObjectivARB");
		glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)getProcAddress("glGetQueryObjectuivARB");

		// GL_ARB_shader_objects
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)getProcAddress("glDeleteObjectARB");
		glGetHandleARB = (PFNGLGETHANDLEARBPROC)getProcAddress("glGetHandleARB");
		glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)getProcAddress("glDetachObjectARB");
		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)getProcAddress("glCreateShaderObjectARB");
		glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)getProcAddress("glShaderSourceARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)getProcAddress("glCompileShaderARB");
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)getProcAddress("glCreateProgramObjectARB");
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)getProcAddress("glAttachObjectARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)getProcAddress("glLinkProgramARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)getProcAddress("glUseProgramObjectARB");
		glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)getProcAddress("glValidateProgramARB");
		glUniform1fARB = (PFNGLUNIFORM1FARBPROC)getProcAddress("glUniform1fARB");
		glUniform2fARB = (PFNGLUNIFORM2FARBPROC)getProcAddress("glUniform2fARB");
		glUniform3fARB = (PFNGLUNIFORM3FARBPROC)getProcAddress("glUniform3fARB");
		glUniform4fARB = (PFNGLUNIFORM4FARBPROC)getProcAddress("glUniform4fARB");
		glUniform1iARB = (PFNGLUNIFORM1IARBPROC)getProcAddress("glUniform1iARB");
		glUniform2iARB = (PFNGLUNIFORM2IARBPROC)getProcAddress("glUniform2iARB");
		glUniform3iARB = (PFNGLUNIFORM3IARBPROC)getProcAddress("glUniform3iARB");
		glUniform4iARB = (PFNGLUNIFORM4IARBPROC)getProcAddress("glUniform4iARB");
		glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)getProcAddress("glUniform1fvARB");
		glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)getProcAddress("glUniform2fvARB");
		glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)getProcAddress("glUniform3fvARB");
		glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)getProcAddress("glUniform4fvARB");
		glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)getProcAddress("glUniform1ivARB");
		glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)getProcAddress("glUniform2ivARB");
		glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)getProcAddress("glUniform3ivARB");
		glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)getProcAddress("glUniform4ivARB");
		glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)getProcAddress("glUniformMatrix2fvARB");
		glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)getProcAddress("glUniformMatrix3fvARB");
		glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)getProcAddress("glUniformMatrix4fvARB");
		glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)getProcAddress("glGetObjectParameterfvARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)getProcAddress("glGetObjectParameterivARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)getProcAddress("glGetInfoLogARB");
		glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)getProcAddress("glGetAttachedObjectsARB");
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)getProcAddress("glGetUniformLocationARB");
		glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)getProcAddress("glGetActiveUniformARB");
		glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)getProcAddress("glGetUniformfvARB");
		glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)getProcAddress("glGetUniformivARB");
		glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)getProcAddress("glGetShaderSourceARB");

		// GL_ARB_vertex_shader
		glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)getProcAddress("glBindAttribLocationARB");
		glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC)getProcAddress("glGetActiveAttribARB");
		glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)getProcAddress("glGetAttribLocationARB");

		// GL_ARB_draw_buffers
		glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)getProcAddress("glDrawBuffersARB");

		// GL_ARB_color_buffer_float
		glClampColorARB = (PFNGLCLAMPCOLORARBPROC)getProcAddress("glClampColorARB");

		// GL_ARB_draw_instanced
		glDrawArraysInstancedARB = (PFNGLDRAWARRAYSINSTANCEDARBPROC)getProcAddress("glDrawArraysInstancedARB");
		glDrawElementsInstancedARB = (PFNGLDRAWELEMENTSINSTANCEDARBPROC)getProcAddress("glDrawElementsInstancedARB");

		// GL_ARB_framebuffer_object
		glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)getProcAddress("glIsRenderbuffer");
		glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)getProcAddress("glBindRenderbuffer");
		glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)getProcAddress("glDeleteRenderbuffers");
		glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)getProcAddress("glGenRenderbuffers");
		glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)getProcAddress("glRenderbufferStorage");
		glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)getProcAddress("glGetRenderbufferParameteriv");
		glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)getProcAddress("glIsFramebuffer");
		glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)getProcAddress("glBindFramebuffer");
		glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)getProcAddress("glDeleteFramebuffers");
		glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)getProcAddress("glGenFramebuffers");
		glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)getProcAddress("glCheckFramebufferStatus");
		glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)getProcAddress("glFramebufferTexture1D");
		glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)getProcAddress("glFramebufferTexture2D");
		glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)getProcAddress("glFramebufferTexture3D");
		glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)getProcAddress("glFramebufferRenderbuffer");
		glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)getProcAddress("glGetFramebufferAttachmentParameteriv");
		glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getProcAddress("glGenerateMipmap");
		glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)getProcAddress("glBlitFramebuffer");
		glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)getProcAddress("glRenderbufferStorageMultisample");
		glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)getProcAddress("glFramebufferTextureLayer");

		// GL_ARB_geometry_shader4
		glProgramParameteriARB = (PFNGLPROGRAMPARAMETERIARBPROC)getProcAddress("glProgramParameteriARB");
		glFramebufferTextureARB = (PFNGLFRAMEBUFFERTEXTUREARBPROC)getProcAddress("glFramebufferTextureARB");
		glFramebufferTextureLayerARB = (PFNGLFRAMEBUFFERTEXTURELAYERARBPROC)getProcAddress("glFramebufferTextureLayerARB");
		glFramebufferTextureFaceARB = (PFNGLFRAMEBUFFERTEXTUREFACEARBPROC)getProcAddress("glFramebufferTextureFaceARB");

		// GL_ARB_instanced_arrays
		glVertexAttribDivisorARB = (PFNGLVERTEXATTRIBDIVISORARBPROC)getProcAddress("glVertexAttribDivisorARB");

		// GL_ARB_map_buffer_range
		glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)getProcAddress("glMapBufferRange");
		glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)getProcAddress("glFlushMappedBufferRange");

		// GL_ARB_texture_buffer_object
		glTexBufferARB = (PFNGLTEXBUFFERARBPROC)getProcAddress("glTexBufferARB");

		// GL_ARB_vertex_array_object
		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getProcAddress("glBindVertexArray");
		glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)getProcAddress("glDeleteVertexArrays");
		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getProcAddress("glGenVertexArrays");
		glIsVertexArray = (PFNGLISVERTEXARRAYPROC)getProcAddress("glIsVertexArray");

		// GL_ARB_uniform_buffer_object
		glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)getProcAddress("glGetUniformIndices");
		glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)getProcAddress("glGetActiveUniformsiv");
		glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)getProcAddress("glGetActiveUniformName");
		glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)getProcAddress("glGetUniformBlockIndex");
		glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)getProcAddress("glGetActiveUniformBlockiv");
		glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)getProcAddress("glGetActiveUniformBlockName");
		glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)getProcAddress("glUniformBlockBinding");

		// GL_ARB_copy_buffer
		glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)getProcAddress("glCopyBufferSubData");

		// GL_ARB_draw_elements_base_vertex
		glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)getProcAddress("glDrawElementsBaseVertex");
		glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)getProcAddress("glDrawRangeElementsBaseVertex");
		glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)getProcAddress("glDrawElementsInstancedBaseVertex");
		glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)getProcAddress("glMultiDrawElementsBaseVertex");

		// GL_ARB_provoking_vertex
		glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)getProcAddress("glProvokingVertex");

		// GL_ARB_sync
		glFenceSync = (PFNGLFENCESYNCPROC)getProcAddress("glFenceSync");
		glIsSync = (PFNGLISSYNCPROC)getProcAddress("glIsSync");
		glDeleteSync = (PFNGLDELETESYNCPROC)getProcAddress("glDeleteSync");
		glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)getProcAddress("glClientWaitSync");
		glWaitSync = (PFNGLWAITSYNCPROC)getProcAddress("glWaitSync");
		glGetInteger64v = (PFNGLGETINTEGER64VPROC)getProcAddress("glGetInteger64v");
		glGetSynciv = (PFNGLGETSYNCIVPROC)getProcAddress("glGetSynciv");

		// GL_ARB_texture_multisample
		glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)getProcAddress("glTexImage2DMultisample");
		glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)getProcAddress("glTexImage3DMultisample");
		glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)getProcAddress("glGetMultisamplefv");
		glSampleMaski = (PFNGLSAMPLEMASKIPROC)getProcAddress("glSampleMaski");

		// GL_ARB_draw_buffers_blend
		glBlendEquationiARB = (PFNGLBLENDEQUATIONIARBPROC)getProcAddress("glBlendEquationiARB");
		glBlendEquationSeparateiARB = (PFNGLBLENDEQUATIONSEPARATEIARBPROC)getProcAddress("glBlendEquationSeparateiARB");
		glBlendFunciARB = (PFNGLBLENDFUNCIARBPROC)getProcAddress("glBlendFunciARB");
		glBlendFuncSeparateiARB = (PFNGLBLENDFUNCSEPARATEIARBPROC)getProcAddress("glBlendFuncSeparateiARB");

		// GL_ARB_sample_shading
		glMinSampleShadingARB = (PFNGLMINSAMPLESHADINGARBPROC)getProcAddress("glMinSampleShadingARB");

		// GL_ARB_shading_language_include
		glNamedStringARB = (PFNGLNAMEDSTRINGARBPROC)getProcAddress("glNamedStringARB");
		glDeleteNamedStringARB = (PFNGLDELETENAMEDSTRINGARBPROC)getProcAddress("glDeleteNamedStringARB");
		glCompileShaderIncludeARB = (PFNGLCOMPILESHADERINCLUDEARBPROC)getProcAddress("glCompileShaderIncludeARB");
		glIsNamedStringARB = (PFNGLISNAMEDSTRINGARBPROC)getProcAddress("glIsNamedStringARB");
		glGetNamedStringARB = (PFNGLGETNAMEDSTRINGARBPROC)getProcAddress("glGetNamedStringARB");
		glGetNamedStringivARB = (PFNGLGETNAMEDSTRINGIVARBPROC)getProcAddress("glGetNamedStringivARB");

		// GL_ARB_blend_func_extended
		glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)getProcAddress("glBindFragDataLocationIndexed");
		glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)getProcAddress("glGetFragDataIndex");

		// GL_ARB_sampler_objects
		glGenSamplers = (PFNGLGENSAMPLERSPROC)getProcAddress("glGenSamplers");
		glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)getProcAddress("glDeleteSamplers");
		glIsSampler = (PFNGLISSAMPLERPROC)getProcAddress("glIsSampler");
		glBindSampler = (PFNGLBINDSAMPLERPROC)getProcAddress("glBindSampler");
		glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)getProcAddress("glSamplerParameteri");
		glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)getProcAddress("glSamplerParameteriv");
		glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)getProcAddress("glSamplerParameterf");
		glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)getProcAddress("glSamplerParameterfv");
		glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)getProcAddress("glSamplerParameterIiv");
		glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)getProcAddress("glSamplerParameterIuiv");
		glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)getProcAddress("glGetSamplerParameteriv");
		glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)getProcAddress("glGetSamplerParameterIiv");
		glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)getProcAddress("glGetSamplerParameterfv");
		glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)getProcAddress("glGetSamplerParameterIuiv");

		// GL_ARB_timer_query
		glQueryCounter = (PFNGLQUERYCOUNTERPROC)getProcAddress("glQueryCounter");
		glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)getProcAddress("glGetQueryObjecti64v");
		glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)getProcAddress("glGetQueryObjectui64v");

		// GL_ARB_vertex_type_2_10_10_10_rev
		glVertexP2ui = (PFNGLVERTEXP2UIPROC)getProcAddress("glVertexP2ui");
		glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)getProcAddress("glVertexP2uiv");
		glVertexP3ui = (PFNGLVERTEXP3UIPROC)getProcAddress("glVertexP3ui");
		glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)getProcAddress("glVertexP3uiv");
		glVertexP4ui = (PFNGLVERTEXP4UIPROC)getProcAddress("glVertexP4ui");
		glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)getProcAddress("glVertexP4uiv");
		glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)getProcAddress("glTexCoordP1ui");
		glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)getProcAddress("glTexCoordP1uiv");
		glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)getProcAddress("glTexCoordP2ui");
		glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)getProcAddress("glTexCoordP2uiv");
		glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)getProcAddress("glTexCoordP3ui");
		glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)getProcAddress("glTexCoordP3uiv");
		glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)getProcAddress("glTexCoordP4ui");
		glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)getProcAddress("glTexCoordP4uiv");
		glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)getProcAddress("glMultiTexCoordP1ui");
		glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)getProcAddress("glMultiTexCoordP1uiv");
		glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)getProcAddress("glMultiTexCoordP2ui");
		glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)getProcAddress("glMultiTexCoordP2uiv");
		glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)getProcAddress("glMultiTexCoordP3ui");
		glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)getProcAddress("glMultiTexCoordP3uiv");
		glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)getProcAddress("glMultiTexCoordP4ui");
		glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)getProcAddress("glMultiTexCoordP4uiv");
		glNormalP3ui = (PFNGLNORMALP3UIPROC)getProcAddress("glNormalP3ui");
		glNormalP3uiv = (PFNGLNORMALP3UIVPROC)getProcAddress("glNormalP3uiv");
		glColorP3ui = (PFNGLCOLORP3UIPROC)getProcAddress("glColorP3ui");
		glColorP3uiv = (PFNGLCOLORP3UIVPROC)getProcAddress("glColorP3uiv");
		glColorP4ui = (PFNGLCOLORP4UIPROC)getProcAddress("glColorP4ui");
		glColorP4uiv = (PFNGLCOLORP4UIVPROC)getProcAddress("glColorP4uiv");
		glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)getProcAddress("glSecondaryColorP3ui");
		glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)getProcAddress("glSecondaryColorP3uiv");
		glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)getProcAddress("glVertexAttribP1ui");
		glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)getProcAddress("glVertexAttribP1uiv");
		glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)getProcAddress("glVertexAttribP2ui");
		glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)getProcAddress("glVertexAttribP2uiv");
		glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)getProcAddress("glVertexAttribP3ui");
		glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)getProcAddress("glVertexAttribP3uiv");
		glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)getProcAddress("glVertexAttribP4ui");
		glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)getProcAddress("glVertexAttribP4uiv");

		// GL_ARB_draw_indirect
		glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)getProcAddress("glDrawArraysIndirect");
		glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)getProcAddress("glDrawElementsIndirect");

		// GL_ARB_gpu_shader_fp64
		glUniform1d = (PFNGLUNIFORM1DPROC)getProcAddress("glUniform1d");
		glUniform2d = (PFNGLUNIFORM2DPROC)getProcAddress("glUniform2d");
		glUniform3d = (PFNGLUNIFORM3DPROC)getProcAddress("glUniform3d");
		glUniform4d = (PFNGLUNIFORM4DPROC)getProcAddress("glUniform4d");
		glUniform1dv = (PFNGLUNIFORM1DVPROC)getProcAddress("glUniform1dv");
		glUniform2dv = (PFNGLUNIFORM2DVPROC)getProcAddress("glUniform2dv");
		glUniform3dv = (PFNGLUNIFORM3DVPROC)getProcAddress("glUniform3dv");
		glUniform4dv = (PFNGLUNIFORM4DVPROC)getProcAddress("glUniform4dv");
		glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)getProcAddress("glUniformMatrix2dv");
		glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)getProcAddress("glUniformMatrix3dv");
		glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)getProcAddress("glUniformMatrix4dv");
		glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)getProcAddress("glUniformMatrix2x3dv");
		glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)getProcAddress("glUniformMatrix2x4dv");
		glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)getProcAddress("glUniformMatrix3x2dv");
		glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)getProcAddress("glUniformMatrix3x4dv");
		glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)getProcAddress("glUniformMatrix4x2dv");
		glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)getProcAddress("glUniformMatrix4x3dv");
		glGetUniformdv = (PFNGLGETUNIFORMDVPROC)getProcAddress("glGetUniformdv");

		// GL_ARB_shader_subroutine
		glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)getProcAddress("glGetSubroutineUniformLocation");
		glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)getProcAddress("glGetSubroutineIndex");
		glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)getProcAddress("glGetActiveSubroutineUniformiv");
		glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)getProcAddress("glGetActiveSubroutineUniformName");
		glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)getProcAddress("glGetActiveSubroutineName");
		glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)getProcAddress("glUniformSubroutinesuiv");
		glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)getProcAddress("glGetUniformSubroutineuiv");
		glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)getProcAddress("glGetProgramStageiv");

		// GL_ARB_tessellation_shader
		glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)getProcAddress("glPatchParameteri");
		glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)getProcAddress("glPatchParameterfv");

		// GL_ARB_transform_feedback2
		glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)getProcAddress("glBindTransformFeedback");
		glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)getProcAddress("glDeleteTransformFeedbacks");
		glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)getProcAddress("glGenTransformFeedbacks");
		glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)getProcAddress("glIsTransformFeedback");
		glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)getProcAddress("glPauseTransformFeedback");
		glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)getProcAddress("glResumeTransformFeedback");
		glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)getProcAddress("glDrawTransformFeedback");

		// GL_ARB_transform_feedback3
		glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)getProcAddress("glDrawTransformFeedbackStream");
		glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)getProcAddress("glBeginQueryIndexed");
		glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)getProcAddress("glEndQueryIndexed");
		glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)getProcAddress("glGetQueryIndexediv");

		// GL_ARB_ES2_compatibility
		glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)getProcAddress("glReleaseShaderCompiler");
		glShaderBinary = (PFNGLSHADERBINARYPROC)getProcAddress("glShaderBinary");
		glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)getProcAddress("glGetShaderPrecisionFormat");
		glDepthRangef = (PFNGLDEPTHRANGEFPROC)getProcAddress("glDepthRangef");
		glClearDepthf = (PFNGLCLEARDEPTHFPROC)getProcAddress("glClearDepthf");

		// GL_ARB_get_program_binary
		glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)getProcAddress("glGetProgramBinary");
		glProgramBinary = (PFNGLPROGRAMBINARYPROC)getProcAddress("glProgramBinary");
		glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)getProcAddress("glProgramParameteri");

		// GL_ARB_separate_shader_objects
		glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)getProcAddress("glUseProgramStages");
		glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)getProcAddress("glActiveShaderProgram");
		glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)getProcAddress("glCreateShaderProgramv");
		glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)getProcAddress("glBindProgramPipeline");
		glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)getProcAddress("glDeleteProgramPipelines");
		glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)getProcAddress("glGenProgramPipelines");
		glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)getProcAddress("glIsProgramPipeline");
		glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)getProcAddress("glGetProgramPipelineiv");
		glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)getProcAddress("glProgramUniform1i");
		glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)getProcAddress("glProgramUniform1iv");
		glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)getProcAddress("glProgramUniform1f");
		glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)getProcAddress("glProgramUniform1fv");
		glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)getProcAddress("glProgramUniform1d");
		glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)getProcAddress("glProgramUniform1dv");
		glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)getProcAddress("glProgramUniform1ui");
		glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)getProcAddress("glProgramUniform1uiv");
		glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)getProcAddress("glProgramUniform2i");
		glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)getProcAddress("glProgramUniform2iv");
		glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)getProcAddress("glProgramUniform2f");
		glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)getProcAddress("glProgramUniform2fv");
		glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)getProcAddress("glProgramUniform2d");
		glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)getProcAddress("glProgramUniform2dv");
		glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)getProcAddress("glProgramUniform2ui");
		glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)getProcAddress("glProgramUniform2uiv");
		glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)getProcAddress("glProgramUniform3i");
		glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)getProcAddress("glProgramUniform3iv");
		glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)getProcAddress("glProgramUniform3f");
		glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)getProcAddress("glProgramUniform3fv");
		glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)getProcAddress("glProgramUniform3d");
		glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)getProcAddress("glProgramUniform3dv");
		glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)getProcAddress("glProgramUniform3ui");
		glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)getProcAddress("glProgramUniform3uiv");
		glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)getProcAddress("glProgramUniform4i");
		glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)getProcAddress("glProgramUniform4iv");
		glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)getProcAddress("glProgramUniform4f");
		glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)getProcAddress("glProgramUniform4fv");
		glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)getProcAddress("glProgramUniform4d");
		glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)getProcAddress("glProgramUniform4dv");
		glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)getProcAddress("glProgramUniform4ui");
		glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)getProcAddress("glProgramUniform4uiv");
		glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)getProcAddress("glProgramUniformMatrix2fv");
		glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)getProcAddress("glProgramUniformMatrix3fv");
		glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)getProcAddress("glProgramUniformMatrix4fv");
		glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)getProcAddress("glProgramUniformMatrix2dv");
		glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)getProcAddress("glProgramUniformMatrix3dv");
		glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)getProcAddress("glProgramUniformMatrix4dv");
		glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)getProcAddress("glProgramUniformMatrix2x3fv");
		glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)getProcAddress("glProgramUniformMatrix3x2fv");
		glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)getProcAddress("glProgramUniformMatrix2x4fv");
		glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)getProcAddress("glProgramUniformMatrix4x2fv");
		glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)getProcAddress("glProgramUniformMatrix3x4fv");
		glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)getProcAddress("glProgramUniformMatrix4x3fv");
		glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)getProcAddress("glProgramUniformMatrix2x3dv");
		glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)getProcAddress("glProgramUniformMatrix3x2dv");
		glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)getProcAddress("glProgramUniformMatrix2x4dv");
		glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)getProcAddress("glProgramUniformMatrix4x2dv");
		glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)getProcAddress("glProgramUniformMatrix3x4dv");
		glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)getProcAddress("glProgramUniformMatrix4x3dv");
		glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)getProcAddress("glValidateProgramPipeline");
		glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)getProcAddress("glGetProgramPipelineInfoLog");

		// GL_ARB_vertex_attrib_64bit
		glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)getProcAddress("glVertexAttribL1d");
		glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)getProcAddress("glVertexAttribL2d");
		glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)getProcAddress("glVertexAttribL3d");
		glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)getProcAddress("glVertexAttribL4d");
		glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)getProcAddress("glVertexAttribL1dv");
		glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)getProcAddress("glVertexAttribL2dv");
		glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)getProcAddress("glVertexAttribL3dv");
		glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)getProcAddress("glVertexAttribL4dv");
		glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)getProcAddress("glVertexAttribLPointer");
		glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)getProcAddress("glGetVertexAttribLdv");

		// GL_ARB_viewport_array
		glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)getProcAddress("glViewportArrayv");
		glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)getProcAddress("glViewportIndexedf");
		glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)getProcAddress("glViewportIndexedfv");
		glScissorArrayv = (PFNGLSCISSORARRAYVPROC)getProcAddress("glScissorArrayv");
		glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)getProcAddress("glScissorIndexed");
		glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)getProcAddress("glScissorIndexedv");
		glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)getProcAddress("glDepthRangeArrayv");
		glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)getProcAddress("glDepthRangeIndexed");
		glGetFloati_v = (PFNGLGETFLOATI_VPROC)getProcAddress("glGetFloati_v");
		glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)getProcAddress("glGetDoublei_v");

		// GL_ARB_cl_event
		glCreateSyncFromCLeventARB = (PFNGLCREATESYNCFROMCLEVENTARBPROC)getProcAddress("glCreateSyncFromCLeventARB");

		// GL_ARB_debug_output
		glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC)getProcAddress("glDebugMessageControlARB");
		glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC)getProcAddress("glDebugMessageInsertARB");
		glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)getProcAddress("glDebugMessageCallbackARB");
		glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC)getProcAddress("glGetDebugMessageLogARB");

		// GL_ARB_robustness
		glGetGraphicsResetStatusARB = (PFNGLGETGRAPHICSRESETSTATUSARBPROC)getProcAddress("glGetGraphicsResetStatusARB");
		glGetnMapdvARB = (PFNGLGETNMAPDVARBPROC)getProcAddress("glGetnMapdvARB");
		glGetnMapfvARB = (PFNGLGETNMAPFVARBPROC)getProcAddress("glGetnMapfvARB");
		glGetnMapivARB = (PFNGLGETNMAPIVARBPROC)getProcAddress("glGetnMapivARB");
		glGetnPixelMapfvARB = (PFNGLGETNPIXELMAPFVARBPROC)getProcAddress("glGetnPixelMapfvARB");
		glGetnPixelMapuivARB = (PFNGLGETNPIXELMAPUIVARBPROC)getProcAddress("glGetnPixelMapuivARB");
		glGetnPixelMapusvARB = (PFNGLGETNPIXELMAPUSVARBPROC)getProcAddress("glGetnPixelMapusvARB");
		glGetnPolygonStippleARB = (PFNGLGETNPOLYGONSTIPPLEARBPROC)getProcAddress("glGetnPolygonStippleARB");
		glGetnColorTableARB = (PFNGLGETNCOLORTABLEARBPROC)getProcAddress("glGetnColorTableARB");
		glGetnConvolutionFilterARB = (PFNGLGETNCONVOLUTIONFILTERARBPROC)getProcAddress("glGetnConvolutionFilterARB");
		glGetnSeparableFilterARB = (PFNGLGETNSEPARABLEFILTERARBPROC)getProcAddress("glGetnSeparableFilterARB");
		glGetnHistogramARB = (PFNGLGETNHISTOGRAMARBPROC)getProcAddress("glGetnHistogramARB");
		glGetnMinmaxARB = (PFNGLGETNMINMAXARBPROC)getProcAddress("glGetnMinmaxARB");
		glGetnTexImageARB = (PFNGLGETNTEXIMAGEARBPROC)getProcAddress("glGetnTexImageARB");
		glReadnPixelsARB = (PFNGLREADNPIXELSARBPROC)getProcAddress("glReadnPixelsARB");
		glGetnCompressedTexImageARB = (PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC)getProcAddress("glGetnCompressedTexImageARB");
		glGetnUniformfvARB = (PFNGLGETNUNIFORMFVARBPROC)getProcAddress("glGetnUniformfvARB");
		glGetnUniformivARB = (PFNGLGETNUNIFORMIVARBPROC)getProcAddress("glGetnUniformivARB");
		glGetnUniformuivARB = (PFNGLGETNUNIFORMUIVARBPROC)getProcAddress("glGetnUniformuivARB");
		glGetnUniformdvARB = (PFNGLGETNUNIFORMDVARBPROC)getProcAddress("glGetnUniformdvARB");

		// GL_EXT_blend_color
		glBlendColorEXT = (PFNGLBLENDCOLOREXTPROC)getProcAddress("glBlendColorEXT");

		// GL_EXT_polygon_offset
		glPolygonOffsetEXT = (PFNGLPOLYGONOFFSETEXTPROC)getProcAddress("glPolygonOffsetEXT");

		// GL_EXT_texture3D
		glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC)getProcAddress("glTexImage3DEXT");
		glTexSubImage3DEXT = (PFNGLTEXSUBIMAGE3DEXTPROC)getProcAddress("glTexSubImage3DEXT");

		// GL_SGIS_texture_filter4
		glGetTexFilterFuncSGIS = (PFNGLGETTEXFILTERFUNCSGISPROC)getProcAddress("glGetTexFilterFuncSGIS");
		glTexFilterFuncSGIS = (PFNGLTEXFILTERFUNCSGISPROC)getProcAddress("glTexFilterFuncSGIS");

		// GL_EXT_subtexture
		glTexSubImage1DEXT = (PFNGLTEXSUBIMAGE1DEXTPROC)getProcAddress("glTexSubImage1DEXT");
		glTexSubImage2DEXT = (PFNGLTEXSUBIMAGE2DEXTPROC)getProcAddress("glTexSubImage2DEXT");

		// GL_EXT_copy_texture
		glCopyTexImage1DEXT = (PFNGLCOPYTEXIMAGE1DEXTPROC)getProcAddress("glCopyTexImage1DEXT");
		glCopyTexImage2DEXT = (PFNGLCOPYTEXIMAGE2DEXTPROC)getProcAddress("glCopyTexImage2DEXT");
		glCopyTexSubImage1DEXT = (PFNGLCOPYTEXSUBIMAGE1DEXTPROC)getProcAddress("glCopyTexSubImage1DEXT");
		glCopyTexSubImage2DEXT = (PFNGLCOPYTEXSUBIMAGE2DEXTPROC)getProcAddress("glCopyTexSubImage2DEXT");
		glCopyTexSubImage3DEXT = (PFNGLCOPYTEXSUBIMAGE3DEXTPROC)getProcAddress("glCopyTexSubImage3DEXT");

		// GL_EXT_histogram
		glGetHistogramEXT = (PFNGLGETHISTOGRAMEXTPROC)getProcAddress("glGetHistogramEXT");
		glGetHistogramParameterfvEXT = (PFNGLGETHISTOGRAMPARAMETERFVEXTPROC)getProcAddress("glGetHistogramParameterfvEXT");
		glGetHistogramParameterivEXT = (PFNGLGETHISTOGRAMPARAMETERIVEXTPROC)getProcAddress("glGetHistogramParameterivEXT");
		glGetMinmaxEXT = (PFNGLGETMINMAXEXTPROC)getProcAddress("glGetMinmaxEXT");
		glGetMinmaxParameterfvEXT = (PFNGLGETMINMAXPARAMETERFVEXTPROC)getProcAddress("glGetMinmaxParameterfvEXT");
		glGetMinmaxParameterivEXT = (PFNGLGETMINMAXPARAMETERIVEXTPROC)getProcAddress("glGetMinmaxParameterivEXT");
		glHistogramEXT = (PFNGLHISTOGRAMEXTPROC)getProcAddress("glHistogramEXT");
		glMinmaxEXT = (PFNGLMINMAXEXTPROC)getProcAddress("glMinmaxEXT");
		glResetHistogramEXT = (PFNGLRESETHISTOGRAMEXTPROC)getProcAddress("glResetHistogramEXT");
		glResetMinmaxEXT = (PFNGLRESETMINMAXEXTPROC)getProcAddress("glResetMinmaxEXT");

		// GL_EXT_convolution
		glConvolutionFilter1DEXT = (PFNGLCONVOLUTIONFILTER1DEXTPROC)getProcAddress("glConvolutionFilter1DEXT");
		glConvolutionFilter2DEXT = (PFNGLCONVOLUTIONFILTER2DEXTPROC)getProcAddress("glConvolutionFilter2DEXT");
		glConvolutionParameterfEXT = (PFNGLCONVOLUTIONPARAMETERFEXTPROC)getProcAddress("glConvolutionParameterfEXT");
		glConvolutionParameterfvEXT = (PFNGLCONVOLUTIONPARAMETERFVEXTPROC)getProcAddress("glConvolutionParameterfvEXT");
		glConvolutionParameteriEXT = (PFNGLCONVOLUTIONPARAMETERIEXTPROC)getProcAddress("glConvolutionParameteriEXT");
		glConvolutionParameterivEXT = (PFNGLCONVOLUTIONPARAMETERIVEXTPROC)getProcAddress("glConvolutionParameterivEXT");
		glCopyConvolutionFilter1DEXT = (PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC)getProcAddress("glCopyConvolutionFilter1DEXT");
		glCopyConvolutionFilter2DEXT = (PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC)getProcAddress("glCopyConvolutionFilter2DEXT");
		glGetConvolutionFilterEXT = (PFNGLGETCONVOLUTIONFILTEREXTPROC)getProcAddress("glGetConvolutionFilterEXT");
		glGetConvolutionParameterfvEXT = (PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC)getProcAddress("glGetConvolutionParameterfvEXT");
		glGetConvolutionParameterivEXT = (PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC)getProcAddress("glGetConvolutionParameterivEXT");
		glGetSeparableFilterEXT = (PFNGLGETSEPARABLEFILTEREXTPROC)getProcAddress("glGetSeparableFilterEXT");
		glSeparableFilter2DEXT = (PFNGLSEPARABLEFILTER2DEXTPROC)getProcAddress("glSeparableFilter2DEXT");

		// GL_SGI_color_table
		glColorTableSGI = (PFNGLCOLORTABLESGIPROC)getProcAddress("glColorTableSGI");
		glColorTableParameterfvSGI = (PFNGLCOLORTABLEPARAMETERFVSGIPROC)getProcAddress("glColorTableParameterfvSGI");
		glColorTableParameterivSGI = (PFNGLCOLORTABLEPARAMETERIVSGIPROC)getProcAddress("glColorTableParameterivSGI");
		glCopyColorTableSGI = (PFNGLCOPYCOLORTABLESGIPROC)getProcAddress("glCopyColorTableSGI");
		glGetColorTableSGI = (PFNGLGETCOLORTABLESGIPROC)getProcAddress("glGetColorTableSGI");
		glGetColorTableParameterfvSGI = (PFNGLGETCOLORTABLEPARAMETERFVSGIPROC)getProcAddress("glGetColorTableParameterfvSGI");
		glGetColorTableParameterivSGI = (PFNGLGETCOLORTABLEPARAMETERIVSGIPROC)getProcAddress("glGetColorTableParameterivSGI");

		// GL_SGIX_pixel_texture
		glPixelTexGenSGIX = (PFNGLPIXELTEXGENSGIXPROC)getProcAddress("glPixelTexGenSGIX");

		// GL_SGIS_pixel_texture
		glPixelTexGenParameteriSGIS = (PFNGLPIXELTEXGENPARAMETERISGISPROC)getProcAddress("glPixelTexGenParameteriSGIS");
		glPixelTexGenParameterivSGIS = (PFNGLPIXELTEXGENPARAMETERIVSGISPROC)getProcAddress("glPixelTexGenParameterivSGIS");
		glPixelTexGenParameterfSGIS = (PFNGLPIXELTEXGENPARAMETERFSGISPROC)getProcAddress("glPixelTexGenParameterfSGIS");
		glPixelTexGenParameterfvSGIS = (PFNGLPIXELTEXGENPARAMETERFVSGISPROC)getProcAddress("glPixelTexGenParameterfvSGIS");
		glGetPixelTexGenParameterivSGIS = (PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC)getProcAddress("glGetPixelTexGenParameterivSGIS");
		glGetPixelTexGenParameterfvSGIS = (PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC)getProcAddress("glGetPixelTexGenParameterfvSGIS");

		// GL_SGIS_texture4D
		glTexImage4DSGIS = (PFNGLTEXIMAGE4DSGISPROC)getProcAddress("glTexImage4DSGIS");
		glTexSubImage4DSGIS = (PFNGLTEXSUBIMAGE4DSGISPROC)getProcAddress("glTexSubImage4DSGIS");

		// GL_EXT_texture_object
		glAreTexturesResidentEXT = (PFNGLARETEXTURESRESIDENTEXTPROC)getProcAddress("glAreTexturesResidentEXT");
		glBindTextureEXT = (PFNGLBINDTEXTUREEXTPROC)getProcAddress("glBindTextureEXT");
		glDeleteTexturesEXT = (PFNGLDELETETEXTURESEXTPROC)getProcAddress("glDeleteTexturesEXT");
		glGenTexturesEXT = (PFNGLGENTEXTURESEXTPROC)getProcAddress("glGenTexturesEXT");
		glIsTextureEXT = (PFNGLISTEXTUREEXTPROC)getProcAddress("glIsTextureEXT");
		glPrioritizeTexturesEXT = (PFNGLPRIORITIZETEXTURESEXTPROC)getProcAddress("glPrioritizeTexturesEXT");

		// GL_SGIS_detail_texture
		glDetailTexFuncSGIS = (PFNGLDETAILTEXFUNCSGISPROC)getProcAddress("glDetailTexFuncSGIS");
		glGetDetailTexFuncSGIS = (PFNGLGETDETAILTEXFUNCSGISPROC)getProcAddress("glGetDetailTexFuncSGIS");

		// GL_SGIS_sharpen_texture
		glSharpenTexFuncSGIS = (PFNGLSHARPENTEXFUNCSGISPROC)getProcAddress("glSharpenTexFuncSGIS");
		glGetSharpenTexFuncSGIS = (PFNGLGETSHARPENTEXFUNCSGISPROC)getProcAddress("glGetSharpenTexFuncSGIS");

		// GL_SGIS_multisample
		glSampleMaskSGIS = (PFNGLSAMPLEMASKSGISPROC)getProcAddress("glSampleMaskSGIS");
		glSamplePatternSGIS = (PFNGLSAMPLEPATTERNSGISPROC)getProcAddress("glSamplePatternSGIS");

		// GL_EXT_vertex_array
		glArrayElementEXT = (PFNGLARRAYELEMENTEXTPROC)getProcAddress("glArrayElementEXT");
		glColorPointerEXT = (PFNGLCOLORPOINTEREXTPROC)getProcAddress("glColorPointerEXT");
		glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)getProcAddress("glDrawArraysEXT");
		glEdgeFlagPointerEXT = (PFNGLEDGEFLAGPOINTEREXTPROC)getProcAddress("glEdgeFlagPointerEXT");
		glGetPointervEXT = (PFNGLGETPOINTERVEXTPROC)getProcAddress("glGetPointervEXT");
		glIndexPointerEXT = (PFNGLINDEXPOINTEREXTPROC)getProcAddress("glIndexPointerEXT");
		glNormalPointerEXT = (PFNGLNORMALPOINTEREXTPROC)getProcAddress("glNormalPointerEXT");
		glTexCoordPointerEXT = (PFNGLTEXCOORDPOINTEREXTPROC)getProcAddress("glTexCoordPointerEXT");
		glVertexPointerEXT = (PFNGLVERTEXPOINTEREXTPROC)getProcAddress("glVertexPointerEXT");

		// GL_EXT_blend_minmax
		glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)getProcAddress("glBlendEquationEXT");

		// GL_SGIX_sprite
		glSpriteParameterfSGIX = (PFNGLSPRITEPARAMETERFSGIXPROC)getProcAddress("glSpriteParameterfSGIX");
		glSpriteParameterfvSGIX = (PFNGLSPRITEPARAMETERFVSGIXPROC)getProcAddress("glSpriteParameterfvSGIX");
		glSpriteParameteriSGIX = (PFNGLSPRITEPARAMETERISGIXPROC)getProcAddress("glSpriteParameteriSGIX");
		glSpriteParameterivSGIX = (PFNGLSPRITEPARAMETERIVSGIXPROC)getProcAddress("glSpriteParameterivSGIX");

		// GL_EXT_point_parameters
		glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)getProcAddress("glPointParameterfEXT");
		glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)getProcAddress("glPointParameterfvEXT");

		// GL_SGIS_point_parameters
		glPointParameterfvSGIS = (PFNGLPOINTPARAMETERFVSGISPROC)getProcAddress("glPointParameterfvSGIS");

		// GL_SGIX_instruments
		glGetInstrumentsSGIX = (PFNGLGETINSTRUMENTSSGIXPROC)getProcAddress("glGetInstrumentsSGIX");
		glInstrumentsBufferSGIX = (PFNGLINSTRUMENTSBUFFERSGIXPROC)getProcAddress("glInstrumentsBufferSGIX");
		glPollInstrumentsSGIX = (PFNGLPOLLINSTRUMENTSSGIXPROC)getProcAddress("glPollInstrumentsSGIX");
		glReadInstrumentsSGIX = (PFNGLREADINSTRUMENTSSGIXPROC)getProcAddress("glReadInstrumentsSGIX");
		glStartInstrumentsSGIX = (PFNGLSTARTINSTRUMENTSSGIXPROC)getProcAddress("glStartInstrumentsSGIX");
		glStopInstrumentsSGIX = (PFNGLSTOPINSTRUMENTSSGIXPROC)getProcAddress("glStopInstrumentsSGIX");

		// GL_SGIX_framezoom
		glFrameZoomSGIX = (PFNGLFRAMEZOOMSGIXPROC)getProcAddress("glFrameZoomSGIX");

		// GL_SGIX_tag_sample_buffer
		glTagSampleBufferSGIX = (PFNGLTAGSAMPLEBUFFERSGIXPROC)getProcAddress("glTagSampleBufferSGIX");

		// GL_SGIX_polynomial_ffd
		glDeformationMap3dSGIX = (PFNGLDEFORMATIONMAP3DSGIXPROC)getProcAddress("glDeformationMap3dSGIX");
		glDeformationMap3fSGIX = (PFNGLDEFORMATIONMAP3FSGIXPROC)getProcAddress("glDeformationMap3fSGIX");
		glDeformSGIX = (PFNGLDEFORMSGIXPROC)getProcAddress("glDeformSGIX");
		glLoadIdentityDeformationMapSGIX = (PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC)getProcAddress("glLoadIdentityDeformationMapSGIX");

		// GL_SGIX_reference_plane
		glReferencePlaneSGIX = (PFNGLREFERENCEPLANESGIXPROC)getProcAddress("glReferencePlaneSGIX");

		// GL_SGIX_flush_raster
		glFlushRasterSGIX = (PFNGLFLUSHRASTERSGIXPROC)getProcAddress("glFlushRasterSGIX");

		// GL_SGIS_fog_function
		glFogFuncSGIS = (PFNGLFOGFUNCSGISPROC)getProcAddress("glFogFuncSGIS");
		glGetFogFuncSGIS = (PFNGLGETFOGFUNCSGISPROC)getProcAddress("glGetFogFuncSGIS");

		// GL_HP_image_transform
		glImageTransformParameteriHP = (PFNGLIMAGETRANSFORMPARAMETERIHPPROC)getProcAddress("glImageTransformParameteriHP");
		glImageTransformParameterfHP = (PFNGLIMAGETRANSFORMPARAMETERFHPPROC)getProcAddress("glImageTransformParameterfHP");
		glImageTransformParameterivHP = (PFNGLIMAGETRANSFORMPARAMETERIVHPPROC)getProcAddress("glImageTransformParameterivHP");
		glImageTransformParameterfvHP = (PFNGLIMAGETRANSFORMPARAMETERFVHPPROC)getProcAddress("glImageTransformParameterfvHP");
		glGetImageTransformParameterivHP = (PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC)getProcAddress("glGetImageTransformParameterivHP");
		glGetImageTransformParameterfvHP = (PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC)getProcAddress("glGetImageTransformParameterfvHP");

		// GL_EXT_color_subtable
		glColorSubTableEXT = (PFNGLCOLORSUBTABLEEXTPROC)getProcAddress("glColorSubTableEXT");
		glCopyColorSubTableEXT = (PFNGLCOPYCOLORSUBTABLEEXTPROC)getProcAddress("glCopyColorSubTableEXT");

		// GL_PGI_misc_hints
		glHintPGI = (PFNGLHINTPGIPROC)getProcAddress("glHintPGI");

		// GL_EXT_paletted_texture
		glColorTableEXT = (PFNGLCOLORTABLEEXTPROC)getProcAddress("glColorTableEXT");
		glGetColorTableEXT = (PFNGLGETCOLORTABLEEXTPROC)getProcAddress("glGetColorTableEXT");
		glGetColorTableParameterivEXT = (PFNGLGETCOLORTABLEPARAMETERIVEXTPROC)getProcAddress("glGetColorTableParameterivEXT");
		glGetColorTableParameterfvEXT = (PFNGLGETCOLORTABLEPARAMETERFVEXTPROC)getProcAddress("glGetColorTableParameterfvEXT");

		// GL_SGIX_list_priority
		glGetListParameterfvSGIX = (PFNGLGETLISTPARAMETERFVSGIXPROC)getProcAddress("glGetListParameterfvSGIX");
		glGetListParameterivSGIX = (PFNGLGETLISTPARAMETERIVSGIXPROC)getProcAddress("glGetListParameterivSGIX");
		glListParameterfSGIX = (PFNGLLISTPARAMETERFSGIXPROC)getProcAddress("glListParameterfSGIX");
		glListParameterfvSGIX = (PFNGLLISTPARAMETERFVSGIXPROC)getProcAddress("glListParameterfvSGIX");
		glListParameteriSGIX = (PFNGLLISTPARAMETERISGIXPROC)getProcAddress("glListParameteriSGIX");
		glListParameterivSGIX = (PFNGLLISTPARAMETERIVSGIXPROC)getProcAddress("glListParameterivSGIX");

		// GL_EXT_index_material
		glIndexMaterialEXT = (PFNGLINDEXMATERIALEXTPROC)getProcAddress("glIndexMaterialEXT");

		// GL_EXT_index_func
		glIndexFuncEXT = (PFNGLINDEXFUNCEXTPROC)getProcAddress("glIndexFuncEXT");

		// GL_EXT_compiled_vertex_array
		glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC)getProcAddress("glLockArraysEXT");
		glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)getProcAddress("glUnlockArraysEXT");

		// GL_EXT_cull_vertex
		glCullParameterdvEXT = (PFNGLCULLPARAMETERDVEXTPROC)getProcAddress("glCullParameterdvEXT");
		glCullParameterfvEXT = (PFNGLCULLPARAMETERFVEXTPROC)getProcAddress("glCullParameterfvEXT");

		// GL_SGIX_fragment_lighting
		glFragmentColorMaterialSGIX = (PFNGLFRAGMENTCOLORMATERIALSGIXPROC)getProcAddress("glFragmentColorMaterialSGIX");
		glFragmentLightfSGIX = (PFNGLFRAGMENTLIGHTFSGIXPROC)getProcAddress("glFragmentLightfSGIX");
		glFragmentLightfvSGIX = (PFNGLFRAGMENTLIGHTFVSGIXPROC)getProcAddress("glFragmentLightfvSGIX");
		glFragmentLightiSGIX = (PFNGLFRAGMENTLIGHTISGIXPROC)getProcAddress("glFragmentLightiSGIX");
		glFragmentLightivSGIX = (PFNGLFRAGMENTLIGHTIVSGIXPROC)getProcAddress("glFragmentLightivSGIX");
		glFragmentLightModelfSGIX = (PFNGLFRAGMENTLIGHTMODELFSGIXPROC)getProcAddress("glFragmentLightModelfSGIX");
		glFragmentLightModelfvSGIX = (PFNGLFRAGMENTLIGHTMODELFVSGIXPROC)getProcAddress("glFragmentLightModelfvSGIX");
		glFragmentLightModeliSGIX = (PFNGLFRAGMENTLIGHTMODELISGIXPROC)getProcAddress("glFragmentLightModeliSGIX");
		glFragmentLightModelivSGIX = (PFNGLFRAGMENTLIGHTMODELIVSGIXPROC)getProcAddress("glFragmentLightModelivSGIX");
		glFragmentMaterialfSGIX = (PFNGLFRAGMENTMATERIALFSGIXPROC)getProcAddress("glFragmentMaterialfSGIX");
		glFragmentMaterialfvSGIX = (PFNGLFRAGMENTMATERIALFVSGIXPROC)getProcAddress("glFragmentMaterialfvSGIX");
		glFragmentMaterialiSGIX = (PFNGLFRAGMENTMATERIALISGIXPROC)getProcAddress("glFragmentMaterialiSGIX");
		glFragmentMaterialivSGIX = (PFNGLFRAGMENTMATERIALIVSGIXPROC)getProcAddress("glFragmentMaterialivSGIX");
		glGetFragmentLightfvSGIX = (PFNGLGETFRAGMENTLIGHTFVSGIXPROC)getProcAddress("glGetFragmentLightfvSGIX");
		glGetFragmentLightivSGIX = (PFNGLGETFRAGMENTLIGHTIVSGIXPROC)getProcAddress("glGetFragmentLightivSGIX");
		glGetFragmentMaterialfvSGIX = (PFNGLGETFRAGMENTMATERIALFVSGIXPROC)getProcAddress("glGetFragmentMaterialfvSGIX");
		glGetFragmentMaterialivSGIX = (PFNGLGETFRAGMENTMATERIALIVSGIXPROC)getProcAddress("glGetFragmentMaterialivSGIX");
		glLightEnviSGIX = (PFNGLLIGHTENVISGIXPROC)getProcAddress("glLightEnviSGIX");

		// GL_EXT_draw_range_elements
		glDrawRangeElementsEXT = (PFNGLDRAWRANGEELEMENTSEXTPROC)getProcAddress("glDrawRangeElementsEXT");

		// GL_EXT_light_texture
		glApplyTextureEXT = (PFNGLAPPLYTEXTUREEXTPROC)getProcAddress("glApplyTextureEXT");
		glTextureLightEXT = (PFNGLTEXTURELIGHTEXTPROC)getProcAddress("glTextureLightEXT");
		glTextureMaterialEXT = (PFNGLTEXTUREMATERIALEXTPROC)getProcAddress("glTextureMaterialEXT");

		// GL_SGIX_async
		glAsyncMarkerSGIX = (PFNGLASYNCMARKERSGIXPROC)getProcAddress("glAsyncMarkerSGIX");
		glFinishAsyncSGIX = (PFNGLFINISHASYNCSGIXPROC)getProcAddress("glFinishAsyncSGIX");
		glPollAsyncSGIX = (PFNGLPOLLASYNCSGIXPROC)getProcAddress("glPollAsyncSGIX");
		glGenAsyncMarkersSGIX = (PFNGLGENASYNCMARKERSSGIXPROC)getProcAddress("glGenAsyncMarkersSGIX");
		glDeleteAsyncMarkersSGIX = (PFNGLDELETEASYNCMARKERSSGIXPROC)getProcAddress("glDeleteAsyncMarkersSGIX");
		glIsAsyncMarkerSGIX = (PFNGLISASYNCMARKERSGIXPROC)getProcAddress("glIsAsyncMarkerSGIX");

		// GL_INTEL_parallel_arrays
		glVertexPointervINTEL = (PFNGLVERTEXPOINTERVINTELPROC)getProcAddress("glVertexPointervINTEL");
		glNormalPointervINTEL = (PFNGLNORMALPOINTERVINTELPROC)getProcAddress("glNormalPointervINTEL");
		glColorPointervINTEL = (PFNGLCOLORPOINTERVINTELPROC)getProcAddress("glColorPointervINTEL");
		glTexCoordPointervINTEL = (PFNGLTEXCOORDPOINTERVINTELPROC)getProcAddress("glTexCoordPointervINTEL");

		// GL_EXT_pixel_transform
		glPixelTransformParameteriEXT = (PFNGLPIXELTRANSFORMPARAMETERIEXTPROC)getProcAddress("glPixelTransformParameteriEXT");
		glPixelTransformParameterfEXT = (PFNGLPIXELTRANSFORMPARAMETERFEXTPROC)getProcAddress("glPixelTransformParameterfEXT");
		glPixelTransformParameterivEXT = (PFNGLPIXELTRANSFORMPARAMETERIVEXTPROC)getProcAddress("glPixelTransformParameterivEXT");
		glPixelTransformParameterfvEXT = (PFNGLPIXELTRANSFORMPARAMETERFVEXTPROC)getProcAddress("glPixelTransformParameterfvEXT");

		// GL_EXT_secondary_color
		glSecondaryColor3bEXT = (PFNGLSECONDARYCOLOR3BEXTPROC)getProcAddress("glSecondaryColor3bEXT");
		glSecondaryColor3bvEXT = (PFNGLSECONDARYCOLOR3BVEXTPROC)getProcAddress("glSecondaryColor3bvEXT");
		glSecondaryColor3dEXT = (PFNGLSECONDARYCOLOR3DEXTPROC)getProcAddress("glSecondaryColor3dEXT");
		glSecondaryColor3dvEXT = (PFNGLSECONDARYCOLOR3DVEXTPROC)getProcAddress("glSecondaryColor3dvEXT");
		glSecondaryColor3fEXT = (PFNGLSECONDARYCOLOR3FEXTPROC)getProcAddress("glSecondaryColor3fEXT");
		glSecondaryColor3fvEXT = (PFNGLSECONDARYCOLOR3FVEXTPROC)getProcAddress("glSecondaryColor3fvEXT");
		glSecondaryColor3iEXT = (PFNGLSECONDARYCOLOR3IEXTPROC)getProcAddress("glSecondaryColor3iEXT");
		glSecondaryColor3ivEXT = (PFNGLSECONDARYCOLOR3IVEXTPROC)getProcAddress("glSecondaryColor3ivEXT");
		glSecondaryColor3sEXT = (PFNGLSECONDARYCOLOR3SEXTPROC)getProcAddress("glSecondaryColor3sEXT");
		glSecondaryColor3svEXT = (PFNGLSECONDARYCOLOR3SVEXTPROC)getProcAddress("glSecondaryColor3svEXT");
		glSecondaryColor3ubEXT = (PFNGLSECONDARYCOLOR3UBEXTPROC)getProcAddress("glSecondaryColor3ubEXT");
		glSecondaryColor3ubvEXT = (PFNGLSECONDARYCOLOR3UBVEXTPROC)getProcAddress("glSecondaryColor3ubvEXT");
		glSecondaryColor3uiEXT = (PFNGLSECONDARYCOLOR3UIEXTPROC)getProcAddress("glSecondaryColor3uiEXT");
		glSecondaryColor3uivEXT = (PFNGLSECONDARYCOLOR3UIVEXTPROC)getProcAddress("glSecondaryColor3uivEXT");
		glSecondaryColor3usEXT = (PFNGLSECONDARYCOLOR3USEXTPROC)getProcAddress("glSecondaryColor3usEXT");
		glSecondaryColor3usvEXT = (PFNGLSECONDARYCOLOR3USVEXTPROC)getProcAddress("glSecondaryColor3usvEXT");
		glSecondaryColorPointerEXT = (PFNGLSECONDARYCOLORPOINTEREXTPROC)getProcAddress("glSecondaryColorPointerEXT");

		// GL_EXT_texture_perturb_normal
		glTextureNormalEXT = (PFNGLTEXTURENORMALEXTPROC)getProcAddress("glTextureNormalEXT");

		// GL_EXT_multi_draw_arrays
		glMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC)getProcAddress("glMultiDrawArraysEXT");
		glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)getProcAddress("glMultiDrawElementsEXT");

		// GL_EXT_fog_coord
		glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC)getProcAddress("glFogCoordfEXT");
		glFogCoordfvEXT = (PFNGLFOGCOORDFVEXTPROC)getProcAddress("glFogCoordfvEXT");
		glFogCoorddEXT = (PFNGLFOGCOORDDEXTPROC)getProcAddress("glFogCoorddEXT");
		glFogCoorddvEXT = (PFNGLFOGCOORDDVEXTPROC)getProcAddress("glFogCoorddvEXT");
		glFogCoordPointerEXT = (PFNGLFOGCOORDPOINTEREXTPROC)getProcAddress("glFogCoordPointerEXT");

		// GL_EXT_coordinate_frame
		glTangent3bEXT = (PFNGLTANGENT3BEXTPROC)getProcAddress("glTangent3bEXT");
		glTangent3bvEXT = (PFNGLTANGENT3BVEXTPROC)getProcAddress("glTangent3bvEXT");
		glTangent3dEXT = (PFNGLTANGENT3DEXTPROC)getProcAddress("glTangent3dEXT");
		glTangent3dvEXT = (PFNGLTANGENT3DVEXTPROC)getProcAddress("glTangent3dvEXT");
		glTangent3fEXT = (PFNGLTANGENT3FEXTPROC)getProcAddress("glTangent3fEXT");
		glTangent3fvEXT = (PFNGLTANGENT3FVEXTPROC)getProcAddress("glTangent3fvEXT");
		glTangent3iEXT = (PFNGLTANGENT3IEXTPROC)getProcAddress("glTangent3iEXT");
		glTangent3ivEXT = (PFNGLTANGENT3IVEXTPROC)getProcAddress("glTangent3ivEXT");
		glTangent3sEXT = (PFNGLTANGENT3SEXTPROC)getProcAddress("glTangent3sEXT");
		glTangent3svEXT = (PFNGLTANGENT3SVEXTPROC)getProcAddress("glTangent3svEXT");
		glBinormal3bEXT = (PFNGLBINORMAL3BEXTPROC)getProcAddress("glBinormal3bEXT");
		glBinormal3bvEXT = (PFNGLBINORMAL3BVEXTPROC)getProcAddress("glBinormal3bvEXT");
		glBinormal3dEXT = (PFNGLBINORMAL3DEXTPROC)getProcAddress("glBinormal3dEXT");
		glBinormal3dvEXT = (PFNGLBINORMAL3DVEXTPROC)getProcAddress("glBinormal3dvEXT");
		glBinormal3fEXT = (PFNGLBINORMAL3FEXTPROC)getProcAddress("glBinormal3fEXT");
		glBinormal3fvEXT = (PFNGLBINORMAL3FVEXTPROC)getProcAddress("glBinormal3fvEXT");
		glBinormal3iEXT = (PFNGLBINORMAL3IEXTPROC)getProcAddress("glBinormal3iEXT");
		glBinormal3ivEXT = (PFNGLBINORMAL3IVEXTPROC)getProcAddress("glBinormal3ivEXT");
		glBinormal3sEXT = (PFNGLBINORMAL3SEXTPROC)getProcAddress("glBinormal3sEXT");
		glBinormal3svEXT = (PFNGLBINORMAL3SVEXTPROC)getProcAddress("glBinormal3svEXT");
		glTangentPointerEXT = (PFNGLTANGENTPOINTEREXTPROC)getProcAddress("glTangentPointerEXT");
		glBinormalPointerEXT = (PFNGLBINORMALPOINTEREXTPROC)getProcAddress("glBinormalPointerEXT");

		// GL_SUNX_constant_data
		glFinishTextureSUNX = (PFNGLFINISHTEXTURESUNXPROC)getProcAddress("glFinishTextureSUNX");

		// GL_SUN_global_alpha
		glGlobalAlphaFactorbSUN = (PFNGLGLOBALALPHAFACTORBSUNPROC)getProcAddress("glGlobalAlphaFactorbSUN");
		glGlobalAlphaFactorsSUN = (PFNGLGLOBALALPHAFACTORSSUNPROC)getProcAddress("glGlobalAlphaFactorsSUN");
		glGlobalAlphaFactoriSUN = (PFNGLGLOBALALPHAFACTORISUNPROC)getProcAddress("glGlobalAlphaFactoriSUN");
		glGlobalAlphaFactorfSUN = (PFNGLGLOBALALPHAFACTORFSUNPROC)getProcAddress("glGlobalAlphaFactorfSUN");
		glGlobalAlphaFactordSUN = (PFNGLGLOBALALPHAFACTORDSUNPROC)getProcAddress("glGlobalAlphaFactordSUN");
		glGlobalAlphaFactorubSUN = (PFNGLGLOBALALPHAFACTORUBSUNPROC)getProcAddress("glGlobalAlphaFactorubSUN");
		glGlobalAlphaFactorusSUN = (PFNGLGLOBALALPHAFACTORUSSUNPROC)getProcAddress("glGlobalAlphaFactorusSUN");
		glGlobalAlphaFactoruiSUN = (PFNGLGLOBALALPHAFACTORUISUNPROC)getProcAddress("glGlobalAlphaFactoruiSUN");

		// GL_SUN_triangle_list
		glReplacementCodeuiSUN = (PFNGLREPLACEMENTCODEUISUNPROC)getProcAddress("glReplacementCodeuiSUN");
		glReplacementCodeusSUN = (PFNGLREPLACEMENTCODEUSSUNPROC)getProcAddress("glReplacementCodeusSUN");
		glReplacementCodeubSUN = (PFNGLREPLACEMENTCODEUBSUNPROC)getProcAddress("glReplacementCodeubSUN");
		glReplacementCodeuivSUN = (PFNGLREPLACEMENTCODEUIVSUNPROC)getProcAddress("glReplacementCodeuivSUN");
		glReplacementCodeusvSUN = (PFNGLREPLACEMENTCODEUSVSUNPROC)getProcAddress("glReplacementCodeusvSUN");
		glReplacementCodeubvSUN = (PFNGLREPLACEMENTCODEUBVSUNPROC)getProcAddress("glReplacementCodeubvSUN");
		glReplacementCodePointerSUN = (PFNGLREPLACEMENTCODEPOINTERSUNPROC)getProcAddress("glReplacementCodePointerSUN");

		// GL_SUN_vertex
		glColor4ubVertex2fSUN = (PFNGLCOLOR4UBVERTEX2FSUNPROC)getProcAddress("glColor4ubVertex2fSUN");
		glColor4ubVertex2fvSUN = (PFNGLCOLOR4UBVERTEX2FVSUNPROC)getProcAddress("glColor4ubVertex2fvSUN");
		glColor4ubVertex3fSUN = (PFNGLCOLOR4UBVERTEX3FSUNPROC)getProcAddress("glColor4ubVertex3fSUN");
		glColor4ubVertex3fvSUN = (PFNGLCOLOR4UBVERTEX3FVSUNPROC)getProcAddress("glColor4ubVertex3fvSUN");
		glColor3fVertex3fSUN = (PFNGLCOLOR3FVERTEX3FSUNPROC)getProcAddress("glColor3fVertex3fSUN");
		glColor3fVertex3fvSUN = (PFNGLCOLOR3FVERTEX3FVSUNPROC)getProcAddress("glColor3fVertex3fvSUN");
		glNormal3fVertex3fSUN = (PFNGLNORMAL3FVERTEX3FSUNPROC)getProcAddress("glNormal3fVertex3fSUN");
		glNormal3fVertex3fvSUN = (PFNGLNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glNormal3fVertex3fvSUN");
		glColor4fNormal3fVertex3fSUN = (PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC)getProcAddress("glColor4fNormal3fVertex3fSUN");
		glColor4fNormal3fVertex3fvSUN = (PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glColor4fNormal3fVertex3fvSUN");
		glTexCoord2fVertex3fSUN = (PFNGLTEXCOORD2FVERTEX3FSUNPROC)getProcAddress("glTexCoord2fVertex3fSUN");
		glTexCoord2fVertex3fvSUN = (PFNGLTEXCOORD2FVERTEX3FVSUNPROC)getProcAddress("glTexCoord2fVertex3fvSUN");
		glTexCoord4fVertex4fSUN = (PFNGLTEXCOORD4FVERTEX4FSUNPROC)getProcAddress("glTexCoord4fVertex4fSUN");
		glTexCoord4fVertex4fvSUN = (PFNGLTEXCOORD4FVERTEX4FVSUNPROC)getProcAddress("glTexCoord4fVertex4fvSUN");
		glTexCoord2fColor4ubVertex3fSUN = (PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC)getProcAddress("glTexCoord2fColor4ubVertex3fSUN");
		glTexCoord2fColor4ubVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC)getProcAddress("glTexCoord2fColor4ubVertex3fvSUN");
		glTexCoord2fColor3fVertex3fSUN = (PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC)getProcAddress("glTexCoord2fColor3fVertex3fSUN");
		glTexCoord2fColor3fVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC)getProcAddress("glTexCoord2fColor3fVertex3fvSUN");
		glTexCoord2fNormal3fVertex3fSUN = (PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC)getProcAddress("glTexCoord2fNormal3fVertex3fSUN");
		glTexCoord2fNormal3fVertex3fvSUN = (PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glTexCoord2fNormal3fVertex3fvSUN");
		glTexCoord2fColor4fNormal3fVertex3fSUN = (PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC)getProcAddress("glTexCoord2fColor4fNormal3fVertex3fSUN");
		glTexCoord2fColor4fNormal3fVertex3fvSUN = (PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glTexCoord2fColor4fNormal3fVertex3fvSUN");
		glTexCoord4fColor4fNormal3fVertex4fSUN = (PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC)getProcAddress("glTexCoord4fColor4fNormal3fVertex4fSUN");
		glTexCoord4fColor4fNormal3fVertex4fvSUN = (PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC)getProcAddress("glTexCoord4fColor4fNormal3fVertex4fvSUN");
		glReplacementCodeuiVertex3fSUN = (PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiVertex3fSUN");
		glReplacementCodeuiVertex3fvSUN = (PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiVertex3fvSUN");
		glReplacementCodeuiColor4ubVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiColor4ubVertex3fSUN");
		glReplacementCodeuiColor4ubVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiColor4ubVertex3fvSUN");
		glReplacementCodeuiColor3fVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiColor3fVertex3fSUN");
		glReplacementCodeuiColor3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiColor3fVertex3fvSUN");
		glReplacementCodeuiNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiNormal3fVertex3fSUN");
		glReplacementCodeuiNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiNormal3fVertex3fvSUN");
		glReplacementCodeuiColor4fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiColor4fNormal3fVertex3fSUN");
		glReplacementCodeuiColor4fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiColor4fNormal3fVertex3fvSUN");
		glReplacementCodeuiTexCoord2fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiTexCoord2fVertex3fSUN");
		glReplacementCodeuiTexCoord2fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiTexCoord2fVertex3fvSUN");
		glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN");
		glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN");
		glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC)getProcAddress("glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN");
		glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN = (PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC)getProcAddress("glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN");

		// GL_EXT_blend_func_separate
		glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)getProcAddress("glBlendFuncSeparateEXT");

		// GL_INGR_blend_func_separate
		glBlendFuncSeparateINGR = (PFNGLBLENDFUNCSEPARATEINGRPROC)getProcAddress("glBlendFuncSeparateINGR");

		// GL_EXT_vertex_weighting
		glVertexWeightfEXT = (PFNGLVERTEXWEIGHTFEXTPROC)getProcAddress("glVertexWeightfEXT");
		glVertexWeightfvEXT = (PFNGLVERTEXWEIGHTFVEXTPROC)getProcAddress("glVertexWeightfvEXT");
		glVertexWeightPointerEXT = (PFNGLVERTEXWEIGHTPOINTEREXTPROC)getProcAddress("glVertexWeightPointerEXT");

		// GL_NV_vertex_array_range
		glFlushVertexArrayRangeNV = (PFNGLFLUSHVERTEXARRAYRANGENVPROC)getProcAddress("glFlushVertexArrayRangeNV");
		glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)getProcAddress("glVertexArrayRangeNV");

		// GL_NV_register_combiners
		glCombinerParameterfvNV = (PFNGLCOMBINERPARAMETERFVNVPROC)getProcAddress("glCombinerParameterfvNV");
		glCombinerParameterfNV = (PFNGLCOMBINERPARAMETERFNVPROC)getProcAddress("glCombinerParameterfNV");
		glCombinerParameterivNV = (PFNGLCOMBINERPARAMETERIVNVPROC)getProcAddress("glCombinerParameterivNV");
		glCombinerParameteriNV = (PFNGLCOMBINERPARAMETERINVPROC)getProcAddress("glCombinerParameteriNV");
		glCombinerInputNV = (PFNGLCOMBINERINPUTNVPROC)getProcAddress("glCombinerInputNV");
		glCombinerOutputNV = (PFNGLCOMBINEROUTPUTNVPROC)getProcAddress("glCombinerOutputNV");
		glFinalCombinerInputNV = (PFNGLFINALCOMBINERINPUTNVPROC)getProcAddress("glFinalCombinerInputNV");
		glGetCombinerInputParameterfvNV = (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)getProcAddress("glGetCombinerInputParameterfvNV");
		glGetCombinerInputParameterivNV = (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)getProcAddress("glGetCombinerInputParameterivNV");
		glGetCombinerOutputParameterfvNV = (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)getProcAddress("glGetCombinerOutputParameterfvNV");
		glGetCombinerOutputParameterivNV = (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)getProcAddress("glGetCombinerOutputParameterivNV");
		glGetFinalCombinerInputParameterfvNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)getProcAddress("glGetFinalCombinerInputParameterfvNV");
		glGetFinalCombinerInputParameterivNV = (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)getProcAddress("glGetFinalCombinerInputParameterivNV");

		// GL_MESA_resize_buffers
		glResizeBuffersMESA = (PFNGLRESIZEBUFFERSMESAPROC)getProcAddress("glResizeBuffersMESA");

		// GL_MESA_window_pos
		glWindowPos2dMESA = (PFNGLWINDOWPOS2DMESAPROC)getProcAddress("glWindowPos2dMESA");
		glWindowPos2dvMESA = (PFNGLWINDOWPOS2DVMESAPROC)getProcAddress("glWindowPos2dvMESA");
		glWindowPos2fMESA = (PFNGLWINDOWPOS2FMESAPROC)getProcAddress("glWindowPos2fMESA");
		glWindowPos2fvMESA = (PFNGLWINDOWPOS2FVMESAPROC)getProcAddress("glWindowPos2fvMESA");
		glWindowPos2iMESA = (PFNGLWINDOWPOS2IMESAPROC)getProcAddress("glWindowPos2iMESA");
		glWindowPos2ivMESA = (PFNGLWINDOWPOS2IVMESAPROC)getProcAddress("glWindowPos2ivMESA");
		glWindowPos2sMESA = (PFNGLWINDOWPOS2SMESAPROC)getProcAddress("glWindowPos2sMESA");
		glWindowPos2svMESA = (PFNGLWINDOWPOS2SVMESAPROC)getProcAddress("glWindowPos2svMESA");
		glWindowPos3dMESA = (PFNGLWINDOWPOS3DMESAPROC)getProcAddress("glWindowPos3dMESA");
		glWindowPos3dvMESA = (PFNGLWINDOWPOS3DVMESAPROC)getProcAddress("glWindowPos3dvMESA");
		glWindowPos3fMESA = (PFNGLWINDOWPOS3FMESAPROC)getProcAddress("glWindowPos3fMESA");
		glWindowPos3fvMESA = (PFNGLWINDOWPOS3FVMESAPROC)getProcAddress("glWindowPos3fvMESA");
		glWindowPos3iMESA = (PFNGLWINDOWPOS3IMESAPROC)getProcAddress("glWindowPos3iMESA");
		glWindowPos3ivMESA = (PFNGLWINDOWPOS3IVMESAPROC)getProcAddress("glWindowPos3ivMESA");
		glWindowPos3sMESA = (PFNGLWINDOWPOS3SMESAPROC)getProcAddress("glWindowPos3sMESA");
		glWindowPos3svMESA = (PFNGLWINDOWPOS3SVMESAPROC)getProcAddress("glWindowPos3svMESA");
		glWindowPos4dMESA = (PFNGLWINDOWPOS4DMESAPROC)getProcAddress("glWindowPos4dMESA");
		glWindowPos4dvMESA = (PFNGLWINDOWPOS4DVMESAPROC)getProcAddress("glWindowPos4dvMESA");
		glWindowPos4fMESA = (PFNGLWINDOWPOS4FMESAPROC)getProcAddress("glWindowPos4fMESA");
		glWindowPos4fvMESA = (PFNGLWINDOWPOS4FVMESAPROC)getProcAddress("glWindowPos4fvMESA");
		glWindowPos4iMESA = (PFNGLWINDOWPOS4IMESAPROC)getProcAddress("glWindowPos4iMESA");
		glWindowPos4ivMESA = (PFNGLWINDOWPOS4IVMESAPROC)getProcAddress("glWindowPos4ivMESA");
		glWindowPos4sMESA = (PFNGLWINDOWPOS4SMESAPROC)getProcAddress("glWindowPos4sMESA");
		glWindowPos4svMESA = (PFNGLWINDOWPOS4SVMESAPROC)getProcAddress("glWindowPos4svMESA");

		// GL_IBM_multimode_draw_arrays
		glMultiModeDrawArraysIBM = (PFNGLMULTIMODEDRAWARRAYSIBMPROC)getProcAddress("glMultiModeDrawArraysIBM");
		glMultiModeDrawElementsIBM = (PFNGLMULTIMODEDRAWELEMENTSIBMPROC)getProcAddress("glMultiModeDrawElementsIBM");

		// GL_IBM_vertex_array_lists
		glColorPointerListIBM = (PFNGLCOLORPOINTERLISTIBMPROC)getProcAddress("glColorPointerListIBM");
		glSecondaryColorPointerListIBM = (PFNGLSECONDARYCOLORPOINTERLISTIBMPROC)getProcAddress("glSecondaryColorPointerListIBM");
		glEdgeFlagPointerListIBM = (PFNGLEDGEFLAGPOINTERLISTIBMPROC)getProcAddress("glEdgeFlagPointerListIBM");
		glFogCoordPointerListIBM = (PFNGLFOGCOORDPOINTERLISTIBMPROC)getProcAddress("glFogCoordPointerListIBM");
		glIndexPointerListIBM = (PFNGLINDEXPOINTERLISTIBMPROC)getProcAddress("glIndexPointerListIBM");
		glNormalPointerListIBM = (PFNGLNORMALPOINTERLISTIBMPROC)getProcAddress("glNormalPointerListIBM");
		glTexCoordPointerListIBM = (PFNGLTEXCOORDPOINTERLISTIBMPROC)getProcAddress("glTexCoordPointerListIBM");
		glVertexPointerListIBM = (PFNGLVERTEXPOINTERLISTIBMPROC)getProcAddress("glVertexPointerListIBM");

		// GL_3DFX_tbuffer
		glTbufferMask3DFX = (PFNGLTBUFFERMASK3DFXPROC)getProcAddress("glTbufferMask3DFX");

		// GL_EXT_multisample
		glSampleMaskEXT = (PFNGLSAMPLEMASKEXTPROC)getProcAddress("glSampleMaskEXT");
		glSamplePatternEXT = (PFNGLSAMPLEPATTERNEXTPROC)getProcAddress("glSamplePatternEXT");

		// GL_SGIS_texture_color_mask
		glTextureColorMaskSGIS = (PFNGLTEXTURECOLORMASKSGISPROC)getProcAddress("glTextureColorMaskSGIS");

		// GL_SGIX_igloo_interface
		glIglooInterfaceSGIX = (PFNGLIGLOOINTERFACESGIXPROC)getProcAddress("glIglooInterfaceSGIX");

		// GL_NV_fence
		glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)getProcAddress("glDeleteFencesNV");
		glGenFencesNV = (PFNGLGENFENCESNVPROC)getProcAddress("glGenFencesNV");
		glIsFenceNV = (PFNGLISFENCENVPROC)getProcAddress("glIsFenceNV");
		glTestFenceNV = (PFNGLTESTFENCENVPROC)getProcAddress("glTestFenceNV");
		glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)getProcAddress("glGetFenceivNV");
		glFinishFenceNV = (PFNGLFINISHFENCENVPROC)getProcAddress("glFinishFenceNV");
		glSetFenceNV = (PFNGLSETFENCENVPROC)getProcAddress("glSetFenceNV");

		// GL_NV_evaluators
		glMapControlPointsNV = (PFNGLMAPCONTROLPOINTSNVPROC)getProcAddress("glMapControlPointsNV");
		glMapParameterivNV = (PFNGLMAPPARAMETERIVNVPROC)getProcAddress("glMapParameterivNV");
		glMapParameterfvNV = (PFNGLMAPPARAMETERFVNVPROC)getProcAddress("glMapParameterfvNV");
		glGetMapControlPointsNV = (PFNGLGETMAPCONTROLPOINTSNVPROC)getProcAddress("glGetMapControlPointsNV");
		glGetMapParameterivNV = (PFNGLGETMAPPARAMETERIVNVPROC)getProcAddress("glGetMapParameterivNV");
		glGetMapParameterfvNV = (PFNGLGETMAPPARAMETERFVNVPROC)getProcAddress("glGetMapParameterfvNV");
		glGetMapAttribParameterivNV = (PFNGLGETMAPATTRIBPARAMETERIVNVPROC)getProcAddress("glGetMapAttribParameterivNV");
		glGetMapAttribParameterfvNV = (PFNGLGETMAPATTRIBPARAMETERFVNVPROC)getProcAddress("glGetMapAttribParameterfvNV");
		glEvalMapsNV = (PFNGLEVALMAPSNVPROC)getProcAddress("glEvalMapsNV");

		// GL_NV_register_combiners2
		glCombinerStageParameterfvNV = (PFNGLCOMBINERSTAGEPARAMETERFVNVPROC)getProcAddress("glCombinerStageParameterfvNV");
		glGetCombinerStageParameterfvNV = (PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC)getProcAddress("glGetCombinerStageParameterfvNV");

		// GL_NV_vertex_program
		glAreProgramsResidentNV = (PFNGLAREPROGRAMSRESIDENTNVPROC)getProcAddress("glAreProgramsResidentNV");
		glBindProgramNV = (PFNGLBINDPROGRAMNVPROC)getProcAddress("glBindProgramNV");
		glDeleteProgramsNV = (PFNGLDELETEPROGRAMSNVPROC)getProcAddress("glDeleteProgramsNV");
		glExecuteProgramNV = (PFNGLEXECUTEPROGRAMNVPROC)getProcAddress("glExecuteProgramNV");
		glGenProgramsNV = (PFNGLGENPROGRAMSNVPROC)getProcAddress("glGenProgramsNV");
		glGetProgramParameterdvNV = (PFNGLGETPROGRAMPARAMETERDVNVPROC)getProcAddress("glGetProgramParameterdvNV");
		glGetProgramParameterfvNV = (PFNGLGETPROGRAMPARAMETERFVNVPROC)getProcAddress("glGetProgramParameterfvNV");
		glGetProgramivNV = (PFNGLGETPROGRAMIVNVPROC)getProcAddress("glGetProgramivNV");
		glGetProgramStringNV = (PFNGLGETPROGRAMSTRINGNVPROC)getProcAddress("glGetProgramStringNV");
		glGetTrackMatrixivNV = (PFNGLGETTRACKMATRIXIVNVPROC)getProcAddress("glGetTrackMatrixivNV");
		glGetVertexAttribdvNV = (PFNGLGETVERTEXATTRIBDVNVPROC)getProcAddress("glGetVertexAttribdvNV");
		glGetVertexAttribfvNV = (PFNGLGETVERTEXATTRIBFVNVPROC)getProcAddress("glGetVertexAttribfvNV");
		glGetVertexAttribivNV = (PFNGLGETVERTEXATTRIBIVNVPROC)getProcAddress("glGetVertexAttribivNV");
		glGetVertexAttribPointervNV = (PFNGLGETVERTEXATTRIBPOINTERVNVPROC)getProcAddress("glGetVertexAttribPointervNV");
		glIsProgramNV = (PFNGLISPROGRAMNVPROC)getProcAddress("glIsProgramNV");
		glLoadProgramNV = (PFNGLLOADPROGRAMNVPROC)getProcAddress("glLoadProgramNV");
		glProgramParameter4dNV = (PFNGLPROGRAMPARAMETER4DNVPROC)getProcAddress("glProgramParameter4dNV");
		glProgramParameter4dvNV = (PFNGLPROGRAMPARAMETER4DVNVPROC)getProcAddress("glProgramParameter4dvNV");
		glProgramParameter4fNV = (PFNGLPROGRAMPARAMETER4FNVPROC)getProcAddress("glProgramParameter4fNV");
		glProgramParameter4fvNV = (PFNGLPROGRAMPARAMETER4FVNVPROC)getProcAddress("glProgramParameter4fvNV");
		glProgramParameters4dvNV = (PFNGLPROGRAMPARAMETERS4DVNVPROC)getProcAddress("glProgramParameters4dvNV");
		glProgramParameters4fvNV = (PFNGLPROGRAMPARAMETERS4FVNVPROC)getProcAddress("glProgramParameters4fvNV");
		glRequestResidentProgramsNV = (PFNGLREQUESTRESIDENTPROGRAMSNVPROC)getProcAddress("glRequestResidentProgramsNV");
		glTrackMatrixNV = (PFNGLTRACKMATRIXNVPROC)getProcAddress("glTrackMatrixNV");
		glVertexAttribPointerNV = (PFNGLVERTEXATTRIBPOINTERNVPROC)getProcAddress("glVertexAttribPointerNV");
		glVertexAttrib1dNV = (PFNGLVERTEXATTRIB1DNVPROC)getProcAddress("glVertexAttrib1dNV");
		glVertexAttrib1dvNV = (PFNGLVERTEXATTRIB1DVNVPROC)getProcAddress("glVertexAttrib1dvNV");
		glVertexAttrib1fNV = (PFNGLVERTEXATTRIB1FNVPROC)getProcAddress("glVertexAttrib1fNV");
		glVertexAttrib1fvNV = (PFNGLVERTEXATTRIB1FVNVPROC)getProcAddress("glVertexAttrib1fvNV");
		glVertexAttrib1sNV = (PFNGLVERTEXATTRIB1SNVPROC)getProcAddress("glVertexAttrib1sNV");
		glVertexAttrib1svNV = (PFNGLVERTEXATTRIB1SVNVPROC)getProcAddress("glVertexAttrib1svNV");
		glVertexAttrib2dNV = (PFNGLVERTEXATTRIB2DNVPROC)getProcAddress("glVertexAttrib2dNV");
		glVertexAttrib2dvNV = (PFNGLVERTEXATTRIB2DVNVPROC)getProcAddress("glVertexAttrib2dvNV");
		glVertexAttrib2fNV = (PFNGLVERTEXATTRIB2FNVPROC)getProcAddress("glVertexAttrib2fNV");
		glVertexAttrib2fvNV = (PFNGLVERTEXATTRIB2FVNVPROC)getProcAddress("glVertexAttrib2fvNV");
		glVertexAttrib2sNV = (PFNGLVERTEXATTRIB2SNVPROC)getProcAddress("glVertexAttrib2sNV");
		glVertexAttrib2svNV = (PFNGLVERTEXATTRIB2SVNVPROC)getProcAddress("glVertexAttrib2svNV");
		glVertexAttrib3dNV = (PFNGLVERTEXATTRIB3DNVPROC)getProcAddress("glVertexAttrib3dNV");
		glVertexAttrib3dvNV = (PFNGLVERTEXATTRIB3DVNVPROC)getProcAddress("glVertexAttrib3dvNV");
		glVertexAttrib3fNV = (PFNGLVERTEXATTRIB3FNVPROC)getProcAddress("glVertexAttrib3fNV");
		glVertexAttrib3fvNV = (PFNGLVERTEXATTRIB3FVNVPROC)getProcAddress("glVertexAttrib3fvNV");
		glVertexAttrib3sNV = (PFNGLVERTEXATTRIB3SNVPROC)getProcAddress("glVertexAttrib3sNV");
		glVertexAttrib3svNV = (PFNGLVERTEXATTRIB3SVNVPROC)getProcAddress("glVertexAttrib3svNV");
		glVertexAttrib4dNV = (PFNGLVERTEXATTRIB4DNVPROC)getProcAddress("glVertexAttrib4dNV");
		glVertexAttrib4dvNV = (PFNGLVERTEXATTRIB4DVNVPROC)getProcAddress("glVertexAttrib4dvNV");
		glVertexAttrib4fNV = (PFNGLVERTEXATTRIB4FNVPROC)getProcAddress("glVertexAttrib4fNV");
		glVertexAttrib4fvNV = (PFNGLVERTEXATTRIB4FVNVPROC)getProcAddress("glVertexAttrib4fvNV");
		glVertexAttrib4sNV = (PFNGLVERTEXATTRIB4SNVPROC)getProcAddress("glVertexAttrib4sNV");
		glVertexAttrib4svNV = (PFNGLVERTEXATTRIB4SVNVPROC)getProcAddress("glVertexAttrib4svNV");
		glVertexAttrib4ubNV = (PFNGLVERTEXATTRIB4UBNVPROC)getProcAddress("glVertexAttrib4ubNV");
		glVertexAttrib4ubvNV = (PFNGLVERTEXATTRIB4UBVNVPROC)getProcAddress("glVertexAttrib4ubvNV");
		glVertexAttribs1dvNV = (PFNGLVERTEXATTRIBS1DVNVPROC)getProcAddress("glVertexAttribs1dvNV");
		glVertexAttribs1fvNV = (PFNGLVERTEXATTRIBS1FVNVPROC)getProcAddress("glVertexAttribs1fvNV");
		glVertexAttribs1svNV = (PFNGLVERTEXATTRIBS1SVNVPROC)getProcAddress("glVertexAttribs1svNV");
		glVertexAttribs2dvNV = (PFNGLVERTEXATTRIBS2DVNVPROC)getProcAddress("glVertexAttribs2dvNV");
		glVertexAttribs2fvNV = (PFNGLVERTEXATTRIBS2FVNVPROC)getProcAddress("glVertexAttribs2fvNV");
		glVertexAttribs2svNV = (PFNGLVERTEXATTRIBS2SVNVPROC)getProcAddress("glVertexAttribs2svNV");
		glVertexAttribs3dvNV = (PFNGLVERTEXATTRIBS3DVNVPROC)getProcAddress("glVertexAttribs3dvNV");
		glVertexAttribs3fvNV = (PFNGLVERTEXATTRIBS3FVNVPROC)getProcAddress("glVertexAttribs3fvNV");
		glVertexAttribs3svNV = (PFNGLVERTEXATTRIBS3SVNVPROC)getProcAddress("glVertexAttribs3svNV");
		glVertexAttribs4dvNV = (PFNGLVERTEXATTRIBS4DVNVPROC)getProcAddress("glVertexAttribs4dvNV");
		glVertexAttribs4fvNV = (PFNGLVERTEXATTRIBS4FVNVPROC)getProcAddress("glVertexAttribs4fvNV");
		glVertexAttribs4svNV = (PFNGLVERTEXATTRIBS4SVNVPROC)getProcAddress("glVertexAttribs4svNV");
		glVertexAttribs4ubvNV = (PFNGLVERTEXATTRIBS4UBVNVPROC)getProcAddress("glVertexAttribs4ubvNV");

		// GL_ATI_envmap_bumpmap
		glTexBumpParameterivATI = (PFNGLTEXBUMPPARAMETERIVATIPROC)getProcAddress("glTexBumpParameterivATI");
		glTexBumpParameterfvATI = (PFNGLTEXBUMPPARAMETERFVATIPROC)getProcAddress("glTexBumpParameterfvATI");
		glGetTexBumpParameterivATI = (PFNGLGETTEXBUMPPARAMETERIVATIPROC)getProcAddress("glGetTexBumpParameterivATI");
		glGetTexBumpParameterfvATI = (PFNGLGETTEXBUMPPARAMETERFVATIPROC)getProcAddress("glGetTexBumpParameterfvATI");

		// GL_ATI_fragment_shader
		glGenFragmentShadersATI = (PFNGLGENFRAGMENTSHADERSATIPROC)getProcAddress("glGenFragmentShadersATI");
		glBindFragmentShaderATI = (PFNGLBINDFRAGMENTSHADERATIPROC)getProcAddress("glBindFragmentShaderATI");
		glDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC)getProcAddress("glDeleteFragmentShaderATI");
		glBeginFragmentShaderATI = (PFNGLBEGINFRAGMENTSHADERATIPROC)getProcAddress("glBeginFragmentShaderATI");
		glEndFragmentShaderATI = (PFNGLENDFRAGMENTSHADERATIPROC)getProcAddress("glEndFragmentShaderATI");
		glPassTexCoordATI = (PFNGLPASSTEXCOORDATIPROC)getProcAddress("glPassTexCoordATI");
		glSampleMapATI = (PFNGLSAMPLEMAPATIPROC)getProcAddress("glSampleMapATI");
		glColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC)getProcAddress("glColorFragmentOp1ATI");
		glColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC)getProcAddress("glColorFragmentOp2ATI");
		glColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC)getProcAddress("glColorFragmentOp3ATI");
		glAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC)getProcAddress("glAlphaFragmentOp1ATI");
		glAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC)getProcAddress("glAlphaFragmentOp2ATI");
		glAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC)getProcAddress("glAlphaFragmentOp3ATI");
		glSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)getProcAddress("glSetFragmentShaderConstantATI");

		// GL_ATI_pn_triangles
		glPNTrianglesiATI = (PFNGLPNTRIANGLESIATIPROC)getProcAddress("glPNTrianglesiATI");
		glPNTrianglesfATI = (PFNGLPNTRIANGLESFATIPROC)getProcAddress("glPNTrianglesfATI");

		// GL_ATI_vertex_array_object
		glNewObjectBufferATI = (PFNGLNEWOBJECTBUFFERATIPROC)getProcAddress("glNewObjectBufferATI");
		glIsObjectBufferATI = (PFNGLISOBJECTBUFFERATIPROC)getProcAddress("glIsObjectBufferATI");
		glUpdateObjectBufferATI = (PFNGLUPDATEOBJECTBUFFERATIPROC)getProcAddress("glUpdateObjectBufferATI");
		glGetObjectBufferfvATI = (PFNGLGETOBJECTBUFFERFVATIPROC)getProcAddress("glGetObjectBufferfvATI");
		glGetObjectBufferivATI = (PFNGLGETOBJECTBUFFERIVATIPROC)getProcAddress("glGetObjectBufferivATI");
		glFreeObjectBufferATI = (PFNGLFREEOBJECTBUFFERATIPROC)getProcAddress("glFreeObjectBufferATI");
		glArrayObjectATI = (PFNGLARRAYOBJECTATIPROC)getProcAddress("glArrayObjectATI");
		glGetArrayObjectfvATI = (PFNGLGETARRAYOBJECTFVATIPROC)getProcAddress("glGetArrayObjectfvATI");
		glGetArrayObjectivATI = (PFNGLGETARRAYOBJECTIVATIPROC)getProcAddress("glGetArrayObjectivATI");
		glVariantArrayObjectATI = (PFNGLVARIANTARRAYOBJECTATIPROC)getProcAddress("glVariantArrayObjectATI");
		glGetVariantArrayObjectfvATI = (PFNGLGETVARIANTARRAYOBJECTFVATIPROC)getProcAddress("glGetVariantArrayObjectfvATI");
		glGetVariantArrayObjectivATI = (PFNGLGETVARIANTARRAYOBJECTIVATIPROC)getProcAddress("glGetVariantArrayObjectivATI");

		// GL_EXT_vertex_shader
		glBeginVertexShaderEXT = (PFNGLBEGINVERTEXSHADEREXTPROC)getProcAddress("glBeginVertexShaderEXT");
		glEndVertexShaderEXT = (PFNGLENDVERTEXSHADEREXTPROC)getProcAddress("glEndVertexShaderEXT");
		glBindVertexShaderEXT = (PFNGLBINDVERTEXSHADEREXTPROC)getProcAddress("glBindVertexShaderEXT");
		glGenVertexShadersEXT = (PFNGLGENVERTEXSHADERSEXTPROC)getProcAddress("glGenVertexShadersEXT");
		glDeleteVertexShaderEXT = (PFNGLDELETEVERTEXSHADEREXTPROC)getProcAddress("glDeleteVertexShaderEXT");
		glShaderOp1EXT = (PFNGLSHADEROP1EXTPROC)getProcAddress("glShaderOp1EXT");
		glShaderOp2EXT = (PFNGLSHADEROP2EXTPROC)getProcAddress("glShaderOp2EXT");
		glShaderOp3EXT = (PFNGLSHADEROP3EXTPROC)getProcAddress("glShaderOp3EXT");
		glSwizzleEXT = (PFNGLSWIZZLEEXTPROC)getProcAddress("glSwizzleEXT");
		glWriteMaskEXT = (PFNGLWRITEMASKEXTPROC)getProcAddress("glWriteMaskEXT");
		glInsertComponentEXT = (PFNGLINSERTCOMPONENTEXTPROC)getProcAddress("glInsertComponentEXT");
		glExtractComponentEXT = (PFNGLEXTRACTCOMPONENTEXTPROC)getProcAddress("glExtractComponentEXT");
		glGenSymbolsEXT = (PFNGLGENSYMBOLSEXTPROC)getProcAddress("glGenSymbolsEXT");
		glSetInvariantEXT = (PFNGLSETINVARIANTEXTPROC)getProcAddress("glSetInvariantEXT");
		glSetLocalConstantEXT = (PFNGLSETLOCALCONSTANTEXTPROC)getProcAddress("glSetLocalConstantEXT");
		glVariantbvEXT = (PFNGLVARIANTBVEXTPROC)getProcAddress("glVariantbvEXT");
		glVariantsvEXT = (PFNGLVARIANTSVEXTPROC)getProcAddress("glVariantsvEXT");
		glVariantivEXT = (PFNGLVARIANTIVEXTPROC)getProcAddress("glVariantivEXT");
		glVariantfvEXT = (PFNGLVARIANTFVEXTPROC)getProcAddress("glVariantfvEXT");
		glVariantdvEXT = (PFNGLVARIANTDVEXTPROC)getProcAddress("glVariantdvEXT");
		glVariantubvEXT = (PFNGLVARIANTUBVEXTPROC)getProcAddress("glVariantubvEXT");
		glVariantusvEXT = (PFNGLVARIANTUSVEXTPROC)getProcAddress("glVariantusvEXT");
		glVariantuivEXT = (PFNGLVARIANTUIVEXTPROC)getProcAddress("glVariantuivEXT");
		glVariantPointerEXT = (PFNGLVARIANTPOINTEREXTPROC)getProcAddress("glVariantPointerEXT");
		glEnableVariantClientStateEXT = (PFNGLENABLEVARIANTCLIENTSTATEEXTPROC)getProcAddress("glEnableVariantClientStateEXT");
		glDisableVariantClientStateEXT = (PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC)getProcAddress("glDisableVariantClientStateEXT");
		glBindLightParameterEXT = (PFNGLBINDLIGHTPARAMETEREXTPROC)getProcAddress("glBindLightParameterEXT");
		glBindMaterialParameterEXT = (PFNGLBINDMATERIALPARAMETEREXTPROC)getProcAddress("glBindMaterialParameterEXT");
		glBindTexGenParameterEXT = (PFNGLBINDTEXGENPARAMETEREXTPROC)getProcAddress("glBindTexGenParameterEXT");
		glBindTextureUnitParameterEXT = (PFNGLBINDTEXTUREUNITPARAMETEREXTPROC)getProcAddress("glBindTextureUnitParameterEXT");
		glBindParameterEXT = (PFNGLBINDPARAMETEREXTPROC)getProcAddress("glBindParameterEXT");
		glIsVariantEnabledEXT = (PFNGLISVARIANTENABLEDEXTPROC)getProcAddress("glIsVariantEnabledEXT");
		glGetVariantBooleanvEXT = (PFNGLGETVARIANTBOOLEANVEXTPROC)getProcAddress("glGetVariantBooleanvEXT");
		glGetVariantIntegervEXT = (PFNGLGETVARIANTINTEGERVEXTPROC)getProcAddress("glGetVariantIntegervEXT");
		glGetVariantFloatvEXT = (PFNGLGETVARIANTFLOATVEXTPROC)getProcAddress("glGetVariantFloatvEXT");
		glGetVariantPointervEXT = (PFNGLGETVARIANTPOINTERVEXTPROC)getProcAddress("glGetVariantPointervEXT");
		glGetInvariantBooleanvEXT = (PFNGLGETINVARIANTBOOLEANVEXTPROC)getProcAddress("glGetInvariantBooleanvEXT");
		glGetInvariantIntegervEXT = (PFNGLGETINVARIANTINTEGERVEXTPROC)getProcAddress("glGetInvariantIntegervEXT");
		glGetInvariantFloatvEXT = (PFNGLGETINVARIANTFLOATVEXTPROC)getProcAddress("glGetInvariantFloatvEXT");
		glGetLocalConstantBooleanvEXT = (PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC)getProcAddress("glGetLocalConstantBooleanvEXT");
		glGetLocalConstantIntegervEXT = (PFNGLGETLOCALCONSTANTINTEGERVEXTPROC)getProcAddress("glGetLocalConstantIntegervEXT");
		glGetLocalConstantFloatvEXT = (PFNGLGETLOCALCONSTANTFLOATVEXTPROC)getProcAddress("glGetLocalConstantFloatvEXT");

		// GL_ATI_vertex_streams
		glVertexStream1sATI = (PFNGLVERTEXSTREAM1SATIPROC)getProcAddress("glVertexStream1sATI");
		glVertexStream1svATI = (PFNGLVERTEXSTREAM1SVATIPROC)getProcAddress("glVertexStream1svATI");
		glVertexStream1iATI = (PFNGLVERTEXSTREAM1IATIPROC)getProcAddress("glVertexStream1iATI");
		glVertexStream1ivATI = (PFNGLVERTEXSTREAM1IVATIPROC)getProcAddress("glVertexStream1ivATI");
		glVertexStream1fATI = (PFNGLVERTEXSTREAM1FATIPROC)getProcAddress("glVertexStream1fATI");
		glVertexStream1fvATI = (PFNGLVERTEXSTREAM1FVATIPROC)getProcAddress("glVertexStream1fvATI");
		glVertexStream1dATI = (PFNGLVERTEXSTREAM1DATIPROC)getProcAddress("glVertexStream1dATI");
		glVertexStream1dvATI = (PFNGLVERTEXSTREAM1DVATIPROC)getProcAddress("glVertexStream1dvATI");
		glVertexStream2sATI = (PFNGLVERTEXSTREAM2SATIPROC)getProcAddress("glVertexStream2sATI");
		glVertexStream2svATI = (PFNGLVERTEXSTREAM2SVATIPROC)getProcAddress("glVertexStream2svATI");
		glVertexStream2iATI = (PFNGLVERTEXSTREAM2IATIPROC)getProcAddress("glVertexStream2iATI");
		glVertexStream2ivATI = (PFNGLVERTEXSTREAM2IVATIPROC)getProcAddress("glVertexStream2ivATI");
		glVertexStream2fATI = (PFNGLVERTEXSTREAM2FATIPROC)getProcAddress("glVertexStream2fATI");
		glVertexStream2fvATI = (PFNGLVERTEXSTREAM2FVATIPROC)getProcAddress("glVertexStream2fvATI");
		glVertexStream2dATI = (PFNGLVERTEXSTREAM2DATIPROC)getProcAddress("glVertexStream2dATI");
		glVertexStream2dvATI = (PFNGLVERTEXSTREAM2DVATIPROC)getProcAddress("glVertexStream2dvATI");
		glVertexStream3sATI = (PFNGLVERTEXSTREAM3SATIPROC)getProcAddress("glVertexStream3sATI");
		glVertexStream3svATI = (PFNGLVERTEXSTREAM3SVATIPROC)getProcAddress("glVertexStream3svATI");
		glVertexStream3iATI = (PFNGLVERTEXSTREAM3IATIPROC)getProcAddress("glVertexStream3iATI");
		glVertexStream3ivATI = (PFNGLVERTEXSTREAM3IVATIPROC)getProcAddress("glVertexStream3ivATI");
		glVertexStream3fATI = (PFNGLVERTEXSTREAM3FATIPROC)getProcAddress("glVertexStream3fATI");
		glVertexStream3fvATI = (PFNGLVERTEXSTREAM3FVATIPROC)getProcAddress("glVertexStream3fvATI");
		glVertexStream3dATI = (PFNGLVERTEXSTREAM3DATIPROC)getProcAddress("glVertexStream3dATI");
		glVertexStream3dvATI = (PFNGLVERTEXSTREAM3DVATIPROC)getProcAddress("glVertexStream3dvATI");
		glVertexStream4sATI = (PFNGLVERTEXSTREAM4SATIPROC)getProcAddress("glVertexStream4sATI");
		glVertexStream4svATI = (PFNGLVERTEXSTREAM4SVATIPROC)getProcAddress("glVertexStream4svATI");
		glVertexStream4iATI = (PFNGLVERTEXSTREAM4IATIPROC)getProcAddress("glVertexStream4iATI");
		glVertexStream4ivATI = (PFNGLVERTEXSTREAM4IVATIPROC)getProcAddress("glVertexStream4ivATI");
		glVertexStream4fATI = (PFNGLVERTEXSTREAM4FATIPROC)getProcAddress("glVertexStream4fATI");
		glVertexStream4fvATI = (PFNGLVERTEXSTREAM4FVATIPROC)getProcAddress("glVertexStream4fvATI");
		glVertexStream4dATI = (PFNGLVERTEXSTREAM4DATIPROC)getProcAddress("glVertexStream4dATI");
		glVertexStream4dvATI = (PFNGLVERTEXSTREAM4DVATIPROC)getProcAddress("glVertexStream4dvATI");
		glNormalStream3bATI = (PFNGLNORMALSTREAM3BATIPROC)getProcAddress("glNormalStream3bATI");
		glNormalStream3bvATI = (PFNGLNORMALSTREAM3BVATIPROC)getProcAddress("glNormalStream3bvATI");
		glNormalStream3sATI = (PFNGLNORMALSTREAM3SATIPROC)getProcAddress("glNormalStream3sATI");
		glNormalStream3svATI = (PFNGLNORMALSTREAM3SVATIPROC)getProcAddress("glNormalStream3svATI");
		glNormalStream3iATI = (PFNGLNORMALSTREAM3IATIPROC)getProcAddress("glNormalStream3iATI");
		glNormalStream3ivATI = (PFNGLNORMALSTREAM3IVATIPROC)getProcAddress("glNormalStream3ivATI");
		glNormalStream3fATI = (PFNGLNORMALSTREAM3FATIPROC)getProcAddress("glNormalStream3fATI");
		glNormalStream3fvATI = (PFNGLNORMALSTREAM3FVATIPROC)getProcAddress("glNormalStream3fvATI");
		glNormalStream3dATI = (PFNGLNORMALSTREAM3DATIPROC)getProcAddress("glNormalStream3dATI");
		glNormalStream3dvATI = (PFNGLNORMALSTREAM3DVATIPROC)getProcAddress("glNormalStream3dvATI");
		glClientActiveVertexStreamATI = (PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC)getProcAddress("glClientActiveVertexStreamATI");
		glVertexBlendEnviATI = (PFNGLVERTEXBLENDENVIATIPROC)getProcAddress("glVertexBlendEnviATI");
		glVertexBlendEnvfATI = (PFNGLVERTEXBLENDENVFATIPROC)getProcAddress("glVertexBlendEnvfATI");

		// GL_ATI_element_array
		glElementPointerATI = (PFNGLELEMENTPOINTERATIPROC)getProcAddress("glElementPointerATI");
		glDrawElementArrayATI = (PFNGLDRAWELEMENTARRAYATIPROC)getProcAddress("glDrawElementArrayATI");
		glDrawRangeElementArrayATI = (PFNGLDRAWRANGEELEMENTARRAYATIPROC)getProcAddress("glDrawRangeElementArrayATI");

		// GL_SUN_mesh_array
		glDrawMeshArraysSUN = (PFNGLDRAWMESHARRAYSSUNPROC)getProcAddress("glDrawMeshArraysSUN");

		// GL_NV_occlusion_query
		glGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC)getProcAddress("glGenOcclusionQueriesNV");
		glDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC)getProcAddress("glDeleteOcclusionQueriesNV");
		glIsOcclusionQueryNV = (PFNGLISOCCLUSIONQUERYNVPROC)getProcAddress("glIsOcclusionQueryNV");
		glBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC)getProcAddress("glBeginOcclusionQueryNV");
		glEndOcclusionQueryNV = (PFNGLENDOCCLUSIONQUERYNVPROC)getProcAddress("glEndOcclusionQueryNV");
		glGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC)getProcAddress("glGetOcclusionQueryivNV");
		glGetOcclusionQueryuivNV = (PFNGLGETOCCLUSIONQUERYUIVNVPROC)getProcAddress("glGetOcclusionQueryuivNV");

		// GL_NV_point_sprite
		glPointParameteriNV = (PFNGLPOINTPARAMETERINVPROC)getProcAddress("glPointParameteriNV");
		glPointParameterivNV = (PFNGLPOINTPARAMETERIVNVPROC)getProcAddress("glPointParameterivNV");

		// GL_EXT_stencil_two_side
		glActiveStencilFaceEXT = (PFNGLACTIVESTENCILFACEEXTPROC)getProcAddress("glActiveStencilFaceEXT");

		// GL_APPLE_element_array
		glElementPointerAPPLE = (PFNGLELEMENTPOINTERAPPLEPROC)getProcAddress("glElementPointerAPPLE");
		glDrawElementArrayAPPLE = (PFNGLDRAWELEMENTARRAYAPPLEPROC)getProcAddress("glDrawElementArrayAPPLE");
		glDrawRangeElementArrayAPPLE = (PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC)getProcAddress("glDrawRangeElementArrayAPPLE");
		glMultiDrawElementArrayAPPLE = (PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC)getProcAddress("glMultiDrawElementArrayAPPLE");
		glMultiDrawRangeElementArrayAPPLE = (PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC)getProcAddress("glMultiDrawRangeElementArrayAPPLE");

		// GL_APPLE_fence
		glGenFencesAPPLE = (PFNGLGENFENCESAPPLEPROC)getProcAddress("glGenFencesAPPLE");
		glDeleteFencesAPPLE = (PFNGLDELETEFENCESAPPLEPROC)getProcAddress("glDeleteFencesAPPLE");
		glSetFenceAPPLE = (PFNGLSETFENCEAPPLEPROC)getProcAddress("glSetFenceAPPLE");
		glIsFenceAPPLE = (PFNGLISFENCEAPPLEPROC)getProcAddress("glIsFenceAPPLE");
		glTestFenceAPPLE = (PFNGLTESTFENCEAPPLEPROC)getProcAddress("glTestFenceAPPLE");
		glFinishFenceAPPLE = (PFNGLFINISHFENCEAPPLEPROC)getProcAddress("glFinishFenceAPPLE");
		glTestObjectAPPLE = (PFNGLTESTOBJECTAPPLEPROC)getProcAddress("glTestObjectAPPLE");
		glFinishObjectAPPLE = (PFNGLFINISHOBJECTAPPLEPROC)getProcAddress("glFinishObjectAPPLE");

		// GL_APPLE_vertex_array_object
		glBindVertexArrayAPPLE = (PFNGLBINDVERTEXARRAYAPPLEPROC)getProcAddress("glBindVertexArrayAPPLE");
		glDeleteVertexArraysAPPLE = (PFNGLDELETEVERTEXARRAYSAPPLEPROC)getProcAddress("glDeleteVertexArraysAPPLE");
		glGenVertexArraysAPPLE = (PFNGLGENVERTEXARRAYSAPPLEPROC)getProcAddress("glGenVertexArraysAPPLE");
		glIsVertexArrayAPPLE = (PFNGLISVERTEXARRAYAPPLEPROC)getProcAddress("glIsVertexArrayAPPLE");

		// GL_APPLE_vertex_array_range
		glVertexArrayRangeAPPLE = (PFNGLVERTEXARRAYRANGEAPPLEPROC)getProcAddress("glVertexArrayRangeAPPLE");
		glFlushVertexArrayRangeAPPLE = (PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC)getProcAddress("glFlushVertexArrayRangeAPPLE");
		glVertexArrayParameteriAPPLE = (PFNGLVERTEXARRAYPARAMETERIAPPLEPROC)getProcAddress("glVertexArrayParameteriAPPLE");

		// GL_ATI_draw_buffers
		glDrawBuffersATI = (PFNGLDRAWBUFFERSATIPROC)getProcAddress("glDrawBuffersATI");

		// GL_NV_fragment_program
		/* Some NV_fragment_program entry points are shared with ARB_vertex_program. */
		glProgramNamedParameter4fNV = (PFNGLPROGRAMNAMEDPARAMETER4FNVPROC)getProcAddress("glProgramNamedParameter4fNV");
		glProgramNamedParameter4dNV = (PFNGLPROGRAMNAMEDPARAMETER4DNVPROC)getProcAddress("glProgramNamedParameter4dNV");
		glProgramNamedParameter4fvNV = (PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC)getProcAddress("glProgramNamedParameter4fvNV");
		glProgramNamedParameter4dvNV = (PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC)getProcAddress("glProgramNamedParameter4dvNV");
		glGetProgramNamedParameterfvNV = (PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC)getProcAddress("glGetProgramNamedParameterfvNV");
		glGetProgramNamedParameterdvNV = (PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC)getProcAddress("glGetProgramNamedParameterdvNV");

		// GL_NV_half_float
		glVertex2hNV = (PFNGLVERTEX2HNVPROC)getProcAddress("glVertex2hNV");
		glVertex2hvNV = (PFNGLVERTEX2HVNVPROC)getProcAddress("glVertex2hvNV");
		glVertex3hNV = (PFNGLVERTEX3HNVPROC)getProcAddress("glVertex3hNV");
		glVertex3hvNV = (PFNGLVERTEX3HVNVPROC)getProcAddress("glVertex3hvNV");
		glVertex4hNV = (PFNGLVERTEX4HNVPROC)getProcAddress("glVertex4hNV");
		glVertex4hvNV = (PFNGLVERTEX4HVNVPROC)getProcAddress("glVertex4hvNV");
		glNormal3hNV = (PFNGLNORMAL3HNVPROC)getProcAddress("glNormal3hNV");
		glNormal3hvNV = (PFNGLNORMAL3HVNVPROC)getProcAddress("glNormal3hvNV");
		glColor3hNV = (PFNGLCOLOR3HNVPROC)getProcAddress("glColor3hNV");
		glColor3hvNV = (PFNGLCOLOR3HVNVPROC)getProcAddress("glColor3hvNV");
		glColor4hNV = (PFNGLCOLOR4HNVPROC)getProcAddress("glColor4hNV");
		glColor4hvNV = (PFNGLCOLOR4HVNVPROC)getProcAddress("glColor4hvNV");
		glTexCoord1hNV = (PFNGLTEXCOORD1HNVPROC)getProcAddress("glTexCoord1hNV");
		glTexCoord1hvNV = (PFNGLTEXCOORD1HVNVPROC)getProcAddress("glTexCoord1hvNV");
		glTexCoord2hNV = (PFNGLTEXCOORD2HNVPROC)getProcAddress("glTexCoord2hNV");
		glTexCoord2hvNV = (PFNGLTEXCOORD2HVNVPROC)getProcAddress("glTexCoord2hvNV");
		glTexCoord3hNV = (PFNGLTEXCOORD3HNVPROC)getProcAddress("glTexCoord3hNV");
		glTexCoord3hvNV = (PFNGLTEXCOORD3HVNVPROC)getProcAddress("glTexCoord3hvNV");
		glTexCoord4hNV = (PFNGLTEXCOORD4HNVPROC)getProcAddress("glTexCoord4hNV");
		glTexCoord4hvNV = (PFNGLTEXCOORD4HVNVPROC)getProcAddress("glTexCoord4hvNV");
		glMultiTexCoord1hNV = (PFNGLMULTITEXCOORD1HNVPROC)getProcAddress("glMultiTexCoord1hNV");
		glMultiTexCoord1hvNV = (PFNGLMULTITEXCOORD1HVNVPROC)getProcAddress("glMultiTexCoord1hvNV");
		glMultiTexCoord2hNV = (PFNGLMULTITEXCOORD2HNVPROC)getProcAddress("glMultiTexCoord2hNV");
		glMultiTexCoord2hvNV = (PFNGLMULTITEXCOORD2HVNVPROC)getProcAddress("glMultiTexCoord2hvNV");
		glMultiTexCoord3hNV = (PFNGLMULTITEXCOORD3HNVPROC)getProcAddress("glMultiTexCoord3hNV");
		glMultiTexCoord3hvNV = (PFNGLMULTITEXCOORD3HVNVPROC)getProcAddress("glMultiTexCoord3hvNV");
		glMultiTexCoord4hNV = (PFNGLMULTITEXCOORD4HNVPROC)getProcAddress("glMultiTexCoord4hNV");
		glMultiTexCoord4hvNV = (PFNGLMULTITEXCOORD4HVNVPROC)getProcAddress("glMultiTexCoord4hvNV");
		glFogCoordhNV = (PFNGLFOGCOORDHNVPROC)getProcAddress("glFogCoordhNV");
		glFogCoordhvNV = (PFNGLFOGCOORDHVNVPROC)getProcAddress("glFogCoordhvNV");
		glSecondaryColor3hNV = (PFNGLSECONDARYCOLOR3HNVPROC)getProcAddress("glSecondaryColor3hNV");
		glSecondaryColor3hvNV = (PFNGLSECONDARYCOLOR3HVNVPROC)getProcAddress("glSecondaryColor3hvNV");
		glVertexWeighthNV = (PFNGLVERTEXWEIGHTHNVPROC)getProcAddress("glVertexWeighthNV");
		glVertexWeighthvNV = (PFNGLVERTEXWEIGHTHVNVPROC)getProcAddress("glVertexWeighthvNV");
		glVertexAttrib1hNV = (PFNGLVERTEXATTRIB1HNVPROC)getProcAddress("glVertexAttrib1hNV");
		glVertexAttrib1hvNV = (PFNGLVERTEXATTRIB1HVNVPROC)getProcAddress("glVertexAttrib1hvNV");
		glVertexAttrib2hNV = (PFNGLVERTEXATTRIB2HNVPROC)getProcAddress("glVertexAttrib2hNV");
		glVertexAttrib2hvNV = (PFNGLVERTEXATTRIB2HVNVPROC)getProcAddress("glVertexAttrib2hvNV");
		glVertexAttrib3hNV = (PFNGLVERTEXATTRIB3HNVPROC)getProcAddress("glVertexAttrib3hNV");
		glVertexAttrib3hvNV = (PFNGLVERTEXATTRIB3HVNVPROC)getProcAddress("glVertexAttrib3hvNV");
		glVertexAttrib4hNV = (PFNGLVERTEXATTRIB4HNVPROC)getProcAddress("glVertexAttrib4hNV");
		glVertexAttrib4hvNV = (PFNGLVERTEXATTRIB4HVNVPROC)getProcAddress("glVertexAttrib4hvNV");
		glVertexAttribs1hvNV = (PFNGLVERTEXATTRIBS1HVNVPROC)getProcAddress("glVertexAttribs1hvNV");
		glVertexAttribs2hvNV = (PFNGLVERTEXATTRIBS2HVNVPROC)getProcAddress("glVertexAttribs2hvNV");
		glVertexAttribs3hvNV = (PFNGLVERTEXATTRIBS3HVNVPROC)getProcAddress("glVertexAttribs3hvNV");
		glVertexAttribs4hvNV = (PFNGLVERTEXATTRIBS4HVNVPROC)getProcAddress("glVertexAttribs4hvNV");

		// GL_NV_pixel_data_range
		glPixelDataRangeNV = (PFNGLPIXELDATARANGENVPROC)getProcAddress("glPixelDataRangeNV");
		glFlushPixelDataRangeNV = (PFNGLFLUSHPIXELDATARANGENVPROC)getProcAddress("glFlushPixelDataRangeNV");

		// GL_NV_primitive_restart
		glPrimitiveRestartNV = (PFNGLPRIMITIVERESTARTNVPROC)getProcAddress("glPrimitiveRestartNV");
		glPrimitiveRestartIndexNV = (PFNGLPRIMITIVERESTARTINDEXNVPROC)getProcAddress("glPrimitiveRestartIndexNV");

		// GL_ATI_map_object_buffer
		glMapObjectBufferATI = (PFNGLMAPOBJECTBUFFERATIPROC)getProcAddress("glMapObjectBufferATI");
		glUnmapObjectBufferATI = (PFNGLUNMAPOBJECTBUFFERATIPROC)getProcAddress("glUnmapObjectBufferATI");

		// GL_ATI_separate_stencil
		glStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)getProcAddress("glStencilOpSeparateATI");
		glStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)getProcAddress("glStencilFuncSeparateATI");

		// GL_ATI_vertex_attrib_array_object
		glVertexAttribArrayObjectATI = (PFNGLVERTEXATTRIBARRAYOBJECTATIPROC)getProcAddress("glVertexAttribArrayObjectATI");
		glGetVertexAttribArrayObjectfvATI = (PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC)getProcAddress("glGetVertexAttribArrayObjectfvATI");
		glGetVertexAttribArrayObjectivATI = (PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC)getProcAddress("glGetVertexAttribArrayObjectivATI");

		// GL_EXT_depth_bounds_test
		glDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC)getProcAddress("glDepthBoundsEXT");

		// GL_EXT_blend_equation_separate
		glBlendEquationSeparateEXT = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)getProcAddress("glBlendEquationSeparateEXT");

		// GL_EXT_framebuffer_object
		glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)getProcAddress("glIsRenderbufferEXT");
		glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)getProcAddress("glBindRenderbufferEXT");
		glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)getProcAddress("glDeleteRenderbuffersEXT");
		glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)getProcAddress("glGenRenderbuffersEXT");
		glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)getProcAddress("glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)getProcAddress("glGetRenderbufferParameterivEXT");
		glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)getProcAddress("glIsFramebufferEXT");
		glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)getProcAddress("glBindFramebufferEXT");
		glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)getProcAddress("glDeleteFramebuffersEXT");
		glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)getProcAddress("glGenFramebuffersEXT");
		glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)getProcAddress("glCheckFramebufferStatusEXT");
		glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)getProcAddress("glFramebufferTexture1DEXT");
		glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)getProcAddress("glFramebufferTexture2DEXT");
		glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)getProcAddress("glFramebufferTexture3DEXT");
		glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)getProcAddress("glFramebufferRenderbufferEXT");
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)getProcAddress("glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)getProcAddress("glGenerateMipmapEXT");

		// GL_GREMEDY_string_marker
		glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC)getProcAddress("glStringMarkerGREMEDY");

		// GL_EXT_stencil_clear_tag
		glStencilClearTagEXT = (PFNGLSTENCILCLEARTAGEXTPROC)getProcAddress("glStencilClearTagEXT");

		// GL_EXT_framebuffer_blit
		glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC)getProcAddress("glBlitFramebufferEXT");

		// GL_EXT_framebuffer_multisample
		glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)getProcAddress("glRenderbufferStorageMultisampleEXT");

		// GL_EXT_timer_query
		glGetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC)getProcAddress("glGetQueryObjecti64vEXT");
		glGetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC)getProcAddress("glGetQueryObjectui64vEXT");

		// GL_EXT_gpu_program_parameters
		glProgramEnvParameters4fvEXT = (PFNGLPROGRAMENVPARAMETERS4FVEXTPROC)getProcAddress("glProgramEnvParameters4fvEXT");
		glProgramLocalParameters4fvEXT = (PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC)getProcAddress("glProgramLocalParameters4fvEXT");

		// GL_APPLE_flush_buffer_range
		glBufferParameteriAPPLE = (PFNGLBUFFERPARAMETERIAPPLEPROC)getProcAddress("glBufferParameteriAPPLE");
		glFlushMappedBufferRangeAPPLE = (PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC)getProcAddress("glFlushMappedBufferRangeAPPLE");

		// GL_NV_gpu_program4
		glProgramLocalParameterI4iNV = (PFNGLPROGRAMLOCALPARAMETERI4INVPROC)getProcAddress("glProgramLocalParameterI4iNV");
		glProgramLocalParameterI4ivNV = (PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC)getProcAddress("glProgramLocalParameterI4ivNV");
		glProgramLocalParametersI4ivNV = (PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC)getProcAddress("glProgramLocalParametersI4ivNV");
		glProgramLocalParameterI4uiNV = (PFNGLPROGRAMLOCALPARAMETERI4UINVPROC)getProcAddress("glProgramLocalParameterI4uiNV");
		glProgramLocalParameterI4uivNV = (PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC)getProcAddress("glProgramLocalParameterI4uivNV");
		glProgramLocalParametersI4uivNV = (PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC)getProcAddress("glProgramLocalParametersI4uivNV");
		glProgramEnvParameterI4iNV = (PFNGLPROGRAMENVPARAMETERI4INVPROC)getProcAddress("glProgramEnvParameterI4iNV");
		glProgramEnvParameterI4ivNV = (PFNGLPROGRAMENVPARAMETERI4IVNVPROC)getProcAddress("glProgramEnvParameterI4ivNV");
		glProgramEnvParametersI4ivNV = (PFNGLPROGRAMENVPARAMETERSI4IVNVPROC)getProcAddress("glProgramEnvParametersI4ivNV");
		glProgramEnvParameterI4uiNV = (PFNGLPROGRAMENVPARAMETERI4UINVPROC)getProcAddress("glProgramEnvParameterI4uiNV");
		glProgramEnvParameterI4uivNV = (PFNGLPROGRAMENVPARAMETERI4UIVNVPROC)getProcAddress("glProgramEnvParameterI4uivNV");
		glProgramEnvParametersI4uivNV = (PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC)getProcAddress("glProgramEnvParametersI4uivNV");
		glGetProgramLocalParameterIivNV = (PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC)getProcAddress("glGetProgramLocalParameterIivNV");
		glGetProgramLocalParameterIuivNV = (PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC)getProcAddress("glGetProgramLocalParameterIuivNV");
		glGetProgramEnvParameterIivNV = (PFNGLGETPROGRAMENVPARAMETERIIVNVPROC)getProcAddress("glGetProgramEnvParameterIivNV");
		glGetProgramEnvParameterIuivNV = (PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC)getProcAddress("glGetProgramEnvParameterIuivNV");

		// GL_NV_geometry_program4
		glProgramVertexLimitNV = (PFNGLPROGRAMVERTEXLIMITNVPROC)getProcAddress("glProgramVertexLimitNV");
		glFramebufferTextureEXT = (PFNGLFRAMEBUFFERTEXTUREEXTPROC)getProcAddress("glFramebufferTextureEXT");
		glFramebufferTextureLayerEXT = (PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)getProcAddress("glFramebufferTextureLayerEXT");
		glFramebufferTextureFaceEXT = (PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC)getProcAddress("glFramebufferTextureFaceEXT");

		// GL_EXT_geometry_shader4
		glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)getProcAddress("glProgramParameteriEXT");

		// GL_NV_vertex_program4
		glVertexAttribI1iEXT = (PFNGLVERTEXATTRIBI1IEXTPROC)getProcAddress("glVertexAttribI1iEXT");
		glVertexAttribI2iEXT = (PFNGLVERTEXATTRIBI2IEXTPROC)getProcAddress("glVertexAttribI2iEXT");
		glVertexAttribI3iEXT = (PFNGLVERTEXATTRIBI3IEXTPROC)getProcAddress("glVertexAttribI3iEXT");
		glVertexAttribI4iEXT = (PFNGLVERTEXATTRIBI4IEXTPROC)getProcAddress("glVertexAttribI4iEXT");
		glVertexAttribI1uiEXT = (PFNGLVERTEXATTRIBI1UIEXTPROC)getProcAddress("glVertexAttribI1uiEXT");
		glVertexAttribI2uiEXT = (PFNGLVERTEXATTRIBI2UIEXTPROC)getProcAddress("glVertexAttribI2uiEXT");
		glVertexAttribI3uiEXT = (PFNGLVERTEXATTRIBI3UIEXTPROC)getProcAddress("glVertexAttribI3uiEXT");
		glVertexAttribI4uiEXT = (PFNGLVERTEXATTRIBI4UIEXTPROC)getProcAddress("glVertexAttribI4uiEXT");
		glVertexAttribI1ivEXT = (PFNGLVERTEXATTRIBI1IVEXTPROC)getProcAddress("glVertexAttribI1ivEXT");
		glVertexAttribI2ivEXT = (PFNGLVERTEXATTRIBI2IVEXTPROC)getProcAddress("glVertexAttribI2ivEXT");
		glVertexAttribI3ivEXT = (PFNGLVERTEXATTRIBI3IVEXTPROC)getProcAddress("glVertexAttribI3ivEXT");
		glVertexAttribI4ivEXT = (PFNGLVERTEXATTRIBI4IVEXTPROC)getProcAddress("glVertexAttribI4ivEXT");
		glVertexAttribI1uivEXT = (PFNGLVERTEXATTRIBI1UIVEXTPROC)getProcAddress("glVertexAttribI1uivEXT");
		glVertexAttribI2uivEXT = (PFNGLVERTEXATTRIBI2UIVEXTPROC)getProcAddress("glVertexAttribI2uivEXT");
		glVertexAttribI3uivEXT = (PFNGLVERTEXATTRIBI3UIVEXTPROC)getProcAddress("glVertexAttribI3uivEXT");
		glVertexAttribI4uivEXT = (PFNGLVERTEXATTRIBI4UIVEXTPROC)getProcAddress("glVertexAttribI4uivEXT");
		glVertexAttribI4bvEXT = (PFNGLVERTEXATTRIBI4BVEXTPROC)getProcAddress("glVertexAttribI4bvEXT");
		glVertexAttribI4svEXT = (PFNGLVERTEXATTRIBI4SVEXTPROC)getProcAddress("glVertexAttribI4svEXT");
		glVertexAttribI4ubvEXT = (PFNGLVERTEXATTRIBI4UBVEXTPROC)getProcAddress("glVertexAttribI4ubvEXT");
		glVertexAttribI4usvEXT = (PFNGLVERTEXATTRIBI4USVEXTPROC)getProcAddress("glVertexAttribI4usvEXT");
		glVertexAttribIPointerEXT = (PFNGLVERTEXATTRIBIPOINTEREXTPROC)getProcAddress("glVertexAttribIPointerEXT");
		glGetVertexAttribIivEXT = (PFNGLGETVERTEXATTRIBIIVEXTPROC)getProcAddress("glGetVertexAttribIivEXT");
		glGetVertexAttribIuivEXT = (PFNGLGETVERTEXATTRIBIUIVEXTPROC)getProcAddress("glGetVertexAttribIuivEXT");

		// GL_EXT_gpu_shader4
		glGetUniformuivEXT = (PFNGLGETUNIFORMUIVEXTPROC)getProcAddress("glGetUniformuivEXT");
		glBindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC)getProcAddress("glBindFragDataLocationEXT");
		glGetFragDataLocationEXT = (PFNGLGETFRAGDATALOCATIONEXTPROC)getProcAddress("glGetFragDataLocationEXT");
		glUniform1uiEXT = (PFNGLUNIFORM1UIEXTPROC)getProcAddress("glUniform1uiEXT");
		glUniform2uiEXT = (PFNGLUNIFORM2UIEXTPROC)getProcAddress("glUniform2uiEXT");
		glUniform3uiEXT = (PFNGLUNIFORM3UIEXTPROC)getProcAddress("glUniform3uiEXT");
		glUniform4uiEXT = (PFNGLUNIFORM4UIEXTPROC)getProcAddress("glUniform4uiEXT");
		glUniform1uivEXT = (PFNGLUNIFORM1UIVEXTPROC)getProcAddress("glUniform1uivEXT");
		glUniform2uivEXT = (PFNGLUNIFORM2UIVEXTPROC)getProcAddress("glUniform2uivEXT");
		glUniform3uivEXT = (PFNGLUNIFORM3UIVEXTPROC)getProcAddress("glUniform3uivEXT");
		glUniform4uivEXT = (PFNGLUNIFORM4UIVEXTPROC)getProcAddress("glUniform4uivEXT");

		// GL_EXT_draw_instanced
		glDrawArraysInstancedEXT = (PFNGLDRAWARRAYSINSTANCEDEXTPROC)getProcAddress("glDrawArraysInstancedEXT");
		glDrawElementsInstancedEXT = (PFNGLDRAWELEMENTSINSTANCEDEXTPROC)getProcAddress("glDrawElementsInstancedEXT");

		// GL_EXT_texture_buffer_object
		glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)getProcAddress("glTexBufferEXT");

		// GL_NV_depth_buffer_float
		glDepthRangedNV = (PFNGLDEPTHRANGEDNVPROC)getProcAddress("glDepthRangedNV");
		glClearDepthdNV = (PFNGLCLEARDEPTHDNVPROC)getProcAddress("glClearDepthdNV");
		glDepthBoundsdNV = (PFNGLDEPTHBOUNDSDNVPROC)getProcAddress("glDepthBoundsdNV");

		// GL_NV_framebuffer_multisample_coverage
		glRenderbufferStorageMultisampleCoverageNV = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC)getProcAddress("glRenderbufferStorageMultisampleCoverageNV");

		// GL_NV_parameter_buffer_object
		glProgramBufferParametersfvNV = (PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC)getProcAddress("glProgramBufferParametersfvNV");
		glProgramBufferParametersIivNV = (PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC)getProcAddress("glProgramBufferParametersIivNV");
		glProgramBufferParametersIuivNV = (PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC)getProcAddress("glProgramBufferParametersIuivNV");

		// GL_EXT_draw_buffers2
		glColorMaskIndexedEXT = (PFNGLCOLORMASKINDEXEDEXTPROC)getProcAddress("glColorMaskIndexedEXT");
		glGetBooleanIndexedvEXT = (PFNGLGETBOOLEANINDEXEDVEXTPROC)getProcAddress("glGetBooleanIndexedvEXT");
		glGetIntegerIndexedvEXT = (PFNGLGETINTEGERINDEXEDVEXTPROC)getProcAddress("glGetIntegerIndexedvEXT");
		glEnableIndexedEXT = (PFNGLENABLEINDEXEDEXTPROC)getProcAddress("glEnableIndexedEXT");
		glDisableIndexedEXT = (PFNGLDISABLEINDEXEDEXTPROC)getProcAddress("glDisableIndexedEXT");
		glIsEnabledIndexedEXT = (PFNGLISENABLEDINDEXEDEXTPROC)getProcAddress("glIsEnabledIndexedEXT");

		// GL_NV_transform_feedback
		glBeginTransformFeedbackNV = (PFNGLBEGINTRANSFORMFEEDBACKNVPROC)getProcAddress("glBeginTransformFeedbackNV");
		glEndTransformFeedbackNV = (PFNGLENDTRANSFORMFEEDBACKNVPROC)getProcAddress("glEndTransformFeedbackNV");
		glTransformFeedbackAttribsNV = (PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC)getProcAddress("glTransformFeedbackAttribsNV");
		glBindBufferRangeNV = (PFNGLBINDBUFFERRANGENVPROC)getProcAddress("glBindBufferRangeNV");
		glBindBufferOffsetNV = (PFNGLBINDBUFFEROFFSETNVPROC)getProcAddress("glBindBufferOffsetNV");
		glBindBufferBaseNV = (PFNGLBINDBUFFERBASENVPROC)getProcAddress("glBindBufferBaseNV");
		glTransformFeedbackVaryingsNV = (PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC)getProcAddress("glTransformFeedbackVaryingsNV");
		glActiveVaryingNV = (PFNGLACTIVEVARYINGNVPROC)getProcAddress("glActiveVaryingNV");
		glGetVaryingLocationNV = (PFNGLGETVARYINGLOCATIONNVPROC)getProcAddress("glGetVaryingLocationNV");
		glGetActiveVaryingNV = (PFNGLGETACTIVEVARYINGNVPROC)getProcAddress("glGetActiveVaryingNV");
		glGetTransformFeedbackVaryingNV = (PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC)getProcAddress("glGetTransformFeedbackVaryingNV");
		glTransformFeedbackStreamAttribsNV = (PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC)getProcAddress("glTransformFeedbackStreamAttribsNV");

		// GL_EXT_bindable_uniform
		glUniformBufferEXT = (PFNGLUNIFORMBUFFEREXTPROC)getProcAddress("glUniformBufferEXT");
		glGetUniformBufferSizeEXT = (PFNGLGETUNIFORMBUFFERSIZEEXTPROC)getProcAddress("glGetUniformBufferSizeEXT");
		glGetUniformOffsetEXT = (PFNGLGETUNIFORMOFFSETEXTPROC)getProcAddress("glGetUniformOffsetEXT");

		// GL_EXT_texture_integer
		glTexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC)getProcAddress("glTexParameterIivEXT");
		glTexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC)getProcAddress("glTexParameterIuivEXT");
		glGetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC)getProcAddress("glGetTexParameterIivEXT");
		glGetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC)getProcAddress("glGetTexParameterIuivEXT");
		glClearColorIiEXT = (PFNGLCLEARCOLORIIEXTPROC)getProcAddress("glClearColorIiEXT");
		glClearColorIuiEXT = (PFNGLCLEARCOLORIUIEXTPROC)getProcAddress("glClearColorIuiEXT");

		// GL_GREMEDY_frame_terminator
		glFrameTerminatorGREMEDY = (PFNGLFRAMETERMINATORGREMEDYPROC)getProcAddress("glFrameTerminatorGREMEDY");

		// GL_NV_conditional_render
		glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC)getProcAddress("glBeginConditionalRenderNV");
		glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)getProcAddress("glEndConditionalRenderNV");

		// GL_NV_present_video
		glPresentFrameKeyedNV = (PFNGLPRESENTFRAMEKEYEDNVPROC)getProcAddress("glPresentFrameKeyedNV");
		glPresentFrameDualFillNV = (PFNGLPRESENTFRAMEDUALFILLNVPROC)getProcAddress("glPresentFrameDualFillNV");
		glGetVideoivNV = (PFNGLGETVIDEOIVNVPROC)getProcAddress("glGetVideoivNV");
		glGetVideouivNV = (PFNGLGETVIDEOUIVNVPROC)getProcAddress("glGetVideouivNV");
		glGetVideoi64vNV = (PFNGLGETVIDEOI64VNVPROC)getProcAddress("glGetVideoi64vNV");
		glGetVideoui64vNV = (PFNGLGETVIDEOUI64VNVPROC)getProcAddress("glGetVideoui64vNV");

		// GL_EXT_transform_feedback
		glBeginTransformFeedbackEXT = (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)getProcAddress("glBeginTransformFeedbackEXT");
		glEndTransformFeedbackEXT = (PFNGLENDTRANSFORMFEEDBACKEXTPROC)getProcAddress("glEndTransformFeedbackEXT");
		glBindBufferRangeEXT = (PFNGLBINDBUFFERRANGEEXTPROC)getProcAddress("glBindBufferRangeEXT");
		glBindBufferOffsetEXT = (PFNGLBINDBUFFEROFFSETEXTPROC)getProcAddress("glBindBufferOffsetEXT");
		glBindBufferBaseEXT = (PFNGLBINDBUFFERBASEEXTPROC)getProcAddress("glBindBufferBaseEXT");
		glTransformFeedbackVaryingsEXT = (PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)getProcAddress("glTransformFeedbackVaryingsEXT");
		glGetTransformFeedbackVaryingEXT = (PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)getProcAddress("glGetTransformFeedbackVaryingEXT");

		// GL_EXT_direct_state_access
		glClientAttribDefaultEXT = (PFNGLCLIENTATTRIBDEFAULTEXTPROC)getProcAddress("glClientAttribDefaultEXT");
		glPushClientAttribDefaultEXT = (PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC)getProcAddress("glPushClientAttribDefaultEXT");
		glMatrixLoadfEXT = (PFNGLMATRIXLOADFEXTPROC)getProcAddress("glMatrixLoadfEXT");
		glMatrixLoaddEXT = (PFNGLMATRIXLOADDEXTPROC)getProcAddress("glMatrixLoaddEXT");
		glMatrixMultfEXT = (PFNGLMATRIXMULTFEXTPROC)getProcAddress("glMatrixMultfEXT");
		glMatrixMultdEXT = (PFNGLMATRIXMULTDEXTPROC)getProcAddress("glMatrixMultdEXT");
		glMatrixLoadIdentityEXT = (PFNGLMATRIXLOADIDENTITYEXTPROC)getProcAddress("glMatrixLoadIdentityEXT");
		glMatrixRotatefEXT = (PFNGLMATRIXROTATEFEXTPROC)getProcAddress("glMatrixRotatefEXT");
		glMatrixRotatedEXT = (PFNGLMATRIXROTATEDEXTPROC)getProcAddress("glMatrixRotatedEXT");
		glMatrixScalefEXT = (PFNGLMATRIXSCALEFEXTPROC)getProcAddress("glMatrixScalefEXT");
		glMatrixScaledEXT = (PFNGLMATRIXSCALEDEXTPROC)getProcAddress("glMatrixScaledEXT");
		glMatrixTranslatefEXT = (PFNGLMATRIXTRANSLATEFEXTPROC)getProcAddress("glMatrixTranslatefEXT");
		glMatrixTranslatedEXT = (PFNGLMATRIXTRANSLATEDEXTPROC)getProcAddress("glMatrixTranslatedEXT");
		glMatrixFrustumEXT = (PFNGLMATRIXFRUSTUMEXTPROC)getProcAddress("glMatrixFrustumEXT");
		glMatrixOrthoEXT = (PFNGLMATRIXORTHOEXTPROC)getProcAddress("glMatrixOrthoEXT");
		glMatrixPopEXT = (PFNGLMATRIXPOPEXTPROC)getProcAddress("glMatrixPopEXT");
		glMatrixPushEXT = (PFNGLMATRIXPUSHEXTPROC)getProcAddress("glMatrixPushEXT");
		glMatrixLoadTransposefEXT = (PFNGLMATRIXLOADTRANSPOSEFEXTPROC)getProcAddress("glMatrixLoadTransposefEXT");
		glMatrixLoadTransposedEXT = (PFNGLMATRIXLOADTRANSPOSEDEXTPROC)getProcAddress("glMatrixLoadTransposedEXT");
		glMatrixMultTransposefEXT = (PFNGLMATRIXMULTTRANSPOSEFEXTPROC)getProcAddress("glMatrixMultTransposefEXT");
		glMatrixMultTransposedEXT = (PFNGLMATRIXMULTTRANSPOSEDEXTPROC)getProcAddress("glMatrixMultTransposedEXT");
		glTextureParameterfEXT = (PFNGLTEXTUREPARAMETERFEXTPROC)getProcAddress("glTextureParameterfEXT");
		glTextureParameterfvEXT = (PFNGLTEXTUREPARAMETERFVEXTPROC)getProcAddress("glTextureParameterfvEXT");
		glTextureParameteriEXT = (PFNGLTEXTUREPARAMETERIEXTPROC)getProcAddress("glTextureParameteriEXT");
		glTextureParameterivEXT = (PFNGLTEXTUREPARAMETERIVEXTPROC)getProcAddress("glTextureParameterivEXT");
		glTextureImage1DEXT = (PFNGLTEXTUREIMAGE1DEXTPROC)getProcAddress("glTextureImage1DEXT");
		glTextureImage2DEXT = (PFNGLTEXTUREIMAGE2DEXTPROC)getProcAddress("glTextureImage2DEXT");
		glTextureSubImage1DEXT = (PFNGLTEXTURESUBIMAGE1DEXTPROC)getProcAddress("glTextureSubImage1DEXT");
		glTextureSubImage2DEXT = (PFNGLTEXTURESUBIMAGE2DEXTPROC)getProcAddress("glTextureSubImage2DEXT");
		glCopyTextureImage1DEXT = (PFNGLCOPYTEXTUREIMAGE1DEXTPROC)getProcAddress("glCopyTextureImage1DEXT");
		glCopyTextureImage2DEXT = (PFNGLCOPYTEXTUREIMAGE2DEXTPROC)getProcAddress("glCopyTextureImage2DEXT");
		glCopyTextureSubImage1DEXT = (PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC)getProcAddress("glCopyTextureSubImage1DEXT");
		glCopyTextureSubImage2DEXT = (PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC)getProcAddress("glCopyTextureSubImage2DEXT");
		glGetTextureImageEXT = (PFNGLGETTEXTUREIMAGEEXTPROC)getProcAddress("glGetTextureImageEXT");
		glGetTextureParameterfvEXT = (PFNGLGETTEXTUREPARAMETERFVEXTPROC)getProcAddress("glGetTextureParameterfvEXT");
		glGetTextureParameterivEXT = (PFNGLGETTEXTUREPARAMETERIVEXTPROC)getProcAddress("glGetTextureParameterivEXT");
		glGetTextureLevelParameterfvEXT = (PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC)getProcAddress("glGetTextureLevelParameterfvEXT");
		glGetTextureLevelParameterivEXT = (PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC)getProcAddress("glGetTextureLevelParameterivEXT");
		glTextureImage3DEXT = (PFNGLTEXTUREIMAGE3DEXTPROC)getProcAddress("glTextureImage3DEXT");
		glTextureSubImage3DEXT = (PFNGLTEXTURESUBIMAGE3DEXTPROC)getProcAddress("glTextureSubImage3DEXT");
		glCopyTextureSubImage3DEXT = (PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC)getProcAddress("glCopyTextureSubImage3DEXT");
		glMultiTexParameterfEXT = (PFNGLMULTITEXPARAMETERFEXTPROC)getProcAddress("glMultiTexParameterfEXT");
		glMultiTexParameterfvEXT = (PFNGLMULTITEXPARAMETERFVEXTPROC)getProcAddress("glMultiTexParameterfvEXT");
		glMultiTexParameteriEXT = (PFNGLMULTITEXPARAMETERIEXTPROC)getProcAddress("glMultiTexParameteriEXT");
		glMultiTexParameterivEXT = (PFNGLMULTITEXPARAMETERIVEXTPROC)getProcAddress("glMultiTexParameterivEXT");
		glMultiTexImage1DEXT = (PFNGLMULTITEXIMAGE1DEXTPROC)getProcAddress("glMultiTexImage1DEXT");
		glMultiTexImage2DEXT = (PFNGLMULTITEXIMAGE2DEXTPROC)getProcAddress("glMultiTexImage2DEXT");
		glMultiTexSubImage1DEXT = (PFNGLMULTITEXSUBIMAGE1DEXTPROC)getProcAddress("glMultiTexSubImage1DEXT");
		glMultiTexSubImage2DEXT = (PFNGLMULTITEXSUBIMAGE2DEXTPROC)getProcAddress("glMultiTexSubImage2DEXT");
		glCopyMultiTexImage1DEXT = (PFNGLCOPYMULTITEXIMAGE1DEXTPROC)getProcAddress("glCopyMultiTexImage1DEXT");
		glCopyMultiTexImage2DEXT = (PFNGLCOPYMULTITEXIMAGE2DEXTPROC)getProcAddress("glCopyMultiTexImage2DEXT");
		glCopyMultiTexSubImage1DEXT = (PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC)getProcAddress("glCopyMultiTexSubImage1DEXT");
		glCopyMultiTexSubImage2DEXT = (PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC)getProcAddress("glCopyMultiTexSubImage2DEXT");
		glGetMultiTexImageEXT = (PFNGLGETMULTITEXIMAGEEXTPROC)getProcAddress("glGetMultiTexImageEXT");
		glGetMultiTexParameterfvEXT = (PFNGLGETMULTITEXPARAMETERFVEXTPROC)getProcAddress("glGetMultiTexParameterfvEXT");
		glGetMultiTexParameterivEXT = (PFNGLGETMULTITEXPARAMETERIVEXTPROC)getProcAddress("glGetMultiTexParameterivEXT");
		glGetMultiTexLevelParameterfvEXT = (PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC)getProcAddress("glGetMultiTexLevelParameterfvEXT");
		glGetMultiTexLevelParameterivEXT = (PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC)getProcAddress("glGetMultiTexLevelParameterivEXT");
		glMultiTexImage3DEXT = (PFNGLMULTITEXIMAGE3DEXTPROC)getProcAddress("glMultiTexImage3DEXT");
		glMultiTexSubImage3DEXT = (PFNGLMULTITEXSUBIMAGE3DEXTPROC)getProcAddress("glMultiTexSubImage3DEXT");
		glCopyMultiTexSubImage3DEXT = (PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC)getProcAddress("glCopyMultiTexSubImage3DEXT");
		glBindMultiTextureEXT = (PFNGLBINDMULTITEXTUREEXTPROC)getProcAddress("glBindMultiTextureEXT");
		glEnableClientStateIndexedEXT = (PFNGLENABLECLIENTSTATEINDEXEDEXTPROC)getProcAddress("glEnableClientStateIndexedEXT");
		glDisableClientStateIndexedEXT = (PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC)getProcAddress("glDisableClientStateIndexedEXT");
		glMultiTexCoordPointerEXT = (PFNGLMULTITEXCOORDPOINTEREXTPROC)getProcAddress("glMultiTexCoordPointerEXT");
		glMultiTexEnvfEXT = (PFNGLMULTITEXENVFEXTPROC)getProcAddress("glMultiTexEnvfEXT");
		glMultiTexEnvfvEXT = (PFNGLMULTITEXENVFVEXTPROC)getProcAddress("glMultiTexEnvfvEXT");
		glMultiTexEnviEXT = (PFNGLMULTITEXENVIEXTPROC)getProcAddress("glMultiTexEnviEXT");
		glMultiTexEnvivEXT = (PFNGLMULTITEXENVIVEXTPROC)getProcAddress("glMultiTexEnvivEXT");
		glMultiTexGendEXT = (PFNGLMULTITEXGENDEXTPROC)getProcAddress("glMultiTexGendEXT");
		glMultiTexGendvEXT = (PFNGLMULTITEXGENDVEXTPROC)getProcAddress("glMultiTexGendvEXT");
		glMultiTexGenfEXT = (PFNGLMULTITEXGENFEXTPROC)getProcAddress("glMultiTexGenfEXT");
		glMultiTexGenfvEXT = (PFNGLMULTITEXGENFVEXTPROC)getProcAddress("glMultiTexGenfvEXT");
		glMultiTexGeniEXT = (PFNGLMULTITEXGENIEXTPROC)getProcAddress("glMultiTexGeniEXT");
		glMultiTexGenivEXT = (PFNGLMULTITEXGENIVEXTPROC)getProcAddress("glMultiTexGenivEXT");
		glGetMultiTexEnvfvEXT = (PFNGLGETMULTITEXENVFVEXTPROC)getProcAddress("glGetMultiTexEnvfvEXT");
		glGetMultiTexEnvivEXT = (PFNGLGETMULTITEXENVIVEXTPROC)getProcAddress("glGetMultiTexEnvivEXT");
		glGetMultiTexGendvEXT = (PFNGLGETMULTITEXGENDVEXTPROC)getProcAddress("glGetMultiTexGendvEXT");
		glGetMultiTexGenfvEXT = (PFNGLGETMULTITEXGENFVEXTPROC)getProcAddress("glGetMultiTexGenfvEXT");
		glGetMultiTexGenivEXT = (PFNGLGETMULTITEXGENIVEXTPROC)getProcAddress("glGetMultiTexGenivEXT");
		glGetFloatIndexedvEXT = (PFNGLGETFLOATINDEXEDVEXTPROC)getProcAddress("glGetFloatIndexedvEXT");
		glGetDoubleIndexedvEXT = (PFNGLGETDOUBLEINDEXEDVEXTPROC)getProcAddress("glGetDoubleIndexedvEXT");
		glGetPointerIndexedvEXT = (PFNGLGETPOINTERINDEXEDVEXTPROC)getProcAddress("glGetPointerIndexedvEXT");
		glCompressedTextureImage3DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC)getProcAddress("glCompressedTextureImage3DEXT");
		glCompressedTextureImage2DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC)getProcAddress("glCompressedTextureImage2DEXT");
		glCompressedTextureImage1DEXT = (PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC)getProcAddress("glCompressedTextureImage1DEXT");
		glCompressedTextureSubImage3DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC)getProcAddress("glCompressedTextureSubImage3DEXT");
		glCompressedTextureSubImage2DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC)getProcAddress("glCompressedTextureSubImage2DEXT");
		glCompressedTextureSubImage1DEXT = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC)getProcAddress("glCompressedTextureSubImage1DEXT");
		glGetCompressedTextureImageEXT = (PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC)getProcAddress("glGetCompressedTextureImageEXT");
		glCompressedMultiTexImage3DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC)getProcAddress("glCompressedMultiTexImage3DEXT");
		glCompressedMultiTexImage2DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC)getProcAddress("glCompressedMultiTexImage2DEXT");
		glCompressedMultiTexImage1DEXT = (PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC)getProcAddress("glCompressedMultiTexImage1DEXT");
		glCompressedMultiTexSubImage3DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC)getProcAddress("glCompressedMultiTexSubImage3DEXT");
		glCompressedMultiTexSubImage2DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC)getProcAddress("glCompressedMultiTexSubImage2DEXT");
		glCompressedMultiTexSubImage1DEXT = (PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC)getProcAddress("glCompressedMultiTexSubImage1DEXT");
		glGetCompressedMultiTexImageEXT = (PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC)getProcAddress("glGetCompressedMultiTexImageEXT");
		glNamedProgramStringEXT = (PFNGLNAMEDPROGRAMSTRINGEXTPROC)getProcAddress("glNamedProgramStringEXT");
		glNamedProgramLocalParameter4dEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC)getProcAddress("glNamedProgramLocalParameter4dEXT");
		glNamedProgramLocalParameter4dvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC)getProcAddress("glNamedProgramLocalParameter4dvEXT");
		glNamedProgramLocalParameter4fEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC)getProcAddress("glNamedProgramLocalParameter4fEXT");
		glNamedProgramLocalParameter4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC)getProcAddress("glNamedProgramLocalParameter4fvEXT");
		glGetNamedProgramLocalParameterdvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC)getProcAddress("glGetNamedProgramLocalParameterdvEXT");
		glGetNamedProgramLocalParameterfvEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC)getProcAddress("glGetNamedProgramLocalParameterfvEXT");
		glGetNamedProgramivEXT = (PFNGLGETNAMEDPROGRAMIVEXTPROC)getProcAddress("glGetNamedProgramivEXT");
		glGetNamedProgramStringEXT = (PFNGLGETNAMEDPROGRAMSTRINGEXTPROC)getProcAddress("glGetNamedProgramStringEXT");
		glNamedProgramLocalParameters4fvEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC)getProcAddress("glNamedProgramLocalParameters4fvEXT");
		glNamedProgramLocalParameterI4iEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC)getProcAddress("glNamedProgramLocalParameterI4iEXT");
		glNamedProgramLocalParameterI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC)getProcAddress("glNamedProgramLocalParameterI4ivEXT");
		glNamedProgramLocalParametersI4ivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC)getProcAddress("glNamedProgramLocalParametersI4ivEXT");
		glNamedProgramLocalParameterI4uiEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC)getProcAddress("glNamedProgramLocalParameterI4uiEXT");
		glNamedProgramLocalParameterI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC)getProcAddress("glNamedProgramLocalParameterI4uivEXT");
		glNamedProgramLocalParametersI4uivEXT = (PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC)getProcAddress("glNamedProgramLocalParametersI4uivEXT");
		glGetNamedProgramLocalParameterIivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC)getProcAddress("glGetNamedProgramLocalParameterIivEXT");
		glGetNamedProgramLocalParameterIuivEXT = (PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC)getProcAddress("glGetNamedProgramLocalParameterIuivEXT");
		glTextureParameterIivEXT = (PFNGLTEXTUREPARAMETERIIVEXTPROC)getProcAddress("glTextureParameterIivEXT");
		glTextureParameterIuivEXT = (PFNGLTEXTUREPARAMETERIUIVEXTPROC)getProcAddress("glTextureParameterIuivEXT");
		glGetTextureParameterIivEXT = (PFNGLGETTEXTUREPARAMETERIIVEXTPROC)getProcAddress("glGetTextureParameterIivEXT");
		glGetTextureParameterIuivEXT = (PFNGLGETTEXTUREPARAMETERIUIVEXTPROC)getProcAddress("glGetTextureParameterIuivEXT");
		glMultiTexParameterIivEXT = (PFNGLMULTITEXPARAMETERIIVEXTPROC)getProcAddress("glMultiTexParameterIivEXT");
		glMultiTexParameterIuivEXT = (PFNGLMULTITEXPARAMETERIUIVEXTPROC)getProcAddress("glMultiTexParameterIuivEXT");
		glGetMultiTexParameterIivEXT = (PFNGLGETMULTITEXPARAMETERIIVEXTPROC)getProcAddress("glGetMultiTexParameterIivEXT");
		glGetMultiTexParameterIuivEXT = (PFNGLGETMULTITEXPARAMETERIUIVEXTPROC)getProcAddress("glGetMultiTexParameterIuivEXT");
		glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC)getProcAddress("glProgramUniform1fEXT");
		glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC)getProcAddress("glProgramUniform2fEXT");
		glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC)getProcAddress("glProgramUniform3fEXT");
		glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC)getProcAddress("glProgramUniform4fEXT");
		glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC)getProcAddress("glProgramUniform1iEXT");
		glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC)getProcAddress("glProgramUniform2iEXT");
		glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC)getProcAddress("glProgramUniform3iEXT");
		glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC)getProcAddress("glProgramUniform4iEXT");
		glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC)getProcAddress("glProgramUniform1fvEXT");
		glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC)getProcAddress("glProgramUniform2fvEXT");
		glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC)getProcAddress("glProgramUniform3fvEXT");
		glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC)getProcAddress("glProgramUniform4fvEXT");
		glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC)getProcAddress("glProgramUniform1ivEXT");
		glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC)getProcAddress("glProgramUniform2ivEXT");
		glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC)getProcAddress("glProgramUniform3ivEXT");
		glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC)getProcAddress("glProgramUniform4ivEXT");
		glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)getProcAddress("glProgramUniformMatrix2fvEXT");
		glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)getProcAddress("glProgramUniformMatrix3fvEXT");
		glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)getProcAddress("glProgramUniformMatrix4fvEXT");
		glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)getProcAddress("glProgramUniformMatrix2x3fvEXT");
		glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)getProcAddress("glProgramUniformMatrix3x2fvEXT");
		glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)getProcAddress("glProgramUniformMatrix2x4fvEXT");
		glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)getProcAddress("glProgramUniformMatrix4x2fvEXT");
		glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)getProcAddress("glProgramUniformMatrix3x4fvEXT");
		glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)getProcAddress("glProgramUniformMatrix4x3fvEXT");
		glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC)getProcAddress("glProgramUniform1uiEXT");
		glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC)getProcAddress("glProgramUniform2uiEXT");
		glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC)getProcAddress("glProgramUniform3uiEXT");
		glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC)getProcAddress("glProgramUniform4uiEXT");
		glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC)getProcAddress("glProgramUniform1uivEXT");
		glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC)getProcAddress("glProgramUniform2uivEXT");
		glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC)getProcAddress("glProgramUniform3uivEXT");
		glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC)getProcAddress("glProgramUniform4uivEXT");
		glNamedBufferDataEXT = (PFNGLNAMEDBUFFERDATAEXTPROC)getProcAddress("glNamedBufferDataEXT");
		glNamedBufferSubDataEXT = (PFNGLNAMEDBUFFERSUBDATAEXTPROC)getProcAddress("glNamedBufferSubDataEXT");
		glMapNamedBufferEXT = (PFNGLMAPNAMEDBUFFEREXTPROC)getProcAddress("glMapNamedBufferEXT");
		glUnmapNamedBufferEXT = (PFNGLUNMAPNAMEDBUFFEREXTPROC)getProcAddress("glUnmapNamedBufferEXT");
		glMapNamedBufferRangeEXT = (PFNGLMAPNAMEDBUFFERRANGEEXTPROC)getProcAddress("glMapNamedBufferRangeEXT");
		glFlushMappedNamedBufferRangeEXT = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC)getProcAddress("glFlushMappedNamedBufferRangeEXT");
		glNamedCopyBufferSubDataEXT = (PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC)getProcAddress("glNamedCopyBufferSubDataEXT");
		glGetNamedBufferParameterivEXT = (PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC)getProcAddress("glGetNamedBufferParameterivEXT");
		glGetNamedBufferPointervEXT = (PFNGLGETNAMEDBUFFERPOINTERVEXTPROC)getProcAddress("glGetNamedBufferPointervEXT");
		glGetNamedBufferSubDataEXT = (PFNGLGETNAMEDBUFFERSUBDATAEXTPROC)getProcAddress("glGetNamedBufferSubDataEXT");
		glTextureBufferEXT = (PFNGLTEXTUREBUFFEREXTPROC)getProcAddress("glTextureBufferEXT");
		glMultiTexBufferEXT = (PFNGLMULTITEXBUFFEREXTPROC)getProcAddress("glMultiTexBufferEXT");
		glNamedRenderbufferStorageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC)getProcAddress("glNamedRenderbufferStorageEXT");
		glGetNamedRenderbufferParameterivEXT = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC)getProcAddress("glGetNamedRenderbufferParameterivEXT");
		glCheckNamedFramebufferStatusEXT = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC)getProcAddress("glCheckNamedFramebufferStatusEXT");
		glNamedFramebufferTexture1DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC)getProcAddress("glNamedFramebufferTexture1DEXT");
		glNamedFramebufferTexture2DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC)getProcAddress("glNamedFramebufferTexture2DEXT");
		glNamedFramebufferTexture3DEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC)getProcAddress("glNamedFramebufferTexture3DEXT");
		glNamedFramebufferRenderbufferEXT = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC)getProcAddress("glNamedFramebufferRenderbufferEXT");
		glGetNamedFramebufferAttachmentParameterivEXT = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)getProcAddress("glGetNamedFramebufferAttachmentParameterivEXT");
		glGenerateTextureMipmapEXT = (PFNGLGENERATETEXTUREMIPMAPEXTPROC)getProcAddress("glGenerateTextureMipmapEXT");
		glGenerateMultiTexMipmapEXT = (PFNGLGENERATEMULTITEXMIPMAPEXTPROC)getProcAddress("glGenerateMultiTexMipmapEXT");
		glFramebufferDrawBufferEXT = (PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC)getProcAddress("glFramebufferDrawBufferEXT");
		glFramebufferDrawBuffersEXT = (PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC)getProcAddress("glFramebufferDrawBuffersEXT");
		glFramebufferReadBufferEXT = (PFNGLFRAMEBUFFERREADBUFFEREXTPROC)getProcAddress("glFramebufferReadBufferEXT");
		glGetFramebufferParameterivEXT = (PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC)getProcAddress("glGetFramebufferParameterivEXT");
		glNamedRenderbufferStorageMultisampleEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)getProcAddress("glNamedRenderbufferStorageMultisampleEXT");
		glNamedRenderbufferStorageMultisampleCoverageEXT = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC)getProcAddress("glNamedRenderbufferStorageMultisampleCoverageEXT");
		glNamedFramebufferTextureEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC)getProcAddress("glNamedFramebufferTextureEXT");
		glNamedFramebufferTextureLayerEXT = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC)getProcAddress("glNamedFramebufferTextureLayerEXT");
		glNamedFramebufferTextureFaceEXT = (PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC)getProcAddress("glNamedFramebufferTextureFaceEXT");
		glTextureRenderbufferEXT = (PFNGLTEXTURERENDERBUFFEREXTPROC)getProcAddress("glTextureRenderbufferEXT");
		glMultiTexRenderbufferEXT = (PFNGLMULTITEXRENDERBUFFEREXTPROC)getProcAddress("glMultiTexRenderbufferEXT");
		glProgramUniform1dEXT = (PFNGLPROGRAMUNIFORM1DEXTPROC)getProcAddress("glProgramUniform1dEXT");
		glProgramUniform2dEXT = (PFNGLPROGRAMUNIFORM2DEXTPROC)getProcAddress("glProgramUniform2dEXT");
		glProgramUniform3dEXT = (PFNGLPROGRAMUNIFORM3DEXTPROC)getProcAddress("glProgramUniform3dEXT");
		glProgramUniform4dEXT = (PFNGLPROGRAMUNIFORM4DEXTPROC)getProcAddress("glProgramUniform4dEXT");
		glProgramUniform1dvEXT = (PFNGLPROGRAMUNIFORM1DVEXTPROC)getProcAddress("glProgramUniform1dvEXT");
		glProgramUniform2dvEXT = (PFNGLPROGRAMUNIFORM2DVEXTPROC)getProcAddress("glProgramUniform2dvEXT");
		glProgramUniform3dvEXT = (PFNGLPROGRAMUNIFORM3DVEXTPROC)getProcAddress("glProgramUniform3dvEXT");
		glProgramUniform4dvEXT = (PFNGLPROGRAMUNIFORM4DVEXTPROC)getProcAddress("glProgramUniform4dvEXT");
		glProgramUniformMatrix2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC)getProcAddress("glProgramUniformMatrix2dvEXT");
		glProgramUniformMatrix3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC)getProcAddress("glProgramUniformMatrix3dvEXT");
		glProgramUniformMatrix4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC)getProcAddress("glProgramUniformMatrix4dvEXT");
		glProgramUniformMatrix2x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC)getProcAddress("glProgramUniformMatrix2x3dvEXT");
		glProgramUniformMatrix2x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC)getProcAddress("glProgramUniformMatrix2x4dvEXT");
		glProgramUniformMatrix3x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC)getProcAddress("glProgramUniformMatrix3x2dvEXT");
		glProgramUniformMatrix3x4dvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC)getProcAddress("glProgramUniformMatrix3x4dvEXT");
		glProgramUniformMatrix4x2dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC)getProcAddress("glProgramUniformMatrix4x2dvEXT");
		glProgramUniformMatrix4x3dvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC)getProcAddress("glProgramUniformMatrix4x3dvEXT");

		// GL_NV_explicit_multisample
		glGetMultisamplefvNV = (PFNGLGETMULTISAMPLEFVNVPROC)getProcAddress("glGetMultisamplefvNV");
		glSampleMaskIndexedNV = (PFNGLSAMPLEMASKINDEXEDNVPROC)getProcAddress("glSampleMaskIndexedNV");
		glTexRenderbufferNV = (PFNGLTEXRENDERBUFFERNVPROC)getProcAddress("glTexRenderbufferNV");

		// GL_NV_transform_feedback2
		glBindTransformFeedbackNV = (PFNGLBINDTRANSFORMFEEDBACKNVPROC)getProcAddress("glBindTransformFeedbackNV");
		glDeleteTransformFeedbacksNV = (PFNGLDELETETRANSFORMFEEDBACKSNVPROC)getProcAddress("glDeleteTransformFeedbacksNV");
		glGenTransformFeedbacksNV = (PFNGLGENTRANSFORMFEEDBACKSNVPROC)getProcAddress("glGenTransformFeedbacksNV");
		glIsTransformFeedbackNV = (PFNGLISTRANSFORMFEEDBACKNVPROC)getProcAddress("glIsTransformFeedbackNV");
		glPauseTransformFeedbackNV = (PFNGLPAUSETRANSFORMFEEDBACKNVPROC)getProcAddress("glPauseTransformFeedbackNV");
		glResumeTransformFeedbackNV = (PFNGLRESUMETRANSFORMFEEDBACKNVPROC)getProcAddress("glResumeTransformFeedbackNV");
		glDrawTransformFeedbackNV = (PFNGLDRAWTRANSFORMFEEDBACKNVPROC)getProcAddress("glDrawTransformFeedbackNV");

		// GL_AMD_performance_monitor
		glGetPerfMonitorGroupsAMD = (PFNGLGETPERFMONITORGROUPSAMDPROC)getProcAddress("glGetPerfMonitorGroupsAMD");
		glGetPerfMonitorCountersAMD = (PFNGLGETPERFMONITORCOUNTERSAMDPROC)getProcAddress("glGetPerfMonitorCountersAMD");
		glGetPerfMonitorGroupStringAMD = (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC)getProcAddress("glGetPerfMonitorGroupStringAMD");
		glGetPerfMonitorCounterStringAMD = (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC)getProcAddress("glGetPerfMonitorCounterStringAMD");
		glGetPerfMonitorCounterInfoAMD = (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC)getProcAddress("glGetPerfMonitorCounterInfoAMD");
		glGenPerfMonitorsAMD = (PFNGLGENPERFMONITORSAMDPROC)getProcAddress("glGenPerfMonitorsAMD");
		glDeletePerfMonitorsAMD = (PFNGLDELETEPERFMONITORSAMDPROC)getProcAddress("glDeletePerfMonitorsAMD");
		glSelectPerfMonitorCountersAMD = (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC)getProcAddress("glSelectPerfMonitorCountersAMD");
		glBeginPerfMonitorAMD = (PFNGLBEGINPERFMONITORAMDPROC)getProcAddress("glBeginPerfMonitorAMD");
		glEndPerfMonitorAMD = (PFNGLENDPERFMONITORAMDPROC)getProcAddress("glEndPerfMonitorAMD");
		glGetPerfMonitorCounterDataAMD = (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC)getProcAddress("glGetPerfMonitorCounterDataAMD");

		// GL_AMD_vertex_shader_tesselator
		glTessellationFactorAMD = (PFNGLTESSELLATIONFACTORAMDPROC)getProcAddress("glTessellationFactorAMD");
		glTessellationModeAMD = (PFNGLTESSELLATIONMODEAMDPROC)getProcAddress("glTessellationModeAMD");

		// GL_EXT_provoking_vertex
		glProvokingVertexEXT = (PFNGLPROVOKINGVERTEXEXTPROC)getProcAddress("glProvokingVertexEXT");

		// GL_AMD_draw_buffers_blend
		glBlendFuncIndexedAMD = (PFNGLBLENDFUNCINDEXEDAMDPROC)getProcAddress("glBlendFuncIndexedAMD");
		glBlendFuncSeparateIndexedAMD = (PFNGLBLENDFUNCSEPARATEINDEXEDAMDPROC)getProcAddress("glBlendFuncSeparateIndexedAMD");
		glBlendEquationIndexedAMD = (PFNGLBLENDEQUATIONINDEXEDAMDPROC)getProcAddress("glBlendEquationIndexedAMD");
		glBlendEquationSeparateIndexedAMD = (PFNGLBLENDEQUATIONSEPARATEINDEXEDAMDPROC)getProcAddress("glBlendEquationSeparateIndexedAMD");

		// GL_APPLE_texture_range
		glTextureRangeAPPLE = (PFNGLTEXTURERANGEAPPLEPROC)getProcAddress("glTextureRangeAPPLE");
		glGetTexParameterPointervAPPLE = (PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC)getProcAddress("glGetTexParameterPointervAPPLE");

		// GL_APPLE_vertex_program_evaluators
		glEnableVertexAttribAPPLE = (PFNGLENABLEVERTEXATTRIBAPPLEPROC)getProcAddress("glEnableVertexAttribAPPLE");
		glDisableVertexAttribAPPLE = (PFNGLDISABLEVERTEXATTRIBAPPLEPROC)getProcAddress("glDisableVertexAttribAPPLE");
		glIsVertexAttribEnabledAPPLE = (PFNGLISVERTEXATTRIBENABLEDAPPLEPROC)getProcAddress("glIsVertexAttribEnabledAPPLE");
		glMapVertexAttrib1dAPPLE = (PFNGLMAPVERTEXATTRIB1DAPPLEPROC)getProcAddress("glMapVertexAttrib1dAPPLE");
		glMapVertexAttrib1fAPPLE = (PFNGLMAPVERTEXATTRIB1FAPPLEPROC)getProcAddress("glMapVertexAttrib1fAPPLE");
		glMapVertexAttrib2dAPPLE = (PFNGLMAPVERTEXATTRIB2DAPPLEPROC)getProcAddress("glMapVertexAttrib2dAPPLE");
		glMapVertexAttrib2fAPPLE = (PFNGLMAPVERTEXATTRIB2FAPPLEPROC)getProcAddress("glMapVertexAttrib2fAPPLE");

		// GL_APPLE_object_purgeable
		glObjectPurgeableAPPLE = (PFNGLOBJECTPURGEABLEAPPLEPROC)getProcAddress("glObjectPurgeableAPPLE");
		glObjectUnpurgeableAPPLE = (PFNGLOBJECTUNPURGEABLEAPPLEPROC)getProcAddress("glObjectUnpurgeableAPPLE");
		glGetObjectParameterivAPPLE = (PFNGLGETOBJECTPARAMETERIVAPPLEPROC)getProcAddress("glGetObjectParameterivAPPLE");

		// GL_NV_video_capture
		glBeginVideoCaptureNV = (PFNGLBEGINVIDEOCAPTURENVPROC)getProcAddress("glBeginVideoCaptureNV");
		glBindVideoCaptureStreamBufferNV = (PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC)getProcAddress("glBindVideoCaptureStreamBufferNV");
		glBindVideoCaptureStreamTextureNV = (PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC)getProcAddress("glBindVideoCaptureStreamTextureNV");
		glEndVideoCaptureNV = (PFNGLENDVIDEOCAPTURENVPROC)getProcAddress("glEndVideoCaptureNV");
		glGetVideoCaptureivNV = (PFNGLGETVIDEOCAPTUREIVNVPROC)getProcAddress("glGetVideoCaptureivNV");
		glGetVideoCaptureStreamivNV = (PFNGLGETVIDEOCAPTURESTREAMIVNVPROC)getProcAddress("glGetVideoCaptureStreamivNV");
		glGetVideoCaptureStreamfvNV = (PFNGLGETVIDEOCAPTURESTREAMFVNVPROC)getProcAddress("glGetVideoCaptureStreamfvNV");
		glGetVideoCaptureStreamdvNV = (PFNGLGETVIDEOCAPTURESTREAMDVNVPROC)getProcAddress("glGetVideoCaptureStreamdvNV");
		glVideoCaptureNV = (PFNGLVIDEOCAPTURENVPROC)getProcAddress("glVideoCaptureNV");
		glVideoCaptureStreamParameterivNV = (PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC)getProcAddress("glVideoCaptureStreamParameterivNV");
		glVideoCaptureStreamParameterfvNV = (PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC)getProcAddress("glVideoCaptureStreamParameterfvNV");
		glVideoCaptureStreamParameterdvNV = (PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC)getProcAddress("glVideoCaptureStreamParameterdvNV");

		// GL_NV_copy_image
		glCopyImageSubDataNV = (PFNGLCOPYIMAGESUBDATANVPROC)getProcAddress("glCopyImageSubDataNV");

		// GL_EXT_separate_shader_objects
		glUseShaderProgramEXT = (PFNGLUSESHADERPROGRAMEXTPROC)getProcAddress("glUseShaderProgramEXT");
		glActiveProgramEXT = (PFNGLACTIVEPROGRAMEXTPROC)getProcAddress("glActiveProgramEXT");
		glCreateShaderProgramEXT = (PFNGLCREATESHADERPROGRAMEXTPROC)getProcAddress("glCreateShaderProgramEXT");

		// GL_NV_shader_buffer_load
		glMakeBufferResidentNV = (PFNGLMAKEBUFFERRESIDENTNVPROC)getProcAddress("glMakeBufferResidentNV");
		glMakeBufferNonResidentNV = (PFNGLMAKEBUFFERNONRESIDENTNVPROC)getProcAddress("glMakeBufferNonResidentNV");
		glIsBufferResidentNV = (PFNGLISBUFFERRESIDENTNVPROC)getProcAddress("glIsBufferResidentNV");
		glMakeNamedBufferResidentNV = (PFNGLMAKENAMEDBUFFERRESIDENTNVPROC)getProcAddress("glMakeNamedBufferResidentNV");
		glMakeNamedBufferNonResidentNV = (PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC)getProcAddress("glMakeNamedBufferNonResidentNV");
		glIsNamedBufferResidentNV = (PFNGLISNAMEDBUFFERRESIDENTNVPROC)getProcAddress("glIsNamedBufferResidentNV");
		glGetBufferParameterui64vNV = (PFNGLGETBUFFERPARAMETERUI64VNVPROC)getProcAddress("glGetBufferParameterui64vNV");
		glGetNamedBufferParameterui64vNV = (PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC)getProcAddress("glGetNamedBufferParameterui64vNV");
		glGetIntegerui64vNV = (PFNGLGETINTEGERUI64VNVPROC)getProcAddress("glGetIntegerui64vNV");
		glUniformui64NV = (PFNGLUNIFORMUI64NVPROC)getProcAddress("glUniformui64NV");
		glUniformui64vNV = (PFNGLUNIFORMUI64VNVPROC)getProcAddress("glUniformui64vNV");
		glGetUniformui64vNV = (PFNGLGETUNIFORMUI64VNVPROC)getProcAddress("glGetUniformui64vNV");
		glProgramUniformui64NV = (PFNGLPROGRAMUNIFORMUI64NVPROC)getProcAddress("glProgramUniformui64NV");
		glProgramUniformui64vNV = (PFNGLPROGRAMUNIFORMUI64VNVPROC)getProcAddress("glProgramUniformui64vNV");

		// GL_NV_vertex_buffer_unified_memory
		glBufferAddressRangeNV = (PFNGLBUFFERADDRESSRANGENVPROC)getProcAddress("glBufferAddressRangeNV");
		glVertexFormatNV = (PFNGLVERTEXFORMATNVPROC)getProcAddress("glVertexFormatNV");
		glNormalFormatNV = (PFNGLNORMALFORMATNVPROC)getProcAddress("glNormalFormatNV");
		glColorFormatNV = (PFNGLCOLORFORMATNVPROC)getProcAddress("glColorFormatNV");
		glIndexFormatNV = (PFNGLINDEXFORMATNVPROC)getProcAddress("glIndexFormatNV");
		glTexCoordFormatNV = (PFNGLTEXCOORDFORMATNVPROC)getProcAddress("glTexCoordFormatNV");
		glEdgeFlagFormatNV = (PFNGLEDGEFLAGFORMATNVPROC)getProcAddress("glEdgeFlagFormatNV");
		glSecondaryColorFormatNV = (PFNGLSECONDARYCOLORFORMATNVPROC)getProcAddress("glSecondaryColorFormatNV");
		glFogCoordFormatNV = (PFNGLFOGCOORDFORMATNVPROC)getProcAddress("glFogCoordFormatNV");
		glVertexAttribFormatNV = (PFNGLVERTEXATTRIBFORMATNVPROC)getProcAddress("glVertexAttribFormatNV");
		glVertexAttribIFormatNV = (PFNGLVERTEXATTRIBIFORMATNVPROC)getProcAddress("glVertexAttribIFormatNV");
		glGetIntegerui64i_vNV = (PFNGLGETINTEGERUI64I_VNVPROC)getProcAddress("glGetIntegerui64i_vNV");

		// GL_NV_texture_barrier
		glTextureBarrierNV = (PFNGLTEXTUREBARRIERNVPROC)getProcAddress("glTextureBarrierNV");

		// GL_EXT_shader_image_load_store
		glBindImageTextureEXT = (PFNGLBINDIMAGETEXTUREEXTPROC)getProcAddress("glBindImageTextureEXT");
		glMemoryBarrierEXT = (PFNGLMEMORYBARRIEREXTPROC)getProcAddress("glMemoryBarrierEXT");

		// GL_EXT_vertex_attrib_64bit
		glVertexAttribL1dEXT = (PFNGLVERTEXATTRIBL1DEXTPROC)getProcAddress("glVertexAttribL1dEXT");
		glVertexAttribL2dEXT = (PFNGLVERTEXATTRIBL2DEXTPROC)getProcAddress("glVertexAttribL2dEXT");
		glVertexAttribL3dEXT = (PFNGLVERTEXATTRIBL3DEXTPROC)getProcAddress("glVertexAttribL3dEXT");
		glVertexAttribL4dEXT = (PFNGLVERTEXATTRIBL4DEXTPROC)getProcAddress("glVertexAttribL4dEXT");
		glVertexAttribL1dvEXT = (PFNGLVERTEXATTRIBL1DVEXTPROC)getProcAddress("glVertexAttribL1dvEXT");
		glVertexAttribL2dvEXT = (PFNGLVERTEXATTRIBL2DVEXTPROC)getProcAddress("glVertexAttribL2dvEXT");
		glVertexAttribL3dvEXT = (PFNGLVERTEXATTRIBL3DVEXTPROC)getProcAddress("glVertexAttribL3dvEXT");
		glVertexAttribL4dvEXT = (PFNGLVERTEXATTRIBL4DVEXTPROC)getProcAddress("glVertexAttribL4dvEXT");
		glVertexAttribLPointerEXT = (PFNGLVERTEXATTRIBLPOINTEREXTPROC)getProcAddress("glVertexAttribLPointerEXT");
		glGetVertexAttribLdvEXT = (PFNGLGETVERTEXATTRIBLDVEXTPROC)getProcAddress("glGetVertexAttribLdvEXT");
		glVertexArrayVertexAttribLOffsetEXT = (PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC)getProcAddress("glVertexArrayVertexAttribLOffsetEXT");

		// GL_NV_gpu_program5
		glProgramSubroutineParametersuivNV = (PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC)getProcAddress("glProgramSubroutineParametersuivNV");
		glGetProgramSubroutineParameteruivNV = (PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC)getProcAddress("glGetProgramSubroutineParameteruivNV");

		// GL_NV_gpu_shader5
		glUniform1i64NV = (PFNGLUNIFORM1I64NVPROC)getProcAddress("glUniform1i64NV");
		glUniform2i64NV = (PFNGLUNIFORM2I64NVPROC)getProcAddress("glUniform2i64NV");
		glUniform3i64NV = (PFNGLUNIFORM3I64NVPROC)getProcAddress("glUniform3i64NV");
		glUniform4i64NV = (PFNGLUNIFORM4I64NVPROC)getProcAddress("glUniform4i64NV");
		glUniform1i64vNV = (PFNGLUNIFORM1I64VNVPROC)getProcAddress("glUniform1i64vNV");
		glUniform2i64vNV = (PFNGLUNIFORM2I64VNVPROC)getProcAddress("glUniform2i64vNV");
		glUniform3i64vNV = (PFNGLUNIFORM3I64VNVPROC)getProcAddress("glUniform3i64vNV");
		glUniform4i64vNV = (PFNGLUNIFORM4I64VNVPROC)getProcAddress("glUniform4i64vNV");
		glUniform1ui64NV = (PFNGLUNIFORM1UI64NVPROC)getProcAddress("glUniform1ui64NV");
		glUniform2ui64NV = (PFNGLUNIFORM2UI64NVPROC)getProcAddress("glUniform2ui64NV");
		glUniform3ui64NV = (PFNGLUNIFORM3UI64NVPROC)getProcAddress("glUniform3ui64NV");
		glUniform4ui64NV = (PFNGLUNIFORM4UI64NVPROC)getProcAddress("glUniform4ui64NV");
		glUniform1ui64vNV = (PFNGLUNIFORM1UI64VNVPROC)getProcAddress("glUniform1ui64vNV");
		glUniform2ui64vNV = (PFNGLUNIFORM2UI64VNVPROC)getProcAddress("glUniform2ui64vNV");
		glUniform3ui64vNV = (PFNGLUNIFORM3UI64VNVPROC)getProcAddress("glUniform3ui64vNV");
		glUniform4ui64vNV = (PFNGLUNIFORM4UI64VNVPROC)getProcAddress("glUniform4ui64vNV");
		glGetUniformi64vNV = (PFNGLGETUNIFORMI64VNVPROC)getProcAddress("glGetUniformi64vNV");
		glProgramUniform1i64NV = (PFNGLPROGRAMUNIFORM1I64NVPROC)getProcAddress("glProgramUniform1i64NV");
		glProgramUniform2i64NV = (PFNGLPROGRAMUNIFORM2I64NVPROC)getProcAddress("glProgramUniform2i64NV");
		glProgramUniform3i64NV = (PFNGLPROGRAMUNIFORM3I64NVPROC)getProcAddress("glProgramUniform3i64NV");
		glProgramUniform4i64NV = (PFNGLPROGRAMUNIFORM4I64NVPROC)getProcAddress("glProgramUniform4i64NV");
		glProgramUniform1i64vNV = (PFNGLPROGRAMUNIFORM1I64VNVPROC)getProcAddress("glProgramUniform1i64vNV");
		glProgramUniform2i64vNV = (PFNGLPROGRAMUNIFORM2I64VNVPROC)getProcAddress("glProgramUniform2i64vNV");
		glProgramUniform3i64vNV = (PFNGLPROGRAMUNIFORM3I64VNVPROC)getProcAddress("glProgramUniform3i64vNV");
		glProgramUniform4i64vNV = (PFNGLPROGRAMUNIFORM4I64VNVPROC)getProcAddress("glProgramUniform4i64vNV");
		glProgramUniform1ui64NV = (PFNGLPROGRAMUNIFORM1UI64NVPROC)getProcAddress("glProgramUniform1ui64NV");
		glProgramUniform2ui64NV = (PFNGLPROGRAMUNIFORM2UI64NVPROC)getProcAddress("glProgramUniform2ui64NV");
		glProgramUniform3ui64NV = (PFNGLPROGRAMUNIFORM3UI64NVPROC)getProcAddress("glProgramUniform3ui64NV");
		glProgramUniform4ui64NV = (PFNGLPROGRAMUNIFORM4UI64NVPROC)getProcAddress("glProgramUniform4ui64NV");
		glProgramUniform1ui64vNV = (PFNGLPROGRAMUNIFORM1UI64VNVPROC)getProcAddress("glProgramUniform1ui64vNV");
		glProgramUniform2ui64vNV = (PFNGLPROGRAMUNIFORM2UI64VNVPROC)getProcAddress("glProgramUniform2ui64vNV");
		glProgramUniform3ui64vNV = (PFNGLPROGRAMUNIFORM3UI64VNVPROC)getProcAddress("glProgramUniform3ui64vNV");
		glProgramUniform4ui64vNV = (PFNGLPROGRAMUNIFORM4UI64VNVPROC)getProcAddress("glProgramUniform4ui64vNV");

		// GL_NV_vertex_attrib_integer_64bit
		glVertexAttribL1i64NV = (PFNGLVERTEXATTRIBL1I64NVPROC)getProcAddress("glVertexAttribL1i64NV");
		glVertexAttribL2i64NV = (PFNGLVERTEXATTRIBL2I64NVPROC)getProcAddress("glVertexAttribL2i64NV");
		glVertexAttribL3i64NV = (PFNGLVERTEXATTRIBL3I64NVPROC)getProcAddress("glVertexAttribL3i64NV");
		glVertexAttribL4i64NV = (PFNGLVERTEXATTRIBL4I64NVPROC)getProcAddress("glVertexAttribL4i64NV");
		glVertexAttribL1i64vNV = (PFNGLVERTEXATTRIBL1I64VNVPROC)getProcAddress("glVertexAttribL1i64vNV");
		glVertexAttribL2i64vNV = (PFNGLVERTEXATTRIBL2I64VNVPROC)getProcAddress("glVertexAttribL2i64vNV");
		glVertexAttribL3i64vNV = (PFNGLVERTEXATTRIBL3I64VNVPROC)getProcAddress("glVertexAttribL3i64vNV");
		glVertexAttribL4i64vNV = (PFNGLVERTEXATTRIBL4I64VNVPROC)getProcAddress("glVertexAttribL4i64vNV");
		glVertexAttribL1ui64NV = (PFNGLVERTEXATTRIBL1UI64NVPROC)getProcAddress("glVertexAttribL1ui64NV");
		glVertexAttribL2ui64NV = (PFNGLVERTEXATTRIBL2UI64NVPROC)getProcAddress("glVertexAttribL2ui64NV");
		glVertexAttribL3ui64NV = (PFNGLVERTEXATTRIBL3UI64NVPROC)getProcAddress("glVertexAttribL3ui64NV");
		glVertexAttribL4ui64NV = (PFNGLVERTEXATTRIBL4UI64NVPROC)getProcAddress("glVertexAttribL4ui64NV");
		glVertexAttribL1ui64vNV = (PFNGLVERTEXATTRIBL1UI64VNVPROC)getProcAddress("glVertexAttribL1ui64vNV");
		glVertexAttribL2ui64vNV = (PFNGLVERTEXATTRIBL2UI64VNVPROC)getProcAddress("glVertexAttribL2ui64vNV");
		glVertexAttribL3ui64vNV = (PFNGLVERTEXATTRIBL3UI64VNVPROC)getProcAddress("glVertexAttribL3ui64vNV");
		glVertexAttribL4ui64vNV = (PFNGLVERTEXATTRIBL4UI64VNVPROC)getProcAddress("glVertexAttribL4ui64vNV");
		glGetVertexAttribLi64vNV = (PFNGLGETVERTEXATTRIBLI64VNVPROC)getProcAddress("glGetVertexAttribLi64vNV");
		glGetVertexAttribLui64vNV = (PFNGLGETVERTEXATTRIBLUI64VNVPROC)getProcAddress("glGetVertexAttribLui64vNV");
		glVertexAttribLFormatNV = (PFNGLVERTEXATTRIBLFORMATNVPROC)getProcAddress("glVertexAttribLFormatNV");

		// GL_AMD_name_gen_delete
		glGenNamesAMD = (PFNGLGENNAMESAMDPROC)getProcAddress("glGenNamesAMD");
		glDeleteNamesAMD = (PFNGLDELETENAMESAMDPROC)getProcAddress("glDeleteNamesAMD");
		glIsNameAMD = (PFNGLISNAMEAMDPROC)getProcAddress("glIsNameAMD");

		// GL_AMD_debug_output
		glDebugMessageEnableAMD = (PFNGLDEBUGMESSAGEENABLEAMDPROC)getProcAddress("glDebugMessageEnableAMD");
		glDebugMessageInsertAMD = (PFNGLDEBUGMESSAGEINSERTAMDPROC)getProcAddress("glDebugMessageInsertAMD");
		glDebugMessageCallbackAMD = (PFNGLDEBUGMESSAGECALLBACKAMDPROC)getProcAddress("glDebugMessageCallbackAMD");
		glGetDebugMessageLogAMD = (PFNGLGETDEBUGMESSAGELOGAMDPROC)getProcAddress("glGetDebugMessageLogAMD");

		// GL_NV_vdpau_interop
		glVDPAUInitNV = (PFNGLVDPAUINITNVPROC)getProcAddress("glVDPAUInitNV");
		glVDPAUFiniNV = (PFNGLVDPAUFININVPROC)getProcAddress("glVDPAUFiniNV");
		glVDPAURegisterVideoSurfaceNV = (PFNGLVDPAUREGISTERVIDEOSURFACENVPROC)getProcAddress("glVDPAURegisterVideoSurfaceNV");
		glVDPAURegisterOutputSurfaceNV = (PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC)getProcAddress("glVDPAURegisterOutputSurfaceNV");
		glVDPAUIsSurfaceNV = (PFNGLVDPAUISSURFACENVPROC)getProcAddress("glVDPAUIsSurfaceNV");
		glVDPAUUnregisterSurfaceNV = (PFNGLVDPAUUNREGISTERSURFACENVPROC)getProcAddress("glVDPAUUnregisterSurfaceNV");
		glVDPAUGetSurfaceivNV = (PFNGLVDPAUGETSURFACEIVNVPROC)getProcAddress("glVDPAUGetSurfaceivNV");
		glVDPAUSurfaceAccessNV = (PFNGLVDPAUSURFACEACCESSNVPROC)getProcAddress("glVDPAUSurfaceAccessNV");
		glVDPAUMapSurfacesNV = (PFNGLVDPAUMAPSURFACESNVPROC)getProcAddress("glVDPAUMapSurfacesNV");
		glVDPAUUnmapSurfacesNV = (PFNGLVDPAUUNMAPSURFACESNVPROC)getProcAddress("glVDPAUUnmapSurfacesNV");

		// --- WGL ------------------------------------------------------------------

	  #if _WIN32
		// WGL_ARB_buffer_region
		wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)getProcAddress("wglCreateBufferRegionARB");
		wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)getProcAddress("wglDeleteBufferRegionARB");
		wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)getProcAddress("wglSaveBufferRegionARB");
		wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)getProcAddress("wglRestoreBufferRegionARB");

		// WGL_ARB_extensions_string
		wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)getProcAddress("wglGetExtensionsStringARB");

		// WGL_ARB_pixel_format
		wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)getProcAddress("wglGetPixelFormatAttribivARB");
		wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)getProcAddress("wglGetPixelFormatAttribfvARB");
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)getProcAddress("wglChoosePixelFormatARB");

		// WGL_ARB_make_current_read
		wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)getProcAddress("wglMakeContextCurrentARB");
		wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)getProcAddress("wglGetCurrentReadDCARB");

		// WGL_ARB_pbuffer
		wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)getProcAddress("wglCreatePbufferARB");
		wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)getProcAddress("wglGetPbufferDCARB");
		wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)getProcAddress("wglReleasePbufferDCARB");
		wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)getProcAddress("wglDestroyPbufferARB");
		wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)getProcAddress("wglQueryPbufferARB");

		// WGL_ARB_render_texture
		wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)getProcAddress("wglBindTexImageARB");
		wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)getProcAddress("wglReleaseTexImageARB");
		wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)getProcAddress("wglSetPbufferAttribARB");

		// WGL_ARB_create_context
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)getProcAddress("wglCreateContextAttribsARB");

		// WGL_EXT_display_color_table
		wglCreateDisplayColorTableEXT = (PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC)getProcAddress("wglCreateDisplayColorTableEXT");
		wglLoadDisplayColorTableEXT = (PFNWGLLOADDISPLAYCOLORTABLEEXTPROC)getProcAddress("wglLoadDisplayColorTableEXT");
		wglBindDisplayColorTableEXT = (PFNWGLBINDDISPLAYCOLORTABLEEXTPROC)getProcAddress("wglBindDisplayColorTableEXT");
		wglDestroyDisplayColorTableEXT = (PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC)getProcAddress("wglDestroyDisplayColorTableEXT");

		// WGL_EXT_extensions_string
		wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)getProcAddress("wglGetExtensionsStringEXT");

		// WGL_EXT_make_current_read
		wglMakeContextCurrentEXT = (PFNWGLMAKECONTEXTCURRENTEXTPROC)getProcAddress("wglMakeContextCurrentEXT");
		wglGetCurrentReadDCEXT = (PFNWGLGETCURRENTREADDCEXTPROC)getProcAddress("wglGetCurrentReadDCEXT");

		// WGL_EXT_pbuffer
		wglCreatePbufferEXT = (PFNWGLCREATEPBUFFEREXTPROC)getProcAddress("wglCreatePbufferEXT");
		wglGetPbufferDCEXT = (PFNWGLGETPBUFFERDCEXTPROC)getProcAddress("wglGetPbufferDCEXT");
		wglReleasePbufferDCEXT = (PFNWGLRELEASEPBUFFERDCEXTPROC)getProcAddress("wglReleasePbufferDCEXT");
		wglDestroyPbufferEXT = (PFNWGLDESTROYPBUFFEREXTPROC)getProcAddress("wglDestroyPbufferEXT");
		wglQueryPbufferEXT = (PFNWGLQUERYPBUFFEREXTPROC)getProcAddress("wglQueryPbufferEXT");

		// WGL_EXT_pixel_format
		wglGetPixelFormatAttribivEXT = (PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)getProcAddress("wglGetPixelFormatAttribivEXT");
		wglGetPixelFormatAttribfvEXT = (PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)getProcAddress("wglGetPixelFormatAttribfvEXT");
		wglChoosePixelFormatEXT = (PFNWGLCHOOSEPIXELFORMATEXTPROC)getProcAddress("wglChoosePixelFormatEXT");

		// WGL_EXT_swap_control
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)getProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)getProcAddress("wglGetSwapIntervalEXT");

		// WGL_NV_vertex_array_range
		wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)getProcAddress("wglAllocateMemoryNV");
		wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)getProcAddress("wglFreeMemoryNV");

		// WGL_OML_sync_control
		wglGetSyncValuesOML = (PFNWGLGETSYNCVALUESOMLPROC)getProcAddress("wglGetSyncValuesOML");
		wglGetMscRateOML = (PFNWGLGETMSCRATEOMLPROC)getProcAddress("wglGetMscRateOML");
		wglSwapBuffersMscOML = (PFNWGLSWAPBUFFERSMSCOMLPROC)getProcAddress("wglSwapBuffersMscOML");
		wglSwapLayerBuffersMscOML = (PFNWGLSWAPLAYERBUFFERSMSCOMLPROC)getProcAddress("wglSwapLayerBuffersMscOML");
		wglWaitForMscOML = (PFNWGLWAITFORMSCOMLPROC)getProcAddress("wglWaitForMscOML");
		wglWaitForSbcOML = (PFNWGLWAITFORSBCOMLPROC)getProcAddress("wglWaitForSbcOML");

		// WGL_I3D_digital_video_control
		wglGetDigitalVideoParametersI3D = (PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC)getProcAddress("wglGetDigitalVideoParametersI3D");
		wglSetDigitalVideoParametersI3D = (PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC)getProcAddress("wglSetDigitalVideoParametersI3D");

		// WGL_I3D_gamma
		wglGetGammaTableParametersI3D = (PFNWGLGETGAMMATABLEPARAMETERSI3DPROC)getProcAddress("wglGetGammaTableParametersI3D");
		wglSetGammaTableParametersI3D = (PFNWGLSETGAMMATABLEPARAMETERSI3DPROC)getProcAddress("wglSetGammaTableParametersI3D");
		wglGetGammaTableI3D = (PFNWGLGETGAMMATABLEI3DPROC)getProcAddress("wglGetGammaTableI3D");
		wglSetGammaTableI3D = (PFNWGLSETGAMMATABLEI3DPROC)getProcAddress("wglSetGammaTableI3D");

		// WGL_I3D_genlock
		wglEnableGenlockI3D = (PFNWGLENABLEGENLOCKI3DPROC)getProcAddress("wglEnableGenlockI3D");
		wglDisableGenlockI3D = (PFNWGLDISABLEGENLOCKI3DPROC)getProcAddress("wglDisableGenlockI3D");
		wglIsEnabledGenlockI3D = (PFNWGLISENABLEDGENLOCKI3DPROC)getProcAddress("wglIsEnabledGenlockI3D");
		wglGenlockSourceI3D = (PFNWGLGENLOCKSOURCEI3DPROC)getProcAddress("wglGenlockSourceI3D");
		wglGetGenlockSourceI3D = (PFNWGLGETGENLOCKSOURCEI3DPROC)getProcAddress("wglGetGenlockSourceI3D");
		wglGenlockSourceEdgeI3D = (PFNWGLGENLOCKSOURCEEDGEI3DPROC)getProcAddress("wglGenlockSourceEdgeI3D");
		wglGetGenlockSourceEdgeI3D = (PFNWGLGETGENLOCKSOURCEEDGEI3DPROC)getProcAddress("wglGetGenlockSourceEdgeI3D");
		wglGenlockSampleRateI3D = (PFNWGLGENLOCKSAMPLERATEI3DPROC)getProcAddress("wglGenlockSampleRateI3D");
		wglGetGenlockSampleRateI3D = (PFNWGLGETGENLOCKSAMPLERATEI3DPROC)getProcAddress("wglGetGenlockSampleRateI3D");
		wglGenlockSourceDelayI3D = (PFNWGLGENLOCKSOURCEDELAYI3DPROC)getProcAddress("wglGenlockSourceDelayI3D");
		wglGetGenlockSourceDelayI3D = (PFNWGLGETGENLOCKSOURCEDELAYI3DPROC)getProcAddress("wglGetGenlockSourceDelayI3D");
		wglQueryGenlockMaxSourceDelayI3D = (PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC)getProcAddress("wglQueryGenlockMaxSourceDelayI3D");

		// WGL_I3D_image_buffer
		wglCreateImageBufferI3D = (PFNWGLCREATEIMAGEBUFFERI3DPROC)getProcAddress("wglCreateImageBufferI3D");
		wglDestroyImageBufferI3D = (PFNWGLDESTROYIMAGEBUFFERI3DPROC)getProcAddress("wglDestroyImageBufferI3D");
		wglAssociateImageBufferEventsI3D = (PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC)getProcAddress("wglAssociateImageBufferEventsI3D");
		wglReleaseImageBufferEventsI3D = (PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC)getProcAddress("wglReleaseImageBufferEventsI3D");

		// WGL_I3D_swap_frame_lock
		wglEnableFrameLockI3D = (PFNWGLENABLEFRAMELOCKI3DPROC)getProcAddress("wglEnableFrameLockI3D");
		wglDisableFrameLockI3D = (PFNWGLDISABLEFRAMELOCKI3DPROC)getProcAddress("wglDisableFrameLockI3D");
		wglIsEnabledFrameLockI3D = (PFNWGLISENABLEDFRAMELOCKI3DPROC)getProcAddress("wglIsEnabledFrameLockI3D");
		wglQueryFrameLockMasterI3D = (PFNWGLQUERYFRAMELOCKMASTERI3DPROC)getProcAddress("wglQueryFrameLockMasterI3D");

		// WGL_I3D_swap_frame_usage
		wglGetFrameUsageI3D = (PFNWGLGETFRAMEUSAGEI3DPROC)getProcAddress("wglGetFrameUsageI3D");
		wglBeginFrameTrackingI3D = (PFNWGLBEGINFRAMETRACKINGI3DPROC)getProcAddress("wglBeginFrameTrackingI3D");
		wglEndFrameTrackingI3D = (PFNWGLENDFRAMETRACKINGI3DPROC)getProcAddress("wglEndFrameTrackingI3D");
		wglQueryFrameTrackingI3D = (PFNWGLQUERYFRAMETRACKINGI3DPROC)getProcAddress("wglQueryFrameTrackingI3D");

		// WGL_3DL_stereo_control
		wglSetStereoEmitterState3DL = (PFNWGLSETSTEREOEMITTERSTATE3DLPROC)getProcAddress("wglSetStereoEmitterState3DL");

		// WGL_NV_present_video
		wglEnumerateVideoDevicesNV = (PFNWGLENUMERATEVIDEODEVICESNVPROC)getProcAddress("wglEnumerateVideoDevicesNV");
		wglBindVideoDeviceNV = (PFNWGLBINDVIDEODEVICENVPROC)getProcAddress("wglBindVideoDeviceNV");
		wglQueryCurrentContextNV = (PFNWGLQUERYCURRENTCONTEXTNVPROC)getProcAddress("wglQueryCurrentContextNV");

		// WGL_NV_video_output
		wglGetVideoDeviceNV = (PFNWGLGETVIDEODEVICENVPROC)getProcAddress("wglGetVideoDeviceNV");
		wglReleaseVideoDeviceNV = (PFNWGLRELEASEVIDEODEVICENVPROC)getProcAddress("wglReleaseVideoDeviceNV");
		wglBindVideoImageNV = (PFNWGLBINDVIDEOIMAGENVPROC)getProcAddress("wglBindVideoImageNV");
		wglReleaseVideoImageNV = (PFNWGLRELEASEVIDEOIMAGENVPROC)getProcAddress("wglReleaseVideoImageNV");
		wglSendPbufferToVideoNV = (PFNWGLSENDPBUFFERTOVIDEONVPROC)getProcAddress("wglSendPbufferToVideoNV");
		wglGetVideoInfoNV = (PFNWGLGETVIDEOINFONVPROC)getProcAddress("wglGetVideoInfoNV");

		// WGL_NV_swap_group
		wglJoinSwapGroupNV = (PFNWGLJOINSWAPGROUPNVPROC)getProcAddress("wglJoinSwapGroupNV");
		wglBindSwapBarrierNV = (PFNWGLBINDSWAPBARRIERNVPROC)getProcAddress("wglBindSwapBarrierNV");
		wglQuerySwapGroupNV = (PFNWGLQUERYSWAPGROUPNVPROC)getProcAddress("wglQuerySwapGroupNV");
		wglQueryMaxSwapGroupsNV = (PFNWGLQUERYMAXSWAPGROUPSNVPROC)getProcAddress("wglQueryMaxSwapGroupsNV");
		wglQueryFrameCountNV = (PFNWGLQUERYFRAMECOUNTNVPROC)getProcAddress("wglQueryFrameCountNV");
		wglResetFrameCountNV = (PFNWGLRESETFRAMECOUNTNVPROC)getProcAddress("wglResetFrameCountNV");

		// WGL_NV_gpu_affinity
		wglEnumGpusNV = (PFNWGLENUMGPUSNVPROC)getProcAddress("wglEnumGpusNV");
		wglEnumGpuDevicesNV = (PFNWGLENUMGPUDEVICESNVPROC)getProcAddress("wglEnumGpuDevicesNV");
		wglCreateAffinityDCNV = (PFNWGLCREATEAFFINITYDCNVPROC)getProcAddress("wglCreateAffinityDCNV");
		wglEnumGpusFromAffinityDCNV = (PFNWGLENUMGPUSFROMAFFINITYDCNVPROC)getProcAddress("wglEnumGpusFromAffinityDCNV");
		wglDeleteDCNV = (PFNWGLDELETEDCNVPROC)getProcAddress("wglDeleteDCNV");

		// WGL_AMD_gpu_association
		wglGetGPUIDsAMD = (PFNWGLGETGPUIDSAMDPROC)getProcAddress("wglGetGPUIDsAMD");
		wglGetGPUInfoAMD = (PFNWGLGETGPUINFOAMDPROC)getProcAddress("wglGetGPUInfoAMD");
		wglGetContextGPUIDAMD = (PFNWGLGETCONTEXTGPUIDAMDPROC)getProcAddress("wglGetContextGPUIDAMD");
		wglCreateAssociatedContextAMD = (PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC)getProcAddress("wglCreateAssociatedContextAMD");
		wglCreateAssociatedContextAttribsAMD = (PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)getProcAddress("wglCreateAssociatedContextAttribsAMD");
		wglDeleteAssociatedContextAMD = (PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC)getProcAddress("wglDeleteAssociatedContextAMD");
		wglMakeAssociatedContextCurrentAMD = (PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)getProcAddress("wglMakeAssociatedContextCurrentAMD");
		wglGetCurrentAssociatedContextAMD = (PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC)getProcAddress("wglGetCurrentAssociatedContextAMD");
		wglBlitContextFramebufferAMD = (PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC)getProcAddress("wglBlitContextFramebufferAMD");

		// WGL_NV_video_capture
		wglBindVideoCaptureDeviceNV = (PFNWGLBINDVIDEOCAPTUREDEVICENVPROC)getProcAddress("wglBindVideoCaptureDeviceNV");
		wglEnumerateVideoCaptureDevicesNV = (PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC)getProcAddress("wglEnumerateVideoCaptureDevicesNV");
		wglLockVideoCaptureDeviceNV = (PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC)getProcAddress("wglLockVideoCaptureDeviceNV");
		wglQueryVideoCaptureDeviceNV = (PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC)getProcAddress("wglQueryVideoCaptureDeviceNV");
		wglReleaseVideoCaptureDeviceNV = (PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC)getProcAddress("wglReleaseVideoCaptureDeviceNV");

		// WGL_NV_copy_image
		wglCopyImageSubDataNV = (PFNWGLCOPYIMAGESUBDATANVPROC)getProcAddress("wglCopyImageSubDataNV");
	  #endif

	  #ifdef _UNIX
		// GLX_EXT_import_context
		glXGetCurrentDisplayEXT = (PFNGLXGETCURRENTDISPLAYEXTPROC)getProcAddress("glXGetCurrentDisplayEXT");
		glXQueryContextInfoEXT = (PFNGLXQUERYCONTEXTINFOEXTPROC)getProcAddress("glXQueryContextInfoEXT");
		glXGetContextIDEXT = (PFNGLXGETCONTEXTIDEXTPROC)getProcAddress("glXGetContextIDEXT");
		glXImportContextEXT = (PFNGLXIMPORTCONTEXTEXTPROC)getProcAddress("glXImportContextEXT");
		glXFreeContextEXT = (PFNGLXFREECONTEXTEXTPROC)getProcAddress("glXFreeContextEXT");

		// GLX_SGIX_fbconfig
		glXGetFBConfigAttribSGIX = (PFNGLXGETFBCONFIGATTRIBSGIXPROC)getProcAddress("glXGetFBConfigAttribSGIX");
		glXChooseFBConfigSGIX = (PFNGLXCHOOSEFBCONFIGSGIXPROC)getProcAddress("glXChooseFBConfigSGIX");
		glXCreateGLXPixmapWithConfigSGIX = (PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC)getProcAddress("glXCreateGLXPixmapWithConfigSGIX");
		glXCreateContextWithConfigSGIX = (PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC)getProcAddress("glXCreateContextWithConfigSGIX");
		glXGetVisualFromFBConfigSGIX = (PFNGLXGETVISUALFROMFBCONFIGSGIXPROC)getProcAddress("glXGetVisualFromFBConfigSGIX");
		glXGetFBConfigFromVisualSGIX = (PFNGLXGETFBCONFIGFROMVISUALSGIXPROC)getProcAddress("glXGetFBConfigFromVisualSGIX");

		// GLX_SGIX_pbuffer
		glXCreateGLXPbufferSGIX = (PFNGLXCREATEGLXPBUFFERSGIXPROC)getProcAddress("glXCreateGLXPbufferSGIX");
		glXDestroyGLXPbufferSGIX = (PFNGLXDESTROYGLXPBUFFERSGIXPROC)getProcAddress("glXDestroyGLXPbufferSGIX");
		glXQueryGLXPbufferSGIX = (PFNGLXQUERYGLXPBUFFERSGIXPROC)getProcAddress("glXQueryGLXPbufferSGIX");
		glXSelectEventSGIX = (PFNGLXSELECTEVENTSGIXPROC)getProcAddress("glXSelectEventSGIX");
		glXGetSelectedEventSGIX = (PFNGLXGETSELECTEDEVENTSGIXPROC)getProcAddress("glXGetSelectedEventSGIX");

		// GLX_SGI_cushion
		glXCushionSGI = (PFNGLXCUSHIONSGIPROC)getProcAddress("glXCushionSGI");

		// GLX_SGIX_video_resize
		glXBindChannelToWindowSGIX = (PFNGLXBINDCHANNELTOWINDOWSGIXPROC)getProcAddress("glXBindChannelToWindowSGIX");
		glXChannelRectSGIX = (PFNGLXCHANNELRECTSGIXPROC)getProcAddress("glXChannelRectSGIX");
		glXQueryChannelRectSGIX = (PFNGLXQUERYCHANNELRECTSGIXPROC)getProcAddress("glXQueryChannelRectSGIX");
		glXQueryChannelDeltasSGIX = (PFNGLXQUERYCHANNELDELTASSGIXPROC)getProcAddress("glXQueryChannelDeltasSGIX");
		glXChannelRectSyncSGIX = (PFNGLXCHANNELRECTSYNCSGIXPROC)getProcAddress("glXChannelRectSyncSGIX");

		// GLX_SGIX_dmbuffer
		glXAssociateDMPbufferSGIX = (PFNGLXASSOCIATEDMPBUFFERSGIXPROC)getProcAddress("glXAssociateDMPbufferSGIX");

		// GLX_SGIX_swap_group
		glXJoinSwapGroupSGIX = (PFNGLXJOINSWAPGROUPSGIXPROC)getProcAddress("glXJoinSwapGroupSGIX");

		// GLX_SGIX_swap_barrier
		glXBindSwapBarrierSGIX = (PFNGLXBINDSWAPBARRIERSGIXPROC)getProcAddress("glXBindSwapBarrierSGIX");
		glXQueryMaxSwapBarriersSGIX = (PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC)getProcAddress("glXQueryMaxSwapBarriersSGIX");

		// GLX_SUN_get_transparent_index
		glXGetTransparentIndexSUN = (PFNGLXGETTRANSPARENTINDEXSUNPROC)getProcAddress("glXGetTransparentIndexSUN");

		// GLX_MESA_copy_sub_buffer
		glXCopySubBufferMESA = (PFNGLXCOPYSUBBUFFERMESAPROC)getProcAddress("glXCopySubBufferMESA");

		// GLX_MESA_pixmap_colormap
		glXCreateGLXPixmapMESA = (PFNGLXCREATEGLXPIXMAPMESAPROC)getProcAddress("glXCreateGLXPixmapMESA");

		// GLX_MESA_release_buffers
		glXReleaseBuffersMESA = (PFNGLXRELEASEBUFFERSMESAPROC)getProcAddress("glXReleaseBuffersMESA");

		// GLX_MESA_set_3dfx_mode
		glXSet3DfxModeMESA = (PFNGLXSET3DFXMODEMESAPROC)getProcAddress("glXSet3DfxModeMESA");

		// GLX_OML_sync_control
		glXGetSyncValuesOML = (PFNGLXGETSYNCVALUESOMLPROC)getProcAddress("glXGetSyncValuesOML");
		glXGetMscRateOML = (PFNGLXGETMSCRATEOMLPROC)getProcAddress("glXGetMscRateOML");
		glXSwapBuffersMscOML = (PFNGLXSWAPBUFFERSMSCOMLPROC)getProcAddress("glXSwapBuffersMscOML");
		glXWaitForMscOML = (PFNGLXWAITFORMSCOMLPROC)getProcAddress("glXWaitForMscOML");
		glXWaitForSbcOML = (PFNGLXWAITFORSBCOMLPROC)getProcAddress("glXWaitForSbcOML");

		// GLX_SGIX_hyperpipe
		glXQueryHyperpipeNetworkSGIX = (PFNGLXQUERYHYPERPIPENETWORKSGIXPROC)getProcAddress("glXQueryHyperpipeNetworkSGIX");
		glXHyperpipeConfigSGIX = (PFNGLXHYPERPIPECONFIGSGIXPROC)getProcAddress("glXHyperpipeConfigSGIX");
		glXQueryHyperpipeConfigSGIX = (PFNGLXQUERYHYPERPIPECONFIGSGIXPROC)getProcAddress("glXQueryHyperpipeConfigSGIX");
		glXDestroyHyperpipeConfigSGIX = (PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC)getProcAddress("glXDestroyHyperpipeConfigSGIX");
		glXBindHyperpipeSGIX = (PFNGLXBINDHYPERPIPESGIXPROC)getProcAddress("glXBindHyperpipeSGIX");
		glXQueryHyperpipeBestAttribSGIX = (PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC)getProcAddress("glXQueryHyperpipeBestAttribSGIX");
		glXHyperpipeAttribSGIX = (PFNGLXHYPERPIPEATTRIBSGIXPROC)getProcAddress("glXHyperpipeAttribSGIX");
		glXQueryHyperpipeAttribSGIX = (PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC)getProcAddress("glXQueryHyperpipeAttribSGIX");

		// GLX_MESA_agp_offset
		glXGetAGPOffsetMESA = (PFNGLXGETAGPOFFSETMESAPROC)getProcAddress("glXGetAGPOffsetMESA");

		// GLX_EXT_texture_from_pixmap
		glXBindTexImageEXT = (PFNGLXBINDTEXIMAGEEXTPROC)getProcAddress("glXBindTexImageEXT");
		glXReleaseTexImageEXT = (PFNGLXRELEASETEXIMAGEEXTPROC)getProcAddress("glXReleaseTexImageEXT");

		// GLX_NV_present_video
		glXEnumerateVideoDevicesNV = (PFNGLXENUMERATEVIDEODEVICESNVPROC)getProcAddress("glXEnumerateVideoDevicesNV");
		glXBindVideoDeviceNV = (PFNGLXBINDVIDEODEVICENVPROC)getProcAddress("glXBindVideoDeviceNV");

		// GLX_NV_video_output
		glXGetVideoDeviceNV = (PFNGLXGETVIDEODEVICENVPROC)getProcAddress("glXGetVideoDeviceNV");
		glXReleaseVideoDeviceNV = (PFNGLXRELEASEVIDEODEVICENVPROC)getProcAddress("glXReleaseVideoDeviceNV");
		glXBindVideoImageNV = (PFNGLXBINDVIDEOIMAGENVPROC)getProcAddress("glXBindVideoImageNV");
		glXReleaseVideoImageNV = (PFNGLXRELEASEVIDEOIMAGENVPROC)getProcAddress("glXReleaseVideoImageNV");
		glXSendPbufferToVideoNV = (PFNGLXSENDPBUFFERTOVIDEONVPROC)getProcAddress("glXSendPbufferToVideoNV");
		glXGetVideoInfoNV = (PFNGLXGETVIDEOINFONVPROC)getProcAddress("glXGetVideoInfoNV");

		// GLX_NV_swap_group
		glXJoinSwapGroupNV = (PFNGLXJOINSWAPGROUPNVPROC)getProcAddress("glXJoinSwapGroupNV");
		glXBindSwapBarrierNV = (PFNGLXBINDSWAPBARRIERNVPROC)getProcAddress("glXBindSwapBarrierNV");
		glXQuerySwapGroupNV = (PFNGLXQUERYSWAPGROUPNVPROC)getProcAddress("glXQuerySwapGroupNV");
		glXQueryMaxSwapGroupsNV = (PFNGLXQUERYMAXSWAPGROUPSNVPROC)getProcAddress("glXQueryMaxSwapGroupsNV");
		glXQueryFrameCountNV = (PFNGLXQUERYFRAMECOUNTNVPROC)getProcAddress("glXQueryFrameCountNV");
		glXResetFrameCountNV = (PFNGLXRESETFRAMECOUNTNVPROC)getProcAddress("glXResetFrameCountNV");

		// GLX_NV_video_capture
		glXBindVideoCaptureDeviceNV = (PFNGLXBINDVIDEOCAPTUREDEVICENVPROC)getProcAddress("glXBindVideoCaptureDeviceNV");
		glXEnumerateVideoCaptureDevicesNV = (PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC)getProcAddress("glXEnumerateVideoCaptureDevicesNV");
		glXLockVideoCaptureDeviceNV = (PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC)getProcAddress("glXLockVideoCaptureDeviceNV");
		glXQueryVideoCaptureDeviceNV = (PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC)getProcAddress("glXQueryVideoCaptureDeviceNV");
		glXReleaseVideoCaptureDeviceNV = (PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC)getProcAddress("glXReleaseVideoCaptureDeviceNV");

		// GLX_EXT_swap_control
		glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)getProcAddress("glXSwapIntervalEXT");

		// GLX_NV_copy_image
		glXCopyImageSubDataNV = (PFNGLXCOPYIMAGESUBDATANVPROC)getProcAddress("glXCopyImageSubDataNV");
	  #endif
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

  // --- WGL ------------------------------------------------------------------

#if _WIN32
  // WGL_ARB_buffer_region
  PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegionARB = NULL;
  PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegionARB = NULL;
  PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegionARB = NULL;
  PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB = NULL;

  // WGL_ARB_extensions_string
  PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;

  // WGL_ARB_pixel_format
  PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = NULL;
  PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = NULL;
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;

  // WGL_ARB_make_current_read
  PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB = NULL;
  PFNWGLGETCURRENTREADDCARBPROC wglGetCurrentReadDCARB = NULL;

  // WGL_ARB_pbuffer
  PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = NULL;
  PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = NULL;
  PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = NULL;
  PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB = NULL;
  PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = NULL;

  // WGL_ARB_render_texture
  PFNWGLBINDTEXIMAGEARBPROC wglBindTexImageARB = NULL;
  PFNWGLRELEASETEXIMAGEARBPROC wglReleaseTexImageARB = NULL;
  PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB = NULL;

  // WGL_ARB_create_context
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

  // WGL_EXT_display_color_table
  PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC wglCreateDisplayColorTableEXT = NULL;
  PFNWGLLOADDISPLAYCOLORTABLEEXTPROC wglLoadDisplayColorTableEXT = NULL;
  PFNWGLBINDDISPLAYCOLORTABLEEXTPROC wglBindDisplayColorTableEXT = NULL;
  PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC wglDestroyDisplayColorTableEXT = NULL;

  // WGL_EXT_extensions_string
  PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = NULL;

  // WGL_EXT_make_current_read
  PFNWGLMAKECONTEXTCURRENTEXTPROC wglMakeContextCurrentEXT = NULL;
  PFNWGLGETCURRENTREADDCEXTPROC wglGetCurrentReadDCEXT = NULL;

  // WGL_EXT_pbuffer
  PFNWGLCREATEPBUFFEREXTPROC wglCreatePbufferEXT = NULL;
  PFNWGLGETPBUFFERDCEXTPROC wglGetPbufferDCEXT = NULL;
  PFNWGLRELEASEPBUFFERDCEXTPROC wglReleasePbufferDCEXT = NULL;
  PFNWGLDESTROYPBUFFEREXTPROC wglDestroyPbufferEXT = NULL;
  PFNWGLQUERYPBUFFEREXTPROC wglQueryPbufferEXT = NULL;

  // WGL_EXT_pixel_format
  PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivEXT = NULL;
  PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvEXT = NULL;
  PFNWGLCHOOSEPIXELFORMATEXTPROC wglChoosePixelFormatEXT = NULL;

  // WGL_EXT_swap_control
  PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
  PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = NULL;

  // WGL_NV_vertex_array_range
  PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV = NULL;
  PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV = NULL;

  // WGL_OML_sync_control
  PFNWGLGETSYNCVALUESOMLPROC wglGetSyncValuesOML = NULL;
  PFNWGLGETMSCRATEOMLPROC wglGetMscRateOML = NULL;
  PFNWGLSWAPBUFFERSMSCOMLPROC wglSwapBuffersMscOML = NULL;
  PFNWGLSWAPLAYERBUFFERSMSCOMLPROC wglSwapLayerBuffersMscOML = NULL;
  PFNWGLWAITFORMSCOMLPROC wglWaitForMscOML = NULL;
  PFNWGLWAITFORSBCOMLPROC wglWaitForSbcOML = NULL;

  // WGL_I3D_digital_video_control
  PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC wglGetDigitalVideoParametersI3D = NULL;
  PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC wglSetDigitalVideoParametersI3D = NULL;

  // WGL_I3D_gamma
  PFNWGLGETGAMMATABLEPARAMETERSI3DPROC wglGetGammaTableParametersI3D = NULL;
  PFNWGLSETGAMMATABLEPARAMETERSI3DPROC wglSetGammaTableParametersI3D = NULL;
  PFNWGLGETGAMMATABLEI3DPROC wglGetGammaTableI3D = NULL;
  PFNWGLSETGAMMATABLEI3DPROC wglSetGammaTableI3D = NULL;

  // WGL_I3D_genlock
  PFNWGLENABLEGENLOCKI3DPROC wglEnableGenlockI3D = NULL;
  PFNWGLDISABLEGENLOCKI3DPROC wglDisableGenlockI3D = NULL;
  PFNWGLISENABLEDGENLOCKI3DPROC wglIsEnabledGenlockI3D = NULL;
  PFNWGLGENLOCKSOURCEI3DPROC wglGenlockSourceI3D = NULL;
  PFNWGLGETGENLOCKSOURCEI3DPROC wglGetGenlockSourceI3D = NULL;
  PFNWGLGENLOCKSOURCEEDGEI3DPROC wglGenlockSourceEdgeI3D = NULL;
  PFNWGLGETGENLOCKSOURCEEDGEI3DPROC wglGetGenlockSourceEdgeI3D = NULL;
  PFNWGLGENLOCKSAMPLERATEI3DPROC wglGenlockSampleRateI3D = NULL;
  PFNWGLGETGENLOCKSAMPLERATEI3DPROC wglGetGenlockSampleRateI3D = NULL;
  PFNWGLGENLOCKSOURCEDELAYI3DPROC wglGenlockSourceDelayI3D = NULL;
  PFNWGLGETGENLOCKSOURCEDELAYI3DPROC wglGetGenlockSourceDelayI3D = NULL;
  PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC wglQueryGenlockMaxSourceDelayI3D = NULL;

  // WGL_I3D_image_buffer
  PFNWGLCREATEIMAGEBUFFERI3DPROC wglCreateImageBufferI3D = NULL;
  PFNWGLDESTROYIMAGEBUFFERI3DPROC wglDestroyImageBufferI3D = NULL;
  PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC wglAssociateImageBufferEventsI3D = NULL;
  PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC wglReleaseImageBufferEventsI3D = NULL;

  // WGL_I3D_swap_frame_lock
  PFNWGLENABLEFRAMELOCKI3DPROC wglEnableFrameLockI3D = NULL;
  PFNWGLDISABLEFRAMELOCKI3DPROC wglDisableFrameLockI3D = NULL;
  PFNWGLISENABLEDFRAMELOCKI3DPROC wglIsEnabledFrameLockI3D = NULL;
  PFNWGLQUERYFRAMELOCKMASTERI3DPROC wglQueryFrameLockMasterI3D = NULL;

  // WGL_I3D_swap_frame_usage
  PFNWGLGETFRAMEUSAGEI3DPROC wglGetFrameUsageI3D = NULL;
  PFNWGLBEGINFRAMETRACKINGI3DPROC wglBeginFrameTrackingI3D = NULL;
  PFNWGLENDFRAMETRACKINGI3DPROC wglEndFrameTrackingI3D = NULL;
  PFNWGLQUERYFRAMETRACKINGI3DPROC wglQueryFrameTrackingI3D = NULL;

  // WGL_3DL_stereo_control
  PFNWGLSETSTEREOEMITTERSTATE3DLPROC wglSetStereoEmitterState3DL = NULL;

  // WGL_NV_present_video
  PFNWGLENUMERATEVIDEODEVICESNVPROC wglEnumerateVideoDevicesNV = NULL;
  PFNWGLBINDVIDEODEVICENVPROC wglBindVideoDeviceNV = NULL;
  PFNWGLQUERYCURRENTCONTEXTNVPROC wglQueryCurrentContextNV = NULL;

  // WGL_NV_video_output
  PFNWGLGETVIDEODEVICENVPROC wglGetVideoDeviceNV = NULL;
  PFNWGLRELEASEVIDEODEVICENVPROC wglReleaseVideoDeviceNV = NULL;
  PFNWGLBINDVIDEOIMAGENVPROC wglBindVideoImageNV = NULL;
  PFNWGLRELEASEVIDEOIMAGENVPROC wglReleaseVideoImageNV = NULL;
  PFNWGLSENDPBUFFERTOVIDEONVPROC wglSendPbufferToVideoNV = NULL;
  PFNWGLGETVIDEOINFONVPROC wglGetVideoInfoNV = NULL;

  // WGL_NV_swap_group
  PFNWGLJOINSWAPGROUPNVPROC wglJoinSwapGroupNV = NULL;
  PFNWGLBINDSWAPBARRIERNVPROC wglBindSwapBarrierNV = NULL;
  PFNWGLQUERYSWAPGROUPNVPROC wglQuerySwapGroupNV = NULL;
  PFNWGLQUERYMAXSWAPGROUPSNVPROC wglQueryMaxSwapGroupsNV = NULL;
  PFNWGLQUERYFRAMECOUNTNVPROC wglQueryFrameCountNV = NULL;
  PFNWGLRESETFRAMECOUNTNVPROC wglResetFrameCountNV = NULL;

  // WGL_NV_gpu_affinity
  PFNWGLENUMGPUSNVPROC wglEnumGpusNV = NULL;
  PFNWGLENUMGPUDEVICESNVPROC wglEnumGpuDevicesNV = NULL;
  PFNWGLCREATEAFFINITYDCNVPROC wglCreateAffinityDCNV = NULL;
  PFNWGLENUMGPUSFROMAFFINITYDCNVPROC wglEnumGpusFromAffinityDCNV = NULL;
  PFNWGLDELETEDCNVPROC wglDeleteDCNV = NULL;

  // WGL_AMD_gpu_association
  PFNWGLGETGPUIDSAMDPROC wglGetGPUIDsAMD = NULL;
  PFNWGLGETGPUINFOAMDPROC wglGetGPUInfoAMD = NULL;
  PFNWGLGETCONTEXTGPUIDAMDPROC wglGetContextGPUIDAMD = NULL;
  PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC wglCreateAssociatedContextAMD = NULL;
  PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC wglCreateAssociatedContextAttribsAMD = NULL;
  PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC wglDeleteAssociatedContextAMD = NULL;
  PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC wglMakeAssociatedContextCurrentAMD = NULL;
  PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC wglGetCurrentAssociatedContextAMD = NULL;
  PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC wglBlitContextFramebufferAMD = NULL;

  // WGL_NV_video_capture
  PFNWGLBINDVIDEOCAPTUREDEVICENVPROC wglBindVideoCaptureDeviceNV = NULL;
  PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC wglEnumerateVideoCaptureDevicesNV = NULL;
  PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC wglLockVideoCaptureDeviceNV = NULL;
  PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC wglQueryVideoCaptureDeviceNV = NULL;
  PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC wglReleaseVideoCaptureDeviceNV = NULL;

  // WGL_NV_copy_image
  PFNWGLCOPYIMAGESUBDATANVPROC wglCopyImageSubDataNV = NULL;
#endif

#ifdef _UNIX
  // GLX_EXT_import_context
  PFNGLXGETCURRENTDISPLAYEXTPROC glXGetCurrentDisplayEXT = NULL;
  PFNGLXQUERYCONTEXTINFOEXTPROC glXQueryContextInfoEXT = NULL;
  PFNGLXGETCONTEXTIDEXTPROC glXGetContextIDEXT = NULL;
  PFNGLXIMPORTCONTEXTEXTPROC glXImportContextEXT = NULL;
  PFNGLXFREECONTEXTEXTPROC glXFreeContextEXT = NULL;

  // GLX_SGIX_fbconfig
  PFNGLXGETFBCONFIGATTRIBSGIXPROC glXGetFBConfigAttribSGIX = NULL;
  PFNGLXCHOOSEFBCONFIGSGIXPROC glXChooseFBConfigSGIX = NULL;
  PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC glXCreateGLXPixmapWithConfigSGIX = NULL;
  PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC glXCreateContextWithConfigSGIX = NULL;
  PFNGLXGETVISUALFROMFBCONFIGSGIXPROC glXGetVisualFromFBConfigSGIX = NULL;
  PFNGLXGETFBCONFIGFROMVISUALSGIXPROC glXGetFBConfigFromVisualSGIX = NULL;

  // GLX_SGIX_pbuffer
  PFNGLXCREATEGLXPBUFFERSGIXPROC glXCreateGLXPbufferSGIX = NULL;
  PFNGLXDESTROYGLXPBUFFERSGIXPROC glXDestroyGLXPbufferSGIX = NULL;
  PFNGLXQUERYGLXPBUFFERSGIXPROC glXQueryGLXPbufferSGIX = NULL;
  PFNGLXSELECTEVENTSGIXPROC glXSelectEventSGIX = NULL;
  PFNGLXGETSELECTEDEVENTSGIXPROC glXGetSelectedEventSGIX = NULL;

  // GLX_SGI_cushion
  PFNGLXCUSHIONSGIPROC glXCushionSGI = NULL;

  // GLX_SGIX_video_resize
  PFNGLXBINDCHANNELTOWINDOWSGIXPROC glXBindChannelToWindowSGIX = NULL;
  PFNGLXCHANNELRECTSGIXPROC glXChannelRectSGIX = NULL;
  PFNGLXQUERYCHANNELRECTSGIXPROC glXQueryChannelRectSGIX = NULL;
  PFNGLXQUERYCHANNELDELTASSGIXPROC glXQueryChannelDeltasSGIX = NULL;
  PFNGLXCHANNELRECTSYNCSGIXPROC glXChannelRectSyncSGIX = NULL;

  // GLX_SGIX_dmbuffer
  PFNGLXASSOCIATEDMPBUFFERSGIXPROC glXAssociateDMPbufferSGIX = NULL;

  // GLX_SGIX_swap_group
  PFNGLXJOINSWAPGROUPSGIXPROC glXJoinSwapGroupSGIX = NULL;

  // GLX_SGIX_swap_barrier
  PFNGLXBINDSWAPBARRIERSGIXPROC glXBindSwapBarrierSGIX = NULL;
  PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC glXQueryMaxSwapBarriersSGIX = NULL;

  // GLX_SUN_get_transparent_index
  PFNGLXGETTRANSPARENTINDEXSUNPROC glXGetTransparentIndexSUN = NULL;

  // GLX_MESA_copy_sub_buffer
  PFNGLXCOPYSUBBUFFERMESAPROC glXCopySubBufferMESA = NULL;

  // GLX_MESA_pixmap_colormap
  PFNGLXCREATEGLXPIXMAPMESAPROC glXCreateGLXPixmapMESA = NULL;

  // GLX_MESA_release_buffers
  PFNGLXRELEASEBUFFERSMESAPROC glXReleaseBuffersMESA = NULL;

  // GLX_MESA_set_3dfx_mode
  PFNGLXSET3DFXMODEMESAPROC glXSet3DfxModeMESA = NULL;

  // GLX_OML_sync_control
  PFNGLXGETSYNCVALUESOMLPROC glXGetSyncValuesOML = NULL;
  PFNGLXGETMSCRATEOMLPROC glXGetMscRateOML = NULL;
  PFNGLXSWAPBUFFERSMSCOMLPROC glXSwapBuffersMscOML = NULL;
  PFNGLXWAITFORMSCOMLPROC glXWaitForMscOML = NULL;
  PFNGLXWAITFORSBCOMLPROC glXWaitForSbcOML = NULL;

  // GLX_SGIX_hyperpipe
  PFNGLXQUERYHYPERPIPENETWORKSGIXPROC glXQueryHyperpipeNetworkSGIX = NULL;
  PFNGLXHYPERPIPECONFIGSGIXPROC glXHyperpipeConfigSGIX = NULL;
  PFNGLXQUERYHYPERPIPECONFIGSGIXPROC glXQueryHyperpipeConfigSGIX = NULL;
  PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC glXDestroyHyperpipeConfigSGIX = NULL;
  PFNGLXBINDHYPERPIPESGIXPROC glXBindHyperpipeSGIX = NULL;
  PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC glXQueryHyperpipeBestAttribSGIX = NULL;
  PFNGLXHYPERPIPEATTRIBSGIXPROC glXHyperpipeAttribSGIX = NULL;
  PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC glXQueryHyperpipeAttribSGIX = NULL;

  // GLX_MESA_agp_offset
  PFNGLXGETAGPOFFSETMESAPROC glXGetAGPOffsetMESA = NULL;

  // GLX_EXT_texture_from_pixmap
  PFNGLXBINDTEXIMAGEEXTPROC glXBindTexImageEXT = NULL;
  PFNGLXRELEASETEXIMAGEEXTPROC glXReleaseTexImageEXT = NULL;

  // GLX_NV_present_video
  PFNGLXENUMERATEVIDEODEVICESNVPROC glXEnumerateVideoDevicesNV = NULL;
  PFNGLXBINDVIDEODEVICENVPROC glXBindVideoDeviceNV = NULL;

  // GLX_NV_video_output
  PFNGLXGETVIDEODEVICENVPROC glXGetVideoDeviceNV = NULL;
  PFNGLXRELEASEVIDEODEVICENVPROC glXReleaseVideoDeviceNV = NULL;
  PFNGLXBINDVIDEOIMAGENVPROC glXBindVideoImageNV = NULL;
  PFNGLXRELEASEVIDEOIMAGENVPROC glXReleaseVideoImageNV = NULL;
  PFNGLXSENDPBUFFERTOVIDEONVPROC glXSendPbufferToVideoNV = NULL;
  PFNGLXGETVIDEOINFONVPROC glXGetVideoInfoNV = NULL;

    // GLX_NV_swap_group
  PFNGLXJOINSWAPGROUPNVPROC glXJoinSwapGroupNV = NULL;
  PFNGLXBINDSWAPBARRIERNVPROC glXBindSwapBarrierNV = NULL;
  PFNGLXQUERYSWAPGROUPNVPROC glXQuerySwapGroupNV = NULL;
  PFNGLXQUERYMAXSWAPGROUPSNVPROC glXQueryMaxSwapGroupsNV = NULL;
  PFNGLXQUERYFRAMECOUNTNVPROC glXQueryFrameCountNV = NULL;
  PFNGLXRESETFRAMECOUNTNVPROC glXResetFrameCountNV = NULL;

    // GLX_NV_video_capture
  PFNGLXBINDVIDEOCAPTUREDEVICENVPROC glXBindVideoCaptureDeviceNV = NULL;
  PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC glXEnumerateVideoCaptureDevicesNV = NULL;
  PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC glXLockVideoCaptureDeviceNV = NULL;
  PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC glXQueryVideoCaptureDeviceNV = NULL;
  PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC glXReleaseVideoCaptureDeviceNV = NULL;

    // GLX_EXT_swap_control
  PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL;

    // GLX_NV_copy_image
  PFNGLXCOPYIMAGESUBDATANVPROC glXCopyImageSubDataNV = NULL;
#endif
}
