"""
Configuration for architecture specific functionality
"""
import enum


class Architecture(str, enum.Enum):
    alpha = "alpha"
    amd64 = "amd64"
    arm = "arm"
    arm64 = "arm64"
    hppa = "hppa"
    ia64 = "ia64"
    ppc = "ppc"
    riscv = "riscv"
    s390 = "s390"
    sh = "sh"
    sparc = "sparc"
    x86 = "x86"


class ProfileModifier(str, enum.Enum):
    """
    These provide programmatic access to
    architecture/profile flags used by Gentoo
    """

    # x86
    I386 = "i368"
    i486 = "i486"
    i686 = "i686"

    # Arm
    arm64 = "arm64"

    # All platforms
    nomultilib = "nomultilib"  # FIXME what's the difference between this and without this flag
    multilib = "multilib"
    hardened = "hardened"
    vanilla = "vanilla"
    selinux = "selinux"
    hardfp = "hardfp"

    # Init
    systemd = "systemd"
    openrc = "openrc"

    # Libc
    uclibc = "uclibc"
    musl = "musl"
