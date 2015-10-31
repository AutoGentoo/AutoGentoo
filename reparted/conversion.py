#This file is part of reparted.

#reparted is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#reparted is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with reparted.  If not, see <http://www.gnu.org/licenses/>.

from ctypes.util import find_library
from ctypes import *

lib = find_library("parted")

if not lib:
    raise Exception("Parted library not found.")

parted = CDLL(lib)

class PedCHSGeometry(Structure):
    _fields_ = [
        ('cylinders', c_int),
        ('heads', c_int),
        ('sectors', c_int),
    ]

PedSector = c_longlong

class PedAlignment(Structure):
    _fields_ = [
        ('offset', PedSector),
        ('grain_size', PedSector),
     ]

class PedDevice(Structure):
    pass

PedDevice._fields_ = [
    ('next', POINTER(PedDevice)),
    ('model', c_char_p),
    ('path', c_char_p),
    ('type', c_int),
    ('sector_size', c_longlong),
    ('phys_sector_size', c_longlong),
    ('length', PedSector),
    ('open_count', c_int),
    ('read_only', c_int),
    ('external_mode', c_int),
    ('dirty', c_int),
    ('boot_dirty', c_int),
    ('hw_geom', PedCHSGeometry),
    ('bios_geom', PedCHSGeometry),
    ('host', c_short),
    ('did', c_short),
    ('arch_specific', c_void_p),
]

class PedGeometry(Structure):
    _fields_ = [
        ('dev', POINTER(PedDevice)),
        ('start', PedSector),
        ('length', PedSector),
        ('end', PedSector),
     ]

class PedPartition(Structure):
    pass

class PedDiskType(Structure):
    pass

class PedFileSystemType(Structure):
    pass

class PedDisk(Structure):
    _fields_ = [
        ('dev', POINTER(PedDevice)),
        ('type', POINTER(PedDiskType)),
        ('block_sizes', c_int),
        ('part_list', POINTER(PedPartition)),
        ('disk_specific', c_void_p),
        ('needs_clobber', c_int),
        ('update_mode', c_int),
     ]

class PedDiskOps(Structure):
    _fields_ = [
        ('probe', c_int),
        ('clobber', c_int),
        ('alloc', POINTER(PedDisk)),
        ('duplicate', POINTER(PedDisk)),
        ('free', c_void_p),
        ('read', c_int),
        ('write', c_int),
        ('disk_set_flag', c_int),
        ('disk_get_flag', c_int),
        ('disk_is_flag_available', c_int),
        ('partition_new', POINTER(PedPartition)),
        ('partition_duplicate', POINTER(PedPartition)),
        ('partition_destroy', c_void_p),
        ('partition_set_system', c_int),
        ('partition_set_flag', c_int),
        ('partition_get_flag', c_int),
        ('partition_is_flag_available', c_int),
        ('partition_set_name', c_void_p),
        ('partition_get_name', c_char_p),
        ('partition_align', c_int),
        ('partition_enumerate', c_int),
        ('partition_check', c_bool),
        ('alloc_metadata', c_int),
        ('get_max_primary_partition_count', c_int),
        ('get_max_supported_partition_count', c_bool),
        ('get_partition_alignment', POINTER(PedAlignment)),
        ('max_length', PedSector),
        ('max_start_sector', PedSector)
    ]

PedDiskType._fields_ = [
    ('next', POINTER(PedDiskType)),
    ('name', c_char_p),
    ('ops', POINTER(PedDiskOps)),
    ('features', c_int)
]

PedFileSystemType._fields_ = [
    ('next', POINTER(PedFileSystemType)),
    ('name', c_char_p),
    ('block_sizes', c_int),
    ('ops', POINTER(PedDiskOps)),
]

PedPartition._fields_ = [
    ('prev', POINTER(PedPartition)),
    ('next', POINTER(PedPartition)),
    ('disk', POINTER(PedDisk)),
    ('geom', PedGeometry),
    ('num', c_int),
    ('type', c_long),
    ('fs_type', POINTER(PedFileSystemType)),
    ('part_list', POINTER(PedPartition)),
    ('disk_specific', c_void_p),
]

class PedConstraint(Structure):
    _fields_ = [
        ('start_align', POINTER(PedAlignment)),
        ('end_align', POINTER(PedAlignment)),
        ('start_range', POINTER(PedGeometry)),
        ('end_range', POINTER(PedGeometry)),
        ('min_size', PedSector),
        ('max_size', PedSector),
     ]

# Device Function conversions
device_get = parted.ped_device_get
device_get.restype = POINTER(PedDevice)
device_get_constraint = parted.ped_device_get_constraint
device_get_constraint.restype = POINTER(PedConstraint)
device_get_optimal_aligned_constraint = parted.ped_device_get_optimal_aligned_constraint
device_get_optimal_aligned_constraint.argtypes = [POINTER(PedDevice)]
device_get_optimal_aligned_constraint.restype = POINTER(PedConstraint)
device_get_minimal_aligned_constraint = parted.ped_device_get_minimal_aligned_constraint
device_get_minimal_aligned_constraint.argtypes = [POINTER(PedDevice)]
device_get_minimal_aligned_constraint.restype = POINTER(PedConstraint)
device_get_optimum_alignment = parted.ped_device_get_optimum_alignment
device_get_optimum_alignment.argtypes = [POINTER(PedDevice)]
device_get_optimum_alignment.restype = POINTER(PedAlignment)
device_get_minimum_alignment = parted.ped_device_get_minimum_alignment
device_get_minimum_alignment.argtypes = [POINTER(PedDevice)]
device_get_minimum_alignment.restype = POINTER(PedAlignment)
device_get__constraint = parted.ped_device_get_constraint
device_get_constraint.argtypes = [POINTER(PedDevice)]
device_get_constraint.restype = POINTER(PedConstraint)

# Disk Function conversions
disk_probe = parted.ped_disk_probe
disk_probe.restype = POINTER(PedDiskType)
disk_new = parted.ped_disk_new
disk_new.restype = POINTER(PedDisk)
disk_new_fresh = parted.ped_disk_new_fresh
disk_new_fresh.argtypes = [POINTER(PedDevice), POINTER(PedDiskType)]
disk_new_fresh.restype = POINTER(PedDisk)
disk_add_partition = parted.ped_disk_add_partition
disk_add_partition.argtypes = [POINTER(PedDisk), POINTER(PedPartition), POINTER(PedConstraint)]
disk_next_partition = parted.ped_disk_next_partition
disk_next_partition.argtypes = [POINTER(PedDisk), POINTER(PedPartition)]
disk_next_partition.restype = POINTER(PedPartition)
disk_get_last_partition_num = parted.ped_disk_get_last_partition_num
disk_get_last_partition_num.argtypes = [POINTER(PedDisk)]
disk_get_partition = parted.ped_disk_get_partition
disk_get_partition.argtypes = [POINTER(PedDisk), c_int]
disk_get_partition.restype = POINTER(PedPartition)
disk_delete_partition = parted.ped_disk_delete_partition
disk_delete_partition.argtypes = [POINTER(PedDisk), POINTER(PedPartition)]
disk_delete_all = parted.ped_disk_delete_all
disk_delete_all.argtypes = [POINTER(PedDisk)]
disk_commit_to_os = parted.ped_disk_commit_to_os
disk_commit_to_os.argtypes = [POINTER(PedDisk)]
disk_commit_to_dev = parted.ped_disk_commit_to_dev
disk_commit_to_dev.argtypes = [POINTER(PedDisk)]
disk_destroy = parted.ped_disk_destroy
disk_destroy.argtypes = [POINTER(PedDisk)]
disk_destroy.restype = None
disk_get_type = parted.ped_disk_type_get
disk_get_type.restype = POINTER(PedDiskType)
disk_remove_partition = parted.ped_disk_remove_partition
disk_remove_partition.argtypes = [POINTER(PedDisk), POINTER(PedPartition)]

# Partition Function conversions
partition_new = parted.ped_partition_new
partition_new.argtypes = [POINTER(PedDisk), c_int, POINTER(PedFileSystemType), PedSector, PedSector]
partition_new.restype = POINTER(PedPartition)
partition_is_busy = parted.ped_partition_is_busy
partition_is_busy.argtypes = [POINTER(PedPartition)]
partition_get_name = parted.ped_partition_get_name
partition_get_name.argtypes = [POINTER(PedPartition)]
partition_get_name.restype = c_char_p
partition_set_name = parted.ped_partition_set_name
partition_set_name.argtypes = [POINTER(PedPartition), c_char_p]
partition_is_flag_available = parted.ped_partition_is_flag_available
partition_is_flag_available.argtypes = [POINTER(PedPartition), c_int]
partition_set_flag = parted.ped_partition_set_flag
partition_set_flag.argtypes = [POINTER(PedPartition), c_int, c_int]
geometry_new = parted.ped_geometry_new
geometry_new.argtypes = [POINTER(PedDevice), PedSector, PedSector]
geometry_new.restype = POINTER(PedGeometry)
constraint_new = parted.ped_constraint_new
constraint_new.argtypes = [POINTER(PedAlignment), POINTER(PedAlignment), POINTER(PedGeometry), POINTER(PedGeometry), PedSector, PedSector]
constraint_new.restype = POINTER(PedConstraint)
constraint_intersect = parted.ped_constraint_intersect
constraint_intersect.argtypes = [POINTER(PedConstraint), POINTER(PedConstraint)]
constraint_intersect.restype = POINTER(PedConstraint)
constraint_destroy = parted.ped_constraint_destroy
constraint_destroy.argtypes = [POINTER(PedConstraint)]
file_system_type_get = parted.ped_file_system_type_get
file_system_type_get.argtypes = [c_char_p]
file_system_type_get.restype = POINTER(PedFileSystemType)