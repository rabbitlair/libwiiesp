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

#ifndef _ESCENARIO_H_
#define _ESCENARIO_H_

	#define _USE_MATH_DEFINES
	#include <cmath>
	#include <cstdlib>
	#include <ctime>
	#include "libwiiesp.h"
	#include "pato.h"
	#include "mira.h"

	class Duckhunt;

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase que gestiona el escenario de juego.
	 *
	 * @details Un objeto de esta clase controla y gestiona los distintos aspectos del escenario de Duck Hunt. En primer
	 * lugar, se encarga de cargar los dos objetos de la clase Mira con los que participan en el juego los dos
	 * jugadores. También tiene la función de generar un pato nuevo cada poco tiempo, siendo el período de tiempo entre
	 * un pato y otro variable (de forma aleatoria, entre 0.25 y 1.5 segundos), de detectar cuándo un jugador acierta a
	 * un pato con un disparo (sumando el pato al correspondiente contador de patos abatidos), controlar los tiempos de
	 * recarga tras un disparo (0.8 segundos), y de comprobar las condiciones de fin de partida (cuando un jugador
	 * llega al objetivo de patos, en principio, el primer jugador que llega a 50 patos abatidos, gana la partida).
	 *
	 */
	class Escenario: public Nivel
	{
		public:

			/**
			 * Constructor de la clase Escenario. Carga el escenario diseñado con Tiled, los actores
			 * contenidos en éste, establece la semilla para la generación de números aleatorios, y lee algunas
			 * propiedades adicionales necesarias desde el archivo TMX del escenario.
			 * @param ruta Ruta absoluta en la tarjeta SD del archivo TMX del escenario.
			 * @param juego Puntero constante del Juego al que pertenece el escenario.
			 * @throw Excepcion se lanza si ocurre algún error al cargar el fichero TMX
			 */
			Escenario(const std::string& ruta, const Duckhunt* juego) throw (Excepcion);

			/**
			 * Destructor de la clase Escenario.
			 */
			~Escenario(void);

			/**
			 * Método que actualiza el estado de los puntos de mira en base al mando asociado a cada jugador.
			 * @param jugador Código identificador del jugador que controla el punto de mira a actualizar.
			 * @param m Referencia constante al mando con el cual el jugador controla el punto de mira.
			 */
			void actualizarPj(const std::string& jugador, const Mando& m);

			/**
			 * Método que actualiza todos los actores no jugadores del nivel. Se encarga de generar nuevos patos,
			 * eliminar los que salen de la pantalla, y de actualizar los existentes.
			 */
			void actualizarNpj(void);

			/**
			 * Este método se encarga de generar los patos y controlar el tiempo de partida.
			 */
			void actualizarEscenario(void);

			/**
			 * Método observador sobre la bandera de fin de nivel.
			 * @return Verdadero si el nivel ha acabado ya, o falso en caso contrario.
			 */
			bool finNivel(void) const;

		private:

			void cargarActores(void);

			void generarPato(void);

			const Duckhunt* _duckhunt;

			std::string _xml_pato;

			bool _fin_nivel;

			u32 _crono_patos, _intervalo_patos, _total_patos;
	};

#endif

