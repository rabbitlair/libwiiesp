//
// Licencia GPLv3
//
// Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
//
// libWiiEsp es software libre: usted puede redistribuirlo y/o modificarlo bajo los términos de la 
// Licencia Pública General GNU publicada por la Fundación para el Software Libre, ya sea la versión 3 
// de la Licencia, o (a su elección) cualquier versión posterior.
//
// libWiiEsp se distribuye con la esperanza de que sea útil, pero SIN GARANTÍA ALGUNA; ni siquiera 
// la garantía implícita MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. Consulte los detalles de
// la Licencia Pública General GNU para obtener una información más detallada.
//
// Debería haber recibido una copia de la Licencia Pública General GNU junto a libWiiEsp. En caso 
// contrario, consulte <http://www.gnu.org/licenses/>.
//

#ifndef _EXCEPCION_H_
#define _EXCEPCION_H_

	#include <stdexcept>
	#include <string>

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción básica
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class Excepcion: public std::exception
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			Excepcion(const std::string& e): _error(e) { };

			/**
			 * Método observador que devuelve una descripción del error que ha provocado la excepción
			 * @return Descripción del error que ha provocado la excepción
			 */
			const char* what() const throw() { return _error.c_str(); };

			/**
			 * Destructor de la clase de excepción
			 */
			virtual ~Excepcion(void) throw() { };

		private:
			std::string _error;
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción para errores relacionados con archivos
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class ArchivoEx: public Excepcion
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			ArchivoEx(const std::string& e): Excepcion(e) { };

			/**
			 * Destructor de la clase de excepción
			 */
			~ArchivoEx(void) throw() { };
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción para errores relacionados con códigos identificadores
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class CodigoEx: public Excepcion
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			CodigoEx(const std::string& e): Excepcion(e) { };

			/**
			 * Destructor de la clase de excepción
			 */
			~CodigoEx(void) throw() { };
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción para errores relacionados con una imagen o textura
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class ImagenEx: public Excepcion
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			ImagenEx(const std::string& e): Excepcion(e) { };

			/**
			 * Destructor de la clase de excepción
			 */
			~ImagenEx(void) throw() { };
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción para errores relacionados con el Nunchuck de un mando
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class NunchuckEx: public Excepcion
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			NunchuckEx(const std::string& e): Excepcion(e) { };

			/**
			 * Destructor de la clase de excepción
			 */
			~NunchuckEx(void) throw() { };
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción para errores relacionados con la tarjeta SD
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class TarjetaEx: public Excepcion
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			TarjetaEx(const std::string& e): Excepcion(e) { };

			/**
			 * Destructor de la clase de excepción
			 */
			~TarjetaEx(void) throw() { };
	};

	/**
	 * @author Ezequiel Vázquez de la Calle
	 * @version 0.9.2
	 * @brief Clase de excepción para errores relacionados con un árbol XML
	 *
	 * @details Cuando se captura, puede informar de qué error se ha producido mediante el método what().
	 *
	 */
	class XmlEx: public Excepcion
	{
		public:

			/**
			 * Constructor de la clase de excepción
			 * @param e Cadena de caracteres que indica el error ocurrido
			 */
			XmlEx(const std::string& e): Excepcion(e) { };

			/**
			 * Destructor de la clase de excepción
			 */
			~XmlEx(void) throw() { };
	};

#endif

