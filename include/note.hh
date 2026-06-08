#pragma once

#include <filesystem>
#include <string>

#define NOTES_DIR "/var/lib/notes/"

namespace notes {

class Note {
public:
  Note(const std::filesystem::path &path) noexcept;
  Note(const std::string &owner, const std::string &id) noexcept
      : owner(owner), id(id) {}

  /**
   * Obtiene el título de la nota leyendo la primera línea del archivo.
   */
  std::string get_title() const;
  /**
   * Obtiene el contenido de la nota leyendo el archivo completo, ignorando las
   * primeras dos líneas (título y línea separadora).
   */
  std::string get_content() const;
  /**
   * Obtiene el nombre del propietario de la nota, que puede ser un usuario o un
   * grupo, dependiendo de cómo se creó la nota.
   */
  std::string get_owner() const;
  /**
   * Obtiene el timestamp de creación de la nota.
   */
  std::string get_timestamp() const;
  /**
   * Obtiene la ruta completa del archivo de la nota, construida a partir del
   * directorio base, el propietario y el ID de la nota.
   */
  std::filesystem::path get_path() const;
  /**
   * Verifica si el archivo de la nota existe en el sistema de archivos.
   */
  bool exists() const;

  /**
   * Abre el editor de texto predeterminado para editar el contenido de la nota.
   */
  void edit() const;
  /**
   * Elimina el archivo de la nota del sistema de archivos.
   */
  void remove() const;
  /**
   * Cambia el propietario del archivo de la nota al grupo especificado.
   */
  void set_owner(const std::string &group_name);

  /**
   * Crea una nueva nota con el título dado y opcionalmente asignada a un grupo.
   * Crea un archivo con un nombre único basado y coloca el encabezado de la
   * nota.
   */
  static Note create(const std::string &title, const std::string &group_name);

private:
  std::string owner;
  std::string id;
};

} // namespace notes
