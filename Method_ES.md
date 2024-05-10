[English version](./Method.md)
<hr />
<h2>Desarrollo teórico de método:</h2>

<p>Podemos dividir la explicación del método en tres niveles de complejidad: una primera aproximación básica en la que se fundamenta la técnica que posteriormente se irá perfeccionando, en la cual se hace uso de ventanas de tamaño fijo; una segunda aproximación en la cual se sigue haciendo uso de ventanas de tamaño fijo pero esta vez se le ha aplicado primero un filtro gaussiano a la imagen; y una tercera aproximación en la que se utilizan ventanas flotantes de tamaño variable para la detección de bordes muy próximos entre sí.</p>

<hr />
<h3>Primera aproximación:</h3>

En esta primera aproximación se utilizarán ventanas de tamaño 3x5. Antes de describir cómo funcionaría el método con este tipo de ventana podríamos establecer la fórmula principal de la que derivará por completo el método.

Si suponemos que un píxel resulta atravesado por un borde, podemos estimar la tonalidad final de los lados a partir de la tonalidad del píxel. Si llamamos F al tono de cada píxel, y A y B a los tonos de los lados opuestos por el borde, podemos establecer matemáticamente la relación que existe entre ellos:

$F(i,j) = B + {{A-B} \over {h^2}} P(i,j)$ 

Donde la h representa el tamaño del lado del píxel, que en adelante tomará el valor de 1, y P el área que cubre el tono A dentro del píxel. Podemos comprobar fácilmente que cuando P es igual a 1 el tono del píxel tendrá el valor de A, mientras que cuando el valor del área P es igual 0 el tono será B.

Gracias a esta sencilla fórmula podemos construir el método con ventanas estáticas. Como dijimos anteriormente en esta primera aproximación utilizaremos un tamaño de ventana de 3x5. Para ilustrar esta aproximación vamos a suponer que el borde atraviesa la ventana de izquierda a derecha y que su pendiente se encuentra entre 0 y 1. Con esta suposición nos aseguramos de que el borde atraviesa por completo la ventana de lado a lado. Si suponemos, además, que el borde es recto podemos calcular los parámetros de esta recta utilizando la variación de las áreas verticales colindantes de la ventana para plantear un sistema de ecuaciones.

<div align="center">
	<img src="./Method_Images/Ventana_3x5.JPG" alt="Ventana 3x5" />
</div>

Siguiendo el esquema de la primera fórmula, podemos establecer las fórmulas de las tonalidades acumuladas de cada franja vertical en relación a los dos tonos del borde y al área que abarca la tonalidad A:

$$S_L = \sum_{n = {j-2}}^{j+2} F_{i-1,n} = 5B + {{A-B} \over {h^2}} L   $$

$$S_M = \sum_{n = {j-2}}^{j+2} F_{i,n} = 5B + {{A-B} \over {h^2}} M     $$

$$S_R = \sum_{n = {j-2}}^{j+2} F_{i+1,n} = 5B + {{A-B} \over {h^2}} R$$

Ahora podemos expresar el área de cada franja como la integral de la recta que atraviesa la franja. Como la recta presenta dos incógnitas (a) y (b) ya podemos plantear un sistema de ecuaciones con las tres anteriores ecuaciones y con las áreas de A expresadas como integrales:

$$L = \int_{-3h/2}^{-h/2} (a + bx + {5 \over 2} h)\, dx = ah - bh^2 + {5 \over 2} h^2$$

$$M = \int_{-h/2}^{h/2} (a + bx + {5 \over 2} h)\, dx = ah + {5 \over 2} h^2$$

$$R = \int_{h/2}^{3h/2} (a + bx + {5 \over 2} h)\, dx = ah + bh^2 + {5 \over 2} h^2$$

Resolviendo el sistema de ecuaciones llegamos a las expresiones de (a) y (b) en función de los tonos A y B y de los valores conocidos de las sumas de los tonos de las tres franjas:

$$a = {{2S_M - 5(A+B)} \over {2(A-B)}} h$$

$$b = {{S_R-S_L} \over {2(A-B)}}$$

Siendo (a) la distancia vertical en la posición central de la ventana.

Para completar esta primera aproximación nos falta determinar qué tonos tomaremos como A y B. Para estimar estos tonos tomamos la media de los tres píxeles de cada esquina opuesta al borde que atraviesa la ventana. Como estamos suponiendo que la pendiente del borde se encuentra entre 0 y 1, deberemos tomar las esquinas superior izquierda e inferior derecha de la ventana, esto es:

$$A = {1 \over 3} (F_{i,j+2} + F_{i+1,j+2} + F_{i+1,j+1})$$

$$B = {1 \over 3} (F_{i-1,j-1} + F_{i-1,j-2} + F_{i,j-2})$$

Con los datos de A, B y de la recta podemos expresar la normal y establecer su signo y su módulo como la diferencia de las tonalidades A y B:

$$N = {{A-B} \over \sqrt{1+b^2}} [b, -1]$$

<hr />
<h3>Aproximación cuadrática:</h3>

<img src="./Method_Images/Ventana_3x5_Cuadrática.JPG" alt="Ventana 3x5 Cuadrática" align="right" width="400" height="400" />

Como en el caso de la aproximación mediante una recta, la aproximación mediante una parábola sigue la misma mecánica, solo que esta vez hay una variable más. Al ser una parábola la curva que atraviesa la ventana, las integrales de cada franja se calcularán ajustándose a la curva $y = a + bx + cx^2$, y sumando su centro al término independiente, obtenemos:

$$L = \int_{-3h/2}^{-h/2} (a + bx + cx^2 + {5 \over 2} h)\, dx = ah - bh^2 + {13 \over 12} ch^3 + {5 \over 2} h^2$$

$$M = \int_{-h/2}^{h/2} (a + bx + cx^2 + {5 \over 2} h)\, dx = ah + {1 \over 12} ch^3 + {5 \over 2} h^2$$

$$R = \int_{h/2}^{3h/2} (a + bx + cx^2 + {5 \over 2} h)\, dx = ah + bh^2 + {13 \over 12} ch^3 + {5 \over 2} h^2$$

Resolviendo el sistema de ecuaciones llegamos al resultado:

$$c = {{S_L + S_R - 2S_M} \over {2(A-B)}}$$

$$b = {{S_R - S_L} \over {2(A-B)}}$$

$$a = {{2S_M-5(A+B)} \over {2(A-B)}} - {1 \over 12} c$$

De lo cual podemos estimar la curvatura en x = 0 como:

$$K = {{2c} \over {(1+b^2)^{3/2}}}$$

<hr />
<h3>Generalización a cualquier valor de pendiente:</h3>

El método tal y como lo hemos explicado hasta ahora se basa en la suposición de que la pendiente de la curva se encuentre entre 0 y 1. Por lo que debemos generalizar el método para que funcione en todas las condiciones posibles. Para generalizar el método podemos distinguir dos situaciones límite. Primero podemos suponer los casos en los que las pendientes del borde estén entre -1 y 1, por lo que la curva resultante se puede detectar usando una ventana vertical de 3x5. El segundo caso, en el que las pendientes de los bordes sean superiores a 1 en valor absoluto, podemos utilizar el mismo método, pero usando esta vez ventanas horizontales de 5x3.

Dentro de estos dos casos, podemos diferenciar, a su vez, dos casos en los que las pendientes se encuentren entre 0 y 1 o entre -1 y 0. La diferencia entre estos dos casos se presenta cuando se intenta calcular los tonos de las esquinas de las ventanas. Para solucionar esto, simplemente hacemos uso de una variable (m) que sume o reste la unidad de modo que se permute los extremos hacia la derecha o hacia la izquierda.

$$
m = \left\{
\begin{array}{rcl}
    1 & \text{si} & f_x(i,j)f_y(i,j) > 0\\
    -1 & \text{si} & f_x(i,j)f_y(i,j) \leq 0
\end{array}
\right.
$$

Por lo que ahora los cálculos de los tonos límite serán:

$$A = {1 \over 3} (F_{i,j+2} + F_{i-m,j+2} + F_{i-m,j+1})$$

$$B = {1 \over 3} (F_{i+m,j-1} + F_{i+m,j-2} + F_{i,j-2})$$

También podemos distinguir las situaciones en las que la curva se doble hacia valores de tonos más altos o más bajos. Por ejemplo, supongamos el caso de una circunferencia en la que el tono interior es mayor al tono exterior, en este caso la curvatura deberá ser siempre negativa, por lo que se hace necesaria una generalización como la anterior pero esta vez para la curvatura:

$$K = {{2cn} \over {(1+b^2)^{3/2}}}$$

Siendo n:

$$
  n = \left\{
  \begin{array}{rcl}
    1 & \text{si} & f_y(i,j) \gt 0\\
   	&           &       \\
    -1 & \text{si} & f_y(i,j) \leq 0\\
 	\end{array}
  \right.
$$

En el caso de que los bordes sean netamente verticales, es decir, en aquellos casos en los que la pendiente sea mayor en valor absoluto a 1, no hará falta ningún cambio en el algoritmo, solo que ahora se ajustará a la expresión $x = a + by + cy^2$, y la distancia horizontal al centro será esta vez en y = 0 igual a (a). El vector normal no variará, y la expresión para los cálculos de las esquinas será:

$$A = {1 \over 3} (F_{i+2,j} + F_{i+2,j-m} + F_{i+1,j-m})$$

$$B = {1 \over 3} (F_{i-1,j+m} + F_{i-2,j+m} + F_{i-2,j})$$

<hr />
<h3>Determinación de los píxeles bordes:</h3>

Para que este método funcione es necesario determinar qué píxeles se marcarán como píxeles bordes. Como explicamos en la introducción, tomamos como píxeles candidatos a todos los que el módulo de su gradiente supere un cierto umbral. Además, la anterior condición no es suficiente ya que para que el píxel sea considerado como borde debe ser también un píxel con un valor máximo entre los de su vecindad. Para determinar si un píxel tiene un valor máximo en su vecindad, debemos considerar si el píxel constituye un borde vertical u horizontal. Si el píxel es vertical su derivada parcial $f_x$ será mayor a $f_y$, en el caso horizontal será al contrario. Por lo tanto será condición suficiente que se cumplan las siguientes desigualdades:

En el caso vertical:

$$
\left\{
\begin{array}{l}
    |f_x(i,j)| > |f_y(i,j)|\\
    \quad\\
    |f_x(i-1,j)| \leq |f_x(i,j)| \geq |f_x(i+1,j)|
\end{array}
\right.
$$

Y en el caso horizontal:

$$
\left\{
\begin{array}{l}
    |f_y(i,j)| \gt |f_x(i,j)| \\
    \quad\\
    |f_y(i,j-1)| \leq |f_y(i,j)| \geq |f_y(i,j+1)| \\
\end{array}
\right.
$$

<hr />
<h3>Segunda aproximación (Imágenes con ruido):</h3>

En los casos en que la imagen que se quiera procesar presente ruido - generalmente toda imagen real lo presenta - tenemos la posibilidad de suavizar los bordes aplicándole primero un filtro gaussiano. El método en este caso no necesita ninguna modificación conceptual, no obstante, sí que produce una alteración en la formulación del sistema de ecuaciones y por lo tanto también en el resultado del mismo.

La matriz gaussiana que utilizaremos será de 3x3 y estará conformada del siguiente modo:

$$
K = \left(
  \begin{array}{lcr}
    a_{11} & a_{01} & a_{11} \\
    a_{01} & a_{00} & a_{01} \\
    a_{11} & a_{01} & a_{11}
  \end{array} \right)
$$

La matriz así mismo debe cumplir que $a_{00} \gt a_{01} \gt a_{11}$, y además $a_{00} + 4a_{01} + 4a_{11} = 1$

Como resultado del proceso de suavizado se produce una alteración el área que abarca los píxeles con valores intermedios entre A y B. Este área es ahora mayor, tal como se puede observar en la siguiente imagen:
 
<div align="center">
	<img src="./Method_Images/Img_Suavizada.JPG" alt="Imagen suavizada" />
</div>

Si llamamos G a la imagen suavizada, y si suponemos de nuevo el caso ideal en el que la pendiente del borde se encuentra entre -1 y 1, y considerando de nuevo la variable (m) del mismo modo que en caso anterior pero esta vez evaluado sobre las derivadas parciales de G; podemos expresar las sumas de las columnas de la ventana como:

$$S_L = \sum_{k = {-3-m}}^{3-m} G_{i-1,j+k}$$

$$S_M = \sum_{k = {-3}}^{3} G_{i,j+k}$$

$$S_R = \sum_{k = {-3+m}}^{3+m} G_{i+1,j+k}$$

Como se puede observar, ahora el tamaño de las franjas verticales es de 7 y se encuentran desplazadas de modo que se ajusten de un modo óptimo al trazo estimado del borde. Esta nueva configuración de las franjas de las ventanas y los valores modificados de los píxeles de G, da lugar a la solución siguiente del sistema:

$$c = {{S_L + S_R - 2S_M} \over {2(A-B)}}$$

$$b = m + {{S_R - S_L} \over {2(A-B)}}$$

$$a = {{2S_M-7(A+B)} \over {2(A-B)}} - {{1+24a_{01}+48a_{11}}  \over 12} c$$

*Hemos omitido los detalles de las operaciones que llevan a esta solución. Si desea profundizar en los cálculos consulte el artículo original.*

Al ser las ventanas distintas también se verá afectada la estimación de los tono A y B. Los tonos para el caso horizontal son ahora:

$$A = {1 \over 3} (F_{i,j+4} + F_{i-m,j+4} + F_{i-m,j+3})$$

$$B = {1 \over 3} (F_{i+m,j-3} + F_{i+m,j-4} + F_{i,j-4})$$

Para el caso de bordes verticales se puede deducir fácilmente de lo explicado hasta ahora, por lo que lo dejamos como ejercicio para el lector.

<hr />
<h3>Detección de bordes próximos mediante ventanas flotantes:</h3>

En los casos en los que la imagen contenga bordes muy cercanos entre sí, de modo que más de un borde atraviese alguna ventana, se hace necesario adaptar el método, para lo cual se ideó que las ventanas pudieran modificar su tamaño dinámicamente.

Para implementar esta solución se hace uso de tres pares de nuevas variables: (l1, l2), (m1, m2), y (r1, r2). Cada par de variables establecerá los límites de su franja, por lo que las sumas de las franjas se verán alteradas, quedando del siguiente modo:

$$S_L = \sum_{k = {l_1}}^{l_2} G_{i-1,j+k}$$

$$S_M = \sum_{k = {m_1}}^{m_2} G_{i,j+k}$$

$$S_R = \sum_{k = {r_1}}^{r_2} G_{i+1,j+k}$$

Al alterar el cálculo de las sumas acumuladas de los tonos de las franjas, se ve alterado el sistema de ecuaciones, operando una vez más se llega a la nueva solución:

$$c = {{S_L + S_R - 2S_M} \over {2(A - B)}} + {{A(2m_2 - l_2 - r_2)-B(2m_1 - l_1 - r_1)} \over {2(A - B)}}$$

$$b = {{S_R - S_L} \over {2(A-B)}} + {{A(l_2 - r_2) - B(l_1 - r_1)} \over {2(A - B)}}            $$

$$a = {{2S_M - A(1 + 2m_2) - B(1 - 2m_1)} \over {2(A - B)}} - {{1 + 24a_01 + 48a_11}  \over 12} c$$

*Hemos vuelto a omitir los calculos intermedios que nos llevan a la solución. El detalle de estos cálculos se puede consultar en el artículo original.*

Las estimaciones de los tonos A y B se realizan de un modo ligeramente diferente. Esta vez no nos sirve la generalización usando la variable (m) por lo que tenemos que distinguir los casos en los que la pendiente del borde es positiva o negativa.
 
Para el caso de bordes horizontales y de pendiente positiva se calcularían como:

$$A = {1 \over 2} (G_{i,j+m_2} + G_{i+1,j+r_2})$$

$$B = {1 \over 2} (G_{i-1,j+l_1} + G_{i,j+m_1})$$

Para bordes horizontales con pendiente negativa:

$$A = {1 \over 2} (G_{i-1,j+l_2} + G_{i,j+m_2})$$

$$B = {1 \over 2} (G_{i,j+m_1} + G_{i+1,j+r_1})$$

Los casos de bordes verticales los dejamos de nuevo como ejercicio para el lector.
