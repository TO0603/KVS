/*****************************************************************************/
/**
 *  @file   RenderBuffer.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "RenderBuffer.h"
#include <kvs/OpenGL>
#include <kvs/Assert>


namespace kvs
{

void RenderBuffer::create( const size_t width, const size_t height )
{
    KVS_ASSERT( m_id == 0 );

    m_width = width;
    m_height = height;
    this->createID();
    GuardedBinder binder( *this );
    this->setRenderbufferStorage( m_internal_format, m_width, m_height );
}

void RenderBuffer::release()
{
    this->deleteID();
    m_width = 0;
    m_height = 0;
}

void RenderBuffer::bind() const
{
    KVS_ASSERT( this->isCreated() );
    KVS_GL_CALL( glBindRenderbuffer( GL_RENDERBUFFER, m_id ) );
}

void RenderBuffer::unbind() const
{
    KVS_ASSERT( this->isBound() );
    KVS_GL_CALL( glBindRenderbuffer( GL_RENDERBUFFER, 0 ) );
}

bool RenderBuffer::isCreated() const
{
    return m_id > 0;
}

bool RenderBuffer::isValid() const
{
    GLboolean result = GL_FALSE;
    KVS_GL_CALL( result = glIsRenderbuffer( m_id ) );
    return result == GL_TRUE;
}

bool RenderBuffer::isBound() const
{
    if ( this->isCreated() ) return false;

    GLint id = kvs::OpenGL::Integer( GL_RENDERBUFFER_BINDING );
    return static_cast<GLuint>( id ) == m_id;
}

void RenderBuffer::createID()
{
    if ( !this->isValid() )
    {
        KVS_GL_CALL( glGenRenderbuffers( 1, &m_id ) );
    }
}

void RenderBuffer::deleteID()
{
    if ( this->isValid() )
    {
        KVS_GL_CALL( glDeleteRenderbuffers( 1, &m_id ) );
    }
    m_id = 0;
}

void RenderBuffer::setRenderbufferStorage( GLenum internal_format, GLsizei width, GLsizei height )
{
    KVS_ASSERT( width > 0 );
    KVS_ASSERT( width <= kvs::OpenGL::MaxRenderBufferSize() );
    KVS_ASSERT( height > 0 );
    KVS_ASSERT( height <= kvs::OpenGL::MaxRenderBufferSize() );
    KVS_GL_CALL( glRenderbufferStorage( GL_RENDERBUFFER, internal_format, width, height ) );
}

RenderBuffer::GuardedBinder::GuardedBinder( const kvs::RenderBuffer& rb ):
    m_rb( rb )
{
    m_id = kvs::OpenGL::Integer( GL_RENDERBUFFER_BINDING );
    if ( m_rb.id() != static_cast<GLuint>( m_id ) ) { m_rb.bind(); }
}

RenderBuffer::GuardedBinder::~GuardedBinder()
{
    if ( static_cast<GLuint>( m_id ) != m_rb.id() )
    {
        KVS_GL_CALL( glBindRenderbuffer( GL_RENDERBUFFER, m_id ) );
    }
}

} // end of namespace kvs
