/****************************************************************************/
/**
 *  @file   Camera.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include <kvs/XformControl>
#include <kvs/Vector2>
#include <kvs/Vector3>
#include <kvs/Matrix33>
#include <kvs/ColorImage>
#include <kvs/Deprecated>


namespace kvs
{

/*==========================================================================*/
/**
 *  Camera class
 */
/*==========================================================================*/
class Camera : public kvs::XformControl
{
public:
    enum ProjectionType
    {
        Perspective = 0,
        Orthogonal = 1,
        Frustum
    };

private:
    kvs::Vec3 m_transform_center{ 0.0f, 0.0f, 0.0f }; // transform center
    ProjectionType m_projection_type = ProjectionType::Perspective; ///< projection type
    float m_field_of_view = 45.0f; ///< field of view [deg]
    float m_front = 1.0f; ///< front plane position
    float m_back = 2000.0f; ///< back plane position
    float m_left = -5.0f; ///< left plane position
    float m_right = 5.0f; ///< right plane position
    float m_bottom = -5.0f; ///< bottom plane position
    float m_top = 5.0f; ///< top plane position
    kvs::Mat4 m_projection_matrix = kvs::Mat4::Identity();
    size_t m_window_width = 512; ///< window width
    size_t m_window_height = 512; ///< window height

public:
    Camera();
    virtual ~Camera() = default;

    void setProjectionType( const ProjectionType type );
    void setProjectionTypeToPerspective() { this->setProjectionType( Perspective ); }
    void setProjectionTypeToOrthogonal() { this->setProjectionType( Orthogonal ); }
    void setProjectionTypeToFrustum() { this->setProjectionType( Frustum ); }
    void setFieldOfView( const float fov );
    void setBack( const float back );
    void setFront( const float front );
    void setLeft( const float left );
    void setRight( const float right );
    void setBottom( const float bottom );
    void setTop( const float top );
    void setProjectionMatrix( const kvs::Mat4& projection_matrix );
    void setWindowSize( const size_t width, const size_t height );
    void setPosition( const kvs::Vec3& position );
    void setPosition( const kvs::Vec3& position, const kvs::Vec3& look_at );
    void setPosition( const kvs::Vec3& position, const kvs::Vec3& look_at, const kvs::Vec3& up );
    void setUpVector( const kvs::Vec3& up_vector );
    void setLookAt( const kvs::Vec3& look_at );

    bool isPerspective() const { return m_projection_type == Perspective; }
    ProjectionType projectionType() const { return m_projection_type; }
    float fieldOfView() const { return m_field_of_view; }
    float back() const { return m_back; }
    float front() const { return m_front; }
    float left() const { return m_left; }
    float right() const { return m_right; }
    float bottom() const { return m_bottom; }
    float top() const { return m_top; }
    size_t windowWidth() const { return m_window_width; }
    size_t windowHeight() const { return m_window_height; }
    float devicePixelRatio() const;
    const kvs::Vec3 position() const;
    const kvs::Vec3 upVector() const;
    const kvs::Vec3 lookAt() const;
    const kvs::Mat4 projectionMatrix() const;
    const kvs::Mat4 viewingMatrix() const;
    const kvs::Vec2 lookAtInDevice() const;

    virtual void update();
    virtual kvs::ColorImage snapshot();

    void resetXform();
    void rotate( const kvs::Mat3& rotation );
    void translate( const kvs::Vec3& translation );
    void scale( const kvs::Vec3& scaling );

    KVS_DEPRECATED( virtual void initialize() ) { *this = Camera(); }

private:
    void computeProjectionMatrix();
};

} // end of namespace kvs
