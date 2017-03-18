
Miscellaneous 
=============


-   [High Pitched Whining Noise on Idle (how to remove in Debian or
    Devuan)](#debian_powertop)
-   [High Pitched Whining Noise on Idle (how to remove in
    Parabola)](#high_pitch_parabola)
-   [X60/T60: Serial port - how to use (for dock owners)](#serial)
-   [Power Management Beeps on Thinkpads](#powerbeeps)
-   [Using diff and patch](patch.html)
-   [Finetune backlight control on intel gpu's](#backlight%20control)
-   [Get EDID: Find out the name (model) of your LCD
    panel](#get_edid_panelname)
-   [How to enable EHCI debugging on the BeagleBone
    Black](bbb_ehci.html)
-   [e1000e driver trouble shooting (Intel NICs)](#e1000-hang)



High Pitched Whining Noise on Idle (how to remove in Debian or Devuan) {#debian_powertop}
======================================================================


Start powertop automatically at boot time {#debian_powertop_autostart}
-----------------------------------------

Included with libreboot is a script called 'powertop.debian'. Run this
as root and it will setup powertop to run with \--auto-tune at boot
time. Load the file in your text editor to see how it does that.

$ **sudo ./resources/scripts/misc/powertop.debian**

Might want to run with \--calibrate first

If powertop doesn't work, another way (reduces battery life slightly)
is to add *processor.max\_cstate=2* to the *linux* line in grub.cfg,
using [this guide](../gnulinux/grub_cbfs.html).





High Pitched Whining Noise on Idle (how to remove in Parabola) {#high_pitch_parabola}
==============================================================

The following removes most of the noise. It reduces what is a high
frequency whine (that not everyone can hear) to a slight buzz (which
most people can't hear or doesn't bother most people).

This is not perfect! The full solution is still not discovered but this
is a step towards that. Also, in some instances you will need to run
'sudo powertop \--auto-tune' again. This needs to be implemented
properly in coreboot itself!

On the X60 with coreboot or libreboot, there is a high pitched sound
when idle. So far we have use processor.max\_cstate=2 or idle=halt in
GRUB. These consume power. Stop using them!

Be root\
**$ su -**

Installed powertop:\
**\# pacman -S powertop**

and added the following to /etc/systemd/system/powertop.service :

    [Unit]
    Description=Powertop tunings

    [Service]
    Type=oneshot
    RemainAfterExit=no
    ExecStart=/usr/bin/powertop --auto-tune
    # "powertop --auto-tune" still needs a terminal for some reason. Possibly a bug?
    Environment="TERM=xterm"

    [Install]
    WantedBy=multi-user.target

Finally, as root do that:\
    \# systemctl enable powertop
**\# systemctl start powertop**

The next time you boot the system, the buzz will be gone.

Might want to run with \--calibrate first

If powertop doesn't work, another way (reduces battery life slightly)
is to add *processor.max\_cstate=2* to the *linux* line in grub.cfg,
using [this guide](../gnulinux/grub_cbfs.html).




X60/T60: Serial port - how to use (for dock owners) {#serial}
===================================================

For the Thinkpad X60 you can use the **"UltraBase X6"** dock (for the
X60 Tablet it is called X6 Tablet UltraBase). For the ThinkPad T60, you
can use the **"Advanced Mini Dock"**.

If you are using one of the ROM images with 'serial' in the name, then
you have serial port enabled in libreboot and you have memtest86+
included inside the ROM. Connect your null modem cable to the serial
port on the dock and connect the other end to a 2nd system using your
USB Serial adapter.

On the 2nd system, you can try this (using GNU Screen):\
**$ sudo screen /dev/ttyUSB0 115200**

How to quit GNU Screen: Ctrl+A then release and press K, and then press
Y.

There are also others like Minicom but I like GNU Screen

By doing this before booting the X60/T60, you will see console output
from libreboot. You will also see GRUB displaying on the serial output,
and you will be able to see MemTest86+ on the serial output aswell. You
can also configure your distro so that a terminal (TTY) is accessible
from the serial console.

The following guide is for Ubuntu, but it should work in Debian and
Devuan, to enable a serial console using GeTTY:\
<https://help.ubuntu.com/community/SerialConsoleHowto> (we DO NOT
recommend Ubuntu, because it contains non-free software in the default
repos. Use Debian or Devuan)

Note: part of the tutorial above requires changing your grub.cfg. Just
change the **linux** line to add instructions for enabling getty. See
[../gnulinux/grub\_cbfs.html](../gnulinux/grub_cbfs.html).




Finetune backlight control on intel gpu's
=========================================

Sometimes the backlight control value (BLC\_PWM\_CTL) set by libreboot
is not ideal. The result is either flicker, which could cause nausea or
epilepsy or an uneven backlight and/or coil whine coming from the
display. To fix this a different value for the gpu reg BLC\_PWM\_CTL
needs to be set. See p94 of
<https://01.org/sites/default/files/documentation/g45_vol_3_register_0_0.pdf>
for more information on this reg. The tool for setting registry values
on intel gpu's is included in intel-gpu-tools. Install intel-gpu-tools:
**sudo apt-get install intel-gpu-tools**

You can set values: **sudo intel\_reg write 0x00061254
&ltyour\_value>**

The value set has the following structure: bits \[31:16\] is PWM
divider. PWM / PWM\_divider = frequency bits \[15:0\] is the duty cycle
and determines the portion of the backlight modulation frequency. A
value of 0 will mean that the display is off. A value equal to the
backlight modulation frequency means full on. The value should not be
larger than the backlight modulation frequency.

On displays with a CCFL backlight start from: 0x60016001 To verify if
all modes work as desired use: **xbacklight -set 10** and gradually
increase until 100. Displays with an LED backlight need a lower
backlight modulation. Do the same thing but start from 0x01290129 . Try
setting different values until you have found a value which presents no
issue.

It is important to know that there are four failure modes:

1.  flickering very fast, which could cause epilepsy (frequency is too
    low. decrease divider)
2.  randomly flickering in random intervals - driver IC cannot keep up
    with toggling the mosfet (frequency too fast, increase divider)
3.  frequency is in audible range and causes coils to whine (frequency
    is too high. increase divider)
4.  backlight is uneven. (CCFL specific, frequency is too high. increase
    divider)

To check for flickering try moving your laptop while looking at it.

A higher frequency equals higher power consumption. You want to find the
highest good working value.

Next this value should be set at boot: either add

    intel_reg write 0x00061254 &ltyour_ideal_value>

before exit 0 in /etc/rc.local or create a systemd service file
/etc/systemd/system/backlight.service:
    [Unit]
    Description=Set BLC_PWM_CTL to a good value
    [Service]
    Type=oneshot
    RemainAfterExit=no
    ExecStart=/usr/bin/intel_reg write 0x00061254 &ltyour_value>
    [Install]
    WantedBy=multi-user.target
            

Now start and enable it: **sudo systemctl start backlight && sudo
systemctl enable backlight**
Special note on i945:

i945 behaves differently. Bit 16 needs to be 1 and the duty cycle is not
updated when backlight is changed. There are no available datasheets on
this target so why it behaves in such a way is not known. So to find a
working value BLC\_PWM\_CTL set bit 16 to 1 and make sure PWM divider ==
duty cycle. see <https://review.coreboot.org/#/c/10624/> on bit 16. The
cause of this issue is that i945, in contrast with to GM45, is set to
work in BLM Legacy Mode. This makes backlight more complicated since the
duty cycle is derived from 3 instead of 2 registers using the following
formula: if(BPC\[7:0\] <> xFF) then BPCR\[15:0\] * BPC\[7:0\]
Else BPCR\[15:0\] BPC is LBB - PCI Backlight Control Register, described
on <http://www.mouser.com/pdfdocs/945gmedatasheet.pdf> on page 315. BPCR
is BLC\_PWM\_CTL described in
<https://01.org/sites/default/files/documentation/g45_vol_3_register_0_0.pdf>
on page 94. More research needs to be done on this target so proceed
with care.



Power Management Beeps on Thinkpads
===================================

When disconnecting or connecting the charger, a beep occurs. When the
battery goes to a critically low charge level, a beep occurs. Nvramtool
is included in libreboot, and can be used to enable or disable this
behaviour.

Disable or enable beeps when removing/adding the charger:\
$     sudo ./nvramtool -w power\_management\_beeps=Enable
$ **sudo ./nvramtool -w power\_management\_beeps=Disable**

Disable or enable beeps when battery is low:\
$     sudo ./nvramtool -w low\_battery\_beep=Enable
$ **sudo ./nvramtool -w low\_battery\_beep=Disable**

A reboot is required, for these changes to take effect.



Get EDID: Find out the name (model) of your LCD panel {#get_edid_panelname}
=====================================================

Get the panel name with     sudo get-edid | strings
Or look in **/sys/class/drm/card0-LVDS-1/edid**

Alternatively you can use i2cdump. In Debian and Devuan, this is in the
package i2c-tools.\
$     sudo modprobe i2c-dev
$ **sudo i2cdump -y 5 0x50** (you might have to change the value for
-y)\
$     sudo rmmod i2c-dev
You'll see the panel name in the output (from the EDID dump).

If neither of these options work (or they are unavailable), physically
removing the LCD panel is an option. Usually, there will be information
printed on the back.

[Back to top of page.](#pagetop)



e1000e driver trouble shooting (Intel NICs) {#e1000-hang}
===========================================

Example error, ¿may happen on weird and complex routing schemes(citation
needed for cause):

    e1000e 0000:00:19.0 enp0s25: Detected Hardware Unit Hang

Possible workaround, tested by Nazara: Disable C-STATES.\
**NOTE: this also disables power management, because disabling C-States
means that your CPU will now be running at full capacity (and therefore
using more power) non-stop, which will drain battery life if this is a
laptop. If power usage is a concern, then you should not use this.
(we're also not sure whether this workaround is appropriate)**

To disable c-states, do this in GNU+Linux:\
**for i in /sys/devices/system/cpu/cpu/cpuidle/state/disable; do echo 1
> $i; done**

You can reproduce this issue more easily by sending lots of traffic
across subnets on the same interface (NIC).

More information, including logs, can be found on [this
page](https://notabug.org/vimuser/libreboot/issues/23).



USB keyboard wakeup on GM45 laptops {#usb_keyboard_gm45}
===================================

Look at resources/scripts/helpers/misc/libreboot\_usb\_bugfix

Put this script in /etc/init.d/ on debian-based systems.



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
