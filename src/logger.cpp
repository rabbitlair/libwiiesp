/**
 * Licencia GPLv3
 *
 * Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
 *
 * libWiiEsp es software libre: usted puede redistribuirlo y/o modificarlo bajo los términos de la 
 * Licencia Pública General GNU publicada por la Fundación para el Software Libre, ya sea la versión 3 
 * de la Licencia, o (a su elección) cualquier versión posterior.
 *
 * libWiiEsp se distribuye con la esperanza de que sea útil, pero SIN GARANTÍA ALGUNA; ni siquiera 
 * la garantía implícita MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. Consulte los detalles de
 * la Licencia Pública General GNU para obtener una información más detallada.
 *
 * Debería haber recibido una copia de la Licencia Pública General GNU junto a libWiiEsp. En caso 
 * contrario, consulte <http://www.gnu.org/licenses/>.
 *
 */

#include "logger.h"
using namespace std;

Logger* Logger::_instance = 0;

void Logger::inicializar(const string& ruta, Nivel nivel) throw (ArchivoEx, TarjetaEx)
{
	_nivel = nivel;

	if(_nivel != OFF)
	{
		// Comprobar que la tarjeta SD está montada
		if(not sdcard->montada())
			throw TarjetaEx("Logger::inicializar - La tarjeta SD no está montada.");

		// Añadir a la ruta la unidad que esté montada en la clase sdcard como prefijo
		string ruta_completa = sdcard->unidad() + ":" + ruta;

		// Abrir el archivo de log en la ruta recibida
		_archivo.open(ruta_completa.c_str());

		// Comprobar que se puede escribir en el archivo de log
		if(not _archivo.good())
			throw ArchivoEx("Logger::inicializar - Error al abrir el archivo: " + ruta);

		_buffer.append("LOG iniciado\n");
	}
}

bool Logger::debug(void) const
{
	return (_nivel != OFF);
}

void Logger::info(const string& texto)
{
	if(_nivel == INFO and texto != "")
		_buffer.append("[INFO] " + texto + "\n");
}

void Logger::aviso(const string& texto)
{
	if((_nivel == INFO or _nivel == AVISO) and texto != "")
		_buffer.append("[AVISO] " + texto + "\n");
}

void Logger::error(const string& texto)
{
	if(_nivel != OFF and texto != "")
		_buffer.append("[ERROR] " + texto + "\n");
}

// Protegidos
Logger::~Logger(void)
{
	if(_archivo.is_open())
	{
		_buffer.append("\nLOG finalizado");
		_archivo << _buffer;
		_archivo.close();
	}
}

