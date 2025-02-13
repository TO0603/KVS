/*****************************************************************************/
/**
 *  @file   OpenXRInteractor.h
 *  @author Keisuke Tajiri
 */
/*****************************************************************************/
#pragma once
#include <kvs/InteractorBase>
#include <kvs/LineObject>
#include <kvs/PointObject>
#include <kvs/PolygonImporter>
#include <SupportOpenXR/Viewer/Screen.h>
#include <SupportOpenXR/RemoteDesktop/RemoteDesktop.h>

namespace kvs
{

namespace openxr
{

class Screen;

/*===========================================================================*/
/**
 *  @brief  OpenXR interactor class.
 */
/*===========================================================================*/
class OpenXRInteractor : public kvs::InteractorBase
{
public:
    using BaseClass = kvs::InteractorBase;

private:
    kvs::openxr::Screen* m_openxr_screen = nullptr;
    kvs::openxr::RemoteDesktop* m_remote_desktop = nullptr;
    kvs::LineObject* m_axis_x = nullptr;
    kvs::LineObject* m_axis_y = nullptr;
    kvs::LineObject* m_axis_z = nullptr;
    kvs::LineObject* m_pointer = nullptr;
    kvs::Vec3 m_pointer_scale = kvs::Vec3::Ones();
    kvs::PolygonObject* m_controller_model[kvs::Side::Max] = { nullptr };
    kvs::Timer m_elapsed_timer = {};

    kvs::PointObject* m_left_hand = nullptr;
    kvs::PointObject* m_right_hand = nullptr;

    kvs::Vec3 m_left_hand_initial_translation;
    kvs::Vec3 m_right_hand_initial_translation;

    kvs::LineObject* m_plot_line_object = nullptr;
    std::pair<int,int> m_plot_line_object_id;
public:
    OpenXRInteractor( kvs::openxr::Screen* screen );
    virtual ~OpenXRInteractor();
    kvs::PointObject* leftHand() const { return m_left_hand; }
    kvs::PointObject* rightHand() const { return m_right_hand; }

    void setLeftHandInitialTranslation( kvs::Vec3 leftHandInitialTranslation ) { m_left_hand_initial_translation = leftHandInitialTranslation; }
    void setRightHandInitialTranslation( kvs::Vec3 rightHandInitialTranslation ) { m_right_hand_initial_translation = rightHandInitialTranslation; }
    kvs::Vec3 getLeftHandInitialTranslation() const { return m_left_hand_initial_translation; }
    kvs::Vec3 getRightHandInitialTranslation() const { return m_right_hand_initial_translation; }

    void setPlotLineObject( kvs::LineObject* object ) { m_plot_line_object = object; }
    kvs::LineObject* getPlotLineObject() { return m_plot_line_object; }

    void setPlotLineObjectID( std::pair<int, int> plotLineObjectID ) { m_plot_line_object_id = plotLineObjectID; }
    std::pair<int,int> getPlotLineObjectID() const { return m_plot_line_object_id; }

    void setCustomMinMaxCoords( kvs::Vec3 minCoords, kvs::Vec3 maxCoords )
    {
        // m_controller_model[0]->setMinMaxObjectCoords( minCoords, maxCoords );
        // m_controller_model[1]->setMinMaxObjectCoords( minCoords, maxCoords );
        m_left_hand->setMinMaxObjectCoords( minCoords, maxCoords );
        m_right_hand->setMinMaxObjectCoords( minCoords, maxCoords );
    };

    void setCustomMinMaxExternal( kvs::Vec3 minExternal, kvs::Vec3 maxExternal )
    {
        // m_controller_model[0]->setMinMaxExternalCoords( minExternal, maxExternal );
        // m_controller_model[1]->setMinMaxExternalCoords( minExternal, maxExternal );
        m_left_hand->setMinMaxExternalCoords( minExternal, maxExternal );
        m_right_hand->setMinMaxExternalCoords( minExternal, maxExternal );
    }

protected:
    virtual void mousePressEvent( kvs::MouseEvent* e ) {}
    virtual void mouseMoveEvent( kvs::MouseEvent* e ) {}
    virtual void mouseReleaseEvent( kvs::MouseEvent* e ) {}
    virtual void wheelEvent( kvs::WheelEvent* e ) {}
    virtual void keyPressEvent( kvs::KeyEvent* e ) {}
    virtual void initializeEvent();
    virtual void paintEvent();
    virtual void timerEvent( kvs::TimeEvent* e );
    virtual void controllerPressEvent( kvs::ControllerEvent* e );
    virtual void controllerReleaseEvent( kvs::ControllerEvent* e );
    virtual void controllerMoveEvent( kvs::ControllerEvent* e );
    virtual void controllerAxisEvent( kvs::ControllerEvent* e );
};

} // end of namespace openxr

} // end of namespace kvs
