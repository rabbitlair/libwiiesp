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

#include "mando.h"
using namespace std;

u8 Mando::mandos_activos = 0;

Mando::Mando(void)
{
	// Configuración predeterminada para los botones
	_botones[BOTON_A] = WPAD_BUTTON_A;
	_botones[BOTON_B] = WPAD_BUTTON_B;
	_botones[BOTON_C] = WPAD_NUNCHUK_BUTTON_C;
	_botones[BOTON_Z] = WPAD_NUNCHUK_BUTTON_Z;
	_botones[BOTON_1] = WPAD_BUTTON_1;
	_botones[BOTON_2] = WPAD_BUTTON_2;
	_botones[BOTON_MAS] = WPAD_BUTTON_PLUS;
	_botones[BOTON_MENOS] = WPAD_BUTTON_MINUS;
	_botones[BOTON_HOME] = WPAD_BUTTON_HOME;
	_botones[BOTON_ARRIBA] = WPAD_BUTTON_UP;
	_botones[BOTON_ABAJO] = WPAD_BUTTON_DOWN;
	_botones[BOTON_DERECHA] = WPAD_BUTTON_RIGHT;
	_botones[BOTON_IZQUIERDA] = WPAD_BUTTON_LEFT;

	// Inicializamos el estado del mando
	_num_botones = _botones.size();
	_actual_mando.resize(_num_botones);
	_old_mando.resize(_num_botones);

	// Primer mapeado: todos los botones sin pulsar
	for(u8 i = 0 ; i < _num_botones ; ++i)
	{
		_actual_mando[i] = false;
		_old_mando[i] = false;
	}

	// Número de mando de la instancia
	_chan = mandos_activos++;
}

Mando::~Mando(void)
{
	mandos_activos--;
}

void Mando::leerDatos(void)
{
	WPAD_ScanPads();
}

bool Mando::conectado(void) const
{
	if(WPAD_GetStatus() == WPAD_STATE_ENABLED)
		return true;
	return false;
}

u8 Mando::chan(void) const
{
	return _chan;
}


// Funciones relativas a los botones

void Mando::actualizar(void)
{
	// Ahora el mapeado nuevo es el viejo
	_old_mando = _actual_mando;

	// Leer todas las nuevas pulsaciones de botones del mando
	u32 pulsado = WPAD_ButtonsDown(_chan);
	// Leer los botones que se estan manteniendo pulsados en el mando
	u32 mantenido = WPAD_ButtonsHeld(_chan);

	// Leer los datos del mando (acelerómetros, puntero, etc.)
	_wmote_datas = WPAD_Data(_chan);
	// Leer mando secundario (expansión)
	WPAD_Probe(_chan, &(_type));

	// Mapear las pulsaciones de botones actuales
	for(map<Boton,u32>::iterator i = _botones.begin() ; i != _botones.end() ; ++i)
	{
		if(pulsado & i->second or mantenido & i->second)
			_actual_mando[i->first] = true;
		else
			_actual_mando[i->first] = false;
	}
}

bool Mando::pressed(Boton boton) const
{
	return _actual_mando[boton];
}

bool Mando::released(Boton boton) const
{
	return (not _actual_mando[boton] and _old_mando[boton]);
}

bool Mando::newPressed(Boton boton) const
{
	return (_actual_mando[boton] and not _old_mando[boton]);
}


// Funciones relativas al puntero infrarrojo

s16 Mando::punteroX(void) const
{
	return _wmote_datas->ir.x;
}

s16 Mando::punteroY(void) const
{
	return _wmote_datas->ir.y;
}

bool Mando::punteroEnPantalla(void) const
{
	return _wmote_datas->ir.valid;
}


// Funciones relativas a la vibración

void Mando::vibrar(u16 tiempo)
{
	WPAD_Rumble(_chan, 1);
	usleep(tiempo);
	WPAD_Rumble(_chan, 0);
}


// Funciones relativas a la orientación

f32 Mando::cabeceo(void) const
{
	return (0 - _wmote_datas->orient.pitch);
}

f32 Mando::viraje(void) const
{
	return _wmote_datas->orient.yaw;
}

f32 Mando::rotacion(void) const
{
	return _wmote_datas->orient.roll;
}


// Funciones relativas al Nunchuck

bool Mando::nunConectado(void) const
{
	return _type == WPAD_EXP_NUNCHUK;
}

Mando::Nunchuck_x Mando::nunPalancaEstadoX(void) const throw (NunchuckEx)
{
	if(not nunConectado())
		throw NunchuckEx("Mando::nunPalancaEstadoX - Nunchuck no conectado");

	// Se deja un margen de +/- 15, debido a que la palanca suelta (centrada) no tiene un valor exacto
	if(nunPalancaValorX() > _wmote_datas->exp.nunchuk.js.center.x + 15)
		return PALANCA_DERECHA;
	else if(nunPalancaValorX() < _wmote_datas->exp.nunchuk.js.center.x - 15)
		return PALANCA_IZQUIERDA;

	return PALANCA_CENTRO_X;
}

Mando::Nunchuck_y Mando::nunPalancaEstadoY(void) const throw (NunchuckEx)
{
	if(not nunConectado())
		throw NunchuckEx("Mando::nunPalancaEstadoY - Nunchuck no conectado");

	// Se deja un margen de +/- 15, debido a que la palanca suelta (centrada) no tiene un valor exacto
	if(nunPalancaValorY() > _wmote_datas->exp.nunchuk.js.center.y + 15)
		return PALANCA_ARRIBA;
	else if(nunPalancaValorY() < _wmote_datas->exp.nunchuk.js.center.y - 15)
		return PALANCA_ABAJO;

	return PALANCA_CENTRO_Y;
}

u16 Mando::nunPalancaValorX(void) const throw (NunchuckEx)
{
	if(not nunConectado())
		throw NunchuckEx("Mando::nunPalancaValorX - Nunchuck no conectado");
	return _wmote_datas->exp.nunchuk.js.pos.x;
}

u16 Mando::nunPalancaValorY(void) const throw (NunchuckEx)
{
	if(not nunConectado())
		throw NunchuckEx("Mando::nunPalancaValorY - Nunchuck no conectado");
	return _wmote_datas->exp.nunchuk.js.pos.y;
}

u16 Mando::nunPalancaCentroX(void) const throw (NunchuckEx)
{
	if(not nunConectado())
		throw NunchuckEx("Mando::nunPalancaCentroX - Nunchuck no conectado");
	return _wmote_datas->exp.nunchuk.js.center.x;
}

u16 Mando::nunPalancaCentroY(void) const throw (NunchuckEx)
{
	if(not nunConectado())
		throw NunchuckEx("Mando::nunPalancaCentroY - Nunchuck no conectado");
	return _wmote_datas->exp.nunchuk.js.center.y;
}

