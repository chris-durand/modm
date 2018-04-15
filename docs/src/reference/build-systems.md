# Build Systems

Before modm our HAL code generation was done as a [SCons][] tool during compilation.
This worked for us, however, made it very difficult to include our code into other build systems.
We therefore made the code generation a separate step with lbuild.

The [`modm:build.scons` module](module/modm-build-scons) extends the default
[SCons build system][scons] with [many custom utilities][scons_tools] for a
smooth integration of embedded tools.
These are the standard commands:

<center>
<table>
<tr>
<td>`scons`</td>
<td>compiles your application and shows static resource consumption</td>
</tr><tr>
<td>`scons build`</td>
<td>compiles your application into an executable</td>
</tr><tr>
<td>`scons size`</td>
<td>displays the static Flash and RAM consumption</td>
</tr><tr>
<td>`scons program`</td>
<td>writes the executable onto your target via Avrdude or OpenOCD</td>
</tr><tr>
<td>`scons symbols`</td>
<td>dumps the symbol table for your executable</td>
</tr><tr>
<td>`scons listing`</td>
<td>Decompiles your executable into an annotated assembly listing</td>
</tr><tr>
<td>`scons -c`</td>
<td>removes the build files</td>
</tr><tr>
<td>`scons verbose=1`</td>
<td>Makes the printout more verbose</td>
</tr>
</table>
</center>

The [`modm:build.cmake` module](module/modm-build-cmake) generates .
This module ships with a Makefile that wraps all of the cmake commands, so you
can use these commands:

<center>
<table>
<tr>
<td>`make`</td>
<td>Generates CMake files and compiles your program into an executable</td>
</tr><tr>
<td>`make build-release`</td>
<td>Compiles your program into an executable</td>
</tr><tr>
<td>`make build-debug`</td>
<td>Compiles your program into a debugable executable</td>
</tr><tr>
<td>`make upload-release`</td>
<td>Writes the executable onto your target</td>
</tr><tr>
<td>`make upload-debug`</td>
<td>Writes the debugable executable onto your target</td>
</tr><tr>
<td>`make gdb`</td>
<td>starts a debug session for the debugable executable</td>
</tr>
</table>
</center>


<!-- ## Advanced SCons Usage

The `modm:build.scons` module generates a `modm/SConscript` file which contains all of
the specific 

To get the best value out of our SCons Tools, we recommend writing your own `SConstruct` file.


## Your own build scripts -->



<!-- - `optimization=[0,1,2,3,s]`: Forces compilation with specified optimization level. Can be used for debug builds. -->


<!-- ### AVR only:

- `fuse`: Writes the fuse bits onto your target.
- `eeprom`: Writes the EEPROM memory onto your target.

### ARM Cortex-M only:

- `debug`: Starts the GDB debug session of your current application in text UI mode.  
           You must execute `openocd-debug` or `lpclink-debug` before running this command!
- `openocd-debug`: Starts the OpenOCD debug server for your target.
- `lpclink-debug`: Starts the LPC-Link debug server for your target.
- `lpclink-init`: Initialize the LPC-Link with its proprietary firmware. -->


[cmake]: https://cmake.org
[scons]: http://scons.org
[scons_tools]: https://github.com/modm-io/scons-build-tools