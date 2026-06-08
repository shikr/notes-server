#include "note.hh"
#include "notemanager.hh"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace notes;
namespace fs = filesystem;

int main(void) {
  if (fs::status(NOTES_DIR).type() != fs::file_type::directory) {
    cerr << "El directorio de notas no existe." << endl;
    cerr << "Asegúrate de crear el directorio adecuadamente para continuar."
         << endl;
    return 1;
  }

  cout << "¡Bienvenido a tus notas!" << endl;
  cout << "Ingresa 'help' para ver los comandos disponibles." << endl;

  while (1) {
    string args;
    cout << NoteManager::get_user() << " >> ";
    getline(cin, args);
    auto tokens = NoteManager::parse_args(args);

    if (tokens[0] == "add") {
      string title;
      cout << "Ingresa el título: ";
      getline(cin, title);

      auto note = Note::create(title, tokens.size() > 1 ? tokens[1] : "");
      note.edit();
    } else if (tokens[0] == "ls") {
      auto notes = NoteManager::get_notes();

      if (notes.empty()) {
        cout << "No tienes notas disponibles." << endl;
      } else {
        cout << "No. | Título" << endl;
        for (size_t i = 0; i < notes.size(); ++i) {
          cout << i + 1 << ". " << notes[i].get_title() << endl;
        }
      }
    } else if (tokens[0] == "cat") {
      try {
        if (tokens.size() < 2)
          throw invalid_argument(
              "Necesitas especificar el número de la nota a mostrar.");

        auto note = NoteManager::get_note(stoul(tokens[1]));
        cout << note.get_title() << endl;
        cout << note.get_content() << endl;
      } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
      }
    } else if (tokens[0] == "edit") {
      try {
        if (tokens.size() < 2)
          throw invalid_argument(
              "Necesitas especificar el numero de la nota a editar");
        auto note = NoteManager::get_note(stoul(tokens[1]));
        note.edit();
      } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
      }
    } else if (tokens[0] == "rm") {
      try {
        if (tokens.size() < 2)
          throw invalid_argument(
              "Necesitas especificar el numero de la nota a eliminar");
        auto note = NoteManager::get_note(stoul(tokens[1]));
        note.remove();
      } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
      }
    } else if (tokens[0] == "groups") {
      auto groups = NoteManager::get_groups();
      if (groups.empty()) {
        cout << "No perteneces a ningun grupo" << endl;
      } else {
        for (size_t i = 0; i < groups.size(); i++) {
          if (i != 0)
            cout << " ";
          cout << groups[i];
        }
        cout << endl;
      }
    } else if (tokens[0] == "chgrp") {
      try {
        if (tokens.size() < 3)
          throw invalid_argument("Uso: chgrp <numero> <grupo>");
        auto note = NoteManager::get_note(stoul(tokens[1]));
        note.set_owner(tokens[2]);
        cout << "Propietario cambiado al grupo " << tokens[2] << endl;
      } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
      }
    } else if (tokens[0] == "groupnotes") {
      try {
        if (tokens.size() < 2)
          throw invalid_argument("Necesita especificar el nombre del grupo");
        string group_name = tokens[1];
        auto groups = NoteManager::get_groups();

        if (find(groups.begin(), groups.end(), group_name) == groups.end())
          throw runtime_error("No perteneces al grupo " + group_name);

        auto notes = NoteManager::get_notes();
        vector<pair<size_t, Note>> group_notes;

        for (size_t i = 0; i < notes.size(); i++) {
          if (notes[i].get_owner() == group_name) {
            group_notes.push_back({i, notes[i]});
          }
        }

        if (group_notes.empty()) {
          cout << "El grupo " << group_name << " no tiene notas" << endl;
        } else {
          cout << "No. | Titulo" << endl;
          for (const auto &[i, note] : group_notes) {
            cout << i + 1 << ". " << note.get_title() << endl;
          }
        }
      } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
      }
    } else if (tokens[0] == "info") {
      try {
        if (tokens.size() < 2)
          throw invalid_argument("Necesitas especificar el numero de la nota");

        auto note = NoteManager::get_note(stoul(tokens[1]));

        cout << "Titulo:  " << note.get_title() << endl;
        cout << "Propietario: " << note.get_owner() << endl;
        cout << "Timestamp: " << note.get_timestamp() << endl;
        cout << "Ruta: " << note.get_path() << endl;
      } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
      }
    } else if (tokens[0] == "help") {
      cout << "Comandos disponibles:" << endl;
      cout << "  add [grupo] - Agregar una nueva nota" << endl;
      cout << "  ls - Listar todas las notas" << endl;
      cout << "  cat <numero> - Ver el contenido de una nota" << endl;
      cout << "  edit <numero> - Editar una nota existente" << endl;
      cout << "  rm <numero> - Eliminar una nota existente" << endl;
      cout << "  groups - Listar los grupos a los que perteneces" << endl;
      cout << "  chgrp <numero> <grupo> - Cambiar el grupo propietario de una "
              "nota"
           << endl;
      cout << "  groupnotes <grupo> - Listar las notas de un grupo específico"
           << endl;
      cout << "  info <numero> - Mostrar información detallada de una nota"
           << endl;
      cout << "  clear - Limpiar la pantalla" << endl;
      cout << "  help - Mostrar esta ayuda" << endl;
      cout << "  exit - Salir del programa" << endl;
    } else if (tokens[0] == "clear") {
      cout << "\033[H\033[J";
    } else if (tokens[0] == "exit") {
      break;
    } else {
      cout << "Comando desconocido. Por favor, inténtelo de nuevo." << endl;
    }
  }

  return 0;
}
