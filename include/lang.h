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

#ifndef _LANG_H_
#define _LANG_H_

	#include <map>
	#include <set>
	#include <string>
	#include "excepcion.h"
	#include "parser.h"
	#include "util.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Soporte para múltiples idiomas en la misma aplicación, basado en un archivo de idiomas en formato XML.
	 * 
	 * @details Esta clase proporciona soporte de idiomas en cualquier aplicación que se desarrolle para Nintendo Wii,
	 * pero también es reutilizable en cualquier aplicación de C++, ya que no tiene dependencia del sistema de la
	 * consola. Implementa un patrón Singleton, de tal manera que el soporte de idiomas está disponible en cualquier
	 * momento y en cualquier parte del sistema.
	 *
	 * La idea sobre la que se trabaja es que cada vez que se quiera escribir un texto en el programa, en lugar de
	 * ponerlo directamente desde el código fuente, se asignará una etiqueta de texto. Dicha etiqueta de texto será
	 * sustituida, en tiempo de ejecución, por la cadena de caracteres asociada a ella, en el idioma que esté marcado
	 * como activo en ese momento en la clase Lang. Así, el texto que se muestra en una etiqueta determinada cambiará
	 * dinámicamente según se active un idioma u otro.
	 *
	 * Los idiomas, las etiquetas y el contenido de cada una de éstas en cada uno de los idiomas se especifican desde
	 * un archivo de datos XML, con un formato concreto. A continuación se muestra un ejemplo de XML válido para el
	 * soporte de idiomas:
	 *
	 * @code
	 * <?xml version="1.0" encoding="UTF-8"?>
	 * <lang>
	 *     <idioma nombre="español">
	 *         <tag nombre="MENU1" valor="Jugar" />
	 *         <tag nombre="MENU2" valor="Puntuación" />
	 *         <tag nombre="MENU3" valor="Salir" />
	 *     </idioma>
	 *     <idioma nombre="english">
	 *         <tag nombre="MENU1" valor="Play" />
	 *         <tag nombre="MENU2" valor="Ranking" />
	 *         <tag nombre="MENU3" valor="Exit" />
	 *     </idioma>
	 * </lang>
	 * @endcode
	 *
	 * Se trabaja con cadenas de caracteres de alto nivel que utilizan como base caracteres anchos (en C/C++, el tipo
	 * wchar_t, que almacena caracteres de 1, 2 ó 4 bytes); así se consigue que cualquier carácter Unicode esté
	 * soportado. Cada idioma viene identificado por su nombre (que es, a su vez, una cadena de caracteres anchos), y
	 * consiste en un diccionario de etiquetas de texto. Una etiqueta de texto tiene dos componentes, que son su código
	 * identificativo (una cadena de caracteres de alto nivel) y el texto que se sustituirá en la etiqueta en el idioma
	 * correspondiente. Es muy importante que cada etiqueta tenga su correspondencia en cada uno de los idiomas
	 * soportados, ya que la clase Lang asume que si una etiqueta aparece en un idioma, aparecerá en todos los demás.
	 *
	 * Funcionamiento interno
	 *
	 * La estructura de datos diseñada para almacenar un idioma consiste en un diccionario (en C++, un std::map), donde
	 * su primera componente es una cadena de caracteres de alto nivel (de tipo std::string) que almacena el nombre de
	 * una etiqueta de texto (y mediante la cual se identifica la etiqueta), y su segunda componente es otra cadena de
	 * caracteres de alto nivel, pero de caracteres anchos (de tipo std::wstring), en la cual se guarda el texto
	 * asociado a la etiqueta traducido al idioma correspondiente.
	 *
	 * Cada idioma se almacena en otro diccionario en la segunda componente de éste, identificado por su nombre (cadena
	 * de caracteres de tipo std::wstring) en la primera componente del diccionario.
	 *
	 * Cuando se inicializa el soporte de idiomas, se le pasa como parámetro una cadena de caracteres con la ruta
	 * absoluta en la tarjeta SD del archivo de idiomas anteriormente descrito. Este archivo se carga en memoria y se
	 * recorre, creando cada idioma que se encuentre, y guardando cada etiqueta de texto asociada a él.
	 *
	 * Al leer el contenido de una etiqueta, éste se almacena temporalmente en una std::string. Antes de guardarlo en
	 * el diccionario del idioma correspondiente, se transforma a UTF32, mediante una función definida en la cabecera
	 * util.h. La explicación de esta transformación es sencilla: cuando se lee un texto con caracteres anchos (que
	 * necesita un espacio mayor que 1 byte), cada carácter ancho se ‘rompe’ en trozos de 1 byte cada uno. Un carácter
	 * ASCII normal ocupará el espacio esperado, es decir, 1 byte. Lo que ocurre dentro de la función antes mencionada
	 * es que se crea una cadena de caracteres anchos (de tipo wchar_t), y se rellena con el contenido leído desde el
	 * archivo después de pasarlo por la función de C llamada mbstowcs. Esta función se encarga de detectar los
	 * caracteres anchos que están ‘rotos’ en trozos de 1 byte, los une, y devuelve toda la cadena recibida como una
	 * cadena de caracteres anchos. Por último, se almacena lo devuelto en una std::wstring, y se introduce en el
	 * idioma correspondiente.
	 *
	 * Otra funcionalidad que ofrece esta clase consiste en activar un idioma concreto mediante su nombre, conocer cuál
	 * está activo en un momento determinado, saber si un idioma concreto existe, y alternar entre todos los idiomas
	 * registrados a través de una función que va activando cada uno según se lo encuentre al recorrer el diccionario.
	 * Este último método obtiene un iterador al idioma activo actual dentro del diccionario, y lo avanza una posición.
	 * Si no se ha llegado al final del diccionario, se activa el idioma encontrado; en caso contrario, se activa el
	 * primero del diccionario.
	 *
	 * Por último, para obtener el texto asociado a una etiqueta concreta en el idioma activo actual, existe una
	 * función que, dado el nombre de la etiqueta mediante un std::string, devuelve el texto asociado a ésta mediante
	 * un std::wstring, el cual se puede dibujar en pantalla (si la fuente seleccionada para ello tiene soporte para el
	 * juego de caracteres necesario), guardar en un fichero, o utilizar en cualquier operación deseada.
	 *
	 * Ejemplo de uso
	 *
	 * @code
	 * // Inicializar el soporte de idiomas mediante el archivo y el idioma activo por defecto (ruso)
	 * lang->inicializar( "/apps/wiipang/xml/lang.xml", "english" );
	 *
	 * // Escribir en pantalla el texto de una etiqueta (ver clase Fuente y clase Galeria
	 * // Se escribiría en pantalla ' Play '
	 * galeria->fuente( "arial" )->escribir( lang->texto( "MENU1" ), 30, 100, 100, 5, 0xFF0000FF );
	 *
	 * // Cambiar el idioma activo a 'español'
	 * lang->activarIdioma( "español" );
	 *
	 * // Escribir la misma etiqueta de texto que antes
	 * // Se escribiría en pantalla ' Jugar '
	 * galeria->fuente( "arial" )->escribir( lang->texto( "MENU1" ), 30, 100, 100, 5, 0xFF0000FF );
	 * @endcode
	 * 
	 */
	class Lang
	{
		public:

			/**
			 * Función estática que devuelve la instancia activa del soporte de idiomas en el sistema. En el caso
			 * de no haber ninguna instancia, se crea y se devuelve. Implementación del patrón Singleton.
			 * @return Puntero a la instancia activa del soporte de idiomas en el sistema
			 */
			static Lang* get_instance(void)
			{
				if(_instance == 0)
				{
					_instance = new Lang();
					atexit(destroy);
				}
				return _instance;
			}

			/**
			 * Función estática que destruye la instancia activa del soporte de idiomas, llamando a su destructor.
			 */
			static void destroy(void)
			{
				delete _instance;
			}

			/**
			 * Método para inicializar el soporte de idiomas
			 * @param ruta Archivo XML con la información del soporte de idiomas
			 * @param idioma_activo Nombre del idioma que estará activo en el sistema desde el principio
			 * @throw ArchivoEx Se lanza si hay algún error al abrir un archivo
			 * @throw CodigoEx Se lanza si se intenta activar un idioma que no existe
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 * @throw XmlEx Se lanza si hay un error relacionado con un árbol XML
			 */
			void inicializar(const std::string& ruta, const std::wstring& idioma_activo)
			throw (ArchivoEx, CodigoEx, TarjetaEx, XmlEx);

			/**
		 	 * Activa un idioma para que, a partir de este momento, se utilice en todas las etiquetas del sistema.
			 * @param idioma Nombre del idioma que se quiere utilizar
			 * @throw CodigoEx Se lanza si el nombre del idioma no corresponde a ninguno existente
			 */
			void activarIdioma(const std::wstring& idioma) throw (CodigoEx);

			/**
			 * Función para conocer el idioma que actualmente está activo en el sistema.
			 * @return Nombre del idioma que está activo en el sistema
			 */
			const std::wstring& idiomaActivo(void) const;

			/**
			 * Función consultora que indica si existe un idioma concreto en el sistema.
			 * @param idioma Nombre del idioma que se desea saber si existe en el sistema.
			 * @return Verdadero si el idioma existe, o falso en caso contrario.
			 */
			bool existeIdioma(const std::wstring& idioma) const;

			/**
			 * Método que va cambiando el idioma activo de forma circular entre los idiomas disponibles
			 */
			void cambiarIdioma(void);

			/**
			 * Función que devuelve el texto, en el idioma activo, asociado a la etiqueta que se recibe.
			 * @param tag Etiqueta de la cual se quiere obtener el texto asociado en el idioma activo.
			 * @return Texto, en el idioma activo del sistema, asociado con la etiqueta que se recibe
			 * @throw CodigoEx Se lanza si la etiqueta solicitada no existe en el idioma activo
			 */
			const std::wstring& texto(const std::string& tag) throw (CodigoEx);

		protected:

			/**
			 * Constructor de la clase Lang. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			Lang(void) { };

			/**
			 * Destructor de la clase Lang. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			~Lang(void);

			/**
			 * Constructor de copia de la clase Lang. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			Lang(const Lang& l);

			/**
			 * Operador de asignación de la clase Lang. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */
			Lang& operator=(const Lang& l);

		private:

			static Lang* _instance;
			typedef std::map<std::string, std::wstring> Idioma;
			std::map<std::wstring, Idioma*> _idiomas;
			std::wstring _idioma_activo;

	};

	#define lang Lang::get_instance()

#endif

