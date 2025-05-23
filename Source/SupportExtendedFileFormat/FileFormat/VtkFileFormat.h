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
#ifndef EXTENDED_FILE_FORMAT__VTK_FILE_FORMAT_H_INCLUDE
#define EXTENDED_FILE_FORMAT__VTK_FILE_FORMAT_H_INCLUDE
#include <algorithm>
#include <functional>
#include <iterator>
#include <string>

#include "kvs/File"
#include "kvs/FileFormatBase"
#include <vtkAVSucdReader.h>
#include <vtkCompositeDataSet.h>
#include <vtkExtractGhostCells.h>
#include <vtkNew.h>
#include <vtkPCellDataToPointData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRemoveGhosts.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace kvs
{
namespace ExtendedFileFormat
{

/**
 * A file IO using VTK.
 */
template <typename VtkDataType_, typename VtkReaderType, typename VtkWriterType>
class VtkFileFormat : public kvs::FileFormatBase
{
public:
    /**
     * A base class type.
     */
    using BaseClass = kvs::FileFormatBase;
    /**
     * An inner VTK data type.
     */
    using VtkDataType = VtkDataType_;
    /**
     * A function to configure a VTK reader.
     */
    using ReaderOptionSetter = std::function<void( VtkReaderType* )>;

public:
    /**
     * Construct an empty IO object.
     *
     * This member is for exporters.
     */
    VtkFileFormat() noexcept: kvs::FileFormatBase()
    {
        set_reader_options = []( VtkReaderType* ) {};
    }
    /**
     * Construct an IO object and read a file.
     *
     * \param[in] filename A file name.
     */
    VtkFileFormat( const std::string& filename ): kvs::FileFormatBase()
    {
        set_reader_options = []( VtkReaderType* ) {};
        read( filename );
    }
    /**
     * Construct an IO object and read a file.
     *
     * \param[in] filename A file name.
     * \param[in] reader_option_setter A function to configure a VTK reader additionally.
     */
    VtkFileFormat( const std::string& filename, ReaderOptionSetter reader_option_setter ):
        kvs::FileFormatBase(), set_reader_options( reader_option_setter )
    {
        read( std::string( filename ) );
    }
    /**
     * Construct an IO object.
     *
     * \param[in] vtk_data A VTK data.
     */
    VtkFileFormat( vtkSmartPointer<VtkDataType> data ): kvs::FileFormatBase() { vtk_data = data; }
    /**
     * Construct an IO object.
     *
     * This takes the owner of the object.
     * So DO NOT free the object manually.
     *
     * \param[in] vtk_data A VTK data.
     */
    VtkFileFormat( VtkDataType* data ): kvs::FileFormatBase() { vtk_data = data; }
    virtual ~VtkFileFormat() {}

public:
    bool read( const std::string& filename ) override
    {
        BaseClass::setFilename( filename );
        BaseClass::setSuccess( false );

        try
        {
            vtkNew<VtkReaderType> reader;
            reader->SetFileName( filename.c_str() );
            set_reader_options( reader );

            if ( std::is_base_of_v<vtkCompositeDataSet, VtkDataType> )
            {
                reader->Update();

                vtk_data = dynamic_cast<VtkDataType*>( reader->GetOutput() );
            }
            else
            {
                if ( std::is_base_of_v<vtkPolyData, VtkDataType> ||
                     std::is_base_of_v<vtkUnstructuredGrid, VtkDataType> )
                {

                    vtkNew<vtkPCellDataToPointData> point_data_sampler;
                    point_data_sampler->SetInputConnection( reader->GetOutputPort() );
                    vtkNew<vtkRemoveGhosts> ghost_remover;
                    ghost_remover->SetInputConnection( point_data_sampler->GetOutputPort() );
                    ghost_remover->Update();

                    vtk_data = dynamic_cast<VtkDataType*>( ghost_remover->GetOutput() );

                    // If vtkAVSucdReader is used, A cell-based fielddata stores the material id.
                    // Remove the material id because it is not needed for visualization.
                    if ( std::is_base_of_v<vtkAVSucdReader, VtkReaderType> )
                    {
                        // vtkCompositeDataSet cannot call GetPointData method
                        if constexpr ( !std::is_base_of_v<vtkCompositeDataSet, VtkDataType> )
                        {
                            vtk_data->GetPointData()->RemoveArray( "Material Id" );
                        }
                    }
                }
                else
                {
                    vtkNew<vtkPCellDataToPointData> point_data_sampler;
                    point_data_sampler->SetInputConnection( reader->GetOutputPort() );

                    point_data_sampler->Update();

                    vtk_data = dynamic_cast<VtkDataType*>( point_data_sampler->GetOutput() );
                }
            }

            bool is_success = vtk_data->GetNumberOfCells() > 0;
            BaseClass::setSuccess( is_success );

            return is_success;
        }
        catch ( std::exception& e )
        {
            throw e;
        }
        catch ( ... )
        {
            return false;
        }
    }
    bool write( const std::string& filename ) override
    {
        BaseClass::setFilename( filename );
        BaseClass::setSuccess( false );

        try
        {
            vtkNew<VtkWriterType> writer;
            writer->SetInputData( vtk_data );
            writer->SetFileName( filename.c_str() );

            bool is_success = writer->Write() == 1;
            BaseClass::setSuccess( is_success );

            return is_success;
        }
        catch ( std::exception& e )
        {
            throw e;
        }
        catch ( ... )
        {
            return false;
        }
    }

public:
    /**
     * Get a VTK data.
     *
     * \return A VTK data.
     */
    vtkSmartPointer<VtkDataType> get() { return vtk_data; }
    /**
     * Get a VTK data.
     *
     * \return A VTK data.
     */
    vtkSmartPointer<VtkDataType> get() const { return vtk_data; }
    /**
     * Set a VTK data.
     *
     * \param[in] object A writing object.
     */
    void set( vtkSmartPointer<VtkDataType> data ) { vtk_data = data; }
    /**
     * Set a VTK data.
     *
     * \param[in] object A writing object.
     */
    void set( VtkDataType* object ) { vtk_data = object; }
    /**
     * Get ghost cell count.
     *
     * \return A count of ghost cells.
     */
    int getGhostCellCount() const
    {
        if ( vtk_data )
        {
            vtkNew<vtkExtractGhostCells> extractor;
            extractor->SetInputData( vtk_data );
            extractor->Update();

            return extractor->GetOutput()->GetNumberOfCells();
        }
        else
        {
            return 0;
        }
    }

private:
    ReaderOptionSetter set_reader_options;
    vtkSmartPointer<VtkDataType> vtk_data;
};

/**
 * A type traits for FileFormat.
 */
template <typename FileFormat>
struct VtkFileFormatTraits
{
    /**
     * An inner VTK data type.
     */
    using VtkDataType = typename FileFormat::VtkDataType;
};
} // namespace ExtendedFileFormat
} // namespace kvs

namespace kvs
{
namespace ExtendedFileFormat
{

namespace detail
{

inline bool CheckExtensionImpl( const std::string& filename, const char* const file_extensions[],
                                std::size_t n )
{
    const kvs::File file( filename );
    auto extension = file.extension();

    std::transform( extension.cbegin(), extension.cend(), extension.begin(), tolower );
    return std::any_of( file_extensions, file_extensions + n,
                        [&extension]( const char* const e ) { return extension == e; } );
}
} // namespace detail
} // namespace ExtendedFileFormat
} // namespace kvs

#endif // CVT__VTK_FILE_FORMAT_H_INCLUDE
