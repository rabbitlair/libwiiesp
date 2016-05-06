//
// Licencia GPLv3
//
// Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
//
// libWiiEsp es software libre: usted puede redistribuirlo y/o modificarlo bajo los términos de la 
// Licencia Pública General GNU publicada por la Fundación para el Software Libre, ya sea la versión 3 
// de la Licencia, o (a su elección) cualquier versión posterior.
//
// libWiiEsp se distribuye con la esperanza de que sea útil, pero SIN GARANTÍA ALGUNA; ni siquiera 
// la garantía implícita MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. Consulte los detalles de
// la Licencia Pública General GNU para obtener una información más detallada.
//
// Debería haber recibido una copia de la Licencia Pública General GNU junto a libWiiEsp. En caso 
// contrario, consulte <http://www.gnu.org/licenses/>.
//

#ifndef _ANIMACION_H_
#define _ANIMACION_H_

	#include <cstring>
	#include <gctypes.h>
	#include <string>
	#include <vector>
	#include "imagen.h"
	#include "screen.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que permite crear una animación a partir de una imagen cargada en memoria
	 * 
	 * @details La clase Animación es un mecanismo para, a partir de una imagen previamente cargada en memoria,
	 * conseguir una simulación de movimiento mediante una secuencia de imágenes en dos dimensiones. Se basa en la idea
	 * de tener, en una misma textura, todos los fotogramas o cuadros que conforman la animación, y el orden por el que
	 * se organizan para mostrar un movimiento fluido.
	 *
	 * La misma idea de animación es la que se utiliza en el cine o la televisión: se muestra una secuencia de
	 * imágenes, una detrás de otra, pero con leves variaciones entre sí. Lo que se consigue con esto es que el cerebro
	 * humano interprete un movimiento reconstruyendo los "huecos" que existen entre una imagen y la siguiente.
	 *
	 * Como ya se ha mencionado, esta clase crea una animación a partir de una única imagen. Esta imagen debe organizar
	 * los fotogramas en forma de rejilla, de tal manera que todos los cuadros que vayan a componer el movimiento se
	 * encuentren en ella formando filas y columnas. Se recomienda ajustar lo mejor posible la cantidad de filas y
	 * columnas respecto al número de cuadros que contenga la animación, por ejemplo, si se van a emplear 20
	 * fotogramas, es razonable tener cuatro o cinco filas y cinco o cuatro columnas respectivamente; por el contrario,
	 * resultaría menos práctico (sobretodo a la hora de editar el archivo de imagen) tener una única fila con 20
	 * columnas. Pero esto es sólo un consejo, ya que la clase Animación soporta tanto el primer caso como el segundo.
	 *
	 * Otro concepto importante sobre las animaciones es el control del tiempo: el ojo humano sólo percibe 24 imágenes
	 * por segundo, de tal manera que un movimiento fluido tendrá ese número de fotogramas por segundo, o fps. Sin
	 * embargo, es posible crear una animación con un mayor o menor número de cuadros. En todo caso, la cantidad de
	 * cuadros que se utilicen para una animación determina tanto su velocidad como su fluidez: por ejemplo, en un
	 * sistema con una tasa de 24 fps, una animación que tenga 24 cuadros se apreciará totalmente fluida, y tardará un
	 * segundo en completar su ciclo; otra animación construida en el mismo sistema sobre 12 cuadros, por otra parte,
	 * tardará medio segundo en concluir, pero será menos fluida que la anterior.
	 *
	 * Teniendo en cuenta que, en un sistema, la tasa de fps es constante, y con el objetivo de intentar homogeneizar
	 * la duración de las animaciones, independientemente del número de cuadros que la compongan, surge el concepto de
	 * retardo. Esta idea consiste en esperar a que pasen algunos fotogramas dibujando el mismo cuadro de la animación
	 * antes de pasar a dibujar el siguiente. Siguiendo el mismo ejemplo anterior, se consigue que las dos animaciones
	 * tarden lo mismo en completarse si se incluye un retardo de dos fotogramas en la segunda de ellas. Es decir, la
	 * animación de 24 cuadros dibujaría uno de éstos en cada actualización, pero la de 12 lo haría cada dos avances
	 * (en una actualización se dibuja un cuadro nuevo, y en la inmediatamente siguiente, se vuelve a dibujar este
	 * mismo), de tal manera que ambas animaciones concluirían en el mismo periodo de tiempo.
	 *
	 * Esta técnica del retardo consigue, además, que la velocidad de todos los movimientos del programa dependan
	 * únicamente de la tasa de fotogramas por segundo, centralizando el control de la velocidad en un punto.
	 *
	 * Un último detalle a mencionar es que esta clase está construida sobre las clases Imagen y Screen. Hay que tener
	 * en cuenta, por ejemplo, que para cargar una imagen en memoria, existe la limitación de que sus medidas en
	 * píxeles deben ser múltiplos de 8. Todos estos detalles se pueden consultar en la documentación de cada una de
	 * las clases.
	 *
	 * Funcionamiento interno
	 *
	 * Cuando se crea una animación, se le deben pasar al constructor de la clase una serie de parámetros, que son un
	 * puntero a la imagen (previamente cargada en memoria) que contenga la rejilla de fotogramas, una cadena de
	 * caracteres de alto nivel (de tipo std::string) que contenga una secuencia de números enteros positivos,
	 * separados por comas y sin espacios (del tipo "0,1,2,3,4,5"), el número de filas y de columnas que hay en la
	 * rejilla, y el retardo a aplicar a la visualización de la animación (como ya se ha mencionado, esta cifra indica
	 * cada cuantas actualizaciones de la animación se va a dibujar un nuevo cuadro: el valor por defecto 1 indica que
	 * se dibuja un nuevo cuadro a cada fotograma).
	 *
	 * La secuencia de números enteros que se recibe indica el orden en el que se van dibujando los distintos cuadros
	 * de la animación. Cada cuadro de la rejilla corresponde con un número entero positivo, siendo el cero el
	 * fotograma de arriba a la izquierda, y avanzando de derecha a izquierda y de arriba abajo. Por ejemplo, una
	 * rejilla de tres columnas y dos filas tendrá en su primera fila (de izquierda a derecha) los cuadros cero, uno y
	 * dos; y la segunda fila, los cuadros tres, cuatro y cinco (igualmente, de izquierda a derecha). Un mismo cuadro
	 * se puede repetir cuantas veces se quiera en la secuencia de una misma animación.
	 *
	 * Un objeto de animación proporciona varios métodos observadores para conocer y acceder fácilmente a las variables
	 * que lo controlan, por ejemplo, la imagen, el ancho y alto en píxeles de un cuadro (todos los cuadros se
	 * consideran iguales), y cuáles son los índices del primer cuadro y del cuadro actual. También existen dos métodos
	 * para modificar el estado de la animación, concretamente son reiniciar (método que establece el paso actual al
	 * primero de la secuencia) y avanzar (que se encarga de calcular el siguiente cuadro a dibujar teniendo en cuenta
	 * el retardo y la propia secuencia de cuadros).
	 *
	 * El método dibujar se encarga de, como su propio nombre indica, plasmar en la pantalla el fotograma
	 * correspondiente al cuadro actual de la animación, en las coordenadas (x,y,z) que se indiquen. Se da la opción de
	 * invertir el fotograma respecto al eje vertical, funcionalidad que proporciona la propia clase Screen a través de
	 * su método dibujarCuadro. Este método realiza una llamada a avanzar, con lo que no es necesario preocuparse por
	 * la gestión de cuadros desde el exterior.
	 *
	 * Una cosa más a tener en cuenta es que el destructor de la clase es el predeterminado, por lo que no se destruye
	 * la imagen asociada a la animación, y hay que destruirla manualmente en caso de que se quiera liberar la memoria
	 * ocupada por ésta.
	 *
	 * Ejemplo de uso
	 *
	 * @code
	 * // Crear una animación a partir de una imagen de 3 filas y 4 columnas (12 cuadros), 
	 * // con un retardo de 2 (pasar a un nuevo cuadro cada dos actualizaciones)
	 * Imagen* rejilla = new Imagen();
	 * rejilla->cargarBmp( "/apps/wiipang/media/rejilla.bmp" );
	 * Animacion anima( rejilla, "0,1,2,3,4,5,6,7,8,9,10,11", 3, 4, 2 );
	 * // Bucle principal del juego
	 * while( 1 )
	 * {
	 *   // Dibujar animación
	 *   anima.dibujar( 100, 100, 50 );
	 *   // Dibujar animación invertida
	 *   anima.dibujar( 250, 100, 51, true );
	 *   // ...
	 *   screen->flip();
	 * }
	 * @endcode
	 * 
	 */
	class Animacion
	{
		public:

			/**
			 * Constructor de la clase Animacion.
			 * @param i Imagen que será base de la animación
			 * @param secuencia Enteros separados por comas, indica los distintos pasos correlativos de una animación
			 * @param filas Número de filas en la rejilla de la imagen i
			 * @param columnas Número de columnas en la rejilla de la imagen i
			 * @param retardo Número de frames que tienen que pasar para que se produzca un avance en la animación
			 */
			Animacion(const Imagen& i, const std::string& secuencia, u8 filas = 1, u8 columnas = 1, u8 retardo = 1);

			/**
			 * Indica si la animación se encuentra en el primer paso.
			 * @return Verdadero si la animación se encuentra en el primer paso, o falso en caso contrario.
			 */
			bool primerPaso(void) const;

			/**
			 * Devuelve el número que indica el paso actual de la animación. El primero es cero.
			 * @return Número que indica el paso actual de la animación.
			 */
			u8 pasoActual(void) const;

			/**
			 * Devuelve el alto, en píxeles, de cada cuadro de la animación
			 * @return Alto, en píxeles, de cada cuadro de la animación
			 */
			u16 alto(void) const;

			/**
			 * Devuelve el ancho, en píxeles, de cada cuadro de la animación
			 * @return Ancho, en píxeles, de cada cuadro de la animación
			 */
			u16 ancho(void) const;

			/**
			 * Devuelve la imagen de la animación.
			 * @return Referencia constante a la imagen de la animación.
			 */
			const Imagen& imagen(void) const;

			/**
			 * Avanza al paso siguiente del actual. Si el actual es el último, se sitúa en el primero.
			 */
			void avanzar(void);

			/**
			 * Sitúa el contador de pasos de la animación en el primer paso.
			 */
			void reiniciar(void);

			/**
			 * Dibuja en la pantalla la imagen asociada con el paso actual de la animación, y avanza al paso siguiente.
			 * @param x Coordenada X donde se dibujará la imagen asociada al paso actual de la animación
			 * @param y Coordenada Y donde se dibujará la imagen asociada al paso actual de la animación
			 * @param z Capa donde se dibujará la imagen asociada al paso actual de la animación (entre 0 y 999)
			 * @param invertir Verdadero si se quiere dibujar el cuadro de la textura invertido respecto al eje vertical
			 */
			void dibujar(s16 x, s16 y, s16 z, bool invertir = false);

		private:

			const Imagen* _imagen;
			u16 _ancho_cuadro, _alto_cuadro;
			u8 _filas, _columnas, _paso, _retardo, _cont_retardo;
			std::vector<s8> _cuadros;
	};

#endif

