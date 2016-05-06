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

#include "musica.h"
using namespace std;

Musica::Musica(const string& ruta, u8 volumen) throw (ArchivoEx, TarjetaEx):
_volumen(volumen)
{
	if(not sdcard->montada())
		throw TarjetaEx("Musica - La tarjeta SD no está montada.");

	// Añadir a la ruta la unidad que esté montada en la clase sdcard como prefijo
	string ruta_completa = sdcard->unidad() + ":" + ruta;

	// Comprobar la existencia del archivo
	if(not sdcard->existe(ruta_completa))
		throw ArchivoEx("Musica - El archivo '" + ruta + "' no existe.");

	// Cargar el archivo en un flujo de fichero
	ifstream archivo;
	archivo.open(ruta_completa.c_str(), ios::binary);

	if(not archivo.good())
		throw ArchivoEx("Musica - Error al abrir el archivo: " + ruta);

	// Obtener el tamaño de la pista
	archivo.seekg(0, ios::end);
	_size = archivo.tellg();
	archivo.seekg(0, ios::beg);

	// Fijar el padding a 32 bytes
	u8 padding = (_size * sizeof(s16)) % 32;

	// Reservar memoria alineada a 32 bytes para el sonido
	_musica = (s16*)memalign(32, _size * sizeof(s16) + padding);
	// Volcar el contenido del flujo en la memoria reservada
	archivo.read((char*)_musica, _size);
	// Fijar en la zona de memoria alineada la información de la pista de música que se ha leído desde la cache
	DCFlushRange(_musica, _size * sizeof(s16) + padding);

	// Una vez cargada la pista de música, se cierra el flujo de fichero
	archivo.close();
}

Musica::~Musica(void)
{
	stop();
	free(_musica);
}

void Musica::play(void) const
{
	stop();
	MP3Player_Volume(_volumen);
	MP3Player_PlayBuffer(_musica, _size, NULL);
}

void Musica::stop(void) const
{
	if(reproduciendo())
		MP3Player_Stop();
}

void Musica::loop(void) const
{
	if(not reproduciendo())
		MP3Player_PlayBuffer(_musica, _size, NULL);
}

bool Musica::reproduciendo(void) const
{
	return MP3Player_IsPlaying();
}

void Musica::setVolumen(u8 volumen)
{
	_volumen = volumen;
}

