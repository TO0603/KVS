/*****************************************************************************/
/**
 *  @file   ScreenBase.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "ScreenBase.h"
#include <kvs/ColorImage>
#include <kvs/OpenGL>
#include <cstdio>
#include <cfenv>


namespace
{

template <typename T>
inline void Flip( T* data, const size_t width, const size_t height, const size_t ncomps )
{
    // NOTE: Gallium softpipe driver doesn't support "upside-down" rendering
    // which would be needed for the OSMESA_Y_UP=TRUE case. Therefore, the
    // rendering pixel data read backed from GPU with glReadPixels need to be
    // flipped. In the current implementation, it is necessary to specify the
    // gallium driver "softpipe" or "llvmpipe" by environment parameter
    // 'KVS_OSMESA_Y_FLIP'. In case that the none-zero value is specified for
    // this parameter, the data will be flipped.
    // e.g.) export KVS_OSMESA_Y_FLIP=1   (flipped) default
    //       export KVS_OSMESA_Y_FLIP=0   (not flipped)
    //
    bool y_flip = true;
    const char* KVS_OSMESA_Y_FLIP( std::getenv( "KVS_OSMESA_Y_FLIP" ) );
    if ( KVS_OSMESA_Y_FLIP != nullptr )
    {
        if ( std::atoi( KVS_OSMESA_Y_FLIP ) == 0 )
        {
            y_flip = false;
        }
    }

    if ( y_flip )
    {
        const size_t stride = width * ncomps;

        T* pdata = data;
        const size_t end_line = height / 2;
        for ( size_t i = 0; i < end_line; i++ )
        {
            T* src = pdata + ( i * stride );
            T* dst = pdata + ( ( height - i - 1 ) * stride );
            for ( size_t j = 0; j < stride; j++ )
            {
                std::swap( *src, *dst );
                src++; dst++;
            }
        }
    }
}

}


namespace kvs
{

namespace osmesa
{

kvs::ValueArray<kvs::UInt8> ScreenBase::readbackColorBuffer( GLenum mode ) const
{
    kvs::OpenGL::SetReadBuffer( mode );
    kvs::OpenGL::SetPixelStorageMode( GL_PACK_ALIGNMENT, GLint(4) );

    const size_t width = this->width();
    const size_t height = this->height();
    kvs::ValueArray<kvs::UInt8> buffer( width * height * 4 );
    kvs::OpenGL::ReadPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data() );
    ::Flip( buffer.data(), width, height, 4 );

    return buffer;
}

kvs::ValueArray<kvs::Real32> ScreenBase::readbackDepthBuffer( GLenum mode ) const
{
    kvs::OpenGL::SetReadBuffer( mode );
    kvs::OpenGL::SetPixelStorageMode( GL_PACK_ALIGNMENT, GLint(4) );

    const size_t width = this->width();
    const size_t height = this->height();
    kvs::ValueArray<kvs::Real32> buffer( width * height );
    kvs::OpenGL::ReadPixels( 0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, buffer.data() );
    ::Flip( buffer.data(), width, height, 1 );

    return buffer;
}

void ScreenBase::create()
{
    // Create OSMesa context
    const GLenum format = OSMESA_RGBA;
    const GLint depth_bits = 24;
    const GLint stencil_bits = 0;
    const GLint accum_bits = 0;
    m_context.create( format, depth_bits, stencil_bits, accum_bits );
    if ( !m_context.isValid() )
    {
        kvsMessageError() << "Cannot create OSMesa context." << std::endl;
        return;
    }

    // Create OSMesa drawing surface
    const GLsizei width = GLsizei( BaseClass::width() );
    const GLsizei height = GLsizei( BaseClass::height() );
    m_surface.create( width, height, format );

    // Bind the context to the surface
    if ( !m_context.makeCurrent( m_surface ) )
    {
        kvsMessageError() << "Cannot bind OSMesa context." << std::endl;
        return;
    }

    // Y coordinates increase downward
    kvs::osmesa::Context::SetYAxisDirectionToDown();

    this->initializeEvent();
}

void ScreenBase::show()
{
    if ( !m_context.isValid() ) { this->create(); }
}

void ScreenBase::redraw()
{
    this->draw();
}

void ScreenBase::draw()
{
    if ( !m_context.isValid() ) { this->create(); }

    fenv_t fe;
    std::feholdexcept( &fe );
    this->paintEvent();
    std::feupdateenv( &fe );
}

kvs::ColorImage ScreenBase::capture() const
{
    const size_t width = BaseClass::width();
    const size_t height = BaseClass::height();

    // RGBA to RGB
    const kvs::ValueArray<kvs::UInt8>& buffer = m_surface.buffer();
    kvs::ValueArray<kvs::UInt8> pixels( width * height * 3 );
    for ( size_t i = 0; i < width * height; i++ )
    {
        pixels[ 3 * i + 0 ] = buffer[ 4 * i + 0 ];
        pixels[ 3 * i + 1 ] = buffer[ 4 * i + 1 ];
        pixels[ 3 * i + 2 ] = buffer[ 4 * i + 2 ];
    }

    return kvs::ColorImage( width, height, pixels );
}

} // end of namespace osmesa

} // end of namespace kvs
