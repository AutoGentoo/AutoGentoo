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

from conversion import *
from exception import *
from size import Size
from partition import Partition
from functools import wraps
import os

disk_features = {
    1 : 'EXTENDED',
    2 : 'PARTITION_NAME'
}

disk_labels = [
    "gpt",
    "msdos"
]

alignment_any = PedAlignment(0, 1)

def diskDecorator(error=False):
    """
    Wraps disk methods to check if the instance of
    Disk points to a initialized disk.
    """
    def wrap(fn):
        @wraps(fn)
        def wrapped(self, *args, **kwargs):
            if bool(self._ped_disk):
                return fn(self, *args, **kwargs)
            if error:
                raise DiskError(606)
            return None
        return wrapped
    return wrap

class Disk(object):
    """
    *Disk class is used as a wrapper to libparted's ped_disk.*

    You need to call this class to list, add or delete partitions.
    A new instance of Disk is initialized by passing the Device instance::

        from reparted import *

        myDevice = Device("/dev/sda")
        myDisk = Disk(myDevice)

    *Args:*

    *   dev:    A Device class instance.
    *   disk:   A ped_disk pointer, usually this is used internally.

    *Raises:*

    *   DiskError

    .. note::

       You need to pass either a Device instance or a ped_disk pointer.
       If a disk is being initialized (no partition table) only the
       set_label method is available, with other methods returning
       None or raising DiskError.
    """
    def __init__(self, device, disk=None):
        self._device = device
        if disk:
            self._disk = disk
        else:
            self._disk = disk_new(device._ped_device)
            if not bool(self._disk):
                raise DiskError(600)

    @property
    def _ped_device(self):
        """
        Returns the ctypes ped_device pointer.
        """
        return self._device._ped_device

    @property
    def _ped_disk(self):
        """
        Returns the ctypes ped_disk pointer.
        """
        return self._disk

    @property
    @diskDecorator()
    def type_name(self):
        """
        Returns the disk type (ie. 'gpt' or 'msdos').
        """
        return self._ped_disk.contents.type.contents.name

    @property
    @diskDecorator()
    def device(self):
        """
        Returns the Device the disk belongs to.
        """
        return self._device

    @property
    @diskDecorator()
    def type_features(self):
        """
        Returns the features available (ie. 'EXTENDED' for
        lvm and 'PARTITION_NAME' for label support).
        """
        feat = self._ped_disk.contents.type.contents.features
        return disk_features[feat]

    @property
    @diskDecorator()
    def block_sizes(self):
        """
        Returns the disk block sizes.
        """
        return self._ped_disk.contents.block_sizes

    @property
    @diskDecorator()
    def needs_clobber(self):
        """
        Returns True if the disk needs clobber.
        """
        return bool(self._ped_disk.contents.needs_clobber)

    @property
    @diskDecorator()
    def update_mode(self):
        """
        Returns True if the disk is set to update mode.
        """
        return bool(self._ped_disk.contents.update_mode)

    @property
    @diskDecorator()
    def total_free_space(self):
        """
        Returns the total free space size as a Size class instance.
        """
        size = Size()
        for part in self.free_partitions():
            size += part.size
        return size

    @property
    @diskDecorator()
    def usable_free_space(self):
        """
        Returns the largest free space size as a Size class instance.
        """
        size = Size()
        for part in self.free_partitions():
            if part.size > size:
                size = part.size
        return size

    @property
    @diskDecorator()
    def size(self):
        """
        Returns the size as a Size class instance.
        """
        return self.device.size

    @diskDecorator()
    def free_partitions(self):
        """
        Returns a list of the current free space allocations as Partition
        instances.

        .. note::

            If the disk is initialized (no partition table) it
            will return None.
        """
        partitions = []
        part = disk_next_partition(self._ped_disk, None)
        while part:
            if part.contents.type != 4:
                part = disk_next_partition(self._ped_disk, part)
                continue
            p = Partition(disk=self, part=part)
            partitions.append(p)
            part = disk_next_partition(self._ped_disk, part)
        return partitions

    @diskDecorator()
    def partitions(self):
        """
        Returns a list of the current disk partitions.

        .. note::

            If the disk is initialized (no partition table) it
            will return None, if the disk has a partition table
            but no partitions it will return an empty list.
        """
        partitions = []
        part = disk_next_partition(self._ped_disk, None)
        while part:
            if part.contents.type > 2:
                part = disk_next_partition(self._ped_disk, part)
                continue
            p = Partition(disk=self, part=part)
            partitions.append(p)
            part = disk_next_partition(self._ped_disk, part)
        return partitions

    @diskDecorator(error=True)
    def add_partition(self, part):
        """
        Adds a partition to disk. You still need to call commit
        for the changes to be made to disk::

            from reparted import *

            myDevice = Device("/dev/sdb")
            myDisk = Disk(myDevice)
            mySize = Size(4, "GB")
            myPartition = Partition(myDisk, mySize)
            myDisk.add_partition(myPartition)
            myDisk.commit()

        *Args:*

        *       part:       A Partition class instance.

        *Raises:*

        *       AddPartitionError

        .. note::

            If the disk is initialized (no partition table) it
            will raise DiskError.
        """
        try:
            p = self.get_partition(part.num)
            if p.geom == part.geom:
                raise AddPartitionError(701)
        except ValueError:
            pass
        partition = part._partition
        start, end, length = part.geom
        range_start = geometry_new(self._ped_device, start, 1)
        range_end = geometry_new(self._ped_device, end, 1)
        user_constraint = constraint_new(alignment_any, alignment_any, range_start,
                                        range_end, 1, self._ped_device.contents.length)
        if not bool(user_constraint):
            raise AddPartitionError(702)
        if part.alignment == 'optimal':
            dev_constraint = device_get_optimal_aligned_constraint(self._ped_device)
        elif part.alignment == 'minimal':
            dev_constraint = device_get_minimal_aligned_constraint(self._ped_device)
        else:
            dev_constraint = device_get_constraint(self._ped_device)
        if not bool(dev_constraint):
            raise AddPartitionError(702)
        final_constraint = constraint_intersect(user_constraint, dev_constraint)
        constraint_destroy(user_constraint)
        constraint_destroy(dev_constraint)
        if not bool(final_constraint):
            raise AddPartitionError(703)
        added = disk_add_partition(self._ped_disk, partition, final_constraint)
        constraint_destroy(final_constraint)
        if not added:
            disk_remove_partition(self._ped_disk, partition)
            raise AddPartitionError(701)
        if part.name:
            set_name = partition_set_name(partition, part.name)
            if not set_name:
                disk_remove_partition(self._ped_disk, partition)
                raise AddPartitionError(704)

    @diskDecorator(error=True)
    def delete_partition(self, part):
        """
        Deletes a partition from disk. Unlike add_partition,
        this method calls commit, use carefully::

            from reparted import *

            myDevice = Device("/dev/sdb")
            myDisk = Disk(myDevice)

            # Get Partition instance and remove it.
            part = myDisk.partitions()[0]
            myDisk.delete_partition(part)

            # Get Partition number and remove it.
            part = myDisk.partitions()[0].num
            myDisk.delete_partition(part)

        *Args:*

        *       part:   A Partition class instance OR partition number.

        *Raises:*

        *       DeletePartitionError, DiskCommitError

        .. note::

            If the disk is initialized (no partition table) it
            will raise DiskError.
        """
        if part and isinstance(part, Partition):
            partition = part._partition
        elif type(part) is int:
            partition = self._get_ped_partition(part)
        else:
            raise DeletePartitionError(705)
        if partition_is_busy(partition):
            raise DeletePartitionError(706)
        disk_delete_partition(self._ped_disk, partition)
        self.commit()
        disk_destroy(self._ped_disk)
        self._disk = disk_new(self._ped_device)

    @diskDecorator()
    def delete_all(self):
        """
        This method deletes all partitions from disk.

        *Raises:*

        *       DiskError, DiskCommitError

        .. note::

            If the disk is initialized (no partition table) it
            will return None.
        """
        disk_delete_all(self._ped_disk)
        return

    @diskDecorator(error=True)
    def commit(self):
        """
        This method commits partition modifications to disk.

        *Raises:*

        *       DiskError, DiskCommitError

        .. note::

            If the disk is initialized (no partition table) it
            will return None.
        """
        to_dev = disk_commit_to_dev(self._ped_disk)
        if not to_dev:
            raise DiskCommitError(601)
        to_os = disk_commit_to_os(self._ped_disk)
        if not to_os:
            raise DiskCommitError(602)

    def _get_ped_partition(self, part_num):
        partition = disk_get_partition(self._ped_disk, part_num)
        if not bool(partition):
            raise PartitionError(705)
        return partition

    @diskDecorator(error=True)
    def get_partition(self, part_num):
        """
        Returns a Partition instance.

        *Args:*

        *       part_num (int):     A partition number.

        *Raises:*

        *       PartitionError

        .. note::

            If the disk is initialized (no partition table) it
            will raise DiskError.
        """
        partition = Partition(disk=self, part=self._get_ped_partition(part_num))
        return partition

    def _destroy_disk(self, disk=None):
        if disk:
            disk_destroy(disk)
        else:
            if self._ped_disk:
                disk_destroy(self._ped_disk)
                self._disk = None
            else:
                raise DiskError(600)

    def set_label(self, label):
        """
        Sets the disk partition table ('gpt' or 'msdos)'.
        This method calls commit to set the label changes.

        *Args:*

        *       label (str):    A partition table type ('gpt' or 'msdos')

        *Raises:*

        *       DiskError
        """
        if label not in disk_labels:
            raise DiskError(603)
        disk_type = disk_get_type(label)
        if not bool(disk_type):
            raise DiskError(604)
        if bool(self._ped_disk):
            self._destroy_disk()
        new_disk = disk_new_fresh(self._ped_device, disk_type)
        if not bool(new_disk):
            raise DiskError(605)
        self._disk = new_disk
        self.commit()
        self._destroy_disk(disk=new_disk)
        self._disk = disk_new(self._ped_device)
