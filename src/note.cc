#include "note.hh"
#include "notemanager.hh"
#include <filesystem>
#include <fstream>
#include <grp.h>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
using namespace notes;
namespace fs = std::filesystem;

void ensure_directory_exists(const fs::path &p) {
  if (!fs::exists(p)) {
    try {
      fs::create_directory(p);
    } catch (const exception &e) {
      throw runtime_error("Failed to create directory: " + string(e.what()));
    }
  }
}

Note::Note(const fs::path &path) noexcept {
  id = path.stem().string();
  owner = path.parent_path().filename().string();
}

string Note::get_title() const {
  ifstream note_file(get_path());
  string title;
  getline(note_file, title);
  return title;
}

string Note::get_content() const {
  ifstream note_file(get_path());
  // Ignorar las primeras 2 líneas (título y línea separadora)
  for (int i = 0; i < 2; ++i)
    note_file.ignore(numeric_limits<streamsize>::max(), '\n');

  string content((istreambuf_iterator<char>(note_file)),
                 istreambuf_iterator<char>());
  return content;
}

string Note::get_owner() const { return owner; }

string Note::get_timestamp() const { return id; }

fs::path Note::get_path() const {
  return fs::path(NOTES_DIR) / owner / (id + ".txt");
}

void Note::edit() const {
  char *env_editor = getenv("EDITOR");
  string editor = env_editor ? env_editor : "nano";

  pid_t pid = fork();

  if (pid == -1) {
    throw runtime_error("Failed to fork process");
  } else if (pid == 0) {
    execlp(editor.c_str(), editor.c_str(), get_path().c_str(), NULL);
    exit(1); // If exec fails
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (status != 0) {
      throw runtime_error("Editor exited with an error");
    }
  }
}

void Note::remove() const { fs::remove(get_path()); }

void Note::set_owner(const string &group_name) {
  struct group *grp = getgrnam(group_name.c_str());
  if (grp == NULL) {
    throw runtime_error("Group not found");
  }

  if (chown(get_path().c_str(), -1, grp->gr_gid) != 0) {
    throw runtime_error("Failed to change group ownership");
  }

  fs::path new_path = fs::path(NOTES_DIR) / group_name / (id + ".txt");
  ensure_directory_exists(new_path.parent_path());
  fs::rename(get_path(), new_path);

  owner = group_name;
}

Note Note::create(const string &title, const string &group_name) {
  auto now = chrono::system_clock::now();
  auto timestamp =
      chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();

  string owner;

  if (group_name.empty())
    owner = NoteManager::get_user();
  else
    owner = group_name;

  Note note(owner, to_string(timestamp));

  ensure_directory_exists(note.get_path().parent_path());

  ofstream note_file(note.get_path());

  note_file << title << endl;
  note_file << "--- Escribe tu nota aquí... ---" << endl;
  note_file.close();

  if (!group_name.empty())
    note.set_owner(group_name);

  return note;
}
