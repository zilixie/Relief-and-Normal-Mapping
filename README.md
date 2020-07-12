# Relief-and-Normal-Mapping & Procedural Texture
Project Website: https://zilixie.github.io/Relief-and-Normal-Mapping/
This project was built on my previous 3D scene editor: https://zilixie.github.io/3D-Scene-Editor/

A 3D scene editor implemented in C++ using OpenGL that supports different types texture techinique: perlin noise, procedural bump, texture mapping, normal mapping, parallax mapping. Library used: Eigen, GLFW, SOIL. It also supports the basic functionalities of a 3D object editor: insert, tranlate, colorize, rotate, scale, animate, delete for triangle mesh objects in .OFF file format and it allows users to export scene into .svg files.

## Perlin Noise
What perlin noise does? Generate smooth pseudo noise given seed in 3D. What can it be used for? produce game scenes such as mountains, seas, clouds...</br>
1.Tessellation Shader</br>
2.Sphere Tangent Space</br>

## procedural bump
<img src="https://github.com/zilixie/Relief-and-Normal-Mapping/blob/master/images/project1.gif" width="720" height="450">

## procedural texture and displacement
<img src="https://github.com/zilixie/Relief-and-Normal-Mapping/blob/master/images/project2.gif" width="720" height="450">

## Parallax Mapping
<img src="https://github.com/zilixie/Relief-and-Normal-Mapping/blob/master/images/project6.gif" width="720" height="544">

## How To Use
>O: Start Translation mode.</br>
>P: Start Deletion mode.</br>
>U: Start Animation mode.</br>
>SPACE: Start/Stop the rotation of camera (by default the camera rotates arounf Y axis).</br>
><, >: Change color for selected object.</br>
>/: Change projection (perspective or orthogonal)</br>
>[,]: change the shading method (vertex normal or face normal)</br>
>↑,↓,←,→: Move the camera on trackball up, down, left, right respectively by 2 degree.</br>
>+,-: Zoom in or out by 3%.</br>
>J,M: Rotate the selected Object by 2 degree clockwise or counter-clockwise around the Y axis.</br>
>H,N: Rotate the selected Object by 2 degree clockwise or counter-clockwise around the Z axis.</br>
>G,B: Rotate the selected Object by 2 degree clockwise or counter-clockwise around the X axis.</br>
>K,L: Scale the selected object up or down by 5% respectively.</br>
>W,S: Move the camera on Y axis.</br>
>E,D: Move the camera on X axis.</br>
>R,F: Move the camera on Z axis.</br>
>Z: Start the animation if the bezier curve for an object was constructed. </br>
>X: Take a screenshot and export scene to .svg file. </br>
>Q: Quit the program.</br>
