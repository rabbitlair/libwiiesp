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

	#include <string>
	#include <vector>
	#include "libwiiesp.h"
	#include "bola.h"
	#include "gancho.h"
	#include "personaje.h"

	class Wiipang;

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase que gestiona el escenario de un nivel.
	 *
	 * @details Esta clase derivada de Nivel se encarga de gestionar todo lo que ocurre en un escenario de WiiPang.
	 * Carga los distintos actores que participan en él (bolas y personaje), establece los estados del personaje
	 * jugador, controla el movimiento de rebote de las bolas, lanza los ganchos y detecta las colisiones entre todos
	 * los elementos y actores del escenario.
	 *
	 */
	class Escenario: public Nivel
	{
		public:

			/**
			 * Constructor de la clase Escenario. Carga el escenario diseñado con Tiled que se le indique, los actores
			 * contenidos en éste y lee algunas propiedades adicionales necesarias desde el archivo TMX del escenario.
			 * @param ruta Ruta absoluta en la tarjeta SD del archivo TMX del escenario.
			 * @param juego Puntero constante del Juego al que pertenece el escenario.
			 * @throw Excepcion se lanza si ocurre algún error al cargar el fichero TMX
			 */
			Escenario(const std::string& ruta, const Wiipang* juego) throw (Excepcion);

			/**
			 * Destructor de la clase Escenario.
			 */
			~Escenario(void);

			/**
			 * Método que actualiza el actor controlado por el jugador indicado, según el estado de su mando asociado.
			 * @param jugador Código identificador del jugador que controla el personaje.
			 * @param m Referencia constante al mando con el cual el jugador controla el personaje,
			 */
			void actualizarPj(const std::string& jugador, const Mando& m);

			/**
			 * Método que actualiza todos los actores no jugadores del nivel. 
			 */
			void actualizarNpj(void);

			/**
			 * Este método comprueba las condiciones de fin de nivel.
			 */
			void actualizarEscenario(void);

			/**
			 * Método observador sobre la bandera de fin de nivel.
			 * @return Verdadero si el nivel ha acabado ya, o falso en caso contrario.
			 */
			bool finNivel(void) const;

			/**
			 * Método observador sobre la bandera de reinicio del nivel.
			 * @return Verdadero si el nivel tiene que reiniciarse, o falso en caso contrario
			 */
			bool reiniciar(void) const;

			/**
			 * Método consultor para el límite inferior de la zona de juego.
			 * @return Valor del límite inferior de la zona de juego.
			 */
			u32 limiteInferior(void) const;

		private:

			void cargarActores(void);

			void romperBola(Bola* bola);

			u32 _x0, _x1, _y0, _y1;

			u8 _num_bolas;

			const Wiipang* _wiipang;

			std::string _xml_bola, _xml_gancho;

			bool _fin_nivel, _reiniciar;

			Personaje* _personaje;

			Gancho* _gancho1, *_gancho2;

			std::vector<Bola*> _bolas_aux;
	};

#endif

