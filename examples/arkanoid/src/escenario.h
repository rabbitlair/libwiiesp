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
	#include "bola.h"
	#include "pala.h"
	#include "ladrillo.h"
	#include "item.h"

	class Arkanoid;

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.1
	 * @brief Clase que gestiona el escenario de un nivel.
	 *
	 * @details Esta clase se encarga de gestionar todos los detalles relativos a un nivel dentro de Arkanoid. Almacena
	 * la pala y la bola, el conjunto de todos los ladrillos, los posibles items que vayan apareciendo, y los tiles que
	 * componen el propio escenario.
	 *
	 * En los métodos correspondientes, se actualizan todos los actores que participan en el juego, y se realizan todos
	 * los cálculos necesarios para el buen funcionamiento de éste. Se considera que acaba el nivel cuando no quedan
	 * ladrillos, o se han perdido todas las vidas del jugador.
	 *
	 * El menú se dibuja a la derecha de la pantalla, de tal manera que se espera que la zona de juego se establezca
	 * respetando las medidas de los niveles que se aportan como ejemplo. Los métodos de control que se contemplan son
	 * la pulsación de los botones de la cruceta del mando (BOTON_ARRIBA y BOTON_ABAJO) o el valor del ángulo de cabeceo
	 * del mando (ambos métodos consideran que se coge el mando en horizontal).
	 *
	 */
	class Escenario: public Nivel
	{
		public:

			/**
			 * Enumeración que contiene los dos posibles métodos de control de la pala: utilizando los botones de la
			 * cruceta del mando, o bien mediante el valor del ángulo de cabeceo del mando (al estilo del Mario Kart de
			 * Nintendo)
			 */
			typedef enum
			{
				BOTONES,
				CABECEO
			} Control;

			/**
			 * Constructor de la clase Escenario. Carga el escenario diseñado con Tiled que se le indique, los actores
			 * contenidos en éste, establece la semilla para la generación de números aleatorios, y lee algunas
			 * propiedades adicionales necesarias desde el archivo TMX del escenario.
			 * @param ruta Ruta absoluta en la tarjeta SD del archivo TMX del escenario.
			 * @param juego Puntero constante del Juego al que pertenece el escenario.
			 * @throw Excepcion se lanza si ocurre algún error al cargar el fichero TMX
			 */
			Escenario(const std::string& ruta, const Arkanoid* juego) throw (Excepcion);

			/**
			 * Destructor de la clase Escenario.
			 */
			~Escenario(void);

			/**
			 * Método que actualiza el estado de la pala según las pulsaciones de botones en el mando correspondiente al
			 * jugador que se indique. Al pulsar el botón 2 se lanza una bola si no existe ya en el juego, y, según el
			 * sistema de control seleccionado, se comprueba si hay que mover la pala o no, moviéndola en caso necesario.
			 * @param jugador Código identificador del jugador que controla la pala.
			 * @param m Referencia constante al mando con el cual el jugador controla la pala.
			 */
			void actualizarPj(const std::string& jugador, const Mando& m);

			/**
			 * Método que actualiza todos los actores no jugadores del nivel. Calcula los rebotes de la bola al
			 * colisionar ésta con el escenario, los ladrillos o la pala, la mueve cuando sea necesario, elimina los
			 * ladrillos que colisionen con la bola, genera los items y los aplica en caso de que sea necesario.
			 */
			void actualizarNpj(void);

			/**
			 * Este método comprueba las condiciones de fin de nivel, y el final de los efectos de los items.
			 */
			void actualizarEscenario(void);

			/**
			 * Método observador sobre la bandera de fin de nivel.
			 * @return Verdadero si el nivel ha acabado ya, o falso en caso contrario.
			 */
			bool finNivel(void) const;

			/**
			 * Método que cambia el sistema de control seleccionado por el usuario.
			 */
			void cambiarControl(void);

			/**
			 * Método que genera un nuevo item, con una probabilidad del 10%. El item aparece en las mismas coordenadas
			 * que el ladrillo que se recibe por parámetro.
			 * @param lad Puntero constante al ladrillo que hace aparecer el item.
			 */
			void generarItem(const Ladrillo* lad);

		private:

			void cargarActores(void);

			u32 _x0, _x1, _y0, _y1, _num_ladrillos, _multiplicador, _crono;

			Pala* _pala;

			Bola* _bola;

			const Arkanoid* _arkanoid;

			std::string _xml_bola, _xml_item;

			Control _control;

			bool _fin_nivel, _item_activo;
	};

#endif

