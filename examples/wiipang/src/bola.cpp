/**
 * Licencia GPLv3
 *
 * Este archivo es parte de libWiiEsp, y su autor es Ezequiel Vázquez De la Calle
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
 */

#include "bola.h"
using namespace std;

Bola::Bola(const std::string& ruta, const Nivel* nivel, const string& color, const string& talla) throw (Excepcion)
: Actor(ruta, nivel), _color(color), _talla(talla), _gravedad(0.4)
{
	// Comprobar que el color y la talla de la bola son parámetros correctos
	if(_talla != "xl" and _talla != "l" and _talla != "m" and _talla != "s")
		throw Excepcion("Bola::Bola() - Talla incorrecta (" + _talla + ")");
	if(_color != "rojo" and _color != "verde" and _color != "azul")
		throw Excepcion("Bola::Bola() - Color incorrecto (" + _color + ")");

	_estado_actual = color + "-" + talla;
	_vy_real = _vy = 0;

	// Leer la velocidad segun la talla de la bola
	parser->cargar(ruta);
	_vy_inicial = parser->atributoU32("g" + _talla, parser->raiz());
}

Bola::~Bola(void)
{
}

void Bola::actualizar(void)
{
	_vy_real += _gravedad;
	_vy = (s16)_vy_real;
	mover(_x + _vx, _y + _vy);
}

const string& Bola::color(void) const
{
	return _color;
}

const string& Bola::talla(void) const
{
	return _talla;
}

void Bola::setVyCero(void)
{
	_vy = 0;
	_vy_real = 0.0;
}

void Bola::setVyInicial(void)
{
	_vy = -_vy_inicial;
	_vy_real = -_vy_inicial;
}

void Bola::setVyReal(f32 vy_real)
{
	_vy = vy_real;
	_vy_real = vy_real;
}

