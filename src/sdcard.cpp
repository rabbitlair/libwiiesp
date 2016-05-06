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

#include "sdcard.h"
using namespace std;

Sdcard* Sdcard::_instance = 0;

void Sdcard::inicializar(const string& unidad)
{
	// Empezamos con la unidad desmontada
	_montada = 0;
	// Guardar el nombre de la unidad
	_unidad = unidad;

	u8 intentos = 0;

	// Mientras que no esté la unidad montada
	while(not _montada)
	{
		// Si se inicia el sistema hardware
		if(__io_wiisd.startup())
		{
			// Montar la unidad con el nombre que se le ha asignado
			_montada = fatMountSimple(_unidad.c_str(), &__io_wiisd);
			// Si no se ha montado, desactivar el sistema hardware
			if(not _montada)
				__io_wiisd.shutdown();
		}
		// Si a los 10 intentos no funciona, salir del programa
		if(++intentos > 10)
			exit(1);
		// Esperamos 50 ms, por si el sistema hardware está bloqueado, antes de volver a intentarlo
		usleep(50000);
	}
}

const string& Sdcard::unidad(void) const
{
	return _unidad;
}

bool Sdcard::montada(void) const
{
	// Si no está montada la tarjeta SD, _montada valdrá 0
	return (_montada != 0);
}

void Sdcard::desmontar(void)
{
	if(montada())
	{
		fatUnmount(_unidad.c_str());
		__io_wiisd.shutdown();
	}
	_montada = 0;
}

bool Sdcard::existe(const string& archivo)
{
	if(_montada == 0)
		return false;

    FILE* fp = NULL;

    fp = fopen(archivo.c_str(), "rb");
    if(fp != NULL)
    {
        fclose( fp );
        return true;
    }
    return false;
}

Sdcard::~Sdcard(void)
{
	// Desmontar la unidad al destruir la instancia del Singleton
	desmontar();
}

