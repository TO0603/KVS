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
#ifndef EXTENDED_FILE_FORMAT__AVS_UCD_H_INCLUDE
#define EXTENDED_FILE_FORMAT__AVS_UCD_H_INCLUDE
#include "kvs/UnstructuredVolumeObject"

#include <vtkAVSucdReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <kvs/extendedfileformat/VtkUnstructuredFileFormat>

namespace kvs
{
namespace ExtendedFileFormat
{

/**
 * An AVS ucd file format reader.
 *
 * This class does not support AVS ucd file writing.
 * This class writes data as a VTU file format.
 */
using AvsUcd = typename kvs::ExtendedFileFormat::VtkUnstructuredFileFormat<vtkUnstructuredGrid, vtkAVSucdReader,
                                                       vtkXMLUnstructuredGridWriter>;
} // namespace ExtendedFileFormat
} // namespace kvs

#endif // CVT__AVS_UCD_H_INCLUDE