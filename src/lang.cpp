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

#include "lang.h"
using namespace std;

Lang* Lang::_instance = 0;

void Lang::activarIdioma(const wstring& idioma) throw (CodigoEx)
{
	if(not existeIdioma(idioma))
		throw CodigoEx("Lang::activarIdioma - No existe el idioma seleccionado para activar");
	_idioma_activo = idioma;
}

const wstring& Lang::idiomaActivo(void) const
{
	return _idioma_activo;
}

bool Lang::existeIdioma(const wstring& idioma) const
{
	if(_idiomas.find(idioma) == _idiomas.end())
		return false;
	return true;
}

void Lang::cambiarIdioma(void)
{
	map<wstring, Idioma*>::iterator it =  _idiomas.find(_idioma_activo);
	if(++it == _idiomas.end())
		_idioma_activo = _idiomas.begin()->first;
	else
		_idioma_activo = it->first;
}

const wstring& Lang::texto(const string& tag) throw (CodigoEx)
{
	Idioma* activo = _idiomas[_idioma_activo];
	Idioma::iterator it = activo->find(tag);
	if(it == activo->end())
		throw CodigoEx("Lang::texto - El tag '" + tag + "' no existe en el idioma activo");
	return (it->second);
}

void Lang::inicializar(const string& ruta, const wstring& idioma_activo) throw (ArchivoEx, CodigoEx, TarjetaEx, XmlEx)
{
	// Comprobar que la SD está montada
	if(not sdcard->montada())
		throw TarjetaEx("Lang - La tarjeta SD no está montada");

	// Abrir el archivo XML
	try {
		parser->cargar(ruta);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Recorrer todos los idiomas presentes en el archivo
	for(TiXmlElement* nodo = parser->raiz()->FirstChildElement() ; nodo ; nodo = nodo->NextSiblingElement())
	{
		// Leer el nombre del idioma
		string nombre = parser->atributo("nombre", nodo);

		if(nombre == "")
			throw XmlEx("Lang::inicializar - Error al leer el nombre de un idioma");

		// Convertir el nombre del idioma a wstring
		wstring nombre_idioma = utf32::convertir(nombre);

		// Crear el idioma y guardarlo en el diccionario junto con su nombre
		Idioma* idioma = new Idioma;
		_idiomas.insert(make_pair(nombre_idioma, idioma));

		// Recorrer todos los tag del idioma
		for(TiXmlElement* tag = nodo->FirstChildElement() ; tag ; tag = tag->NextSiblingElement())
		{
			// Leer los atributos
			string nombre_tag = parser->atributo("nombre", tag);
			string valor = parser->atributo("valor", tag);

			if(nombre_tag == "" or valor == "")
				throw XmlEx("Lang::inicializar - Error al cargar un atributo de un tag");

			// Convertir el valor del tag a formato UTF32
			wstring tag_valor = utf32::convertir(valor);

			// Insertar la etiqueta y su valor en el idioma correspondiente
			_idiomas[nombre_idioma]->insert(make_pair(nombre_tag, tag_valor));
		}
	}

	// Establecer el idioma activo en el sistema de idiomas
	try {
		activarIdioma(idioma_activo);
	} catch(const Excepcion& e) {
		throw e;
	}
}

// Protegidos
Lang::~Lang(void)
{
	for(map<wstring, Idioma*>::iterator i = _idiomas.begin() ; i != _idiomas.end() ; ++i)
		delete i->second;
}

