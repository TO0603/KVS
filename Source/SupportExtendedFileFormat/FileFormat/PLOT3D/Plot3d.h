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
#ifndef EXTENDED_FILE_FORMAT__PLOT3D_H_INCLUDE
#define EXTENDED_FILE_FORMAT__PLOT3D_H_INCLUDE
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiBlockPLOT3DReader.h>

#include <kvs/extendedfileformat/VtkCompositeDataSetFileFormat>

namespace kvs
{
namespace ExtendedFileFormat
{

/**
 * A PLOT3D file reader.
 */
using Plot3d = kvs::ExtendedFileFormat::VtkCompositeDataSetFileFormat<vtkMultiBlockDataSet, vtkMultiBlockPLOT3DReader>;
} // namespace ExtendedFileFormat
} // namespace kvs
#endif // CVT__PLOT3D_H_INCLUDE