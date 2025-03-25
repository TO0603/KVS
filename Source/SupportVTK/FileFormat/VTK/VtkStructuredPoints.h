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
#ifndef CVT__VTK_STRUCTURED_POINTS_H_INCLUDE
#define CVT__VTK_STRUCTURED_POINTS_H_INCLUDE
#include "kvs/StructuredVolumeObject"
#include <vtkImageData.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPointsWriter.h>

#include <kvs/vtk/VtkFileFormat>

namespace kvs
{
namespace vtk
{

/**
 * A VTK StructuredPoints (ImageData) file IO.
 */
using VtkStructuredPoints =
    typename kvs::vtk::VtkFileFormat<vtkImageData, vtkStructuredPointsReader, vtkStructuredPointsWriter>;
} // namespace vtk
} // namespace kvs

#endif // CVT__VTK_STRUCTURED_POINTS_H_INCLUDE