<h1>Accurate subpixel edge location:</h1>


This project is a C ++ implementation of the method developed in the article ["Accurate subpixel edge location based on partial area effect"](https://www.sciencedirect.com/science/article/abs/pii/S0262885612001850) written by Agustín Trujillo et al. His implementation in Matlab can be downloaded from [MathWroks](https://es.mathworks.com/matlabcentral/fileexchange/48908-accurate-subpixel-edge-location). The slides showing how this method works can be downloaded from MathWorks or from this repository.

This project is part of my Final Degree Project that we have developed from the University of Las Palmas de Gran Canaria in the 2019/20 academic year to obtain the Computer Engineering degree in the specialty of Computing.

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

We can divide the explanation of the method into three levels of complexity: a first basic approach on which the technique is based, which will subsequently be perfected, in which fixed-size windows are used; a second approach where fixed-size windows are still used, but this time a Gaussian filter is applied to the image first; and a third approach that uses floating windows of variable size for detection of very close edges.

<hr />
<h3>First approximation:</h3>

In this first approximation 3x5 size windows will be used. Before describing how the method works with this type of window, we can establish the main formula from which the method will be completely derived.

If we assume that a pixel is traversed by an edge, we can estimate the final hue of the sides from the hue of the pixel. If we call F the tone of each pixel, and A and B the tones of the sides opposite the edge, we can mathematically establish the relationship between them:

![\displaystyle F(i,j) = B + {{A-B} \over {h^2}} P(i,j)](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20F(i%2Cj)%20%3D%20B%20%2B%20%7B%7BA-B%7D%20%5Cover%20%7Bh%5E2%7D%7D%20P(i%2Cj))

Where h represents the size of the pixel side, which will henceforth take the value of 1, and P the area that covers tone A within the pixel. We can easily verify that when P is equal to 1, the hue of the pixel will have the value of A, while when the value of the area P is equal to 0, the hue will be B.

Thanks to this simple formula we can build the method with static windows. As we said earlier in this first approximation we will use a window size of 3x5. To illustrate this approximation we are going to assume that the edge crosses the window from left to right and that its slope is between 0 and 1. With this assumption we make sure that the edge completely goes through the window from side to side. If we also assume that the edge is straight, we can calculate the parameters of this line using the variation of the adjoining vertical areas of the window to propose a system of equations.

<div align="center">
	<img src="./Method_Images/Ventana_3x5.JPG" alt="Ventana 3x5" />
</div>

Following the scheme of the first formula, we can establish the formulas of the accumulated hues of each vertical strip in relation to the two hues of the border and to the area covered by the hue A:

![\displaystyle S_L = \sum_{n = {j-2}}^{j+2} F_{i-1,n} = 5B + {{A-B} \over {h^2}} L   ](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20S_L%20%3D%20%5Csum_%7Bn%20%3D%20%7Bj-2%7D%7D%5E%7Bj%2B2%7D%20F_%7Bi-1%2Cn%7D%20%3D%205B%20%2B%20%7B%7BA-B%7D%20%5Cover%20%7Bh%5E2%7D%7D%20L%20%20%20)

![\displaystyle S_M = \sum_{n = {j-2}}^{j+2} F_{i,n} = 5B + {{A-B} \over {h^2}} M     ](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20S_M%20%3D%20%5Csum_%7Bn%20%3D%20%7Bj-2%7D%7D%5E%7Bj%2B2%7D%20F_%7Bi%2Cn%7D%20%3D%205B%20%2B%20%7B%7BA-B%7D%20%5Cover%20%7Bh%5E2%7D%7D%20M%20%20%20%20%20)

![\displaystyle S_R = \sum_{n = {j-2}}^{j+2} F_{i+1,n} = 5B + {{A-B} \over {h^2}} R](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20S_R%20%3D%20%5Csum_%7Bn%20%3D%20%7Bj-2%7D%7D%5E%7Bj%2B2%7D%20F_%7Bi%2B1%2Cn%7D%20%3D%205B%20%2B%20%7B%7BA-B%7D%20%5Cover%20%7Bh%5E2%7D%7D%20R)

Now we can express the area of ​​each strip as the integral of the line that crosses the strip. Since the line presents two unknown quantities (a) and (b) we can already propose a system of equations with the three previous equations and with the areas of A expressed as integrals:

![\displaystyle L = \int_{-3h/2}^{-h/2} (a + bx + {5 \over 2} h)\, dx = ah - bh^2 + {5 \over 2} h^2](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20L%20%3D%20%5Cint_%7B-3h%2F2%7D%5E%7B-h%2F2%7D%20(a%20%2B%20bx%20%2B%20%7B5%20%5Cover%202%7D%20h)%5C%2C%20dx%20%3D%20ah%20-%20bh%5E2%20%2B%20%7B5%20%5Cover%202%7D%20h%5E2)

![\displaystyle M = \int_{-h/2}^{h/2} (a + bx + {5 \over 2} h)\, dx = ah + {5 \over 2} h^2](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20M%20%3D%20%5Cint_%7B-h%2F2%7D%5E%7Bh%2F2%7D%20(a%20%2B%20bx%20%2B%20%7B5%20%5Cover%202%7D%20h)%5C%2C%20dx%20%3D%20ah%20%2B%20%7B5%20%5Cover%202%7D%20h%5E2)

![\displaystyle R = \int_{h/2}^{3h/2} (a + bx + {5 \over 2} h)\, dx = ah + bh^2 + {5 \over 2} h^2](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20R%20%3D%20%5Cint_%7Bh%2F2%7D%5E%7B3h%2F2%7D%20(a%20%2B%20bx%20%2B%20%7B5%20%5Cover%202%7D%20h)%5C%2C%20dx%20%3D%20ah%20%2B%20bh%5E2%20%2B%20%7B5%20%5Cover%202%7D%20h%5E2)

Solving the system of equations we reach the expressions of (a) and (b) as a function of the hues A and B and the known values ​​of the sums of the hues of the three strips:

![\displaystyle a = {{2S_M - 5(A+B)} \over {2(A-B)}} h](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20a%20%3D%20%7B%7B2S_M%20-%205(A%2BB)%7D%20%5Cover%20%7B2(A-B)%7D%7D%20h)

![\displaystyle b = {{S_R-S_L} \over {2(A-B)}}](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20b%20%3D%20%7B%7BS_R-S_L%7D%20%5Cover%20%7B2(A-B)%7D%7D)

Being (a) the vertical distance in the central position of the window.










