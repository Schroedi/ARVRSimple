# Powerwall module for the Godot engine

This GDNative module adds asymetric stereo projection to Godot. Currently planar projection for powerwalls and fish-tank like applications are supported. In the sample the camera tracking is done with VRPN.

# Status
The module works but is a bit hacky. It assumes elements with specific names and in specific positions to be present in the project. It was only tested on linux. For Windows or Mac the libraries need to be build and added to the project. 

Currently the projection seems slightly off. It looks like the rotation center is not completely correct. This could also be caused by a wrong calibation of the tracking to godot coordinate systems though.

# Compiling
1. Check out the project with submodules
2. Compile the ARVR module from the git root: scons -p=x11
3. Open the sample in the demo folder
4. Adjust the screens size and position relative to the ARVR root (this should be improved)
5. Set your VRPN tracker's name in PowerwallCamer.gd


# Thanks
This project borrows a lot from the excellent work by BastiaanOlij. Further, I would like give a special thanks to thank bojidar-bg and Karoffel who helped me tremendiously with getting started at the Godot dev sprint.
