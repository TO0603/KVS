/*
 * Copyright (c) 2022 Japan Atomic Energy Agency
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <kvs/extendedfileformat/VtkImport>

#include <deque>
#include <stdexcept>
#include <vector>

#include "kvs/LineObject"
#include "kvs/PointObject"
#include "kvs/PolygonObject"
#include "kvs/StructuredVolumeObject"
#include "kvs/Type"
#include "kvs/UnstructuredVolumeObject"
#include "kvs/ValueArray"
#include "kvs/Vector3"
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkTypedArray.h>
#include <vtkUnstructuredGrid.h>

namespace
{
template <typename VtkPointSetPointerType>
kvs::ValueArray<kvs::Real32> GetCoordinates( VtkPointSetPointerType data )
{
    std::vector<kvs::Real32> coords( data->GetNumberOfPoints() * 3 );

#pragma omp parallel for
    for ( vtkIdType i = 0; i < data->GetNumberOfPoints(); ++i )
    {
        auto p = data->GetPoint( i );

        coords[i * 3] = static_cast<kvs::Real32>( p[0] );
        coords[i * 3 + 1] = static_cast<kvs::Real32>( p[1] );
        coords[i * 3 + 2] = static_cast<kvs::Real32>( p[2] );
    }

    return kvs::ValueArray<kvs::Real32>( coords );
}

template <typename VtkPointSetPointerType>
kvs::Vector3ui GetResolution( VtkPointSetPointerType data )
{
    kvs::Vector3ui resolution;

    int dimensions[3];
    data->GetDimensions( dimensions );
    resolution[0] = static_cast<unsigned int>( dimensions[0] );
    resolution[1] = static_cast<unsigned int>( dimensions[1] );
    resolution[2] = static_cast<unsigned int>( dimensions[2] );

    return resolution;
}

template <typename VtkPointSetPointerType>
int GetCountOfComponents( VtkPointSetPointerType data )
{
    auto point_data = data->GetPointData();
    int component_count = 0;

    for ( vtkIdType i = 0; i < point_data->GetNumberOfArrays(); ++i )
    {
        auto array = point_data->GetArray( i );
        component_count += array->GetNumberOfComponents();
    }

    return component_count;
}

template <typename T>
kvs::AnyValueArray GetValueArrayImpl( vtkPointData* point_data, vtkIdType node_count,
                                      int component_count )
{
    if ( component_count == 0 )
    {
        kvs::ValueArray<T> t_values;

        return kvs::AnyValueArray( t_values );
    }
    else
    {
        kvs::ValueArray<T> t_values( component_count * node_count );
        int component_head = 0;

        for ( vtkIdType i = 0; i < point_data->GetNumberOfArrays(); ++i )
        {
            auto array = point_data->GetArray( i );

#pragma omp parallel for
            for ( vtkIdType p = 0; p < node_count; ++p )
            {
                auto t = array->GetTuple( p );

                for ( int c = 0; c < array->GetNumberOfComponents(); ++c )
                {
                    t_values[( component_head + c ) * node_count + p] = static_cast<T>( t[c] );
                }
            }

            component_head += array->GetNumberOfComponents();
        }

        return kvs::AnyValueArray( t_values );
    }
}

template <typename VtkPointSetPointerType>
kvs::VolumeObjectBase::Values GetValueArray( VtkPointSetPointerType data, int component_count )
{
    auto point_data = data->GetPointData();

    if ( component_count > 0 )
    {
        int array_type = 0;
        for ( vtkIdType i = 0; i < point_data->GetNumberOfArrays(); ++i )
        {
            auto array = point_data->GetArray( i );

            switch ( array->GetArrayType() )
            {
            case VTK_TYPE_UINT8:
                array_type = std::max( array_type, 0 );
                break;
            case VTK_TYPE_UINT16:
                array_type = std::max( array_type, 1 );
                break;
            case VTK_TYPE_UINT32:
                array_type = std::max( array_type, 2 );
                break;
            case VTK_TYPE_UINT64:
                array_type = std::max( array_type, 3 );
                break;
            case VTK_TYPE_INT8:
                array_type = std::max( array_type, 4 );
                break;
            case VTK_TYPE_INT16:
                array_type = std::max( array_type, 5 );
                break;
            case VTK_TYPE_INT32:
                array_type = std::max( array_type, 6 );
                break;
            case VTK_TYPE_INT64:
                array_type = std::max( array_type, 7 );
                break;
            case VTK_FLOAT:
                array_type = std::max( array_type, 8 );
                break;
            case VTK_DOUBLE:
            default:
                array_type = std::numeric_limits<int>::max();
            }
        }

        // This statement will be removed in the future
        // Currently, only float type is supported in the server program
        // Convert all types to float until the server program supports non-float types
        array_type = 8;

        switch ( array_type )
        {
        case 0:
            return ::GetValueArrayImpl<kvs::UInt8>( data->GetPointData(), data->GetNumberOfPoints(),
                                                    component_count );
        case 1:
            return ::GetValueArrayImpl<kvs::UInt16>( data->GetPointData(),
                                                     data->GetNumberOfPoints(), component_count );
        case 2:
            return ::GetValueArrayImpl<kvs::UInt32>( data->GetPointData(),
                                                     data->GetNumberOfPoints(), component_count );
        case 3:
            return ::GetValueArrayImpl<kvs::UInt64>( data->GetPointData(),
                                                     data->GetNumberOfPoints(), component_count );
        case 4:
            return ::GetValueArrayImpl<kvs::Int8>( data->GetPointData(), data->GetNumberOfPoints(),
                                                   component_count );
        case 5:
            return ::GetValueArrayImpl<kvs::Int16>( data->GetPointData(), data->GetNumberOfPoints(),
                                                    component_count );
        case 6:
            return ::GetValueArrayImpl<kvs::Int32>( data->GetPointData(), data->GetNumberOfPoints(),
                                                    component_count );
        case 7:
            return ::GetValueArrayImpl<kvs::Int64>( data->GetPointData(), data->GetNumberOfPoints(),
                                                    component_count );
        case 8:
            return ::GetValueArrayImpl<kvs::Real32>( data->GetPointData(),
                                                     data->GetNumberOfPoints(), component_count );
        default:
            return ::GetValueArrayImpl<kvs::Real64>( data->GetPointData(),
                                                     data->GetNumberOfPoints(), component_count );
        }
    }
    else
    {
        return ::GetValueArrayImpl<kvs::Real64>( data->GetPointData(), data->GetNumberOfPoints(),
                                                 0 );
    }
}

kvs::UnstructuredVolumeObject::CellType GetKvsCellType( int type )
{
    switch ( type )
    {
    case VTK_TETRA:
        return kvs::UnstructuredVolumeObject::Tetrahedra;
    case VTK_QUADRATIC_TETRA:
        return kvs::UnstructuredVolumeObject::QuadraticTetrahedra;
    case VTK_HEXAHEDRON:
    case VTK_VOXEL:
        return kvs::UnstructuredVolumeObject::Hexahedra;
    case VTK_QUADRATIC_HEXAHEDRON:
        return kvs::UnstructuredVolumeObject::QuadraticHexahedra;
    case VTK_VERTEX:
        return kvs::UnstructuredVolumeObject::Point;
    case VTK_WEDGE:
    case VTK_QUADRATIC_WEDGE:
        return kvs::UnstructuredVolumeObject::Prism;
    case VTK_PYRAMID:
    case VTK_QUADRATIC_PYRAMID:
        return kvs::UnstructuredVolumeObject::Pyramid;
    default:
        return kvs::UnstructuredVolumeObject::UnknownCellType;
    }
}

template <typename K, typename V, typename O>
void Reorder( K& kvs, V& vtk, O& order, int n )
{
    for ( int i = 0; i < n; ++i )
    {
        *kvs = vtk->GetId( order[i] );
        ++kvs;
    }
}

template <typename DestinationIterator>
void ReorderElementNodeIndices( DestinationIterator& kvs_connection, vtkIdList* vtk_cell,
                                int vtk_cell_type )
{
    switch ( vtk_cell_type )
    {
    case VTK_TRIANGLE: {
        constexpr int order[] = { 0, 1, 2 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_PIXEL: {
        constexpr int order[] = { 0, 1, 3, 2 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_QUAD: {
        constexpr int order[] = { 0, 1, 2, 3 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_TETRA: {
        constexpr int order[] = { 0, 3, 2, 1 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_QUADRATIC_TETRA: {
        constexpr int order[] = { 0, 3, 2, 1, 7, 6, 4, 9, 5, 8 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_HEXAHEDRON: {
        constexpr int order[] = { 4, 5, 6, 7, 0, 1, 2, 3 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_VOXEL: {
        constexpr int order[] = { 4, 5, 7, 6, 0, 1, 3, 2 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_QUADRATIC_HEXAHEDRON: {
        constexpr int order[] = { 7,  4,  5,  6, 3, 0,  1,  2,  15, 12,
                                  13, 14, 11, 8, 9, 10, 19, 16, 17, 18 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_VERTEX: {
        constexpr int order[] = { 0 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_PYRAMID:
    case VTK_QUADRATIC_PYRAMID: {
        constexpr int order[] = { 4, 0, 1, 2, 3 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    case VTK_WEDGE:
    case VTK_QUADRATIC_WEDGE: {
        constexpr int order[] = { 0, 1, 2, 3, 4, 5 };
        ::Reorder( kvs_connection, vtk_cell, order, sizeof( order ) / sizeof( int ) );
        break;
    }
    default:
        throw std::runtime_error( "Unsupported VTK cell type" );
    }
}
} // namespace

void kvs::ExtendedFileFormat::detail::ImportPolygonObject( kvs::PolygonObject* polygon_object,
                                       vtkSmartPointer<vtkPolyData> data )
{
    auto coords = ::GetCoordinates( data );
    polygon_object->setCoords( coords );

    // Normals
    std::vector<kvs::Real32> normals( data->GetNumberOfCells() * 3 );
    auto stl_normals = data->GetCellData()->GetNormals();

    if ( !stl_normals )
    {
        stl_normals = data->GetCellData()->GetArray( "cellNormals" );
    }

    if ( !stl_normals )
    {
        vtkNew<vtkPolyDataNormals> normals_filter;

        normals_filter->SetInputData( data );
        normals_filter->AutoOrientNormalsOn();
        normals_filter->ComputeCellNormalsOn();
        normals_filter->ComputePointNormalsOff();
        normals_filter->SetOutputPointsPrecision(
            vtkAlgorithm::DesiredOutputPrecision::SINGLE_PRECISION );

        normals_filter->Update();

        stl_normals = normals_filter->GetOutput()->GetCellData()->GetArray( "Normals" );
        data->GetCellData()->AddArray( stl_normals );

        if ( !stl_normals )
        {
            throw std::runtime_error( "Failed to read normals" );
        }
    }

    auto n_head = normals.begin();
    for ( vtkIdType i = 0; i < data->GetNumberOfCells(); ++i )
    {
        auto n = stl_normals->GetTuple( i );

        *n_head = static_cast<kvs::Real32>( n[0] );
        *( n_head + 1 ) = static_cast<kvs::Real32>( n[1] );
        *( n_head + 2 ) = static_cast<kvs::Real32>( n[2] );

        n_head += 3;
    }

    auto v_normals = kvs::ValueArray<kvs::Real32>( normals );

    // Connectivity
    auto cell_array = data->GetPolys();
    std::vector<kvs::UInt32> connections( cell_array->GetNumberOfConnectivityIds() );

    bool is_triangle_mesh = data->GetCell( 0 ) && data->GetCell( 0 )->GetCellType() == VTK_TRIANGLE;
    auto head = connections.begin();
    vtkNew<vtkIdList> cell_ids;
    for ( vtkIdType i = 0; i < data->GetNumberOfCells(); ++i )
    {
        auto cell = data->GetCell( i );

        if ( is_triangle_mesh )
        {
            if ( cell->GetCellType() == VTK_TRIANGLE )
            {
                data->GetCellPoints( i, cell_ids );
                ::ReorderElementNodeIndices( head, cell_ids, VTK_TRIANGLE );
            }
        }
        else
        {
            if ( cell->GetCellType() == VTK_QUAD || cell->GetCellType() == VTK_PIXEL )
            {
                data->GetCellPoints( i, cell_ids );
                ::ReorderElementNodeIndices( head, cell_ids, cell->GetCellType() );
            }
        }
    }
    connections.resize( std::distance( connections.begin(), head ) );
    auto v_connections = kvs::ValueArray<kvs::UInt32>( connections );

    polygon_object->setPolygonType( ( is_triangle_mesh )
                                        ? kvs::PolygonObject::PolygonType::Triangle
                                        : kvs::PolygonObject::PolygonType::Quadrangle );
    polygon_object->setNormalType( kvs::PolygonObject::PolygonNormal );
    polygon_object->setNormals( v_normals );
    polygon_object->setConnections( v_connections );
    polygon_object->updateMinMaxCoords();
}

void kvs::ExtendedFileFormat::detail::ImportPolygonObject( kvs::PolygonObject* polygon_object,
                                       vtkSmartPointer<vtkUnstructuredGrid> data )
{
    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints( data->GetPoints() );
    polydata->SetPolys( data->GetCells() );

    kvs::ExtendedFileFormat::detail::ImportPolygonObject( polygon_object, polydata );
}

void kvs::ExtendedFileFormat::detail::ImportRectilinearObject( kvs::StructuredVolumeObject* rectilinear_object,
                                           vtkSmartPointer<vtkRectilinearGrid> data )
{
    kvs::Vector3ui resolution = ::GetResolution( data );

    auto dimensions = data->GetDimensions();
    kvs::StructuredVolumeObject::Coords xyz_coords(
        std::accumulate( dimensions, dimensions + 3, 0, std::plus<int>() ) );
    auto coords_itr = xyz_coords.begin();

    {
        auto x = data->GetXCoordinates();
        for ( int i = 0; i < x->GetNumberOfTuples(); ++i )
        {
            *( coords_itr++ ) = static_cast<float>( x->GetTuple( i )[0] );
        }
    }
    {
        auto y = data->GetYCoordinates();
        for ( int i = 0; i < y->GetNumberOfTuples(); ++i )
        {
            *( coords_itr++ ) = static_cast<float>( y->GetTuple( i )[0] );
        }
    }
    {
        auto z = data->GetZCoordinates();
        for ( int i = 0; i < z->GetNumberOfTuples(); ++i )
        {
            *( coords_itr++ ) = static_cast<float>( z->GetTuple( i )[0] );
        }
    }

    rectilinear_object->setGridType( kvs::StructuredVolumeObject::Rectilinear );
    rectilinear_object->setResolution( resolution );
    rectilinear_object->setCoords( xyz_coords );

    auto component_count = ::GetCountOfComponents( data );
    rectilinear_object->setVeclen( component_count );
    if ( component_count != 0 )
    {
        auto values = ::GetValueArray( data, component_count );
        rectilinear_object->setValues( values );
    }

    rectilinear_object->updateMinMaxValues();
}

void kvs::ExtendedFileFormat::detail::ImportUniformStructuredVolumeObject( kvs::StructuredVolumeObject* uniform_object,
                                                       vtkSmartPointer<vtkImageData> data )
{
    auto bounding_box = data->GetBounds();
    kvs::Vector3f min_obj_coord( bounding_box[0], bounding_box[2], bounding_box[4] );
    kvs::Vector3f max_obj_coord( bounding_box[1], bounding_box[3], bounding_box[5] );
    uniform_object->setMinMaxObjectCoords( min_obj_coord, max_obj_coord );
    uniform_object->setMinMaxExternalCoords( min_obj_coord, max_obj_coord );

    uniform_object->setGridType( kvs::StructuredVolumeObject::Uniform );
    uniform_object->setResolution( ::GetResolution( data ) );

    auto component_count = ::GetCountOfComponents( data );
    uniform_object->setVeclen( component_count );
    if ( component_count != 0 )
    {
        auto values = ::GetValueArray( data, component_count );
        uniform_object->setValues( values );
        uniform_object->updateMinMaxValues();
    }
}

void kvs::ExtendedFileFormat::detail::ImportIrregularStructuredVolumeObject(
    kvs::StructuredVolumeObject* irregular_object, vtkSmartPointer<vtkStructuredGrid> data )
{
    auto bounding_box = data->GetBounds();
    kvs::Vector3f min_obj_coord( bounding_box[0], bounding_box[2], bounding_box[4] );
    kvs::Vector3f max_obj_coord( bounding_box[1], bounding_box[3], bounding_box[5] );
    irregular_object->setMinMaxObjectCoords( min_obj_coord, max_obj_coord );

    irregular_object->setGridType( kvs::StructuredVolumeObject::Curvilinear );
    irregular_object->setResolution( ::GetResolution( data ) );

    auto v_coords = GetCoordinates( data );
    irregular_object->setCoords( v_coords );

    auto component_count = ::GetCountOfComponents( data );
    irregular_object->setVeclen( component_count );
    if ( component_count != 0 )
    {
        auto values = ::GetValueArray( data, component_count );
        irregular_object->setValues( values );
        irregular_object->updateMinMaxValues();
    }
    irregular_object->updateMinMaxCoords();
}

void kvs::ExtendedFileFormat::detail::ImportUnstructuredVolumeObject( kvs::UnstructuredVolumeObject* object,
                                                  vtkSmartPointer<vtkUnstructuredGrid> data )
{
    object->setNumberOfNodes( data->GetNumberOfPoints() );
    object->setNumberOfCells( data->GetNumberOfCells() );

    auto first_cell = data->GetCell( 0 );
    auto cell_type = first_cell->GetCellType();
    object->setCellType( ::GetKvsCellType( cell_type ) );

    auto coords = GetCoordinates( data );
    object->setCoords( coords );

    auto cell_point_count = first_cell->GetNumberOfPoints();
    kvs::UnstructuredVolumeObject::Connections connections( cell_point_count *
                                                            data->GetNumberOfCells() );

    auto head = connections.begin();
    for ( vtkIdType i = 0; i < data->GetNumberOfCells(); ++i )
    {
        auto cell = data->GetCell( i );

        if ( cell->GetCellType() != cell_type )
        {
            throw std::runtime_error( "hetero cell types" );
        }

        ::ReorderElementNodeIndices( head, cell->GetPointIds(), cell->GetCellType() );
    }
    object->setConnections( connections );

    auto component_count = ::GetCountOfComponents( data );

    object->setVeclen( component_count );
    if ( component_count != 0 )
    {
        auto values = ::GetValueArray( data, component_count );
        object->setValues( values );
        object->updateMinMaxValues();
    }

    object->updateMinMaxCoords();
}

void kvs::ExtendedFileFormat::detail::ImportPointObject( kvs::PointObject* object,
                                     vtkSmartPointer<vtkUnstructuredGrid> data )
{
    auto coords = GetCoordinates( data );
    object->setCoords( coords );

    std::vector<kvs::Real32> normals( data->GetNumberOfPoints() * 3, 0 );
    for ( int i = 0; i < data->GetNumberOfPoints(); ++i )
    {
        normals[i * 3] = 1.0;
    }
    auto v_normals = kvs::ValueArray<kvs::Real32>( normals );
    object->setNormals( v_normals );

    object->updateMinMaxCoords();
}

void kvs::ExtendedFileFormat::detail::ImportLineObject( kvs::LineObject* object,
                                    vtkSmartPointer<vtkUnstructuredGrid> data )
{
    object->setLineType( kvs::LineObject::Segment );

    auto coords = GetCoordinates( data );
    object->setCoords( coords );

    std::deque<kvs::UInt32> connection_deque;
    for ( vtkIdType i = 0; i < data->GetNumberOfCells(); ++i )
    {
        auto cell = data->GetCell( i );

        switch ( cell->GetCellType() )
        {
        case VTK_LINE: {
            connection_deque.push_back( static_cast<kvs::UInt32>( cell->GetPointId( 0 ) ) );
            connection_deque.push_back( static_cast<kvs::UInt32>( cell->GetPointId( 1 ) ) );
            break;
        }
        case VTK_POLY_LINE: {
            auto ids = cell->GetPointIds();
            for ( int j = 0; j < ids->GetNumberOfIds() - 1; ++j )
            {
                connection_deque.push_back( static_cast<kvs::UInt32>( cell->GetPointId( j ) ) );
                connection_deque.push_back( static_cast<kvs::UInt32>( cell->GetPointId( j + 1 ) ) );
            }
            break;
        }
        }
    }
    kvs::ValueArray<kvs::UInt32> connections( connection_deque.size() );
    std::copy( connection_deque.begin(), connection_deque.end(), connections.begin() );
    object->setConnections( connections );

    object->updateMinMaxCoords();
}
