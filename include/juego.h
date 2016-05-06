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

#ifndef _JUEGO_H_
#define _JUEGO_H_

	#include <ios>
	#include <map>
	#include <sstream>
	#include <string>
	#include <vector>
	#include "excepcion.h"
	#include "galeria.h"
	#include "lang.h"
	#include "logger.h"
	#include "mando.h"
	#include "util.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que proporciona una base para definir el objeto principal del programa de manera fácil.
	 * 
	 * @details Esta clase abstracta proporciona una plantilla sobre la que construir el objeto principal de toda
	 * aplicación desarrollada con LibWiiEsp. Consiste en dos apartados bien diferenciados, que son la inicialización
	 * de todos los subsistemas de la consola y de la biblioteca (esto se realiza desde el constructor), y la ejecución
	 * del bucle principal de la aplicación. Como clase abstracta que es, no se puede instanciar directamente, si no
	 * que hay que crear a partir de ella una clase derivada en la que se definan los métodos que el programador
	 * considere oportunos para gestionar el programa.
	 *
	 * Además de la inicialización de la consola y el control del bucle principal del programa, esta clase también se
	 * encarga de gestionar la entrada de hasta cuatro mandos en la consola. Para ello, dispone de un diccionario (de
	 * tipo std::map) en el que cada jugador, que está identificado por un código único (de tipo std::string), tiene
	 * asociado un mando concreto de la consola. Para acceder al mando de un jugador, basta con buscar en el
	 * diccionario mediante el código identificativo del éste.
	 *
	 * El constructor de la clase recibe como parámetro un archivo XML con un formato concreto, en el que se deben
	 * especificar las opciones de configuración para el programa, como son el nivel de logging deseado y la ruta
	 * completa del archivo de log que se generará en el caso de estar el sistema de log activado, el color considerado
	 * transparente y que no se dibujará en la pantalla (en formato 0xRRGGBBAA, ver documentación de la clase Screen
	 * para más información), el número de fotogramas por segundo (FPS) que se quiere que tenga el videojuego, la ruta
	 * absoluta hasta el archivo XML donde se especifican los recursos multimedia que deben ser cargados en la galería
	 * de medias (más información en la descripción de la clase Galeria), la ruta absoluta hasta el archivo XML donde
	 * se especifican las etiquetas de texto del sistema de idiomas y el nombre del idioma por defecto (más detalles en
	 * la descripción de la clase Lang), y por último, la configuración de jugadores, consistente en cuatro atributos
	 * de tipo cadena de caracteres en los que se deben especificar los códigos identificadores para cada uno de los
	 * jugadores.
	 *
	 * A continuación, se muestra un ejemplo del archivo de configuración esperado:
	 *
	 * @code
	 * <?xml version="1.0" encoding="UTF-8"?>
	 * <conf>
	 *   <log valor="/apps/wiipang/info.log" nivel="3" />
	 *   <alpha valor="0xFF00FFFF" />
	 *   <fps valor="25" />
	 *   <galeria valor="/apps/wiipang/xml/galeria.xml" />
	 *   <lang valor="/apps/wiipang/xml/lang.xml" defecto="english" />
	 *   <jugadores pj1="pj1" pj2="pj2" pj3="" pj4="" />
	 * </conf>
	 * @endcode
	 *
	 * El constructor de la clase Juego, que debe ser llamado en el constructor de la correspondiente clase derivada,
	 * se encarga en primer lugar de montar la primera partición de la tarjeta SD (debe ser una partición con sistema
	 * de ficheros FAT), cargar el árbol XML del archivo de configuración en memoria mediante la clase Parser, e
	 * inicializar el sistema de logging según se haya especificado en el archivo de configuración. Si en alguna de
	 * estas operaciones se produjera un error, se saldría del programa mediante una llamada a la función exit().
	 *
	 * A partir de ese momento, el constructor inicializará los sistemas de vídeo (clase Screen), controles (clase
	 * Mando), sonido (clase Sonido) y la biblioteca FreeType de gestión de fuentes (clase Fuente). A continuación,
	 * establece el color transparente del sistema y el número de FPS, después de lo cual, leerá la configuración de
	 * los jugadores, y creará una instancia de Mando para cada jugador que, en el archivo de configuración, no tenga
	 * una cadena vacía como identificador. Posteriormente, guarda cada mando asociado al código del jugador
	 * correspondiente en el diccionario de Controles.
	 *
	 * Por último, se cargan todos los recursos media en la Galeria, y las etiquetas de texto para los idiomas del
	 * sistema en la clase Lang.
	 *
	 * Después de haber inicializado la consola partiendo del archivo de configuración, el método virtual run()
	 * proporciona una implementación básica del bucle principal del programa, en la que se controlan las posibles
	 * excepciones que puedan lanzarse (las cuales captura y almacena su mensaje en el archivo de log), se mantiene
	 * constante la tasa de fotogramas por segundo, y se actualizan todos los mandos conectados a la consola y se
	 * gestiona correctamente la actualización de los gráficos a cada fotograma. Junto a este método run(), también se
	 * proporciona un método virtual puro, llamado cargar(), y que se ejecuta antes de entrar en el bucle principal.
	 * Este método permite realizar las operaciones que se estimen necesarias antes de comenzar la ejecución del bucle,
	 * en el caso de que fueran necesarias (en caso contrario, bastaría con definir el método como una función vacía a
	 * la hora de derivar la clase Juego).
	 *
	 * Igualmente, al ser el método run() virtual, si el programador necesita otro tipo de gestión para el bucle
	 * principal de la aplicación, basta con que lo redefina en la clase derivada; a pesar de ello, tendrá disponible
	 * un sencillo ejemplo de control del bucle de la aplicación en la definición del método en la clase Juego.
	 *
	 * Por último, especificar un detalle, y es que el destructor de la clase Juego se encarga de liberar la memoria
	 * ocupada por los objetos de la clase Mando, de tal manera que no hay que preocuparse por ello.
	 * 
	 */
	class Juego
	{
		public:

			/**
			 * Constructor de la clase abstracta Juego. Carga un archivo XML con la configuración que se quiere 
			 * aplicar al programa. El archivo debe estar situado en la tarjeta SD. Después de leer el archivo XML,
			 * inicializa la consola Nintendo Wii según los parámetros establecidos en la configuración.
			 * @param ruta Ruta absoluta hasta el archivo de configuración.
			 */
			Juego(const std::string& ruta);

			/**
			 * Destructor virtual de la clase abstracta Juego. Llama a la función exit(0) después de liberar la
			 * memoria ocupada. Esto es obligado para que se disparen las funciones de la pila atexit(), y que
			 * de no ser contemplado, produciría un error en la consola.
			 */
			virtual ~Juego(void);

			/**
			 * Método que contiene el bucle principal del juego. Establece el control de tiempo para mantener los
			 * FPS constantes (según el valor leído desde el archivo de configuración), y se encarga de actualizar
			 * los mandos conectados a la consola a cada iteración. Toda la lógica del programa que deba ir en este
			 * bucle principal se debe implementar en el método virtual frame().
			 */
			virtual void run(void);

		protected:

			/**
			 * Método virtual puro en el que se debe implementar toda la lógica que se necesite ejecutar antes de que
			 * comience el bucle principal de la aplicación.
			 */
			virtual void cargar(void) = 0;

			/**
			 * Método virtual puro en el que se debe implementar toda la lógica que se necesite ejecutar durante el
			 * bucle principal de la aplicación.
			 */
			virtual bool frame(void) = 0;

			/**
			 * Estructura para almacenar los mandos conectados a la consola, asociado cada uno al código de
			 * identificación del jugador al que corresponde el mando. Si en el XML de configuración aparece
			 * pj1=jugador1", entonces se creará un mando para el primer jugador con el código "jugador1".
			 * Si en el XML, por ejemplo, aparece pj4="", entonces no se crea mando para el cuarto jugador.
			 */
			typedef std::map<std::string, Mando*> Controles;

			/**
			 * Estructura que almacena los mandos conectados a la consola.
			 */
			Controles _mandos;

			/**
			 * Tasa de fotogramas por segundo que tendrá la aplicación.
			 */
			u8 _fps;
	};

#endif

