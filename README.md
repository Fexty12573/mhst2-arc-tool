# mhst2-arc-repacker
mh stories 2 arc repacker based on https://github.com/Silvris/MH-Tools-and-Scripts/blob/master/MHStories2ARCRepack.py

## Usage
* Drag unpacked arc folder onto the exe with orderlog inside
* Wait

## Building
Used libraries can be installed either manually or with vcpkg.
#### vcpkg
```
$ vcpkg install zlib cryptopp --triplet=x64-windows-static
```

There is an option to enable progress bars (which massively slows down the code). The libraries for that can be installed with
```
$ vcpkg install indicators fmt --triplet=x64-windows-static
```

You can omit the `-static` if you wish to use the libraries in dll form.

Then simply open the solution is Visual Studio, make sure to select x64 as the target, and build.
