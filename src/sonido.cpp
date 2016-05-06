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

#include "sonido.h"
using namespace std;

Sonido::Sonido(const string& ruta, u8 volder, u8 volizq) throw (ArchivoEx, TarjetaEx):
_volder(volder), _volizq(volizq)
{
	if(not sdcard->montada())
		throw TarjetaEx("Sonido - La tarjeta SD no está montada.");

	// Añadir a la ruta la unidad que esté montada en la clase sdcard como prefijo
	string ruta_completa = sdcard->unidad() + ":" + ruta;

	// Comprobar la existencia del archivo
	if(not sdcard->existe(ruta_completa))
		throw ArchivoEx("Sonido - El archivo '" + ruta + "' no existe.");

	// Cargar el archivo en un flujo de fichero
	ifstream archivo;
	archivo.open(ruta_completa.c_str(), ios::binary);

	if(not archivo.good())
		throw ArchivoEx("Sonido - Error al abrir el archivo: " + ruta);

	// Obtener el tamaño del sonido
	archivo.seekg(0, ios::end);
	_size = archivo.tellg();
	archivo.seekg(0, ios::beg);

	// Fijar el padding a 32 bytes
	u8 padding = (_size * sizeof(s16)) % 32;

	// Reservar memoria alineada a 32 bytes para el sonido
	_sonido = (s16*)memalign(32, _size * sizeof(s16) + padding);
	// Volcar el contenido del flujo en la memoria reservada
	archivo.read((char*)_sonido, _size);
	// Fijar en la zona de memoria alineada la información del sonido que se ha leído desde la cache
	DCFlushRange(_sonido, _size * sizeof(s16) + padding);

	// Una vez cargado el sonido, se cierra el flujo de fichero
	archivo.close();
}

Sonido::~Sonido(void)
{
	free(_sonido);
}

bool Sonido::play(void) const
{
	if(ASND_SetVoice(ASND_GetFirstUnusedVoice(), VOICE_STEREO_16BIT, 48000, 0, _sonido, _size, _volizq, _volder, NULL) == SND_OK)
		return true;
	return false;
}

void Sonido::setVolumenIzquierdo(u8 volumen)
{
	_volizq = volumen;
}

void Sonido::setVolumenDerecho(u8 volumen)
{
	_volder = volumen;
}

