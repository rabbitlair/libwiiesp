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

#include "escenario.h"
#include "wiipang.h"
using namespace std;

Escenario::Escenario(const std::string& ruta, const Wiipang* juego) throw (Excepcion)
: Nivel(ruta), _wiipang(juego), _personaje(NULL), _gancho1(NULL), _gancho2(NULL)
{
	_num_bolas = 0;
	cargarActores();
	_fin_nivel = false;
	_reiniciar = false;

	// Leer propiedades extras del escenario: limites de la zona de juego, y la ruta al archivo de la bola
	parser->cargar(ruta);
	TiXmlElement* propiedades = parser->buscar("properties", parser->raiz());
	for(TiXmlElement* prop = propiedades->FirstChildElement() ; prop ; prop = prop->NextSiblingElement())		
	{
		string nombre = parser->atributo("name", prop);

		if(nombre == "x0")
			_x0 = parser->atributoU32("value", prop);
		else if(nombre == "x1")
			_x1 = parser->atributoU32("value", prop);
		else if(nombre == "y0")
			_y0 = parser->atributoU32("value", prop);
		else if(nombre == "y1")
			_y1 = parser->atributoU32("value", prop);
		else if(nombre == "xml_bola")
			_xml_bola = parser->atributo("value", prop);
		else if(nombre == "xml_gancho")
			_xml_gancho = parser->atributo("value", prop);
	}
}

Escenario::~Escenario(void)
{
}

void Escenario::actualizarPj(const std::string& jugador, const Mando& m)
{
	// Si el personaje ha muerto y ha salido de la pantalla, será NULL
	if(_personaje == NULL)
		return;

	string estado = _jugadores[jugador]->estado();

	// Desplazamiento hacia la izquierda
	if(m.pressed(Mando::BOTON_ARRIBA) and estado != "muerto" and estado != "ganar")
	{
		// Cambiar el estado del actor
		if(_jugadores[jugador]->estado() == "normal" or _jugadores[jugador]->estado() == "mover")
			_jugadores[jugador]->setEstado("mover");

		// Si hay colision con el escenario, hacer retroceder al actor a la posicion anterior
		if(colision(_jugadores[jugador]))
			_jugadores[jugador]->mover(
				_jugadores[jugador]->x() - _jugadores[jugador]->velX(), _jugadores[jugador]->y());

		// Orientar el vector de movimiento hacia la izquierda
		s16 vel_x = _jugadores[jugador]->velX();
		if(vel_x > 0)
			vel_x *= -1;
		_jugadores[jugador]->setVelX(vel_x);

		// Orientar el dibujo hacia la izquierda
		_jugadores[jugador]->invertirDibujo(true);
	}
	// Desplazamiento hacia la derecha
	else if(m.pressed(Mando::BOTON_ABAJO) and estado != "muerto" and estado != "ganar")
	{
		// Cambiar el estado del actor
		if(_jugadores[jugador]->estado() == "normal" or _jugadores[jugador]->estado() == "mover")
			_jugadores[jugador]->setEstado("mover");

		// Si hay colision con el escenario, hacer retroceder al actor a la posicion anterior
		if(colision(_jugadores[jugador]))
			_jugadores[jugador]->mover(
				_jugadores[jugador]->x() - _jugadores[jugador]->velX(), _jugadores[jugador]->y());

		// Orientar el vector de movimiento hacia la derecha
		s16 vel_x = _jugadores[jugador]->velX();
		if(vel_x < 0)
			vel_x *= -1;
		_jugadores[jugador]->setVelX(vel_x);

		// Orientar el dibujo hacia la derecha
		_jugadores[jugador]->invertirDibujo(false);
	}
	// No hay desplazamiento del personaje
	else
	{
		if(_jugadores[jugador]->estado() == "mover")
			_jugadores[jugador]->setEstado("normal");
	}

	// Disparar un gancho
	if(m.newPressed(Mando::BOTON_2) and (estado == "normal" or estado == "mover"))
	{
		if(_gancho1 == NULL)
		{
			_gancho1 = new Gancho(_xml_gancho, this, 1);
			_gancho1->mover(_personaje->x() + _personaje->ancho() / 2, _y1 - _gancho1->alto() - 1);
			_actores.push_back(_gancho1);
		}
		else if(_gancho2 == NULL)
		{
			_gancho2 = new Gancho(_xml_gancho, this, 2);
			_gancho2->mover(_personaje->x() + _personaje->ancho() / 2, _y1 - _gancho1->alto() - 1);
			_actores.push_back(_gancho2);
		}
	}

	// Actualizar el actor
	_jugadores[jugador]->actualizar();

	// Si el personaje con el estado "muerto" sale de la pantalla, se elimina
	if(colisionBordes(_jugadores[jugador]))
	{
		delete _personaje;
		_personaje = NULL;
		_jugadores.erase(jugador);
		usleep(500000);
	}
}

void Escenario::actualizarNpj(void)
{
	// Si el personaje no existe, o tiene el estado "muerto" no se actualiza nada
	if(_personaje == NULL or _personaje->estado() == "muerto")
		return;

	for(Actores::iterator i = _actores.begin() ; i != _actores.end() ; ++i)
	{
		// Si el actor es una bola
		if((*i)->tipoActor() == "bola")
		{
			Bola* pBola = static_cast<Bola*>(*i);
			// Si hay colision con el escenario
			if(colision(*i))
			{
				// Colision horizontal
				if((*i)->x() < _x0 or (*i)->x() + (*i)->ancho() > _x1)
					(*i)->setVelX((*i)->velX() * (-1));
	
				// Colision vertical
				if((*i)->y() < _y0)
					pBola->setVyCero();
				else if((*i)->y() + (*i)->alto() > _y1)
					pBola->setVyInicial();
			}
			// Colision de la bola con el personaje: el personaje muere
			if((*i)->colision(*_personaje))
			{
				usleep(500000);
				_personaje->setEstado("muerto");
				const_cast<Wiipang*>(_wiipang)->vidas()--;
				_reiniciar = true;
			}
			// Colision de la bola con un gancho: el gancho desaparece y la bola se deshace en otras dos
			if(_gancho1 != NULL and (*i)->colision(*_gancho1))
			{
				romperBola(pBola);
				delete *i;
				*i = NULL;
				_num_bolas--;
				const_cast<Wiipang*>(_wiipang)->puntos() += 100;
				_gancho1->setDestruir();
			}
			else if(_gancho2 != NULL and (*i)->colision(*_gancho2))
			{
				romperBola(pBola);
				delete *i;
				*i = NULL;
				_num_bolas--;
				const_cast<Wiipang*>(_wiipang)->puntos() += 100;
				_gancho2->setDestruir();
			}
		}
		else if((*i)->tipoActor() == "gancho")
		{
			Gancho* pGancho = static_cast<Gancho*>(*i);

			// Eliminar el gancho si choca con el escenario o esta marcado para destruir
			if(colision(*i) or pGancho->destruir())
			{
				u8 id = pGancho->id();
				if(id == 1)
				{
					delete *i;
					_gancho1 = NULL;
				}
				else if(id == 2)
				{
					delete *i;
					_gancho2 = NULL;
				}
				*i = NULL;
			}
		}

		// Si el actor no ha sido eliminado, actualizarlo, en caso contrario, quitarlo del vector de actores
		if(*i != NULL)
			(*i)->actualizar();
		else
		{
			i = _actores.erase(i);
			i--;
		}
	}

	// Volcar las nuevas bolas en el vector de actores no jugadores
	for(vector<Bola*>::iterator i = _bolas_aux.begin() ; i != _bolas_aux.end() ; ++i)
		_actores.push_back(*i);
	_bolas_aux.clear();
}

void Escenario::actualizarEscenario(void)
{
	// Comprobaciones de fin de nivel
	if(_personaje == NULL or _num_bolas == 0)
		_fin_nivel = true;
	if(_num_bolas == 0)
		_jugadores.begin()->second->setEstado("ganar");
}

bool Escenario::finNivel(void) const
{
	return _fin_nivel;
}

bool Escenario::reiniciar(void) const
{
	return _reiniciar;
}

u32 Escenario::limiteInferior(void) const
{
	return _y1 - _alto_un_tile;
}

void Escenario::cargarActores(void)
{
	for(Temporal::iterator i = _temporal.begin() ; i != _temporal.end() ; ++i)
	{
		if(i->tipo_actor.find("bola-") != string::npos)
		{
			// Obtener el color y la talla de la bola separando la cadena por los guiones
			char* temp = NULL;
			temp = strtok((char*)i->tipo_actor.c_str(), "-");
			string color = strtok(NULL, "-");
			string talla = strtok(NULL, "-");
			string direccion = strtok(NULL, "-");
			// Crear el actor
			Bola* b = new Bola(i->xml, this, color, talla);
			b->mover(i->x, i->y);
			if(direccion == "iz")
				b->setVelX(b->velX() * -1);
			_actores.push_back(b);
			_num_bolas++;
		}
		if(i->tipo_actor == "personaje")
		{
			_personaje = new Personaje(i->xml, this);
			_personaje->mover(i->x, i->y);
			_jugadores.insert(std::make_pair(i->jugador, _personaje));
		}
	}
	_temporal.clear();
}

void Escenario::romperBola(Bola* bola)
{
	// Si la bola tiene talla "s", salir
	if(bola->talla() == "s")
		return;

	// Obtener los datos donde apareceran las nuevas bolas
	u32 x = bola->x() + bola->ancho() / 2;
	u32 y = bola->y() + bola->alto() / 2;
	s16 vel_x = bola->velX();
	string color = bola->color();
	string talla;
	if(bola->talla() == "xl")
		talla = "l";
	if(bola->talla() == "l")
		talla = "m";
	if(bola->talla() == "m")
		talla = "s";

	_num_bolas += 2;

	// TODO crear una factoria de actores en LibWiiEsp y utilizarla aqui
	Bola* b1 = new Bola(_xml_bola, this, color, talla);
	b1->mover(x - b1->ancho() / 2, y - b1->alto() / 2);
	b1->setVelX(vel_x);
	b1->setVyReal(-5);
	_bolas_aux.push_back(b1);

	Bola* b2 = new Bola(_xml_bola, this, color, talla);
	b2->mover(x - b2->ancho() / 2, y - b2->alto() / 2);
	b2->setVelX(vel_x * -1);
	b2->setVyReal(-5);
	_bolas_aux.push_back(b2);
}

