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

#ifndef _GANCHO_H_
#define _GANCHO_H_

	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Gancho lanzado por el jugador para destruir las bolas.
	 *
	 * @details Un gancho es el arma de que dispone el jugador para eliminar las bolas que rebotan por la pantalla.
	 * Cuando se crea un gancho, éste aparece en las mismas coordenadas que el jugador, y avanza (crece) verticalmente
	 * hasta llegar al límite superior de la zona de juego, momento en el que desaparece. Mientras avanza, puede
	 * colisionar con una bola, en cuyo caso, el gancho y la bola son destruidos. La detección de colisiones entre el
	 * gancho y las bolas o el escenario se realiza en la clase Escenario. Sin embargo, la gestión del tamaño del gancho
	 * se realiza en el objeto que representa al propio gancho.
	 *
	 */
	class Gancho: public Actor
	{
		public:

			/**
			 * Constructor de la clase Gancho.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del gancho.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Gancho(const std::string& ruta, const Nivel* nivel, const u8 id) throw (Excepcion);

			/**
			 * Destructor de la clase Gancho.
			 */
			~Gancho(void);

			/**
			 * Método de actualización del gancho.
			 */
			void actualizar(void);

			/**
			 * Sobrecarga del método de dibujo de la clase base Actor. Esta implementación dibuja el gancho de una forma
			 * diferente a como se realiza la operación en actores comunes. En primer lugar, se dibuja la punta del
			 * gancho en las coordenadas correspondientes a la situación del gancho, y después se recorre la distancia
			 * desde la parte inferior del gancho hasta la parte inferior de la zona de juego, dibujando en cada paso la
			 * animación correspondiente del cable del gancho.
			 * @param x Coordenada X del punto superior izquierdo donde se comienza a dibujar el gancho
			 * @param y Coordenada Y del punto superior izquierdo donde se comienza a dibujar el gancho
			 * @param z Capa de dibujo donde se va a dibujar el gancho
			 */
			void dibujar(s16 x, s16 y, s16 z);

			/**
			 * Método consultor para el atributo que indica el identificador del gancho.
			 * @return Identificador del gancho.
			 */
			u8 id(void) const;

			/**
			 * Método consultor para el atributo que indica si el gancho debe destruirse o no.
			 * @return Verdadero si el objeto que representa al gancho debe destruirse, o falso en caso contrario.
			 */
			bool destruir(void) const;

			/**
			 * Método que establece la bandera de destrucción del gancho a verdadera.
			 */
			void setDestruir(void);

		private:

			const u8 _gancho_id;

			bool _destruir;

	};

#endif

