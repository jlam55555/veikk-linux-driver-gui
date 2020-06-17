# VEIKK Linux Driver Configuration Tool
A graphical configuration tool to easily customize the module parameters for the
[VEIKK Linux Driver][1] (v2.0) for [VEIKK][0]-brand drawing tablets.

![image highlighting the v2 release][11]

---

**Note: You must run the configuration tool as root to be able to apply any
changes.**

### Implemented options
This GUI allows you to customize the sysfs module parameters for the VEIKK
Linux driver. The exact format and requirements of all parameters in sysfs is
fully documented in [the driver code][3]. Currently, the following options are
supported:

##### Screen mapping
Select a rectangular region of the screen to map. The mapped area of the screen
may be set manually by typing in the values, or you may drag the mapped region
out on the screen preview for convenience.
- *Screen width, height*: automatically set (needed by driver to set screen map)
- *Screen map x, y*: pixel coordinates of the top left of the mapped area of the
  screen
- *Screen map width, height*: pixel dimensions of the mapped area of the screen
  (must be positive)

##### Orientation
Choose one of four tablet orientations. Select these in the dropdown.
- Default
- Rotated 90deg CCW
- Flipped (rotated 180deg)
- Rotated 90deg CW

##### Pressure mapping
Set the parameters for a cubic function mapping physical to transmitted
pressure. The coefficients are for a 1x1 square, and upscaled to the tablet's
resolution; i.e., if the input pressure (x-axis) ranges from [0, 1], this
mapping maps takes the input pressure to an output pressure also in [0, 1].
These may be inputted manually or calculated automatically by dragging the
control points on the pressure mapping graph. A list of default mappings is also
provided in a dropdown for convenience.
- *a0, a1, a2, a3*: Coefficients to the cubic mapping
`p_out=a0+a1*p_in+a2*p_in**2+a3*p_in**3`

---

### Compiling and executing the tool
To compile, make sure you have `qmake` installed on your system, with a Qt
version (`>=5.12`). (For development, the Qt Creator IDE w/ Qt Designer is
recommended; this should come bundled with Qt.) Run:

```bash
qmake && make all clean
```

If this fails to build, you can cleanup, and then ensure you are using the correct qmake by re-running with this:

```bash
make clean
QT_SELECT=5 qmake && make all clean
```

This should generate the binary. **Make sure to run it as root, or else you
will not be able to edit the module parameters in sysfs.**

```bash
sudo ./veikk-linux-driver-gui
```

If this fails to connect to the display when runing as root, ensure that root
is able to use your display, and then re-run with with the correct DISPLAY var:

```bash
xhost local:root
sudo DISPLAY=$DISPLAY sudo ./veikk-linux-driver-gui
```

Enjoy configuring your driver!
    
---

### Development status
Currently, development is focused on the S640, but PR's are welcome. The
developer only has an S640 at the moment and is unable to directly develop new
features for other models, but open to feedback and questions at jonlamdev
at gmail dot com.

##### Future features
- More detailed warnings/feedback to user (e.g., when not launched with root
  privileges)
- Ability to save/load configurations from files
- Ability to apply module parameters on startup (currently they don't persist
  through a reboot cycle)
- Button mapping and other configuration options
- Device-unique configuration
- Packaging for easier distribution

[0]: https://www.veikk.com/
[1]: https://github.com/jlam55555/veikk-linux-driver
[2]: https://twitter.com/jlam55555/status/1138285016209854464?s=20
[3]: http://eis.jonlamdev.com/posts/on-developing-a-linux-driver
[4]: https://github.com/jlam55555/veikk-linux-driver/blob/master/veikk_modparms.c
[5]: https://github.com/jlam55555/veikk-s640-driver/issues
[6]: https://github.com/jlam55555/veikk-linux-driver/issues/3
[7]: https://github.com/artixnous
[8]: https://aur.archlinux.org/packages/input-veikk-dkms/
[9]: ./veikk_modparms.c
[10]: https://github.com/jlam55555/veikk-linux-driver-gui
[11]: https://i.imgur.com/Mug8gRn.jpg
