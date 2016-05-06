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

#ifndef _PATO_H_
#define _PATO_H_

	#define _USE_MATH_DEFINES
	#include <cmath>
	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Patos a los que los jugadores deben disparar para ganar puntos en el juego.
	 *
	 * @details Un objeto de esta clase es un pato que se genera aleatoriamente en un punto de los laterales de la
	 * pantalla, con un movimiento rectilíneo hacia el otro lado de ésta. Puede ser abatido por un jugador que controle
	 * un objeto de la clase Mira, sumando en este caso una cantidad concreta de puntos al marcador del jugador que lo
	 * consiga.
	 *
	 * Un pato tiene una combinación de colores aleatoria (de entre tres posibles), y el ángulo de su vuelo respecto a
	 * la horizontal y su velocidad son determinados también aleatoriamente. Cuando es abatido, un pato cae
	 * verticalmente hasta la parte inferior de la pantalla, tras permanecer 0,35 segundos en un estado de "impactado".
	 *
	 */
	class Pato: public Actor
	{
		public:

			/**
			 * Constructor de la clase Pato.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Pato.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Pato(const std::string& ruta, const Nivel* nivel) throw (Excepcion);

			/**
			 * Destructor de la clase Pato.
			 */
			~Pato(void);

			/**
			 * Método de actualización del pato. Aquí se ejecuta el movimiento del actor.
			 */
			void actualizar(void);

			/**
			 * Metodo consultor para el módulo del vector de movimiento del pato
			 * @return Valor del módulo del vector de movimiento del pato
			 */
			f32 velocidad(void) const;

			/**
			 * Metodo consultor para el ángulo del vector de movimiento del pato
			 * @return Valor del ángulo del vector de movimiento del pato
			 */
			f32 direccion(void) const;

			/**
			 * Metodo consultor para el crono del pato. Si es cero, es que el crono no ha sido activado. Este crono
			 * controla la transición desde el estado "impacto" al estado "muerto".
			 * @return Valor del crono del pato
			 */
			u32 crono(void) const;

			/**
			 * Metodo modificador para la velocidad (módulo del vector de movimiento) del pato
			 * @param v Nuevo valor del módulo que tendrá el vector de movimiento del pato
			 */
			void setVelocidad(f32 v);

			/**
			 * Metodo modificador para la dirección (ángulo del vector de movimiento) del pato
			 * @param d Nuevo valor del ángulo que tendrá el vector de movimiento del pato
			 */
			void setDireccion(f32 d);

			/**
			 * Metodo modificador para el crono del pato. Este crono controla la transición desde el estado 
			 * "impacto" al estado "muerto".
			 * @param d Nuevo valor del crono del pato
			 */
			void setCrono(u32 c);

		private:

			// La velocidad del pato es el módulo del vector de movimiento de éste, cuyas componentes 
			// horizontal y vertical son, respectivamente, _vx y _vy.
			f32 _velocidad;

			// La direccion del pato es el ángulo en radianes que forma su vector de movimiento
			// respecto a unos ejes cartesianos
			f32 _direccion;

			// Almacena el instante en el que colisiona con un disparo, y puede ser consultado
			u32 _crono;
	};

#endif

