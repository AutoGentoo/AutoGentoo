import enum

from .autogentoo_cportage import AtomVersion, UseFlag, Portage, init, Ebuild
from . import autogentoo_cportage


class UseOperatorT(enum.IntEnum):
    NONE = autogentoo_cportage.USE_OP_NONE
    DISABLE = autogentoo_cportage.USE_OP_DISABLE
    ENABLE = autogentoo_cportage.USE_OP_ENABLE
    LEAST_ONE = autogentoo_cportage.USE_OP_LEAST_ONE
    EXACT_ONE = autogentoo_cportage.USE_OP_EXACT_ONE
    MOST_ONE = autogentoo_cportage.USE_OP_MOST_ONE


class UseStateT(enum.IntEnum):
    UNKNOWN = autogentoo_cportage.USE_STATE_UNKNOWN
    DISABLED = autogentoo_cportage.USE_STATE_DISABLED
    ENABLED = autogentoo_cportage.USE_STATE_ENABLED


class AtomUseT(enum.IntEnum):
    DISABLE = autogentoo_cportage.ATOM_USE_DISABLE
    ENABLE = autogentoo_cportage.ATOM_USE_ENABLE
    ENABLE_IF_ON = autogentoo_cportage.ATOM_USE_ENABLE_IF_ON
    DISABLE_IF_OFF = autogentoo_cportage.ATOM_USE_DISABLE_IF_OFF
    EQUAL = autogentoo_cportage.ATOM_USE_EQUAL
    OPPOSITE = autogentoo_cportage.ATOM_USE_OPPOSITE


class AtomUseDefaultT(enum.IntEnum):
    NONE = autogentoo_cportage.ATOM_DEFAULT_NONE
    ON = autogentoo_cportage.ATOM_DEFAULT_ON
    OFF = autogentoo_cportage.ATOM_DEFAULT_OFF


class AtomVersionT(enum.IntFlag):
    NONE = autogentoo_cportage.ATOM_VERSION_NONE
    E = autogentoo_cportage.ATOM_VERSION_E
    L = autogentoo_cportage.ATOM_VERSION_L
    G = autogentoo_cportage.ATOM_VERSION_G
    REV = autogentoo_cportage.ATOM_VERSION_REV
    ALL = autogentoo_cportage.ATOM_VERSION_ALL
    GE = autogentoo_cportage.ATOM_VERSION_GE
    LE = autogentoo_cportage.ATOM_VERSION_LE


class AtomBlockT(enum.IntEnum):
    NONE = autogentoo_cportage.ATOM_BLOCK_NONE
    SOFT = autogentoo_cportage.ATOM_BLOCK_SOFT
    HARD = autogentoo_cportage.ATOM_BLOCK_HARD


class AtomSlotT(enum.IntEnum):
    IGNORE = autogentoo_cportage.ATOM_SLOT_IGNORE
    REBUILD = autogentoo_cportage.ATOM_SLOT_REBUILD


class AtomVersionPreT(enum.IntEnum):
    ALPHA = autogentoo_cportage.ATOM_PREFIX_ALPHA
    BETA = autogentoo_cportage.ATOM_PREFIX_ALPHA
    PRE = autogentoo_cportage.ATOM_PREFIX_ALPHA
    RC = autogentoo_cportage.ATOM_PREFIX_ALPHA
    NONE = autogentoo_cportage.ATOM_PREFIX_ALPHA
    P = autogentoo_cportage.ATOM_PREFIX_ALPHA


class ArchT(enum.IntEnum):
    ARCH_AMD64 = autogentoo_cportage.ARCH_AMD64
    ARCH_X86 = autogentoo_cportage.ARCH_X86
    ARCH_ARM = autogentoo_cportage.ARCH_ARM
    ARCH_ARM64 = autogentoo_cportage.ARCH_ARM64
    ARCH_HPPA = autogentoo_cportage.ARCH_HPPA
    ARCH_IA64 = autogentoo_cportage.ARCH_IA64
    ARCH_PPC = autogentoo_cportage.ARCH_PPC
    ARCH_PPC64 = autogentoo_cportage.ARCH_PPC64
    ARCH_SPARC = autogentoo_cportage.ARCH_SPARC
    ARCH_END = autogentoo_cportage.ARCH_END


class KeywordT(enum.IntEnum):
    KEYWORD_BROKEN = autogentoo_cportage.KEYWORD_BROKEN
    KEYWORD_NONE = autogentoo_cportage.KEYWORD_NONE
    KEYWORD_UNSTABLE = autogentoo_cportage.KEYWORD_UNSTABLE
    KEYWORD_STABLE = autogentoo_cportage.KEYWORD_STABLE


class AtomFlag(autogentoo_cportage.AtomFlag):
    @property
    # type: ignore
    def option(self) -> AtomUseT:
        return AtomUseT(super().option)

    @property
    # type: ignore
    def default(self) -> AtomUseDefaultT:
        return AtomUseDefaultT(super().option)


class RequiredUse(autogentoo_cportage.RequiredUse):
    @property
    # type: ignore
    def operator(self) -> UseOperatorT:
        return UseOperatorT(super().operator)


class Atom(autogentoo_cportage.Atom):
    @property
    # type: ignore
    def slot_opts(self) -> AtomSlotT:
        return AtomSlotT(super().slot_opts)

    @property
    # type: ignore
    def range(self) -> AtomVersionT:
        return AtomVersionT(super().range)

    @property
    # type: ignore
    def blocks(self) -> AtomBlockT:
        return AtomBlockT(super().blocks)


class Dependency(autogentoo_cportage.Dependency):
    @property
    # type: ignore
    def use_operator(self) -> UseOperatorT:
        return UseOperatorT(super().use_operator)

    @property
    # type: ignore
    def use_condition(self) -> UseOperatorT:
        return UseOperatorT(super().use_condition)
