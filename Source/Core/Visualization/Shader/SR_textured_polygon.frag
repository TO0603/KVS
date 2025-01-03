/*****************************************************************************/
/**
 *  @file   polygon.frag
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
#include "shading.h"
#include "qualifire.h"
#include "texture.h"


// Input parameters from vertex shader
FragIn vec3 position; // vertex position in camera coordinate
FragIn vec3 normal; // normal vector in camera coodinate
FragIn vec2 index; // index for accessing to the random texture
FragIn vec2 uv_coords; // uv coords for 2d texture

//Shading Types.
uniform bool  is_lambert_shading;
uniform bool  is_phong_shading;
uniform bool  is_blinn_phong_shading;

// Uniform parameters.
uniform sampler2D random_texture; // random texture to generate random number
uniform sampler2D material_texture;
uniform float random_texture_size_inv; // reciprocal value of the random texture size
uniform vec2 random_offset; // offset values for accessing to the random texture
uniform ShadingParameter shading; // shading parameters


/*===========================================================================*/
/**
 *  @brief  Returns random index.
 *  @param  p [in] pixel coordinate value
 *  @return random index as 2d vector
 */
/*===========================================================================*/
vec2 RandomIndex( in vec2 p )
{
    float x = float( int( index.x ) * 73 );
    float y = float( int( index.y ) * 31 );
    return ( vec2( x, y ) + random_offset + p ) * random_texture_size_inv;
}

/*===========================================================================*/
/**
 *  @brief  Main function of fragment shader.
 */
/*===========================================================================*/
void main()
{
    vec4 colorRGBA = texture2D(material_texture, uv_coords);
    vec3 color = colorRGBA.rgb;
    float alpha = colorRGBA.a;
    if ( alpha == 0.0 ) { discard; return; }

    // Stochastic color assignment.
    float R = LookupTexture2D( random_texture, RandomIndex( gl_FragCoord.xy ) ).a;
    if ( R > alpha ) { discard; return; }

    // Light position in camera coordinate.
    vec3 light_position = gl_LightSource[0].position.xyz;

    // Light vector (L) and Normal vector (N) in camera coordinate.
    vec3 L = normalize( light_position - position );
    vec3 N = normalize( normal );

    // Shading.
    vec3 shaded_color;
    if ( is_lambert_shading )
    {
        shaded_color = ShadingLambert( shading, color, L, N );
    }
    else if ( is_phong_shading )
    {
        vec3 V = normalize ( -position );
        shaded_color = ShadingPhong( shading, color, L, N, V );
    }
    else if ( is_blinn_phong_shading )
    {
        vec3 V = normalize ( -position );
        shaded_color = ShadingBlinnPhong( shading, color, L, N, V );
    }
    else
    {
        shaded_color = ShadingNone( shading, color );
    }

    gl_FragColor = vec4( shaded_color, 1.0 );
}
