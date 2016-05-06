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

#include "wiipang.h"
using namespace std;

Wiipang::Wiipang(const std::string& ruta)
: Juego(ruta), _escenario(NULL), _puntos(0), _vidas(3), _nivel(1)
{
	// Leer la ruta del archivo de niveles
	parser->cargar(ruta);
	TiXmlElement* nodo = parser->buscar("niveles");
	string xml_niveles = parser->atributo("valor", nodo);

	// Leer los distintos niveles
	parser->cargar(xml_niveles);
	for(TiXmlElement* el = parser->raiz()->FirstChildElement() ; el ; el = el->NextSiblingElement())
		_niveles.push_back(parser->atributo("xml", el));
}

Wiipang::~Wiipang(void)
{
}

void Wiipang::cargar(void)
{
	presentacion(_mandos.begin()->first);
	_escenario = new Escenario(_niveles[0], this);
}

bool Wiipang::frame(void)
{
	// Controlar las condiciones de fin de escenario
	if(_escenario->finNivel())
	{
		bool reiniciar = _escenario->reiniciar();
		// Eliminar el nivel actual
		_escenario->musica().stop();
		delete _escenario;
		// Esperar 1 segundo
		usleep(1000000);
		// Si se han perdido todas las vidas, salir
		if(_vidas < 1)
		{
			finJuego(_mandos.begin()->first);
			return true;
		}
		// Si se avanza al siguiente nivel, mostrar la pantalla de fin de nivel y se aumenta en 1 el contador
		if(not reiniciar)
		{
			nivelCompletado(_mandos.begin()->first);
			// Si se ha superado el último nivel, salir
			if(++_nivel > _niveles.size())
				return true;
		}
		_escenario = new Escenario(_niveles[_nivel-1], this);
	}

	// Pausar el juego al pulsar HOME, cambiar el idioma al pulsar MAS
	for(Controles::iterator i = _mandos.begin() ; i != _mandos.end() ; ++i)
	{
		_escenario->actualizarPj(i->first, *(i->second));
		if(i->second->newPressed(Mando::BOTON_HOME))
			if(pausa(i->first))
				return true;
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

void Wiipang::dibujarMenu(void)
{
	wchar_t* cadena = (wchar_t*)memalign(32, sizeof(wchar_t)*200);
	swprintf(cadena, 200, L"%ls: %d", lang->texto("PUNTOS").c_str(), _puntos);
	galeria->fuente("arial").escribir(cadena, 25, 75, 475, 399, 0xFFFFFFFF);
	swprintf(cadena, 200, L"%ls: %d", lang->texto("VIDAS").c_str(), _vidas);
	galeria->fuente("arial").escribir(cadena, 25, 320, 475, 399, 0xFFFFFFFF);
	swprintf(cadena, 200, L"%ls: %d", lang->texto("NIVEL").c_str(), _nivel);
	galeria->fuente("arial").escribir(cadena, 25, 450, 475, 399, 0xFFFFFFFF);
	free(cadena);
}

bool Wiipang::pausa(const string& jugador)
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

void Wiipang::presentacion(const string& jugador)
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

		galeria->imagen("logo").dibujar(160, 60, 1);
		galeria->fuente("arial").escribir("Desarrollado por Ezequiel Vázquez de la Calle", 20, 120, 210, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("Gráficos: UPi, Música: Victor Losa", 20, 190, 240, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("Ejemplo de libWiiEsp", 20, 230, 270, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("https://forja.rediris.es/projects/libwiiesp", 20, 145, 300, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir("Pulsar A para comenzar", 30, 160, 400, 1, 0xFF0000FF);
		galeria->fuente("arial").escribir("Juego original de Mitchell Co, (c) 1989", 20, 140, 500, 1, 0xFFFFFFFF);
		screen->flip();
	}
}

void Wiipang::nivelCompletado(const string& jugador)
{
	galeria->sonido("endlevel").play();
	wchar_t* cadena = (wchar_t*)memalign(32, 128*sizeof(wchar_t));
	while(1)
	{
		Mando::leerDatos();
		_mandos[jugador]->actualizar();
		// Si pulsa el botón A se continua
		if(_mandos[jugador]->newPressed(Mando::BOTON_A))
		{
			free(cadena);
			return;
		}
		// Cambiar el idioma al pulsar el boton MAS
		if(_mandos[jugador]->newPressed(Mando::BOTON_MAS))
			lang->cambiarIdioma();

		galeria->fuente("arial").escribir(lang->texto("COMPLETADO"), 40, 125, 200, 1, 0x00FF00FF);
		swprintf(cadena, 128, L"%ls: %d", lang->texto("PUNTOS").c_str(), _puntos);
		galeria->fuente("arial").escribir(cadena, 30, 125, 275, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir(lang->texto("PULSAR"), 30, 125, 350, 1, 0xFF0000FF);
		screen->flip();
	}
}

void Wiipang::finJuego(const string& jugador)
{
	wchar_t* cadena = (wchar_t*)memalign(32, 128*sizeof(wchar_t));
	while(1)
	{
		Mando::leerDatos();
		_mandos[jugador]->actualizar();
		// Si pulsa el botón A se sale del programa
		if(_mandos[jugador]->newPressed(Mando::BOTON_A))
		{
			free(cadena);
			return;
		}
		// Cambiar el idioma al pulsar el boton MAS
		if(_mandos[jugador]->newPressed(Mando::BOTON_MAS))
			lang->cambiarIdioma();

		galeria->fuente("arial").escribir(lang->texto("FINAL"), 40, 125, 200, 1, 0x00FF00FF);
		swprintf(cadena, 128, L"%ls: %d", lang->texto("PUNTOS").c_str(), _puntos);
		galeria->fuente("arial").escribir(cadena, 30, 125, 275, 1, 0xFFFFFFFF);
		galeria->fuente("arial").escribir(lang->texto("PULSAR"), 30, 125, 350, 1, 0xFF0000FF);
		screen->flip();
	}
}

u32 Wiipang::vidas(void) const
{
	return _vidas;
}

u32 Wiipang::puntos(void) const
{
	return _puntos;
}

u32 Wiipang::nivel(void) const
{
	return _nivel;
}

u32& Wiipang::vidas(void)
{
	return _vidas;
}

u32& Wiipang::puntos(void)
{
	return _puntos;
}

u32& Wiipang::nivel(void)
{
	return _nivel;
}

