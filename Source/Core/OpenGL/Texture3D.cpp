/****************************************************************************/
/**
 *  @file   Texture3D.cpp
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#include "Texture3D.h"
#include <kvs/Math>
#include <kvs/OpenGL>
#include <kvs/Assert>
#include <iostream>


namespace kvs
{

/*===========================================================================*/
/**
 *  @brief  Unbinds 3D textures.
 */
/*===========================================================================*/
void Texture3D::Unbind()
{
    kvs::Texture::Unbind( GL_TEXTURE_3D );
}

/*==========================================================================*/
/**
 *  @brief  Create the texture.
 */
/*==========================================================================*/
void Texture3D::create( const size_t width, const size_t height, const size_t depth, const void* data )
{
    KVS_ASSERT( width > 0 );
    KVS_ASSERT( static_cast<GLint>( width ) <= kvs::OpenGL::MaxTextureSize() );
    KVS_ASSERT( height > 0 );
    KVS_ASSERT( static_cast<GLint>( height ) <= kvs::OpenGL::MaxTextureSize() );
    KVS_ASSERT( depth > 0 );
    KVS_ASSERT( static_cast<GLint>( depth ) <= kvs::OpenGL::MaxTextureSize() );

    BaseClass::createID();
    BaseClass::setSize( width, height, depth );
    BaseClass::GuardedBinder binder( *this );
    BaseClass::setParameter( GL_TEXTURE_MAG_FILTER, BaseClass::magFilter() );
    BaseClass::setParameter( GL_TEXTURE_MIN_FILTER, BaseClass::minFilter() );
    BaseClass::setParameter( GL_TEXTURE_WRAP_S, BaseClass::wrapS() );
    BaseClass::setParameter( GL_TEXTURE_WRAP_T, BaseClass::wrapT() );
    BaseClass::setParameter( GL_TEXTURE_WRAP_R, BaseClass::wrapR() );
    this->load( width, height, depth, data );
}

/*==========================================================================*/
/**
 *  @brief  Release the texture.
 */
/*==========================================================================*/
void Texture3D::release()
{
    BaseClass::deleteID();
    m_is_loaded = false;
}

/*==========================================================================*/
/**
 *  @brief  Load the texture data to the GPU.
 *  @param  width  [in] texture width
 *  @param  height [in] texture height
 *  @param  depth  [in] texture depth
 *  @param  data [in] pointer to the pixel data
 *  @param  xoffset [in] texel offset in the x direction within the pixel data
 *  @param  yoffset [in] texel offset in the y direction within the pixel data
 *  @param  zoffset [in] texel offset in the z direction within the pixel data
 */
/*==========================================================================*/
void Texture3D::load(
    const size_t width,
    const size_t height,
    const size_t depth,
    const void*  data,
    const size_t xoffset,
    const size_t yoffset,
    const size_t zoffset )
{
    const GLint swap = kvs::OpenGL::Integer( GL_UNPACK_SWAP_BYTES );
    const GLint alignment = kvs::OpenGL::Integer( GL_UNPACK_ALIGNMENT );
    BaseClass::setPixelStorageMode( GL_UNPACK_SWAP_BYTES, swap ? GL_TRUE : GL_FALSE );
    BaseClass::setPixelStorageMode( GL_UNPACK_ALIGNMENT, 1 );

    if ( !m_is_loaded )
    {
        BaseClass::setImage3D( width, height, depth, data );
        m_is_loaded = true;
    }
    else
    {
        BaseClass::setSubImage3D( width, height, depth, data, xoffset, yoffset, zoffset );
    }

    BaseClass::setPixelStorageMode( GL_UNPACK_SWAP_BYTES, swap );
    BaseClass::setPixelStorageMode( GL_UNPACK_ALIGNMENT, alignment );
}

/*==========================================================================*/
/**
 *  @brief  Load texture data to from frame buffer.
 *  @param  x [in] x position of the left corner of the row of pixels
 *  @param  y [in] y position of the left corner of the row of pixels
 *  @param  width [in] texture width
 *  @param  height [in] texture height
 *  @param  depth [in] texture depth (not used)
 *  @param  xoffset [in] texel offset in the x-direction within the pixel data
 *  @param  yoffset [in] texel offset in the y-direction within the pixel data
 *  @param  zoffset [in] texel offset in the z-direction within the pixel data
 */
/*==========================================================================*/
void Texture3D::loadFromFrameBuffer(
    const int x,
    const int y,
    const size_t width,
    const size_t height,
    const size_t depth,
    const size_t xoffset,
    const size_t yoffset,
    const size_t zoffset )
{
    KVS_ASSERT( this->isLoaded() );
//    const GLint swap = kvs::OpenGL::Integer( GL_UNPACK_SWAP_BYTES );
//    const GLint alignment = kvs::OpenGL::Integer( GL_UNPACK_ALIGNMENT );
//    BaseClass::setPixelStorageMode( GL_UNPACK_SWAP_BYTES, swap ? GL_TRUE : GL_FALSE );
//    BaseClass::setPixelStorageMode( GL_UNPACK_ALIGNMENT, 1 );

    BaseClass::copySubImage3D( x, y, width, height, xoffset, yoffset, zoffset );

//    BaseClass::setPixelStorageMode( GL_UNPACK_SWAP_BYTES, swap );
//    BaseClass::setPixelStorageMode( GL_UNPACK_ALIGNMENT, alignment );
}

} // end of namespace kvs
