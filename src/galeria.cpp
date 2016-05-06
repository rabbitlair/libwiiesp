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

#include "galeria.h"
using namespace std;

Galeria* Galeria::_instance = 0;

const Imagen& Galeria::imagen(const string& codigo) throw (CodigoEx)
{
	if(_map_imagen.find(codigo) == _map_imagen.end())
		throw CodigoEx("Galeria::imagen - Código erróneo (" + codigo + ")");
	return *(_map_imagen[codigo]);
}

const Musica& Galeria::musica(const string& codigo) throw (CodigoEx)
{
	if(_map_musica.find(codigo) == _map_musica.end())
		throw CodigoEx("Galeria::musica - Código erróneo (" + codigo + ")");
	return *(_map_musica[codigo]);
}

const Sonido& Galeria::sonido(const string& codigo) throw (CodigoEx)
{
	if(_map_sonido.find(codigo) == _map_sonido.end())
		throw CodigoEx("Galeria::sonido - Código erróneo (" + codigo + ")");
	return *(_map_sonido[codigo]);
}

const Fuente& Galeria::fuente(const string& codigo) throw (CodigoEx)
{
	if(_map_fuente.find(codigo) == _map_fuente.end())
		throw CodigoEx("Galeria::fuente - Código erróneo (" + codigo + ")");
	return *(_map_fuente[codigo]);
}

void Galeria::inicializar(const string& ruta) throw (ArchivoEx, ImagenEx, TarjetaEx, XmlEx)
{
	// Comprobar que la SD está montada
	if(not sdcard->montada())
		throw TarjetaEx("Galeria - La tarjeta SD no está montada");

	// Abrir el archivo XML
	try {
		parser->cargar(ruta);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Recorrer todos los nodos hijo de la raiz
	for(TiXmlElement* media = parser->raiz()->FirstChildElement() ; media ; media = media->NextSiblingElement())
	{
		string tipo = media->ValueStr();

		// Según el tipo de nodo, llamar a una función de carga o a otra
		try {
			if(tipo == "imagen")
				leerImagen(media);
			else if(tipo == "musica")
				leerMusica(media);
			else if(tipo == "sonido")
				leerSonido(media);
			else if(tipo == "fuente")
				leerFuente(media);
		} catch(const Excepcion& e) {
			throw e;
		}
	}
}

// Métodos protegidos
Galeria::~Galeria(void)
{
	for(map<string, Imagen*>::iterator i = _map_imagen.begin() ; i != _map_imagen.end() ; ++i)
		delete i->second;
	for(map<string, Musica*>::iterator i = _map_musica.begin() ; i != _map_musica.end() ; ++i)
		delete i->second;
	for(map<string, Sonido*>::iterator i = _map_sonido.begin() ; i != _map_sonido.end() ; ++i)
		delete i->second;
	for(map<string, Fuente*>::iterator i = _map_fuente.begin() ; i != _map_fuente.end() ; ++i)
		delete i->second;
}

// Métodos privados
void Galeria::leerImagen(TiXmlElement* nodo) throw (ArchivoEx, ImagenEx, TarjetaEx, XmlEx)
{
	// Leer los atributos del tag XML
	string codigo = parser->atributo("codigo", nodo);
	string ruta = parser->atributo("ruta", nodo);
	string formato = parser->atributo("formato", nodo);

	if(codigo == "" or ruta == "" or formato == "")
		throw XmlEx("Galeria::leerImagen - Error al cargar un atributo");

	// Cargar la imagen
	Imagen* i = new Imagen;
	if(formato == "bmp")
		try {
			i->cargarBmp(ruta);
		} catch(const Excepcion& e) {
			delete i;
			throw e;
		}
	else
	{
		delete i;
		throw ImagenEx("Galeria::leerImagen - Formato de imagen no soportado");
	}

	// Insertar la imagen en el diccionario
	_map_imagen.insert(make_pair(codigo, i));
}

void Galeria::leerMusica(TiXmlElement* nodo) throw (ArchivoEx, TarjetaEx, XmlEx)
{
	// Leer los atributos del tag XML
	string codigo = parser->atributo("codigo", nodo);
	string ruta = parser->atributo("ruta", nodo);
	u32 volumen = parser->atributoU32("volumen", nodo);

	if(codigo == "" or ruta == "")
		throw XmlEx("Galeria::leerMusica - Error al cargar un atributo");

	// Cargar la pista de música
	Musica* m;
	try {
		m = new Musica(ruta, volumen);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Insertar la pista de música en el diccionario
	_map_musica.insert(make_pair(codigo, m));
}

void Galeria::leerSonido(TiXmlElement* nodo) throw (ArchivoEx, TarjetaEx, XmlEx)
{
	// Leer los atributos del tag XML
	string codigo = parser->atributo("codigo", nodo);
	string ruta = parser->atributo("ruta", nodo);
	u32 volumen = parser->atributoU32("volumen", nodo);

	if(codigo == "" or ruta == "")
		throw XmlEx("Galeria::leerSonido - Error al cargar un atributo");

	// Cargar el sonido
	Sonido* s;
	try {
		s = new Sonido(ruta, volumen, volumen);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Insertar el sonido en el diccionario
	_map_sonido.insert(make_pair(codigo, s));
}

void Galeria::leerFuente(TiXmlElement* nodo) throw (ArchivoEx, TarjetaEx, XmlEx)
{
	// Leer los atributos del tag XML
	string codigo = parser->atributo("codigo", nodo);
	string ruta = parser->atributo("ruta", nodo);

	if(codigo == "" or ruta == "")
		throw XmlEx("Galeria::leerFuente - Error al cargar un atributo");

	// Cargar la fuente
	Fuente* f;
	try {
		f = new Fuente(ruta);
	} catch(const Excepcion& e) {
		throw e;
	}

	// Insertar la fuente en el diccionario
	_map_fuente.insert(make_pair(codigo, f));
}

