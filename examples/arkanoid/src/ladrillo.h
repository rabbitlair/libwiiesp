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

#ifndef _LADRILLO_H_
#define _LADRILLO_H_

	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase que representa un ladrillo que hay que destruir.
	 *
	 * @details Los objetos de esta clase son el objetivo que hay que golpear con la bola para que desaparezcan,
	 * pudiendo producir un objeto de la clase Item al ser destruido. Cada ladrillo suma una cantidad de puntos al
	 * marcador del jugador, y según dónde le golpee la bola, provocará que ésta modifique su vector de movimiento de
	 * una forma u otra. Un nivel acaba cuando no quedan más ladrillos en él. Por otro lado, puede tener varios colores,
	 * que son negro, blanco, gris, rosa, verde, azul, amarillo y rojo.
	 *
	 */
	class Ladrillo: public Actor
	{
		public:

			/**
			 * Constructor de la clase Ladrillo.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Ladrillo.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @param color Cadena de caracteres con el nombre del color que tendrá el ladrillo.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Ladrillo(const std::string& ruta, const Nivel* nivel, const std::string& color) throw (Excepcion);

			/**
			 * Destructor de la clase Ladrillo.
			 */
			~Ladrillo(void);

			/**
			 * Método de actualización del ladrillo.
			 */
			void actualizar(void);
	};

#endif

