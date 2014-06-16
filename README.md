# blender file loading for open frameworks

This addon enables native loading of Blenders blend file format. It translates the data into extended of core objects like ofNode, ofMesh, ofCamera.

Most objects extend ofNode and have ofLight, ofCamera etc as a child object.
   

## Features

### supports

* scenes
* meshes (tris and quads)
* materials (colors & textures)
* animation data (position, rotation, scale, camera lens, object visibility)
* cameras
* lights
* scene hierarchy 
* markers

## limitations

* Only tested with Blender 2.7, will not work with blender 2.4 versions and only partially with blender 2.6

## platforms

Developed and tested on linux (Arch 64bit), but should also work on Mac & Windows. Requires c++11.
