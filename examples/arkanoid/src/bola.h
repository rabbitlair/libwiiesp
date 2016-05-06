//
// Licencia GPLv3
//
// Este archivo es parte de libWiiEsp, y su autor es Ezequiel Vázquez De la Calle
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

#ifndef _BOLA_H_
#define _BOLA_H_

	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Representación de la bola con la que se destruyen los ladrillos.
	 *
	 * @details Esta clase deriva de la clase abstracta Actor, y gestiona la bola que destruye los ladrillos en el
	 * juego. El movimiento de la bola consiste en un vector, representado por un ángulo respecto a la abscisa positiva
	 * (0,infinito), y un módulo (que determina la velocidad de la bola). Según estos dos atributos del vector, se
	 * calculan en el escenario los rebotes de la bola (ángulo y módulo nuevos), de tal manera que los cálculos se
	 * resumen en operaciones relativamente sencillas de trigonometría. El vector, una vez calculados sus nuevos
	 * atributos, se descompone en sus dos componentes horizontal y vertical, que se asignan a las velocidades
	 * horizontal y vertical del actor bola, respectivamente.
	 *
	 */
	class Bola: public Actor
	{
		public:

			/**
			 * Constructor de la clase Bola.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Bola.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Bola(const std::string& ruta, const Nivel* nivel) throw (Excepcion);

			/**
			 * Destructor de la clase Bola.
			 */
			~Bola(void);

			/**
			 * Método de actualización de la bola. Aquí se ejecuta el movimiento del actor en base al vector.
			 */
			void actualizar(void);

			/**
			 * Metodo consultor para el módulo del vector de movimiento de la bola
			 * @return Valor del módulo del vector de movimiento de la bola
			 */
			f32 velocidad(void) const;

			/**
			 * Metodo consultor para el valor máximo que puede tener el módulo del vector de movimiento de la bola
			 * @return Valor máximo del módulo del vector de movimiento de la bola
			 */
			f32 maxVelocidad(void) const;

			/**
			 * Metodo consultor para el ángulo del vector de movimiento de la bola
			 * @return Valor del ángulo del vector de movimiento de la bola
			 */
			f32 direccion(void) const;

			/**
			 * Metodo modificador para la velocidad (módulo del vector de movimiento) de la bola
			 * @param v Nuevo valor del módulo que tendrá el vector de movimiento de la bola.
			 */
			void setVelocidad(f32 v);

			/**
			 * Metodo modificador para la dirección (ángulo del vector de movimiento) de la bola
			 * @param d Nuevo valor del ángulo que tendrá el vector de movimiento de la bola.
			 */
			void setDireccion(f32 d);

		private:

			// La velocidad de la bola es el módulo del vector de movimiento de ésta, cuyas componentes 
			// horizontal y vertical son, respectivamente, _vx y _vy.
			f32 _velocidad;

			// La direccion de la bola es el ángulo en radianes que forma su vector de movimiento
			// respecto a unos ejes cartesianos
			f32 _direccion;

			f32 _max_vel;
	};

#endif

