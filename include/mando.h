//
// Licencia GPLv3
//
// Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
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

#ifndef _MANDO_H_
#define _MANDO_H_

	#include <iterator>
	#include <map>
	#include <unistd.h>
	#include <valarray>
	#include <wiiuse/wpad.h>
	#include "excepcion.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que permite leer información desde un Wii Remote.
	 *
	 * @details Clase que representa al Wii Remote, permitiendo acceder en cada momento al estado de pulsación de los
	 * botones, la situación del puntero infrarrojo mediante sus coordenadas, y el estado de la palanca de la
	 * expansión Nunchuck.
	 *
	 * Funcionamiento interno
	 *
	 * La libogc proporciona una estructura (definida como WPADData), en la cual se plasma toda la información relativa
	 * a todos los mandos cuando se llama a la función WPAD_ScanPads(). Importante: Una llamada a WPAD_ScanPads()
	 * actualiza la información de todos los mandos, así que hay que tener cuidado de llamarla sólo una vez por frame
	 * (a menos que se le quiera dar otro uso distinto al habitual de 'leer la entrada una vez por frame y actualizar el
	 * mundo del juego en consecuencia'). En un mismo frame, hay que llamar una vez a WPAD_ScanPads() para actualizar
	 * la estructura WPADData, y después actualizar cada mando por separado (mapeo de botones, estado del puntero
	 * infrarrojo, etc.)
	 *
	 * Cada mando conectado a la consola se identifica en esta estructura WPADData con un número denominado chan, que
	 * tiene los valores desde 0 a 3, y que es único para cada mando. Importante: La libogc sólo tiene acceso a los
	 * mandos que tienen sincronización permanente, con la consola; es decir, que los mandos que se vayan a utilizar no
	 * deben estar conectados a la consola en modo invitado (para más información sobre la sincronización permanente y
	 * el modo invitado, consultar el manual de la propia consola Wii). Además, podemos saber la extensión que tiene
	 * conectada el mando (Nunchuck, mando clásico, o incluso la guitarra del Guitar Hero 3) mediante la función
	 * WPAD_Probe(u32 chan, u32* type); se guarda la información en type.
	 *
	 * El mapeado de los botones se realiza aparte, haciendo uso de dos valarrays booleanos, uno de los cuales
	 * representa las pulsaciones de todos los botones en el momento actual, y el otro, las pulsaciones en el momento
	 * anterior. De esta manera, en cualquier frame podemos saber si un botón está siendo  pulsado, si se acaba de
	 * soltar, o si lo acabamos de pulsar. En realidad, la detección de las pulsaciones se hace llamando a la función
	 * WPAD_ButtonsDown(u32 chan), y almacenando el resultado en una variable u32 llamada _pulsado. Dicho resultado es
	 * un entero de 32 bits, donde cada bit indica una pulsación de un botón concreto,  y si se compara mediante un
	 * operador AND de bits (&) con los valores de cada botón que proporciona libogc, nos dirá si dicho botón ha sido
	 * pulsado. Así pues, si para cada botón, comparamos su valor conocido con _pulsado y lo almacenamos en su lugar
	 * correspondiente en el valarray de pulsaciones, tendremos un mapeo completo de todos los botones del mando.
	 *
	 * Un detalle más, muy importante, y es que cada vez que se crea una instancia de la clase, se toma como chan el
	 * número actual de mandos activos (es decir, si ya tengo dos mandos, uno con chan 0 y otro con chan 1, la nueva
	 * instancia tendrá, sí o sí, el chan 2). Hay que tener cuidado de no crear indiscriminadamente instancias de Mando,
	 * porque sólo habrá una instancia en todo el sistema con un chan determinado; y un mando, una vez inicializado con
	 * un chan, no puede modificarlo. Es decir, que si se crea una instancia de Mando para controlar el primer mando,
	 * hay que utilizar esa instancia en todo el sistema, nada de crear una instancia en otro sitio para intentar
	 * controlar el primer mando.
	 *
	 * Uso de la clase
	 *
	 * Antes de utilizar una instancia de Mando, hay que inicializar la clase, esto se hace con la llamada
	 *
	 * @code
	 * // Inicializar la clase Mando
	 * Mando::inicializar(u16 ancho, u16 alto);
	 * @endcode
	 *
	 * donde los parámetros son el alto y ancho de la pantalla en píxeles.
	 *
	 * La mayoría de los métodos que componen la clase Mando no son más que métodos observadores sobre el estado
	 * descrito las estructuras de datos internas, y que se describen por sí solos en la documentación que acompaña
	 * a la clase.
	 *
	 * Métodos de orientación del mando
	 *
	 * Hay tres métodos observadores (cabeceo, viraje y rotacion), que devuelven un valor entre 180 y -180. Estos
	 * valores representan el ángulo de giro respecto a tres ejes (x, y, z) del mando, suponiendo que el (0,0,0)
	 * se da cuando el mando está apuntando perpendicularmente a la pantalla, y con los botones hacia arriba.
	 *
	 * Para más información sobre estos tres métodos, consultar su descripción.
	 *
	 * Ejemplos de uso:
	 * 
	 * @code
	 * // Actualizar los dos mandos:
	 * Mando::leerDatos();
	 * m1.actualizar();
	 * m2.actualizar();
	 *
	 * // Saber las coordenadas en pantalla del puntero infrarrojo del mando 1:
	 * m1.punteroX();
	 * m1.punteroY();
	 *
	 * // Saber si el segundo mando ha pulsado el botón A:
	 * if ( m2.newPressed( Mando::BOTON_A ) )
	 * // ... Hacer algo
	 *
	 * // Hacer vibrar el primer mando durante 300 milisegundos:
	 * m1.vibrar( 300000 );
	 *
	 * // ¿Tiene el Nunchuck del segundo mando la palanca pulsada hacia la derecha?
	 * if ( m2.nunPalancaEstadoX() == Mando::PALANCA_DERECHA )
	 * // ... Hacer algo
	 * @endcode
	 * 
	 */
	class Mando
	{
		public:

			/**
			 * Enumeración en la que cada elemento representa uno de los botones mapeados del Wii Remote
			 */
			typedef enum boton_str {
				BOTON_A,
				BOTON_B,
				BOTON_C,
				BOTON_Z,
				BOTON_1,
				BOTON_2,
				BOTON_MAS,
				BOTON_MENOS,
				BOTON_HOME,
				BOTON_ARRIBA,
				BOTON_ABAJO,
				BOTON_DERECHA,
				BOTON_IZQUIERDA
			} Boton;

			/**
			 * Enumeración que representa los estados horizontales de la palanca del Nunchuck
			 */
			typedef enum nunchuck_x_str {
				PALANCA_CENTRO_X,
				PALANCA_DERECHA,
				PALANCA_IZQUIERDA
			} Nunchuck_x;

			/**
			 * Enumeración que representa los estados verticales de la palanca del Nunchuck
			 */
			typedef enum nunchuck_y_str {
				PALANCA_CENTRO_Y,
				PALANCA_ARRIBA,
				PALANCA_ABAJO
			} Nunchuck_y;

			/**
			 * Constructor predeterminado de la clase Mando.
			 */
			Mando(void);

			/**
			 * Destructor de la clase Mando.
			 */
			~Mando(void);

			/**
			 * Método de clase para leer los datos de estado de todos los Wii Remotes conectados. Debe llamarse 
			 * una única vez por cada frame.
			 */
			static void leerDatos(void);

			/**
			 * Método de clase para inicializar el sistema de bluetooth de los mandos.
			 * @param ancho_pantalla Ancho en píxeles de la pantalla
			 * @param alto_pantalla Alto en píxeles de la pantalla
			 */
			static void inicializar(u16 ancho_pantalla, u16 alto_pantalla) {
				WPAD_Init();
				WPAD_SetVRes(WPAD_CHAN_ALL, ancho_pantalla, alto_pantalla);
				WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
				WPAD_SetIdleTimeout(60);
				atexit(WPAD_Shutdown);
			};

			/**
			 * Método observador para saber si el Wii Remote está conectado o no.
			 * @return True si el mando está conectado, o False si no lo está
			 */
			bool conectado(void) const;

			/**
			 * Método observador para conocer el mando concreto (de 0 a 3) con el que se está trabajando.
			 * @return Número de mando (de 0 a 3) con el que se está trabajando en la instancia.
			 */
			u8 chan(void) const;

			/**
			 * Actualiza el estado del Wii Remote. Mapea los botones pulsados (conservando el estado anterior,
			 * para detectar nuevas pulsaciones, o botones soltados), los acelerómetros y el puntero infrarrojo.
			 */
			void actualizar(void);


			// Funciones relativas a los botones

			/**
			 * Método observador para saber si un botón está pulsado en un momento determinado.
			 * @param boton Botón que se desea saber si está pulsado o no.
			 * @return Devuelve True si el botón está pulsado, o False en caso contrario.
			 */
			bool pressed(Boton boton) const;

			/**
			 * Método observador para saber si un botón ha sido soltado en un momento determinado.
			 * @param boton Botón que se desea saber si ha dejado de estar pulsado o no.
			 * @return Devuelve True si el botón se ha soltado, o False en caso contrario.
			 */
			bool released(Boton boton) const;

			/**
			 * Método observador para saber si un botón ha sido pulsado en un momento determinado.
			 * @param boton Botón que se desea saber si ha sido pulsado o no.
			 * @return Devuelve True si el botón se acaba de pulsar, o False en caso contrario.
			 */
			bool newPressed(Boton boton) const;


			// Funciones relativas al puntero infrarrojo

			/**
			 * Método observador para la coordenada X del puntero infrarrojo del Wii Remote. La coordenada X
			 * tiene su posición cero en la izquierda de la pantalla, y aumenta a medida que se acerca a la
			 * derecha de la pantalla.
			 * @return Coordenada X, en píxeles, del puntero infrarrojo del Wii Remote
			 */
			s16 punteroX(void) const;

			/**
			 * Método observador para la coordenada Y del puntero infrarrojo del Wii Remote. La coordenada Y
			 * tiene su posición cero en la parte superior de la pantalla, y aumenta a medida que se acerca 
			 * a la parte inferior de la pantalla.
			 * @return Coordenada Y, en píxeles, del puntero infrarrojo del Wii Remote
			 */
			s16 punteroY(void) const;

			/**
			 * Método observador que permite conocer si el puntero infrarrojo del Wii Remote se encuentra 
			 * dentro de los límites de la pantalla.
			 * @return True si el puntero infrarrojo está dentro de la pantalla; en caso contrario, False
			 */
			bool punteroEnPantalla(void) const;


			// Funciones relativas a la vibración

			/**
			 * Método que hace vibrar el Wii Remote durante una cantidad concreta de tiempo.
			 * @param tiempo Tiempo, en microsegundos, que se quiere hacer vibrar el Wii Remote
			 */
			void vibrar(u16 tiempo);


			// Funciones relativas a la orientación

			/**
			 * Valor del cabeceo del Wii Remote. El cabeceo del mando es el ángulo que forma éste
			 * con un plano horizontal perpendicular a la pantalla, si se le hace girar sobre el eje X. 
			 * El ángulo es positivo si la punta del mando queda por encima de la parte trasera, y 
			 * negativo en caso contrario.
			 * @return Valor del ángulo de cabeceo del Wii Remote.
			 */
			f32 cabeceo(void) const;

			/**
			 * Valor del viraje del Wii Remote. El viraje del mando es el ángulo que forma éste con
			 * un plano vertical perpendicular a la pantalla, si se le hace girar sobre el eje Z.
			 * El ángulo es positivo si la punta del mando queda a la derecha de la parte trasera, y
			 * negativo en caso contrario.
			 * @return Valor del ángulo de viraje del Wii Remote.
			 */
			f32 viraje(void) const;

			/**
			 * Valor de la rotación del Wii Remote. La rotación del mando es un ángulo que mide el
			 * giro de éste sobre el eje Y (como si estuviera dando una vuelta de campana). El ángulo
			 * es positivo si la rotación es hacia la derecha, y negativo en caso contrario.
			 * @return Valor del ángulo de rotación del Wii Remote.
			 */
			f32 rotacion(void) const;


			// Funciones relativas al Nunchuck

			/**
			 * Método observador que indica si el Nunchuck está conectado al Wii Remote.
			 * @return True si el Nunchuck está conectado, o False si no lo está.
			 */
			bool nunConectado(void) const;

			/**
			 * Método observador para conocer el estado básico horizontal de la palanca del Nunchuck. Este 
			 * método devuelve PALANCA_CENTRADA, PALANCA_IZQUIERDA o PALANCA_DERECHA, dependiendo del estado 
			 * de pulsación horizontal de la palanca.
			 * Si se necesita un control más exacto de los valores de la coordenada X de la palanca del Nunchuck,
			 * es conveniente utilizar los métodos 'nunPalancaValorX' y 'nunPalancaCentroX'
			 * @return Estado básico horizontal de la palanca del Nunchuck
			 * @throw NunchuckEx Se lanza si el Nunchuck no está conectado al WiiRemote
			 */
			Nunchuck_x nunPalancaEstadoX(void) const throw (NunchuckEx);

			/**
			 * Método observador para conocer el estado básico vertical de la palanca del Nunchuck. Este 
			 * método devuelve PALANCA_CENTRADA, PALANCA_ARRIBA o PALANCA_ABAJO, dependiendo del estado 
			 * de pulsación vertical de la palanca.
			 * Si se necesita un control más exacto de los valores de la coordenada Y de la palanca del Nunchuck,
			 * es conveniente utilizar los métodos 'nunPalancaValorY' y 'nunPalancaCentroY'
			 * @return Estado básico vertical de la palanca del Nunchuck
			 * @throw NunchuckEx Se lanza si el Nunchuck no está conectado al WiiRemote
			 */
			Nunchuck_y nunPalancaEstadoY(void) const throw (NunchuckEx);

			/**
			 * Método observador para el valor de la coordenada horizontal de la palanca del Nunchuck.
			 * Si este valor es superior al valor central, se considera que la palanca está siendo pulsada
			 * hacia la derecha, en caso contrario, está siendo pulsada hacia la izquierda.
			 * @return Valor de la coordenada horizontal de la palanca del Nunchuck
			 * @throw NunchuckEx Se lanza si el Nunchuck no está conectado al WiiRemote
			 */
			u16 nunPalancaValorX(void) const throw (NunchuckEx);

			/**
			 * Método observador para el valor de la coordenada vertical de la palanca del Nunchuck.
			 * Si este valor es superior al valor central, se considera que la palanca está siendo pulsada
			 * hacia abajo, en caso contrario, está siendo pulsada hacia arriba.
			 * @return Valor de la coordenada vertical de la palanca del Nunchuck
			 * @throw NunchuckEx Se lanza si el Nunchuck no está conectado al WiiRemote
			 */
			u16 nunPalancaValorY(void) const throw (NunchuckEx);

			/**
			 * Método observador para el valor central horizontal de la palanca del Nunchuck. Este valor es
			 * constante, pero la palanca suelta (supuestamente centrada) no siempre devuelve el mismo valor,
			 * por lo que es recomendable dar un margen de +/- 15 a la hora de utilizar este valor.
			 * @return Valor central de la coordenada X de la palanca del Nunchuck.
			 * @throw NunchuckEx Se lanza si el Nunchuck no está conectado al WiiRemote
			 */
			u16 nunPalancaCentroX(void) const throw (NunchuckEx);

			/**
			 * Método observador para el valor central vertical de la palanca del Nunchuck. Este valor es
			 * constante, pero la palanca suelta (supuestamente centrada) no siempre devuelve el mismo valor,
			 * por lo que es recomendable dar un margen de +/- 15 a la hora de utilizar este valor.
			 * @return Valor central de la coordenada Y de la palanca del Nunchuck.
			 * @throw NunchuckEx Se lanza si el Nunchuck no está conectado al WiiRemote
			 */
			u16 nunPalancaCentroY(void) const throw (NunchuckEx);

		private:

			WPADData* _wmote_datas;
			std::valarray<bool> _actual_mando;
			std::valarray<bool> _old_mando;
			u32 _type;
			u8 _num_botones, _chan;
			std::map<Boton, u32> _botones;

			// Número de mandos conectados a la aplicación
			static u8 mandos_activos;

			/**
			 * Constructor de copia de la clase Mando. Se encuentra en la zona privada para evitar la copia.
			 */
			Mando(const Mando& m);

			/**
			 * Operador de asignación de la clase Mando. Se encuentra en la zona privada para evitar la copia.
			 */
			Mando& operator=(const Mando& m);

	};

#endif

