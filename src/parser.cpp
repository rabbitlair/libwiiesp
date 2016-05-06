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

#include "parser.h"
using namespace std;

Parser* Parser::_instance = 0;

void Parser::cargar(const std::string& ruta) throw (ArchivoEx, TarjetaEx)
{
	// Comprobar que la SD está montada
	if(not sdcard->montada())
		throw TarjetaEx("Parser - La tarjeta SD no está montada");

	// Añadir a la ruta la unidad que esté montada en la clase sdcard como prefijo
	string archivo = sdcard->unidad() + ":" + ruta;

	// Comprobar la existencia del archivo
	if(not sdcard->existe(archivo))
		throw ArchivoEx("Parser::cargar - El archivo '" + archivo + "' no existe.");

	_doc = TiXmlDocument(archivo);

	try{
		_doc.LoadFile(TIXML_ENCODING_UTF8);
	}
	catch(...) {
		throw ArchivoEx("Parser - Error al cargar el archivo '" + archivo + "'");
	}
}

TiXmlElement* Parser::raiz(void)
{
	return _doc.RootElement();
}

string Parser::contenido(const TiXmlElement* el) const
{
	if(not el)
		return string();
	char* temp = (char*)el->GetText();
	if(temp == NULL)
		return string();
	string a(temp);
	free(temp);
	return a;
}

string Parser::atributo(const string& nombre, const TiXmlElement* el) const
{
	if(not el)
		return string();
	string temp;
	if(el->QueryStringAttribute(nombre.c_str(), &temp) != TIXML_SUCCESS)
		return string();
	return temp;
}

u32 Parser::atributoU32(const std::string& nombre, const TiXmlElement* el) const
{
	if(not el)
		return 0;
	u32 temp;
	if(el->QueryIntAttribute(nombre.c_str(), (int*)&temp) != TIXML_SUCCESS)
		return 0;
	return temp;
}

f32 Parser::atributoF32(const std::string& nombre, const TiXmlElement* el) const
{
	if(not el)
		return 0.0;
	f32 temp;
	if(el->QueryFloatAttribute(nombre.c_str(), (float*)&temp) != TIXML_SUCCESS)
		return 0.0;
	return temp;
}

TiXmlElement* Parser::buscar(const std::string& valor, TiXmlElement* el)
{
	TiXmlElement* padre = el;
	if(not padre)
		padre = _doc.RootElement();

	bool stop = false;
	TiXmlElement* e = NULL;

	if(padre){
		if(padre->ValueStr() == valor)
			return padre;
		else
			buscar_aux(valor, padre, e, stop);
	}

	return e;
}

TiXmlElement* Parser::siguiente(TiXmlElement* el)
{
	if(not el)
		return NULL;

	string tipo = el->ValueStr();

	for(TiXmlElement* hermano = el->NextSiblingElement() ; hermano ; hermano = hermano->NextSiblingElement())
	{
		if(hermano->ValueStr() == tipo)
			return hermano;
	}
	return NULL;
}

void Parser::buscar_aux(const std::string& valor, TiXmlElement* padre, TiXmlElement*& el, bool& stop)
{
	if(padre and not stop){
		for(TiXmlElement* hijo = padre->FirstChildElement() ; hijo and not stop ; hijo = hijo->NextSiblingElement())
		{
			if(hijo->ValueStr() == valor)
			{
				el = hijo;
				stop = true;
			}
			else
				buscar_aux(valor, hijo, el, stop);
		}
	}
}

