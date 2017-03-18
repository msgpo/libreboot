
Depthcharge is currently not documented, since it is in the new build
system. Instructions for how to build boards that have depthcharge are
included in the BUILD\_HOWTO file in libreboot.git or \_src.



Building libreboot from source 
==============================

This section relates to building libreboot from source, and working with
the git repository.


-   [Install build dependencies](#build_dependencies)
-   [Get the full source code from metadata (git clone)](#build_meta)
-   [How to build "bucts" (for LenovoBIOS
    X60/X60S/X60T/T60)](#build_bucts)
-   [How to build "flashrom"](#build_flashrom)
-   [How to build the ROM images](#build)



Install build dependencies {#build_dependencies}
==========================

Before doing anything, you need the dependencies first. This is true if
you want to build libreboot from source, with either
libreboot\_src.tar.xz or git. **If you are using libreboot\_util.tar.xz
(binary archive) then you can ignore this, because ROM images and
statically compiled executables for the utilities are included.**


For Debian Stretch (may also work on Debian Jessie), you can run the
following command:\
$     sudo ./oldbuild dependencies debian
(this will also work in Devuan)

For Parabola, you can run the following command:\
$     sudo ./oldbuild dependencies parabola
or:\
\# **./oldbuild dependencies parabola**


For other GNU+Linux distributions, you can adapt the existing scripts.




Get the full source code from metadata (git clone) {#build_meta}
==================================================

If you downloaded libreboot from git, then there are some steps to
download and patch the source code for all relevant dependencies. The
archive in the git repository used to be available as a tarball called
'libreboot\_meta.tar.gz'. It contains 'metadata' (scripts) which
define how the source was created (where it came from).

You can use the scripts included to download everything.

First, [install the build dependencies](#build_dependencies).

Since libreboot makes extensive use of git, you need to configure git
properly. If you have not yet configured git, then the minimum
requirement is:\
$     git config \--global user.name "Your Name"
$     git config \--global user.email your@emailaddress.com
This is what will also appear in git logs if you ever commit your own
changes to a given repository. For more information, see
<http://git-scm.com/doc>.

Another nice config for you (optional, but recommended):\
$     git config \--global core.editor nano
$     git config \--global color.status auto
$     git config \--global color.branch auto
$     git config \--global color.interactive auto
$ **git config \--global color.diff auto**

After that, run the script:\
$ **./download all**

What this did was download everything (grub, coreboot, memtest86+,
bucts, flashrom) at the versions last tested for this release, and patch
them. Read the script in a text editor to learn more.

To build the ROM images, see [\#build](#build).

[Back to top of page.](#pagetop)



How to build "bucts" (for LenovoBIOS X60/X60S/X60T/T60) {#build_bucts}
=========================================================

**This is for Lenovo BIOS users on the ThinkPad X60/X60S, X60 Tablet and
T60. If you have coreboot or libreboot running already, ignore this.**

BUC.TS isn't really specific to these laptops, but is a bit inside the
a register in the chipset on some Intel systems.

Bucts is needed when flashing in software the X60/X60S/X60T/T60 ROM
while Lenovo BIOS is running; external flashing will be safe regardless.
Each ROM contains identical data inside the two final 64K region in the
file*. This corresponds to the final two 64K regions in the flash chip.
Lenovo BIOS will prevent you from writing the final one, so running
"**bucts 1**" will set the system to boot from the other block instead
(which is writeable along with everything beneath it when using a
patched flashrom. see [\#build\_flashrom](#build_flashrom)). After
shutting down and booting up after the first flash of libreboot, the
final 64K block is writeable so you flash the ROM again with an
unpatched flashrom and run "**bucts 0**" to make the system boot from
the normal (highest) block again.

*Libreboot ROM images have identical data in those two 64KiB regions
because dd is used to do that, by the build system. If you're building
from upstream (coreboot), you have to do it manually.

BUC.TS is backed up (powered) by the NVRAM battery (or CMOS battery, as
some people call it). On thinkpads, this is typically in a yellow
plastic package with the battery inside, connected via power lines to
the mainboard. Removing that battery removes power to BUC.TS, resetting
the bit back to 0 (if you previously set it to 1).

BUC.TS utility is included in libreboot\_src.tar.xz and
libreboot\_util.tar.xz.\
**If you downloaded from git, follow [\#build\_meta](#build_meta) before
you proceed.**

"BUC" means "**B**ack**u**p **C**ontrol" (it's a register) and
"TS" means "**T**op **S**wap" (it's a status bit). Hence "bucts"
(BUC.TS). TS 1 and TS 0 corresponds to bucts 1 and bucts 0.

If you have the binary release archive, you'll find executables under
./bucts/. Otherwise if you need to build from source, continue reading.

First, [install the build dependencies](#build_dependencies).

To build bucts, do this in the main directory:\
$ **./oldbuild module bucts**

To statically compile it, do this:\
$ **./oldbuild module bucts static**

The "builddeps" script in libreboot\_src also makes use of
builddeps-bucts.

[Back to top of page.](#pagetop)



How to build "flashrom" {#build_flashrom}
=========================

Flashrom is the utility for flashing/dumping ROM images. This is what
you will use to install libreboot.

Flashrom source code is included in libreboot\_src.tar.xz and
libreboot\_util.tar.xz.\
**If you downloaded from git, follow [\#build\_meta](#build_meta) before
you proceed.**

If you are using the binary release archive, then there are already
binaries included under ./flashrom/. The flashing scripts will try to
choose the correct one for you. Otherwise if you wish to re-build
flashrom from source, continue reading.

First, [install the build dependencies](#build_dependencies).

To build it, do the following in the main directory:\
$ **./oldbuild module flashrom**

To statically compile it, do the following in the main directory:\
$ **./oldbuild module flashrom static**

After you've done that, under ./flashrom/ you will find the following
executables:

-   **flashrom**
    -   For flashing while coreboot or libreboot is running.
-   **flashrom\_lenovobios\_sst**
    -   This is patched for flashing while Lenovo BIOS is running on an
        X60 or T60 with the SST25VF016B (SST) flash chip.
-   **flashrom\_lenovobios\_macronix**
    -   This is patched for flashing while Lenovo BIOS is running on an
        X60 or T60 with the MX25L1605D (Macronix) flash chip.

The "builddeps" script in libreboot\_src also makes use of
builddeps-flashrom.

[Back to top of page.](#pagetop)



How to build the ROM images {#build}
===========================

You don't need to do much, as there are scripts already written for you
that can build everything automatically.

You can build libreboot from source on a 32-bit (i686) or 64-bit
(x86\_64) system. Recommended (if possible): x86\_64. ASUS KFSN4-DRE has
64-bit CPUs. On a ThinkPad T60, you can replace the CPU (Core 2 Duo
T5600, T7200 or T7600. T5600 recommended) for 64-bit support. On an
X60s, you can replace the board with one that has a Core 2 Duo L7400
(you could also use an X60 Tablet board with the same CPU). On an X60,
you can replace the board with one that has a Core 2 Duo T5600 or T7200
(T5600 is recommended). All MacBook2,1 laptops are 64-bit, as are all
ThinkPad X200, X200S, X200 Tablet, R400, T400 and T500 laptops. Warning:
MacBook1,1 laptops are all 32-bit only.

First, [install the build dependencies](#build_dependencies).

If you downloaded libreboot from git, refer to
[\#build\_meta](#build_meta).

Build all of the components used in libreboot:\
$ **./oldbuild module all**

You can also build each modules separately, using *./oldbuild module
modulename*. To see the possible values for *modulename*, use:\
$ **./oldbuild module list**

After that, build the ROM images (for all boards):\
$     ./oldbuild roms withgrub
Alternatively, you can build for a specific board or set of boards. For
example:\
$     ./oldbuild roms withgrub x60
$     ./oldbuild roms withgrub x200\_8mb
$     ./oldbuild roms withgrub x60 x200\_8mb
The list of board options can be found by looking at the directory names
in **resources/libreboot/config/grub/**.

To clean (reverse) everything, do the following:\
$ **./oldbuild clean all**

The ROM images will be stored under **bin/*payload*/**, where *payload*
could be *grub*, *seabios*, or whatever other payload those images were
built for.


Preparing release archives (optional)
-------------------------------------

**This is only confirmed to work (tested) in Debian Stretch. Parabola
*fails* at this stage (for now). For all other distros, YMMV. This
will also work in Devuan.**

This is mainly intended for use with the git repository. These commands
will work in the release archive (\_src), unless otherwise noted below.

The archives will appear under *release/oldbuildsystem/${version}/*;
${version} will either be set using *git describe* or, if a *version*
file already exists (\_src release archive), then it will simply re-use
that.

Tag the current commit, and that version will appear in both the
${version} string on the directory under *release/oldbuildsystem/*, and
in the file names of the archives. Otherwise, whatever git uses for *git
describe \--tags HEAD* will be used.

Utilities (static executables):\
$ **./oldbuild release util**

Archive containing flashrom and bucts source code:\
$ **./oldbuild release tobuild**

Documentation archive (**does not work on \_src release archive, only
git**):\
$ **./oldbuild release docs**

ROM image archives:\
$ **./oldbuild release roms**

Source code archive:\
$ **./oldbuild release src**

SHA512 sums of all other release archives that have been generated:\
$ **./oldbuild release sha512sums**

If you are building on an i686 host, this will build statically linked
32-bit binaries in the binary release archive that you created, for:
**nvramtool, cbfstool, ich9deblob, cbmem**.

If you are building on an x86\_64 host, this will build statically
linked 32- *and* 64-bit binaries for **cbmem**, **ich9deblob**,
**cbfstool** and **nvramtool**.

**To include statically linked i686 and x86\_64 binaries for bucts and
flashrom, you will need to build them on a chroot, a virtual system or a
real system where the host uses each given architecture. These packages
are difficult to cross-compile, and the libreboot project is still
figuring out how to deal with them.**

The same applies if you want to include statically linked flashrom
binaries for ARM.

armv7l binaries (tested on a BeagleBone Black) are also included in
libreboot\_util, for:

-   cbfstool
-   ich9gen
-   ich9deblob
-   flashrom

If you are building binaries on a live system or chroot (for
flashrom/bucts), you can use the following to statically link them:\
$     ./oldbuild module flashrom static
$ **./oldbuild module bucts static**

The same conditions as above apply for ARM (except, building bucts on
ARM is pointless, and for flashrom you only need the normal executable
since the lenovobios\_sst and \_macronix executables are meant to run on
an X60/T60 while lenovo bios is present, working around the security
restrictions).

The command that you used for generating the release archives will also
run the following command:\
$     ./oldbuild release tobuild
The archive **tobuild.tar.xz** will have been created under
**release/oldbuildsystem/**, containing bucts, flashrom and all other
required resources for building them.

You'll find that the files libreboot\_util.tar.xz and
libreboot\_src.tar.xz have been created, under
**release/oldbuildsystem/**.

The ROM images will be stored in separate archives for each system,
under **release/oldbuildsystem/rom/**.





Copyright © 2014, 2015, 2016 Leah Rowe <info@minifree.org>\
Permission is granted to copy, distribute and/or modify this document
under the terms of the Creative Commons Attribution-ShareAlike 4.0
International license or any later version published by Creative
Commons; A copy of the license can be found at
[../cc-by-sa-4.0.txt](../cc-by-sa-4.0.txt)

Updated versions of the license (when available) can be found at
<https://creativecommons.org/licenses/by-sa/4.0/legalcode>

UNLESS OTHERWISE SEPARATELY UNDERTAKEN BY THE LICENSOR, TO THE EXTENT
POSSIBLE, THE LICENSOR OFFERS THE LICENSED MATERIAL AS-IS AND
AS-AVAILABLE, AND MAKES NO REPRESENTATIONS OR WARRANTIES OF ANY KIND
CONCERNING THE LICENSED MATERIAL, WHETHER EXPRESS, IMPLIED, STATUTORY,
OR OTHER. THIS INCLUDES, WITHOUT LIMITATION, WARRANTIES OF TITLE,
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT,
ABSENCE OF LATENT OR OTHER DEFECTS, ACCURACY, OR THE PRESENCE OR ABSENCE
OF ERRORS, WHETHER OR NOT KNOWN OR DISCOVERABLE. WHERE DISCLAIMERS OF
WARRANTIES ARE NOT ALLOWED IN FULL OR IN PART, THIS DISCLAIMER MAY NOT
APPLY TO YOU.

TO THE EXTENT POSSIBLE, IN NO EVENT WILL THE LICENSOR BE LIABLE TO YOU
ON ANY LEGAL THEORY (INCLUDING, WITHOUT LIMITATION, NEGLIGENCE) OR
OTHERWISE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, CONSEQUENTIAL,
PUNITIVE, EXEMPLARY, OR OTHER LOSSES, COSTS, EXPENSES, OR DAMAGES
ARISING OUT OF THIS PUBLIC LICENSE OR USE OF THE LICENSED MATERIAL, EVEN
IF THE LICENSOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSSES,
COSTS, EXPENSES, OR DAMAGES. WHERE A LIMITATION OF LIABILITY IS NOT
ALLOWED IN FULL OR IN PART, THIS LIMITATION MAY NOT APPLY TO YOU.

The disclaimer of warranties and limitation of liability provided above
shall be interpreted in a manner that, to the extent possible, most
closely approximates an absolute disclaimer and waiver of all liability.
