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

#include "nivel.h"
using namespace std;

Nivel::Nivel(const string& ruta) throw (ArchivoEx, TarjetaEx): _scroll_x(0), _scroll_y(0)
{
	// Comprobar que la SD está montada
	if(not sdcard->montada())
		throw TarjetaEx("Nivel - La tarjeta SD no está montada");

	// Abrir el archivo XML
	try {
		parser->cargar(ruta);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Leer la información de la etiqueta 'map'
	_ancho_tiles = parser->atributoU32("width", parser->raiz());
	_alto_tiles = parser->atributoU32("height", parser->raiz());
	_ancho_un_tile = parser->atributoU32("tilewidth", parser->raiz());
	_alto_un_tile = parser->atributoU32("tileheight", parser->raiz());

	// Calcular las medidas del escenario
	_ancho_nivel = _ancho_tiles * _ancho_un_tile;
	_alto_nivel = _alto_tiles * _alto_un_tile;

	// Calcular el número de filas y columnas de la imagen del tileset
	TiXmlElement* imagen_tileset = parser->buscar("image", parser->raiz());
	_filas_tileset = parser->atributoU32("height", imagen_tileset) / _alto_un_tile;
	_columnas_tileset = parser->atributoU32("width", imagen_tileset) / _ancho_un_tile;

	// Leer las propiedades de 'map'
	TiXmlElement* propiedades = parser->buscar("properties", parser->raiz());
	leerPropiedades(propiedades);

	// Reservar memoria para las matrices de tiles
	_escenario[PLATAFORMAS] = (Tile**)memalign(32, _alto_tiles * sizeof(Tile*));
	_escenario[ESCENARIO] = (Tile**)memalign(32, _alto_tiles * sizeof(Tile*));
	for(u16 i = 0 ; i < _alto_tiles ; ++i)
	{
		_escenario[PLATAFORMAS][i] = (Tile*)memalign(32, _ancho_tiles * sizeof(Tile));
		_escenario[ESCENARIO][i] = (Tile*)memalign(32, _ancho_tiles * sizeof(Tile));
	}

	// Leer el escenario (tiles atravesables), tile por tile
	TiXmlElement* escenario = parser->buscar("layer", parser->raiz());
	leerEscenario(escenario);

	// Leer las plataformas (tiles no atravesables, con caja de colision), tile por tile
	TiXmlElement* plataformas = parser->siguiente(escenario);
	leerPlataformas(plataformas);

	// Leer los actores y almacenarlos en la estructura temporal
	TiXmlElement* actores = parser->buscar("objectgroup", parser->raiz());
	leerActores(actores);

	_scroll_x = 0;
	_scroll_y = 0;
}

Nivel::~Nivel(void)
{
	// Destruir las figuras de colision de las plataformas
	for(u16 i = 0 ; i < _filas_tileset ; ++i)
		for(u16 j = 0 ; j < _columnas_tileset ; ++j)
			delete _escenario[PLATAFORMAS][i][j].colision;
	_escenario.clear();

	// Destruir los actores no jugadores
	for(Actores::iterator i = _actores.begin() ; i != _actores.end() ; ++i)
		delete *i;

	// Destruir los actores jugadores
	for(Jugadores::iterator i = _jugadores.begin() ; i != _jugadores.end() ; ++i)
		delete i->second;
}

u32 Nivel::ancho(void) const
{
	return _ancho_nivel;
}

u32 Nivel::alto(void) const
{
	return _alto_nivel;
}

u32 Nivel::xScroll(void) const
{
	return _scroll_x;
}

u32 Nivel::yScroll(void) const
{
	return _scroll_y;
}

const Musica& Nivel::musica(void) const
{
	return galeria->musica(_musica);
}

void Nivel::moverScroll(u32 x, u32 y)
{
	// Si las coordenadas nuevas no hacen que se dibuje en la pantalla una parte del mapa que no existe
	// (que las coordenadas no se salgan del mapa de tiles)
	if(x >= 0 and y >= 0 and x <= _ancho_nivel - screen->ancho() and y <= _alto_nivel - screen->alto())
	{
		_scroll_x = x;
		_scroll_y = y;
	}
}

void Nivel::dibujar(void)
{
	u16 limite_x = screen->ancho();
	u16 limite_y = screen->alto();

	// Dibujar el fondo de pantalla
	screen->dibujarTextura(
				galeria->imagen(_imagen_fondo).textura(),
				0, 0, 900, 
				galeria->imagen(_imagen_fondo).ancho(),
				galeria->imagen(_imagen_fondo).alto());

	// Dibujar los tiles que aparezcan en la pantalla
	for(Escenario::const_iterator capa = _escenario.begin() ; capa != _escenario.end() ; ++capa)
		for(u16 i = 0 ; i < _alto_tiles ; ++i)
			for(u16 j = 0 ; j < _ancho_tiles ; ++j)
				if((capa->second[i][j].x + _ancho_un_tile )>= _scroll_x and
					capa->second[i][j].x <= _scroll_x + limite_x and
					(capa->second[i][j].y + _alto_un_tile) >= _scroll_y and
					capa->second[i][j].y <= _scroll_y + limite_y and
					capa->second[i][j].gid != 0)
				{
					screen->dibujarCuadro(
							galeria->imagen(_imagen_tileset).textura(),
							galeria->imagen(_imagen_tileset).ancho(),
							galeria->imagen(_imagen_tileset).alto(),
							capa->second[i][j].x - _scroll_x, capa->second[i][j].y - _scroll_y, 800,
							((capa->second[i][j].gid - 1) % _columnas_tileset) * (_ancho_un_tile),
							((capa->second[i][j].gid - 1) / _columnas_tileset) * (_alto_un_tile),
							_ancho_un_tile,
							_alto_un_tile);
				}

	// Dibujar los actores no jugadores
	for(Actores::const_iterator i = _actores.begin() ; i != _actores.end() ; ++i)
	{
		bool aparece_x = (u32)(*i)->x() + (*i)->ancho() >= _scroll_x and (u32)(*i)->x() <= _scroll_x + limite_x;
		bool aparece_y = (u32)(*i)->y() + (*i)->alto() >= _scroll_y and (u32)(*i)->y() <= _scroll_y + limite_y;
		// Si esta dentro de los limites de la pantalla
		if(aparece_x and aparece_y)
			(*i)->dibujar((*i)->x() - _scroll_x, (*i)->y() - _scroll_y, 700);
	}

	// Dibujar los actores jugadores
	for(Jugadores::const_iterator i = _jugadores.begin() ; i != _jugadores.end() ; ++i)
		i->second->dibujar(i->second->x() - _scroll_x, i->second->y() - _scroll_y, 9);

}

bool Nivel::colision(const Actor* a)
{
	Tile** mapa = _escenario[PLATAFORMAS];
	Actor::CajasColision actor = a->cajasColision();
	u32 tiles_x0 = a->x() / _ancho_un_tile;
	u32 tiles_x1 = (a->x() + a->ancho()) / _ancho_un_tile;
	u32 tiles_y0 = a->y() / _alto_un_tile;
	u32 tiles_y1 = (a->y() + a->alto()) / _alto_un_tile;

	// Comprobar colisiones solo en la capa PLATAFORMAS, y solo en los tiles que toquen al actor
	for(Actor::CajasColision::iterator i = actor.begin() ; i != actor.end() ; ++i)
		for(u16 y = tiles_y0 ; y <= tiles_y1 ; ++y)
			for(u16 x = tiles_x0 ; x <= tiles_x1 ; ++x)
				if(mapa[y][x].colision != NULL)
					// El actor tiene desplazamiento, pero el tile no
					if(mapa[y][x].colision->hayColision(*i, 0, 0, a->x() + a->velX(), a->y() + a->velY()))
						return true;

	if(colisionBordes(a))
		return true;

	return false;
}

bool Nivel::colisionBordes(const Actor* a)
{
	// Si sale por la izquierda o por arriba
	if(a->x() + a->velX() <= 0 or a->y() + a->velY() <= 0)
		return true;
	// Si sale por la derecha o por abajo
	if(a->x() + a->ancho() + a->velX() >= _ancho_nivel or a->y() + a->alto() + a->velY() >= _alto_nivel)
		return true;
	return false;
}

// Métodos protegidos
void Nivel::leerPropiedades(TiXmlElement* propiedades)
{
	for(TiXmlElement* prop = propiedades->FirstChildElement() ; prop ; prop = prop->NextSiblingElement())		
	{
		string nombre = parser->atributo("name", prop);

		if(nombre == "imagen_fondo")
			_imagen_fondo = parser->atributo("value", prop);
		else if(nombre == "imagen_tileset")
			_imagen_tileset = parser->atributo("value", prop);
		else if(nombre == "musica")
			_musica = parser->atributo("value", prop);
	}
}

void Nivel::leerActores(TiXmlElement* actores)
{
	// Actor temporal auxiliar para leer los actores
	ActorTemp a;

	// Leer los actores
	for(TiXmlElement* actor = actores->FirstChildElement() ; actor ; actor = actor->NextSiblingElement())		
	{
		// Atributos del elemento XML del actor
		a.tipo_actor = parser->atributo("type", actor);
		a.x = parser->atributoU32("x", actor);
		a.y = parser->atributoU32("y", actor);

		// Propiedades del actor: si la primera es jugador, se lee y se prepara la siguiente
		// La propiedad jugador se sabe que va siempre antes, porque Tiled las ordena alfabeticamente
		TiXmlElement* propiedad = parser->buscar("property", actor);
		if(parser->atributo("name", propiedad) == "jugador")
		{
			a.jugador = parser->atributo("value", propiedad);
			propiedad = parser->siguiente(propiedad);
		}
		else
			a.jugador = "";
		a.xml = parser->atributo("value", propiedad);
		_temporal.push_back(a);
	}
}

void Nivel::leerEscenario(TiXmlElement* escenario)
{
	// Por si no hay capa de escenario
	if(escenario == NULL)
		return;

	u32 x = 0;
	u32 y = 0;
	TiXmlElement* tiles = parser->buscar("data", escenario);
	for(TiXmlElement* tile = tiles->FirstChildElement() ; tile ; tile = tile->NextSiblingElement())		
	{
		// Si el atributo gid es mayor que cero, entonces se trata de un tile
		u32 gid = parser->atributoU32("gid", tile);
		_escenario[ESCENARIO][y][x].x = x * _ancho_un_tile;
		_escenario[ESCENARIO][y][x].y = y * _alto_un_tile;
		_escenario[ESCENARIO][y][x].gid = gid;
		_escenario[ESCENARIO][y][x].colision = NULL;

		// Calcular cuando se llega al final de una fila de tiles
		if(++x >= _ancho_tiles)
		{
			x = 0;
			y++;
		}
	}
}

void Nivel::leerPlataformas(TiXmlElement* plataformas)
{
	// Por si no hay capa de plataformas
	if(plataformas == NULL)
		return;

	u32 x = 0;
	u32 y = 0;
	TiXmlElement* tiles = parser->buscar("data", plataformas);
	for(TiXmlElement* tile = tiles->FirstChildElement() ; tile ; tile = tile->NextSiblingElement())		
	{
		// Si el atributo gid es mayor que cero, entonces se trata de un tile
		u32 gid = parser->atributoU32("gid", tile);
		_escenario[PLATAFORMAS][y][x].x = x * _ancho_un_tile;
		_escenario[PLATAFORMAS][y][x].y = y * _alto_un_tile;
		_escenario[PLATAFORMAS][y][x].gid = gid;
		if(gid > 0)
			_escenario[PLATAFORMAS][y][x].colision = new Rectangulo(
					Punto(x * _ancho_un_tile, y * _alto_un_tile),
					Punto((x+1) * _ancho_un_tile, y * _alto_un_tile),
					Punto((x+1) * _ancho_un_tile, (y+1) * _alto_un_tile),
					Punto(x * _ancho_un_tile, (y+1) * _alto_un_tile));
		else
			_escenario[PLATAFORMAS][y][x].colision = NULL;

		// Calcular cuando se llega al final de una fila de tiles
		if(++x >= _ancho_tiles)
		{
			x = 0;
			y++;
		}
	}
}

