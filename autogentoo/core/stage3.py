"""
Manages locations of Stage3 tarballs
This tarball will bootstrap the Gentoo setup
"""
import re
import urllib.request
from pathlib import Path
from typing import Set, Optional

from autogentoo.core.mirror import Mirror, DEFAULT_MIRROR
from autogentoo.core.architecture import Architecture, ProfileModifier


class Stage3:
    mirror: Mirror
    architecture: Architecture
    modifiers: Set[str]

    url: Path

    def __init__(self, request: "Stage3Request", url_path: Path):
        self.url = url_path
        self.mirror = request.mirror
        self.architecture = request.architecture

        filename = url_path.name

        # Strip the extension and the 'stage3' prefix
        assert filename.startswith("stage3-"), filename
        filename = filename[7:filename.find('.')]

        # Delete the final token, its always the version ID
        self.modifiers = set(re.split('[-_+]', filename)[:-1])
        self.modifiers.discard('')  # discard any empty tokens

    def full_url(self) -> str:
        return f"/releases/{self.architecture}/autobuilds/{str(self.url)}"

    def download(self, output_path: Path):
        self.mirror.download(self.full_url(), output_path)


class Stage3Request:
    mirror: Mirror
    architecture: Architecture
    modifiers: Set[ProfileModifier]
    match_modifiers_exactly: bool

    def __init__(self,
                 architecture: Architecture,
                 modifiers: Optional[Set[ProfileModifier]],
                 mirror: Mirror = DEFAULT_MIRROR,
                 match_modifiers_exactly: bool = False):
        self.mirror = mirror
        self.architecture = architecture
        self.modifiers = modifiers
        self.match_modifiers_exactly = match_modifiers_exactly

    def _arch_mirror_path(self) -> str:
        return f"/releases/{self.architecture}/autobuilds/"

    def find(self) -> Stage3:
        tarballs = []

        with urllib.request.urlopen(self._arch_mirror_path() + "latest-stage3.txt") as f:
            for line in f.read().decode().split("\n"):
                line: str = line.strip()
                if line.startswith("#"):
                    continue

                path_raw, size = line.split(" ")
                path = Path(path_raw)
                if not path.name.startswith("stage3"):
                    # Skip stage4 tarballs
                    continue

                tarballs.append(Stage3(self, path))

        if not tarballs:
            raise FileNotFoundError("No tarballs found for %s" % self.architecture)

        best_stage3 = None
        for tarball in tarballs:
            if not self.match_modifiers_exactly:
                # Check if this is better than our current match
                if self.modifiers.issubset(tarball.modifiers):
                    if best_stage3:
                        # Optimize the flags as much as possible
                        if len(tarball.modifiers) < len(best_stage3.modifiers):
                            best_stage3 = tarball
                    else:
                        # Nothing to compare to yet
                        best_stage3 = tarball
            else:
                if self.modifiers == tarball.modifiers:
                    best_stage3 = tarball
                    break

        if not best_stage3:
            raise FileNotFoundError("No tarball for %s found in %s"
                                    % (self.modifiers, tarballs))

        return best_stage3
