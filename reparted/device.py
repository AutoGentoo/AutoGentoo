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
from size import *
from disk import Disk
from exception import DeviceError
import os

device_type = {
    0 : 'UNKNOWN',
    1 : 'SCSI',
    2 : 'IDE',
    3 : 'DAC960',
    4 : 'CPQARRAY',
    5 : 'FILE',
    6 : 'ATARAID',
    7 : 'I20',
    8 : 'UBD',
    9 : 'DASD',
    10 : 'VIODASD',
    11 : 'SX8',
    12 : 'DM',
    13 : 'XVD',
    14 : 'SDMMC',
    15 : 'VIRTBLK',
    16 : 'AOE',
    17 : 'MD',
}

standard_devices = [
    "/dev/hda",
    "/dev/hdb",
    "/dev/hdc",
    "/dev/hdd",
    "/dev/hde",
    "/dev/hdf",
    "/dev/hdg",
    "/dev/hdh",
    "/dev/sda",
    "/dev/sdb",
    "/dev/sdc",
    "/dev/sdd",
    "/dev/sde",
    "/dev/sdf"
]

def device_probe(path):
    if not os.path.exists(path):
        return False
    dev = device_get(path)
    if bool(dev):
        return dev
    else:
        return False

class Device(object):
    """
    *Device class is used as a wrapper to libparted's ped_device.*

    You need to call this class before calling Disk. You can create
    a new instance of Device by specifying a path or let it probe
    standard devices::

        from reparted import *

        # specify the device path
        myDevice = Device("/dev/sdb")

        # probe standard devices
        myDevice2 = Device()

    *Args:*

    *   path (str):     Path to your local device of choice (ie. '/dev/sda').
    *   dev:            A ped_device pointer, usually this is used internally.

    *Raises:*

    *   DeviceError

    .. note::

       If called without any parameters it will probe all standard devices
       and default to the first one it finds.

    """
    def __init__(self, path=None, dev=None):
        """
        Initialize Device class, if ped_device pointer is Null it will
        raise DeviceError.
        """
        if path:
            self._device = device_probe(path)
        elif dev:
            self._device = dev
        else:
            self._device = self._probe_ped_device()
        if not bool(self._device):
            raise DeviceError(500)
        length = self.length * self.sector_size
        size = Size(length=length, units="B", dev=self)
        self._size = size

    @property
    def _ped_device(self):
        """
        Returns the ctypes ped_device pointer.
        """
        return self._device

    @property
    def length(self):
        """
        Returns the length in sectors of the device.
        """
        return self._ped_device.contents.length

    @property
    def path(self):
        """
        Returns the device path (ie. '/dev/sda').
        """
        return self._ped_device.contents.path

    @property
    def model(self):
        """
        Returns the device model (ie. 'ATA VBOX HARDDISK').
        """
        return self._ped_device.contents.model

    @property
    def type(self):
        """
        Returns the device type (ie. 'SCSI').
        """
        return device_type[self._ped_device.contents.type]

    @property
    def sector_size(self):
        return self._ped_device.contents.sector_size

    @property
    def phys_sector_size(self):
        """
        Returns the physical sector size.
        """
        return self._ped_device.contents.phys_sector_size

    @property
    def open_count(self):
        """
        Returns the number of times the device has been opened.
        """
        return self._ped_device.contents.open_count

    @property
    def read_only(self):
        """
        Returns True if the device is set as read only.
        """
        return bool(self._ped_device.contents.read_only)

    @property
    def external_mode(self):
        """
        Returns True if the device is set to external mode.
        """
        return bool(self._ped_device.contents.path)

    @property
    def dirty(self):
        """
        Returns True if the device is dirty.
        """
        return bool(self._ped_device.contents.dirty)

    @property
    def boot_dirty(self):
        """
        Returns True if the device is set to boot dirty.
        """
        return bool(self._ped_device.contents.boot_dirty)

    @property
    def hw_geom(self):
        """
        Returns the hardware geometry as a 3-tuple:

            (cylinders, heads, sectors)
        """
        cylinders = self._ped_device.contents.hw_geom.cylinders
        heads = self._ped_device.contents.hw_geom.heads
        sectors = self._ped_device.contents.hw_geom.sectors
        return (cylinders, heads, sectors)

    @property
    def bios_geom(self):
        """
        Returns the bios geometry as a 3-tuple:

            (cylinders, heads, sectors)
        """
        cylinders = self._ped_device.contents.bios_geom.cylinders
        heads = self._ped_device.contents.bios_geom.heads
        sectors = self._ped_device.contents.bios_geom.sectors
        return (cylinders, heads, sectors)

    @property
    def host(self):
        """
        Returns the device host.
        """
        return self._ped_device.contents.host

    @property
    def did(self):
        """
        Returns the device did.
        """
        return self._ped_device.contents.did

    @property
    def size(self):
        """
        Returns the size as a Size class instance.
        """
        return self._size

    def _probe_ped_device(self):
        for path in standard_devices:
            dev = device_probe(path)
            if bool(dev):
                return dev
        raise DeviceError(500)

def probe_standard_devices():
    """
    This function probes all standard devices and returns a list
    containing instances of Device of the found standard devices::

        from reparted.device import probe_standard_devices

        probe_standard_devices()
        [<reparted.device.Device object at 0xb7854d8c>,
        <reparted.device.Device object at 0xb7607eac>]
    """
    devices = []
    for path in standard_devices:
        dev = device_probe(path)
        if dev:
            device = Device(dev=dev)
            devices.append(device)
    return devices