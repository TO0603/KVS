/****************************************************************************/
/**
 *  @file   OpacityMap.cpp
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#include "OpacityMap.h"
#include <kvs/Assert>
#include <kvs/Math>


namespace
{

// Default opacity map
kvs::OpacityMap::OpacityMapFunction DefaultOpacityMap = kvs::OpacityMap::Linear;

} // end of namespace


namespace kvs
{

void OpacityMap::SetDefaultOpacityMap( OpacityMapFunction func )
{
    ::DefaultOpacityMap = func;
}

kvs::OpacityMap OpacityMap::DefaultOpacityMap( const size_t resolution, const float max_opacity )
{
    return ::DefaultOpacityMap( resolution, max_opacity );
}

kvs::OpacityMap OpacityMap::Constant( const size_t resolution, const float max_opacity )
{
    Table table( resolution );
    for ( size_t i = 0; i < resolution; ++i )
    {
        table[i] = max_opacity;
    }
    return kvs::OpacityMap( table );
}

kvs::OpacityMap OpacityMap::Linear( const size_t resolution, const float max_opacity )
{
    Table table( resolution );
    const auto scale = 1.0f / static_cast<float>( resolution - 1 );
    for ( size_t i = 0; i < resolution; ++i )
    {
        table[i] = static_cast<float>( i ) * scale * max_opacity;
    }
    return kvs::OpacityMap( table );
}

kvs::OpacityMap OpacityMap::Gaussian( const size_t resolution, const float max_opacity )
{
    const auto u = 1.0f; // mean: [0,1]
    const auto s = 0.2f; // standard deviation: [0,1]
    return OpacityMap::Gaussian( resolution, u, s, max_opacity );
}

kvs::OpacityMap OpacityMap::Gaussian( const size_t resolution, const float u, const float s, const float max_opacity )
{
    Table table( resolution );
    const auto s2 = s * s;
    const auto scale = 1.0f / static_cast<float>( resolution - 1 );
    for ( size_t i = 0; i < resolution; ++i )
    {
        const auto x = static_cast<float>( i ) * scale;
        table[i] = std::exp( -1.0f * ( x - u ) * ( x - u ) / ( 2.0f * s2 ) ) * max_opacity;
    }
    return kvs::OpacityMap( table );
}

/*==========================================================================*/
/**
 *  @brief  Constructs a new OpacityMap class.
 *  @param  resolution [in] resolution
 *  @param  min_value [in] min value
 *  @param  max_value [in] max value
 */
/*==========================================================================*/
OpacityMap::OpacityMap( const size_t resolution, const float min_value, const float max_value ):
    m_resolution( resolution ),
    m_min_value( min_value ),
    m_max_value( max_value ),
    m_points(),
    m_table()
{
}

/*==========================================================================*/
/**
 *  @brief  Constructs a new OpacityMap class.
 *  @param  table [in] opacity value table
 *  @param  min_value [in] min value
 *  @param  max_value [in] max value
 */
/*==========================================================================*/
OpacityMap::OpacityMap( const OpacityMap::Table& table, const float min_value, const float max_value ):
    m_resolution( table.size() ),
    m_min_value( min_value ),
    m_max_value( max_value ),
    m_points(),
    m_table( table )
{
}

OpacityMap::OpacityMap( const size_t resolution, const Points& points ):
    m_resolution( resolution )
{
    this->setPoints( points );
    this->create();
}

OpacityMap::OpacityMap( const size_t resolution, const Points& points, const float min_value, const float max_value )
{
    this->setPoints( points );
    this->setRange( min_value, max_value );
    this->create();
}

OpacityMap::OpacityMap( const size_t resolution, const std::list<float>& opacities )
{
    this->setPoints( opacities );
    this->create();
}

/*===========================================================================*/
/**
 *  @brief  Returns true if the range is specified.
 *  @return ture if the range is specified
 */
/*===========================================================================*/
bool OpacityMap::hasRange() const
{
    return !kvs::Math::Equal( m_min_value, m_max_value );
}

void OpacityMap::setPoints( const std::list<float>& opacities )
{
    const size_t nopacities = opacities.size();
    const float stride = 1.0f / ( nopacities - 1 );

    this->addPoint( 0.0f, opacities.front() ); // start point
    auto opacity = opacities.begin(); opacity++;
    auto end = opacities.end(); end--;
    size_t index = 1;
    while ( opacity != end )
    {
        const float value = kvs::Math::Round( m_resolution * stride * index );
        this->addPoint( value, *opacity );
        opacity++; index++;
    }
    this->addPoint( float( m_resolution - 1 ), opacities.back() ); // end point
}

/*===========================================================================*/
/**
 *  @brief  Adds a control point.
 *  @param  value [in] scalar value in [min_value, max_value]
 *  @param  opacity [in] opacity value in [0,1]
 */
/*===========================================================================*/
void OpacityMap::addPoint( const float value, const float opacity )
{
    m_points.push_back( Point( value, kvs::Math::Clamp( opacity, 0.0f, 1.0f ) ) );
}

/*===========================================================================*/
/**
 *  @brief  Removes the constrol point associated with the given scalar value.
 *  @param  value [in] scalar value in [min_value, max_value]
 */
/*===========================================================================*/
void OpacityMap::removePoint( const float value )
{
    m_points.remove_if( [ value ]( Point& p ) { return kvs::Math::Equal( p.first, value ); } );
}

/*==========================================================================*/
/**
 *  @brief  Creates the opacity map.
 */
/*==========================================================================*/
void OpacityMap::create()
{
    kvs::Real32 min_value = 0.0f;
    kvs::Real32 max_value = static_cast<kvs::Real32>( m_resolution - 1 );
    if ( this->hasRange() )
    {
        min_value = this->minValue();
        max_value = this->maxValue();
    }

    if ( m_points.size() == 0 )
    {
        *this = ::DefaultOpacityMap( m_resolution, 1.0f );
    }
    else
    {
        m_table.allocate( m_resolution );
        m_points.sort( [] ( const Point& p1, const Point& p2 ) { return p1.first < p2.first; } );

        if ( m_points.front().first > min_value ) this->addPoint( min_value, 0.0f );
        if ( m_points.back().first < max_value ) this->addPoint( max_value, 1.0f );

        const float stride = ( max_value - min_value ) / static_cast<float>( m_resolution - 1 );
        float f = min_value;
        for ( size_t i = 0; i < m_resolution; ++i, f += stride )
        {
            Points::iterator p = m_points.begin();
            Points::iterator last = m_points.end();

            float opacity = 0.0f;
            Point p0( min_value, 0.0f );
            Point p1( max_value, 1.0f );
            while ( p != last )
            {
                const float s = p->first;
                if ( kvs::Math::Equal( f, s ) )
                {
                    opacity = p->second;
                    break;
                }
                else if ( f < s )
                {
                    p1 = *p;
                    // Interpolate.
                    const float s0 = p0.first;
                    const float s1 = p1.first;
                    const float a0 = p0.second;
                    const float a1 = p1.second;
                    opacity = kvs::Math::Mix( a0, a1, ( f - s0 ) / ( s1 - s0 ) );
                    break;
                }
                else
                {
                    p0 = *p;
                    ++p;
                    if ( p == last )
                    {
                        if ( kvs::Math::Equal( p0.first, max_value ) )
                        {
                            opacity = p0.second;
                        }
                    }
                }
            }

            m_table[i] = opacity;
        }
    }
}

/*==========================================================================*/
/**
 *  @brief  Returns the opacity value which is specified by the table index.
 *  @param  index [in] table index
 *  @return opacity value
 */
/*==========================================================================*/
kvs::Real32 OpacityMap::operator []( const size_t index ) const
{
    KVS_ASSERT( index < this->resolution() );
    return m_table[index];
}

/*===========================================================================*/
/**
 *  @brief  Returns interpolated opacity value by assuming piecewise linear map.
 *  @param  value [in] value
 *  @return interpolated opacity value
 */
/*===========================================================================*/
kvs::Real32 OpacityMap::at( const float value ) const
{
    const float v0 = kvs::Math::Clamp( value, m_min_value, m_max_value );
    const float r = static_cast<float>( m_resolution - 1 );
    const float v = ( v0 - m_min_value ) / ( m_max_value - m_min_value ) * r;
    const size_t s0 = static_cast<size_t>( v );
    const size_t s1 = kvs::Math::Min( s0 + 1, m_resolution - 1 );

    const kvs::Real32 a0 = m_table[ s0 ];
    const kvs::Real32 a1 = m_table[ s1 ];
    return kvs::Math::Mix( a0, a1, v - s0 );
}

/*==========================================================================*/
/**
 *  Substitution operator =.
 *  @param opacity_map [in] opacity map
 *  @retval opacity map
 */
/*==========================================================================*/
OpacityMap& OpacityMap::operator =( const OpacityMap& rhs )
{
    m_resolution = rhs.m_resolution;
    m_min_value = rhs.m_min_value;
    m_max_value = rhs.m_max_value;
    m_points = rhs.m_points;
    m_table = rhs.m_table;
    return *this;
}

} // end of namespace kvs
