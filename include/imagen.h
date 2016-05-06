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

#ifndef _IMAGEN_H_
#define _IMAGEN_H_

	#include <fstream>
	#include <malloc.h>
	#include <string>
	#include "excepcion.h"
	#include "screen.h"
	#include "sdcard.h"
	#include "util.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que representa una imagen cargada en memoria, lista para ser dibujada en la pantalla.
	 * 
	 * @details Se puede decir que esta clase Imagen es una abstracción de todos los mecanismos del sistema de la
	 * consola Nintendo Wii que es necesario conocer para dibujar una textura en la pantalla. Su sencillo uso hace que,
	 * en lugar de complicadas funciones de dibujo, rectángulos, píxeles y demás, únicamente se deba tener en cuenta un
	 * archivo de mapa de bits, y unas coordenadas (x,y,z).
	 *
	 * Cada instancia de esta clase representa y gestiona la imagen de un archivo bitmap cargado en la memoria de la
	 * consola. Dicho bitmap se carga desde la tarjeta SD dinámicamente, pero existen ciertas limitaciones a la hora de
	 * trabajar con una imagen concreta.
	 *
	 * Para empezar, las medidas de la imagen (alto y ancho), en píxeles, deben ser, obligatoriamente, múltiplos de 8.
	 * Si este detalle no se tiene en cuenta, se produce un error del sistema en tiempo de ejecución. Esta restricción
	 * está relacionada con la organización de las texturas en tiles de 4×4 píxeles (ver clase Screen para más
	 * información), y el problema de cargar una textura cuyas medidas no sean múltiplos 8 es que, seguramente, se
	 * rompan los tiles formados, o que incluso no se pudiera reorganizar la información de píxeles de la textura en
	 * forma de tiles.
	 *
	 * El máximo tamaño para una textura es de 1024×1024 píxeles, y ello se debe a que ése es el tamaño máximo del
	 * buffer FIFO que se encarga de enviar la información de un frame al procesador gráfico (consultar la clase
	 * Screen). El formato para indicar el color transparente es el mismo que se utiliza en la clase Screen, es decir,
	 * un entero de 32 bits, sin signo, y con la forma 0xRRGGBBAA.
	 *
	 * Otra limitación importante, aunque esta ya no es de la consola si no de la propia biblioteca, es que, de momento,
	 * sólo se pueden cargar imágenes en formato bitmap (BMP), de 24 bits de color directo, y 8 bits por cada componente
	 * de color. El por qué de esta restricción es exclusivamente el tiempo que habría que dedicar a desarrollar métodos
	 * de carga para cualquier formato de BMP (algunos formatos de bitmap trabajan con color indirecto y paletas de
	 * color), JPG o PNG (como ejemplos más comunes). De todas formas, en sucesivas versiones de la clase se irán
	 * desarrollando estas funcionalidades para dotarla de mayor potencia.
	 *
	 * El formato de color para un píxel que utiliza de forma nativa la Nintendo Wii es RGB5A3. Este formato, de 16
	 * bits, consiste en que cada componente del píxel se representa con 5 bits, y el último bit es el canal alpha (la
	 * transparencia). Esto sería, sobre el papel, formato RGB5A1 (y de hecho, generalmente se comporta de la misma
	 * manera), pero la diferencia está en que, si el canal alpha está activado, la consola trabajará como si cada
	 * componente del píxel ocupara 4 bits (es decir, 4 bits para rojo, 4 bits para verde, 4 bits para azul, 3 bits para
	 * el canal alpha, y 1 bit para indicar que éste está activo). Los tres bits ‘extra’ del canal alpha se toman, cada
	 * uno, de una componente de color. 
	 *
	 * Por último, hay que tener en cuenta que la Wii trabaja en Big Endian, y que por lo tanto, hay que tratar todo
	 * aquello que se lea desde un dispositivo para que el Endian no ocasione conflictos. Para más información, 
	 * consultar el módulo de utilidades.
	 *
	 * Funcionamiento interno de la clase Imagen
	 *
	 * La clase Imagen se compone de cuatro atributos internos, y uno estático y público. Éste último es el color que se
	 * considerará como color transparente, y es compartido por todas las instancias de la clase. Los atributos internos
	 * son dos enteros de 16 bits sin signo, que indican el ancho y el alto de la imagen en píxeles, una dirección que
	 * apunta hacia la memoria donde se almacena la información de color de los píxeles que componen la imagen, y la
	 * dirección de un objeto GXTexObj, que es el atributo que realmente se utilizará para dibujar en la pantalla.
	 *
	 * El proceso de cargar una imagen es sencillo. En primer lugar, se crea una instancia de la clase, y después se
	 * llama al método cargarBMP(), pasándole como parámetro un std::string que indique la ruta absoluta hasta el
	 * archivo de imagen a cargar (debe estar en la SD). Cuando se vaya a implementar el soporte para otro formato de
	 * imagen, habrá que definir un método similar, pero específico para el formato nuevo, como por ejemplo cargarPNG()
	 * o cargarJPG().
	 *
	 * El método de carga de bitmaps comprueba primero que la tarjeta SD esté montada y operativa (ver clase Sdcard). A
	 * continuación, abre el archivo que se le ha indicado en la ruta, y trata de leer los 14 primeros bytes del archivo
	 * (que corresponden con la cabecera de archivo de todo bitmap). Se comprueba, mediante el campo de tipo de la
	 * cabecera, que el archivo es efectivamente un BMP, en caso contrario, se cierra el flujo de entrada y se lanza una
	 * excepción. Si todo va bien, se procede a leer los siguientes 40 bytes, que son la cabecera de la propia imagen
	 * (que no del archivo, que es la otra). Se toman de ésta los valores de ancho y alto en píxeles de la imagen, y la
	 * profundidad de color. Como ya se ha comentado antes, si el ancho y el alto no son múltiplos de ocho, no se puede
	 * continuar, y por tanto, se lanza una excepción.
	 *
	 * El dato de la profundidad de color es importante, ya que permite diferenciar entre los distintos formatos de
	 * BMP que existen. Ya se ha comentado que, de momento, sólo se da soporte a los bitmaps de 24 bits de color, así
	 * que, si el dato es correcto, se reserva la memoria alineada y se llama al método correspondiente; en caso
	 * contrario, se lanza una excepción.
	 *
	 * El método cargarBmp24() se encarga de la tarea específica de leer desde la entrada un bitmap de 24 bits. Si se
	 * quisiera ampliar la clase, dotándola de la posibilidad de leer otros formatos de bitmap, únicamente habría que
	 * añadir la comprobación en el método cargarBmp(), y crear el método específico de carga del formato.
	 *
	 * Un bitmap de 24 bits de color, con 8 bits de color directo por componente, se carga leyendo la información de la
	 * imagen de 3 bytes en 3 bytes. Posteriormente, se trabaja a nivel de bit (con operadores de desplazamiento) para
	 * transformar el formato de píxel del BMP (BGR) en RGB5A3, y se van guardando, en orden de llegada, los píxeles con
	 * este último formato en la zona de memoria reservada.
	 *
	 * Por último, se fijan los datos en la memoria desde la caché, y se crea el objeto de textura haciendo uso de la
	 * función que proporciona la clase Screen. Dibujar una imagen es algo trivial, ya que se recurre a los métodos
	 * dibujarTextura() o dibujarCuadro(), también de la clase Screen. Finalmente, mencionar que el destructor de la
	 * clase se encarga de liberar la memoria ocupada por la textura y la información de los píxeles.
	 *
	 * Un sencillo ejemplo de uso:
	 *
	 * @code
	 * // Crear una instancia de Imagen
	 * Imagen i;
	 * // Cargar un bitmap
	 * i.cargarBMP( "/apps/wiipang/media/imagen.bmp" );
	 * // Dibujar la textura en unas coordenadas (x, y, z)
	 * i.dibujar( 100, 200, 15 );
	 * @endcode
	 *
	 */
	class Imagen
	{
		public:

			/**
			 * Variable de clase que determina el color considerado transparente, y que por tanto será invisible.
			 */
			static u32 alpha;

			/**
			 * Constructor de la clase Imagen. El objeto creado tiene todos sus atributos a cero.
			 */
			Imagen(void);

			/**
			 * Función para cargar una imagen desde un archivo bitmap que debe estar en la tarjeta SD.
			 * De momento, sólo se da soporte a los bitmaps de 24 bits de color directo (sin paleta).
			 * @param ruta Ruta absoluta hasta el fichero que contiene la imagen.
			 * @throw ArchivoEx Se lanza si hay algún error al abrir el archivo al que apunta la ruta aportada
			 * @throw ImagenEx Se lanza si sucede un error relacionado con la propia imagen
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			void cargarBmp(const std::string& ruta) throw (ArchivoEx, ImagenEx, TarjetaEx);

			/**
			 * Dibuja la imagen en la pantalla en unas coordenadas (x,y,z).
			 * @param x Coordenada X del punto superior izquierdo del lugar donde se quiere dibujar la imagen.
			 * @param y Coordenada Y del punto superior izquierdo del lugar donde se quiere dibujar la imagen.
			 * @param z Coordenada Z (capa) en la que se dibujará la imagen. Entre 0 y 999.
			 * @return Verdadero si se ha dibujado la imagen, y falso en caso contrario.
			 */
			bool dibujar(s16 x, s16 y, s16 z) const;

			/**
			 * Método observador para el ancho en píxeles de la imagen.
			 * @return Número de píxeles de ancho de la imagen.
			 */
			u16 ancho(void) const;

			/**
			 * Método observador para el alto en píxeles de la imagen.
			 * @return Número de píxeles de alto de la imagen.
			 */
			u16 alto(void) const;

			/**
			 * Método observador para la textura de la imagen.
			 * @return Objeto de textura de la imagen.
			 */
			GXTexObj* textura(void) const;

			/**
			 * Destructor de la clase Imagen. Libera la memoria ocupada por la textura y la información
			 * de los píxeles.
			 */
			~Imagen(void);

		protected:

			/**
			 * Constructor de copia de la clase Imagen. Se encuentra en la zona protegida para no permitir
			 * la copia.
			 */
			Imagen(const Imagen& i);

			/**
			 * Operador de asignación de la clase Imagen. Se encuentra en la zona protegida para no permitir
			 * la asignación.
			 */
			Imagen& operator=(const Imagen& i);

		private:
			#pragma pack(push,bitmap_data,1)
			// Estructura para la cabecera de un archivo BMP
			typedef struct tagBitmapFileHeader
			{
				u16 bfType;
				u32 bfSize;
				u16 bfReserved1;
				u16 bfReserved2;
				u32 bfOffBits;
			} BitmapFileHeader;

			// Estructura para la cabecera de una imagen BMP
			typedef struct tagBitmapInfoHeader
			{
				u32 biSize;
				u32 biWidth;
				u32 biHeight;
				u16 biPlanes;
				u16 biBitCount;
				u32 biCompression;
				u32 biSizeImage;
				u32 biXPelsPerMeter;
				u32 biYPelsPerMeter;
				u32 biClrUsed;
				u32 biClrImportant;
			} BitmapInfoHeader;
			#pragma pack(pop,bitmap_data)

			// Cargar un BMP con color de 24 bits (8 bits por componente)
			// Debe ser llamada únicamente desde CargarBmp()
			void cargarBmp24(std::ifstream& archivo);
			// Para dejar el objeto imagen como recién creado, liberando la memoria ocupada
			void reset(void);

			GXTexObj* _imagen;
			u16* _pixelData;
			u16 _alto;
			u16 _ancho;
	};

#endif

