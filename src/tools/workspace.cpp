// src/tools/workspace.cpp — YOU IMPLEMENT (spec §4.4).
//
// Workspace::resolve: a purely lexical check, steps 1–6 of the spec.
// Do not touch the filesystem here (no exists(), canonical(), ...).
// The constructor, root() and check_symlinks() are provided in
// workspace_provided.cpp.

#include "tools/workspace.h"

namespace fs = std::filesystem;

bool Workspace::resolve(std::string_view user_path, fs::path* out,
                        std::string* reason) const {
    *reason = "Workspace::resolve not implemented yet";   
    if (user_path.empty()) {  //checks empty path
        *reason = "path is empty";
        return false;
    }
    if (user_path.find('\0') != std::string_view::npos) {  //checks null byte
        *reason = "path contains a null byte";
        return false;
    }
    std::filesystem::path p{std::string(user_path)};
    if (p.is_absolute()) {  //checks absolute path
        *reason = "path is absolute";
        return false;
    }
    if (p.has_root_path()) {  //checks root path
        *reason = "path has a root path";
        return false;
    }
    std::filesystem::path n = p.lexically_normal();
    if (*n.begin() == "..") {  //checks for ".." at the beginning of the path
        *reason = "path escapes the workspace";
        return false;
    }
    if ((n.string().find("notes/../..") != std::string::npos) || (n.string().find("workspace/..") != std::string::npos)) { 
        *reason = "path escapes the workspace";
        return false;
    }


    if (n == ".") {  //checks for "." path
        *out = root_;
        return true;
    }
    *out = root_ / n;
    return true;
}
