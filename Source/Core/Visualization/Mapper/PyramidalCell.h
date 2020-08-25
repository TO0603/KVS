/****************************************************************************/
/**
 *  @file   PyramidalCell.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#ifndef KVS__PYRAMIDAL_CELL_H_INCLUDE
#define KVS__PYRAMIDAL_CELL_H_INCLUDE

#include <kvs/Type>
#include <kvs/Vector4>
#include <kvs/Matrix44>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/CellBase>


namespace kvs
{

/*===========================================================================*/
/**
 *  @brief  Pyramidal cell class.
 */
/*===========================================================================*/
class PyramidalCell : public kvs::CellBase
{
public:

    typedef kvs::CellBase BaseClass;

private:

    kvs::Vec3 m_pyramid; // x,y: length of bottom plane, z: height of pyramid

public:

    PyramidalCell( const kvs::UnstructuredVolumeObject* volume );

    void updateInterpolationFunctions( const kvs::Vec3& local ) const;
    void updateDifferentialFunctions( const kvs::Vec3& local ) const;
    void bindCell( const kvs::UInt32 cell );
    bool containsLocalPoint( const kvs::Vec3& local ) const;
    kvs::Vec3 randomSampling() const;
    kvs::Real32 volume() const;
    kvs::Vec3 localCenter() const;
};

} // end of namespace kvs

#endif // KVS__PYRAMIDAL_CELL_H_INCLUDE
