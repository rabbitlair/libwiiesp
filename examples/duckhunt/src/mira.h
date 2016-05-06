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

#ifndef _MIRA_H_
#define _MIRA_H_

	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Punto de mira controlado por un jugador para cazar a los patos.
	 *
	 * @details Un objeto instanciado a partir de esta clase será el punto de mira de la escopeta de un jugador, con la
	 * cual debe intentar abatir la mayor cantidad de patos que sea capaz. La escopeta se dispara al pulsar el botón B
	 * del mando asociado al jugador correspondiente, y tarda en recargar 0,8 segundos (estos detalles se controlan en
	 * la clase Escenario).
	 *
	 */
	class Mira: public Actor
	{
		public:

			/**
			 * Constructor de la clase Mira.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Mira.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @param jugador Identificador del jugador que controlará el punto de mira (pj1 o pj2)
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Mira(const std::string& ruta, const Nivel* nivel, const std::string& jugador) throw (Excepcion);

			/**
			 * Destructor de la clase Mira.
			 */
			~Mira(void);

			/**
			 * Método de actualización del punto de mira.
			 */
			void actualizar(void);

			/**
			 * Método consultor para el temporizador que indica el "tick" en el que se disparó por última vez.
			 * @return Valor del temporizador que indica el "tick" en el que se disparó por última vez.
			 */
			u32 crono(void) const;

			/**
			 * Método consultor para saber si se está recargando o no en este momento.
			 * @return Verdadero si se está recargando (no se puede disparar) o falso en caso contrario.
			 */
			bool recargando(void) const;

			/**
			 * Método modificador para el temporizador que indica el "tick" en el que se disparó por última vez.
			 * @param c Nuevo valor del temporizador que indica el "tick" en el que se disparó por última vez.
			 */
			void setCrono(u32 c);

			/**
			 * Método modificador para indicar si se está recargando o no en este momento.
			 * @param r Verdadero si se está recargando (no se puede disparar) o falso en caso contrario.
			 */
			void setRecargando(bool r);

		private:

			u32 _crono;

			bool _recargando;
	};

#endif

