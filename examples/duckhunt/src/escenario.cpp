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
#include "duckhunt.h"
using namespace std;

Escenario::Escenario(const std::string& ruta, const Duckhunt* juego) throw (Excepcion)
: Nivel(ruta), _duckhunt(juego), _crono_patos(0), _intervalo_patos(1000)
{
	_fin_nivel = false;
	cargarActores();
	srand(time(0));

	// Leer propiedades extras del escenario: limites de la zona de juego, y archivos de la bola y los items
	parser->cargar(ruta);
	TiXmlElement* propiedades = parser->buscar("properties", parser->raiz());
	for(TiXmlElement* prop = propiedades->FirstChildElement() ; prop ; prop = prop->NextSiblingElement())		
	{
		string nombre = parser->atributo("name", prop);
		if(nombre == "xml_pato")
			_xml_pato = parser->atributo("value", prop);
		if(nombre == "total_patos")
			_total_patos = parser->atributoU32("value", prop);
	}

	// Situar la ventana de la pantalla en su posición, para que los patos que se generen no aparezcan colisionando
	// con los bordes del escenario (que es la condición de destrucción de un pato que ha escapado)
	moverScroll(96, 96);
}

Escenario::~Escenario(void)
{
}

void Escenario::actualizarPj(const std::string& jugador, const Mando& m)
{
	// TODO Para dar la victoria automáticamente al jugador 1
//	if(m.newPressed(Mando::BOTON_2))
//		const_cast<Duckhunt*>(_duckhunt)->patosJ1() = _total_patos;

	Mira* pMira = static_cast<Mira*>(_jugadores[jugador]);

	// Actualizar el actor
	s16 pos_x = m.punteroX() + _scroll_x;
	s16 pos_y = m.punteroY() + _scroll_y;
	_jugadores[jugador]->mover(pos_x, pos_y);

	// Comprobar si se está recargando (la recarga dura 0,8 segundos)
	u32 tick = (gettick()/(u32)TB_TIMER_CLOCK);
	if(tick - pMira->crono() > 800)
		pMira->setRecargando(false);

	// Indicar al jugador correspondiente que se esta recargando
	if(pMira->recargando())
	{
		if(jugador == _jugadores.begin()->first)
		{
			screen->dibujarRectangulo(110, 440, 255, 440, 255, 465, 110, 465, 2, 0x000000FF);
			galeria->fuente("arial").escribir(lang->texto("RECARGA"), 20, 120, 450, 1, 0xFF0000FF);
		}
		else if(jugador == (++_jugadores.begin())->first)
		{
			screen->dibujarRectangulo(380, 440, 525, 440, 525, 465, 380, 465, 2, 0x000000FF);
			galeria->fuente("arial").escribir(lang->texto("RECARGA"), 20, 390, 450, 1, 0xFF0000FF);
		}
	}

	// Se dispara cuando se pulsa el boton B, si no se esta en plena recarga
	// Comprobar si el disparo le da a algun pato, si es asi, cambiar el estado del pato
	if(m.newPressed(Mando::BOTON_B) and not pMira->recargando())
	{
		galeria->sonido("disparo").play();
		pMira->setCrono(tick);
		pMira->setRecargando(true);
		for(Actores::iterator i = _actores.begin() ; i != _actores.end() ; ++i)
		{
			if(_jugadores[jugador]->colision(**i))
			{
				if((*i)->estado() == "volar")
					(*i)->setEstado("impacto");

				static_cast<Pato*>(*i)->setCrono(gettick()/(u32)TB_TIMER_CLOCK);
				// Sumar los puntos al primer jugador o al segundo
				if(jugador == _jugadores.begin()->first)
					const_cast<Duckhunt*>(_duckhunt)->patosJ1() += 1;
				else if(jugador == (++_jugadores.begin())->first)
					const_cast<Duckhunt*>(_duckhunt)->patosJ2() += 1;
			}
		}
	}
}

void Escenario::actualizarNpj(void)
{
	for(Actores::iterator i = _actores.begin() ; i != _actores.end() ; ++i)
	{
		// Si el pato sale de los limites de la pantalla, eliminarlo
		if(colisionBordes(*i))
		{
			delete *i;
			i = _actores.erase(i);
			i--;
		}
		else
			(*i)->actualizar();
	}
}

void Escenario::actualizarEscenario(void)
{
	// Generar un pato cuando pase el intervalo, y generar un nuevo intervalo hasta el siguiente pato
	u32 tick_ms = (gettick()/(u32)TB_TIMER_CLOCK);

	if(abs((f32)(tick_ms - _crono_patos)) > _intervalo_patos)
	{
		generarPato();
		galeria->sonido("cuack").play();
		_intervalo_patos = (250 + rand() % 1251);
		_crono_patos = (gettick()/(u32)TB_TIMER_CLOCK);
	}

	// Comprobar si alguno de los jugadores llega al objetivo de patos
	if(_duckhunt->patosJ1() == _total_patos or _duckhunt->patosJ2() == _total_patos)
		_fin_nivel = true;
}

bool Escenario::finNivel(void) const
{
	return _fin_nivel;
}

void Escenario::cargarActores(void)
{
	for(Temporal::iterator i = _temporal.begin() ; i != _temporal.end() ; ++i)
	{
		if(i->tipo_actor == "mira_j1") {
			Mira* m1 = new Mira(i->xml, this, "pj1");
			m1->mover(i->x, i->y);
			_jugadores.insert(std::make_pair(i->jugador, m1));
		}
		else if(i->tipo_actor == "mira_j2") {
			Mira* m2 = new Mira(i->xml, this, "pj2");
			m2->mover(i->x, i->y);
			_jugadores.insert(std::make_pair(i->jugador, m2));
		}
	}
	_temporal.clear();
}

void Escenario::generarPato(void)
{
	Pato* p = new Pato(_xml_pato, this);

	u8 lateral = rand() % 2;
	u16 inicio_y = 90 + rand() % 425;
	u16 inicio_x = 0;
	f32 angulo = (-M_PI/6.0 + rand() * (M_PI/3.0) / RAND_MAX);

	// Si lateral es cero, sale por la izquierda
	if(lateral == 0)
	{
		inicio_x = _scroll_x - p->ancho();
		p->setDireccion(M_PI/2.0 + angulo);
	}
	// Si el lateral no es cero, sale por la derecha
	else
	{
		p->invertirDibujo(true);
		inicio_x = 640 + _scroll_x;
		p->setDireccion(3.0*M_PI/2.0 + angulo);
	}
	p->mover(inicio_x, inicio_y);
	_actores.push_back(p);
}

