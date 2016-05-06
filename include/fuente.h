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

#ifndef _FUENTE_H_
#define _FUENTE_H_

	#include <gctypes.h>
	#include <string>
	#include "excepcion.h"
	#include "ft2build.h"
	#include "screen.h"
	#include "sdcard.h"
	#include "util.h"
	#include FT_FREETYPE_H

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase que permite gestionar una fuente de texto a través de FreeType2
	 *
	 * @details Esta clase está relacionada con la carga de archivos y utilización de fuentes de texto, con el objetivo
	 * de proporcionar de una manera sencilla y eficaz las herramientas necesarias para escribir textos en la pantalla.
	 * La biblioteca que sirve de base para trabajar con los archivos de fuentes es un port de FreeType2 a Nintendo
	 * Wii, de tal manera que estarán soportados todos los formatos de archivos de fuentes que estén contemplados en
	 * esta biblioteca.
	 *
	 * Cada instancia de esta clase representa un único archivo de fuentes de texto cargado en memoria y listo para ser
	 * utilizado. Los archivos de fuentes se almacenarán en la tarjeta SD de forma dinámica, de tal manera que en
	 * cualquier punto del programa se podrá acceder a la tarjeta SD y cargar una nueva fuente de texto en el sistema.
	 *
	 * Las cadenas "tradicionales" std::string trabajan con un byte por carácter (es decir, con un carácter base de tipo
	 * char, 8 bits). En el formato Unicode, sin embargo, un carácter puede ocupar hasta cuatro caracteres, con lo cual
	 * se hace evidente que hay que utilizar otra estructura de datos para poder dar soporte a otros alfabetos además
	 * del latino. La solución es utilizar cadenas con caracteres base que puedan almacenar varios bytes por carácter,
	 * con el tipo wchar_t.
	 *
	 * Se ha implementado soporte para escribir en pantalla caracteres Unicode de hasta 32 bits, para lo cual se utiliza
	 * la clase std::wstring, que no es más que una std::string pero utilizando, en lugar de char como carácter base, el
	 * tipo wchar_t. Debido a ello, se pueden escribir en la pantalla todos los caracteres existentes, desde el alfabeto
	 * latino, hasta kanjis japoneses, pasando por cirílico y más.
	 *
	 * Para transformar una std::string en una std::wstring se reserva memoria alineada de tipo wchar_t, para tantos
	 * caracteres como tenga la cadena original. Después se utiliza la función mbstowcs(), que transforma una cadena con
	 * caracteres de un sólo byte (pero que puede estar almacenando caracteres multibyte, sólo que éstos estarían
	 * repartidos entre varios caracteres) en una cadena de caracteres de 32 bits por carácter.
	 *
	 * Una vez tenemos una cadena de caracteres de 32 bits por carácter con el texto que se quiere escribir en la
	 * pantalla, el proceso para hacerlo es sencillo. Se trata de recorrer el texto, carácter a carácter, extrayendo el
	 * bitmap asociado a un carácter cada vez (esta funcionalidad la proporciona FreeType2, ya que un archivo de fuentes
	 * contiene un gráfico vectorial por cada carácter, a partir del cual se obtiene el mapa de bits), y dibujando el
	 * bitmap en las coordenadas correspondientes de la pantalla. Este dibujo se realiza píxel a píxel, ya que la imagen
	 * extraída tiene píxeles coloreados (los que componen el propio carácter) y píxeles con valor cero (un mapa de bits
	 * es rectangular, de tal manera que hay píxeles invisibles, y éstos son los que tiene el valor nulo).
	 *
	 * Es muy importante tener claro que, para poder utilizar un juego de caracteres concreto, éste debe estar
	 * contemplado en el archivo de fuentes (es decir, que si se quiere escribir un texto en chino mandarín, el archivo
	 * de fuentes debe soportar chino mandarín, en otro caso, se escribirán caracteres no esperados en la pantalla).
	 *
	 * Por último, un detalle que se ha contemplado, es que si el archivo de fuentes soporta kerning, éste se utiliza
	 * para todos los caracteres. El kerning es una funcionalidad que permite que los caracteres no tengan todos la
	 * misma separación, sino que, según el carácter, la separación entre estos sea la adecuada (para ilustrar esta
	 * situación, basta con pensar que el carácter ‘i’ no tiene el mismo ancho que el carácter ‘m’, y si la separación
	 * entre caracteres es la misma, puede no quedar bien el que algunas letras se vean con más separación que otras en
	 * una misma palabra).
	 *
	 * Funcionamiento interno
	 *
	 * Antes de poder utilizar ninguna fuente de texto, hay que inicializar la biblioteca FreeType2, mediante el método
	 * de clase Fuente::inicializar().
	 *
	 * El constructor de la clase se ocupa de cargar un archivo de fuentes alojado en la SD (lo primero que hace es
	 * comprobar que la unidad de la tarjeta SD esté montada y lista para ser utilizada). Se utiliza una función de la
	 * propia biblioteca FreeType2 para realizar la carga, ya que el port de ésta a Wii abstrae de tener que preocuparse
	 * por el Endian, la memoria alineada y demás detalles.
	 *
	 * Una vez cargada la fuente, para escribir un texto, ya sea desde un std::string o desde un std::wstring, basta con
	 * indicar el texto a escribir, las coordenadas (x, y, z) en las cuales se quiere escribir el texto (las coordenadas
	 * corresponden a la esquina superior izquierda del texto, para cualquier duda sobre el sistema de coordenadas,
	 * consultar la documentación de la clase Screen), el tamaño de los caracteres, y el color de relleno de éstos, en
	 * formato hexadecimal 0xRRGGBBAA (consultar clase Screen para el formato de color).
	 *
	 * Es importante tener en cuenta que si el texto escrito se sale de la pantalla, la clase no proporciona ninguna
	 * manera de saberlo, así que si se quiere controlar esta situación, se tienen que hacer los cálculos antes de
	 * llamar a los métodos de escritura.
	 *
	 * Cada píxel del mapa de bits generado por FreeType2 que se vaya a dibujar, se hará con el método dibujarPunto() de
	 * la clase Screen.
	 *
	 * Por último, el destructor de la clase se encarga de liberar la memoria ocupada por las estructuras internas
	 * (concretamente, una estructura propia de FreeType2 que se genera en el constructor).
	 *
	 * Sencillo ejemplo de uso
	 *
	 * @code
	 * // Inicializar la clase
	 * Fuente::inicializar();
	 * // Cargar la fuente Verdana
	 * Fuente verdana( "/apps/wiipang/media/verdana.ttf" );
	 * // Escribir un texto con caracteres latinos, tamaño 30, coordenadas (50,50,4) y en color rojo
	 * verdana.escribir( "Hola", 30, 50, 50, 4, 0xFF0000FF);
	 * @endcode
	 *
	 */
	class Fuente
	{
		public:

			/**
			 * Variable de clase que almacena el manejador de la biblioteca FreeType2
			 */
			static FT_Library library;

			/**
			 * Método de clase para inicializar la biblioteca de fuentes.
			 */
			static void inicializar(void) {
				FT_Init_FreeType(&library);
				atexit(apagar);
			};

			/**
			 * Método de clase para "apagar" la biblioteca de fuentes.
			 */
			static void apagar(void) {
				FT_Done_FreeType(library);
			};

			/**
			 * Constructor de la clase Fuente. Lee un archivo de fuentes desde la tarjeta SD
			 * @param ruta Ruta absoluta hasta el archivo de fuentes
			 * @throw ArchivoEx Se lanza si hay algún error al abrir el archivo al que apunta la ruta aportada
			 * @throw TarjetaEx Se lanza si ocurre un error relacionado con la tarjeta SD
			 */
			Fuente(const std::string& ruta) throw (ArchivoEx, TarjetaEx);

			/**
			 * Método para escribir en pantalla un texto con la fuente almacenada en la instancia.
			 * @param texto Cadena de texto de alto nivel que se quiere escribir en la pantalla
			 * @param tam Tamaño en píxeles con el que se quiere dibujar el texto
			 * @param x Coordenada X en pantalla del punto superior izquierdo del primer carácter que se dibujara
			 * @param y Coordenada Y en pantalla del punto superior izquierdo del primer carácter que se dibujara
			 * @param z Capa en la que se dibujará el texto (primer plano es 0, fondo es 999)
			 * @param color Color en el que se quiere dibujar el texto, en formato 0xRRGGBBAA
			 */
			void escribir(const std::string& texto, u8 tam, s16 x, s16 y, s16 z, u32 color) const;

			/**
			 * Método para escribir en pantalla un texto de 32 bits por carácter con la fuente de la instancia.
			 * @param texto Cadena de texto de alto nivel de 32 bits que se quiere escribir en la pantalla
			 * @param tam Tamaño en píxeles con el que se quiere dibujar el texto
			 * @param x Coordenada X en pantalla del punto superior izquierdo del primer carácter que se dibujara
			 * @param y Coordenada Y en pantalla del punto superior izquierdo del primer carácter que se dibujara
			 * @param z Capa en la que se dibujará el texto (primer plano es 0, fondo es 999)
			 * @param color Color en el que se quiere dibujar el texto, en formato 0xRRGGBBAA
			 */
			void escribir(const std::wstring& texto, u8 tam, s16 x, s16 y, s16 z, u32 color) const;

			/**
			 * Destructor de la clase Fuente
			 */
			~Fuente(void);

		private:
			// Función que dibuja un carácter concreto, en unas coordenadas y un color determinados
			void dibujarCaracter(FT_Bitmap *bitmap, s16 x, s16 y, s16 z, u32 color) const;
			FT_Face _face;
			bool _kerning;
	};

#endif
