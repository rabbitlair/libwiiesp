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

#include "actor.h"
#include "nivel.h"
using namespace std;

Actor::Actor(const std::string& ruta, const Nivel* nivel) throw (ArchivoEx, CodigoEx, TarjetaEx, XmlEx)
: _nivel(nivel)
{
	_x = _y = _x_previo = _y_previo = 0;
	_estado_previo = _estado_actual = "normal";
	_invertida = false;
	try {
		cargarDatosIniciales(ruta);
	} catch(const Excepcion& e) {
		throw e;
	}
}

Actor::~Actor(void)
{
	// Eliminar todas las animaciones
	for(Animaciones::iterator i = _map_animaciones.begin() ; i != _map_animaciones.end() ; ++i)
		delete i->second;
	_map_animaciones.clear();

	// Eliminar todas las cajas de colisión
	for(Colisiones::iterator i = _map_colisiones.begin() ; i != _map_colisiones.end() ; ++i)
		for(CajasColision::iterator j = i->second.begin() ; j != i->second.end() ; ++j)
			delete *j;
	_map_colisiones.clear();
}

// Métodos consultores

u32 Actor::x(void) const
{
	return _x;
}

u32 Actor::y(void) const
{
	return _y;
}

u32 Actor::xPrevio(void) const
{
	return _x_previo;
}

u32 Actor::yPrevio(void) const
{
	return _y_previo;
}

s16 Actor::velX(void) const
{
	return _vx;
}

s16 Actor::velY(void) const
{
	return _vy;
}

const string& Actor::estado(void) const
{
	return _estado_actual;
}

const string& Actor::estadoPrevio(void) const
{
	return _estado_previo;
}

const Actor::CajasColision& Actor::cajasColision(void) const
{
	Colisiones::const_iterator i = _map_colisiones.find(_estado_actual);

	// Si no se ha encontrado cajas de colision para el estado actual, se toma el estado normal
	if(i == _map_colisiones.end())
		return _map_colisiones.find("normal")->second;
	else
		return _map_colisiones.find(_estado_actual)->second;
}

u16 Actor::ancho(void) const
{
	Animaciones::const_iterator i = _map_animaciones.find(_estado_actual);

	// Si no se ha encontrado una animación para el estado actual, se toma el estado normal
	if(i == _map_animaciones.end())
		return _map_animaciones.find("normal")->second->ancho();
	else
		return _map_animaciones.find(_estado_actual)->second->ancho();
}

u16 Actor::alto(void) const
{
	Animaciones::const_iterator i = _map_animaciones.find(_estado_actual);

	// Si no se ha encontrado una animación para el estado actual, se toma el estado normal
	if(i == _map_animaciones.end())
		return _map_animaciones.find("normal")->second->alto();
	else
		return _map_animaciones.find(_estado_actual)->second->alto();
}

const string& Actor::tipoActor(void) const
{
	return _tipo_actor;
}

// Métodos modificadores

void Actor::mover(u32 x, u32 y)
{
	// Evitar la salida del nivel horizontalmente
	if(x > 0 and x + ancho() < _nivel->ancho())
	{
		_x_previo = _x;
		_x = x;
	}

	// Evitar la salida del nivel verticalmente
	if(y > 0 and y + alto() < _nivel->alto())
	{
		_y_previo = _y;
		_y = y;
	}
}

void Actor::setVelX(s16 vx)
{
	_vx = vx;
}

void Actor::setVelY(s16 vy)
{
	_vy = vy;
}

bool Actor::setEstado(const string& e)
{
	Animaciones::const_iterator i = _map_animaciones.find(e);
	Colisiones::const_iterator j = _map_colisiones.find(e);

	if(i != _map_animaciones.end() and j != _map_colisiones.end())
	{
		_estado_previo = _estado_actual;
		_estado_actual = e;
		return true;
	}
	return false;
}

bool Actor::colision(const Actor& a)
{
	CajasColision externo = a.cajasColision();
	CajasColision interno = cajasColision();

	for(CajasColision::iterator i = externo.begin() ; i != externo.end() ; ++i)
		for(CajasColision::iterator j = interno.begin() ; j != interno.end() ; ++j)
			if((*i)->hayColision(*j, a.x() + a.velX(), a.y() + a.velY(), _x + _vx, _y + _vy))
				return true;

	return false;
}

void Actor::invertirDibujo(bool inv)
{
	_invertida = inv;
}

void Actor::dibujar(s16 x, s16 y, s16 z)
{
	Animaciones::const_iterator i = _map_animaciones.find(_estado_actual);

	// Si no se ha encontrado una animación para el estado actual, se toma el estado normal
	if(i == _map_animaciones.end())
		_map_animaciones["normal"]->dibujar(x, y, z, _invertida);
	else
		_map_animaciones[_estado_actual]->dibujar(x, y, z, _invertida);
}

// Métodos protegidos

void Actor::cargarDatosIniciales(const string& ruta) throw (ArchivoEx, TarjetaEx, CodigoEx, XmlEx)
{
	// Comprobar que la SD está montada
	if(not sdcard->montada())
		throw TarjetaEx("Actor::cargarDatosIniciales - La tarjeta SD no está montada");

	// Abrir el archivo XML
	try {
		parser->cargar(ruta);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Datos de velocidad y dirección
	u32 vx = parser->atributoU32("vx", parser->raiz());
	_vx = vx;
	u32 vy = parser->atributoU32("vy", parser->raiz());
	_vy = vy;
	_tipo_actor = parser->atributo("tipo", parser->raiz());

	// Cargar las animaciones y las colisiones
	leerColisiones(parser->buscar("colisiones"));
	try {
		leerAnimaciones(parser->buscar("animaciones"));
	} catch(const Excepcion& e) {
		throw e;
	}
}

void Actor::leerAnimaciones(TiXmlElement* nodo) throw (CodigoEx, XmlEx)
{
	// Recorrer los nodos animacion
	for(TiXmlElement* hijo = nodo->FirstChildElement() ; hijo ; hijo = hijo->NextSiblingElement())
	{
		// Leer todos los atributos esperados
		string estado = parser->atributo("estado", hijo);
		string codigo_imagen = parser->atributo("img", hijo);
		string secuencia = parser->atributo("sec", hijo);
		u32 filas = parser->atributoU32("filas", hijo);
		u32 columnas = parser->atributoU32("columnas", hijo);
		u32 retardo = parser->atributoU32("retardo", hijo);

		if(estado == "" or codigo_imagen == "" or secuencia == "" or filas == 0 or columnas == 0)
			throw XmlEx("Actor::leerAnimaciones - Error al leer un atributo");

		// Comprobar que no existe una animación para el estado
		if(_map_animaciones.find(estado) != _map_animaciones.end())
			throw CodigoEx("Actor::leerAnimaciones - Ya existe una animación para el estado '" + estado + "'");

		// Crear la animacion y guardarla en el correspondiente estado
		Animacion* a = new Animacion(galeria->imagen(codigo_imagen), secuencia, filas, columnas, retardo);
		_map_animaciones.insert(make_pair(estado, a));
	}
}

void Actor::leerColisiones(TiXmlElement* nodo)
{
	// Recorrer los nodos de colision
	for(TiXmlElement* hijo = nodo->FirstChildElement() ; hijo ; hijo = hijo->NextSiblingElement())
	{
		// Leer el estado al que se asociará la figura
		string estado = parser->atributo("estado", hijo);

		// Identificar el tipo de nodo
		string tipo = hijo->ValueStr();
		Figura* figura = NULL;

		// Leer el tipo de figura
		if(tipo == "rectangulo")
			figura = Figura::leerRectangulo(hijo);
		else if(tipo == "circulo")
			figura = Figura::leerCirculo(hijo);
		else if(tipo == "punto")
			figura = Figura::leerPunto(hijo);

		// Guardar la colision en el conjunto del estado e
		// Si no existe el estado como clave en el mapa de colisiones, añadirlo
		if(_map_colisiones.find(estado) == _map_colisiones.end())
			_map_colisiones.insert(make_pair(estado, CajasColision()));
		if(figura != NULL)
			_map_colisiones[estado].insert(figura);
	}
}

