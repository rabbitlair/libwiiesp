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

#include "imagen.h"
using namespace std;

u32 Imagen::alpha = 0;

Imagen::Imagen(void)
{
	_imagen = NULL;
	_pixelData = NULL;
	reset();
}

void Imagen::cargarBmp(const string& ruta) throw (ArchivoEx, ImagenEx, TarjetaEx)
{
	reset();

	if(not sdcard->montada())
		throw TarjetaEx("Imagen::cargarBmp - La tarjeta SD no está montada.");

	// Añadir a la ruta la unidad que esté montada en la clase sdcard como prefijo
	string ruta_completa = sdcard->unidad() + ":" + ruta;

	// Comprobar la existencia del archivo
	if(not sdcard->existe(ruta_completa))
		throw ArchivoEx("Imagen::cargarBmp - El archivo '" + ruta + "' no existe.");

	// Cargar el archivo en un flujo de fichero
	ifstream archivo;
	archivo.open(ruta_completa.c_str(), ios::binary);

	if(not archivo.good())
		throw ArchivoEx("Imagen::cargarBmp - Error al abrir el archivo: " + ruta);

	// Leer la cabecera del fichero
	BitmapFileHeader bmfh;
	archivo.rdbuf()->sgetn((char*)&bmfh, sizeof(BitmapFileHeader));

	// Si el bfType no es este valor, entonces el archivo no contiene un BitMap
	if(endian::swap16(bmfh.bfType) != 19778)
	{
		archivo.close();
		throw ImagenEx("Imagen::cargarBmp - El archivo no es un mapa de bits");
	}

	// Leer la cabecera de información de la imagen
	BitmapInfoHeader bmih;
	archivo.rdbuf()->sgetn((char*)&bmih, sizeof(BitmapInfoHeader));
	_ancho = (u16)endian::swap32(bmih.biWidth);
	_alto = (u16)endian::swap32(bmih.biHeight);
	u16 bpp = endian::swap16(bmih.biBitCount);

	if(_ancho % 8 != 0 or _alto % 8 != 0)
	{
		archivo.close();
		throw ImagenEx("Imagen::cargarBmp - Las medidas de la imagen no son múltiplo de 8");
	}

	// Número de píxeles
	u32 pixel_num = _alto * _ancho;

	// Fijar el padding a 32 bytes
	u8 padding = (pixel_num * sizeof(s16)) % 32;

	// Reservar la memoria necesaria para la imagen
	// En el formato RGB5A3, cada pixel tendra 16 bits
	_pixelData = (u16*)memalign(32, pixel_num * sizeof(u16) + padding);

	// Si no es un BMP de 24 bits, lanzar excepción
	if(bpp == 24)
		cargarBmp24(archivo);
	else
	{
		archivo.close();
		reset();
		throw ImagenEx("Imagen::cargarBmp - Formato de BMP no soportado (sólo 24 bits)");
	}

	// Fijar en la zona de memoria alineada la información de la imagen que se ha leído desde la cache
	DCFlushRange(_pixelData, pixel_num * sizeof(u16) + padding);

	// Crear el objeto de textura y guardar en él la información de la imagen en píxeles
	_imagen = new GXTexObj;
	screen->crearTextura(_imagen, _pixelData, _ancho, _alto);
}

bool Imagen::dibujar(s16 x, s16 y, s16 z) const
{
	if(_imagen != NULL and _pixelData != NULL and z < 1000)
	{
		screen->dibujarTextura(_imagen, x, y, z, _ancho, _alto);
		return true;
	}
	return false;
}

u16 Imagen::ancho(void) const
{
	return _ancho;
}

u16 Imagen::alto(void) const
{
	return _alto;
}

GXTexObj* Imagen::textura(void) const
{
	return _imagen;
}

Imagen::~Imagen(void)
{
	reset();
}

// Métodos privados
void Imagen::cargarBmp24(ifstream & archivo)
{
	// Desplazamiento: una imagen BMP tendrá desplazamiento en cada fila de 
	// píxeles si el tamaño de ésta no es múltiplo de 4 (en bytes)
	u8 offset = _ancho % 4;

	// La posición de cada pixel se tiene que calcular antes de insertarlo en la memoria alineada,
	// ya que un BMP hay que leerlo de abajo a arriba y de izquierda a derecha
	u32 pixel_count = 0;
	u32 pixel_num = _alto * _ancho;
	u32 y = _alto - 1;
	u32 x = 0;

	// Variables para almacenar los distintos componentes de color de cada pixel de la imagen
	u8 b = 0;
	u8 g = 0;
	u8 r = 0;

	while(pixel_count < pixel_num)
	{
		// Leer la información de un pixel (un BMP tiene la ordenación de color
		// BGR en lugar de lo común RGB)
		archivo.rdbuf()->sgetn((char*)&b, sizeof(u8));
		archivo.rdbuf()->sgetn((char*)&g, sizeof(u8));
		archivo.rdbuf()->sgetn((char*)&r, sizeof(u8));

		// Procesar el pixel y almacenarlo en la memoria alineada
		// Si el pixel es del color alpha, se toman sólo los 4 bits de mayor peso de cada color, y se establecen
		// 4 bits de alpha con valor 0 (invisible)
		// Si el pixel no es del color alpha, se toman los 5 bits de mayor peso de cada color, y se establece un
		// bit de alpha con valor 1 (visible)
		if(r == pixel::rojo(Imagen::alpha) and g == pixel::verde(Imagen::alpha) and b == pixel::azul(Imagen::alpha))
			_pixelData[_ancho * y + x] = (b>>4) | ((g>>4)<<4) | ((r>>4)<<8) | (0<<13);
		else
			_pixelData[_ancho * y + x] = (b>>3) | ((g>>3)<<5) | ((r>>3)<<10) | (1<<15);
		pixel_count++;
		x++;

		// Al final de cada fila de pixeles (en el BMP), puede haber desplazamiento (ya que el número de bytes de una
		// fila debe ser múltiplo de 4 bytes). Estas instrucciones son para saltar dicho desplazamiento.
		if(pixel_count % _ancho == 0)
		{
			x = 0;
			y--;
			for(u8 j = 0 ; j < offset ; ++j)
				if(archivo.rdbuf()->in_avail() > 0)
					archivo.rdbuf()->sgetn(NULL, sizeof(u8));
		}
	}
}

void Imagen::reset(void)
{
	delete _imagen;
	free(_pixelData);
	_imagen = NULL;
	_pixelData = NULL;
	_alto = 0;
	_ancho = 0;
}

