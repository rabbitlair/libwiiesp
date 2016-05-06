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

#include "gancho.h"
#include "escenario.h"
using namespace std;

Gancho::Gancho(const std::string& ruta, const Nivel* nivel, const u8 id) throw (Excepcion)
: Actor(ruta, nivel), _gancho_id(id), _destruir(false)
{
	_estado_actual = "normal";
}

Gancho::~Gancho(void)
{
}

void Gancho::actualizar(void)
{
	mover(_x, _y - _vy);
	Rectangulo* r = static_cast<Rectangulo*>(*(_map_colisiones["normal"].begin()));
	r->p3().y() += _vy;
	r->p4().y() += _vy;
	r->centro().y() += (_vy/2);
}

void Gancho::dibujar(s16 x, s16 y, s16 z)
{
	_map_animaciones["punta"]->dibujar(x, y, z);
	for(u32 yy = _y + alto() ; yy < static_cast<const Escenario*>(_nivel)->limiteInferior() ; yy += alto())
		_map_animaciones["cable"]->dibujar(x, yy, z);
}

u8 Gancho::id(void) const
{
	return _gancho_id;
}

bool Gancho::destruir(void) const
{
	return _destruir;
}

void Gancho::setDestruir(void)
{
	_destruir = true;
}

