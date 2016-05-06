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

#ifndef _LIBWIIESP_H_
#define _LIBWIIESP_H_

	#include "actor.h"
	#include "animacion.h"
	#include "colision.h"
	#include "excepcion.h"
	#include "fuente.h"
	#include "galeria.h"
	#include "imagen.h"
	#include "juego.h"
	#include "lang.h"
	#include "logger.h"
	#include "mando.h"
	#include "musica.h"
	#include "nivel.h"
	#include "parser.h"
	#include "screen.h"
	#include "sdcard.h"
	#include "sonido.h"
	#include "util.h"

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @mainpage LibWiiEsp
	 *
	 * @details LibWiiEsp es una biblioteca libre de desarrollo de juegos en dos dimensiones para Nintendo Wii. Pretende
	 * ser una aproximación sencilla, pero efectiva y genérica, al desarrollo para la consola de Nintendo,
	 * proporcionando una serie de subsistemas útiles a la hora de desarrollar todo tipo de juegos.
	 *
	 * LibWiiEsp está construida sobre el kit de desarrollo DevKitPro, y más concretamente su versión para PowerPC,
	 * denominada DevKitPPC (en su versión r21). DevKitPro ofrece una serie de compiladores, enlazadores, reglas de
	 * compilación, bibliotecas de C++ y útiles (wiiload addr2line, etc.) para compilar y ejecutar programas en una
	 * consola Nintendo Wii que disponga de HomeBrew Channel instalado (la versión del menú de la consola o los iOS de
	 * que disponga la consola no influyen en la ejecución del software generado con LibWiiEsp). Como base de
	 * desarrollo, se toma la biblioteca de bajo nivel libOgc (en su versión 1.8.4), junto con la adaptación al sistema
	 * de la consola de libfat (versión 1.0.7). Ambas herramientas están disponibles en la forja del proyecto.
	 *
	 * LibWiiEsp está pensada para ser un conjunto de herramientas que faciliten la entrada del programador en el
	 * desarrollo para Nintendo Wii. La biblioteca, junto con su documentación completa, conforma un primer paso en la
	 * programación para la consola, pero esta primera versión no está pensada para ir mucho más allá que ser una
	 * introducción en el mundillo.
	 *
	 * Un último apunte; esta biblioteca no hubiera sido posible sin la ayuda de Hermes (elotrolado.net), quien es uno
	 * de los mayores sceners de Wii, y que ha desarrollado (junto a otros muchos) la biblioteca de bajo nivel libOgc,
	 * además de publicar varios tutoriales de iniciación a la programación para Wii.
	 * 
	 * 
	 * \section a Licencia GPLv3
	 *
	 * libWiiEsp es software libre: usted puede redistribuirlo y/o modificarlo bajo los términos de la 
	 * Licencia Pública General GNU publicada por la Fundación para el Software Libre, ya sea la versión 3 
	 * de la Licencia, o (a su elección) cualquier versión posterior.
	 *
	 * libWiiEsp se distribuye con la esperanza de que sea útil, pero SIN GARANTÍA ALGUNA; ni siquiera 
	 * la garantía implícita MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. Consulte los detalles de
	 * la Licencia Pública General GNU para obtener una información más detallada.
	 *
	 *
	 * \section b Autoría
	 *
	 * LibWiiEsp está desarrollada por Ezequiel Vázquez De la calle (ezequielvazq@gmail.com)
	 *
	 *
	 * \section c Detalles sobre instalación y uso
	 *
	 * Consultar el manual de la biblioteca en la forja del proyecto.
	 *
	 *
	 * \section d Enlaces
	 *
	 * Página del proyecto: http://libwiiesp.forja.rediris.es/
	 *
	 * Forja del proyecto: https://forja.rediris.es/projects/libwiiesp/
	 *
	 * Licencia GPLv3: http://www.gnu.org/licenses/gpl.html
	 *
	 */

#endif

