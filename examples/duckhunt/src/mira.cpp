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

#include "mira.h"
using namespace std;

Mira::Mira(const std::string& ruta, const Nivel* nivel, const std::string& jugador) throw (Excepcion)
: Actor(ruta, nivel), _crono(0), _recargando(false)
{
	_estado_actual = "normal-" + jugador;
}

Mira::~Mira(void)
{
}

void Mira::actualizar(void)
{
}

u32 Mira::crono(void) const
{
	return _crono;
}

bool Mira::recargando(void) const
{
	return _recargando;
}

void Mira::setCrono(u32 c)
{
	_crono = c;
}

void Mira::setRecargando(bool r)
{
	_recargando = r;
}

