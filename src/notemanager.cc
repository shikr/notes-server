#include "notemanager.hh"
#include <cstring>
#include <filesystem>
#include <grp.h>
#include <pwd.h>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace notes;
namespace fs = std::filesystem;

string NoteManager::get_user() {
  struct passwd *pw = getpwuid(getuid());
  if (pw == NULL) {
    throw runtime_error("Failed to get user information");
  }

  return pw->pw_name;
}

Note NoteManager::get_note(size_t id) {
  auto notes = get_notes();
  if (id == 0 || id > notes.size()) {
    throw out_of_range("Invalid note ID");
  }

  return notes[id - 1];
}

vector<Note> NoteManager::get_notes() {
  vector<Note> notes;
  fs::path base(NOTES_DIR);
  string user = get_user();
  auto user_dir = base / user;

  if (fs::exists(user_dir)) {
    for (const auto &entry : fs::directory_iterator(user_dir)) {
      if (entry.is_regular_file()) {
        notes.push_back(entry.path());
      }
    }
  }

  for (const auto &group_name : get_groups()) {
    auto group_dir = base / group_name;
    if (!fs::exists(group_dir))
      continue;

    for (const auto &entry : fs::directory_iterator(group_dir)) {
      if (entry.is_regular_file()) {
        notes.push_back(entry.path());
      }
    }
  }

  std::sort(notes.begin(), notes.end(), [](Note a, Note b) {
    return a.get_timestamp() < b.get_timestamp();
  });

  return notes;
}

vector<string> NoteManager::get_groups() {
  struct passwd *pw = getpwuid(getuid());
  if (pw == NULL) {
    throw runtime_error("Failed to get user information");
  }

  vector<string> groups;

  int ngroups = 0;
  getgrouplist(pw->pw_name, pw->pw_gid, NULL, &ngroups);
  vector<gid_t> group_ids(ngroups);
  getgrouplist(pw->pw_name, pw->pw_gid, group_ids.data(), &ngroups);

  for (gid_t gid : group_ids) {
    struct group *grp = getgrgid(gid);
    if (grp != NULL && strcmp(grp->gr_name, pw->pw_name) != 0) {
      groups.push_back(grp->gr_name);
    }
  }

  return groups;
}

vector<string> NoteManager::parse_args(const string &args) noexcept {
  vector<string> tokens;
  size_t start = 0, end = 0;

  while ((end = args.find(' ', start)) != string::npos) {
    tokens.push_back(args.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(args.substr(start));

  return tokens;
}
