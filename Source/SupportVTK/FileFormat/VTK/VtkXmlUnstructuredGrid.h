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
#ifndef CVT__VTK_XML_UNSTRUCTURED_GRID_H_INCLUDE
#define CVT__VTK_XML_UNSTRUCTURED_GRID_H_INCLUDE
#include "kvs/UnstructuredVolumeObject"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <kvs/vtk/VtkUnstructuredFileFormat>

namespace kvs
{
namespace vtk
{

/**
 * A VTK XML UnstructuredGrid file IO.
 */
using VtkXmlUnstructuredGrid =
    typename kvs::vtk::VtkUnstructuredFileFormat<vtkUnstructuredGrid, vtkXMLUnstructuredGridReader,
                                            vtkXMLUnstructuredGridWriter>;
} // namespace vtk
} // namespace kvs

#endif // CVT__VTK_XML_UNSTRUCTURED_GRID_H_INCLUDE