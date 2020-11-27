//
// Created by atuser on 5/5/19.
//

#ifndef AUTOGENTOO_FEATURES_H
#define AUTOGENTOO_FEATURES_H

typedef enum {
    FEATURE_NONE,

    /** When commiting work to cvs with repoman(1), assume that all existing SRC_URI digests are correct. This feature also affects digest generation via ebuild(1) and emerge(1) (emerge generates digests only when the digest feature is enabled). Existing digests for files that do not exist in ${DISTDIR} will be automatically assumed even when assume-digests is not enabled. If a file exists in ${DISTDIR} but its size does not match the existing digest, the digest will be regenerated regardless of whether or not assume-digests is enabled. The ebuild(1) digest command has a --force option that can be used to force regeneration of digests. */
    FEATURE_ASSUME_DIGESTS,

    /** Perform docompress (controllable file compression) before creating binary package. When this option is enabled (the default), documentation files are already compressed inside binary packages. When it is disabled, binary packages contain uncompressed documentation and Portage compresses it before installing. */
    FEATURE_BINPKG_DOCOMPRESS,

    /** Perform file stripping before creating binary package. When this option is enabled (the default), executables are already stripped inside binary packages. When it is disabled, binary packages contain unstripped executables and Portage strips (or splits) them before installing. */
    /** binpkg-dostrip must be enabled for installsources to work. */
    FEATURE_BINPKG_DOSTRIP,

    /** Keep logs from successful binary package merges. This is relevant only when PORTAGE_LOGDIR is set. */
    FEATURE_BINPKG_LOGS,

    /** Enable support for multiple binary package instances per ebuild. Having multiple instances is useful for a number of purposes, such as retaining builds that were built with different USE flags or linked against different versions of libraries. The location of any particular package within PKGDIR can be expressed as follows: */
    /** ${PKGDIR}/${CATEGORY}/${PN}/${PF}-${BUILD_ID}. */
    FEATURE_BINPKG_MULTI_INSTANCE,

    /**
     * The build-id starts at 1 for the first build of a particular ebuild, and is incremented by 1 for each new build. It is possible to share a writable PKGDIR over NFS, and locking ensures that each package added to PKGDIR will have a unique build-id. It is not necessary to migrate an existing PKGDIR to the new layout, since portage is capable of working with a mixed PKGDIR layout, where packages using the old layout are allowed to remain in place.
     * The new PKGDIR layout is backward-compatible with binhost clients running older portage, since the file format is identical, the per-package PATH attribute in the 'Packages' index directs them to download the file from the correct URI, and they automatically use BUILD_TIME metadata to select the latest builds.
     * There is currently no automated way to prune old builds from PKGDIR, although it is possible to remove packages manually, and then run aqemaint --fix binhost' to update the ${PKGDIR}/Packages index.
     * */
    FEATURE_XPAK,

    /** Binary packages will be created for all packages that are merged. Also see quickpkg(1) and emerge(1) --buildpkg and --buildpkgonly options. */
    FEATURE_BUILDPKG,

    /** Build binary packages for just packages in the system set. */
    FEATURE_BUILDSYSPKG,

    /** Enable a special progress indicator when emerge(1) is calculating dependencies. */
    FEATURE_CANDY,

    /** Use case-insensitive file name comparisions when merging and unmerging files. Most users should not enable this feature, since most filesystems are case-sensitive. You should only enable this feature if you are using portage to install files to a case-insensitive filesystem. */
    FEATURE_CASE_INSENSITIVE_FS,

    /** Enable portage support for the ccache package. If the ccache dir is not present in the user's environment, then portage will default to ${PORTAGE_TMPDIR}/ccache.
     * Warning: This eature is known to cause numerous compilation failures. Sometimes ccache will retain stale code objects or corrupted files, which can lead to packages that cannot be emerged. If this happens (if you receive errors like "File not recognized: File truncated"), try recompiling the application with ccache disabled before reporting a bug. Unless you are doing development work, do not enable ccache.
     * */
    FEATURE_CCACHE,

    /** Use Linux control group to control processes spawned by ebuilds. This allows emerge to safely kill all subprocesses when ebuild phase exits. */
    FEATURE_CGROUP,

    /** Enable automatic execution of the command specified by the PORTAGE_LOGDIR_CLEAN variable. The default PORTAGE_LOGDIR_CLEAN setting will remove all files from PORTAGE_LOGDIR that were last modified at least 7 days ago. */
    FEATURE_CLEAN_LOGS,

    /** A QA-feature to ensure that a package doesn't overwrite files it doesn't own. The COLLISION_IGNORE variable can be used to selectively disable this feature. Also see the related protect-owned feature. */
    FEATURE_COLLISION_PROTECT,

    /** The causes all build logs to be compressed while they are being written. Log file names have an extension that is appropriate for the compression type. Currently, only gzip(1) compression is supported, so build logs will have a '.gz' extension when this feature is enabled. */
    FEATURE_COMPRESS_BUILD_LOGS,

    /** If set then a compressed copy of 'Packages' index file will be written. This feature is intended for Gentoo binhosts using certain webservers (such as, but not limited to, Nginx with gzip_static module) to avoid redundant on-the-fly compression. The resulting file will be called 'Packages.gz' and its modification time will match that of 'Packages'. */
    FEATURE_COMPRESS_INDEX,

    /** Compress the debug sections in the split debug files with zlib to save space. Make sure you have built both binutils and gdb with USE=zlib support for this to work. See splitdebug for general split debug information (upon which this feature depends). */
    FEATURE_COMPRESSDEBUG,

    /** This causes the CONFIG_PROTECT behavior to be skipped for files that have not been modified since they were installed. This feature is enabled by default. */
    FEATURE_CONFIG_PROTECT_IF_MODIFIED,

    /** Autogenerate digests for packages when running the emerge(1), ebuild(1), or repoman(1) commands. If the assume-digests feature is also enabled then existing SRC_URI digests will be reused whenever they are available. */
    FEATURE_DIGEST,

    /** Enable portage support for the distcc package. */
    FEATURE_DISTCC,

    /** Enable portage support for the distcc package with pump mode. */
    FEATURE_DISTC_PUMP,

    /** Portage uses lockfiles to ensure competing instances don't clobber each other's files. This feature is enabled by default but may cause heartache on less intelligent remote filesystems like NFSv2 and some strangely configured Samba server (oplocks off, NFS re-export). A tool /usr/lib/portage/bin/clean_locks exists to help handle lock issues when a problem arises (normally due to a crash or disconnect). */
    FEATURE_DISTLOCKS,

    /** When a package is downgraded to a lower version, call quickpkg(1) in order to create a backup of the installed version before it is unmerged (if a binary package of the same version does not already exist). Also see the related unmerge-backup feature. */
    FEATURE_DOWNGRADE_BACKUP,

    /** Use locks to ensure that unsandboxed ebuild phases never execute concurrently. Also see parallel-install. */
    FEATURE_EBUILD_LOCKS,

    /** Clean up temporary files after a build failure. This is particularly useful if you have PORTAGE_TMPDIR on tmpfs. If this feature is enabled, you probably also want to enable PORTAGE_LOGDIR in order to save the build log. Both the ebuild(1) command and the noclean feature cause the fail-clean feature to be automatically disabled. */
    FEATURE_FAIL_CLEAN,

    /** Enable fakeroot for the install and package phases when a non-root user runs the ebuild(1) command. */
    FEATURE_FAKEROOT,

    /** Modifies .la files to not include other .la files and some other fixes (order of flags, duplicated entries, ...) */
    FEATURE_FIXLAFILES,

    /** Only fetch files from configured mirrors, ignoring SRC_URI, except when mirror is in the ebuild(5) RESTRICT variable. */
    FEATURE_FORCE_MIRROR,

    /** Enable prefix support for all ebuilds, regardless of EAPI, since older EAPIs would otherwise be useless with prefix configurations. This brings compatibility with the prefix branch of portage, which also supports EPREFIX for all EAPIs (for obvious reasons). */
    FEATURE_FORCE_PREFIX,

    /** Force emerges to always try to fetch files from the PORTAGE_BINHOST. See make.conf(5) for more information. */
    FEATURE_GETBINPKG,

    /** Enable portage support for the icecream package. */
    FEATURE_ICECREAM,

    /** Install source code into /usr/src/debug/${CATEGORY}/${PF} (also see splitdebug). This feature works only if debugedit is installed, CFLAGS is set to include debug information (such as with the -ggdb flag) and binpkg-dostrip is enabled. */
    FEATURE_INSTALLSOURCES,

    /** Isolate the ebuild phase functions from host IPC namespace. Supported only on Linux. Requires IPC namespace support in kernel. */
    FEATURE_IPC_SANDBOX,

    /** Do not delete the ${T} directory after the merge process. */
    FEATURE_KEEPTEMP,

    /** Do not delete the ${WORKDIR} directory after the merge process. ${WORKDIR} can then be reused since this feature disables most of the clean phase that runs prior to each build. Due to lack of proper cleanup, this feature can interfere with normal emerge operation and therefore it should not be left enabled for more than a short period of time. */
    FEATURE_KEEPWORK,

    /** When mirror is enabled in FEATURES, fetch files even when mirror is also in the ebuild(5) RESTRICT variable. Do NOT use lmirror for clients that need to override RESTRICT when fetching from a local mirror, but instead use a "local" mirror setting in /etc/portage/mirrors, as described in portage(5). */
    FEATURE_LMIRROR,

    /** After a package is merged or unmerged, sync relevant files to disk in order to avoid data-loss in the event of a power failure. This feature is enabled by default. */
    FEATURE_MERGE_SYNC,

    /** Automatically perform a metadata transfer when `emerge --sync` is run. In versions of portage >=2.1.5, this feature is disabled by default. When metadata-transfer is disabled, metadata cache from the ${repository_location}/metadata/md5-cache/ directory will be used directly (if available). */
    FEATURE_METADATA_TRANSFER,

    /** Fetch everything in SRC_URI regardless of USE settings, except do not fetch anything when mirror is in RESTRICT. */
    FEATURE_MIRROR,

    /** Isolate the ebuild phase functions from host mount namespace. This makes it possible for ebuild to alter mountpoints without affecting the host system. Supported only on Linux. Requires mount namespace support in kernel. */
    FEATURE_MOUNT_SANDBOX,

    /** Many Makefiles assume that their libraries should go to /usr/lib, or $(prefix)/lib. This assumption can cause a serious mess if /usr/lib isn't a symlink to /usr/lib64. To find the bad packages, we have a portage feature called multilib-strict. It will prevent emerge from putting 64bit libraries into anything other than (/usr)/lib64. */
    FEATURE_MULTILIB_STRICT,

    /** Isolate the ebuild phase functions from host network interfaces. Supported only on Linux. Requires network namespace support in kernel. */
    FEATURE_NETWORK_SANDBOX,

    /** Enable escaping network-sandbox through SOCKSv5 proxy. Enabling distcc feature also enables the proxy.
     * If asyncio Python module is available (requires Python 3.3, built-in since Python 3.4) Portage will additionally spawn an isolated SOCKSv5 proxy on UNIX socket. The socket address will be exported as PORTAGE_SOCKS5_PROXY and the processes running inside the sandbox can use it to access host's network when desired. Portage automatically configures new enough distcc to use the proxy.
     * */
    FEATURE_NETWORK_SANDBOX_PROXY,

    /** Enable GLEP 42 news support. See https://www.gentoo.org/glep/glep-0042.html. */
    FEATURE_NEWS,

    /** When utilizing ebuild(1), only run the function requested. Also, forces the corresponding ebuild and eclasses to be sourced again for each phase, in order to account for modifications. */
    FEATURE_NOAUTO,

    /** Do not delete the the source and temporary files after the merge process. */
    FEATURE_NOCLEAN,

    /** Do not install doc files (/usr/share/doc). */
    FEATURE_NODOC,

    /** Do not install info pages. */
    FEATURE_NOINFO,

    /** Do not install manpages. */
    FEATURE_NOMAN,

    /** Prevents the stripping of binaries that are merged to the live filesystem. */
    FEATURE_NOSTRIP,

    /** Disables xterm titlebar updates (which contains status info). */
    FEATURE_NOTITLES,

    /** Fetch in the background while compiling. Run `tail -f /var/log/emerge-fetch.log` in a terminal to view parallel-fetch progress. */
    FEATURE_PARALLEL_FETCH,

    /** Use finer-grained locks when installing packages, allowing for greater parallelization. For additional parallelization, disable ebuild-locks. */
    FEATURE_PARALLEL_INSTALL,

    /** Isolate the process space for the ebuild processes. This makes it possible to cleanly kill all processes spawned by the ebuild. Supported only on Linux. Requires PID and mount namespace support in kernel. /proc is remounted inside the mount namespace to account for new PID namespace. */
    FEATURE_PID_SANDBOX,

    /** If prelink(8) is installed then use it to undo any prelinks on files before computing checksums for merge and unmerge. This feature is useful only if prelink(8) is installed and accurate checksums (despite prelinking) are needed for some reason such as for checking the integrity of installed files or because the unmerge-orphans feature is disabled.
     * Note that normal emerging of packages from source computes the checksums before things will be prelinked, so in such cases, this feature isn't required either. Undoing prelinking while merging is only required when using tools like quickpkg(1) which can cause already prelinked files to be merged.
     * */
    FEATURE_PRELINK_CHECKSUMS,

    /** Preserve libraries when the sonames change during upgrade or downgrade. Libraries are preserved only if consumers of those libraries are detected. Preserved libraries are automatically removed when there are no remaining consumers. Run `emerge @preserved-rebuild` in order to rebuild all consumers of preserved libraries. */
    FEATURE_PRESERVE_LIBS,

    /** This is identical to the collision-protect feature except that files may be overwritten if they are not explicitly listed in the contents of a currently installed package. This is particularly useful on systems that have lots of orphan files that have been left behind by older versions of portage that did not support the unmerge-orphans feature. Like collision-protect, the COLLISION_IGNORE variable can be used to selectively disable this feature. It is recommended to leave either protect-owned or collision-protect enabled at all times, since otherwise file collisions between packages may result in files being overwritten or uninstalled at inappropriate times. If collision-protect is enabled then it takes precedence over protect-owned. */
    FEATURE_PROTECT_OWNED,

    /** Output a verbose trace of python execution to stderr when a command's --debug option is enabled. */
    FEATURE_PYTHON_TRACE,

    /** Enable sandbox-ing when running emerge(1) and ebuild(1). */
    FEATURE_SANDBOX,

    /** Enable SELinux sandbox-ing. Do not toggle this FEATURE yourself. */
    FEATURE_SESANDBOX,

    /** Stands for Smart Filesystem Permissions. Before merging packages to the live filesystem, automatically search for and set permissions on setuid and setgid files. Files that are setuid have the group and other read bits removed while files that are setgid have the other read bit removed. See also suidctl below. */
    FEATURE_SFPERMS,

    /** When commiting work to cvs with repoman(1), sign the Manifest with a GPG key. Read about the PORTAGE_GPG_KEY variable in make.conf(5). */
    FEATURE_SIGN,

    /** Skip write access checks on DISTDIR when fetching files. This is useful when FETCHCOMMAND and RESUMECOMMAND are used to forward fetch requests to a server that exposes DISTDIR as a read-only NFS share. A read-only DISTDIR is not compatible with the distlocks, so it is recommended to also add "-distlocks" to FEATURES in order to avoid warning messages that are triggered by this incompatibility. */
    FEATURE_SKIPROCHECK,

    /** Store logs created by PORTAGE_ELOG_SYSTEM="save" in category subdirectories of PORTAGE_LOGDIR/elog, instead of using PORTAGE_LOGDIR/elog directly. */
    FEATURE_SPLIT_ELOG,

    /** Store build logs in category subdirectories of PORTAGE_LOGDIR/build, instead of using PORTAGE_LOGDIR directly. */
    FEATURE_SPLIT_LOG,

    /** Prior to stripping ELF etdyn and etexec files, the debugging info is stored for later use by various debuggers. This feature is disabled by nostrip. You should also consider setting compressdebug so the files don't suck up a lot of space. For installation of source code, see installsources. */
    FEATURE_SPLITDEBUG,

    /** Have portage react strongly to conditions that have the potential to be dangerous (like missing or incorrect digests for ebuilds). */
    FEATURE_STRICT,

    /** Have portage strictly require keepdir calls in ebuilds. Empty directories installed without explicit keepdir will be removed. */
    FEATURE_STRICT_KEEPDIR,

    /** Have portage react strongly to conditions that may conflict with system security provisions (for example textrels, executable stack). Read about the QA_STRICT_* variables in make.conf(5). */
    FEATURE_STRICTER,

    /** Before merging packages to the live filesystem, automatically strip setuid bits from any file that is not listed in /etc/portage/suidctl.conf. */
    FEATURE_SUIDCTL,

    /** Run package-specific tests during each merge to help make sure the package compiled properly. See test in ebuild(1) and src_test() in ebuild(5). This feature implies the "test" USE flag if it is a member of IUSE, either explicitly or implicitly (see ebuild(5) for more information about IUSE). The "test" USE flag is also automatically disabled when the "test" feature is disabled. */
    FEATURE_TEST,

    /** If "test" is enabled FEATURES and the test phase of an ebuild fails, continue to execute the remaining phases as if the failure had not occurred. Note that the test phase for a specific package may be disabled by masking the "test" USE flag in package.use.mask (see portage(5)). */
    FEATURE_TEST_FAIL_CONTINUE,

    /** Filter out any unknown values that the FEATURES variable contains. */
    FEATURE_UNKNOWN_FEATURES_FILTER,

    /** Warn if FEATURES contains one or more unknown values. */
    FEATURE_UNKNOWN_FEATURES_WARN,

    /** Call quickpkg(1) to create a backup of each package before it is unmerged (if a binary package of the same version does not already exist). Also see the related downgrade-backup feature. */
    FEATURE_UNMERGE_BACKUP,

    /** Keep logs from successful unmerge phases. This is relevant only when PORTAGE_LOGDIR is set. */
    FEATURE_UNMERGE_LOGS,

    /** If a file is not claimed by another package in the same slot and it is not protected by CONFIG_PROTECT, unmerge it even if the modification time or checksum differs from the file that was originally installed. */
    FEATURE_UNMERGE_ORPHANS,

    /** When portage is run as root, drop privileges to portage:portage during the fetching of package sources. */
    FEATURE_USERFETCH,

    /** Allow portage to drop root privileges and compile packages as portage:portage without a sandbox (unless usersandbox is also used). */
    FEATURE_USERPRIV,

    /** Enable the sandbox in the compile phase, when running without root privs (userpriv). */
    FEATURE_USERSANDBOX,

    /** Drop privileges to the owner of ${repository_location} for emerge(1) --sync operations. Note that this feature assumes that all subdirectories of ${repository_location} have the same ownership as ${repository_location} itself. It is the user's responsibility to ensure correct ownership, since otherwise Portage would have to waste time validating ownership for each and every sync operation. */
    FEATURE_USERSYNC,

    /** Enable GPG verification when using emerge-webrsync. */
    FEATURE_WEBRSYNC_GPG,

    /** Preserve extended attributes (filesystem-stored metadata) when installing files (see attr(1)). The PORTAGE_XATTR_EXCLUDE variable may be used to exclude specific attributes from being preserved. */
    FEATURE_XATTR,
} feature_t;

#endif //AUTOGENTOO_FEATURES_H
