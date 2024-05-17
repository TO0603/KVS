/****************************************************************************/
/**
 *  @file   PointImporter.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include "ImporterBase.h"
#include <kvs/Module>
#include <kvs/PointObject>
#include <kvs/KVSMLPointObject>
#include <kvs/LAS>


namespace kvs
{

/*==========================================================================*/
/**
 *  @brief  Point object importer class.
 */
/*==========================================================================*/
class PointImporter : public kvs::ImporterBase, public kvs::PointObject
{
    kvsModule( kvs::PointImporter, Importer );
    kvsModuleBaseClass( kvs::ImporterBase );
    kvsModuleSuperClass( kvs::PointObject );

public:
    PointImporter();
    PointImporter( const std::string& filename );
    PointImporter( const kvs::FileFormatBase* file_format );
    virtual ~PointImporter();

    SuperClass* exec( const kvs::FileFormatBase* file_format );

private:
    void import( const kvs::KVSMLPointObject* kvsml );
    void import( const kvs::LAS* las );
};

} // end of namespace kvs
