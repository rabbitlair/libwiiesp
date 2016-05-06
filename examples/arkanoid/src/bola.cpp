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

Bola::Bola(const std::string& ruta, const Nivel* nivel) throw (Excepcion): Actor(ruta, nivel)
{
	_estado_actual = "mover";
	_velocidad = (f32)sqrt(_vx * _vx + _vy * _vy);
	_direccion = (f32)acos((_velocidad * _velocidad + _vx * _vx - _vy * _vy )/(2 * _velocidad * _vx));
	parser->cargar(ruta);
	_max_vel = (f32)parser->atributoF32("max_vel", parser->raiz());
}

Bola::~Bola(void)
{
}

void Bola::actualizar(void)
{
	if(_estado_actual == "normal")
		mover(_x, _y);
	if(_estado_actual == "mover")
		mover(_x + _vx, _y + _vy);
}

f32 Bola::velocidad(void) const
{
	return _velocidad;
}

f32 Bola::maxVelocidad(void) const
{
	return _max_vel;
}

f32 Bola::direccion(void) const
{
	return _direccion;
}

void Bola::setVelocidad(f32 v)
{
	_velocidad = v;
	_vx = _velocidad * sin(_direccion);
	_vy = _velocidad * cos(_direccion);
}

void Bola::setDireccion(f32 d)
{
	_direccion = d;
	_vx = _velocidad * sin(_direccion);
	_vy = _velocidad * cos(_direccion);
}

