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
#include "arkanoid.h"
using namespace std;

Escenario::Escenario(const std::string& ruta, const Arkanoid* juego) throw (Excepcion)
: Nivel(ruta), _num_ladrillos(0), _multiplicador(1), _bola(NULL), _arkanoid(juego), _control(BOTONES)
{
	_fin_nivel = false;
	_item_activo = false;
	cargarActores();
	srand(time(0));

	// Leer propiedades extras del escenario: limites de la zona de juego, y archivos de la bola y los items
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
		else if(nombre == "xml_item")
			_xml_item = parser->atributo("value", prop);
	}
}

Escenario::~Escenario(void)
{
}

void Escenario::actualizarPj(const std::string& jugador, const Mando& m)
{
//	TODO Para acabar un nivel inmediatamente
//	if(m.newPressed(Mando::BOTON_B))
//		_num_ladrillos = 0;

	// Lanzar la bola si no existe
	if(_bola == NULL and m.newPressed(Mando::BOTON_2))
	{
		_bola = new Bola(_xml_bola, this);
		_bola->mover(_pala->x() + 10, _pala->y() - 30);
		_actores.push_back(_bola);
	}

	// Desplazar la pala hacia la izquierda
	if((m.pressed(Mando::BOTON_ARRIBA) and _control == BOTONES) or (m.cabeceo() < -15.0 and _control == CABECEO))
	{
		// Cambiar el estado del actor
		if(_jugadores[jugador]->estado() == "normal" or _jugadores[jugador]->estado() == "mover")
			_jugadores[jugador]->setEstado("mover");
		else if(_jugadores[jugador]->estado() == "normalg" or _jugadores[jugador]->estado() == "moverg")
			_jugadores[jugador]->setEstado("moverg");
		else if(_jugadores[jugador]->estado() == "normalp" or _jugadores[jugador]->estado() == "moverp")
			_jugadores[jugador]->setEstado("moverp");

		// Si hay colision con el escenario, hacer retroceder al actor a la posicion anterior
		if(colision(_jugadores[jugador]))
			_jugadores[jugador]->mover(
				_jugadores[jugador]->x() - _jugadores[jugador]->velX(), _jugadores[jugador]->y());

		// Orientar el vector de movimiento hacia la izquierda
		s16 vel_x = _jugadores[jugador]->velX();
		if(vel_x > 0)
			vel_x *= -1;
		_jugadores[jugador]->setVelX(vel_x);

	}
	// Desplazar la pala hacia la derecha
	else if((m.pressed(Mando::BOTON_ABAJO) and _control == BOTONES) or (m.cabeceo() > 15.0 and _control == CABECEO))
	{
		// Cambiar el estado del actor
		if(_jugadores[jugador]->estado() == "normal" or _jugadores[jugador]->estado() == "mover")
			_jugadores[jugador]->setEstado("mover");
		else if(_jugadores[jugador]->estado() == "normalg" or _jugadores[jugador]->estado() == "moverg")
			_jugadores[jugador]->setEstado("moverg");
		if(_jugadores[jugador]->estado() == "normalp" or _jugadores[jugador]->estado() == "moverp")
			_jugadores[jugador]->setEstado("moverp");

		// Si hay colision con el escenario, hacer retroceder al actor a la posicion anterior
		if(colision(_jugadores[jugador]))
			_jugadores[jugador]->mover(
				_jugadores[jugador]->x() - _jugadores[jugador]->velX(), _jugadores[jugador]->y());

		// Orientar el vector de movimiento hacia la derecha
		s16 vel_x = _jugadores[jugador]->velX();
		if(vel_x < 0)
			vel_x *= -1;
		_jugadores[jugador]->setVelX(vel_x);

	}
	// No hay desplazamiento de la pala
	else
	{
		if(_jugadores[jugador]->estado() == "mover")
			_jugadores[jugador]->setEstado("normal");
		else if(_jugadores[jugador]->estado() == "moverg")
			_jugadores[jugador]->setEstado("normalg");
		if(_jugadores[jugador]->estado() == "moverp")
			_jugadores[jugador]->setEstado("normalp");
	}

	// Actualizar el actor
	_jugadores[jugador]->actualizar();
}

void Escenario::actualizarNpj(void)
{
	for(Actores::iterator i = _actores.begin() ; i != _actores.end() ; ++i)
	{
		// Si el actor es la bola
		if((*i)->tipoActor() == "bola")
		{
			// Si hay colision con algun elemento del escenario
			if(colision(_bola))
			{
				// Colision horizontal derecha
				if(_bola->x() + _bola->ancho() >= _x1)
					_bola->setDireccion(_bola->direccion() + (M_PI/2.0 - _bola->direccion()) * 2.0 + M_PI);

				// Colision horizontal izquierda
				else if(_bola->x() <= _x0)
					_bola->setDireccion(_bola->direccion() + (3*M_PI/2.0 - _bola->direccion()) * 2.0 + M_PI);

				// Colision vertical arriba
				else if(_bola->y() <= _y0)
					_bola->setDireccion(_bola->direccion() + (M_PI*2.0 - _bola->direccion()) * 2.0 + M_PI);

				// Colision vertical abajo
				else if(_bola->y() + _bola->alto() >= _y1)
				{
					delete _bola;
					_bola = NULL;
					(*i) = NULL;
					const_cast<Arkanoid*>(_arkanoid)->vidas()--;
				}

				// Colision en diagonal
				else
					_bola->setDireccion(_bola->direccion() + M_PI);

				if(_bola != NULL)
				{
					// Asegurar que el ángulo de la bola no es mayor que 2PI o menor que cero
					while(_bola->direccion() > M_PI*2.0)
						_bola->setDireccion(_bola->direccion() - M_PI*2.0);
					while(_bola->direccion() < 0.0)
						_bola->setDireccion(_bola->direccion() + M_PI*2.0);
					if(_bola->direccion() == M_PI/2.0)
						_bola->setDireccion(_bola->direccion() + M_PI/10);
				}
			}
			// Colision con la pala
			else if(_bola != NULL and _bola->colision(*_pala))
			{
				// Calcular la distancia entre los centros de la pala y de la bola en el momento de la colision
				// y calcular el nuevo angulo de la bola en base a este centro: cuanto mas lejos, mayor angulo
				f32 centroBola = _bola->x() + _bola->ancho() / 2;
				f32 centroPala = _pala->x() + _pala->ancho() / 2;
				f32 lugarColision = (f32)(centroPala - centroBola) / (_pala->ancho() / 2);
				f32 anguloBola = lugarColision * M_PI/6.0 + M_PI;
				_bola->setDireccion(_bola->direccion() + (anguloBola - _bola->direccion()) * 2.0 + M_PI);
				while(_bola->direccion() > M_PI*2.0)
					_bola->setDireccion(_bola->direccion() - M_PI*2.0);

				// Asegurar que la dirección de la bola no es demasiado horizontal
				if(_bola->direccion() < (M_PI - (M_PI/2.0 - M_PI/6.0)))
					_bola->setDireccion(M_PI - (M_PI/2.0 - M_PI/6.0));
				else if(_bola->direccion() > (M_PI + (M_PI/2.0 - M_PI/6.0)))
					_bola->setDireccion(M_PI + (M_PI/2.0 - M_PI/6.0));
			}
		}
		// Si el actor es un ladrillo
		if(_bola != NULL and (*i)->tipoActor() == "ladrillo")
		{
			if((*i)->colision(*_bola))
			{
				// Calcular los centros de ambos objetos
				f32 ladrillo_x = (*i)->x() + (*i)->ancho() / 2;
				f32 ladrillo_y = (*i)->y() + (*i)->alto() / 2;
				f32 bola_x = _bola->x() + _bola->ancho() / 2;
				f32 bola_y = _bola->y() + _bola->alto() / 2;

				// La bola esta en la vertical del ladrillo
				if(bola_x >= (*i)->x() and bola_x <= (*i)->x() + (*i)->ancho())
				{
					// La bola esta debajo del ladrillo
					if(bola_y > ladrillo_y)
						_bola->setDireccion(_bola->direccion() + (M_PI*2.0 - _bola->direccion()) * 2.0 + M_PI);
					// La bola esta arriba del ladrillo
					else
						_bola->setDireccion(_bola->direccion() + (M_PI - _bola->direccion()) * 2.0 + M_PI);
				}
				// La bola no esta en la vertical del ladrillo, esta en la horizontal
				else if(bola_y >= (*i)->y() and bola_y <= (*i)->y() + (*i)->alto())
				{
					// La bola esta a la derecha del ladrillo
					if(bola_x > ladrillo_x)
						_bola->setDireccion(_bola->direccion() + (3*M_PI/2.0 - _bola->direccion()) * 2.0 + M_PI);
					// La bola esta a la izquierda del ladrillo
					else
						_bola->setDireccion(_bola->direccion() + (M_PI/2.0 - _bola->direccion()) * 2.0 + M_PI);
				}
				// Colision en diagonal
				else
					_bola->setDireccion(_bola->direccion() + M_PI);

				// Aumentar la velocidad si se elimina un ladrillo, pero no superar la velocidad maxima
				if(_bola->velocidad() < _bola->maxVelocidad())
					_bola->setVelocidad(_bola->velocidad() + 0.5);

				// Eliminar el ladrillo
				generarItem(static_cast<Ladrillo*>(*i));
				delete (*i);
				*i = NULL;
				_num_ladrillos--;
				const_cast<Arkanoid*>(_arkanoid)->puntos() += (100 * _multiplicador);
			}
		}
		// Si el actor es un item
		if(*i != NULL and (*i)->tipoActor() == "item")
		{
			// Si colisiona con la pala
			if((*i)->colision(*_pala))
			{
				// Vida extra
				if((*i)->estado() == "azul")
					const_cast<Arkanoid*>(_arkanoid)->vidas()++;
				// Pala grande durante 15 segundos
				else if((*i)->estado() == "amarillo")
				{
					_item_activo = true;
					_crono = gettick()/(u32)TB_TIMER_CLOCK;
					if(_pala->estado() == "normal" or _pala->estado() == "normalp")
						_pala->setEstado("normalg");
					if(_pala->estado() == "mover" or _pala->estado() == "moverp")
						_pala->setEstado("moverg");
					// Corregir la posicion horizontal de la pala si fuera necesario
					s32 desplazamiento = _pala->x() + _pala->ancho() - _x1;
					if(desplazamiento >= 0)
						_pala->mover(_pala->x() - desplazamiento - 1 , _pala->y());
				}
				// Multiplicador de puntos x2 durante 15 segundos
				else if((*i)->estado() == "verde")
				{
					_item_activo = true;
					_crono = gettick()/(u32)TB_TIMER_CLOCK;
					_multiplicador = 2;
				}
				// Pala pequeña durante 15 segundos
				else if((*i)->estado() == "rojo")
				{
					_item_activo = true;
					_crono = gettick()/(u32)TB_TIMER_CLOCK;
					if(_pala->estado() == "normal" or _pala->estado() == "normalg")
						_pala->setEstado("normalp");
					if(_pala->estado() == "mover" or _pala->estado() == "moverg")
						_pala->setEstado("moverp");
				}

				// Despues de haber cogido el item, este desaparece
				delete (*i);
				*i = NULL;
			}
			// Si llega al fondo de la pantalla, desaparece el item
			else if((*i)->y() + (*i)->alto() >= _y1)
			{
				delete (*i);
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
}

void Escenario::actualizarEscenario(void)
{
	// Comprobaciones de fin de nivel
	if(_arkanoid->vidas() == 0 or _num_ladrillos == 0)
		_fin_nivel = true;
	// Comprobaciones de fin de efecto de item
	if((gettick()/(u32)TB_TIMER_CLOCK - _crono) > 12000 and _item_activo == true)
	{
		_item_activo = false;
		_crono = 0;
		// Multiplicador de puntos a 1
		_multiplicador = 1;
		// Tamaño de la pala normal
		if(_pala->estado() == "normalg" or _pala->estado() == "normalp")
			_pala->setEstado("normal");
		if(_pala->estado() == "moverg" or _pala->estado() == "moverp")
			_pala->setEstado("mover");
		// Corregir la posicion horizontal de la pala si fuera necesario
		s32 desplazamiento = _pala->x() + _pala->ancho() - _x1;
		if(desplazamiento >= 0)
			_pala->mover(_pala->x() - desplazamiento - 1 , _pala->y());
	}
}

bool Escenario::finNivel(void) const
{
	return _fin_nivel;
}

void Escenario::cambiarControl(void)
{
	// Cambiar el sistema de control si el usuario asi lo quiere
	if(_control == BOTONES)
		_control = CABECEO;
	else if(_control == CABECEO)
		_control = BOTONES;
}

void Escenario::generarItem(const Ladrillo* lad)
{
	// Un 10% de que salga un item
	if((rand() % 101) > 90)
	{
		Item* i = new Item(_xml_item, this);
		i->mover(lad->x() + lad->ancho()/2, lad->y());
		_actores.push_back(i);
	}
}

void Escenario::cargarActores(void)
{
	for(Temporal::iterator i = _temporal.begin() ; i != _temporal.end() ; ++i)
	{
		if(i->tipo_actor == "pala") {
			_pala = new Pala(i->xml, this);
			_pala->mover(i->x, i->y);
			_jugadores.insert(std::make_pair(i->jugador, _pala));
		} else if(i->tipo_actor.find("ladrillo-") != string::npos) {
			Ladrillo* l = new Ladrillo(i->xml, this, i->tipo_actor.substr(9));
			l->mover(i->x, i->y);
			_actores.push_back(l);
			_num_ladrillos++;
		}
	}
	_temporal.clear();
}

