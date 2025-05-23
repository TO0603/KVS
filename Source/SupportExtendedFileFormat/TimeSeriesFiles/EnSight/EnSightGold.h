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
#ifndef EXTENDED_FILE_FORMAT__ENSIGHT_GOLD_H_INCLUDE
#define EXTENDED_FILE_FORMAT__ENSIGHT_GOLD_H_INCLUDE

#include <string>

#include <vtkCompositeDataSet.h>
#include <vtkEnSightGoldBinaryReader.h>
#include <vtkEnSightGoldReader.h>

#include <vtkDataArray.h>
#include <vtkDataArrayCollection.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkSmartPointer.h>

#include <kvs/extendedfileformat/VtkXmlMultiBlock>

namespace kvs
{
namespace ExtendedFileFormat
{
namespace detail
{
template <typename VtkEnSightGoldReader>
class EnSightTimeStepIterator
{
public:
    EnSightTimeStepIterator( vtkSmartPointer<VtkEnSightGoldReader> reader,
                             vtkSmartPointer<vtkDataArray> time_steps, int current = 0 ):
        reader( reader ), time_steps( time_steps ), current( current )
    {
    }

public:
    std::pair<double, kvs::ExtendedFileFormat::VtkXmlMultiBlock> operator*()
    {
        auto t = time_steps->GetTuple( current )[0];
        reader->SetTimeValue( t );
        reader->Update();

        return std::make_pair( t, kvs::ExtendedFileFormat::VtkXmlMultiBlock( reader->GetOutput() ) );
    }
    EnSightTimeStepIterator<VtkEnSightGoldReader> operator++()
    {
        ++current;
        return *this;
    }
    EnSightTimeStepIterator<VtkEnSightGoldReader> operator++( int )
    {
        return EnSightTimeStepIterator<VtkEnSightGoldReader>( reader, time_steps, current++ );
    }
    bool operator==( const EnSightTimeStepIterator<VtkEnSightGoldReader>& other ) noexcept
    {
        return current == other.current;
    }
    bool operator==( const EnSightTimeStepIterator<VtkEnSightGoldReader>& other ) const noexcept
    {
        return current == other.current;
    }
    bool operator!=( const EnSightTimeStepIterator<VtkEnSightGoldReader>& other ) noexcept
    {
        return current != other.current;
    }
    bool operator!=( const EnSightTimeStepIterator<VtkEnSightGoldReader>& other ) const noexcept
    {
        return current != other.current;
    }

private:
    vtkSmartPointer<VtkEnSightGoldReader> reader;
    vtkSmartPointer<vtkDataArray> time_steps;
    int current;
};

template <typename VtkEnSightGoldReader>
class EnSightTimeStepContainer
{
public:
    EnSightTimeStepContainer( vtkSmartPointer<VtkEnSightGoldReader> reader ): reader( reader )
    {
        auto time_sets = reader->GetTimeSets();
        if ( time_sets->GetNumberOfItems() > 0 )
        {
            time_steps = time_sets->GetItem( 0 );
        }
        else
        {
            throw std::runtime_error( "No time steps" );
        }
    }

public:
    int lastTimeStep() { return time_steps->GetNumberOfTuples() - 1; }
    int lastTimeStep() const { return time_steps->GetNumberOfTuples() - 1; }
    kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader> begin()
    {
        return kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader>( reader, time_steps, 0 );
    }
    kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader> begin() const
    {
        return kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader>( reader, time_steps, 0 );
    }
    kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader> end()
    {
        return kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader>(
            reader, time_steps, time_steps->GetNumberOfTuples() );
    }
    kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader> end() const
    {
        return kvs::ExtendedFileFormat::detail::EnSightTimeStepIterator<VtkEnSightGoldReader>(
            reader, time_steps, time_steps->GetNumberOfTuples() );
    }

private:
    vtkSmartPointer<VtkEnSightGoldReader> reader;
    vtkSmartPointer<vtkDataArray> time_steps;
};
} // namespace detail
} // namespace ExtendedFileFormat
} // namespace kvs

namespace kvs
{
namespace ExtendedFileFormat
{

/**
 * An interface of an EnSight Gold file.
 *
 * This class is to access each time step data in an EnSight Gold file.
 *
 * \tparam VtkEnSightGoldReader `vtkEnSightGoldReader` or `vtkEnSightGoldBinaryReader`.
 */
template <typename VtkEnSightGoldReader = vtkEnSightGoldReader>
class EnSightGold
{
public:
    /**
     * Construct an IO.
     *
     * \param[in] filename A file name.
     */
    EnSightGold( const std::string& filename )
    {
        reader = vtkSmartPointer<VtkEnSightGoldReader>::New();
        reader->ReadAllVariablesOn();
        reader->SetCaseFileName( filename.c_str() );
        reader->UpdateInformation();
    }
    /**
     * Construct an IO.
     *
     * \param[in] filename A file name.
     */
    EnSightGold( std::string&& filename )
    {
        reader = vtkSmartPointer<VtkEnSightGoldReader>::New();
        reader->ReadAllVariablesOn();
        reader->SetCaseFileName( filename.c_str() );
        reader->ReadAllVariablesOn();
        reader->UpdateInformation();
    }

public:
    /**
     * Get an interface to iterate each time step file format.
     *
     * e.g.
     *
     * ```c++
     * kvs::ExtendedFileFormat::EnSightGold ensight;
     *
     * for (auto file_format : ensight.eachTimeStep()) {
     *     // ...
     * }
     * ```
     *
     * \return An interface to iterate each time step file format.
     */
    kvs::ExtendedFileFormat::detail::EnSightTimeStepContainer<VtkEnSightGoldReader> eachTimeStep()
    {
        return kvs::ExtendedFileFormat::detail::EnSightTimeStepContainer<VtkEnSightGoldReader>( reader );
    }
    /**
     * Get an interface to iterate each time step file format.
     *
     * e.g.
     *
     * ```c++
     * kvs::ExtendedFileFormat::EnSightGold ensight;
     *
     * for (auto file_format : ensight.eachTimeStep()) {
     *     // ...
     * }
     * ```
     *
     * \return An interface to iterate each time step file format.
     */
    kvs::ExtendedFileFormat::detail::EnSightTimeStepContainer<VtkEnSightGoldReader> eachTimeStep() const
    {
        return kvs::ExtendedFileFormat::detail::EnSightTimeStepContainer<VtkEnSightGoldReader>( reader );
    }
    /**
     * Get the last time step.
     *
     * This function does NOT return a time step count.
     * If there are 50 files for [0-49] time steps, this returns 49.
     *
     * \return The last time step. Zero-based numbering.
     */
    int lastTimeStep()
    {
        kvs::ExtendedFileFormat::detail::EnSightTimeStepContainer<VtkEnSightGoldReader> container( reader );
        return container.lastTimeStep();
    }
    /**
     * Get the last time step.
     *
     * This function does NOT return a time step count.
     * If there are 50 files for [0-49] time steps, this returns 49.
     *
     * \return The last time step. Zero-based numbering.
     */
    int lastTimeStep() const
    {
        kvs::ExtendedFileFormat::detail::EnSightTimeStepContainer<VtkEnSightGoldReader> container( reader );
        return container.lastTimeStep();
    }

private:
    vtkSmartPointer<VtkEnSightGoldReader> reader;
};
} // namespace ExtendedFileFormat
} // namespace kvs

#endif // CVT__ENSIGHT_GOLD_H_INCLUDE