# blender file loading for open frameworks

This addon enables native loading of Blenders blend file format. It translates the data into extended of core objects like ofNode, ofMesh, ofCamera.

Most objects have an inheritance structure like this

[ADD IMAGE HERE]

## Features

### partially implemented

* scenes

### planned

* materials (colors & textures)
* animation data (position, rotation, scale)
* scene hierarchy 
* meshes (tris and quads)
* cameras
* lights

## limitations

* Compressed blend files are not supported 
* Only tested with Blender 2.7

## platforms

Developed on linux (Arch 64bit), but should also work on Mac & Windows.