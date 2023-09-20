/*****************************************************************************/
/**
 *  @file   StochasticUniformGridRenderer.cpp
 *  @author Naoya Maeda, Naohisa Sakamoto
 */
/*****************************************************************************/
#include "StochasticUniformGridRenderer.h"
#include <cmath>
#include <cfloat>
#include <kvs/OpenGL>
#include <kvs/StructuredVolumeObject>
#include <kvs/TransferFunction>
#include <kvs/ColorMap>
#include <kvs/OpacityMap>
#include <kvs/ValueArray>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/Coordinate>
#include <kvs/Assert>
#include <kvs/Message>
#include <kvs/Xorshift128>


namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns a random number as integer value.
 *  @return random number
 */
/*===========================================================================*/
inline int RandomNumber()
{
    const int C = 12347;
    static kvs::Xorshift128 R;
    return C * R.randInteger();
}

} // end of namespace


namespace kvs
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new StochasticUniformGridRenderer class.
 */
/*===========================================================================*/
StochasticUniformGridRenderer::StochasticUniformGridRenderer():
    StochasticRendererBase( new Engine() )
{
}

void StochasticUniformGridRenderer::setEdgeFactor( const float factor )
{
    static_cast<Engine&>( engine() ).setEdgeFactor( factor );
}

/*===========================================================================*/
/**
 *  @brief  Sets a sampling step.
 *  @param  transfer_function [in] transfer function
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::setSamplingStep( const float step )
{
    static_cast<Engine&>( engine() ).setSamplingStep( step );
}

/*===========================================================================*/
/**
 *  @brief  Sets a transfer function.
 *  @param  transfer_function [in] transfer function
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::setTransferFunction(
    const kvs::TransferFunction& transfer_function )
{
    static_cast<Engine&>( engine() ).setTransferFunction( transfer_function );
}

/*===========================================================================*/
/**
 *  @brief  Returns a transfer function.
 *  @return transfer function
 */
/*===========================================================================*/
const kvs::TransferFunction& StochasticUniformGridRenderer::transferFunction() const
{
    return static_cast<const Engine&>( engine() ).transferFunction();
}

/*===========================================================================*/
/**
 *  @brief  Returns sampling step size.
 *  @return sampling step size
 */
/*===========================================================================*/
float StochasticUniformGridRenderer::samplingStep() const
{
    return static_cast<const Engine&>( engine() ).samplingStep();
}

void StochasticUniformGridRenderer::setVertexShaderFile( const std::string& file )
{
    static_cast<Engine&>( engine() ).setVertexShaderFile( file );
}

void StochasticUniformGridRenderer::setFragmentShaderFile( const std::string& file )
{
    static_cast<Engine&>( engine() ).setFragmentShaderFile( file );
}

void StochasticUniformGridRenderer::setShaderFiles( const std::string& vert_file, const std::string& frag_file )
{
    static_cast<Engine&>( engine() ).setShaderFiles( vert_file, frag_file );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Engine class.
 */
/*===========================================================================*/
StochasticUniformGridRenderer::Engine::Engine()
{
    m_render_pass.setShaderFiles(
        "SR_uniform_grid.vert",
        "SR_uniform_grid.frag" );
}

/*===========================================================================*/
/**
 *  @brief  Releases the GPU resources.
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::release()
{
    m_transfer_function_texture.release();
    m_transfer_function_changed = true;

    m_entry_texture.release();
    m_exit_texture.release();
    m_entry_exit_framebuffer.release();

    m_volume_buffer.release();
    m_bounding_cube_buffer.release();

    m_render_pass.release();
    m_bounding_render_pass.release();
}

/*===========================================================================*/
/**
 *  @brief  Create.
 *  @param  object [in] pointer to the volume object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::create(
    kvs::ObjectBase* object,
    kvs::Camera* camera,
    kvs::Light* light )
{
    auto* volume = kvs::StructuredVolumeObject::DownCast( object );
    BaseClass::attachObject( object );
    BaseClass::createRandomTexture();

    this->create_shader_program( BaseClass::shader(), BaseClass::isShadingEnabled() );

    const float dpr = camera->devicePixelRatio();
    const size_t framebuffer_width = static_cast<size_t>( camera->windowWidth() * dpr );
    const size_t framebuffer_height = static_cast<size_t>( camera->windowHeight() * dpr );
    this->create_framebuffer( framebuffer_width, framebuffer_height );

    this->create_buffer_object( volume );
}

/*===========================================================================*/
/**
 *  @brief  Update.
 *  @param  object [in] pointer to the object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::update(
    kvs::ObjectBase* object,
    kvs::Camera* camera,
    kvs::Light* light )
{
    const float dpr = camera->devicePixelRatio();
    const size_t framebuffer_width = static_cast<size_t>( camera->windowWidth() * dpr );
    const size_t framebuffer_height = static_cast<size_t>( camera->windowHeight() * dpr );
    this->update_framebuffer( framebuffer_width, framebuffer_height );
}

/*===========================================================================*/
/**
 *  @brief  Set up.
 *  @param  object [in] pointer to the object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::setup(
    kvs::ObjectBase* object,
    kvs::Camera* camera,
    kvs::Light* light )
{
    if ( m_transfer_function_changed )
    {
        const size_t width = m_transfer_function.resolution();
        const auto table = m_transfer_function.table();
        m_transfer_function_texture.release();
        m_transfer_function_texture.setWrapS( GL_CLAMP_TO_EDGE );
        m_transfer_function_texture.setMagFilter( GL_LINEAR );
        m_transfer_function_texture.setMinFilter( GL_LINEAR );
        m_transfer_function_texture.setPixelFormat( GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT  );
        m_transfer_function_texture.create( width, table.data() );
        m_transfer_function_changed = false;
    }

    this->setup_shader_program( BaseClass::shader(), object, camera, light );
}

/*===========================================================================*/
/**
 *  @brief  Draw an ensemble.
 *  @param  object [in] pointer to the line object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::draw( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::OpenGL::Enable( GL_DEPTH_TEST );
    kvs::OpenGL::Enable( GL_TEXTURE_2D );

    kvs::ProgramObject::Binder unit( m_render_pass.shaderProgram() );
    this->draw_buffer_object( kvs::StructuredVolumeObject::DownCast( object ) );
}

/*===========================================================================*/
/**
 *  @brief  Creates shader program.
 *  @param  shading_model [in] shading model
 *  @param  shading_enabled [in] if true, shading will be enabled
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::create_shader_program(
    const kvs::Shader::ShadingModel& shading_model,
    const bool shading_enabled )
{
    m_bounding_render_pass.create();
    m_render_pass.create( shading_model, shading_enabled );

    auto& shader = m_render_pass.shaderProgram();
    kvs::ProgramObject::Binder bind( shader );
    shader.setUniform( "sampling_step", m_step );
    shader.setUniform( "volume_data", 0 );
    shader.setUniform( "exit_points", 1 );
    shader.setUniform( "entry_points", 2 );
    shader.setUniform( "transfer_function_data", 3 );
    shader.setUniform( "random_texture", 4 );
}

/*===========================================================================*/
/**
 *  @brief  Updates shader program.
 *  @param  shading_model [in] shading model
 *  @param  shading_enabled [in] if true, shading will be enabled
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::update_shader_program(
    const kvs::Shader::ShadingModel& shading_model,
    const bool shading_enabled )
{
    m_render_pass.release();
    m_bounding_render_pass.release();
    this->create_shader_program( shading_model, shading_enabled );
}

/*===========================================================================*/
/**
 *  @brief  Setups shader program.
 *  @param  shading_model [in] shading model
 *  @param  object [in] pointer to the object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::setup_shader_program(
    const kvs::Shader::ShadingModel& shading_model,
    const kvs::ObjectBase* object,
    const kvs::Camera* camera,
    const kvs::Light* light )
{
    // Setup entry/exit textures by drawing bounding cube to FBO
    {
        // Change renderig target to the entry/exit FBO.
        kvs::FrameBufferObject::GuardedBinder binder( m_entry_exit_framebuffer );
        m_bounding_render_pass.setup();
        m_bounding_render_pass.draw();
    }

    {
        m_render_pass.setup( shading_model, object, camera, light );
        m_render_pass.shaderProgram().bind();
        m_render_pass.shaderProgram().setUniform( "random_texture_size_inv", 1.0f / randomTextureSize() );
        m_render_pass.shaderProgram().setUniform( "edge_factor", m_edge_factor );
        m_render_pass.shaderProgram().unbind();
    }

    // Setup OpenGL statement.
    kvs::OpenGL::Disable( GL_CULL_FACE );
    kvs::OpenGL::Enable( GL_DEPTH_TEST );

    if ( BaseClass::isShadingEnabled() ) kvs::OpenGL::Enable( GL_LIGHTING );
    else kvs::OpenGL::Disable( GL_LIGHTING );
}

/*===========================================================================*/
/**
 *  @brief  Creates framebuffer.
 *  @param  width [in] framebuffer width
 *  @param  height [in] framebuffer height
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::create_framebuffer(
    const size_t width,
    const size_t height )
{
    m_entry_texture.setWrapS( GL_CLAMP_TO_BORDER );
    m_entry_texture.setWrapT( GL_CLAMP_TO_BORDER );
    m_entry_texture.setMagFilter( GL_LINEAR );
    m_entry_texture.setMinFilter( GL_LINEAR );
    m_entry_texture.setPixelFormat( GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT );
    m_entry_texture.create( width, height );

    m_exit_texture.setWrapS( GL_CLAMP_TO_BORDER );
    m_exit_texture.setWrapT( GL_CLAMP_TO_BORDER );
    m_exit_texture.setMagFilter( GL_LINEAR );
    m_exit_texture.setMinFilter( GL_LINEAR );
    m_exit_texture.setPixelFormat( GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT  );
    m_exit_texture.create( width, height );

    m_entry_exit_framebuffer.create();
    m_entry_exit_framebuffer.attachColorTexture( m_exit_texture, 0 );
    m_entry_exit_framebuffer.attachColorTexture( m_entry_texture, 1 );

    auto& shader = m_render_pass.shaderProgram();
    kvs::ProgramObject::Binder bind( shader );
    shader.setUniform( "width", static_cast<GLfloat>( width ) );
    shader.setUniform( "height", static_cast<GLfloat>( height ) );
}

/*===========================================================================*/
/**
 *  @brief  Updates framebuffer
 *  @param  width [in] framebuffer width
 *  @param  height [in] framebuffer height
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::update_framebuffer(
    const size_t width,
    const size_t height )
{
    m_entry_texture.release();
    m_exit_texture.release();
    m_entry_exit_framebuffer.release();
    this->create_framebuffer( width, height );
}

/*===========================================================================*/
/**
 *  @brief  Create buffer object.
 *  @param  volume [in] pointer to the volume object
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::create_buffer_object(
    const kvs::StructuredVolumeObject* volume )
{
    m_volume_buffer.create( volume, this->transferFunction() );
    m_bounding_cube_buffer.create( volume );

    // Set uniform variables.
    const kvs::Vec3 r( volume->resolution() );
    const kvs::Real32 max_ngrids = kvs::Math::Max( r.x(), r.y(), r.z() );
    const kvs::Vec3 ratio( r.x() / max_ngrids, r.y() / max_ngrids, r.z() / max_ngrids );
    const kvs::Vec3 reciprocal( 1.0f / r.x(), 1.0f / r.y(), 1.0f / r.z() );
    kvs::Real32 min_range = 0.0f;
    kvs::Real32 max_range = 0.0f;
    kvs::Real32 min_value = this->transferFunction().colorMap().minValue();
    kvs::Real32 max_value = this->transferFunction().colorMap().maxValue();
    const std::type_info& type = volume->values().typeInfo()->type();
    if ( type == typeid( kvs::UInt8 ) )
    {
        min_range = 0.0f;
        max_range = 255.0f;
        if ( !this->transferFunction().hasRange() )
        {
            min_value = 0.0f;
            max_value = 255.0f;
        }
    }
    else if ( type == typeid( kvs::Int8 ) )
    {
        min_range = static_cast<kvs::Real32>( kvs::Value<kvs::UInt8>::Min() );
        max_range = static_cast<kvs::Real32>( kvs::Value<kvs::UInt8>::Max() );
        if ( !this->transferFunction().hasRange() )
        {
            min_value = -128.0f;
            max_value = 127.0f;
        }
    }
    else if ( type == typeid( kvs::UInt16 ) )
    {
        min_range = static_cast<kvs::Real32>( kvs::Value<kvs::UInt16>::Min() );
        max_range = static_cast<kvs::Real32>( kvs::Value<kvs::UInt16>::Max() );
        if ( !this->transferFunction().hasRange() )
        {
            min_value = static_cast<kvs::Real32>( volume->minValue() );
            max_value = static_cast<kvs::Real32>( volume->maxValue() );
        }
    }
    else if ( type == typeid( kvs::Int16 ) )
    {
        min_range = static_cast<kvs::Real32>( kvs::Value<kvs::Int16>::Min() );
        max_range = static_cast<kvs::Real32>( kvs::Value<kvs::Int16>::Max() );
        if ( !this->transferFunction().hasRange() )
        {
            min_value = static_cast<kvs::Real32>( volume->minValue() );
            max_value = static_cast<kvs::Real32>( volume->maxValue() );
        }
    }
    else if ( type == typeid( kvs::UInt32 ) ||
              type == typeid( kvs::Int32  ) ||
              type == typeid( kvs::Real32 ) ||
              type == typeid( kvs::Real64 ) )
    {
        min_range = 0.0f;
        max_range = 1.0f;
        min_value = 0.0f;
        max_value = 1.0f;
    }
    else
    {
        kvsMessageError( "Not supported data type '%s'.",
                         volume->values().typeInfo()->typeName() );
    }

    auto& shader = m_render_pass.shaderProgram();
    kvs::ProgramObject::Binder bind( shader );
    shader.setUniform( "volume.resolution", r );
    shader.setUniform( "volume.resolution_ratio", ratio );
    shader.setUniform( "volume.resolution_reciprocal", reciprocal );
    shader.setUniform( "volume.min_range", min_range );
    shader.setUniform( "volume.max_range", max_range );
    shader.setUniform( "transfer_function.min_value", min_value );
    shader.setUniform( "transfer_function.max_value", max_value );
}

/*===========================================================================*/
/**
 *  @brief  Updates buffer object.
 *  @param  volume [in] pointer to the volume object
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::update_buffer_object(
    const kvs::StructuredVolumeObject* volume )
{
    m_volume_buffer.release();
    m_bounding_cube_buffer.release();
    this->create_buffer_object( volume );
}

/*===========================================================================*/
/**
 *  @brief  Draws buffer object.
 *  @param  volume [in] pointer to the volume object
 */
/*===========================================================================*/
void StochasticUniformGridRenderer::Engine::draw_buffer_object(
    const kvs::StructuredVolumeObject* volume )
{
    const size_t size = BaseClass::randomTextureSize();
    const int count = BaseClass::repetitionCount() * ::RandomNumber();
    const float offset_x = static_cast<float>( ( count ) % size );
    const float offset_y = static_cast<float>( ( count / size ) % size );
    const kvs::Vec2 random_offset( offset_x, offset_y );
    m_render_pass.shaderProgram().setUniform( "random_offset", random_offset );

    kvs::Texture::Binder unit0( m_volume_buffer.manager(), 0 );
    kvs::Texture::Binder unit1( m_exit_texture, 1 );
    kvs::Texture::Binder unit2( m_entry_texture, 2 );
    kvs::Texture::Binder unit3( m_transfer_function_texture, 3 );
    kvs::Texture::Binder unit4( BaseClass::randomTexture(), 4 );
    m_volume_buffer.draw();
}

} // end of namespace kvs
