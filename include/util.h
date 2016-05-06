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

#ifndef _UTIL_H_
#define _UTIL_H_

	#include <gctypes.h>
	#include <malloc.h>
	#include <ogc/lwp_watchdog.h>
	#include <string>
	#include <unistd.h>

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Grupo de funciones para transformar variables desde big endian a little endian, y viceversa
	 *
	 * @details El procesador de la Nintendo Wii trabaja con big endian como representación interna de los bytes, al
	 * contrario que las platadormas Intel, que utilizan little endian. Esto significa que, para poder leer información
	 * de un fichero importado desde una plataforma Intel, hay que invertir el orden de los bytes que se vayan leyendo.
	 * Las funciones que se presentan en este espacio de nombres sirven para invertir el orden de variables de 16
	 * y 32 bytes, de tal manera que la Nintendo Wii pueda leer ficheros binarios importados desde un PC.
	 *
	 */
	namespace endian
	{
		/**
		 * Cambia el endian de una variable de 16 bits
		 * @param a Variable de 16 bits a la que se desea cambiar el endian
		 * @return Valor de la variable con el endian cambiado
		 */
		u16 inline swap16(u16 a)
		{
			return ((a<<8) | (a>>8));
		}

		/**
		 * Cambia el endian de una variable de 32 bits
		 * @param a Variable de 32 bits a la que se desea cambiar el endian
		 * @return Valor de la variable con el endian cambiado
		 */
		u32 inline swap32(u32 a)
		{
			return ((a)<<24 | (((a)<<8) & 0x00FF0000) | (((a)>>8) & 0x0000FF00) | (a)>>24);
		}
	}

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Grupo de funciones para controlar el tiempo en una aplicación.
	 * 
	 * @details Con las funciones incluidas en este espacio de nombres, se pueden controlar fácilmente el tiempo
	 * transcurrido entre dos puntos de la aplicación; aplicado a un videojuego, se puede retardar la ejecución del
	 * programa el periodo necesario para mantener una tasa de fotogramas por segundo constante.
	 * 
	 */
	namespace tiempo
	{
		/**
		 * Variable para almacenar una medida de tiempo. Definirla a cero antes de entrar en el bucle principal
		 */
		static u32 tick;

		/**
		 * Función que retarda la ejecución del programa el tiempo necesario para mantener los fps constantes
		 * @param fps Frames por segundo que se quieren mantener constantes
		 */
		void inline controlarFps(const u8 fps)
		{
			u32 nuevo = (gettick()/(u32)TB_TIMER_CLOCK);
			if((nuevo - tick) < 1000/fps)
				usleep((1000/fps - (nuevo - tick)) * 1000);
			tick = nuevo;
		}
	}

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Grupo de funciones para manejar los colores con el formato 0xRRGGBBAA
	 * 
	 * @details Gracias a este grupo de funciones, resulta muy cómodo trabajar con los colores de los píxeles, ya que
	 * permiten tanto formar un color a partir de sus tres componentes, como obtener un componente (rojo, verde
	 * o azul) a partir de un color completo.
	 * 
	 */
	namespace pixel
	{

		/**
		 * Función para formar un color a partir de sus tres componentes
		 * @param r Componente rojo del color que se quiere formar
		 * @param g Componente verde del color que se quiere formar
		 * @param b Componente azul del color que se quiere formar
		 * @return Color en formato 0xRRGGBBAA, con el componente alpha visible al 100%
		 */
		u32 inline color(u8 r, u8 g, u8 b)
		{
			return (u32) (((u32)r) <<24) | ((((u32)g) &0xFF) <<16) | ((((u32)b) &0xFF) << 8) | (((u32)0xFF) &0xFF);
		}

		/**
		 * Función para obtener el componente rojo de un color en formato 0xRRGGBBAA
		 * @param color Color del cual se quiere obtener el componente rojo
		 * @return Componente rojo del color que se recibe
		 */
		u8 inline rojo(u32 color)
		{
			return (u8) ((color>>24) &0xFF);
		}

		/**
		 * Función para obtener el componente verde de un color en formato 0xRRGGBBAA
		 * @param color Color del cual se quiere obtener el componente verde
		 * @return Componente verde del color que se recibe
		 */
		u8 inline verde(u32 color)
		{
			return (u8) ((color>>16) &0xFF);
		}

		/**
		 * Función para obtener el componente azul de un color en formato 0xRRGGBBAA
		 * @param color Color del cual se quiere obtener el componente azul
		 * @return Componente azul del color que se recibe
		 */
		u8 inline azul(u32 color)
		{
			return (u8) ((color>>8) &0xFF);
		}
	}

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Grupo de funciones para convertir cadenas de caracteres a distintos formatos
	 * 
	 * @details Con este grupo de funciones se puede cambiar el formato a una cadena de caracteres de alto nivel, de tal
	 * manera que se pueda representar en UTF32, y por tanto, pueda almacenar caracteres de otros alfabetos que
	 * no sean los contemplados en la tabla ASCII original.
	 *
	 */
	namespace utf32
	{
		/**
		 * Función para cambiar la codificación a un std::string, de tal manera que se represente en un std::wstring
		 * con formato UTF32. La conversión se realiza reservando la memoria alineada necesaria para almacenar el
		 * mismo número de caracteres que tiene la cadena std::string original, pero con el tipo base wchar_t. Se
		 * copia la cadena original entera a esta zona de memoria reservada con la función mbstowcs, que cambia la
		 * codificación, se añade un carácter de fin de cadena al final y se crea una cadena de alto nivel std::wstring
		 * a partir de esta cadena de bajo nivel de caracteres anchos. Por último, se libera la memoria ocupada por
		 * la cadena de bajo nivel temporal.
		 * @param cadena Cadena de caracteres que se quiere codificar en UTF32
		 * @return Cadena de caracteres con codificación utf32
		 */
		std::wstring inline convertir(const std::string& cadena)
		{
			wchar_t *utf32 = (wchar_t*)memalign(32, (cadena.length() + 1) * sizeof(wchar_t));
			u32 length = mbstowcs(utf32, cadena.c_str(), cadena.length() + 1);
			utf32[length] = L'\0';
			std::wstring cadena_utf32(utf32);
			free(utf32);
			return cadena_utf32;
		}
	}

#endif

