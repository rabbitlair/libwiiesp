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

#ifndef _ACTOR_H_
#define _ACTOR_H_

	#include <gctypes.h>
	#include <map>
	#include <set>
	#include <string>
	#include <valarray>
	#include "animacion.h"
	#include "colision.h"
	#include "excepcion.h"
	#include "galeria.h"
	#include "parser.h"

	class Nivel;

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que proporciona una base para definir actores (también llamados sprites) de una manera fácil.
	 * 
	 * @details Un actor es la unidad básica en el desarrollo de un videojuego en dos dimensiones, y consiste en un
	 * objeto con entidad propia que se puede visualizar en la pantalla, y que almacena información sobre sus distintas
	 * características. Para facilitar la creación de actores que participen en el universo de un videojuego, libWiiEsp
	 * proporciona esta clase abstracta Actor, que permite controlar desde un mismo sitio los distintos estados por los
	 * que puede pasar un actor durante el transcurso del videojuego.
	 *
	 * Las características de que dispone un actor son las siguientes: posición actual dentro del universo del juego
	 * (pareja de coordenadas (x,y)), posición previa del actor en el universo del juego (otro par de coordenadas
	 * (x,y)), velocidad de movimiento horizontal y vertical, estado actual del actor, estado previo del actor, y el
	 * tipo de actor. Por supuesto, al derivar esta clase abstracta, se pueden añadir
	 * más características según se crea necesario.
	 *
	 * Realizando un acercamiento más técnico y concreto, un actor no es más que un conjunto de las características
	 * antes mencionadas, además de dos diccionarios donde, a cada estado del actor, le corresponde una animación y un
	 * conjunto de cajas de colisión, y que proporciona una manera sencilla y efectiva de cargar todas estas
	 * características desde un archivo XML almacenado en la tarjeta SD de la consola.
	 *
	 * Existen dos referencias para las coordenadas de un actor. En primer lugar, están las coordenadas de posición del
	 * actor dentro del escenario que conforma el propio juego, y que se refieren al desplazamiento en píxeles hacia la
	 * derecha del actor respecto al límite izquierdo del escenario (coordenada X), y al desplazamiento hacia abajo,
	 * también en píxeles, del actor respecto al límite superior de dicho escenario (coordenada Y). Por otra parte,
	 * existe otra referencia para las coordenadas de dibujo de un actor en la pantalla, y que se refieren a la
	 * posición respecto a los límites izquierdo y superior de la pantalla, además de la capa en la que se dibuja el
	 * actor (consultar la documentación de la clase Screen para más información respecto al dibujo de texturas en la
	 * pantalla). Para ilustrar esta diferencia, nada mejor que un ejemplo: supongamos un escenario de 4000 píxeles de
	 * ancho y 2000 de alto. La pantalla de la consola Nintendo Wii con una configuración PAL y proporción 4:3 tendrá
	 * un tamaño de 640 píxeles de ancho, y 528 de alto. Si el actor a dibujar se encuentra en las coordenadas (1700,
	 * 1000) del escenario, se utilizarán estos valores para los cálculos de colisiones, de cambios de posición y de
	 * estados. Sin embargo, a la hora de dibujar al actor, hay que tener en cuenta el propio desplazamiento de la
	 * pantalla sobre el escenario; si la pantalla tuviera su punto (0,0) en el píxel (1600, 900) del escenario,
	 * entonces las coordenadas de dibujo del actor serán (100,100). El desplazamiento de la pantalla respecto a los
	 * límites del escenario se controla desde la clase que controla el propio escenario, que será una clase derivada
	 * de la clase abstracta Nivel (ver documentación de esta clase para más información).
	 *
	 * El comportamiento de un actor viene definido por los distintos estados que puede adoptar a lo largo de la
	 * ejecución del juego. Cada estado tiene asociado un comportamiento concreto, y que se ejecutará en cada iteración
	 * del bucle principal del juego en la que el actor tenga activo ese estado concreto. Las transiciones entre
	 * estados se realizan externamente (generalmente, desde la clase que gestiona el escenario, y que deriva de la
	 * clase abstracta Nivel). Los estados se identifican mediante una cadena de caracteres de alto nivel (de tipo
	 * std::string), y se crean según aparezcan en el archivo XML que define las animaciones y las cajas de colisión
	 * para cada estado, es decir, un estado sólo se creará si aparece en algún momento en el mencionado archivo XML.
	 * El comportamiento del actor según su estado actual debe definirse en el método virtual puro actualizar cuando se
	 * derive la clase Actor.
	 *
	 * Un detalle muy importante es que existe un estado obligatorio que hay que incluir forzosamente, y es el estado
	 * "normal", que es el que se toma por defecto. Si este estado faltara en el archivo XML del actor, se produciría
	 * un error y/o un comportamiento inesperado.
	 *
	 * La cadena de caracteres que identifica a un estado se toma como clave para dos diccionarios, uno que almacena
	 * una animación asociada al estado concreto (consultar documentación de la clase Animación para cualquier duda
	 * sobre ésta), y otra que almacena un conjunto (de tipo std::set<Figura*>) de figuras de colisión también
	 * asociadas al mismo estado. Estos diccionarios que tienen códigos de estado como clave identificativa se utilizan
	 * para que, en cada iteración del bucle principal, se dibuje la animación del actor correspondiente con el estado
	 * actual de éste, y sólo se tengan en cuenta las cajas de colisión asociadas al estado actual del actor para
	 * evaluar sus colisiones. Se proporcionan métodos para evaluar colisiones entre dos actores de una manera muy
	 * sencilla, y también para dibujar el actor en la pantalla en base a unas coordenadas de pantalla que se reciben
	 * desde fuera de la clase (la diferencia entre las coordenadas de posición y las de pantalla se explican unos
	 * párrafos más arriba).
	 *
	 * Otro detalle más es que se almacena un identificador de tipo de actor, es decir, todos los actores que estén
	 * gestionados por la misma clase derivada de Actor deberán tener este atributo con el mismo valor.
	 *
	 * Como ya se ha comentado, se consigue una separación completa de código fuente y datos, de tal manera que para
	 * cambiar las figuras de colisión, los estados o las animaciones de un actor, basta con modificar el archivo XML
	 * desde el cual se lee toda esta información. Este archivo tendrá una estructura parecida a esta:
	 *
	 * @code
	 * <?xml version="1.0" encoding="UTF-8"?>
	 * <actor vx="3" vy="3" tipo="jugador">
	 *   <animaciones>
	 *     <animacion estado="normal" img="chief" sec="0" filas="1" columnas="5" retardo="3" />
	 *     <animacion estado="mover" img="chief" sec="0,1,2,3,4" filas="1" columnas="5" retardo="3" />
	 *   <animacion estado="muerte" img="chief" sec="5" filas="1" columnas="6" retardo="0" />
	 *  </animaciones>
	 *  <colisiones>
	 *    <rectangulo estado="normal" x1="27" y1="21" x2="55" y2="21" x3="55" y3="96" x4="27" y4="96" />
	 *    <circulo estado="normal" cx="41" cy="13" radio="8" />
	 *    <rectangulo estado="mover" x1="27" y1="21" x2="55" y2="21" x3="55" y3="96" x4="27" y4="96" />
	 *    <circulo estado="mover" cx="41" cy="13" radio="8" />
	 *    <sinfigura estado="muerte" />
	 *   </colisiones>
	 * </actor>
	 * @endcode
	 *
	 * En el elemento raíz se observan tres atributos, que son la velocidad de movimiento horizontal del actor en
	 * píxeles por fotogramas, la velocidad de movimiento vertical, y el tipo de actor.
	 *
	 * Se pueden apreciar dos grandes bloques, uno para las animaciones, y otro para las cajas de colisión. Cada
	 * animación y figura incluye la información necesaria para ser creada, además de un estado al que se asociará.
	 * Sólo puede haber una animación por estado; pero no ocurre así para las cajas de colisión, de las cuales puede
	 * haber todas las que se deseen en cada estado. Cade destacar que, en ambos bloques, aparece el estado obligatorio
	 * "normal", como ya se indicó anteriormente. Para más información sobre los parámetros de los constructores de
	 * cajas de colisión o animaciones, consultar la documentación de cada una de estas clases.
	 *
	 * Por último, cabe destacar que al derivar la clase Actor, se le pueden añadir nuevos atributos y métodos según se
	 * considere necesario, consiguiendo así partir de una base como es la propia clase Actor, pero pudiendo llegar a
	 * la complejidad que se desee.
	 *
	 * Hay que mencionar que a la hora de crear un actor, es apropiado diseñar los cambios entre estados como un
	 * autómata finito determinado, que como ya se ha explicado, realizaría sus transiciones entre estados de forma
	 * externa. Si se realiza de esta manera, es muy fácil desarrollar un actor en un período de tiempo relativamente
	 * pequeño.
	 *
	 */
	class Actor
	{
		public:

			/**
			 * Conjunto de cajas de colisión.
			 */
			typedef std::set<Figura*> CajasColision;

			/**
			 * Diccionario que asocia un estado con un conjunto de cajas de colisión.
			 */
			typedef std::map<std::string, CajasColision> Colisiones;

			/**
			 * Diccionario que asocia un estado con una animación.
			 */
			typedef std::map<std::string, Animacion*> Animaciones;

			/**
			 * Constructor de la clase Actor. Establece el estado "normal" y la posición a cero.
			 * @param ruta Ruta absoluta en la tarjeta SD del archivo XML de datos del actor.
			 * @param nivel Puntero constante al nivel en el que se mueve el actor.
			 * @throw ArchivoEx Se lanza si hay algún error al abrir un archivo
			 * @throw CodigoEx Se lanza si se detectan dos animaciones para un mismo estado.
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 * @throw XmlEx Se lanza si hay un error relacionado con un árbol XML
			 */
			Actor(const std::string& ruta, const Nivel* nivel) throw (ArchivoEx, CodigoEx, TarjetaEx, XmlEx);

			/**
			 * Destructor virtual de la clase Actor. Libera la memoria ocupada por las cajas de colisión y por
			 * las animaciones asociadas a cada estado del actor.
			 */
			virtual ~Actor(void);

			/**
			 * Método consultor que devuelve el valor de la coordenada X actual del actor.
			 * @return Coordenada X del actor.
			 */
			u32 x(void) const;

			/**
			 * Método consultor que devuelve el valor de la coordenada Y actual del actor.
			 * @return Coordenada Y del actor.
			 */
			u32 y(void) const;

			/**
			 * Método consultor que devuelve el valor de la coordenada X del actor en la iteracion anterior del
			 * bucle principal del programa.
			 * @return Valor de la coordenada X del actor en la iteracion anterior del bucle principal
			 */
			u32 xPrevio(void) const;

			/**
			 * Método consultor que devuelve el valor de la coordenada Y del actor en la iteracion anterior del
			 * bucle principal del programa.
			 * @return Valor de la coordenada Y del actor en la iteracion anterior del bucle principal
			 */
			u32 yPrevio(void) const;

			/**
			 * Método consultor que devuelve el número de píxeles que se desplaza horizontalmente el actor en cada
			 * iteración del bucle principal.
			 * @return Valor de la velocidad horizontal del actor.
			 */
			s16 velX(void) const;

			/**
			 * Método consultor que devuelve el número de píxeles que se desplaza verticalmente el actor en cada
			 * iteración del bucle principal.
			 * @return Valor de la velocidad vertical del actor.
			 */
			s16 velY(void) const;

			/**
			 * Método consultor que devuelve el estado actual en el que se encuentra el actor.
			 * @return Estado actual en el que se encuentra el actor.
			 */
			const std::string& estado(void) const;

			/**
			 * Método consultor que devuelve el estado del actor en la iteración anterior del bucle principal.
			 * @return Estado del actor en la iteración anterior del bucle principal.
			 */
			const std::string& estadoPrevio(void) const;

			/**
			 * Método consultor que devuelve una referencia constante al conjunto de figuras de colisión que
			 * corresponden al estado actual del actor.
			 * @return Referencia constante al conjunto de cajas de colisión del estado actual del actor.
			 */
			const CajasColision& cajasColision(void) const;

			/**
			 * Método consultor que devuelve el ancho en píxeles de un cuadro de la animación que corresponde al
			 * estado actual del actor.
			 * @return Ancho en píxeles de un cuadro de la animación del estado actual del actor.
			 */
			u16 ancho(void) const;

			/**
			 * Método consultor que devuelve el alto en píxeles de un cuadro de la animación que corresponde al
			 * estado actual del actor.
			 * @return Alto en píxeles de un cuadro de la animación del estado actual del actor.
			 */
			u16 alto(void) const;

			/**
			 * Método consultor que indica el tipo de actor.
			 * @return Tipo de actor.
			 */
			const std::string& tipoActor(void) const;

			/**
			 * Método que modifica la posición del actor estableciendo sus nuevas coordenadas. Se toma como origen
			 * el punto superior izquierdo del escenario. Si aumenta la X, más a la derecha estará el actor respecto
			 * del punto x = 0; y si aumenta la Y, más abajo estará el actor respecto del punto y = 0.
			 * @param x Nuevo valor para la coordenada X del actor.
			 * @param y Nuevo valor para la coordenada Y del actor.
			 */
			void mover(u32 x, u32 y);

			/**
			 * Método que modifica la velocidad de desplazamiento horizontal del actor. Indica el número de píxeles
			 * que se cambia la posición al realizar un movimiento horizontal.
			 * @param vx Nuevo valor para la velocidad de desplazamiento horizontal.
			 */
			void setVelX(s16 vx);

			/**
			 * Método que modifica la velocidad de desplazamiento vertical del actor. Indica el número de píxeles
			 * que se cambia la posición al realizar un movimiento vertical.
			 * @param vy Nuevo valor para la velocidad de desplazamiento vertical.
			 */
			void setVelY(s16 vy);

			/**
			 * Método que modifica el estado del actor. Almacena el estado actual, que pasa a ser el anterior.
			 * Si el estado que se recibe no se encuentra en los diccionarios de animaciones y cajas de colisión
			 * no se modifica el estado, y se devuelve un valor falso.
			 * @param e Nuevo estado en el que se encuentra el actor.
			 * @return Verdadero si se ha cambiado el estado del actor, y falso en caso contrario.
			 */
			bool setEstado(const std::string& e);

			/**
			 * Método que establece la orientación de la imagen del actor en la pantalla, sobre el eje vertical.
			 * @param inv Verdadero si se debe dibujar la imagen invertida, falso en caso contrario.
			 */
			void invertirDibujo(bool inv);

			/**
			 * Método que dibuja el cuadro correspondiente de la animación asociada al estado actual del actor.
			 * Las coordenadas que recibe son coordenadas de la pantalla, no del escenario del juego donde se
			 * encuentra el actor; y hacen referencia al punto superior izquierdo de un rectángulo imaginario
			 * que contendría al actor. Se permite redefinir el método por si fuera necesario para dibujar un
			 * tipo de actor especial.
			 * @param x Coordenada X de la pantalla donde se dibujará el actor.
			 * @param y Coordenada Y de la pantalla donde se dibujará el actor.
			 * @param z Capa en la que se dibujará el actor. Más atrás cuanto mayor sea Z. Entre 0 y 999.
			 */
			virtual void dibujar(s16 x, s16 y, s16 z);

			/**
			 * Método para saber si el actor al que pertenece la función colisiona con otro externo. Un actor
			 * colisiona con otro si lo hacen entre sí, al menos, una figura de colisión de cada uno de ellos.
			 * @param a Actor externo con el que se quiere evaluar si hay colisión.
			 * @return Verdadero si hay colisión entre los actores, y falso en caso contrario.
			 */
			bool colision(const Actor& a);

			/**
			 * Método virtual puro, en el que se debe implementar el comportamiento del actor (la actualización
			 * de las variables internas del actor) según el estado actual de éste. Las modificaciones sobre el
			 * estado actual del actor vendrán dadas desde fuera mediante el método setEstado.
			 */
			virtual void actualizar(void) = 0;

		protected:

			/**
			 * Método que, a partir de la ruta de un archivo XML con un formato concreto, abre éste y lanza
			 * los métodos que leen los datos iniciales: las animaciones y las cajas de colisión asociadas
			 * a cada estado.
			 * @param ruta Ruta absoluta al archivo XML que contiene la información del actor.
			 * @throw ArchivoEx Se lanza si hay algún error al abrir un archivo
			 * @throw CodigoEx Se lanza si se detectan dos animaciones para un mismo estado.
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 * @throw XmlEx Se lanza si el árbol XML con los datos de la animación esuviera incompleto.
			 */
			void cargarDatosIniciales(const std::string& ruta) throw (ArchivoEx, TarjetaEx, CodigoEx, XmlEx);

			/**
			 * Método que, a partir de un elemento de un árbol XML, lee las animaciones de un actor. Cada
			 * animación se espera que tenga una serie de elementos en el árbol XML, y se asocia con un
			 * estado concreto del actor. Un estado sólo puede tener una animación asociada.
			 * @param nodo Elemento de un árbol XML que contiene las animaciones de un actor.
			 * @throw CodigoEx Se lanza si se detectan dos animaciones para un mismo estado.
			 * @throw XmlEx Se lanza si el árbol XML con los datos de la animación esuviera incompleto.
			 */
			void leerAnimaciones(TiXmlElement* nodo) throw (CodigoEx, XmlEx);

			/**
			 * Método que, a partir de un elemento de un árbol XML, lee las cajas de colisión de un actor. Cada
			 * cajas de colisión se espera que tenga una serie de elementos en el árbol XML, y se asocia con un
			 * estado concreto del actor.
			 * @param nodo Elemento de un árbol XML que contiene las cajas de colisión de un actor.
			 */
			void leerColisiones(TiXmlElement* nodo);

			/**
			 * Número de píxeles que se desplaza horizontalmente el actor en cada actualización 
			 */
			s16 _vx;

			/**
			 * Número de píxeles que se desplaza verticalmente el actor en cada actualización 
			 */
			s16 _vy;

			/**
			 * Coordenada X de la posición del actor en el escenario del juego. Representa la distancia en
			 * píxeles del punto superior izquierdo del actor respecto al límite izquierdo del escenario.
			 */
			u32 _x;

			/**
			 * Coordenada Y de la posición del actor en el escenario del juego. Representa la distancia en
			 * píxeles del punto superior izquierdo del actor respecto al límite superior del escenario.
			 */
			u32 _y;

			/**
			 * Coordenada X de la posición del actor en la anterior actualización.
			 */
			u32 _x_previo;

			/**
			 * Coordenada Y de la posición del actor en la anterior actualización.
			 */
			u32 _y_previo;

			/**
			 * Estado actual del actor.
			 */
			std::string _estado_actual;

			/**
			 * Estado anterior del actor.
			 */
			std::string _estado_previo;

			/**
			 * Tipo de actor.
			 */
			std::string _tipo_actor;

			/**
			 * Indica si se debe dibujar la animación del actor invertida o no.
			 */
			bool _invertida;

			/**
			 * Diccionario de colisiones del actor, que asocia un estado con un conjunto de cajas de colisión.
			 */
			Colisiones _map_colisiones;

			/**
			 * Diccionario de animaciones del actor, que asocia un estado con una animación.
			 */
			Animaciones _map_animaciones;

			/**
			 * Referencia al nivel en el que está participando el actor.
			 */
			const Nivel* _nivel;
	};

#endif

