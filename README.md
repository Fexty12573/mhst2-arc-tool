# mhst2-arc-tool
Monster Hunter Stories 2 Arc un- and repacker based on https://github.com/Silvris/MH-Tools-and-Scripts.

Specifically the Extract and Repack python scripts.

## Usage
#### Unpacking
* Drag a packed .arc file from the game folder onto the exe.
  * The tool will automatically recognize that you are trying to unpack something
  * Inside the unpacked folder there will be an `orderlog.txt`, do *not* delete it, you will need it to repack the arc!
  * It will also generate an `[arc name]-decrypt.arc`, which you can delete after unpacking is done.
#### Repacking
* Drag an unpacked arc **folder** onto the exe.
  * The folder *needs* to have an `orderlog.txt` inside that lists all of the arcs files.
  * Output will be `[folder name]-repacked.arc`

## Building
Used libraries can be installed either manually or with vcpkg.
#### vcpkg
```
$ vcpkg install zlib cryptopp boost-crc --triplet=x64-windows-static
```

There is an option to enable progress bars (which massively slows down the code). The libraries for that can be installed with
```
$ vcpkg install indicators fmt --triplet=x64-windows-static
```

You can omit the `-static` if you wish to use the libraries in dll form.

Then simply open the solution is Visual Studio, make sure to select x64 as the target, and build.
