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

#ifndef _MUSICA_H_
#define _MUSICA_H_

	#include <fstream>
	#include <gccore.h>
	#include <malloc.h>
	#include <mp3player.h>
	#include <string>
	#include "excepcion.h"
	#include "sdcard.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que representa una pista de música que puede ser reproducida en la consola Nintendo Wii
	 *
	 * @details Esta clase, como todas las clases que representan recursos media, proporciona una abstracción de las
	 * funciones y estructuras de datos necesarias para utilizar dichos recursos, en este caso, la clase Musica permite
	 * cargar y reproducir archivos de audio en formato MP3, y está pensada para las pistas de música. La biblioteca de
	 * bajo nivel que utiliza esta clase para poder utilizar dichas pistas es un port de libmad.
	 *
	 * Cada instancia de la clase representa una pista de música, independiente de todas las demás, y preparada para
	 * ser reproducida en cualquier punto del programa. Se crea partir de un archivo de audio en formato MP3, cargando
	 * éste desde la tarjeta SD de la consola. En principio, cualquier formato (frecuencia, bitrate) de MP3 puede ser
	 * utilizado con esta clase, pero se recomienda reducir el bitrate a 128 kbps y mantener la frecuencia a 44100 Hz,
	 * así como no almacenar demasiadas pistas a la vez en memoria, ya que la memoria de la que dispone la consola es
	 * limitada (64 megas), y cada pista se carga tal cual mediante un flujo de entrada desde ficheros (con lo que el
	 * consumo de espacio es bastante alto).
	 *
	 * En la descripción de la clase Sonido puede obtenerse más información sobre cómo trabaja el subsistema de sonido
	 * de la consola. A partir de ahí, se sabe que existe una voz (un flujo de datos que se pasan directamente al
	 * procesador de sonido, el DSP) reservada exclusivamente para pistas de música. La implementación de libmad sobre
	 * la que se trabaja con la clase Musica hace uso de dicha voz, de tal manera que sólo se puede reproducir una
	 * única pista de música a la vez. Por otro lado, es seguro que no se interferirá con la reproducción de un efecto
	 * de sonido encapsulado en una instancia de la clase Sonido. Además, al ser un port, no hay que preocuparse de
	 * detalles como el Endian al leer, ya que las funciones de libmad están preparadas para encargarse de dichos
	 * detalles.
	 *
	 * Al reproducir una pista, por defecto sólo se reproduce una vez. De hecho, libmad no da soporte a la reproducción
	 * ininterrumpida como tal. Si se quiere mantener una pista en reproducción infinita (hasta que se detenga
	 * explícitamente o se salga del programa) hay que indicarlo al utilizar la función play() mediante su parámetro
	 * booleano (hay que especificarlo en cada reproducción; por defecto, se toma el valor false), y llamar a loop() en
	 * cada iteración del bucle principal (este método se encarga de iniciar la reproducción de nuevo cuando se acabe
	 * la pista en reproducción, pero sólo lo hace si se indicó reproducción continua al iniciarla).
	 *
	 * Funcionamiento interno
	 *
	 * Esta clase consiste únicamente en un flujo de bytes (la propia pista de música), atributos simples (tamaño,
	 * indicador de reproducción infinita y volumen) y la interfaz de la libmad.
	 *
	 * A la hora de crear una instancia de la clase, lo primero que se hace es comprobar que la tarjeta SD está montada
	 * y lista para ser utilizada. En caso de no ser así, se lanza una excepción indicando el error. Posteriormente, se
	 * abre el archivo que se recibe como parámetro mediante la ruta absoluta de éste en la tarjeta. Hay otro parámetro
	 * opcional, un entero de 8 bits sin signo (u8), que representa el volumen de la pista de audio (siendo 0 el
	 * volumen mínimo, y 255 el máximo), y que se puede modificar en tiempo de ejecución mediante el método modificador
	 * setVolumen(). El archivo MP3 se abre mediante un flujo de entrada de bytes ifstream, y se lee en bloque sobre
	 * una zona de memoria reservada previamente, de tipo puntero entero de 16 bits con signo (s16), alineada a 32
	 * bytes, y convenientemente ajustada a un tamaño múltiplo de 32 bytes también. Si todo va bien, se cierra el flujo
	 * de entrada, y queda lista la pista de sonido para ser reproducida.
	 *
	 * Para iniciar la reproducción de la pista, basta con llamar al método play() de la instancia, indicando si se
	 * quiere reproducir una vez, o de forma ininterrumpida (tal y como se ha explicado previamente). Este método se
	 * encarga de parar cualquier reproducción que haya en curso, establecer el volumen de la nueva reproducción y
	 * enviar el flujo de bytes que compone la pista de audio a la voz reservada para música, realizándose estas dos
	 * últimas tareas a través de la interfaz que proporciona libmad.
	 *
	 * En cualquier momento se puede detener la reproducción de una pista de música llamando al método stop(), y
	 * también conocer si se está reproduciendo una pista de música o no mediante el método reproduciendo(). Estas dos
	 * funciones también trabajan directamente con la interfaz de libmad.
	 *
	 * Por último, el destructor se encarga de detener la reproducción y de liberar la memoria ocupada por la pista de
	 * música.
	 *
	 * Hay que mencionar que esta clase tiene una "limitación" importante, y es que si se está reproduciendo una pista
	 * de audio A, y se realiza una llamada al método stop() de una instancia B, la reproducción de A se detendría.
	 * Este comportamiento es intencionado, ya que interesa que, si se está reproduciendo una pista al iniciar la
	 * reproducción de otra distinta, la primera se detenga para poder iniciar la nueva.
	 *
	 * Ejemplo de uso
	 *
	 * @code
	 *	// Crear una instancia a partir de un archivo MP3
	 *	Musica rock( "/apps/wiipang/media/rock.mp3", 255 );
	 *	// Iniciar la reproducción
	 *	rock.play();
	 *	// Detener la reproducción, en el caso de que se esté reproduciendo
	 *	if ( rock.reproduciendo() )
	 *		rock.stop();
	 *	// Iniciar una reproducción infinita
	 *	rock.play( true );
	 *	// Mantener la reproducción infinita en cada iteración del bucle principal
	 *	while( 1 ) {
	 *		// ...
	 *		rock.loop();
	 *		// ...
	 *	}
	 * @endcode
	 *
	 */
	class Musica
	{
		public:

			/**
			 * Constructor predeterminado de la clase Musica.
			 * @param ruta Ruta absoluta del fichero MP3 desde el que se cargará la pista de música
			 * @param volumen Volumen de la pista de sonido
			 * @throw ArchivoEx Se lanza si hay algún error al abrir el archivo al que apunta la ruta aportada
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			Musica(const std::string& ruta, u8 volumen = 255) throw (ArchivoEx, TarjetaEx);

			/**
			 * Destructor de la clase Musica.
			 */
			~Musica(void);

			/**
			 * Método que reproduce una pista de música, con el volumen prefijado.
			 */
			void play(void) const;

			/**
			 * Método que detiene la reproducción de una pista de música.
			 */
			void stop(void) const;

			/**
			 * Método que mantiene la reproducción infinita de la pista de música. Se debe llamar dentro del bucle
			 * principal de la aplicación para mantener la pista reproduciéndose de forma contínua. Si no se quiere
			 * reproducir infinitamente, basta con no llamar a este método.
			 */
			void loop(void) const;

			/**
			 * Método que indica si se está reproduciendo la pista actualmente.
			 * @return Verdadero si se está reproduciedo la pista, o falso en caso contrario.
			 */
			bool reproduciendo(void) const;

			/**
			 * Función modificadora del volumen de la pista de música.
			 * @param volumen Nuevo valor para el volumen de la pista de música (entre 0 y 255)
			 */
			void setVolumen(u8 volumen);

		protected:

			/**
			 * Constructor de copia de la clase Musica. Se encuentra en la zona protegida para no permitir
			 * la copia.
			 */
			Musica(const Musica& m);

			/**
			 * Operador de asignación de la clase Musica. Se encuentra en la zona protegida para no permitir
			 * la asignación.
			 */
			Musica& operator=(const Musica& m);

		private:
			bool _loop;
			u8 _volumen;
			s16* _musica;
			u32 _size;
	};

#endif

