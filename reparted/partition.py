from conversion import *
from exception import *
from size import Size
import os

partition_type = {
    0 : 'NORMAL',
    1 : 'LOGICAL',
    2 : 'EXTENDED',
    4 : 'FREESPACE',
    8 : 'METADATA',
    10 : 'PROTECTED'
}

partition_flag = {
    "BOOT" : 1,
    "ROOT" : 2,
    "SWAP" : 3,
    "HIDDEN" : 4,
    "RAID" : 5,
    "LVM" : 6,
    "LBA" : 7,
    "HPSERVICE" : 8,
    "PALO" : 9,
    "PREP" : 10,
    "MSFT_RESERVED" : 11,
    "BIOS_GRUB" : 12,
    "APPLE_TV_RECOVERY" : 13,
    "DIAG" : 14,
    "LEGACY_BOOT" : 15
}

valid_types = {
    'gpt' : ['NORMAL'],
    'msdos' : ['NORMAL', 'LOGICAL', 'EXTENDED']
}

class Partition(object):
    """
    *Partition class is used as a wrapper to libparted's ped_partition.*

    You need can create Partition instances and add them to disk. A new
    Partition instance can be initialized::

        from reparted import *

        myDevice = Device("/dev/sda")
        myDisk = Disk(myDevice)
        mySize = Size(4, "GB")

        # Defaults
        myPartition = Partition(myDisk, mySize)

        # Different filesystem and minimal alignment.
        myPartition = Partition(myDisk, mySize, fs="ext4", align="minimal")

        # Initialize with a name
        if myDisk.type_features == 'PARTITION_NAME':
            myPartition = Partition(myDisk, mySize, name="test")

    *Args:*

    *   disk:           A Disk class instance.
    *   size:           A Size class instance.
    *   type (str):     The partition type (ie. 'NORMAL', 'LOGICAL', etc...).
    *   fs (str):       The filesystem type (ie. 'ext3', 'ext4', etc...).
    *   align (str):    The partition alignment, 'minimal' or 'optimal'.
    *   name (str):     The partition name.
    *   start (int):    The start sector for the partition.
    *   end (int):      The end sector for the partition.
    *   part:           A ctypes ped_partition pointer.

    *Raises:*

    *    PartitionError

    .. note::

       Name is only available when partition type is 'NORMAL'.
       The start and end arguments are optional and you should only use them when
       your want to specify such attributes, otherwise use optimal alignment.
       The part argument is optional and mostly for internal use.
    """
    def __init__(self, disk, size=None, type='NORMAL', fs='ext3', align='optimal',
                    name='', start=None, end=None, part=None):
        self._disk = disk
        if part:
            self._align = None
            self._partition = part
            sectors = self._partition.contents.geom.length
            ln = sectors * disk.device.sector_size
            self._size = Size(length=ln, units='B', dev=disk.device)
        elif size:
            self._align = align
            self._verify_type(type)
            if type != 'EXTENDED' and fs != None:
                filesystem = file_system_type_get(fs)
            else:
                filesystem = None
            if align == 'optimal' or align == 'minimal':
                dev = disk._ped_device
                a_start, a_end = self._get_alignment(dev, align, start, end, size, type)
            else:
                raise PartitionError(708)
            part_type = [key for key,val in partition_type.iteritems() if val == type][0]
            self._partition = partition_new(disk._ped_disk, part_type, filesystem, a_start, a_end)
            sectors = self._partition.contents.geom.length
            size.sectors = sectors
            self._size = size
            if name:
                self.set_name(name)
        else:
            raise PartitionError(700)

    def _verify_type(self, p_type):
        if p_type not in valid_types.get(self._disk.type_name):
            raise PartitionError(711)
        if p_type == 'LOGICAL' or p_type == 'EXTENDED':
            ext = [p for p in self._disk.partitions() if p.type == 'EXTENDED']
            if not ext and p_type == 'LOGICAL':
                raise PartitionError(713)
            if ext and p_type == 'EXTENDED':
                raise PartitionError(714)

    @property
    def disk(self):
        """
        Returns the Disk instance this partition belongs to.
        """
        return self._disk

    @property
    def device(self):
        """
        Returns the Device instance this partition belongs to.
        """
        return self.disk.device

    @property
    def geom(self):
        """
        Returns the partition geometry as a 3-tuple:

            (start, end, length)
        """
        start =  self._partition.contents.geom.start
        end =  self._partition.contents.geom.end
        length =  self._partition.contents.geom.length
        return (start, end, length)

    @property
    def size(self):
        """
        Returns the size as a Size class instance.
        """
        return self._size

    @property
    def num(self):
        """
        Returns the partition number. If the partition is of type 'FREESPACE'
        it will return -1.
        """
        return self._partition.contents.num

    @property
    def type(self):
        """
        Returns the partition type.
        """
        return partition_type[self._partition.contents.type]

    @property
    def fs_type(self):
        """
        Returns the partition filesystem type.
        """
        try:
            fs = self._partition.contents.fs_type.contents.name
        except ValueError:
            fs = None
        return fs

    @property
    def name(self):
        """
        Returns the partition name if names are supported by disk type,
        otherwise returns None.
        """
        if self.disk.type_features != 'PARTITION_NAME' or self.type == 'FREESPACE':
            return None
        return partition_get_name(self._partition)

    @property
    def alignment(self):
        """
        Returns the partition alignment ('optimal' or 'minimal').

        .. note::

            If you specify a 'minimal' alignment when creating a partition
            but the start sector falls in what would be considered an
            optimal alignment this method will return 'optimal'.
        """
        if self._align:
            return self._align
        else:
            optimal = device_get_optimum_alignment(self.disk._ped_device)
            minimal = device_get_minimum_alignment(self.disk._ped_device)
            start, e, l = self.geom
            if start % optimal.contents.grain_size == optimal.contents.offset:
                return 'optimal'
            if start % minimal.contents.grain_size == minimal.contents.offset:
                return 'minimal'
        return None

    def set_name(self, name):
        """
        Sets the partition name. If the disk type does not support
        partition names it will raise NotImplementedError.

        *Args:*

        *       name (str):         The partition name.

        *Raises:*

        *       NotImplementedError, PartitionError
        """
        if self.disk.type_features != 'PARTITION_NAME' or self.type == 'FREESPACE':
            raise NotImplementedError("The disk does not support partition names.")
        new_name = partition_set_name(self._partition, name)
        if not new_name:
            raise PartitionError(704)
        return

    def _snap_sectors(self, start, end, size, type):
        if start:
            if not end:
                end = start + size.sectors - 1
            if (end - start) != (size.sectors - 1):
                raise PartitionError(709)
        elif type == 'LOGICAL':
            try:
                last = [p for p in self._disk.partitions() if p.type == 'LOGICAL'][-1]
            except IndexError:
                last = [p for p in self._disk.partitions() if p.type == 'EXTENDED'][-1]
            ls, e, ln = last.geom
            start = ls + 1
            end = start + size.sectors - 1
        else:
            size = Size()
            last = None
            for part in self.disk.free_partitions():
                if part.size > size:
                    last = part
            try:
                start, e, ln = part.geom
            except AttributeError:
                raise PartitionError(712)
            end = start + size.sectors - 1
        return (start, end)

    def _get_alignment(self, dev, align, start, end, size, type):
        const = getattr(parted, "ped_device_get_%s_aligned_constraint" % align)
        constraint = const(dev)
        start_offset = constraint.contents.start_align.contents.offset
        start_grain = constraint.contents.start_align.contents.grain_size
        end_offset = constraint.contents.end_align.contents.offset
        end_grain = constraint.contents.end_align.contents.grain_size
        snap_start, snap_end = self._snap_sectors(start, end, size, type)
        if snap_start % start_grain == start_offset:
            start = snap_start
        else:
            start = ((snap_start / start_grain) + 1) * start_grain
        end = start + size.sectors
        if (end - end_offset) % end_grain != end_offset:
            end = ((end / end_grain) * end_grain) + end_offset
        return (start, end)

    def _check_flag(self, flag):
        if not flag in partition_flag.keys():
            raise PartitionError(710)
        check = partition_is_flag_available(self._partition, partition_flag[flag])
        if not check:
            raise PartitionError(710)

    def set_flag(self, flag, state):
        """
        Sets the partition flag (ie. 'BOOT', 'ROOT', etc...).

        *Args:*

        *       flag (str):         The partition flag.
        *       state (bool):       Toggle the flag state (True or False).
        """
        if self.type != 'FREESPACE':
            self._check_flag(flag)
            partition_set_flag(self._partition, partition_flag[flag], int(state))
        else:
            raise NotImplementedError("Operatin not supported on free space.")