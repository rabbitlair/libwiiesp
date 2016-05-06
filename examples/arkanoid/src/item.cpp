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

#include "item.h"
using namespace std;

Item::Item(const std::string& ruta, const Nivel* nivel) throw (Excepcion): Actor(ruta, nivel)
{
	u32 res = rand() % 4;
	if(res == 0)
		_estado_actual = "azul";
	else if(res == 1)
		_estado_actual = "amarillo";
	else if(res == 2)
		_estado_actual = "verde";
	else
		_estado_actual = "rojo";
}

Item::~Item(void)
{
}

void Item::actualizar(void)
{
	if(_estado_actual == "azul" or _estado_actual == "amarillo" or _estado_actual == "verde" or _estado_actual == "rojo")
		mover(_x, _y + _vy);
}

