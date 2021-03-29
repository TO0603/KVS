/****************************************************************************/
/**
 *  @file   FrameBuffer.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include <kvs/OpenGL>
#include <kvs/Deprecated>


namespace kvs
{

/*==========================================================================*/
/**
 *  Frame buffer class.
 */
/*==========================================================================*/
class FrameBuffer
{
private:
    GLenum m_format = 0; ///< pixel data format
    GLenum m_type = 0; ///< pixel data type

public:
    FrameBuffer( const GLenum format, const GLenum type ): m_format( format ), m_type( type ) {}
    FrameBuffer() = default;

    GLenum format() const { return m_format; }
    GLenum type() const { return m_type; }
    void setFormat( const GLenum format );
    void setType( const GLenum type );
    void readPixels( const int x, const int y, const size_t width, const size_t height, void* pixels, const GLenum buffer = 0 ) const;
    void drawPixels( const int x, const int y, const size_t width, const size_t height, const void* pixels, const GLenum buffer = 0 ) const;

public:
    KVS_DEPRECATED( void read(
        const int    width,
        const int    height,
        void**       pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( void read(
        const int    x,
        const int    y,
        const int    width,
        const int    height,
        void**       pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( static void read(
        const int    x,
        const int    y,
        const int    width,
        const int    height,
        const GLenum format,
        const GLenum type,
        void**       pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( void draw(
        const int    width,
        const int    height,
        const void*  pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( void draw(
        const int    width,
        const int    height,
        const int*   viewport,
        const void*  pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( void draw(
        const int    x,
        const int    y,
        const int    width,
        const int    height,
        const int*   viewport,
        const void*  pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( void draw(
        const int    x,
        const int    y,
        const int    width,
        const int    height,
        const void*  pixels,
        const GLenum buffer = 0 ) );

    KVS_DEPRECATED( void draw(
        const int    x,
        const int    y,
        const int    width,
        const int    height,
        const GLenum format,
        const GLenum type,
        const int*   viewport,
        const void*  pixels,
        const GLenum buffer = 0 ) );
};

} // end of namespace kvs
