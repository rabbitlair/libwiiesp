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

#ifndef _ITEM_H_
#define _ITEM_H_

	#include <cstdlib>
	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase que representa un objeto que puede caer al destruir un ladrillo.
	 *
	 * @details Un item es un objeto que modifica el comportamiento del juego. Las cuatro posibles modificaciones que
	 * pueden darse son una vida extra (objeto azul), obtener doble puntuación durante 12 segundos (objeto verde),
	 * aumentar el tamaño de la pala durante 12 segundos (objeto amarillo), o reducir el tamaño de la pala durante 12
	 * segundos (objeto rojo). La implementación de estas modificaciones se encuentra en la clase Escenario.
	 *
	 * Cuando el objeto llega a la parte inferior de la pantalla, se destruye. Su efecto se aplica cuando colisiona con
	 * la pala que controla el jugador.
	 *
	 */
	class Item: public Actor
	{
		public:

			/**
			 * Constructor de la clase Item. Al crear el actor se genera aleatoriamente el tipo de item que será.
			 * Las cuatro posbilidades tienen un 25% de probabilidad de salir.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Item.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Item(const std::string& ruta, const Nivel* nivel) throw (Excepcion);

			/**
			 * Destructor de la clase Item.
			 */
			~Item(void);

			/**
			 * Método de actualización del item. Aquí se ejecuta el movimiento de caída actor.
			 */
			void actualizar(void);
	};

#endif

