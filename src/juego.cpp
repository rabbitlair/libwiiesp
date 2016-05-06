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

#include "juego.h"
using namespace std;

Juego::Juego(const string& ruta)
{
	// Si ocurre una excepción en este punto, se sale del programa
	try {
		// Inicializar la tarjeta y cargar el archivo de configuracion
		sdcard->inicializar();
		parser->cargar(ruta);

		// Leer la configuracion e inicializar el sistema de log
		TiXmlElement* nodo_log = parser->buscar("log");
		string ruta_log = parser->atributo("valor", nodo_log);
		u32 nivel_log = parser->atributoU32("nivel", nodo_log);
		Logger::Nivel nivel = Logger::OFF;
		if(nivel_log == 1)
			nivel = Logger::ERROR;
		else if(nivel_log == 2)
			nivel = Logger::AVISO;
		else if(nivel_log == 3)
			nivel = Logger::INFO;
		logger->inicializar(ruta_log, nivel);

	} catch(...) {
		exit(0);
	}

	// A partir de aquí, si ocurre una excepción, ésta se registra en el log
	try {

		// Inicialización de cada sistema o biblioteca
		screen->inicializar();
		Mando::inicializar(screen->ancho(), screen->alto());
		Sonido::inicializar();
		Fuente::inicializar();

		// Establecer color alpha para las imágenes, y FPS indicados
		string color_alpha = parser->atributo("valor", parser->buscar("alpha"));
		stringstream convert(color_alpha);
		convert >> std::hex >> Imagen::alpha;
		_fps = parser->atributoU32("valor", parser->buscar("fps"));

		// Cargar los controles de los jugadores
		TiXmlElement* nodo_jugadores = parser->buscar("jugadores");
		string pj1 = parser->atributo("pj1", nodo_jugadores);
		if(pj1 != "")
			_mandos.insert(make_pair(pj1, new Mando));
		string pj2 = parser->atributo("pj2", nodo_jugadores);
		if(pj2 != "")
			_mandos.insert(make_pair(pj2, new Mando));
		string pj3 = parser->atributo("pj3", nodo_jugadores);
		if(pj3 != "")
			_mandos.insert(make_pair(pj3, new Mando));
		string pj4 = parser->atributo("pj4", nodo_jugadores);
		if(pj4 != "")
			_mandos.insert(make_pair(pj4, new Mando));

		// Lectura de datos referentes al archivo XML del soporte de idiomas
		TiXmlElement* nodo_lang = parser->buscar("lang");
		string ruta_lang = parser->atributo("valor", nodo_lang);
		wstring defecto_lang = utf32::convertir(parser->atributo("defecto", nodo_lang));

		// Cargar la biblioteca de medias
		galeria->inicializar(parser->atributo("valor", parser->buscar("galeria")));

		// Cargar el soporte de idiomas
		lang->inicializar(ruta_lang, defecto_lang);

	} catch(const std::exception& e) {
		logger->error(e.what());
		exit(0);
	}
}

Juego::~Juego(void)
{
	for(Controles::iterator i = _mandos.begin() ; i != _mandos.end() ; ++i)
		delete i->second;
	_mandos.clear();
	exit(0);
}

void Juego::run(void)
{
	try {
		// Cargar lo que sea necesario antes de comenzar el bucle principal
		cargar();

		// Variables para el control del bucle principal del juego
		bool salir = false;
		tiempo::tick = 0;

		// Bucle principal del juego
		while(not salir) 
		{
			// Leer la información de todos los mandos y actualizarlos
			Mando::leerDatos();
			for(Controles::iterator i = _mandos.begin() ; i != _mandos.end() ; ++i)
				i->second->actualizar();

			// Gestionar un frame
			salir = frame();

			// Esperar la sincronizacion de video
			screen->flip();

			// Control de tiempo: para mantener el framerate constante
			tiempo::controlarFps(_fps);
		}

	} catch(const std::exception& e) {
		logger->error(e.what());
	}
}

