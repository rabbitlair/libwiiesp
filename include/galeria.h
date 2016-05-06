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

#ifndef _GALERIA_H_
#define _GALERIA_H_

	#include <iterator>
	#include <map>
	#include <string>
	#include "excepcion.h"
	#include "fuente.h"
	#include "imagen.h"
	#include "musica.h"
	#include "parser.h"
	#include "sonido.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que gestiona los recursos media del sistema, y facilita su carga en memoria y el acceso a ellos.
	 * 
	 * @details Esta clase consiste en un conjunto de diccionarios (en C++, maps), uno por cada tipo de recurso media
	 * que se vaya a utilizar en el desarrollo del juego. La carga de éstos se realiza al principio de la ejecución a
	 * partir de un archivo XML con un formato concreto. Además, esta clase implementa un patrón Singleton, de tal
	 * manera que los recursos media están disponibles en todo momento, y en cualquier parte del sistema.
	 *
	 * En estos momentos, los cuatro recursos que se contemplan son: imágenes, sonidos, pistas de música y fuentes de
	 * texto. Cada uno de estos recursos se encuentra disponible mediante un código único de identificación, en formato
	 * std::string, y que se lee desde el ya mencionado archivo de datos XML. Si en un futuro se decidiera implementar
	 * un nuevo tipo de recurso, basta con implementar su correspondiente diccionario, la función de lectura desde el
	 * XML, y las funciones consultoras.
	 *
	 * Dos recursos de distinto tipo pueden tener identificadores idénticos, ya que se archivan cada uno en su
	 * diccionario.Por otra parte, los recursos se crean mediante su constructor, y con los parámetros que cada uno de
	 * ellos espera (como mínimo, un recurso debe tener su código identificativo y la ruta hasta el archivo que
	 * contiene el media que se va a cargar). Por ejemplo, si se va a cargar un sonido, los parámetros que se esperan
	 * son el código (en formato std::string), la ruta absoluta hasta el archivo situado en la tarjeta SD (también en
	 * formato std::string), y los valores numéricos (enteros de 8 bits sin signo) que indican el volumen de cada uno
	 * de los dos canales de reproducción.
	 *
	 * Funcionamiento interno
	 *
	 * Los diccionarios sobre los que trabaja esta clase consisten en parejas formadas por una cadena de caracteres de
	 * alto nivel (del tipo ya mencionado, std::string), que actúa como clave identificativa, y un puntero a la zona de
	 * memoria donde se encuentra el recurso correspondiente. Cuando se solicita a la clase un recurso media, esta
	 * operación se hace a partir del código del media. El método comprueba que el código recibido corresponde a un
	 * recurso, lo busca y devuelve el puntero que apunta a él; si el código no estuviera registrado en el diccionario,
	 * se lanzaría una excepción.
	 *
	 * La clase necesita que la tarjeta SD esté montada para poder acceder a los archivos que contienen los recursos;
	 * en caso contrario, se lanzará una excepción al intentar acceder a éstos.
	 *
	 * A la hora de inicializar la galería de recursos, se le debe pasar como parámetro la ruta absoluta (en la tarjeta
	 * SD) hasta el archivo XML en el que se encuentra la información de los media. Este archivo debe tener un nodo
	 * raíz cuyos hijos serán, cada uno de ellos, un recurso media. El método que se encarga de recorrer la información
	 * e ir creando cada recurso a partir de ésta irá leyendo cada etiqueta de recurso, la identificará por su tipo, y
	 * según sea éste, intentará leer unos u otros atributos de la etiqueta. A continuación se muestra un ejemplo de
	 * etiqueta válida para cada uno de los cuatro tipos de recursos contemplados en estos momentos por la clase:
	 *
	 * @code
	 * <imagen codigo="fondo" formato="bmp" ruta="/apps/wiipang/media/fondo.bmp" />
	 * <musica codigo="rock" volumen="128" ruta="/apps/wiipang/media/rock.mp3" />
	 * <sonido codigo="sound" volumen="255" ruta="/apps/wiipang/media/sound.pcm" />
	 * <fuente codigo="arial" ruta="/apps/wiipang/media/arial.ttf" />
	 * @endcode
	 *
	 * Añadir un nuevo recurso media al sistema es tan sencillo como copiar el archivo a la tarjeta SD (se recomienda
	 * mantener una carpeta donde se almacenen todos los recursos, por ejemplo, una llamada 'media'), y después añadir
	 * la etiqueta correspondiente al archivo que contenga la información de la galería. De esta forma se evita tener
	 * que volver a compilar el proyecto en caso de querer añadir o modificar un recurso, consiguiendo así una
	 * separación de código y datos.
	 *
	 * Por último, al crear la galería, se establece que, cuando se salga del programa, se llame a su destructor que, a
	 * su vez, llama a los destructores de cada uno de los recursos almacenados, de tal manera que se evitan fugas de
	 * memoria.
	 *
	 * Ejemplo de uso
	 * @code
	 * // Inicialización de la galería a partir de un XML
	 * galeria->inicializar("/apps/wiipang/xml/galeria.xml");
	 * // Ejemplo de acceso a cada uno de los recursos
	 * galeria->sonido("sound")->play();
	 * galeria->fuente("arial")->escribir("¡Funciona!", 30, 80, 100, 10, 0x00FF00FF);
	 * galeria->imagen("fondo")->dibujar(0, 0, 900);
	 * galeria->musica("rock")->play();
	 * @endcode
	 * 
	 */
	class Galeria
	{
		public:

			/**
			 * Función estática que devuelve la instancia activa de la galeria de medias en el sistema. En el caso
			 * de no haber ninguna instancia, se crea y se devuelve. Implementación del patrón Singleton.
			 * @return Puntero a la instancia activa de la galeria de medias en el sistema
			 */
			static Galeria* get_instance(void)
			{
				if(_instance == 0)
				{
					_instance = new Galeria();
					atexit(destroy);
				}
				return _instance;
			}

			/**
			 * Función estática que destruye la instancia activa de la galeria de medias, llamando a su destructor.
			 */
			static void destroy(void)
			{
				delete _instance;
			}

			/**
			 * Método para inicializar la galería, y cargar los medias en la biblioteca de medias.
			 * @param ruta Ruta absoluta hasta el archivo XML en el que se encuentra la información de los media
			 * @throw ArchivoEx Se lanza si hay algún error al abrir un archivo
			 * @throw ImagenEx Se lanza si sucede un error relacionado con la carga de una imagen
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 * @throw XmlEx Se lanza si hay un error relacionado con un árbol XML
			 */
			void inicializar(const std::string& ruta) throw (ArchivoEx, ImagenEx, TarjetaEx, XmlEx);

			/**
			 * Método que devuelve un puntero a la imagen que tenga el código que se indica
			 * @param codigo Código de la imagen que se quiere obtener
			 * @return Puntero a la imagen que corresponde al código introducido
			 * @throw CodigoEx Se lanza si el código recibido no corresponde a ninguna imagen
			 */
			const Imagen& imagen(const std::string& codigo) throw (CodigoEx);

			/**
			 * Método que devuelve un puntero a la pista de música que tenga el código que se indica
			 * @param codigo Código de la pista de música que se quiere obtener
			 * @return Puntero a la pista de música que corresponde al código introducido
			 * @throw CodigoEx Se lanza si el código recibido no corresponde a ninguna pista de música
			 */
			const Musica& musica(const std::string& codigo) throw (CodigoEx);

			/**
			 * Método que devuelve un puntero al sonido que tenga el código que se indica
			 * @param codigo Código del sonido que se quiere obtener
			 * @return Puntero al sonido que corresponde al código introducido
			 * @throw CodigoEx Se lanza si el código recibido no corresponde a ningún sonido
			 */
			const Sonido& sonido(const std::string& codigo) throw (CodigoEx);

			/**
			 * Método que devuelve un puntero a la fuente que tenga el código que se indica
			 * @param codigo Código de la fuente que se quiere obtener
			 * @return Puntero a la fuente que corresponde al código introducido
			 * @throw CodigoEx Se lanza si el código recibido no corresponde a ninguna fuente de texto
			 */
			const Fuente& fuente(const std::string& codigo) throw (CodigoEx);

		protected:

			/**
			 * Constructor de la clase Galeria. Se encuentra en la zona protegida debido a la 
			 * implementación del patrón Singleton.
			 */
			Galeria(void) { };

			/**
			 * Destructor de la clase Galeria. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */			
			~Galeria(void);

			/**
			 * Constructor de copia de la clase Galeria. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton.
			 */
			Galeria(const Galeria& g);

			/**
			 * Operador de asignación de la clase Galeria. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton.
			 */
			Galeria& operator=(const Galeria& g);

		private:

			void leerImagen(TiXmlElement* nodo) throw (ArchivoEx, ImagenEx, TarjetaEx, XmlEx);
			void leerMusica(TiXmlElement* nodo) throw (ArchivoEx, TarjetaEx, XmlEx);
			void leerSonido(TiXmlElement* nodo) throw (ArchivoEx, TarjetaEx, XmlEx);
			void leerFuente(TiXmlElement* nodo) throw (ArchivoEx, TarjetaEx, XmlEx);

			static Galeria* _instance;
			std::map<std::string, Imagen*> _map_imagen;
			std::map<std::string, Musica*> _map_musica;
			std::map<std::string, Sonido*> _map_sonido;
			std::map<std::string, Fuente*> _map_fuente;
	};

	#define galeria Galeria::get_instance()

#endif

