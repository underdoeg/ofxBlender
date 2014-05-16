# blender file loading for open frameworks

This addon enables native loading of Blenders blend file format. It translates the data into extended of core objects like ofNode, ofMesh, ofCamera.

Most objects have an inheritance structure like this

[ADD IMAGE HERE]

## Features

### supports

* scenes
* meshes (tris and quads)
* materials (colors & textures)
* animation data (position, rotation, scale)
* cameras
* lights

### planned

* scene hierarchy 

## limitations

* Only tested with Blender 2.7, will not work with blender 2.4 versions and only partially with blender 2.6

## platforms

Developed and tested on linux (Arch 64bit), but should also work on Mac & Windows.
