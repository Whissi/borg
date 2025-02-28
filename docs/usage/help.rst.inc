.. IMPORTANT: this file is auto-generated from borg's built-in help, do not edit!

.. _borg_patterns:

borg help patterns
~~~~~~~~~~~~~~~~~~


The path/filenames used as input for the pattern matching start from the
currently active recursion root. You usually give the recursion root(s)
when invoking borg and these can be either relative or absolute paths.

So, when you give `relative/` as root, the paths going into the matcher
will look like `relative/.../file.ext`. When you give `/absolute/` as
root, they will look like `/absolute/.../file.ext`.

File paths in Borg archives are always stored normalized and relative.
This means that e.g. ``borg create /path/to/repo ../some/path`` will
store all files as `some/path/.../file.ext` and ``borg create
/path/to/repo /home/user`` will store all files as
`home/user/.../file.ext`.

A directory exclusion pattern can end either with or without a slash ('/').
If it ends with a slash, such as `some/path/`, the directory will be
included but not its content. If it does not end with a slash, such as
`some/path`, both the directory and content will be excluded.

File patterns support these styles: fnmatch, shell, regular expressions,
path prefixes and path full-matches. By default, fnmatch is used for
``--exclude`` patterns and shell-style is used for the ``--pattern``
option. For commands that support patterns in their ``PATH`` argument
like (``borg list``), the default pattern is path prefix.

Starting with Borg 1.2, for all but regular expression pattern matching
styles, all paths are treated as relative, meaning that a leading path
separator is removed after normalizing and before matching. This allows
you to use absolute or relative patterns arbitrarily.

If followed by a colon (':') the first two characters of a pattern are
used as a style selector. Explicit style selection is necessary when a
non-default style is desired or when the desired pattern starts with
two alphanumeric characters followed by a colon (i.e. `aa:something/*`).

`Fnmatch <https://docs.python.org/3/library/fnmatch.html>`_, selector `fm:`
    This is the default style for ``--exclude`` and ``--exclude-from``.
    These patterns use a variant of shell pattern syntax, with '\*' matching
    any number of characters, '?' matching any single character, '[...]'
    matching any single character specified, including ranges, and '[!...]'
    matching any character not specified. For the purpose of these patterns,
    the path separator (backslash for Windows and '/' on other systems) is not
    treated specially. Wrap meta-characters in brackets for a literal
    match (i.e. `[?]` to match the literal character `?`). For a path
    to match a pattern, the full path must match, or it must match
    from the start of the full path to just before a path separator. Except
    for the root path, paths will never end in the path separator when
    matching is attempted.  Thus, if a given pattern ends in a path
    separator, a '\*' is appended before matching is attempted. A leading
    path separator is always removed.

Shell-style patterns, selector `sh:`
    This is the default style for ``--pattern`` and ``--patterns-from``.
    Like fnmatch patterns these are similar to shell patterns. The difference
    is that the pattern may include `**/` for matching zero or more directory
    levels, `*` for matching zero or more arbitrary characters with the
    exception of any path separator. A leading path separator is always removed.

Regular expressions, selector `re:`
    Regular expressions similar to those found in Perl are supported. Unlike
    shell patterns regular expressions are not required to match the full
    path and any substring match is sufficient. It is strongly recommended to
    anchor patterns to the start ('^'), to the end ('$') or both. Path
    separators (backslash for Windows and '/' on other systems) in paths are
    always normalized to a forward slash ('/') before applying a pattern. The
    regular expression syntax is described in the `Python documentation for
    the re module <https://docs.python.org/3/library/re.html>`_.

Path prefix, selector `pp:`
    This pattern style is useful to match whole sub-directories. The pattern
    `pp:root/somedir` matches `root/somedir` and everything therein. A leading
    path separator is always removed.

Path full-match, selector `pf:`
    This pattern style is (only) useful to match full paths.
    This is kind of a pseudo pattern as it can not have any variable or
    unspecified parts - the full path must be given. `pf:root/file.ext` matches
    `root/file.ext` only. A leading path separator is always removed.

    Implementation note: this is implemented via very time-efficient O(1)
    hashtable lookups (this means you can have huge amounts of such patterns
    without impacting performance much).
    Due to that, this kind of pattern does not respect any context or order.
    If you use such a pattern to include a file, it will always be included
    (if the directory recursion encounters it).
    Other include/exclude patterns that would normally match will be ignored.
    Same logic applies for exclude.

.. note::

    `re:`, `sh:` and `fm:` patterns are all implemented on top of the Python SRE
    engine. It is very easy to formulate patterns for each of these types which
    requires an inordinate amount of time to match paths. If untrusted users
    are able to supply patterns, ensure they cannot supply `re:` patterns.
    Further, ensure that `sh:` and `fm:` patterns only contain a handful of
    wildcards at most.

Exclusions can be passed via the command line option ``--exclude``. When used
from within a shell, the patterns should be quoted to protect them from
expansion.

The ``--exclude-from`` option permits loading exclusion patterns from a text
file with one pattern per line. Lines empty or starting with the number sign
('#') after removing whitespace on both ends are ignored. The optional style
selector prefix is also supported for patterns loaded from a file. Due to
whitespace removal, paths with whitespace at the beginning or end can only be
excluded using regular expressions.

To test your exclusion patterns without performing an actual backup you can
run ``borg create --list --dry-run ...``.

Examples::

    # Exclude '/home/user/file.o' but not '/home/user/file.odt':
    $ borg create -e '*.o' backup /

    # Exclude '/home/user/junk' and '/home/user/subdir/junk' but
    # not '/home/user/importantjunk' or '/etc/junk':
    $ borg create -e '/home/*/junk' backup /

    # Exclude the contents of '/home/user/cache' but not the directory itself:
    $ borg create -e home/user/cache/ backup /

    # The file '/home/user/cache/important' is *not* backed up:
    $ borg create -e /home/user/cache/ backup / /home/user/cache/important

    # The contents of directories in '/home' are not backed up when their name
    # ends in '.tmp'
    $ borg create --exclude 're:^/home/[^/]+\.tmp/' backup /

    # Load exclusions from file
    $ cat >exclude.txt <<EOF
    # Comment line
    /home/*/junk
    *.tmp
    fm:aa:something/*
    re:^/home/[^/]+\.tmp/
    sh:/home/*/.thumbnails
    # Example with spaces, no need to escape as it is processed by borg
    some file with spaces.txt
    EOF
    $ borg create --exclude-from exclude.txt backup /

A more general and easier to use way to define filename matching patterns exists
with the ``--pattern`` and ``--patterns-from`` options. Using these, you may
specify the backup roots (starting points) and patterns for inclusion/exclusion.
A root path starts with the prefix `R`, followed by a path (a plain path, not a
file pattern). An include rule starts with the prefix +, an exclude rule starts
with the prefix -, an exclude-norecurse rule starts with !, all followed by a pattern.

.. note::

    Via ``--pattern`` or ``--patterns-from`` you can define BOTH inclusion and exclusion
    of files using pattern prefixes ``+`` and ``-``. With ``--exclude`` and
    ``--exclude-from`` ONLY excludes are defined.

Inclusion patterns are useful to include paths that are contained in an excluded
path. The first matching pattern is used so if an include pattern matches before
an exclude pattern, the file is backed up. If an exclude-norecurse pattern matches
a directory, it won't recurse into it and won't discover any potential matches for
include rules below that directory.

.. note::

    It's possible that a sub-directory/file is matched while parent directories are not.
    In that case, parent directories are not backed up thus their user, group, permission,
    etc. can not be restored.

Note that the default pattern style for ``--pattern`` and ``--patterns-from`` is
shell style (`sh:`), so those patterns behave similar to rsync include/exclude
patterns. The pattern style can be set via the `P` prefix.

Patterns (``--pattern``) and excludes (``--exclude``) from the command line are
considered first (in the order of appearance). Then patterns from ``--patterns-from``
are added. Exclusion patterns from ``--exclude-from`` files are appended last.

Examples::

    # backup pics, but not the ones from 2018, except the good ones:
    # note: using = is essential to avoid cmdline argument parsing issues.
    borg create --pattern=+pics/2018/good --pattern=-pics/2018 repo::arch pics

    # use a file with patterns:
    borg create --patterns-from patterns.lst repo::arch

The patterns.lst file could look like that::

    # "sh:" pattern style is the default, so the following line is not needed:
    P sh
    R /
    # can be rebuild
    - /home/*/.cache
    # they're downloads for a reason
    - /home/*/Downloads
    # susan is a nice person
    # include susans home
    + /home/susan
    # also back up this exact file
    + pf:/home/bobby/specialfile.txt
    # don't backup the other home directories
    - /home/*
    # don't even look in /proc
    ! /proc

You can specify recursion roots either on the command line or in a patternfile::

    # these two commands do the same thing
    borg create --exclude /home/bobby/junk repo::arch /home/bobby /home/susan
    borg create --patterns-from patternfile.lst repo::arch

The patternfile::

    # note that excludes use fm: by default and patternfiles use sh: by default.
    # therefore, we need to specify fm: to have the same exact behavior.
    P fm
    R /home/bobby
    R /home/susan

    - /home/bobby/junk

This allows you to share the same patterns between multiple repositories
without needing to specify them on the command line.

.. _borg_placeholders:

borg help placeholders
~~~~~~~~~~~~~~~~~~~~~~


Repository (or Archive) URLs, ``--prefix``, ``--glob-archives``, ``--comment``
and ``--remote-path`` values support these placeholders:

{hostname}
    The (short) hostname of the machine.

{fqdn}
    The full name of the machine.

{reverse-fqdn}
    The full name of the machine in reverse domain name notation.

{now}
    The current local date and time, by default in ISO-8601 format.
    You can also supply your own `format string <https://docs.python.org/3.8/library/datetime.html#strftime-and-strptime-behavior>`_, e.g. {now:%Y-%m-%d_%H:%M:%S}

{utcnow}
    The current UTC date and time, by default in ISO-8601 format.
    You can also supply your own `format string <https://docs.python.org/3.8/library/datetime.html#strftime-and-strptime-behavior>`_, e.g. {utcnow:%Y-%m-%d_%H:%M:%S}

{user}
    The user name (or UID, if no name is available) of the user running borg.

{pid}
    The current process ID.

{borgversion}
    The version of borg, e.g.: 1.0.8rc1

{borgmajor}
    The version of borg, only the major version, e.g.: 1

{borgminor}
    The version of borg, only major and minor version, e.g.: 1.0

{borgpatch}
    The version of borg, only major, minor and patch version, e.g.: 1.0.8

If literal curly braces need to be used, double them for escaping::

    borg create /path/to/repo::{{literal_text}}

Examples::

    borg create /path/to/repo::{hostname}-{user}-{utcnow} ...
    borg create /path/to/repo::{hostname}-{now:%Y-%m-%d_%H:%M:%S} ...
    borg prune --prefix '{hostname}-' ...

.. note::
    systemd uses a difficult, non-standard syntax for command lines in unit files (refer to
    the `systemd.unit(5)` manual page).

    When invoking borg from unit files, pay particular attention to escaping,
    especially when using the now/utcnow placeholders, since systemd performs its own
    %-based variable replacement even in quoted text. To avoid interference from systemd,
    double all percent signs (``{hostname}-{now:%Y-%m-%d_%H:%M:%S}``
    becomes ``{hostname}-{now:%%Y-%%m-%%d_%%H:%%M:%%S}``).

.. _borg_compression:

borg help compression
~~~~~~~~~~~~~~~~~~~~~


It is no problem to mix different compression methods in one repo,
deduplication is done on the source data chunks (not on the compressed
or encrypted data).

If some specific chunk was once compressed and stored into the repo, creating
another backup that also uses this chunk will not change the stored chunk.
So if you use different compression specs for the backups, whichever stores a
chunk first determines its compression. See also borg recreate.

Compression is lz4 by default. If you want something else, you have to specify what you want.

Valid compression specifiers are:

none
    Do not compress.

lz4
    Use lz4 compression. Very high speed, very low compression. (default)

zstd[,L]
    Use zstd ("zstandard") compression, a modern wide-range algorithm.
    If you do not explicitly give the compression level L (ranging from 1
    to 22), it will use level 3.
    Archives compressed with zstd are not compatible with borg < 1.1.4.

zlib[,L]
    Use zlib ("gz") compression. Medium speed, medium compression.
    If you do not explicitly give the compression level L (ranging from 0
    to 9), it will use level 6.
    Giving level 0 (means "no compression", but still has zlib protocol
    overhead) is usually pointless, you better use "none" compression.

lzma[,L]
    Use lzma ("xz") compression. Low speed, high compression.
    If you do not explicitly give the compression level L (ranging from 0
    to 9), it will use level 6.
    Giving levels above 6 is pointless and counterproductive because it does
    not compress better due to the buffer size used by borg - but it wastes
    lots of CPU cycles and RAM.

auto,C[,L]
    Use a built-in heuristic to decide per chunk whether to compress or not.
    The heuristic tries with lz4 whether the data is compressible.
    For incompressible data, it will not use compression (uses "none").
    For compressible data, it uses the given C[,L] compression - with C[,L]
    being any valid compression specifier.

obfuscate,SPEC,C[,L]
    Use compressed-size obfuscation to make fingerprinting attacks based on
    the observable stored chunk size more difficult.
    Note:
    - you must combine this with encryption or it won't make any sense.
    - your repo size will be bigger, of course.

    The SPEC value will determine how the size obfuscation will work:

    Relative random reciprocal size variation:
    Size will increase by a factor, relative to the compressed data size.
    Smaller factors are often used, larger factors rarely.
    1: factor 0.01 .. 100.0
    2: factor 0.1 .. 1000.0
    3: factor 1.0 .. 10000.0
    4: factor 10.0 .. 100000.0
    5: factor 100.0 .. 1000000.0
    6: factor 1000.0 .. 10000000.0

    Add a randomly sized padding up to the given size:
    110: 1kiB
    ...
    120: 1MiB
    ...
    123: 8MiB (max.)

Examples::

    borg create --compression lz4 REPO::ARCHIVE data
    borg create --compression zstd REPO::ARCHIVE data
    borg create --compression zstd,10 REPO::ARCHIVE data
    borg create --compression zlib REPO::ARCHIVE data
    borg create --compression zlib,1 REPO::ARCHIVE data
    borg create --compression auto,lzma,6 REPO::ARCHIVE data
    borg create --compression auto,lzma ...
    borg create --compression obfuscate,3,none ...
    borg create --compression obfuscate,3,auto,zstd,10 ...
    borg create --compression obfuscate,2,zstd,6 ...

