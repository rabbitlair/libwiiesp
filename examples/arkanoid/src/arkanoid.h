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

#ifndef _ARKANOID_H_
#define _ARKANOID_H_

	#include <string>
	#include <vector>
	#include "libwiiesp.h"
	#include "escenario.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase principal de Arkanoid.
	 *
	 * @details Esta clase deriva de la clase abstracta Juego, y se encarga de inicializar la consola a partir de un
	 * archivo XML de configuración que recibe en su constructor. Gestiona las pantallas que deben mostrarse al jugador,
	 * así como los estados del mando, el nivel a cargar en cada momento, la cantidad de puntos y vidas, y el bucle
	 * principal de la aplicación.
	 *
	 */
	class Arkanoid: public Juego
	{
		public:

			/**
			 * Constructor de la clase Arkanoid. Inicializa la Nintendo Wii a partir de un archivo XML de configuración.
			 * @param ruta Ruta absoluta en la tarjeta SD del archivo XML de configuración.
			 */
			Arkanoid(const std::string& ruta);

			/**
			 * Destructor de la clase Arkanoid.
			 */
			~Arkanoid(void);

			/**
			 * Método que dibuja el menú del juego a la derecha de la pantalla.
			 */
			void dibujarMenu(void) const;

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
			 * Metodo que muestra la pantalla de nivel completado. Sólo puede avanzar al siguiente nivel un jugador.
			 * @param jugador Código identificador del jugador que puede avanzar el juego.
			 */
			void nivelCompletado(const std::string& jugador);

			/**
			 * Metodo que muestra la pantalla de fin del juego. Sólo puede salir del juego un jugador.
			 * @param jugador Código identificador del jugador que puede salir del juego a partir de esta pantalla.
			 */
			void finJuego(const std::string& jugador);

			/**
			 * Metodo consultor para el número de vidas que le quedan al jugador.
			 * @return Número de vidas que le quedan al jugador.
			 */
			u32 vidas(void) const;

			/**
			 * Metodo consultor para el número de puntos que tiene acumulados el jugador.
			 * @return Número de puntos que tiene acumulados el jugador.
			 */
			u32 puntos(void) const;

			/**
			 * Metodo consultor para el número de nivel actual.
			 * @return Número de nivel actual.
			 */
			u32 nivel(void) const;

			/**
			 * Metodo modificador para el número de vidas que le quedan al jugador.
			 * @return Referencia al número de vidas que le quedan al jugador.
			 */
			u32& vidas(void);

			/**
			 * Metodo modificador para el número de puntos que tiene acumulados el jugador.
			 * @return Referencia al número de puntos que tiene acumulados el jugador.
			 */
			u32& puntos(void);

			/**
			 * Metodo modificador para el número de nivel actual.
			 * @return Referencia al número de nivel actual.
			 */
			u32& nivel(void);

		private:

			void cargar(void);

			bool frame(void);

			Escenario* _escenario;

			std::vector<std::string> _niveles;

			u32 _puntos, _vidas, _nivel;
	};

#endif

