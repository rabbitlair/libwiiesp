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

	#include <cmath>
	#include "libwiiesp.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Representación de una bola que rebota en el escenario y que el jugador debe destruir con los ganchos.
	 *
	 * @details Una bola es un actor controlado por la máquina, que tiene un movimiento que simula un rebote con
	 * gravedad, y que puede interactuar con el actor jugador (eliminándolo y restándole una vida) y con los ganchos que
	 * lanza éste (que provocan que la bola sea destruida).
	 *
	 * El movimiento de la bola tiene dos componentes: la horizontal tiene una velocidad uniforme que es idéntica para
	 * todos los tipos de bola, y la vertical parte de una velocidad inicial propia del tamaño de la bola cuando ésta se
	 * encuentra en la parte inferior de la zona de juego. A medida que la bola va ascendiendo, su velocidad vertical se
	 * va reduciendo (con una aceleración constante) hasta llegar a cero, momento en el cual comienza de nuevo a
	 * aumentar hasta que colisiona con la parte inferior de la zona de juego, y se reinicia el proceso.
	 *
	 * Cuando una bola colisiona con un gancho lanzado por el jugador, ésta es destruida, y en su lugar aparecen dos
	 * bolas nuevas, del tamaño inmediatamente inferior a la primera, y cada una con una componente horizontal de
	 * movimiento opuesta a la otra. Si la bola tuviera el tamaño mínimo, simplemente desaparecería.
	 *
	 * Tanto las funciones de rebote como las colisiones de una bola con el actor jugador o los ganchos se gestionan
	 * desde la clase Escenario.
	 *
	 */
	class Bola: public Actor
	{
		public:

			/**
			 * Constructor de la clase Bola.
			 * @param ruta Ruta absoluta en la tarjeta SD del arhivo XML que contiene los datos del actor Bola.
			 * @param nivel Puntero constante al nivel en el que participa el actor.
			 * @param color Color de la bola. Puede ser "verde", "rojo" o "azul"
			 * @param talla Tamaño de la bola, puede ser "xl", "l", "m" o "s".
			 * @throw Excepcion Se lanza si ocurre algún error al crear el actor.
			 */
			Bola(const std::string& ruta, const Nivel* nivel, const std::string& color, const std::string& talla)
			throw (Excepcion);

			/**
			 * Destructor de la clase Bola.
			 */
			~Bola(void);

			/**
			 * Método de actualización de la bola. Aquí se ejecuta el movimiento del actor.
			 */
			void actualizar(void);

			/**
			 * Método consultor sobre el atributo que indica el color de la bola.
			 * @return Referencia constante al color de la bola.
			 */
			const std::string& color(void) const;

			/**
			 * Método consultor sobre el atributo que indica el tamaño (talla) de la bola
			 * @return Referencia constante al tamaño de la bola.
			 */
			const std::string& talla(void) const;

			/**
			 * Método que establece la velocidad vertical de la bola a cero.
			 */
			void setVyCero(void);

			/**
			 * Método que establece la velocidad vertical de la bola a su valor inicial.
			 */
			void setVyInicial(void);

			/**
			 * Método que establece la velocidad vertical de la bola a un valor concreto.
			 * @param vy_real Nuevo valor para la vy_real de la bola.
			 */
			void setVyReal(f32 vy_real);

		private:

			std::string _color, _talla;

			const f32 _gravedad;

			f32 _vy_real;

			s16 _vy_inicial;
	};

#endif

