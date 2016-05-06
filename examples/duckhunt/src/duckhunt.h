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

#ifndef _DUCKHUNT_H_
#define _DUCKHUNT_H_

	#include <string>
	#include "libwiiesp.h"
	#include "escenario.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase principal de Duck Hunt.
	 *
	 * @details Esta clase deriva de la clase abstracta Juego, y se encarga de inicializar la consola a partir de un
	 * archivo XML de configuración que recibe en su constructor. Gestiona las pantallas que deben mostrarse al jugador,
	 * así como los estados de los mandos, las puntuaciones de los jugadores y el bucle principal de la aplicación.
	 *
	 */
	class Duckhunt: public Juego
	{
		public:

			/**
			 * Constructor de la clase Duckhunt. Inicializa la Nintendo Wii a partir de un archivo XML de configuración.
			 * @param ruta Ruta absoluta en la tarjeta SD del archivo XML de configuración.
			 */
			Duckhunt(const std::string& ruta);

			/**
			 * Destructor de la clase Duckhunt.
			 */
			~Duckhunt(void);

			/**
			 * Metodo que pausa el juego, mostrando el cartel de pausa en el idioma activo del sistema. Sólo puede
			 * reanudar el juego el mismo jugador que lo pausó.
			 * @param jugador Código identificador del jugador que pausó el juego.
			 * @return Si es verdadero, se sale del juego, en caso contrario, se reanuda el juego.
			 */
			bool pausa(const std::string& jugador);

			/**
			 * Metodo que muestra la pantalla de presentación del juego. Sólo puede iniciar el juego un jugador.
			 * @param jugador Código identificador del jugador que puede iniciar el juego.
			 */
			void presentacion(const std::string& jugador);

			/**
			 * Metodo que dibuja el menú del juego en la pantalla. El menú incluye información sobre la cantidad
			 * de patos abatidos por parte de cada jugador.
			 */
			void dibujarMenu(void) const;

			/**
			 * Método que muestra la pantalla de fin del juego, cuando un jugador llega al objetivo de patos cazados
			 * @param jugador Número del jugador que gana la partida
			 */
			void finJuego(const u8 jugador);

			/**
			 * Metodo consultor para el número de patos abatidos por el jugador 1
			 * @return Número de puntatos abatidos por el jugador 1
			 */
			u32 patosJ1(void) const;

			/**
			 * Metodo consultor para el número de patos abatidos por el jugador 2
			 * @return Número de puntatos abatidos por el jugador 2
			 */
			u32 patosJ2(void) const;

			/**
			 * Metodo modificador para el número de patos abatidos por el jugador 1
			 * @return Referencia al número de patos abatidos por el jugador 1
			 */
			u32& patosJ1(void);

			/**
			 * Metodo modificador para el número de patos abatidos por el jugador 2
			 * @return Referencia al número de patos abatidos por el jugador 2
			 */
			u32& patosJ2(void);

		private:

			void cargar(void);

			bool frame(void);

			u32 _patos_j1, _patos_j2, _tiempo;

			Escenario* _escenario;
	};

#endif

