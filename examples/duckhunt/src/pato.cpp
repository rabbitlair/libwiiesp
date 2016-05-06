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

#include "pato.h"
using namespace std;

Pato::Pato(const std::string& ruta, const Nivel* nivel) throw (Excepcion): Actor(ruta, nivel)
{
	_estado_actual = "volar";
	_velocidad = (f32)(10.0 + rand() * (8.0) / RAND_MAX);
	_direccion = (f32)acos((_velocidad * _velocidad + _vx * _vx - _vy * _vy )/(2 * _velocidad * _vx));
	_crono = 0;
}

Pato::~Pato(void)
{
}

void Pato::actualizar(void)
{
	// Si esta volando, mover segun la direccion y velocidad
	if(_estado_actual == "volar")
		mover(_x + _vx, _y + _vy);
	// Si ha sido impactado, no se mueve
	else if(_estado_actual == "impacto")
	{
		// Si han pasado 0,35 segundos desde el impacto, el pato cae
		if((gettick()/(u32)TB_TIMER_CLOCK - crono()) > 350)
		{
			setEstado("muerto");
			setVelocidad(15.0);
			setDireccion(2*M_PI);
		}
	}
	// Si esta muerto, cae
	else if(_estado_actual == "muerto")
		mover(_x + _vx, _y + _vy);
}

f32 Pato::velocidad(void) const
{
	return _velocidad;
}

f32 Pato::direccion(void) const
{
	return _direccion;
}

u32 Pato::crono(void) const
{
	return _crono;
}

void Pato::setVelocidad(f32 v)
{
	_velocidad = v;
	_vx = _velocidad * sin(_direccion);
	_vy = _velocidad * cos(_direccion);
}

void Pato::setDireccion(f32 d)
{
	_direccion = d;
	_vx = _velocidad * sin(_direccion);
	_vy = _velocidad * cos(_direccion);
}

void Pato::setCrono(u32 c)
{
	_crono = c;
}

