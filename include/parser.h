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

#ifndef _PARSER_H_
#define _PARSER_H_

	#include <gctypes.h>
	#include <string>
	#include <tinyxml.h>
	#include "excepcion.h"
	#include "sdcard.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que facilita la lectura de información desde archivos XML situados en la tarjeta SD de la consola
	 *
	 * @details La clase Parser no es más que una interfaz sencilla y directa para la lectura de información desde
	 * archivos XML situados en la tarjeta SD de la consola Nintendo Wii. Abstrae las estructuras de datos y funciones
	 * de la biblioteca externa TinyXML, de tal manera que no hay que preocuparse por las comprobaciones y posibles
	 * errores que puedan surgir en el proceso de lectura. Implementa un patrón Singleton, de tal manera que es
	 * accesible desde cualquier punto del programa, y sólo existe una instancia en el sistema.
	 *
	 * Comentar que no hay que preocuparse por los detalles de BigEndian, reserva de memoria alineada a 32 bytes y
	 * demás, porque la versión de TinyXML que se utiliza esta portada a Nintendo Wii, lo cual significa que ya
	 * contempla de base estos asuntos.
	 *
	 * Este parser funciona cargando el árbol del documento por completo en memoria, y después realizando búsquedas y
	 * lecturas sobre éste, lo cual es una implementación de DOM (Document Object Model, o modelo de objetos del
	 * documento en la lengua de Cervantes), que es una interfaz estándar para manipular y acceder a un archivo XML. Al
	 * trabajar con árboles, se dispone de un elemento raíz en el documento, a partir del cual van desplegándose
	 * elementos hijos, y pueden aplicarse todas las técnicas y algoritmos relativos a árboles. El documento XML, una
	 * vez cargado en memoria, se encontrará almacenado dentro de la propia clase hasta que, o bien se destruya ésta, o
	 * se cargue un nuevo árbol XML.
	 *
	 * Derivado de esto último, es importante tener en cuenta que si se está trabajando con un árbol XML, no se debe
	 * cargar otro nuevo hasta haber finalizado con el primero, ya que el cargar un segundo árbol implica la
	 * destrucción del existente, y por tanto, para volver a operar sobre el original, es necesario cargarlo otra vez.
	 * Sin embargo, al cargar por segunda vez un mismo archivo XML, las variables que apuntaban a elementos del primer
	 * árbol quedan apuntando a zonas de memoria liberadas; de ahí el cuidado necesario al trabajar con varios archivos
	 * XML.
	 *
	 * Funcionamiento interno
	 *
	 * La clase únicamente tiene como atributo privado un documento XML de tipo TiXmlDocument, que viene de serie con
	 * la biblioteca TinyXML (la definición del tipo, no el atributo). Cuando se realiza una llamada al método cargar,
	 * éste recibe como parámetro una cadena de caracteres de alto nivel (de tipo std::string) que indica la ruta
	 * absoluta hasta el archivo XML del cual se quiere cargar en memoria su árbol de elementos. Primero comprueba que
	 * la tarjeta SD está preparada para trabajar, y después de eso, intenta cargar el árbol completo en el atributo
	 * privado que almacenará el documento. Si algo fallara, se lanzaría una excepción con un texto descriptivo sobre
	 * el error. Un detalle a mencionar es que la codificación de caracteres con la que se intenta abrir el archivo XML
	 * es UTF8.
	 *
	 * Una vez se ha abierto correctamente el archivo, y se ha cargado el árbol de elementos en memoria, hay varias
	 * posibilidades para extraer información de éste. En primer lugar, se puede acceder al elemento raíz del árbol
	 * mediante el método raiz. También, dado un elemento del árbol, se puede extraer el contenido de éste (si lo
	 * tuviera) mediante el método contenido, que lo devuelve en forma de std::string. Si no existiera contenido en el
	 * elemento, se devuelve una cadena vacía.
	 *
	 * Para acceder a los atributos de un elemento concreto, puede hacerse mediante los métodos atributo (que intenta
	 * devolver el valor del atributo como una cadena de caracteres de alto nivel; si no existe el atributo o el
	 * elemento, devuelve una cadena vacía), atributoU32 y atributoF32 (que funcionan de la misma manera, sólo que
	 * devolviendo el valor como un entero de 32 bits sin signo, o un decimal de coma flotante de 32 bits,
	 * respectivamente. Si no existieran el elemento o el atributo, se devuelve el valor cero).
	 *
	 * La navegación por los elementos del árbol es sencilla, basta con llamar al método buscar con el valor del
	 * elemento que se quiere localizar. Se puede especificar otro elemento a partir del cual se comenzará la búsqueda,
	 * que se realiza recursivamente en preorden, y devuelve la primera ocurrencia de elemento que coincida con el
	 * valor indicado. Si no se determina un elemento como raíz de la búsqueda, se toma el elemento raíz del árbol como
	 * inicio. Por último, si no se localizara ningún elemento coincidente en valor con el deseado, se devuelve el
	 * valor NULL.
	 *
	 * Otra manera de localizar un elemento es, a partir de uno dado, buscar el siguiente elemento hermano con el mismo
	 * valor. Se considera que un elemento es hermano de otro si tienen el mismo padre (elemento inmediatamente
	 * superior en el árbol). El siguiente hermano con el mismo valor es aquel elemento que, estando entre los hermanos
	 * del elemento original, tiene el mismo valor, y está situado a la derecha del primero. El método siguiente
	 * intenta localizar el hermano inmediatamente consecutivo con el mismo valor a un elemento dado, y es útil para
	 * evitar bucles que recorran todos los hijos de un elemento dado, en el caso de que se estén buscando sólo los
	 * hijos que tengan un valor determinado. Al igual que en la búsqueda, si no se localizara un elemento que coincida
	 * con los criterios del método, se devuelve el valor NULL.
	 *
	 * Ejemplo de uso
	 *
	 * @code
	 * // Cargar un archivo XML
	 * parser->cargar( "/apps/wiipang/xml/galeria.xml" );
	 * // Buscar el primer elemento con valor 'musica' a partir del elemento raíz
	 * TiXmlElement* musica = parser->buscar( 'musica', parser->raiz() );
	 * // Leer un atributo llamado codigo, con formato cadena de caracteres
	 * std::string ruta = parser->atributo( "codigo", musica );
	 * // Leer un atributo llamado volumen, con formato de entero de 32 bits sin signo
	 * u32 volumen = parser->atributo( "volumen", musica );
	 * // Buscar el primer hermano de 'musica' con el mismo valor ("musica" también)
	 * TiXmlElement* otra_musica = parser->siguiente( musica );
	 * @endcode
	 *
	 */
	class Parser
	{
		public:

			/**
			 * Función estática que devuelve la instancia activa del parser XML en el sistema. En el caso
			 * de no haber ninguna instancia, se crea y se devuelve. Implementación del patrón Singleton.
			 * @return Puntero a la instancia activa del parser XML en el sistema
			 */
			static Parser* get_instance(void)
			{
				if(_instance == 0)
				{
					_instance = new Parser();
					atexit(destroy);
				}
				return _instance;
			}

			/**
			 * Función estática que destruye la instancia activa del parser XML, llamando a su destructor.
			 */
			static void destroy(void)
			{
				delete _instance;
			}

			/**
			 * Método que carga en memoria un archivo XML que esté almacenado en la tarjeta SD. El archivo queda
			 * listo para ser accedido en cualquier momento, y desde cualquier parte del programa (gracias a la
			 * implementación del patrón Singleton).
			 * @param ruta Ruta absoluta hasta el archivo XML a cargar, debe estar en la tajeta SD
			 * @throw ArchivoEx Se lanza si hay algún error al abrir el archivo al que apunta la ruta aportada
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			void cargar(const std::string& ruta) throw (ArchivoEx, TarjetaEx);

			/**
			 * Método que devuelve el elemento raíz del árbol XML del archivo que esté cargado en memoria.
			 * @return Elemento raíz del árbol XML del archivo que está actualmente cargado
			 */
			TiXmlElement* raiz(void);

			/**
			 * Método que devuelve el texto contenido en un elemento XML que se recibe como parámetro. Si el
			 * elemento no existe, o no tiene contenido, se devuelve una cadena vacía.
			 * @param el Elemento XML del árbol del cual se quiere conocer su contenido
			 * @return Cadena de texto con el contenido del elemento recibido
			 */
			std::string contenido(const TiXmlElement* el) const;
		
			/**
			 * Método que devuelve el texto contenido en un atributo de un elemento XML que se recibe como 
			 * parámetro. Si el elemento o el atributo no existen, se devuelve una cadena vacía.
			 * @param nombre Nombre del atributo cuyo valor se quiere conocer
			 * @param el Elemento XML del árbol del cual se quiere conocer el valor de un atributo
			 * @return Cadena de texto con el contenido del atributo solicitado
			 */
			std::string atributo(const std::string& nombre, const TiXmlElement* el) const;

			/**
			 * Método que devuelve el contenido de un atributo de un elemento XML que se recibe como 
			 * parámetro, en formato entero de 32 bits sin signo. Si el elemento o el atributo no existen, 
			 * se devuelve el valor 0.
			 * @param nombre Nombre del atributo cuyo valor se quiere conocer
			 * @param el Elemento XML del árbol del cual se quiere conocer el valor de un atributo
			 * @return Entero de 32 bits sin signo con el contenido del atributo solicitado
			 */
			u32 atributoU32(const std::string& nombre, const TiXmlElement* el) const;

			/**
			 * Método que devuelve el contenido de un atributo de un elemento XML que se recibe como 
			 * parámetro, en formato de coma flotante de 32 bits sin signo. Si el elemento o el atributo 
			 * no existen, se devuelve el valor 0.0.
			 * @param nombre Nombre del atributo cuyo valor se quiere conocer
			 * @param el Elemento XML del árbol del cual se quiere conocer el valor de un atributo
			 * @return Número decimal en coma flotante de 32 bits con el contenido del atributo solicitado
			 */
			f32 atributoF32(const std::string& nombre, const TiXmlElement* el) const;

			/**
			 * Método que busca un elemento cuyo valor sea el que se recibe por parámetro. La búsqueda es recursiva
			 * entre todos los hijos del elemento que se reciba (si no se recibe ninguno, la búsqueda comienza
			 * en el elemento raíz del árbol XML). Si no se encuentra el elemento solicitado, se devuelve NULL.
			 * @param valor Valor del elemento a buscar
			 * @param el Elemento del árbol XML a partir del que se comenzará la búsqueda
			 * @return Elemento cuyo valor coincide con el introducido por parámetro
			 */
			TiXmlElement* buscar(const std::string& valor, TiXmlElement* el = 0);

			/**
			 * Método que devuelve el siguiente elemento hermano (es decir, que se encuentra en la misma profundidad
			 * en el árbol XML, y que tiene el mismo elemento padre) con el mismo valor que el recibido por parámetro.
			 * Si no se encontrara, se devuelve el valor NULL.
			 * @param el Elemento del árbol XML a partir del que se busca el siguiente hermano
			 * @return Elemento hermano consecutivo al recibido, con el mismo valor que éste.
			 */
			TiXmlElement* siguiente(TiXmlElement* el);

		protected:
			
			/**
			 * Constructor de la clase Parser. Se encuentra en la zona protegida debido a la 
			 * implementación del patrón Singleton.
			 */
			Parser(void) { };

			/**
			 * Destructor de la clase Parser. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */			
			~Parser(void) { };

			/**
			 * Constructor de copia de la clase Parser. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton.
			 */
			Parser(const Parser& p);

			/**
			 * Operador de asignación de la clase Parser. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton.
			 */
			Parser& operator=(const Parser& p);

		private:

			// Función auxiliar para construir la recursividad en el recorrido del árbol XML de la búsqueda
			void buscar_aux(const std::string& valor, TiXmlElement* padre, TiXmlElement*& el, bool& stop);

			static Parser* _instance;
			TiXmlDocument _doc;

	};

	#define parser Parser::get_instance()

#endif

