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

#include "colision.h"
using namespace std;

Figura* Figura::leerRectangulo(TiXmlElement* nodo)
{
	// Punto 1
	u32 x1 = parser->atributoU32("x1", nodo);
	u32 y1 = parser->atributoU32("y1", nodo);

	// Punto 2
	u32 x2 = parser->atributoU32("x2", nodo);
	u32 y2 = parser->atributoU32("y2", nodo);

	// Punto 3
	u32 x3 = parser->atributoU32("x3", nodo);
	u32 y3 = parser->atributoU32("y3", nodo);

	// Punto 4
	u32 x4 = parser->atributoU32("x4", nodo);
	u32 y4 = parser->atributoU32("y4", nodo);

	return new Rectangulo(Punto(x1, y1), Punto(x2, y2), Punto(x3, y3), Punto(x4, y4));
}

Figura* Figura::leerCirculo(TiXmlElement* nodo)
{
	// Leer el centro
	u32 cx = parser->atributoU32("cx", nodo);
	u32 cy = parser->atributoU32("cy", nodo);
	Punto centro(cx, cy);

	// Leer el radio
	f32 radio = parser->atributoF32("radio", nodo);

	return new Circulo(centro, radio);
}

Figura* Figura::leerPunto(TiXmlElement* nodo)
{
	u32 x = parser->atributoU32("x", nodo);
	u32 y = parser->atributoU32("y", nodo);

	return new Punto(x, y);
}

bool Punto::hayColision(Circulo* c, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	// Si la distancia entre el centro del ciculo y el punto es menor o igual al radio
	f32 distancia_x = abs((c->centro().x() + dx2) - (_x + dx1));
	f32 distancia_y = abs((c->centro().y() + dy2) - (_y + dy1));
	if(sqrt(pow(distancia_x, 2) + pow(distancia_y, 2)) <= c->radio())
		return true;
	return false;
}

bool Punto::hayColision(Rectangulo* r, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	// Si x esta entre r->p1.x y r->p2.x, y y esta entre r->p1.y y r->p4.y
	if((_x + dx1) >= (r->p1().x() + dx2) and (_x + dx1) <= (r->p2().x() + dx2) and
	(_y + dy1) >= (r->p1().y() + dy2) and (_y + dy1) <= (r->p4().y() + dy2))
		return true;
	return false;
}

bool Rectangulo::hayColision(Circulo* c, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	s16 lado_x = abs(_p2.x() - _centro.x()) + c->radio();
	s16 lado_y = abs(_p4.y() - _centro.y()) + c->radio();

	s16 distancia_x = abs((_centro.x() + dx1) - (c->centro().x() + dx2));
	s16 distancia_y = abs((_centro.y() + dy1) - (c->centro().y() + dy2));
 
	if(distancia_x <= lado_x and distancia_y <= lado_y)
		return true;
	return false;
}

bool Rectangulo::hayColision(Rectangulo* r, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	bool contacto_x = false;
	bool contacto_y = false;

	// Saber si el ancho (x) y alto (y) de this es mayor que el ancho o alto de r
	bool rx_mayor = (abs(r->p2().x() - r->p1().x()) > abs(_p2.x() - _p1.x()));
	bool ry_mayor = (abs(r->p1().y() - r->p4().y()) > abs(_p1.y() - _p4.y()));

	// Si this es mas ancho que r
	if(rx_mayor)
	{
		// Se comprueba si p1 de this esta entre p1 y p2 de r en x
		if((_p1.x() + dx1) <= (r->p2().x() + dx2) and (_p1.x() + dx1) >= (r->p1().x() + dx2))
			contacto_x = true;
		// Se comprueba si p2 de this esta entre p1 y p2 de r en x
		if((_p2.x() + dx1) <= (r->p2().x() + dx2) and (_p2.x() + dx1) >= (r->p1().x() + dx2))
			contacto_x = true;
	}
	else
	{
		// Se comprueba si p1 de r esta entre p1 y p2 de this en x
		if((r->p1().x() + dx2) <= (_p2.x() + dx1) and (r->p1().x() + dx2) >= (_p1.x() + dx1))
			contacto_x = true;
		// Se comprueba si p2 de r esta entre p1 y p2 de this en x
		if((r->p2().x() + dx2) <= (_p2.x() + dx1) and (r->p2().x() + dx2) >= (_p1.x() + dx1))
			contacto_x = true;
	}

	// Si this es mas alto que r
	if(ry_mayor)
	{
		// Se comprueba si p1 de this esta entre p1 y p4 de r en y
		if((_p1.y() + dy1) >= (r->p1().y() + dy2) and (_p1.y() + dy1) <= (r->p4().y() + dy2))
			contacto_y = true;

		// Se comprueba si p4 de this esta entre p1 y p4 de r en y
		if((_p4.y() + dy1) >= (r->p1().y() + dy2) and (_p4.y() + dy1) <= (r->p4().y() + dy2))
			contacto_y = true;
	}
	else
	{
		// Se comprueba si p1 de r esta entre p1 y p4 de this en y
		if((r->p1().y() + dy2) >= (_p1.y() + dy1) and (r->p1().y() + dy2) <= (_p4.y() + dy1))
			contacto_y = true;

		// Se comprueba si p4 de r esta entre p1 y p4 de this en y
		if((r->p4().y() + dy2) >= (_p1.y() + dy1) and (r->p4().y() + dy2) <= (_p4.y() + dy1))
			contacto_y = true;
	}

	if(contacto_x and contacto_y)
		return true;
	return false;
}

bool Rectangulo::hayColision(Punto* p, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	// Si p->x esta entre p1.x y p2.x, y p->y esta entre p1.y y p4.y
	if((p->x() + dx2) >= (_p1.x() + dx1) and (p->x() + dx2) <= (_p2.x() + dx1) and
	(p->y() + dy2) >= (_p1.y() + dy1) and (p->y() + dy2) <= (_p4.y() + dy1))
		return true;
	return false;
}

bool Circulo::hayColision(Circulo* c, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	f32 distancia_x = abs((_centro.x() + dx1) - (c->centro().x() + dx2));
	f32 distancia_y = abs((_centro.y() + dy1) - (c->centro().y() + dy2));
	if(pow(distancia_x, 2) + pow(distancia_y, 2) <= pow(_radio + c->radio(), 2))
		return true;
	return false;
}

bool Circulo::hayColision(Rectangulo* r, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	s16 lado_x = abs(r->p2().x() - r->centro().x()) + _radio;
	s16 lado_y = abs(r->p4().y() - r->centro().y()) + _radio;

	s16 distancia_x = abs((r->centro().x() + dx2) - (_centro.x() + dx1));
	s16 distancia_y = abs((r->centro().y() + dy2) - (_centro.y() + dy1));
 
    if(distancia_x <= lado_x and distancia_y <= lado_y)
        return true;
     return false;
}

bool Circulo::hayColision(Punto* p, s16 dx1, s16 dy1, s16 dx2, s16 dy2)
{
	// Si la distancia entre el centro del ciculo y el punto es menor o igual al radio
	f32 distancia_x = abs((_centro.x() + dx1) - (p->x() + dx2));
	f32 distancia_y = abs((_centro.y() + dy1) - (p->y() + dy2));
	if(pow(distancia_x, 2) + pow(distancia_y, 2) <= pow(_radio, 2))
		return true;
	return false;
}

