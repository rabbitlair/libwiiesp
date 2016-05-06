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

#ifndef _PERSONAJE_H_
#define _PERSONAJE_H_

	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Personaje controlado por el jugador.
	 *
	 * @details Actor controlado por el jugador, que debe evitar ser aplastado por las bolas, y que puede lanzar ganchos
	 * para destruir éstas. La gestión de los distintos estados del actor, el lanzamiento de ganchos y la detección de
	 * colisiones se realiza desde la clase Escenario.
	 *
	 */
	class Personaje: public Actor
	{
		public:

			/**
			 * Constructor de la clase Personaje.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del personaje.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Personaje(const std::string& ruta, const Nivel* nivel) throw (Excepcion);

			/**
			 * Destructor de la clase Personaje.
			 */
			~Personaje(void);

			/**
			 * Método de actualización del personaje.
			 */
			void actualizar(void);

		private:

			bool _choque;

			f32 _vy_real;

	};

#endif

