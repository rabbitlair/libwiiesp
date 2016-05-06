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

#include "fuente.h"
using namespace std;

FT_Library Fuente::library;

Fuente::Fuente(const string& ruta) throw (ArchivoEx, TarjetaEx)
{
	if(not sdcard->montada())
		throw TarjetaEx("Fuente - La tarjeta SD no está montada.");

	// Añadir a la ruta la unidad que esté montada en la clase sdcard como prefijo
	string ruta_completa = sdcard->unidad() + ":" + ruta;

	// Comprobar la existencia del archivo
	if(not sdcard->existe(ruta_completa))
		throw ArchivoEx("Fuente - El archivo '" + ruta + "' no existe.");

	// Cargar la 'face' de la fuente desde el archivo, si da error, se lanza una excepción
	u32 error = FT_New_Face(library, ruta_completa.c_str(), 0, &_face);
	_kerning = FT_HAS_KERNING(_face);
	if(error)
		throw ArchivoEx("Fuente - Error al cargar la fuente '" + ruta + "'");
}

void Fuente::escribir(const string& texto, u8 tam, s16 x, s16 y, s16 z, u32 color) const
{
	escribir(utf32::convertir(texto), tam, x, y, z, color);
}

void Fuente::escribir(const wstring& texto, u8 tam, s16 x, s16 y, s16 z, u32 color) const
{
	// Establecer el tamaño de la fuente
	u32 error = FT_Set_Pixel_Sizes(_face, 0, tam);
	// Si no se puede establecer el tamaño, se pone el tamaño por defecto, que es 12
	if(error) {
		tam = 12;
		FT_Set_Pixel_Sizes(_face, 0, tam);
	}

	FT_GlyphSlot slot = _face->glyph;
	u32 actual = 0;
	u32 anterior = 0;

	// Procesar todo el texto
	for(u16 contador = 0 ; contador < texto.length() ; ++contador)
	{
		actual = FT_Get_Char_Index(_face, texto[contador]);

		if(_kerning and anterior and actual)
		{
            FT_Vector delta;
            FT_Get_Kerning(_face, anterior, actual, FT_KERNING_DEFAULT, &delta);
			x += delta.x >> 6;
        }

		// Cargar un carácter
		FT_Load_Char(_face, texto[contador], FT_LOAD_RENDER);
		// Dibujar el carácter en las coordenadas que correspondan
		dibujarCaracter(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top + tam/2, z, color);
		// Calcular el desplazamiento del 'cursor' en la pantalla (en píxeles) para pintar el siguiente carácter
        x += slot->advance.x >> 6;
		anterior = actual;
	}
}

Fuente::~Fuente(void)
{
	FT_Done_Face(_face);
}

// Métodos privados
void Fuente::dibujarCaracter(FT_Bitmap *bitmap, s16 x, s16 y, s16 z, u32 color) const
{
    s16 i, j, p, q;
    s16 x_max = x + bitmap->width;
    s16 y_max = y + bitmap->rows;

	for(i = x, p = 0 ; i < x_max ; i++, p++)
		for(j = y, q = 0 ; j < y_max ; j++, q++)
			// Sólo se pintan los píxeles que correspondan a la parte rellena del carácter
			if(*(bitmap->buffer + (q * bitmap->width) + p))
				screen->dibujarPunto(i, j, z, color);
}

