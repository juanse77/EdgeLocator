<h2>Implementation details:</h2>

The different approaches to the method have been coded into separate classes. They all inherit their common behavior from the abstract class *AbstractEdgeLocator*. The class that includes the first approximation is *BasicEdgeLocator*. This first implementation is a transcription of the method using static 3x5 and 5x3 windows and does not make use of edge smoothing. The second approach is implemented in the class *BasicEdgeLocatorSmoothed*, as its name suggests, in this class the edges are smoothed before calculating the rest of the parameters. In the same way as in the previous method, it uses static windows but 3x9 or 9x3 in size, due to the edge expansion effect described above in the explanation of the method. The third approach is developed in the classs *EdgeLocatorFloatingWindowsSmoothed*. Image smoothing and floating windows are already used in this class. In addition to these three classes, class *EdgeLocatorFloatingWindows* has been developed, that it works with floating windows but does not use any kind of smoothing prior to calculations.

<div align="center">
	<img src="./ClassDiagram/EdgeLocator.svg" alt="Class diagram" />
</div>

The edge detection process follows the same scheme in all cases, that is: first the image is converted to black and white, the second step would be the smoothing of the image, if the chosen option does not use it, it would go to the next step. With the image already in black and white, the partial derivatives would be calculated and with them the gradient, later the horizontal edge detection method is called, followed by the vertical edge detection method, and finally the method to visualize the calculated edges in the process is called. In the edge detection steps, the results will be saved in a dynamic structure *Vector* which stores objects *Edge*.

Objects *Edge* are the minimum unit of information of the result of the execution. The structure of object *Edge* is as follows:

- position: Stores the position of the edge pixel within the image byte array.
- x: Represents the horizontal position from left to right in Cartesian coordinates of the edge pixel.
- y: Represents the vertical position from top to bottom in Cartesian coordinates of the edge pixel.
- nx: Represents the x component of the normalized normal vector.
- ny: Represents the y component of the normalized normal vector.
- curv: Stores the curvature of the edge pixel.
- i0: Stores the lowest color intensity of the edge.
- i1: Stores the highest color intensity of the edge.

<hr />
<h3>Considerations for converting Matlab code to C++:</h3>

In the process of translating Matlab code to C++ code, it must be taken into account that arrays in Matlab are stored in base 1, while in C ++ they are stored in base 0. Another important consideration to keep in mind is that the Matlab arrays are organized in Column-Major, while in C++ they are organized internally in Row-Major. These remarkable differences between Matlab and C++ require a meticulous effort in handling the indexes of the matrices.