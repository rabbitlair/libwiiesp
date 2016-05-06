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

#ifndef _SDCARD_H_
#define _SDCARD_H_

	#include <cstdio>
	#include <cstdlib>
	#include <fat.h>
	#include <sdcard/wiisd_io.h>
	#include <string>
	#include <unistd.h>

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que gestiona la ranura para tarjetas SD de la consola Wii
	 *
	 * @details Esta clase implementa un patrón Singleton, debido a que la consola Nintendo Wii sólo dispone de una
	 * ranura lectora de tarjetas SD. Además, para desarrollar juegos y aplicaciones relativamente sencillas en dos
	 * dimensiones, sólo se necesita acceder a un único dispositivo de almacenamiento masivo, por lo que se ha decidido
	 * no desarrollar más allá que esta clase.
	 *
	 * Es cierto que la libogc puede permitir acceder a dispositivos USB de almacenamiento (comúnmente conocidos como
	 * "lápices usb"), pero se ha descartado implementar esta funcionalidad en la biblioteca, tanto por simplificar su
	 * utilización, como por ahorro de tiempo. Está planeado, en un futuro, implementar soporte para dispositivos USB.
	 *
	 * Siguiendo con la norma de la sencillez, la clase Sdcard sólo puede montar una única partición. El formato en el
	 * que debe estar dicha partición es FAT o FAT32, ya que la biblioteca auxiliar que utiliza libogc para montar
	 * unidades y particiones es una implementación para Wii de libfat.
	 *
	 * Funcionamiento interno
	 *
	 * La mecánica interna de esta clase se basa en realizar diez intentos de montar la primera partición de la tarjeta
	 * SD insertada en la ranura de la consola. Si fallaran los diez intentos, la clase sale del programa con un código
	 * de error 1.
	 *
	 * El proceso que se sigue al intentar montar una partición consiste en inicializar el hardware del lector de
	 * tarjetas. Si esto se consigue, se intenta montar, con un sistema de archivos FAT o FAT32,  la primera partición
	 * válida de la tarjeta SD. Si no se consigue, se apaga el hardware de la ranura, se contabiliza el intento, y se
	 * prueba de nuevo con el mismo proceso.
	 *
	 * Hay que tener en cuenta que una tarjeta puede tener varias particiones, y éstas pueden ser de distintos sistemas
	 * de archivos. Pues bien, esta clase espera que la primera partición esté formateada como FAT/FAT32, porque es la
	 * única a la que puede acceder. Así pues, queda bajo la responsabilidad del usuario la correcta preparación de la
	 * SD para poder utilizar esta clase.
	 *
	 * Utilización de la clase
	 *
	 * Cuando todo sale bien, se monta la partición con el nombre de unidad que se recibe por parámetro en el método
	 * inicializador, y se pone a disposición del resto del sistema una serie de funciones para conocer el nombre de
	 * esta unidad, saber si la partición está montada o no, y para desmontarla en cualquier momento. Si se desmonta
	 * (por el motivo que sea) la unidad, se puede volver a utilizar inicializándola de nuevo.
	 *
	 * Generalmente, esta clase se inicia al principio del programa, y se desmonta al final, ya que si se utiliza
	 * también la clase Logger (consultar documentación de dicha clase), ésta realiza la escritura al final de la
	 * ejecución.
	 *
	 * Para realizar una operación de lectura y/o escritura, basta con anteponer el nombre de la unidad, seguida de dos
	 * puntos (:), y después la dirección absoluta del recurso a cargar/modificar en formato UNIX (por ejemplo,
	 * string( sdcard->unidad() + ':' + '/apps/wiipang/xml/media.xml' ) sería la forma de abrir el archivo media.xml).
	 *
	 * Unos últimos detalles. La propia implementación del patrón Singleton se encarga de que se destruya la
	 * instancia activa de la clase en el sistema al salir del programa mediante la instrucción exit().También hay que
	 * tener en cuenta que las clases que encapsulan recursos media (como la clase Imagen, o la clase Musica) requieren
	 * que Sdcard haya montado previamente la unidad FAT/FAT32 sobre la que se van a realizar las operaciones de entrada
	 * y salida.
	 *
	 * Pequeño ejemplo de uso:
	 *
	 * @code
	 *	// Inicializar el lector de tarjetas, y montar una unidad llamada 'SD'
	 * 	sdcard->inicializar("SD");
	 *
	 * // Guardar en un std::string el nombre de la unidad
	 * std::string unidad = sdcard->unidad();
	 *
	 * // Saber si está montada la partición; si lo está desmontarla
	 * if ( sdcard->montada() )
	 * 		sdcard->desmontar();
	 * @endcode
	 *
	 */
	class Sdcard
	{
		public:

			/**
			 * Método de clase que devuelve la dirección de memoria de la instancia activa en el sistema
			 * de la clase Sdcard. Si aún no existe dicha instancia, la crea y la devuelve. Además, establece
			 * que al salir del programa, se destruya por medio de una llamada al destructor.
			 */
			static Sdcard* get_instance(void)
			{
				if(_instance == 0)
				{
					_instance = new Sdcard();
					atexit(destroy);
				}
				return _instance;
			}

			/**
			 * Método de clase que limpia la memoria ocupada por la instancia activa en el sistema
			 * de la clase Sdcard, llamando a su destructor.
			 */
			static void destroy(void)
			{
				delete _instance;
			}

			/**
			 * Método que inicializa la clase. Intenta montar la primera partición válida de la tarjeta SD
			 * insertada en la consola, utilizando un sistema de archivos FAT/FAT32. Se realizan diez intentos,
			 * si no se ha conseguido un montaje correcto tras dichos intentos, se sale de programa con código
			 * de error 1.
			 * @param unidad Nombre de unidad que se le asignará a la partición montada.
			 */
			void inicializar(const std::string& unidad = "SD");

			/**
			 * Método consultor para el nombre de la unidad asignado a la partición montada
			 * @return Nombre de la unidad asignado a la partición montada
			 */
			const std::string& unidad(void) const;

			/**
			 * Método consultor para saber si la unidad está montada o no.
			 * @return Devuelve verdadero si la unidad está montada, o falso en caso contrario.
			 */
			bool montada(void) const;

			/**
			 * Método que desmonta la partición de la unidad. Si no estuviera montada, no hace nada.
			 */
			void desmontar(void);

			/**
			 * Método que comprueba si un archivo existe en la tarjeta SD.
			 * @param archivo Ruta absoluta del archivo que se quiere comprobar.
			 */
			bool existe(const std::string& archivo);


		protected:

			/**
			 * Constructor de la clase Sdcard. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			Sdcard(void) { };

			/**
			 * Destructor de la clase Sdcard. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton. Desmonta la partición si estuviera montada.
			 */
			~Sdcard(void);

			/**
			 * Constructor de copia de la clase Sdcard. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			Sdcard(const Sdcard& l);

			/**
			 * Operador de asignación de la clase Sdcard. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			Sdcard& operator=(const Sdcard& l);

		private:

			static Sdcard* _instance;
			u16 _montada;
			std::string _unidad;
	};

	#define sdcard Sdcard::get_instance()

#endif

