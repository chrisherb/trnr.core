# trnr.core

A collection of Max externals used in the development of the M4L devices by Ternär Music Technology.

## Externals

- **[redux](https://github.com/ChrisHerb/trnr.core/blob/master/source/projects/trnr.core.redux_tilde/trnr.core.redux_tilde.cpp)** bitrate reducer with ulaw compansion
- **[retrobuf](https://github.com/ChrisHerb/trnr.core/blob/master/source/projects/trnr.core.retrobuf_tilde/trnr.core.retrobuf_tilde.cpp)** read from a buffer like it's 1995
- **[txvoice](https://github.com/ChrisHerb/trnr.core/blob/master/source/projects/trnr.core.txvoice_tilde/trnr.core.txvoice_tilde.cpp)** 3 operator FM synth voice
- **[ysvf](https://github.com/ChrisHerb/trnr.core/blob/master/source/projects/trnr.core.ysvf_tilde/trnr.core.ysvf_tilde.cpp)** state-variable filter based on the Y filters by [Chris Johnson](https://github.com/airwindows)

## Building

### Mac

Run `cmake -G Xcode ..`

Next run `cmake --build .` or open the Xcode project from this "build" folder and use the GUI.

Note: you can add the `-j4` option where "4" is the number of cores to use. This can help to speed up your builds, though sometimes the error output is interleaved in such a way as to make troubleshooting more difficult.

If you are running on a Mac M1 machine, you will likely see an error `cannot be loaded due to system security policy` when loading your externals in Max. To resolve this, you can ad-hoc codesign your external with `codesign --force --deep -s - myobject.mxo`.

### Windows

You can run `cmake --help` to get a list of the options available. Assuming some version of Visual Studio 2019, the commands to generate the projects will look like this:

`cmake -G "Visual Studio 16 2019" ..`

Or using Visual Studio 2017 it will look like this:

`cmake -G "Visual Studio 15 2017 Win64" ..`

Having generated the projects, you can now build by opening the .sln file in the build folder with the Visual Studio app (just double-click the .sln file) or you can build on the command line like this:

`cmake --build . --config Release`

## License

This project is licenced under the MIT license.
