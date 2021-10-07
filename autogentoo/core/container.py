"""
Manages docker containers
"""
import enum

import docker

from core.event import EventHost
from core.stage3 import Stage3
from util import filesystem


class Container(EventHost):
    """
    Docker containers will manage the build service
    for Gentoo clients. They provide the environment
    to emerge packages for our clients.
    """

    class Status(enum.IntEnum):
        UNINITIALIZED = 1
        BUILDING_IMAGE = 2
        BUILDING_CONTAINER = 3
        READY = 4

    name: str
    stage3: Stage3
    status: Status
    container_logs: str

    def __init__(self,
                 name: str,
                 stage3: Stage3,
                 ):
        super().__init__()
        self.name = name
        self.stage3 = stage3
        self.container_logs = ""
        self.status = Container.Status.UNINITIALIZED

    def image_name(self) -> str:
        return "autogentoo-i.%s" % self.name

    def container_name(self) -> str:
        return "autogentoo-c.%s" % self.name

    def set_status(self, status: Status):
        self.status = status
        self.broadcast("status_update", self.status)

    def build(self):
        # Generate the image
        docker.from_env().images.build(
            fileobj=filesystem.get("core/Dockerfile").open("r"),
            tag=self.image_name(),
            forcerm=True,  # Force remove intermediate containers, even after unsuccessful builds
            buildargs={
                "STAGE3_URL": self.stage3.url,
            }
        )

        self.container_logs = docker.from_env().containers.run()
