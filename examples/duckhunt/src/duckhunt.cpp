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

#include "duckhunt.h"
using namespace std;

Duckhunt::Duckhunt(const std::string& ruta)
: Juego(ruta), _patos_j1(0), _patos_j2(0), _escenario(NULL)
{
}

Duckhunt::~Duckhunt(void)
{
}

void Duckhunt::cargar(void)
{
	presentacion(_mandos.begin()->first);
	_escenario = new Escenario("/apps/duckhunt/xml/escenario.tmx", this);
}

bool Duckhunt::frame(void)
{
	// Condiciones de fin de juego
	if(_escenario->finNivel())
	{
		// Eliminar el escenario
		_escenario->musica().stop();
		delete _escenario;
		// Esperar 1 segundo
		usleep(1000000);
		// Mostrar la pantalla de fin del juego felicitando al jugador que ha ganado
		finJuego((_patos_j1 > _patos_j2 ? 1 : 2));
		return true;
	}

	// Pausar el juego al pulsar HOME, cambiar el idioma al pulsar MAS
	for(Controles::iterator i = _mandos.begin() ; i != _mandos.end() ; ++i)
	{
		_escenario->actualizarPj(i->first, *(i->second));
		if(i->second->newPressed(Mando::BOTON_HOME))
			if(pausa(i->first))
			{
				delete _escenario;
				return true;
			}
		if(i->second->newPressed(Mando::BOTON_MAS))
			lang->cambiarIdioma();
	}

	_escenario->actualizarNpj();
	_escenario->actualizarEscenario();
	_escenario->musica().loop();
	_escenario->dibujar();
	dibujarMenu();

	return false;
}

bool Duckhunt::pausa(const string& jugador)
{
	galeria->sonido("pausa").play();
	while(1)
	{
		// Leer el mando del mismo jugador que ha pausado el juego
		Mando::leerDatos();
		_mandos[jugador]->actualizar();
		// Si pulsa el botón HOME se sale de la pausa
		if(_mandos[jugador]->newPressed(Mando::BOTON_HOME))
		{
			galeria->sonido("pausa").play();
			return false;
		}
		// Si pulsa el botón A se sale del juego
		if(_mandos[jugador]->newPressed(Mando::BOTON_A))
			return true;
		// Mostrar un letrero que indique que estamos en pausa
		_escenario->dibujar();
		dibujarMenu();
		galeria->fuente("arial").escribir(lang->texto("PAUSA"), 30, 260, 255, 1, 0xFFFF00FF);
		screen->flip();
	}
}

void Duckhunt::presentacion(const string& jugador)
{
	while(1)
	{
		// Leer el mando del mismo jugador que ha pausado el juego
		Mando::leerDatos();
		_mandos[jugador]->actualizar();
		// Si pulsa el botón A se sale de la pausa
		if(_mandos[jugador]->newPressed(Mando::BOTON_A))
		{
			galeria->sonido("menu-ok").play();
			return;
		}

		galeria->imagen("logo").dibujar(160, 80, 1);
		galeria->fuente("arial").escribir("Desarrollado por Ezequiel Vázquez de la Calle", 20, 120, 210, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("Gráficos: jojodu001 y Marc6310, Música: Victor Losa", 20, 90, 240, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("Ejemplo de libWiiEsp", 20, 230, 270, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("https://forja.rediris.es/projects/libwiiesp", 20, 145, 300, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("Pulsar A para comenzar", 30, 160, 400, 1, 0xFF0000FF);
		galeria->fuente("arial").escribir("Juego original de Nintendo, (c) 1984", 20, 165, 500, 1, 0xFFFFFFFF);
		screen->flip();
	}
}

void Duckhunt::dibujarMenu(void) const
{
	screen->dibujarRectangulo(50, 470, 590, 470, 590, 515, 50, 515, 400, 0x000000FF);

	wchar_t* cadena = (wchar_t*)memalign(32, sizeof(wchar_t)*128);
	swprintf(cadena, 128, L"%ls: %d", lang->texto("PATOS1").c_str(), _patos_j1);
	galeria->fuente("arial").escribir(cadena, 25, 100, 485, 399, 0xFFFFFFFF);
	swprintf(cadena, 128, L"%ls: %d", lang->texto("PATOS2").c_str(), _patos_j2);
	galeria->fuente("arial").escribir(cadena, 25, 375, 485, 399, 0xFFFFFFFF);
	free(cadena);
}

void Duckhunt::finJuego(const u8 jugador)
{
	wchar_t* cadena = (wchar_t*)memalign(32, 128*sizeof(wchar_t));
	while(1)
	{
		Mando::leerDatos();
		_mandos.begin()->second->actualizar();
		// Si pulsa el botón A se sale del programa
		if(_mandos.begin()->second->newPressed(Mando::BOTON_A))
		{
			free(cadena);
			return;
		}
		// Cambiar el idioma al pulsar el boton MAS
		if(_mandos.begin()->second->newPressed(Mando::BOTON_MAS))
			lang->cambiarIdioma();

		galeria->fuente("arial").escribir(lang->texto("FINAL"), 40, 125, 125, 1, 0x00FF00FF);
		swprintf(cadena, 128, L"%ls: %d", lang->texto("PATOS1").c_str(), _patos_j1);
		galeria->fuente("arial").escribir(cadena, 30, 125, 200, 1, 0xFFFFFFFF);
		swprintf(cadena, 128, L"%ls: %d", lang->texto("PATOS2").c_str(), _patos_j2);
		galeria->fuente("arial").escribir(cadena, 30, 125, 275, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir(lang->texto("PULSAR"), 30, 125, 350, 1, 0xFF0000FF);
		screen->flip();
	}
}

u32 Duckhunt::patosJ1(void) const
{
	return _patos_j1;
}

u32 Duckhunt::patosJ2(void) const
{
	return _patos_j2;
}

u32& Duckhunt::patosJ1(void)
{
	return _patos_j1;
}

u32& Duckhunt::patosJ2(void)
{
	return _patos_j2;
}

