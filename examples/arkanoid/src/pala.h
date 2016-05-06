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

#ifndef _PALA_H_
#define _PALA_H_

	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Pala controlada por el jugador para destruir los ladrillos, y para evitar que la bola salga de la pantalla.
	 *
	 * @details Un objeto de esta clase constituye el actor que controla el jugador a la hora de jugar a Arkanoid. Tiene
	 * seis posibles estados, agrupados por parejas, en el que cada par representa a la pala con un tamaño concreto
	 * (pequeña, normal o grande) con los estados "normal" (no hay movimiento) y "mover" (la pala se está moviendo). Los
	 * rectángulos de colisión para cada estado se determinan, como en todos los actores, mediante un archivo XML de
	 * información del actor. Los cambios de estado se realizan desde la clase Escenario.
	 *
	 */
	class Pala: public Actor
	{
		public:

			/**
			 * Constructor de la clase Pala.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Pala.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Pala(const std::string& ruta, const Nivel* nivel) throw (Excepcion);

			/**
			 * Destructor de la clase Pala.
			 */
			~Pala(void);

			/**
			 * Método de actualización de la pala.
			 */
			void actualizar(void);
	};

#endif

