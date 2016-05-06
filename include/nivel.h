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

#ifndef _NIVEL_H_
#define _NIVEL_H_

	#include <map>
	#include <string>
	#include <vector>
	#include "actor.h"
	#include "colision.h"
	#include "excepcion.h"
	#include "galeria.h"
	#include "mando.h"
	#include "parser.h"
	#include "screen.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que proporciona una base para definir niveles (escenarios donde participan actores) de manera fácil.
	 * 
	 * @details Un nivel, desde el punto de vista de LibWiiEsp, no es más que un escenario donde los actores participan
	 * en el juego. Está formado por tres elementos principales que son los actores, el conjunto de tiles que componen
	 * el propio nivel y una imagen de fondo. También tiene asociada una pista de música propia, una imagen desde la
	 * que se toman los tiles que se dibujan, y unas coordenadas de scroll para la pantalla, así como un ancho y un
	 * alto.
	 *
	 * El escenario que presenta un nivel es de forma rectangular, y tiene un ancho y un alto definidos por el número
	 * de tiles que lo componen. A pesar de ello, en todo momento están disponibles sus medidas en píxeles. Dentro de
	 * este escenario, todos los elementos tienen una pareja de coordenadas que indican la distancia horizontal hacia
	 * la derecha respecto al límite izquierdo del nivel (coordenada X), y la distancia vertical hacia abajo respecto
	 * al límite superior del nivel (coordenada Y). Así pues, el origen de coordenadas del nivel (el punto (0,0)) es el
	 * vértice superior izquierdo del rectángulo que forma el escenario.
	 *
	 * En la pantalla sólo puede dibujarse una parte del escenario del nivel, ya que ésta tiene un tamaño determinado
	 * por el modo de vídeo. La parte del nivel que se dibuja en la pantalla en un momento determinado es la ventana
	 * del nivel, y es un rectángulo con las mismas medidas que la pantalla, y que tiene unas coordenadas respecto al
	 * punto (0,0) del escenario. Estas coordenadas son el scroll del nivel, y se puede modificar mediante el método
	 * moverScroll().
	 *
	 * La imagen de fondo del nivel debe tener el tamaño de la pantalla completa (para una televisión PAL de
	 * proporciones 4:3, las medidas en píxeles son 640×528), y será fija durante todo el transcurso del nivel. La idea
	 * de esta imagen de fondo estática es para situar un paisaje que acompañe al nivel, ya que la sensación de avance
	 * se produce con los propios tiles que componen la estructura del nivel.
	 *
	 * Existen dos tipos de tiles en un nivel, que se distinguen únicamente en que los tiles no atravesables tienen un
	 * rectángulo de colisión con el mismo tamaño y posición, y por lo tanto, provocará que un actor que colisione con
	 * él pueda reaccionar de una manera; sin embargo, los tiles atravesables no disponen de esta figura de colisión, y
	 * por lo tanto tienen únicamente un objetivo decorativo, para dotar de mayor detalle al escenario del nivel, pero
	 * no provocarán una colisión cuando un actor los toque. Todos estos tiles se almacenan en la misma estructura (de
	 * tipo Escenario), y para comprobar si un actor concreto colisiona con algún elemento del nivel se proporciona el
	 * método colision() (información útil para saber, por ejemplo en un juego de plataformas, si el actor está cayendo
	 * o está sobre una plataforma). Si el programador necesita un mayor detalle en la detección de colisiones entre
	 * actores y escenario, siempre puede añadir los métodos y atributos que considere necesarios para ello al crear la
	 * clase derivada de Nivel. La detección de colisiones entre un actor y los tiles del escenario está optimizada
	 * para que sólo se evalúe la colisión con los tiles sobre los que se encuentre el actor, evitando cálculos
	 * innecesarios.
	 *
	 * Se proporciona un método virtual puro actualizarEscenario() en el que se pueden implementar comportamientos para
	 * el escenario (como por ejemplo, plataformas destructibles, o en movimiento, además de cambiar las coordenadas
	 * del scroll de la pantalla), y un método que dibuja la ventana del nivel en la pantalla de la consola.
	 *
	 * Los actores que participan en un nivel se distinguen entre los actores que son dirigidos por los jugadores, y
	 * los actores que controla la propia CPU. Cada uno de ellos se almacena en una estructura distinta, y dispone de
	 * un método concreto para actualizarlo. Concretamente, el método para actualizar los actores no jugadores es
	 * actualizarNpj(), en el que se supone que se debe implementar la actualización de todos los actores controlados
	 * por la máquina (la decisión sobre cómo llevar a cabo esta operación queda, por entero, en manos del
	 * programador). Igualmente, para actualizar los actores jugadores, se dispone del método actualizarPj(), que debe
	 * actualizar a un único jugador en cada llamada, y lo hace recibiendo el código identificador único del jugador, y
	 * el mando asociado a él.
	 *
	 * Hay que destacar especialmente la filosofía de esta clase Nivel junto con la clase Actor. En cada clase derivada
	 * de Actor se debe definir el comportamiento de cada actor dependiendo de su estado actual. Sin embargo, las
	 * transiciones entre estados de un actor pueden realizarse desde el Nivel en el que el actor está participando,
	 * o bien en el propio actor, ya que éste dispone de una referencia constante al Nivel en el que se encuentra,
	 * aunque lo que se recomienda es tomar la primera opción, y situar las transiciones entre estados en el método de
	 * actualización del Nivel, de tal manera que los actores respondan con un comportamiento u otro, dependiendo de
	 * cómo se les haya definido, y siempre según el estado que tengan activado.
	 *
	 * Una vez descritos los elementos que conforman el escenario de un nivel, hay que detallar especialmente el
	 * proceso de carga de un nivel desde un archivo TMX creado con el editor de mapas de tiles Tiled
	 * (http://www.mapeditor.org/):
	 *
	 * Cuando se crea un nivel, el constructor recibe un archivo TMX que se carga desde la tarjeta SD gracias a la
	 * clase Parser. Una vez en memoria, se lee el ancho y alto del nivel en tiles, y el ancho y el alto en píxeles de
	 * un único tile (las medidas de un tile deben ser múltiplos de 8, ya que llevarán una imagen asociada, ver
	 * documentación de Imagen para más información). A partir de estos datos, se calcula el ancho y alto del escenario
	 * del nivel en píxeles.
	 *
	 * Después, el proceso de lectura desde el archivo TMX continúa leyendo las propiedades del mapa, que son
	 * imagen_fondo (que es el código que debe tener la imagen de fondo del nivel en la Galeria de medias),
	 * imagen_tileset (código que la imagen del tileset deberá tener asociado en la Galeria de medias del sistema) y
	 * musica (código identificador de la pista de música en la Galeria de medias del sistema). Tras leer las
	 * propiedades del mapa de tiles, se procede a leer las tres capas que debe contener el archivo TMX, que son las
	 * siguientes:
	 *
	 * 1. Capa ‘escenario’ (nombre obligatorio): capa de patrones del editor Tiled, debe contener los tiles
	 * atravesables.
	 * 2. Capa ‘plataformas’ (nombre obligatorio): capa de patrones del editor Tiled, que contiene los tiles no
	 * atravesables (los que tendrán figura de colisión asociada)
	 * 3. Capa ‘actores’ (nombre obligatorio): capa de objetos del editor Tiled. Cada objeto definido en esta capa debe
	 * tener la propiedad xml con la dirección absoluta en la tarjeta SD del archivo XML de descripción del actor como
	 * valor. Si además, el actor es un actor jugador, se espera que tenga otra propiedad llamada jugador, y cuyo valor
	 * debe ser el código identificador del jugador. También debe tener el identificador de su tipo en el campo Tipo.
	 *
	 * Hay que tener en cuenta otro detalle importante, y es que, al llamar al constructor, la información de los
	 * actores se guardan en una estructura temporal. Por ese motivo, existe el método virtual puro cargarActores(),
	 * que ha de ser implementado por el programador al derivar la clase Nivel, y que se debe encargar de recorrer esta
	 * estructura temporal, crear cada actor utilizando el constructor de la clase correspondiente al tipo del actor, y
	 * almacenarlo en la estructura correspondiente (dependiendo de si es un actor jugador o no jugador). Por último,
	 * es conveniente que este método vacíe la estructura temporal para no malgastar memoria.
	 *
	 * Esto último es necesario ya que, en la propia clase abstracta Nivel que forma parte de LibWiiEsp no se conoce
	 * qué clases derivadas habrá de Actor, y por tanto, se deja en manos del programador implementar la creación de
	 * Actores en el nivel. A continuación, se establece el scroll del nivel al valor (0,0), y se da por concluida la
	 * carga del nivel. Para conocer todos los detalles sobre la creación de niveles con el editor Tiled, consultar el
	 * manual de LibWiiEsp.
	 *
	 * Ejemplo de uso
	 *
	 * Con todo lo descrito, una vez cargado un nivel a partir de su archivo TMX, ya está listo para empezar a jugar.
	 * Un ejemplo de bucle principal utilizando un único nivel sería el siguiente:
	 *
	 * @code
	 * Mando m();
	 * NivelPrueba* nivel = new NivelPrueba( "/apps/wiipang/xml/prueba.tmx" );
	 * bool salir = false;
	 * while(not salir) {
	 *   // Actualizar el mando del jugador
	 *   Mando::leerDatos();
	 *   m.actualizar();
	 *   // Salir al pulsar HOME
	 *   if(m.newPressed(Mando::BOTON_HOME))
	 *     salir = true;
	 *   // Actualizar el jugador en base a su mando (cambios de estado)
	 *   nivel->actualizarPj("pj1", m);
	 *   // Actualizar los personajes no jugadores (cambios de estado)
	 *   nivel->actualizarNpj();
	 *   // Actualiza los elementos del nivel que no son actores (scroll)
	 *   nivel->actualizarEscenario();
	 *   // Mantener la reproducción continua de la pista de música del nivel
	 *   nivel->musica().loop();
	 *   // Dibujar el nivel
	 *   nivel->dibujar();
	 * }
	 * @endcode
	 *
	 * En este ejemplo, se considera que se ha derivado la clase abstracta Nivel en la clase NivelPrueba, a la que se
	 * le han implementado los métodos actualizarPj(), actualizarNpj() y actualizarEscenario(), y que los actores del
	 * nivel se vuelcan desde la estructura temporal con la definición del método cargarActores().
	 * 
	 */
	class Nivel
	{
		public:

			/**
			 * @brief Estructura que almacena los datos de un tile.
			 * @details Se compone de una pareja de coordenadas (X,Y), que especifican la distancia del punto superior
			 * izquierdo del tile respecto al origen de coordenadas (0,0) del nivel, un entero de 32 bits que indica
			 * el número identificador del tile dentro del mapa de tiles generado con la aplicación Tiled, y un puntero
			 * a un rectángulo de colisión asociado al tile (puede ser NULL).
			 */
			typedef struct tile
			{
				u32 x;
				u32 y;
				u32 gid;
				Rectangulo* colision;
			} Tile;

			/**
			 * Nombres de las distintas capas de Tiles que componen un escenario
			 */
			typedef enum
			{
				PLATAFORMAS,
				ESCENARIO
			} Capa;

			/**
			 * Dicionario que almacena todos los tiles que componen un nivel.
			 */
			typedef std::map<Capa, Tile**> Escenario;

			/**
			 * Vector que almacena todos los actores no jugadores que participan en un nivel.
			 */
			typedef std::vector<Actor*> Actores;

			/**
			 * Diccionario que almacena los actores jugadores que participan en un nivel, asociando
			 * cada jugador con un código identificativo único.
			 */
			typedef std::map<std::string, Actor*> Jugadores;

			/**
			 * Constructor de la clase Nivel. Carga un archivo TMX generado con el editor de mapas de tiles Tiled,
			 * creando todas las estructuras necesarias del nivel, los actores no jugadores y los actores jugadores.
			 * @param ruta Ruta absoluta hasta el archivo TMX generado con Tiled que almacena la información del nivel
			 * @throw ArchivoEx Se lanza si hay algún error al abrir un archivo
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			Nivel(const std::string& ruta) throw (ArchivoEx, TarjetaEx);

			/**
			 * Destructor virtual de la clase. Destruye todos los tiles y actores.
			 */
			virtual ~Nivel(void);

			/**
			 * Método consultor para conocer el ancho en píxeles del escenario que compone el nivel.
			 * @return Ancho en píxeles del escenario que compone el nivel.
			 */
			u32 ancho(void) const;

			/**
			 * Método consultor para conocer el alto en píxeles del escenario que compone el nivel.
			 * @return Alto en píxeles del escenario que compone el nivel.
			 */
			u32 alto(void) const;

			/**
			 * Método consultor para saber la posición del scroll horizontal del nivel. El scroll horizontal es un
			 * entero de 32 bits sin signo, que indica la distancia, desde el límite izquierdo del escenario 
			 * y hacia la izquierda, a la que se encuentra la esquina superior izquierda de la parte del nivel 
			 * que se dibujará en la pantalla de la consola.
			 * @return Posición del scroll horizontal.
			 */
			u32 xScroll(void) const;

			/**
			 * Método consultor para saber la posición del scroll vertical del nivel. El scroll vertical es un
			 * entero de 32 bits sin signo, que indica la distancia, desde el límite superior del escenario 
			 * y hacia abajo, a la que se encuentra la esquina superior izquierda de la parte del nivel 
			 * que se dibujará en la pantalla de la consola.
			 * @return Posición del scroll vertical.
			 */
			u32 yScroll(void) const;

			/**
			 * Método consultor para obtener la pista de música del nivel.
			 * @return Referencia constante a la pista de música del nivel.
			 */
			const Musica& musica(void) const;

			/**
			 * Método que modifica la posición del scroll estableciendo sus nuevas coordenadas. Se toma como origen
			 * el punto superior izquierdo del escenario. Si aumenta la X, más a la derecha estará el scroll respecto
			 * del punto x = 0; y si aumenta la Y, más abajo estará el scroll respecto del punto y = 0.
			 * @param x Nuevo valor para la coordenada X del scroll.
			 * @param y Nuevo valor para la coordenada Y del scroll.
			 */
			void moverScroll(u32 x, u32 y);

			/**
			 * Método para conocer si un actor externo colisiona con, al menos, un tile del escenario del nivel.
			 * @param a Puntero constante al actor del cual se quiere saber si colisiona con el escenario del nivel.
			 * @return Verdadero si hay colisión, o falso en caso contrario.
			 */
			bool colision(const Actor* a);

			/**
			 * Método para conocer si un actor externo colisiona con uno de los límites del nivel.
			 * @param a Puntero constante al actor del cual se quiere saber si colisiona con los límites del nivel.
			 * @return Verdadero si hay colisión, o falso en caso contrario.
			 */
			bool colisionBordes(const Actor* a);

			/**
			 * Método virtual puro en el que se debe implementar la actualización de un actor jugador en base al
			 * estado del mando asociado a él. En este método se deben gestionar las transiciones entre estados del
			 * autómata finito determinado que controla el comportamiento del Actor.
			 * @param jugador Código identificador único del actor jugador que se quiere actualizar.
			 * @param m Mando asociado al actor jugador que se quiere actualizar.
			 */
			virtual void actualizarPj(const std::string& jugador, const Mando& m) = 0;

			/**
			 * Método virtual puro en el que se debe implementar la actualización de un actor no jugador. En este
			 * método se deben gestionar las transiciones entre estados del autómata finito determinado que controla
			 * el comportamiento del Actor. En este caso, no se depende de un mando, si no del estado del nivel.
			 */
			virtual void actualizarNpj(void) = 0;

			/**
			 * Método virtual puro en el que se debe implementar la actualización de todos los aspectos del escenario
			 * del nivel que no correspondan a ningún actor, por ejemplo, el scroll, las zonas de escenario
			 * destructibles, etc.
			 */
			virtual void actualizarEscenario(void) = 0;

			/**
			 * Método que dibuja la parte del escenario del nivel que marque las coordenadas del scroll en la pantalla.
			 * Las coordenadas de scroll indicarán el punto superior izquierdo de la parte rectangular del escenario
			 * del nivel a dibujar. Se dibujan también todos los actores que se encuentren dentro de esta sección.
			 */
			void dibujar(void);

		protected:

			/**
			 * @brief Estructura para almacenar de forma temporal la información de un actor.
			 * @details Se compone de un identificador del tipo de actor al que pertenece la información, la ruta
			 * hasta el archivo XML desde donde se cargan los datos iniciales del actor, las coordenadas (X,Y) del
			 * actor respecto al origen de coordenadas del nivel, y una cadena de caracteres que guarda el código
			 * identificador del jugador al que representa el actor (si este último atributo es una cadena vacía
			 * se asume que el actor no es controlado por ningún jugador).
			 */
			typedef struct actor
			{
				std::string tipo_actor;
				std::string xml;
				u32 x;
				u32 y;
				std::string jugador;
			} ActorTemp;

			/**
			 * Vector para almacenar de forma temporal la información de todos los actores el nivel. Se cargan 
			 * los datos en el constructor del nivel. Para crear cada actor como tal hay que definir el método 
			 * virtual cargarActores(), el cual se deja en manos del programador, y que debe recoger la información
			 * almacenada en este vector y crear cada actor utilizando el constructor de la clase derivada de Actor que
			 * corresponda al tipo de actor de cada registro temporal.
			 */
			typedef std::vector<ActorTemp> Temporal;

			/**
			 * Método que, a partir de un elemento de un árbol XML, se encarga de leer todas las propiedades del nivel.
			 * @param propiedades Elemento XML donde se almacenan las propiedades del nivel.
			 */
			void leerPropiedades(TiXmlElement* propiedades);

			/**
			 * Método que, a partir de un elemento de un árbol XML, se encarga de leer los tiles que componen el
			 * escenario del nivel. Un tile forma parte del escenario si no tiene asociado ninguna caja de colisión,
			 * es decir, si es un tile atravesable.
			 * @param escenario Elemento XML donde se almacenan los tiles del escenario del nivel.
			 */
			void leerEscenario(TiXmlElement* escenario);

			/**
			 * Método que, a partir de un elemento de un árbol XML, se encarga de leer los tiles que componen las
			 * platformas del nivel. Un tile forma parte de las plataformas si tiene asociada una caja de colisión,
			 * es decir, si es un tile no atravesable.
			 * @param plataformas Elemento XML donde se almacenan los tiles de las plataformas del nivel.
			 */
			void leerPlataformas(TiXmlElement* plataformas);

			/**
			 * Método que, a partir de un elemento de un árbol XML, se encarga de leer todos los actores que 
			 * participan en el nivel, tanto los jugadores como los no jugadores, y los almacena en la estructura
			 * temporal de tipo Temporal.
			 * @param actores Elemento XML donde se almacena la información de los actores del nivel.
			 */
			void leerActores(TiXmlElement* actores);

			/**
			 * Método virtual puro que hay que definir en las clases derivadas, y que se debe encargar de volcar la
			 * información de los actores, almacenada en la estructura temporal que se rellena en el constructor
			 * del nivel, en las estructuras definitivas de los actores. Para cada actor, debe identificar su tipo
			 * y llamar al constructor de la clase adecuada con los parámetros de inicialización que se proporcionan
			 * desde esta estructura temporal. Es recomendable que vacíe la estructura temporal cuando finalice.
			 * Debe ser llamado en el constructor de la clase derivada de Nivel.
			 */
			virtual void cargarActores(void) = 0;

			/**
			 * Código con el que se almacena la imagen de fondo del nivel en la Galeria.
			 */
			std::string _imagen_fondo;

			/**
			 * Código con el que se almacena la imagen del tileset del nivel en la Galeria.
			 */
			std::string _imagen_tileset;

			/**
			 * Código con el que se almacena la pista de música del nivel en la Galeria.
			 */
			std::string _musica;

			/**
			 * Ancho en píxeles de un tile
			 */
			u32 _ancho_un_tile;

			/**
			 * Alto en píxeles de un tile
			 */
			u32 _alto_un_tile;

			/**
			 * Ancho, medido en tiles, del nivel
			 */
			u32 _ancho_tiles;

			/**
			 * Alto, medido en tiles, del nivel
			 */
			u32 _alto_tiles;

			/**
			 * Posición del scroll horizontal del nivel.
			 */
			u32 _scroll_x;

			/**
			 * Posición del scroll vertical del nivel.
			 */
			u32 _scroll_y;

			/**
			 * Ancho del nivel medido en píxeles
			 */
			u32 _ancho_nivel;

			/**
			 * Alto del nivel medido en píxeles
			 */
			u32 _alto_nivel;

			/**
			 * Número de filas de tiles que componen la imagen del tileset.
			 */
			u16 _filas_tileset;

			/**
			 * Número de columnas de tiles que componen la imagen del tileset.
			 */
			u16 _columnas_tileset;

			/**
			 * Estructura que almacena todos los tiles del nivel, tanto los atravesables como los no atravesables.
			 */
			Escenario _escenario;

			/**
			 * Estructura que almacena todos los actores no jugadores del nivel.
			 */
			Actores _actores;

			/**
			 * Estructura que almacena todos los actores jugadores del nivel.
			 */
			Jugadores _jugadores;

			/**
			 * Estructura que guarda temporalmente todos los actores del nivel. Esto se hace en el constructor, y se
			 * hace para que el constructor no deba llamar a una función virtual pura de carga de actores (el
			 * compilador daría error). Como solución a este problema, se ha decidido que el constructor carga la
			 * información de los actores en esta estructura temporal, y después, en el constructor de la clase 
			 * derivada de Nivel, se vuelca esta información en las estructuras definitivas para los actores.
			 */
			Temporal _temporal;
	};

#endif

