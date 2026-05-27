# HamoodViewer
HamoodViewer is a physically based 3D model viewer desktop application for windows. Uses openGL and is written in C++.
![alt text](https://github.com/UnkushB/hamoodViewer/blob/3449a223c60e17d045aaabe3c0ab5d58647b0dcb/readMeImages/default.PNG)

# Features
Physically Based Rendering w/ Image Based Lighting

![alt text](https://github.com/UnkushB/hamoodViewer/blob/3807e4143cdc24cbb55902e1bf374bddb77243f5/readMeImages/metaltest.PNG)

Blended Weighted Oit

![alt text](https://github.com/UnkushB/hamoodViewer/blob/3807e4143cdc24cbb55902e1bf374bddb77243f5/readMeImages/oit.PNG)

Shadow Mapping

![alt text](https://github.com/UnkushB/hamoodViewer/blob/9eb90a62b70df24886012588e6d718c6bd109a7d/readMeImages/shadows.PNG)

# Usage
First clone or download the repo.

Then create a folder in the project root to build the project in.

From there run cmake from the project root. cmake -B ./build -> cmake --build ./build

You can also build from inside your build folder.

Once built, run the executable from the project root.

If your using visual studio you might need to change your build root

and maybe set your configuration type to be release.

One way of doing this is clicking project -> Cmake setting for hamoodViewer

Then setting configuration type to release and build root to "${projectDir}\build"

# External Libraries
- GLFW
- GLM
- Assimp
- GLAD
- stb_image
