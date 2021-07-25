from functools import cached_property
from pathlib import Path
from typing import Dict, Union

import cportage
from common import (
    RequiredUseException,
    DependencyContainer,
    ResolutionException,
    UseSuggestion,
    SuggestionExpression,
    InvalidExpressionException
)
from resolve import (
    resolve_all,
    Emerge,
    emerge_init,
    PackageResolutionSession
)


class Portage:
    DEFAULT_PORTAGE_PATH: str = "/var/db/repos/gentoo"
    DEFAULT_CACHE_PATH: str = "/var/db/repos/gentoo/metadata"

    root: Path
    repositories: Dict[str, cportage.Portage]

    def __init__(self, root: Union[str, Path]):
        self.root = Path(root)
        self.repositories = {}

        gentoo = cportage.Portage()
        gentoo.initialize_repository(
                Portage.DEFAULT_PORTAGE_PATH,
                Portage.DEFAULT_CACHE_PATH
        )
        self.add_repository("gentoo", gentoo)

    def get_config_path(self, name: str) -> Path:
        return self.root / "etc" / "portage" / name

    def add_repository(self, name, repo: cportage.Portage):
        self.repositories[name] = repo

    @cached_property
    def default(self) -> cportage.Portage:
        return self.repositories["gentoo"]
