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

#include "pala.h"
using namespace std;

Pala::Pala(const std::string& ruta, const Nivel* nivel) throw (Excepcion): Actor(ruta, nivel)
{
	_estado_actual = "normal";
}

Pala::~Pala(void)
{
}

void Pala::actualizar(void)
{
	if(_estado_actual == "normal" or _estado_actual == "normalp" or _estado_actual == "normalg")
		mover(_x, _y);
	if(_estado_actual == "mover" or _estado_actual == "moverp" or _estado_actual == "moverg")
		mover(_x + _vx, _y);
}

