<h1>Accurate subpixel edge location:</h1>


This project is a C ++ implementation of the method developed in the article ["Accurate subpixel edge location based on partial area effect"](https://www.sciencedirect.com/science/article/abs/pii/S0262885612001850) written by Agustín Trujillo et al. His implementation in Matlab can be downloaded from [MathWroks](https://es.mathworks.com/matlabcentral/fileexchange/48908-accurate-subpixel-edge-location). The slides showing how this method works can be downloaded from MathWorks or from this repository.

This work is part of my Final Degree Project that we have developed from the University of Las Palmas de Gran Canaria in the academic year 2019/20 to obtain the degree of Computer Engineering in the specialty of Computing.

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

The first step in the algorithm is to transform the image into black and white. With the black and white image, the edges are detected at the pixel level, for which the Sobel filters are used. With the Sobel filters we obtain the partial derivatives, and with them the matrix of the gradient vectors of all the pixels of the image. Finally, in this first step, the gradient modulus of each pixel is used to detect all pixels that have a value greater than a certain threshold and that are also maximums between their nearby pixels.

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

To complete this first approximation we need to determine what hues we will take as A and B. To estimate these hues, we take the average of the three pixels of each corner opposite the edge that throughs the window. Since we are assuming that the edge slope is between 0 and 1, we will have to take the upper left and lower right corners of the window, this is:

![\displaystyle A = {1 \over 3} (F_{i,j+2} + F_{i+1,j+2} + F_{i+1,j+1})](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20A%20%3D%20%7B1%20%5Cover%203%7D%20(F_%7Bi%2Cj%2B2%7D%20%2B%20F_%7Bi%2B1%2Cj%2B2%7D%20%2B%20F_%7Bi%2B1%2Cj%2B1%7D))

![\displaystyle B = {1 \over 3} (F_{i-1,j-1} + F_{i-1,j-2} + F_{i,j-2})](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20B%20%3D%20%7B1%20%5Cover%203%7D%20(F_%7Bi-1%2Cj-1%7D%20%2B%20F_%7Bi-1%2Cj-2%7D%20%2B%20F_%7Bi%2Cj-2%7D))

With the data of A, B and the line we can express the normal and establish its sign and its modulus as the difference of the hues A and B:

![\displaystyle N = {{A-B} \over \sqrt{1+b^2}} \[b, -1\]](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20N%20%3D%20%7B%7BA-B%7D%20%5Cover%20%5Csqrt%7B1%2Bb%5E2%7D%7D%20%5Bb%2C%20-1%5D)

<hr />
<h3>Quadratic approximation:</h3>

<img src="./Method_Images/Ventana_3x5_Cuadrática.JPG" alt="Ventana 3x5 Cuadrática" align="right" width="400" height="400" />

As in the case of the approximation by means of a straight line, the approximation by means of a parabola follows the same mechanics, only this time there is one more variable. Since the curve through the window is a parabola, the integrals of each strip will be calculated adjusting to the curve ![y = a + bx + cx^2](https://render.githubusercontent.com/render/math?math=y%20%3D%20a%20%2B%20bx%20%2B%20cx%5E2), and adding its center to the independent term, we obtain:

![\displaystyle L = \int_{-3h/2}^{-h/2} (a + bx + cx^2 + {5 \over 2} h)\, dx = ah - bh^2 + {13 \over 12} ch^3 + {5 \over 2} h^2](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20L%20%3D%20%5Cint_%7B-3h%2F2%7D%5E%7B-h%2F2%7D%20(a%20%2B%20bx%20%2B%20cx%5E2%20%2B%20%7B5%20%5Cover%202%7D%20h)%5C%2C%20dx%20%3D%20ah%20-%20bh%5E2%20%2B%20%7B13%20%5Cover%2012%7D%20ch%5E3%20%2B%20%7B5%20%5Cover%202%7D%20h%5E2)

![\displaystyle M = \int_{-h/2}^{h/2} (a + bx + cx^2 + {5 \over 2} h)\, dx = ah + {1 \over 12} ch^3 + {5 \over 2} h^2](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20M%20%3D%20%5Cint_%7B-h%2F2%7D%5E%7Bh%2F2%7D%20(a%20%2B%20bx%20%2B%20cx%5E2%20%2B%20%7B5%20%5Cover%202%7D%20h)%5C%2C%20dx%20%3D%20ah%20%2B%20%7B1%20%5Cover%2012%7D%20ch%5E3%20%2B%20%7B5%20%5Cover%202%7D%20h%5E2)

![\displaystyle R = \int_{h/2}^{3h/2} (a + bx + cx^2 + {5 \over 2} h)\, dx = ah + bh^2 + {13 \over 12} ch^3 + {5 \over 2} h^2](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20R%20%3D%20%5Cint_%7Bh%2F2%7D%5E%7B3h%2F2%7D%20(a%20%2B%20bx%20%2B%20cx%5E2%20%2B%20%7B5%20%5Cover%202%7D%20h)%5C%2C%20dx%20%3D%20ah%20%2B%20bh%5E2%20%2B%20%7B13%20%5Cover%2012%7D%20ch%5E3%20%2B%20%7B5%20%5Cover%202%7D%20h%5E2)

Solving the system of equations we get the result:

![\displaystyle c = {{S_L + S_R - 2S_M} \over {2(A-B)}}](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20c%20%3D%20%7B%7BS_L%20%2B%20S_R%20-%202S_M%7D%20%5Cover%20%7B2(A-B)%7D%7D)

![\displaystyle b = {{S_R - S_L} \over {2(A-B)}}](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20b%20%3D%20%7B%7BS_R%20-%20S_L%7D%20%5Cover%20%7B2(A-B)%7D%7D)

![\displaystyle a = {{2S_M-5(A+B)} \over {2(A-B)}} - {1 \over 12} c](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20a%20%3D%20%7B%7B2S_M-5(A%2BB)%7D%20%5Cover%20%7B2(A-B)%7D%7D%20-%20%7B1%20%5Cover%2012%7D%20c)

From which we can estimate the curvature at x = 0 as:

![\displaystyle K = {{2c} \over {(1+b^2)^{3/2}}}](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20K%20%3D%20%7B%7B2c%7D%20%5Cover%20%7B(1%2Bb%5E2)%5E%7B3%2F2%7D%7D%7D)

<hr />
<h3>Generalization to any slope value:</h3>

The method as we have explained so far is based on the assumption that the slope of the curve is between 0 and 1. So we must generalize the method so that it works in all possible conditions. To generalize the method we can distinguish two limit situations. We can first suppose the cases where the edge slopes are between -1 and 1, so the resulting curve can be detected using a 3x5 vertical window. The second case, in which the edge slopes are greater than 1 in absolute value, we can use the same method but this time using 5x3 horizontal windows.

Within these two cases, we can differentiate, in turn, two cases in which the slopes are between 0 and 1 or between -1 and 0. The difference between these two cases apears when it attempts to calculating the hues of the corners of the windows. To solve this, we simply make use of a variable (m) that adds or subtracts the unit so that the ends are permuted to the right or to the left.

![\displaystyle m = \left\{ \begin{array}{rcl} 	1 & \mbox{si} & f_x(i,j)f_y(i,j) \gt 0\\ 	  &           &       \\ 	-1 & \mbox{si} & f_x(i,j)f_y(i,j) \leq 0\\ 	\end{array} \right.](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20m%20%3D%20%5Cleft%5C%7B%20%5Cbegin%7Barray%7D%7Brcl%7D%20%091%20%26%20%5Cmbox%7Bsi%7D%20%26%20f_x(i%2Cj)f_y(i%2Cj)%20%5Cgt%200%5C%5C%20%09%20%20%26%20%20%20%20%20%20%20%20%20%20%20%26%20%20%20%20%20%20%20%5C%5C%20%09-1%20%26%20%5Cmbox%7Bsi%7D%20%26%20f_x(i%2Cj)f_y(i%2Cj)%20%5Cleq%200%5C%5C%20%09%5Cend%7Barray%7D%20%5Cright.)

So now the calculations of the limit hues will be:

![\displaystyle A = {1 \over 3} (F_{i,j+2} + F_{i-m,j+2} + F_{i-m,j+1})](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20A%20%3D%20%7B1%20%5Cover%203%7D%20(F_%7Bi%2Cj%2B2%7D%20%2B%20F_%7Bi-m%2Cj%2B2%7D%20%2B%20F_%7Bi-m%2Cj%2B1%7D))

![\displaystyle B = {1 \over 3} (F_{i+m,j-1} + F_{i+m,j-2} + F_{i,j-2})](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20B%20%3D%20%7B1%20%5Cover%203%7D%20(F_%7Bi%2Bm%2Cj-1%7D%20%2B%20F_%7Bi%2Bm%2Cj-2%7D%20%2B%20F_%7Bi%2Cj-2%7D))

We can also distinguish situations where the curve bends toward higher or lower hue values. For example, suppose the case of a circumference in which the interior hue is greater than the exterior hue, in this case the curvature must always be negative, so a generalization like the previous one is necessary, but this time for the curvature:

![\displaystyle K = {{2cn} \over {(1+b^2)^{3/2}}}](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20K%20%3D%20%7B%7B2cn%7D%20%5Cover%20%7B(1%2Bb%5E2)%5E%7B3%2F2%7D%7D%7D)

Being n:

![\displaystyle n = \left\{ \begin{array}{rcl} 	1 & \mbox{si} & f_y(i,j) \gt 0\\ 	  &           &       \\ 	-1 & \mbox{si} & f_y(i,j) \leq 0\\ 	\end{array} \right.](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20n%20%3D%20%5Cleft%5C%7B%20%5Cbegin%7Barray%7D%7Brcl%7D%20%091%20%26%20%5Cmbox%7Bsi%7D%20%26%20f_y(i%2Cj)%20%5Cgt%200%5C%5C%20%09%20%20%26%20%20%20%20%20%20%20%20%20%20%20%26%20%20%20%20%20%20%20%5C%5C%20%09-1%20%26%20%5Cmbox%7Bsi%7D%20%26%20f_y(i%2Cj)%20%5Cleq%200%5C%5C%20%09%5Cend%7Barray%7D%20%5Cright.)

In the case that the edges are purely vertical, that is, in those cases in which the slope is greater in absolute value than 1, no change in the algorithm will be necessary, only it will now adjust to the expression ![x = a + by + cy^2](https://render.githubusercontent.com/render/math?math=x%20%3D%20a%20%2B%20by%20%2B%20cy%5E2), and the horizontal distance to the center will be this time at y = 0 equal to (a). The normal vector will not vary, and the expression for the corner calculations will be:

![\displaystyle A = {1 \over 3} (F_{i+2,j} + F_{i+2,j-m} + F_{i+1,j-m})](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20A%20%3D%20%7B1%20%5Cover%203%7D%20(F_%7Bi%2B2%2Cj%7D%20%2B%20F_%7Bi%2B2%2Cj-m%7D%20%2B%20F_%7Bi%2B1%2Cj-m%7D))

![\displaystyle B = {1 \over 3} (F_{i-1,j+m} + F_{i-2,j+m} + F_{i-2,j})](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20B%20%3D%20%7B1%20%5Cover%203%7D%20(F_%7Bi-1%2Cj%2Bm%7D%20%2B%20F_%7Bi-2%2Cj%2Bm%7D%20%2B%20F_%7Bi-2%2Cj%7D))

<hr />
<h3>Determination of edge pixels:</h3>



















