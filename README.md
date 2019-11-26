# Powerwall module for the Godot engine

This GDNative module adds asymmetric stereo projection to Godot. Currently planar projection for powerwalls and fish-tank like applications are supported. In the sample the camera tracking is done with VRPN.

# Status
The module works but is a bit hacky. 
- The powerwall size and position as well as the tracking system orientation is configured in C++
- The VRPN tracker is configured in the C++ code as well
Currently the projection seems slightly off. It looks like the rotation center is not completely correct. This could also be caused by a wrong calibration of the tracking to godot coordinate systems though.

# Compiling
1. Check out the project with submodules (git submodule update --init)
2. Build the project with cmake
3. Delete the cmake lines in the VRPN project in the build folder that check for in tree builds. They seem to be broken. Just follow the error and delete the loop - build again
4. Open the sample in the demo folder

# Thanks
This project borrows a lot from the excellent work by BastiaanOlij. Further, I would like give a special thanks to thank bojidar-bg and Karoffel who helped me tremendously with getting started at the Godot dev sprint spring 2019.
