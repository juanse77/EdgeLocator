<h1>Método de detección de bordes a nivel subpixel:</h1>

Este proyecto es una implantación en C++ del método desarrollado en el artículo ["Accurate subpixel edge location based on partial area effect"](https://www.sciencedirect.com/science/article/abs/pii/S0262885612001850) escrito por Agustín Trujillo et al. Su implementación en Matlab se puede descargar de [MathWroks](https://es.mathworks.com/matlabcentral/fileexchange/48908-accurate-subpixel-edge-location). Las diapositivas que muestran el funcionamiento de este método se pueden descargar de MathWorks o de este mismo repositorio. Este proyecto forma parte del mi trabajo de fin de grado (TFG) que hemos desarrollado desde la Universidad de Las Palmas de Gran Canaria en el curso 2019/20 para la obtención del título de Ingeniería Informática en la especialidad de Computación.

<h3>Autores</h3>

- Agustín Rafael Trujillo Pino - <agustin.trujillo@ulpgc.es>
- Juan Sebastián Ramírez Artiles - <juan.ramirez107@alu.ulpgc.es>

<h3>Instalación del proyecto en Visual Studio</h3>

Para la implementación del método hemos hecho uso de la librería OpenCV en la versión 4.3.0 x64 vc15, y del software Microsoft Visual C++ 2019 en su versión Community. Para aclarar los detalles de la instalación puede consultar el documento [Instalación VS.pdf](https://github.com/juanse77/EdgeLocator/blob/master/Instalaci%C3%B3n%20VS.pdf).

<h2>Descripción del método</h2>

<p>En este documento vamos a explicar brevemente en qué consiste el método y cómo se ha implantado en C++.</p>

<p>Este método es una mejora importante de los métodos clásicos de detección de bordes utilizados tradicionalmente, como son los filtros Sobel o los Canny. En este método se consigue detectar no solo los bordes a nivel de pixel sino que se logra hacer una estimación bastante precisa del trazado de los bordes a nivel subpixel. Este método permite calcular los datos de posición del trazo dentro del pixel, así como su curvatura y su normal.</p>

<p>En el primer paso del algoritmo se transforma la imagen a blanco y negro. Con la imagen en blanco y negro se pasa a detectar los bordes a nivel pixel, para lo cual se utilizan los filtros Sobel. Con los filtros Sobel obtenemos las derivadas parciales en los ejes X e Y, y con ello una matriz de los vectores gradientes de todos los pixeles de la imagen. Por último en este primer paso, se utiliza la matriz de vectores gradientes para detectar los pixeles que posean un valor mayor a un cierto umbral y que su módulo sea máximo entre los píxeles de su vecindad.</p>

<p>Para detectar los trazos a nivel subpixel se hace uso de un sistema de ventanas que abarcan las zonas próximas de cada pixel borde, de modo que, tomando los valores de intensidad de color de los pixeles de los extremos más alejados de los bordes, se procede a realizar los cálculos que finalmente permitirán trazar las líneas continuas a través de cada pixel borde.</p>  

<p>Podemos dividir la explicación del método en tres niveles de complejidad: una primera aproximación básica en la que se fundamenta la técnica que posteriormente se irá perfeccionando, en la cual se hace uso de ventanas de tamaño fijo; una segunda aproximación en la cual se sigue haciendo uso de ventanas de tamaño fijo pero esta vez se le ha aplicado primero un filtro gaussiano a la imagen; y una tercera aproximación en la que se utilizan ventanas flotantes de tamaño variable para la detección de los bordes.</p>

<h3>Primera aproximación</h3>

En esta primera paroximación se utilizarán ventanas de tamaño 3x5. Antes de describir cómo funcionaría el método con este tipo de ventana podríamos establecer la fórmula principal de la que derivará por completo el método.

Si suponemos que un pixel resulta atravesado por un borde, podemos estimar la tonalidad final de los lados a partir de la tonalidad del pixel. Si llamamos F al tono de cada pixel, y A y B a los valores de los lados opuestos por el borde, podemos establecer matemáticamente la relación que existe entre ellos:

<div style="text-align:center">

![\displaystyle F(i,j) = B + {{A-B} \over {h^2}} P(i,j)](https://render.githubusercontent.com/render/math?math=%5Cdisplaystyle%20F(i%2Cj)%20%3D%20B%20%2B%20%7B%7BA-B%7D%20%5Cover%20%7Bh%5E2%7D%7D%20P(i%2Cj)) 

</div>
