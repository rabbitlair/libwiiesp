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

#include "personaje.h"
using namespace std;

Personaje::Personaje(const std::string& ruta, const Nivel* nivel) throw (Excepcion)
: Actor(ruta, nivel), _choque(false), _vy_real(-10.0)
{
	_estado_actual = "normal";
}

Personaje::~Personaje(void)
{
}

void Personaje::actualizar(void)
{
	if(_estado_actual == "mover")
		mover(_x + _vx, _y);
	if(_estado_actual == "muerto")
	{
		_vy_real += 0.3;
		_vy = (s16)_vy_real;
		mover(_x + _vx, _y + _vy);
		if(not _choque and const_cast<Nivel*>(_nivel)->colision(this))
		{
			_vx *= -2;
			_vy_real = -3.0;
			_choque = true;
		}
	}
}

