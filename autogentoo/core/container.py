"""
Manages docker containers
"""
from typing import Optional
import docker

from core.stage3 import Stage3


class Container:
    name: str

    def __init__(self,
                 name: str,
                 stage3: Stage3,

                 ):
        docker.from_env().images.build(

        )
        self.client = docker.from_env().containers.run()
