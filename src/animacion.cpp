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

#include "animacion.h"
using namespace std;

Animacion::Animacion(const Imagen& i, const string& secuencia, u8 filas, u8 columnas, u8 retardo)
: _imagen(&i), _filas(filas), _columnas(columnas), _paso(0), _retardo(retardo), _cont_retardo(0)
{
	_ancho_cuadro = _imagen->ancho() / _columnas;
	_alto_cuadro = _imagen->alto() / _filas;

	string frames_tmp(secuencia);
	char *proximo;

	// Trabajamos con una copia de los cuadros indicados
	for(proximo = strtok((char*)frames_tmp.c_str(), ","); proximo ; )
	{
		// Desmembramos la cadena separada por comas
		_cuadros.push_back(atoi(proximo));
		proximo = strtok(NULL, ",\0");
	}

	free(proximo);

	// El indicador de que un cuadro es el último es -1
	_cuadros.push_back(-1);
}

bool Animacion::primerPaso(void) const
{
	return (_paso == 0);
}

u8 Animacion::pasoActual(void) const
{
	return _paso;
}

u16 Animacion::alto(void) const
{
	return _alto_cuadro;
}

u16 Animacion::ancho(void) const
{
	return _ancho_cuadro;
}

const Imagen& Animacion::imagen(void) const
{
	return *(_imagen);
}

void Animacion::avanzar(void)
{
	// Si ha pasado el número de frames '_retardo' desde el último cambio, se avanza un cuadro
	// Si el paso actual es el último, el siguiente será el primero; en caso contrario, se avanza al siguiente
	if((++_cont_retardo) >= _retardo)
	{
		// Si el cuadro siguiente es el -1, indica que la animación acaba
		if(_cuadros[++_paso] == -1) 
			reiniciar();
		else
			_cont_retardo = 0;
	}
}

void Animacion::reiniciar(void)
{
	_paso = 0;
	_cont_retardo = 0;
}

void Animacion::dibujar(s16 x, s16 y, s16 z, bool invertir)
{
	// Punto superior izquierdo del cuadro en la rejilla
	s16 origen_x = ((_cuadros[_paso] % _columnas) * (_ancho_cuadro));
	s16 origen_y = ((_cuadros[_paso] / _columnas) * (_alto_cuadro));

	// Dibujar el cuadro de la textura en las coordenadas que se reciben
	screen->dibujarCuadro(_imagen->textura(), _imagen->ancho(), _imagen->alto(), x, y, z,
							origen_x, origen_y, _ancho_cuadro, _alto_cuadro, invertir);

	// Avanzar al siguiente cuadro de la animación
	avanzar();
}

