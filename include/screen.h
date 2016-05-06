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

#ifndef _SCREEN_H_
#define _SCREEN_H_

	#include <cmath>
	#include <cstdlib>
	#include <cstring>
	#include <gccore.h>
	#include <malloc.h>

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que gestiona los gráficos en la Nintendo Wii.
	 * 
	 * @details La libogc proporciona una biblioteca de bajo nivel para trabajar con los gráficos, llamada GX. La GX
	 * trabaja, de base, en tres dimensiones, y soporta varias proyecciones gráficas, pero como este es un proyecto en
	 * 2D, sólo se utilizará la proyección ortográfica. Básicamente, se trata de que todo lo que hay en el mundo del
	 * juego se proyecta sobre la pantalla de TV de forma perpendicular (como si el foco de luz estuviera en el
	 * infinito), de tal manera que se muestra como si sólo hubiera dos dimensiones, y además, la profundidad no se
	 * tiene en cuenta: se ve al mismo tamaño un objeto, esté en la capa 0, o en la 999.
	 *
	 * GX espera que, a cada frame, se le pase un buffer con lo que tiene que dibujar en ese momento. Esta clase Screen
	 * utiliza un buffer (de ahora en adelante FIFO, First Input First Output, nombre descriptivo sobre cómo funciona)
	 * de 1MB, que es el tamaño máximo que está soportado, y es un tamaño más que sobrado para dibujar toda la pantalla
	 * en un frame concreto. Además, implementa un sistema de doble buffer de una forma sencilla, de tal manera que se
	 * alternan: cada vez que hay que copiar datos al FIFO, se hace desde un buffer distinto. Es decir, que habrá un
	 * framebuffer activo en cada frame, que será en el que se dibuje todo, y al final del frame, el contenido de dicho
	 * buffer se copia al FIFO.
	 *
	 * El modo de pantalla (PAL, NTSC, PAL60) se toma automáticamente de la configuración que tenga la consola en el
	 * momento de inicializar la clase, de tal manera que no hay que preocuparse. Además, cuando se configura el modo de
	 * vídeo, pasamos a tener disponibles el ancho y el alto de la pantalla en píxeles. Se proporciona también el uso de
	 * capas de profundidad, que van desde 0 (primer plano) hasta 999 (capa más profunda). Nota: de momento, esta clase
	 * sólo trabaja con proporción 4:3, en un futuro no muy lejano se implementará soporte para 16:9.
	 *
	 * Cada punto que se vaya a dibujar en la pantalla consta de tres coordenadas (x,y,z), donde X es la distancia
	 * horizontal en píxeles hasta el lado izquierdo de la pantalla, Y es la distancia vertical en píxeles hasta el
	 * borde superior de la pantalla, y Z es la capa en la que se va a dibujar (como se ha dicho ya, es un número entero
	 * positivo que, cuanto más se acerca de 0, más cerca del primer plano estará el punto). El tipo de estas tres
	 * coordenadas es s16 (entero con signo de 16 bits).
	 *
	 * Cuando se dibuja un punto, hay que darle color para que se vea (si no, estaríamos hablando de un píxel
	 * invisible). El formato de color esperado es un entero sin signo de 32 bits (tipo u32 en la biblioteca), con el
	 * formato hexadecimal 0xRRGGBBAA, donde cada pareja representa un componente del color: RR es rojo representado en
	 * 8 bits, GG es verde, BB es azul, y AA es el canal alpha. Este canal alpha tiene una peculiaridad, y es que todo
	 * lo que tenga un valor inferior a 0×08 será considerado invisible.
	 *
	 * La GX de libogc proporciona un serie de funciones de dibujo primitivas para plasmar en la pantalla figuras
	 * geométricas. Las que se utilizan en la clase Screen son las siguientes:
	 *   1. GX_POINTS: Dibuja un sólo punto.
	 *   2. GX_LINES: Dibuja una línea recta entre dos puntos, de un píxel de ancho.
	 *   3. GX_TRIANGLES: Dibuja un triángulo a partir de tres puntos.
	 *   4. GX_QUADS: Dibujar un rectángulo a partir de cuatro puntos.
	 *
	 * Utilizar estas funciones primitivas para dibujar figuras geométricas es muy sencillo. Primero, hay que establecer
	 * una serie de descriptores, o dicho de otro modo, parámetros que le pasamos a la GX para que sepa qué vamos a
	 * hacer. Estos descriptores incluyen información sobre qué vamos a mandarle y cómo, por ejemplo, se le puede decir
	 * "se van a pintar puntos representados por tres variables s16, con color directo utilizando una variable u32, y no
	 * se va a utilizar textura". Posteriormente, con la instrucción 
	 * GX_Begin(primitiva, GX_VTXFMT0, numero_de_puntos_a_dibujar), comenzamos el bloque de dibujo propiamente dicho.
	 * Esta función indica qué primitiva se va a utilizar, y cuántos puntos se emplearán. Por ejemplo, para dibujar dos
	 * triángulos, la llamada sería:
	 *
	 * @code
	 * // Primitiva GX_TRIANGLES, y 6 vértices, para dibujar dos triángulos
	 * GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6)
	 * @endcode
	 *
	 * Seguidamente, se procede a dibujar los seis puntos, siguiendo el mismo formato que se ha indicado antes con los
	 * descriptores. Para dibujar un punto, la llamada es:
	 *
	 * @code
	 * // Dibujar un punto invisible en las coordenadas (x,y,z)
	 * GX_Position3s16(s16 x, s16 y, s16 z);
	 * // Rellenar el último punto dibujado con color rojo (0xRR0000AA)
	 * GX_Color1u32(0xFF0000FF);
	 * @endcode
	 *
	 * Finalmente, hay que cerrar el bloque de dibujo con la función GX_End()
	 *
	 * Descripción de la clase Screen
	 *
	 * La clase Screen implementa un patrón Singleton, ya que es lógico pensar que sólo se va a utilizar una pantalla en
	 * un juego. Tiene tres partes bien diferenciadas, que son las funciones que gestionan la pantalla propiamente
	 * dicha, las funciones relativas a texturas, y los métodos que se encargan de dibujar figuras geométricas en el
	 * framebuffer.
	 *
	 * Las funciones de gestión de pantalla son, principalmente, la de inicialización (inicializar(), llamar al
	 * principio del programa una sola vez), la de fin de frame (flip(), que se encarga de copiar lo que tengamos en el
	 * framebuffer actual al FIFO y de enviarlo al chip gráfico de la consola), y dos métodos consultores para saber, en
	 * todo momento, el ancho y alto de la pantalla en píxeles.
	 *
	 * El punto fuerte de la clase Screen tiene que ver con las texturas. Se proporciona una función, crearTextura(),
	 * que a partir de una zona de memoria en la cual haya píxeles de 8 bits por color y en formato RGB, creará un
	 * objeto de textura GXTexObj, con el formato que utiliza la GX, y preparado para trabajar con él. El formato de
	 * imagen que utiliza la Wii es RGB5A3, de 16 bits. Se representa con 5 bits para cada color, y uno para el canal
	 * alpha, pero se puede utilizar un píxel de cada color para determinar niveles de transparencia más complejos. Esta
	 * función de creación de texturas modifica la zona de memoria que almacena la información de los píxeles,
	 * convirtiéndola en tiles de 4×4.
	 *
	 * Una vez creada la textura, se puede dibujar tal cual en la pantalla utilizando la función dibujarTextura(),
	 * indicando sus coordenadas y su ancho y alto. También se puede dibujar una parte de esa textura, utilizando la
	 * función dibujarCuadro(), a la cual hay que pasarle las coordenadas de dibujo, el ancho y alto de la textura
	 * completa, y tanto las coordenadas X e Y de la parte de la textura que se quiere dibujar, como el ancho y el alto
	 * del cuadro. Internamente, estas dos funciones de dibujo hacen uso de una función privada de la clase,
	 * configurarTextura(), que se encarga de establecer los descriptores de la GX para dibujar texturas.
	 *
	 * El tercer bloque de funciones de dibujo de la clase Screen son, básicamente, funciones que permiten dibujar
	 * formas geométricas (como son un punto, una línea recta, un rectángulo y un círculo), en un color plano, y a
	 * partir de las primitivas que se describieron anteriormente. Importante: al dibujar un rectángulo, el orden de los
	 * puntos debe ser, sí o sí, izquierda-arriba, derecha-arriba, derecha-abajo, e izquierda-abajo. Si no se sigue esta
	 * recomendación, existe riesgo de provocar un cuelgue en el sistema. Internamente, estas funciones de dibujo hacen
	 * uso de una función privada de la clase, configurarColor(), que se encarga de establecer los descriptores de la GX
	 * para dibujar con color directo de relleno.
	 *
	 * Para más información sobre los métodos de la clase Screen, consultar su descripción.
	 *
	 * Ejemplo de uso:
	 *
	 * @code
	 *	screen->inicializar();
	 *
	 *	// Bucle principal
	 *	while(1) {
	 *		// Gestionar entrada, realizar cálculos, etc.
	 *		// ...
	 *
	 *		// Fase de dibujo: un circulo rojo y un rectangulo azul
	 *		screen->dibujarCirculo(100, 100, 5, 30.0f, 0xFF0000FF);
	 *		screen->dibujarRectangulo(300, 200, 400, 200, 400, 250, 300, 250, 10, 0x0000FFFF);
	 *
	 *		// Fin de frame
	 *		screen->flip();
	 *	}
	 * @endcode
	 *
	 */
	class Screen
	{
		public:

			/**
			 * Método de clase que devuelve la dirección de memoria de la instancia activa en el sistema
			 * de la clase Screen. Si aún no existe dicha instancia, la crea y la devuelve. Además, establece
			 * que al salir del programa, se destruya por medio de una llamada al destructor.
			 */
			static Screen* get_instance(void)
			{
				if(_instance == 0)
				{
					_instance = new Screen();
					atexit(destroy);
				}
				return _instance;
			}

			/**
			 * Método de clase que limpia la memoria ocupada por la instancia activa en el sistema
			 * de la clase Screen, llamando a su destructor.
			 */
			static void destroy(void)
			{
				delete _instance;
			}

			/**
			 * Método que inicializa todo el sistema de vídeo de la consola Nintendo Wii. Se debe llamar al comienzo
			 * del programa una sola vez.
			 */
			void inicializar(void);

			/**
			 * Método observador para el ancho de la pantalla en píxeles
			 * @return Ancho de la pantalla en píxeles
			 */
			u16 ancho(void) const;

			/**
			 * Método observador para el alto de la pantalla en píxeles
			 * @return Alto de la pantalla en píxeles
			 */
			u16 alto(void) const;

			/**
			 * Método que da por finalizado un frame. Si se ha marcado el flag interno de actualización de
			 * gráficos, se copia al buffer FIFO el contenido del framebuffer activo, y se establece el otro
			 * como activo para el siguiente frame.
			 */
			void flip(void);

			/**
			 * Método que, a partir de una zona de memoria con información de píxeles, crea un objeto de textura
			 * GXTexObj, con el cual puede trabajar la biblioteca de bajo nivel GX. El formato de la zona de memoria
			 * debe ser una secuencia de píxeles, con 24 bits por píxel (ocho bits por color, primero rojo, seguido
			 * de verde y finalmente azul). Las medidas de la imagen formada por la información de estos píxeles
			 * debe tener medidas (alto y ancho) múltiplos de 8.
			 * @param textura Dirección de memoria a un objeto GXTexObj recién creado.
			 * @param pixeles Dirección a la zona de memoria donde se encuentra la información de la imagen.
			 * @param ancho Ancho en píxeles que tiene la imagen que se convertirá en textura.
			 * @param alto Alto en píxeles que tiene la imagen que se convertirá en textura.
			 */
			void crearTextura(GXTexObj* textura, void* pixeles, u16 ancho, u16 alto);

			/**
			 * Método que dibuja una textura en formato GXTexObj en unas coordenadas (x,y,z) de la pantalla. En
			 * esas coordenadas se dibujará la esquina superior izquierda de la textura.
			 * @param textura Dirección de memoria de la textura que se va a dibujar.
			 * @param x Coordenada X donde se comenzará a dibujar la textura.
			 * @param y Coordenada Y donde se comenzará a dibujar la textura.
			 * @param z Coordenada Z (capa) donde se comenzará a dibujar la textura. Entre 0 y 999.
			 * @param ancho Ancho en píxeles de la textura a dibujar
			 * @param alto Alto en píxeles de la textura a dibujar
			 */
			void dibujarTextura(GXTexObj* textura, s16 x, s16 y, s16 z, u16 ancho, u16 alto);

			/**
			 * Método que dibuja una parte de una textura en formato GXTexObj en unas coordenadas (x,y,z)
			 * de la pantalla. En esas coordenadas se dibujará la esquina superior izquierda de la zona de la
			 * textura que se quiera dibujar. Dicha zona viene determinada por unas coordenadas (x,y), que indican,
			 * respectivamente, la distancia en horizontal al lado izquierdo de la textura, y en vertical al lado
			 * superior de ésta, del punto superior izquierdo del cuadro de la textura que se va a dibujar. Este
			 * cuadro también se define por su alto y ancho en píxeles.
			 * @param textura Dirección de memoria de la textura que se va a dibujar.
			 * @param texAncho Ancho en píxeles de la textura a dibujar
			 * @param texAlto Alto en píxeles de la textura a dibujar
			 * @param x Coordenada X donde se comenzará a dibujar la textura.
			 * @param y Coordenada Y donde se comenzará a dibujar la textura.
			 * @param z Coordenada Z (capa) donde se comenzará a dibujar la textura. Entre 0 y 999.
			 * @param cuadroX Coordenada X (interna a la textura) donde comienza el cuadro a dibujar.
			 * @param cuadroY Coordenada Y (interna a la textura) donde comienza el cuadro a dibujar.
			 * @param cuadroAncho Ancho en píxeles del cuadro de la textura que se va a dibujar.
			 * @param cuadroAlto Alto en píxeles del cuadro de la textura que se va a dibujar.
			 * @param invertido Verdadero si se quiere dibujar el cuadro de la textura invertido respecto al eje vert.
			 */
			void dibujarCuadro(GXTexObj* textura, u16 texAncho, u16 texAlto, s16 x, s16 y, s16 z,
								s16 cuadroX, s16 cuadroY, u16 cuadroAncho, u16 cuadroAlto, bool invertido = false);

			/**
			 * Método que dibuja un punto de un color concreto en unas coordenadas (x,y,z).
			 * @param x Coordenada X donde se dibujará el punto.
			 * @param y Coordenada Y donde se dibujará el punto.
			 * @param z Coordenada Z (capa) donde se dibujará el punto. Entre 0 y 999.
			 * @param color Entero de 32 bits sin signo que indica el color del punto a dibujar. Formato 0xRRGGBBAA
			 */
			void dibujarPunto(s16 x, s16 y, s16 z, u32 color);

			/**
			 * Método que dibuja una recta rellena de un color concreto entre dos puntos.
			 * @param x1 Coordenada X del punto origen de la recta.
			 * @param y1 Coordenada Y del punto origen de la recta.
			 * @param x2 Coordenada X del punto final de la recta.
			 * @param y2 Coordenada Y del punto final de la recta.
			 * @param z Coordenada Z (capa) donde se dibujará la recta. Entre 0 y 999.
			 * @param ancho Ancho en píxeles de la recta
			 * @param color Entero de 32 bits sin signo que indica el color de la recta a dibujar. Formato 0xRRGGBBAA
			 */
			void dibujarLinea(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u16 ancho, u32 color);

			/**
			 * Método que dibuja un rectángulo relleno de un color concreto a partir de cuatro puntos. El orden de
			 * los puntos debe ser, obligatoriamente, izquierda-arriba, derecha-arriba, derecha-abajo, izquierda-abajo.
			 * Si no se sigue esta recomendación, puede darse un comportamiento inesperado.
			 * @param x1 Coordenada X del punto superior izquierdo del rectángulo.
			 * @param y1 Coordenada Y del punto superior izquierdo del rectángulo.
			 * @param x2 Coordenada X del punto superior derecho del rectángulo.
			 * @param y2 Coordenada Y del punto superior derecho del rectángulo.
			 * @param x3 Coordenada X del punto inferior derecho del rectángulo.
			 * @param y3 Coordenada Y del punto inferior derecho del rectángulo.
			 * @param x4 Coordenada X del punto inferior izquierdo del rectángulo.
			 * @param y4 Coordenada Y del punto inferior izquierdo del rectángulo.
			 * @param z Coordenada Z (capa) donde se dibujará el rectángulo. Entre 0 y 999.
			 * @param color Entero de 32 bits sin signo que indica el color del rectángulo. Formato 0xRRGGBBAA
			 */
			void dibujarRectangulo(s16 x1, s16 y1, s16 x2, s16 y2, s16 x3, s16 y3, s16 x4, s16 y4, s16 z, u32 color);

			/**
			 * Método que dibuja un círculo relleno de un color concreto a partir de un punto y un radio.
			 * @param x Coordenada X del centro del círculo.
			 * @param y Coordenada Y del centro del círculo.
			 * @param z Coordenada Z (capa) donde se dibujará el círculo. Entre 0 y 999.
			 * @param r Radio, en píxeles, del círculo.
			 * @param color Entero de 32 bits sin signo que indica el color del círculo. Formato 0xRRGGBBAA
			 */
			void dibujarCirculo(s16 x, s16 y, s16 z, f32 r, u32 color);

		protected:

			/**
			 * Constructor de la clase Screen. En la zona protegida debido a la implementación del patrón Singleton.
			 * La llamada al constructor inicializa el sistema de vídeo de la Nintendo Wii.
			 * Fija la constante FIFO_SIZE, que será el tamaño del bus que envía los datos al procesador 
			 * gráfico en cada frame.
			 */
			Screen(void): FIFO_SIZE(1024 * 1024) { };

			/**
			 * Destructor de la clase Screen. Se encuentra en la zona protegida debido a la implementación 
			 * del patrón Singleton.
			 */			
			~Screen(void);

			/**
			 * Constructor de copia de la clase Screen. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton (evita la copia).
			 */
			Screen(const Screen& s);

			/**
			 * Operador de asignación de la clase Screen. Se encuentra en la zona protegida debido a la
			 * implementación del patrón Singleton (evita la copia)
			 */
			Screen& operator=(const Screen& s);

		private:

			// Instancia del patrón Singleton
			static Screen* _instance;

			// Puntero al modo de pantalla
			GXRModeObj* _screenMode;
			// Dos buffers de frame para el sistema de doble buffer
			void* _frameBuffer[2];
			// Buffer para pasar la información de cada frame al procesador gráfico
			void* _fifoBuffer;
			// Tamaño del fifoBuffer
			const u32 FIFO_SIZE;
			// Color de fondo por defecto
			GXColor _backgroundColor;
			// Matriz de proyección
			Mtx44 _proyeccion;
			// Matriz mundial
			Mtx	_modelView;
			// Alto de la pantalla
			u16 _alto_pantalla;
			// Ancho de la pantalla
			u16 _ancho_pantalla;
			// Indica el buffer actual (0 o 1)
			u8 _frame_actual;
			// Indicador de si hace falta actualizar la pantalla o no
			u8 _update_scr;
			// Flag que indica si los gráficos se han actualizado en este frame
			u8 _update_gfx;

			// Método para crear tiles de 4x4 a partir de una memoria de pixeles
			void tiling4x4(void* mem, u16 ancho, u16 alto);
			// Método para calcular el seno de un ángulo respecto a 16384 (no respecto a 360º)
			u32 seno(u32 ang);
			// Método para calcular el coseno de un ángulo respecto a 16384 (no respecto a 360º)
			u32 coseno(u32 ang);
			// Método que prepara el procesador gráfico para dibujar color directo (en formato u32)
			void configurarColor(void);
			// Método que prepara el procesador gráfico para dibujar una textura
			void configurarTextura(GXTexObj* textura, u8 escala);
	};

	#define screen Screen::get_instance()

#endif
