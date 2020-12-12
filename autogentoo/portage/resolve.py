from typing import List

from autogentoo.cportage import *


class ResolvedDependency:
    parent: Dependency

    bdepend: List[Dependency]
    depend: List[Dependency]
    rdepend: List[Dependency]
    pdepend: List[Dependency]

    def __init__(self):
        pass
