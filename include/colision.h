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

#ifndef _COLISION_H_
#define _COLISION_H_

	#include <cmath>
	#include <cstdlib>
	#include <gctypes.h>
	#include "excepcion.h"
	#include "parser.h"

	// Declaraciones anticipadas
	class Circulo;
	class Punto;
	class Rectangulo;

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase abstracta que sirve como base a todas las figuras de colisión definidas en el sistema.
	 *
	 * @details La clase abstracta Figura es la base del módulo de gestión de colisiones integrado en LibWiiEsp. Se
	 * basa en una implementación de la técnica double dispatch (que a su vez es una implementación del patrón
	 * Visitante) consistente en una especie de polimorfismo donde la elección del método a ejecutar depende no sólo
	 * del objeto que lo ejecuta, si no del tipo del parámetro que recibe. En la práctica, al emplear esta técnica se
	 * consigue evitar la comprobación de tipos mediante estructuras de tipo condicional (if, case, etc.)  cuando se
	 * quieren comparar dos objetos de clases derivadas de Figura.
	 *
	 * Entrado un poco más en detalle, double dispatch implica que toda clase derivada de Figura implementará un método
	 * hayColision, en el que recibirá un puntero a zona de memoria donde se almacenará otra Figura. Este método
	 * devolverá un valor booleano, que será verdadero si hay colisión entre las dos figuras, o falso en caso
	 * contrario. Internamente, devuelve el resultado de llamar al método hayColision de la segunda figura, pasando
	 * como parámetro la figura actual (es decir, el objeto this en el contexto de la primera figura). Con este
	 * intercambio de parámetros se consigue que esta segunda llamada se realice conociendo el tipo exacto del
	 * parámetro que se pasa, ya que en el contexto de ejecución (se hace desde un método de la primera figura) se
	 * conoce el tipo del objeto this. Al llamarse el método hayColision de la segunda figura, recibiendo un parámetro
	 * con el tipo perfectamente identificado, esta segunda figura ya sabe los tipos de ambos objetos (conoce su propio
	 * tipo por el contexto de ejecución, y el de la primera figura por lo explicado anteriormente), de tal manera que
	 * se ejecuta el método adecuado con los cálculos necesarios para averiguar si hay colisión entre ambas figuras o
	 * no.
	 *
	 * Gracias a esta técnica de double dispatch se consigue una mayor escalabilidad del código de las colisiones, ya
	 * que resulta muy sencillo añadir nuevas figuras que detecten colisiones entre sí y entre las ya existentes.
	 * Únicamente basta con derivar la clase abstracta Figura, definir el método hayColision que recibe un puntero a
	 * Figura (tal y como se ha descrito e implementan las figuras existentes), y crear los métodos hayColision que
	 * calculen si se dan colisiones entre los distintos tipos de figuras. El inconveniente de esta técnica es que se
	 * debe duplicar algo de código, debido a que un método de Figura1 que compruebe la colisión entre Figura1 y
	 * Figura2 debe estar definido exactamente igual al método de Figura2 que evalúe la colisión entre esos dos tipos
	 * de figuras.
	 *
	 * El otro aspecto importante relativo a las figuras de colisión es el desplazamiento de éstas. Una figura de
	 * colisión está determinada, como mínimo, por una pareja de coordenadas cuyo punto de origen (es decir, el (0,0))
	 * corresponde con el punto superior izquierdo de un objeto del sistema; objeto que tendrá, a su vez, una pareja de
	 * coordenadas (x,y) respecto al límite izquierdo del escenario (coordenada X, cuanto mayor sea, más alejado hacia
	 * la derecha estará el objeto de este límite izquierdo) y al límite superior del escenario (coordenada Y, cuanto
	 * mayor sea, más alejado hacia abajo estará el objeto de este límite superior). Es decir, las coordenadas de una
	 * figura de colisión son relativas a un punto que no tiene por qué ser el origen (0,0) de las coordenadas de los
	 * objetos del sistema.
	 *
	 * Por ejemplo, suponiendo un personaje que tenga asociado un rectángulo de colisiones, este personaje tiene unas
	 * coordenadas (x,y) respecto al punto superior izquierdo del escenario, y que determinan la posición del personaje
	 * en dicho escenario. Esta pareja de coordenadas variará en función del comportamiento del personaje durante la
	 * ejecución del juego. Sin embargo, las parejas de coordenadas de los puntos que determinan el rectángulo de
	 * colisiones se mantendrán fijas en todo momento, ya que son relativas al punto superior izquierdo del personaje,
	 * y no al del escenario. Esto provoca que, para conocer la posición de un punto del rectángulo respecto al origen
	 * de coordenadas del escenario, haya que sumar el valor de la coordenada X del personaje con el de la coordenada X
	 * del punto; y análogamente para las coordenadas Y.
	 *
	 * Ciertamente, es más complicado gestionar dos sistemas de referencia (uno para los objetos del sistema, y otro
	 * para las figuras de colisión), pero se ha decidido que cada figura de colisión esté asociada a un objeto del
	 * sistema, y que sus coordenadas sean relativas al punto superior izquierdo de su objeto asociado, porque supone
	 * un coste muy alto en procesamiento el tener que estar actualizando las posiciones absolutas de todas las figuras
	 * de colisión en cada iteración del bucle principal.
	 *
	 * En base a todo lo explicado, para poder realizar los cálculos de colisiones correctamente hay que contar con la
	 * posición absoluta del objeto que tiene asociado cada figura de colisión. Esto se consigue con el concepto de
	 * desplazamiento, y que no es más que dos parejas de valores que se le pasan al método hayColision de una figura
	 * cuando se quiere saber si colisiona con otra. Cada pareja de valores son las coordenadas del objeto asociado a
	 * una de las figuras, para que se tengan en cuenta a la hora de calcular si hay colisión o no. Concretamente, la
	 * primera pareja de valores dx1 y dy1 indican el desplazamiento de la figura de la cual se llama a su método
	 * hayColision, y los valores dx2 y dy2, el desplazamiento de la figura que se recibe por parámetro.
	 *
	 */
	class Figura
	{
		public:

			/**
			 * Método virtual puro para saber si una figura que se recibe por parámetro colisiona con la figura
			 * a la que pertenece el método. La definición de este método debe ser siempre la misma, ya que con
			 * ello se consigue la implementación de la técnica de double dispatch. Es la siguiente:
			 *     return f->hayColision(this, dx2, dy2, dx1, dy1);
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal de la figura actual
			 * @param dy1 Desplazamiento vertical de la figura actual
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			virtual bool hayColision(Figura* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) = 0;

			/**
			 * Método virtual puro para saber si un punto que se recibe por parámetro colisiona con la figura
			 * a la que pertenece el método.
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal de la figura actual
			 * @param dy1 Desplazamiento vertical de la figura actual
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			virtual bool hayColision(Punto* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) = 0;

			/**
			 * Método virtual puro para saber si un círculo que se recibe por parámetro colisiona con la figura
			 * a la que pertenece el método.
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal de la figura actual
			 * @param dy1 Desplazamiento vertical de la figura actual
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			virtual bool hayColision(Circulo* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) = 0;

			/**
			 * Método virtual puro para saber si un rectángulo que se recibe por parámetro colisiona con la figura
			 * a la que pertenece el método.
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal de la figura actual
			 * @param dy1 Desplazamiento vertical de la figura actual
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			virtual bool hayColision(Rectangulo* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) = 0;

			/**
			 * Destructor virtual de la clase Figura.
			 */
			virtual ~Figura(void) { };

			/**
			 * Método que lee un rectángulo de colisión desde un elemento de un árbol XML.
			 * @param nodo Elemento de un árbol XML que contiene la información de un rectángulo de colisión.
			 * @return Puntero a un rectángulo de colisión creado a partir de la información del elemento XML.
			 */
			static Figura* leerRectangulo(TiXmlElement* nodo);

			/**
			 * Método que lee un círculo de colisión desde un elemento de un árbol XML.
			 * @param nodo Elemento de un árbol XML que contiene la información de un círculo de colisión.
			 * @return Puntero a un círculo de colisión creado a partir de la información del elemento XML.
			 */
			static Figura* leerCirculo(TiXmlElement* nodo);

			/**
			 * Método que lee un punto de colisión desde un elemento de un árbol XML.
			 * @param nodo Elemento de un árbol XML que contiene la información de un punto de colisión.
			 * @return Puntero a un punto de colisión creado a partir de la información del elemento XML.
			 */
			static Figura* leerPunto(TiXmlElement* nodo);
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que representa un punto en el sistema de gestión de colisiones
	 * 
	 * @details Se representa mediante dos enteros de 16 bits con signo, y deriva de la clase Figura.
	 */
	class Punto: public Figura
	{
		public:

			/**
			 * Constructor de la clase Punto
			 * @param x Coordenada X del punto. Por defecto, tiene valor 0.
			 * @param y Coordenada Y del punto. Por defecto, tiene valor 0.
			 */
			Punto(s16 x = 0, s16 y = 0): _x(x), _y(y) { };

			/**
			 * Método para saber si una figura de tipo desconocido que se recibe por parámetro colisiona con el
			 * punto. Implementación de la técnica de double dispatch.
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del punto
			 * @param dy1 Desplazamiento vertical del punto
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Figura* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) {
				return f->hayColision(this, dx2, dy2, dx1, dy1);
			};

			/**
			 * Método para saber si un círculo que se recibe por parámetro colisiona con el punto actual.
			 * Implementación de la técnica de double dispatch.
			 * @param c Puntero a un círculo con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del punto
			 * @param dy1 Desplazamiento vertical del punto
			 * @param dx2 Desplazamiento horizontal del círculo que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del círculo que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Circulo* c, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método para saber si un rectángulo que se recibe por parámetro colisiona con el punto actual.
			 * Implementación de la técnica de double dispatch.
			 * @param r Puntero a un rectángulo con el que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del punto
			 * @param dy1 Desplazamiento vertical del punto
			 * @param dx2 Desplazamiento horizontal del rectángulo que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del rectángulo que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Rectangulo* r, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método para saber si un punto que se recibe por parámetro colisiona con el punto actual.
			 * Implementación de la técnica de double dispatch.
			 * @param p Puntero a un punto con el que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del punto
			 * @param dy1 Desplazamiento vertical del punto
			 * @param dx2 Desplazamiento horizontal del punto que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del punto que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Punto* p, s16 dx1, s16 dy1, s16 dx2, s16 dy2) { return false; }

			/**
			 * Método modificador que devuelve una referencia a la coordenada X del punto.
			 * @return Referencia a la coordenada X del punto.
			 */
			s16& x(void) { return _x; };

			/**
			 * Método consultor que devuelve la coordenada X del punto.
			 * @return Valor de la coordenada X del punto.
			 */
			const s16& x(void) const { return _x; };

			/**
			 * Método modificador que devuelve una referencia a la coordenada Y del punto.
			 * @return Referencia a la coordenada Y del punto.
			 */
			s16& y(void) { return _y; };

			/**
			 * Método consultor que devuelve la coordenada Y del punto.
			 * @return Valor de la coordenada Y del punto.
			 */
			const s16& y(void) const { return _y; };

		private:

			s16 _x, _y;
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que representa un rectángulo en el sistema de gestión de colisiones
	 * 
	 * @details Se representa mediante cuatro puntos de tipo Punto, y deriva de la clase Figura. Se considera que sus 
	 * lados son paralelos a los ejes, pero se representa con los 4 puntos en lugar de con un punto y el 
	 * ancho y alto para, en un futuro, permitir rectangulos no alineados con los ejes.
	 */
	class Rectangulo: public Figura
	{
		public:

			/**
			 * Constructor de la clase Rectángulo. El orden de los puntos esperado es superior izquierdo (p1),
			 * superior derecho (p2), inferior derecho (p3) e inferior izquierdo (p4).
			 * @param p1 Punto superior izquierdo del rectángulo.
			 * @param p2 Punto superior derecho del rectángulo.
			 * @param p3 Punto inferior izquierdo del rectángulo.
			 * @param p4 Punto inferior derecho del rectángulo.
			 */
			Rectangulo(Punto p1, Punto p2, Punto p3, Punto p4): _p1(p1), _p2(p2), _p3(p3), _p4(p4)
			{
				_centro.x() = _p1.x() + abs(_p1.x() - _p2.x())/2;
				_centro.y() = _p1.y() + abs(_p1.y() - _p4.y())/2;
			};

			/**
			 * Método para saber si una figura de tipo desconocido que se recibe por parámetro colisiona con el
			 * rectángulo. Implementación de la técnica de double dispatch.
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del rectángulo
			 * @param dy1 Desplazamiento vertical del rectángulo
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Figura* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) {
				return f->hayColision(this, dx2, dy2, dx1, dy1);
			};

			/**
			 * Método para saber si un círculo que se recibe por parámetro colisiona con el
			 * rectángulo. Implementación de la técnica de double dispatch.
			 * @param c Puntero a un círculo con el que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del rectángulo
			 * @param dy1 Desplazamiento vertical del rectángulo
			 * @param dx2 Desplazamiento horizontal del círculo que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del círculo que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Circulo* c, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método para saber si un rectángulo desconocido que se recibe por parámetro colisiona con el
			 * rectángulo. Implementación de la técnica de double dispatch.
			 * @param r Puntero a un rectángulo con el que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del rectángulo
			 * @param dy1 Desplazamiento vertical del rectángulo
			 * @param dx2 Desplazamiento horizontal del rectángulo que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del rectángulo que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Rectangulo* r, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método para saber si un punto que se recibe por parámetro colisiona con el
			 * rectángulo. Implementación de la técnica de double dispatch.
			 * @param p Puntero a un punto con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del rectángulo
			 * @param dy1 Desplazamiento vertical del rectángulo
			 * @param dx2 Desplazamiento horizontal del punto que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del punto que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Punto* p, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método modificador que devuelve una referencia al punto superior izquierdo del rectángulo
			 * @return Referencia al punto superior izquierdo del rectángulo
			 */
			Punto& p1(void) { return _p1; };

			/**
			 * Método consultor que devuelve el punto superior izquierdo del rectángulo
			 * @return Punto superior izquierdo del rectángulo
			 */
			const Punto& p1(void) const { return _p1; };

			/**
			 * Método modificador que devuelve una referencia al punto superior derecho del rectángulo
			 * @return Referencia al punto superior derecho del rectángulo
			 */
			Punto& p2(void) { return _p2; };

			/**
			 * Método consultor que devuelve el punto superior derecho del rectángulo
			 * @return Punto superior derecho del rectángulo
			 */
			const Punto& p2(void) const { return _p2; };

			/**
			 * Método modificador que devuelve una referencia al punto inferior derecho del rectángulo
			 * @return Referencia al punto inferior derecho del rectángulo
			 */
			Punto& p3(void) { return _p3; };

			/**
			 * Método consultor que devuelve el punto inferior derecho del rectángulo
			 * @return Punto inferior derecho del rectángulo
			 */
			const Punto& p3(void) const { return _p3; };

			/**
			 * Método modificador que devuelve una referencia al punto inferior izquierdo del rectángulo
			 * @return Referencia al punto inferior izquierdo del rectángulo
			 */
			Punto& p4(void) { return _p4; };

			/**
			 * Método consultor que devuelve el punto inferior izquierdo del rectángulo
			 * @return Punto inferior izquierdo del rectángulo
			 */
			const Punto& p4(void) const { return _p4; };

			/**
			 * Método modificador que devuelve el centro del rectángulo
			 * @return Referencia al centro del rectángulo
			 */
			Punto& centro(void) { return _centro; };

			/**
			 * Método consultor que devuelve el centro del rectángulo
			 * @return Centro del rectángulo
			 */
			const Punto& centro(void) const { return _centro; };

		private:

			Punto _p1, _p2, _p3, _p4, _centro;
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que representa un círculo en el sistema de gestión de colisiones
	 * 
	 * @details Se representa mediante un punto (su centro) y un radio, que es un decimal en coma flotante de 32 bits.
	 * Deriva de la clase Figura.
	 */
	class Circulo: public Figura
	{
		public:

			/**
			 * Constructor de la clase Círculo.
			 * @param centro Centro del círculo
			 * @param radio Radio del círculo
			 */
			Circulo(Punto centro, f32 radio): _centro(centro), _radio(radio) { };

			/**
			 * Método para saber si una figura de tipo desconocido que se recibe por parámetro colisiona con el
			 * círculo. Implementación de la técnica de double dispatch.
			 * @param f Puntero a una figura con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del círculo
			 * @param dy1 Desplazamiento vertical del círculo
			 * @param dx2 Desplazamiento horizontal de la figura que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical de la figura que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Figura* f, s16 dx1, s16 dy1, s16 dx2, s16 dy2) {
				return f->hayColision(this, dx2, dy2, dx1, dy1);
			};

			/**
			 * Método para saber si un círculo que se recibe por parámetro colisiona con el
			 * círculo. Implementación de la técnica de double dispatch.
			 * @param c Puntero a un círculo con el que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del círculo
			 * @param dy1 Desplazamiento vertical del círculo
			 * @param dx2 Desplazamiento horizontal del círculo que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del círculo que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Circulo* c, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método para saber si un rectángulo desconocido que se recibe por parámetro colisiona con el
			 * círculo. Implementación de la técnica de double dispatch.
			 * @param r Puntero a un rectángulo con el que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del círculo
			 * @param dy1 Desplazamiento vertical del círculo
			 * @param dx2 Desplazamiento horizontal del rectángulo que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del rectángulo que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Rectangulo* r, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método para saber si un punto que se recibe por parámetro colisiona con el
			 * círculo. Implementación de la técnica de double dispatch.
			 * @param p Puntero a un punto con la que se quiere comprobar si existe colisión
			 * @param dx1 Desplazamiento horizontal del círculo
			 * @param dy1 Desplazamiento vertical del círculo
			 * @param dx2 Desplazamiento horizontal del punto que se recibe por parámetro
			 * @param dy2 Desplazamiento vertical del punto que se recibe por parámetro
			 * @return Verdadero si hay colisión entre las figuras, o falso en caso contrario
			 */
			bool hayColision(Punto* p, s16 dx1, s16 dy1, s16 dx2, s16 dy2);

			/**
			 * Método modificador que devuelve una referencia al centro del circulo
			 * @return Referencia al centro del circulo
			 */
			Punto& centro(void) { return _centro; };

			/**
			 * Método consultor que devuelve el centro del circulo
			 * @return Punto centro del circulo
			 */
			const Punto& centro(void) const { return _centro; };

			/**
			 * Método modificador que devuelve una referencia al radio del circulo
			 * @return Referencia al radio del circulo
			 */
			f32& radio(void) { return _radio; };

			/**
			 * Método consultor que devuelve el centro del circulo
			 * @return Punto centro del circulo
			 */
			const f32& radio(void) const { return _radio; };

		private:

			Punto _centro;
			f32 _radio;
	};

#endif

