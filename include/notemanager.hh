#pragma once

#include "note.hh"
#include <string>
#include <vector>

namespace notes {

class NoteManager {
public:
  /**
   * Obtiene el nombre de usuario.
   */
  static std::string get_user();
  /**
   * Obtiene una nota específica por su número en la lista.
   */
  static Note get_note(size_t id);
  /**
   * Obtiene todas las notas disponibles para el usuario actual, incluyendo las
   * notas personales y las notas de los grupos a los que pertenece.
   */
  static std::vector<Note> get_notes();
  /**
   * Obtiene los nombres de los grupos a los que pertenece el usuario actual.
   */
  static std::vector<std::string> get_groups();
  /**
   * Extrae los comandos y argumentos de una cadena de entrada, dividiéndolos
   * por espacios.
   */
  static std::vector<std::string> parse_args(const std::string &args) noexcept;
};

} // namespace notes
