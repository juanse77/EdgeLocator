<h1>Accurate subpixel edge location:</h1>


This project is a C ++ implementation of the method developed in the article ["Accurate subpixel edge location based on partial area effect"](https://www.sciencedirect.com/science/article/abs/pii/S0262885612001850) written by Agustín Trujillo et al. His implementation in Matlab can be downloaded from [MathWroks](https://es.mathworks.com/matlabcentral/fileexchange/48908-accurate-subpixel-edge-location). The slides showing how this method works can be downloaded from MathWorks or from this repository.

This project is part of my Final Degree Project (TFG) that we have developed from the University of Las Palmas de Gran Canaria in the 2019/20 academic year to obtain the Computer Engineering degree in the specialty of Computing.

<hr />
<h3>Authors:</h3>

- Agustín Rafael Trujillo Pino - <agustin.trujillo@ulpgc.es> - Tutor
- Juan Sebastián Ramírez Artiles - <juan.ramirez107@alu.ulpgc.es>

<hr />
<h3>Installation of the project in Visual Studio</h3>

For the implementation of the method we have used the OpenCV library in the version 4.3.0 x64 vc15, and Microsoft Visual C ++ 2019 software in its Community version. To clarify the details of the installation you can consult the document [VS Instalation.pdf](https://github.com/juanse77/EdgeLocator/blob/master/Instalaci%C3%B3n%20VS.pdf).

<hr />
<h2>Method description:</h2>

In this document we will briefly explain what the method consists of and how it has been implemented in C++.

This method is a major improvement over traditionally used classic edge detection methods such as Sobel or Canny filters. In this method it is possible to detect not only the edges at the pixel level but it is also possible to make a fairly accurate estimate of the tracing of the edges at the sub-pixel level. This method allows you to calculate the position data of the trace within the pixel, as well as its curvature and its normal.

The first step in the algorithm is to transform the image into black and white. With the black and white image, the edges are detected at the pixel level, for which the Sobel filters are used. With the Sobel filters we obtain the partial derivatives, and with them the matrix of the gradient vectors of all the pixels of the image. Finally, in this first step, the gradient modules of each pixel are used to detect all the pixels that have a value greater than a certain threshold and that are also maximum among the pixels in their proximity.

To detect the traces at sub-pixel level, a system of windows has been devised that covers the proximal areas of each edge pixel, so that, taking the color intensity values ​​of the window pixels at the ends furthest from the edge portion, we can perform the calculations that will finally allow we to draw the solid lines through each edge pixel.

We can divide the method explanation into three levels of complexity: a first basic approach on which the technique is based, which will subsequently be perfected, in which makes use of fixed-size windows; a second approach in which fixed-size windows continue making use, but this time a Gaussian filter has been applied to the image first; and a third approach in which variable size floating windows are used for the very close edges detection.

<hr />
<h3>First approximation:</h3>

In this first approximation 3x5 size windows will be used. Before describing how the method works with this type of window, we can establish the main formula from which the method will be completely derived.

If we assume that a pixel is traversed by an edge, we can estimate the final hue of the sides from the hue of the pixel. If we call F the tone of each pixel, and A and B the tones of the sides opposite the edge, we can mathematically establish the relationship between them:

![\displaystyle F(i,j) = B + {{A-B} \over {h^2}} P(i,j)](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20F(i%2Cj)%20%3D%20B%20%2B%20%7B%7BA-B%7D%20%5Cover%20%7Bh%5E2%7D%7D%20P(i%2Cj))

















