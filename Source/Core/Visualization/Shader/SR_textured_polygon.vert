/*****************************************************************************/
/**
 *  @file   polygon.vert
 *  @author Jun Nishimura, Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#version 120
#include "qualifire.h"


// Input parameters.
VertIn vec2 random_index; // index for accessing to the random texture
VertIn vec2 texture_2d_coord; // 2d texture coords

// Output parameters to fragment shader.
VertOut vec3 position; // vertex position in camera coordinate
VertOut vec3 normal; // normal vector in camera coodinate
VertOut vec2 index; // index for accessing to the random texture
VertOut vec2 uv_coords; // uv coords for 2d texture

// Uniform parameter.
uniform float polygon_offset; // polygon offset in clip coordinate

// Uniform variables (OpenGL variables).
uniform mat4 ModelViewMatrix; // model-view matrix
uniform mat4 ModelViewProjectionMatrix; // model-view projection matrix
uniform mat3 NormalMatrix; // normal matrix


/*===========================================================================*/
/**
 *  @brief  Main function of vertex shader.
 */
/*===========================================================================*/
void main()
{
    gl_Position = ModelViewProjectionMatrix * gl_Vertex;
    gl_Position.z -= polygon_offset;
    gl_FrontColor = gl_Color;

    position = ( ModelViewMatrix * gl_Vertex ).xyz;
    normal = NormalMatrix * gl_Normal;
    index = random_index;
    uv_coords = texture_2d_coord;
}