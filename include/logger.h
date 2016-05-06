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

#ifndef _LOGGER_H_
#define _LOGGER_H_

	#include <fstream>
	#include <string>
	#include "excepcion.h"
	#include "sdcard.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Sistema de registro de mensajes de información, aviso o error en un archivo de texto plano en la SD
	 *
	 * @details Esta clase pretende ser una forma sencilla pero eficaz de registrar los eventos que se
	 * deseen durante la ejecución de un programa. Al ejecutarse un programa directamente en la videoconsola, no se
	 * dispone de ninguna herramienta que permita conocer el valor de expresiones o variables en tiempo de ejecución.
	 * Logger es de ayuda cuando sucede algún comportamiento anómalo o no esperado en la ejecución, ya que permite
	 * almacenar cadenas de texto en un búffer, y guardar éste en un archivo situado en la tarjeta SD para poder
	 * consultarlo posteriormente.
	 *
	 * La clase proporciona cuatro niveles de registro, cada uno de los cuales se centra en un tipo concreto de
	 * información. Estos niveles son apagado (no se registra ningún evento) información (aquí se incluye cualquier
	 * tipo de información relevante sobre el comportamiento del sistema durante la ejecución), aviso (en este nivel se
	 * debería incluir información concreta sobre comportamientos que, sin llegar a provocar un error, no son
	 * esperados), y error (nivel de registro para almacenar mensajes sobre errores que pueden provocar la parada de la
	 * ejecución).
	 *
	 * Logger implementa un patrón Singleton, de tal manera que se tendrá una única instancia fácilmente localizable en
	 * el sistema. Además, está pensada para ser combinada con un sistema de gestión de excepciones, ya que todos los
	 * eventos e informaciones que se registran en el archivo de log son cadenas de texto de alto nivel (de tipo
	 * std::string). Esto último facilita enormemente el poder registrar los mensajes de toda excepción que disponga de
	 * un método a la usanza de what() de las excepciones estándar (toda aquella que derive de std::exception), que
	 * devuelve una cadena de texto con el mensaje de la excepción.
	 *
	 * Se debe tener en cuenta que las operaciones de lectura, y más aún las de escritura, en la tarjeta SD de la
	 * consola son muy lentas. Es por este motivo por el que se permite desactivar la función de log, mediante el nivel
	 * de registro OFF, cuando el programa no necesite información de depuración. Desactivando el registro de eventos
	 * se consigue que, al final del programa (que es cuando se produce la escritura del búffer en el archivo de log),
	 * no se sobrecargue el sistema con la operación de salida, y la ejecución pueda terminar en un intervalo de tiempo
	 * normal.
	 *
	 * Funcionamiento interno
	 *
	 * Los niveles de registro están agrupados en una enumeración que, a su vez, se define como tipo público de la
	 * clase para facilitar su utilización. El orden es desde el nivel más restrictivo (OFF, no se registra nada) a más
	 * amplio (INFO, se registra todo). Un nivel más amplio incluye a los más restrictivos que él, lo cual quiere decir
	 * que si, por ejemplo, se activara el nivel AVISO, se registrarían tanto mensajes de aviso como de error. Es por
	 * esto que se recomienda, en caso de necesitar representar los niveles como enteros, se utilice el cero para OFF,
	 * uno para ERROR, dos para AVISO y tres para INFO, con la intención de respetar la jerarquía de niveles. Por
	 * supuesto, la decisión final queda en manos del desarrollador.
	 *
	 * Cuando se inicializa el sistema de registro de eventos, se guarda el nivel de log introducido como parámetro, y
	 * si éste no es OFF (no registrar nada), se sigue adelante. Se comprueba que la tarjeta SD esté disponible para
	 * realizar operaciones de entrada/salida, se abre el archivo de log indicado en el primer parámetro en modo
	 * escritura (si no existiera, se crea), y se establece una cadena de caracteres de alto nivel como búffer para los
	 * mensajes de log.
	 *
	 * La clase irá registrando eventos en el búffer, cada uno en una línea, y comenzando por las cadenas [INFO] para
	 * el nivel de información, [AVISO] para el nivel de aviso, y [ERROR] para una cadena que contenga un mensaje con
	 * nivel de registro error. El guardado de eventos se realiza por orden de ejecución, de tal manera que cuanto más
	 * adelante se encuentre una línea en el archivo de log, será posterior en su ejecución a los eventos reflejados en
	 * líneas anteriores.
	 *
	 * Por último, hay que tener en cuenta que el volcado del búffer en el archivo se realiza cuando se destruye la
	 * instancia de la clase. La ventaja de esta decisión es que, al realizarse un único guardado, el rendimiento del
	 * programa con Logger activado no se ve afectado durante la ejecución, hecho que merece la pena aún sabiendo que,
	 * al finalizar la aplicación, habrá que esperar a que finalice la operación de salida.
	 *
	 * Ejemplo de uso
	 *
	 * @code
	 * // Inicializar Logger con nivel de registro de avisos
	 * logger->inicializar( "/apps/wiipang/info.log", Logger::AVISO );
	 * // Registrar un mensaje de error
	 * logger->error( "Esto es un mensaje de error" );
	 * // Registrar un mensaje de aviso
	 * logger->aviso( "Esto es un mensaje de aviso" );
	 * // Registrar un mensaje de información, que no se guardará en el archivo de log
	 * logger->info( "Esto es un mensaje de información, no se guardará" );
	 * @endcode
	 *
	 */
	class Logger
	{
		public:

			/**
			 * Niveles de log, desde el más amplio (INFO) hasta el más restrictivo (OFF, no se registra nada)
			 */
			typedef enum nivel_log
			{
				OFF,
				ERROR,
				AVISO,
				INFO
			} Nivel;

			/**
			 * Función estática que devuelve la instancia activa del log en el sistema. En el caso
			 * de no haber ninguna instancia, se crea y se devuelve. Implementación del patrón Singleton.
			 * @return Puntero a la instancia activa del log en el sistema
			 */
			static Logger* get_instance(void)
			{
				if(_instance == 0)
				{
					_instance = new Logger();
					atexit(destroy);
				}
				return _instance;
			}

			/**
			 * Función estática que destruye la instancia activa del log, llamando a su destructor.
			 */
			static void destroy(void)
			{
				delete _instance;
			}

			/**
			 * Método que inicializa la clase, abriendo el archivo que recibe por parámetro. La clase 
			 * sólo registra logs si el segundo parámetro es verdadero; esto se hace así para poder
			 * desactivar el modo debug de una forma fácil.
			 * @param ruta Ruta absoluta del archivo de logs en la SD
			 * @param nivel Nivel de severidad en el registro de logs
			 * @throw ArchivoEx Se lanza si hay algún error al abrir un archivo
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			void inicializar(const std::string& ruta, Nivel nivel = OFF) throw (ArchivoEx, TarjetaEx);

			/**
			 * Método consultor para conocer si está activado el modo debug
			 * @return Verdadero si el modo debug está activado, y false si no lo está
			 */
			bool debug(void) const;

			/**
			 * Método para introducir un evento de log en la categoría de información
			 * @param texto Texto que se quiere introducir en el log en la categoría de información
			 */
			void info(const std::string& texto);

			/**
			 * Método para introducir un evento de log en la categoría de aviso
			 * @param texto Texto que se quiere introducir en el log en la categoría de aviso
			 */
			void aviso(const std::string& texto);

			/**
			 * Método para introducir un evento de log en la categoría de error
			 * @param texto Texto que se quiere introducir en el log en la categoría de error
			 */
			void error(const std::string& texto);


		protected:

			/**
			 * Constructor de la clase Logger. Se encuentra en la zona protegida debido a la 
			 * implementación del patrón Singleton.
			 */
			Logger(void) { };

			/**
			 * Destructor de la clase Logger. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */			
			~Logger(void);

			/**
			 * Constructor de copia de la clase Logger. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton.
			 */
			Logger(const Logger& l);

			/**
			 * Operador de asignación de la clase Logger. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton.
			 */
			Logger& operator=(const Logger& l);


		private:

			static Logger* _instance;
			std::ofstream _archivo;
			Nivel _nivel;
			std::string _buffer;
	};

	#define logger Logger::get_instance()

#endif

