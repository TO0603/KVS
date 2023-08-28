/*****************************************************************************/
/**
 *  @file   Screen.h
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#pragma once
#include "ScreenBase.h"
#include <string>
#include <kvs/Scene>
#include <kvs/ObjectBase>
#include <kvs/RendererBase>
#include <kvs/VisualizationPipeline>
#include <kvs/EventListener>


namespace kvs
{

namespace osmesa
{

class Screen : public kvs::osmesa::ScreenBase
{
public:
    using BaseClass = kvs::osmesa::ScreenBase;
    using ControlTarget = kvs::Scene::ControlTarget;

private:
    bool m_enable_default_paint_event = true; ///< flag for default paint event
    kvs::Scene* m_scene = nullptr; ///< scene

public:
    Screen();
    virtual ~Screen();

    kvs::Scene* scene() { return m_scene; }
    const kvs::Scene* scene() const { return m_scene; }

    void setSize( const int width, const int height );
    void setGeometry( const int x, const int y, const int width, const int height );
    void setBackgroundColor( const kvs::RGBColor& color );
    void setBackgroundColor( const kvs::RGBColor& color1, const kvs::RGBColor& color2 );
    void setBackgroundImage( const kvs::ColorImage& image );
    void setEvent( kvs::EventListener* event, const std::string& name = "" );
    void addEvent( kvs::EventListener* event, const std::string& name = "" );

    const std::pair<int,int> registerObject( kvs::ObjectBase* object, kvs::RendererBase* renderer = 0 );
    const std::pair<int,int> registerObject( kvs::VisualizationPipeline* pipeline );

    virtual void enable();
    virtual void disable();
    virtual void reset();

    virtual void initializeEvent();
    virtual void paintEvent();

protected:
    virtual void defaultPaintEvent();
};

} // end of namespace osmesa

} // end of namespace kvs
