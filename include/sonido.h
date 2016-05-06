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

#ifndef _SONIDO_H_
#define _SONIDO_H_

	#include <asndlib.h>
	#include <fstream>
	#include <gccore.h>
	#include <malloc.h>
	#include <string>
	#include "excepcion.h"
	#include "sdcard.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que representa un efecto de sonido que se puede reproducir.
	 *
	 * @details Al igual que el resto de clases que representa a un recurso multimedia, la clase Sonido proporciona
	 * abstracción sobre las funciones y estructuras de datos necesarias para reproducir efectos de sonido en la Wii.
	 * La base sobre la que se trabaja es la libASND, que forma parte de la libogc.
	 *
	 * Cada instancia de la clase representa un efecto de sonido concreto, que puede ser reproducido en cualquier
	 * momento de la ejecución. Se crea a partir de un archivo de sonido, con un formato bastante concreto, pero que es
	 * muy fácil de obtener a partir de prácticamente cualquier archivo de audio mediante el uso de SoX, una herramienta
	 * libre, que consiste en un conversor de formatos de audio muy potente, y que se utiliza en línea de comandos de
	 * cualquier sistema UNIX (de hecho, se incluye en los repositorios de Ubuntu).
	 *
	 * El mencionado formato al que necesitamos convertir todos nuestros efectos de sonido (que no música, reitero), es
	 * PCM crudo (es decir, sólo la información del sonido, sin cabeceras de archivo), con distribución de bytes en big
	 * endian, samples de 16 bits con signo, frecuencia de 48000 Hz y 2 canales. Evidentemente, la consola puede
	 * reproducir muchos más formatos de sonido (mono y estéreo, samples de 8 y 16 bits, y frecuencias en un rango que
	 * va desde 1 Hz hasta los 144000Hz), pero se ha seleccionado éste porque, según la documentación consultada, es el
	 * formato nativo con el que trabaja la consola.
	 *
	 * Para transformar un sonido input.wav en otro con el formato explicado output.pcm, se utilizaría la siguiente
	 * orden en la consola de comandos:
	 *
	 * @code
	 * sox input.wav -V3 --encoding signed-integer --type raw --bits 16 --endian big --channels 2 --rate 48000 output.pcm
	 * @endcode
	 *
	 * Una vez finalizada la conversión, basta con copiar el archivo output.pcm a la tarjeta SD, y trabajar con la
	 * clase Sonido para tener el efecto disponible para su reproducción.
	 *
	 * Existe un detalle importante a tener en cuenta, y es relativo al sistema de sonido de la Nintendo Wii. La libASND
	 * puede mezclar 16 voces de sonido, una de las cuales está reservada para la pista de música. Esto es así porque la
	 * cantidad de instrucciones a procesar para mezclar 16 voces con la diversidad de formatos expuesta antes es muy
	 * elevada.
	 *
	 * En la libASND, una voz no es más que un flujo de datos, cuyo estado se comprueba con una interrupción de audio
	 * que es llamada cada 21,333 ms. Si hubiera entrada en una voz, ésta pasaría directamente al DSP, que es un
	 * procesador cuya única tarea es la de procesar los efectos de sonido. Con esto se consigue descongestionar al
	 * procesador principal, y obtener una mayor potencia a la hora de trabajar con el audio.
	 *
	 * Cada vez que un sonido es reproducido, se busca una voz desocupada (que no esté reproduciendo nada en ese
	 * momento), y se envía allí la secuencia de datos que componen el sonido. Evidentemente, esto quiere decir que sólo
	 * se pueden reproducir, en un mismo momento, 15 efectos de sonido y una pista de música, cantidad más que
	 * suficiente para el desarrollo de juegos en dos dimensiones. Si en algún momento, se intentaran reproducir más
	 * sonidos de los que se permiten, no ocurriría ningún error, porque existe un mecanismo que evita que haya
	 * conflictos: si no hay ninguna voz libre, el sonido no se intenta reproducir.
	 *
	 * Funcionamiento interno de la clase Sonido
	 *
	 * Esta clase es muy sencilla en su implementación, ya que únicamente carga desde un archivo alojado en la tarjeta
	 * SD un flujo de bytes, que se almacena tal cual en memoria, y que se envía a la función de reproducción cuando se
	 * le solicita. Para realizar la carga a memoria, se comprueba en primer lugar que la unidad de la tarjeta esté
	 * montada y disponible para realizar una operación de lectura. Posteriormente, se comprueba el tamaño del archivo,
	 * y se reserva la correspondiente memoria alineada a 32 bytes, con formato de enteros de 16 bits con signo (s16
	 * como tipo del puntero). En último lugar, se lee el propio archivo a la zona de memoria alineada, y se cierra el
	 * flujo de bytes.
	 *
	 * Un sonido dispondrá también de dos valores enteros de 8 bits sin signo, que representan el volumen de cada uno de
	 * los dos canales de audio. Estos dos valores se le pueden pasar al constructor junto con la ruta absoluta del
	 * archivo a cargar, y también se pueden modificar en tiempo de ejecución, de tal manera que, en ese sentido, la
	 * clase es bastante flexible.
	 *
	 * Para escuchar un sonido, se busca la primera voz para efectos que esté libre, y se vuelca en ella la información
	 * del sonido, comenzando la reproducción. Como esta clase está pensada para pequeños efectos de sonido, no se
	 * proporciona un método de parada o pausa de la reproducción, ya que se sobreentiende que un efecto de sonido dura,
	 * a lo sumo, cinco o seis segundos.
	 *
	 * Existe una función estática y pública, llamada Sonido::inicializar(), a la cual es necesario llamar antes de
	 * trabajar con la clase. Dicha función también establece que, al salir del programa, se apague el subsistema de
	 * audio de la consola.
	 *
	 * Por último, un pequeño fragmento de código para ilustrar el uso de la clase:
	 *
	 * @code
	 * // Cargar el sonido, previamente formateado con SoX, desde la SD. Volumen de los dos canales por defecto
	 * Sonido s( "/apps/wiipang/media/output.pcm" );
	 * //Modificar los volúmenes de ambos canales, dando mayor volumen al canal derecho
	 * s.setVolumenDerecho(255);
	 * s.setVolumenIzquierdo(100);
	 * // Reproducir el sonido con los valores actuales de volumen
	 * s.play();
	 * @endcode
	 *
	 */
	class Sonido
	{
		public:

			/**
			 * Constructor predeterminado de la clase Sonido.
			 * @param ruta Ruta absoluta del fichero de sonido desde el que se cargará el efecto de sonido
			 * @param volder Volumen del canal derecho
			 * @param volizq Volumen del canal izquierdo
			 * @throw ArchivoEx Se lanza si hay algún error al abrir el archivo al que apunta la ruta aportada
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			Sonido(const std::string& ruta, u8 volder = 255, u8 volizq = 255) throw (ArchivoEx, TarjetaEx);

			/**
			 * Destructor de la clase Sonido.
			 */
			~Sonido(void);

			/**
			 * Método que reproduce un sonido una sola vez, con el volumen prefijado para cada canal.
			 * Si el sonido pudiera reproducirse correctamente, se devuelve un valor True, en caso contrario,
			 * el método devuelve un valor False.
			 * @return Se devuelve verdadero si el sonido se reproduce correctamente, en caso contrario, falso.
			 */
			bool play(void) const;

			/**
			 * Función modificadora del volumen del canal izquierdo del sonido.
			 * @param volumen Nuevo valor para el volumen del canal izquierdo (entre 0 y 255).
			 */
			void setVolumenIzquierdo(u8 volumen);

			/**
			 * Función modificadora del volumen del canal derecho del sonido.
			 * @param volumen Nuevo valor para el volumen del canal derecho (entre 0 y 255).
			 */
			void setVolumenDerecho(u8 volumen);

			/**
			 * Método de clase para inicializar el sistema de sonido de la consola.
			 */
			static void inicializar(void) {
				ASND_Init();
				ASND_Pause(0);
				atexit(ASND_End);
			};

		protected:

			/**
			 * Constructor de copia de la clase Sonido. Se encuentra en la zona protegida para no permitir
			 * la copia.
			 */
			Sonido(const Sonido& m);

			/**
			 * Operador de asignación de la clase Sonido. Se encuentra en la zona protegida para no permitir
			 * la asignación.
			 */
			Sonido& operator=(const Sonido& m);

		private:
			u8 _volder, _volizq;
			s16* _sonido;
			u32 _size;
	};

#endif

