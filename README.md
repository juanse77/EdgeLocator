<h1>Accurate subpixel edge location:</h1>

This project is a C++ implementation of the method developed in the article ["Accurate subpixel edge location based on partial area effect"](https://www.sciencedirect.com/science/article/abs/pii/S0262885612001850) written by Agustín Trujillo et al. His implementation in Matlab can be downloaded from [MathWroks](https://es.mathworks.com/matlabcentral/fileexchange/48908-accurate-subpixel-edge-location). The slides showing how this method works can be downloaded from MathWorks or from this repository.

This work is part of my Final Degree Project that we have developed from the University of Las Palmas de Gran Canaria in the academic year 2019/20 to obtain the degree of Computer Engineering in the specialty of Computing.

<hr />
<h3>Authors:</h3>

- Agustín Rafael Trujillo Pino - <agustin.trujillo@ulpgc.es> - Tutor
- Juan Sebastián Ramírez Artiles - <juan.ramirez107@alu.ulpgc.es>

<hr />
<h3>Installation of the project in Visual Studio:</h3>

For the implementation of the method we have used the OpenCV library in the version 4.3.0 x64 vc15, and Microsoft Visual C ++ 2019 software in its Community version. To clarify the details of the installation you can consult the document [VS Instalation.pdf](https://github.com/juanse77/EdgeLocator/blob/master/Instalation_in_VS.pdf). However, if you want to test the operation of the application, you can run the downloadable precompiled program from this same repository -[EdgeLocator_1.1](EdgeLocator_1.1.zip)-. The details of its functioning are described below in the point *Application interface*.

<hr />
<h2>Description of the Method:</h2>

In this document we will briefly explain what the method consists of and how it has been implemented in C++.

This method is a major improvement over traditionally used classic edge detection methods such as Sobel or Canny filters. In this method it is possible to detect not only the edges at the pixel level but it is also possible to make a fairly accurate estimate of the tracing of the edges at the sub-pixel level. This method allows you to calculate the position data of the trace within the pixel, as well as its curvature and its normal.

Test image             |  Angio capture
:-------------------------:|:-------------------------:
![](./Captures/Test/FloatingSmoothed_8_11.JPG)  |  ![](./Captures/Real/angio2_2.JPG)

The first step in the algorithm is to transform the image into black and white. With the black and white image, the edges are detected at the pixel level, for which the Sobel filters are used. With the Sobel filters we obtain the partial derivatives, and with them the matrix of the gradient vectors of all the pixels of the image. Finally, in this first step, the gradient modulus of each pixel is used to detect all pixels that have a value greater than a certain threshold and that are also maximums between their nearby pixels.

To detect the traces at sub-pixel level, a system of windows has been devised that covers the proximal areas of each edge pixel, so that, taking the color intensity values ​​of the window pixels at the ends furthest from the edge portion, we can perform the calculations that will finally allow us to draw the lines through each edge pixel.

More information in:

- [Theoretical development of the method](./Method.md).
- [Implementation details](./Implementation.md).

<hr />
<h3>Application interface:</h3>

The application interface is the console and uses the native OpenCV window. To test the application you can download the sources and compile them or use the binary file [EdgeLocator_1.1](./EdgeLocator_1.1.zip) contained in this repository. When you unzip it you will see that there is a folder structure. In the images folder you must place the image files you want to process. The jsonData and accuracyResults folders are for saving execution results. In the jsonData folder, the parameters of the edges calculated in the execution will be saved in json format, as long as the -s option has been activated in the command. The accuracyResults folder will be used to store the statistics of the test execution.

The command accepts several parameters that alter its operation. The command format is:

*EdgeLocator.exe -f (fileName | \"test\" [-e (10-60) -i (8-58)]) [-o (1-2)] [-t (10-255)] [-m (0-3)] [-n] [-s]*

An example could be:

*EdgeLocator.exe -f angio2.png -t 20*

Another example for the test image would be:

*EdgeLocator.exe -f test -e 11 -i 8 -t 10 -n*

Where the -f option can be the name of the input file that must exist in the images folder, or the word "test" that activates the test image and that supports the -e options to set the outer radius of the ring, and - i to set the inner radius of the ring. The -e and -i options have no effect when processing an actual image. The -t option corresponds to the gradient threshold. The -o option corresponds to the fit order: 1 to fit lines, and 2 to fit parabolas. The -m option allows you to select the version of the method to use: 0 floating windows with smoothing; 1 floating windows without smoothing; 2 static windows with smoothing; and 3 static windows without smoothing. The -n option activates the display of normal vectors. Finally, the -s option allows you to dump all the calculated edge parameters in the method to a json file. The file will be stored in the jsonData folder and its name will be the same as that of the input image but its extension will now be .json.

The default values ​​for optional parameters are:
- m: 0
- o: 2
- t: 20
- i: 20
- e: 25

The running program allows interaction with a set of keyboard controls. The arrow keys are used to move around the image, while the (u) key increases the zoom, the (d) key reduces the zoom, and the key (q) aborts the program.

This implementation in C++ of the method has been conceived with the intention of being a tool that facilitates the implementation in any other language, be it Java, Python, or some .Net technology. We hope you liked the method and we encourage you to download the source code and experiment with it.

Thank you for reading the guide notes and we are at your disposal through our email addresses.

<hr />
<h3>Resources used:</h3>

- [Visual Studio 2019 Community](https://visualstudio.microsoft.com/es/vs/community/).
- [OpenCV 4.3.0](https://opencv.org/opencv-4-3-0/).
- [JSON nlomann library](https://github.com/nlohmann/json).
- [GetOpt for Windows](https://github.com/iotivity/iotivity/tree/master/resource/c_common/windows/src).
