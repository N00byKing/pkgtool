// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstring>
#include <fstream>
#include <unordered_map>
#include "log.h"
#include "path_util.h"
#include "scope_exit.h"

#ifndef MAX_PATH
// This is the maximum number of UTF-8 code units permissible in all other OSes' file paths
#define MAX_PATH 1024
#endif

namespace Common::FS {

namespace fs = std::filesystem;

static auto UserPaths = [] {

    // Try the portable user directory first.
    auto user_dir = std::filesystem::current_path() / PORTABLE_DIR;
    if (!std::filesystem::exists(user_dir)) {
        // If it doesn't exist, use the standard path for the platform instead.
        // NOTE: On Windows we currently just create the portable directory instead.
        const char* xdg_data_home = getenv("XDG_DATA_HOME");
        if (xdg_data_home != nullptr && strlen(xdg_data_home) > 0) {
            user_dir = std::filesystem::path(xdg_data_home) / "shadPS4";
        } else {
            user_dir = std::filesystem::path(getenv("HOME")) / ".local" / "share" / "shadPS4";
        }
    }

    std::unordered_map<PathType, fs::path> paths;

    const auto create_path = [&](PathType shad_path, const fs::path& new_path) {
        std::filesystem::create_directory(new_path);
        paths.insert_or_assign(shad_path, new_path);
    };

    create_path(PathType::UserDir, user_dir);
    create_path(PathType::LogDir, user_dir / LOG_DIR);
    create_path(PathType::ScreenshotsDir, user_dir / SCREENSHOTS_DIR);
    create_path(PathType::ShaderDir, user_dir / SHADER_DIR);
    create_path(PathType::SaveDataDir, user_dir / SAVEDATA_DIR);
    create_path(PathType::GameDataDir, user_dir / GAMEDATA_DIR);
    create_path(PathType::TempDataDir, user_dir / TEMPDATA_DIR);
    create_path(PathType::SysModuleDir, user_dir / SYSMODULES_DIR);
    create_path(PathType::DownloadDir, user_dir / DOWNLOAD_DIR);
    create_path(PathType::CapturesDir, user_dir / CAPTURES_DIR);
    create_path(PathType::CheatsDir, user_dir / CHEATS_DIR);
    create_path(PathType::PatchesDir, user_dir / PATCHES_DIR);
    create_path(PathType::MetaDataDir, user_dir / METADATA_DIR);
    create_path(PathType::CustomTrophy, user_dir / CUSTOM_TROPHY);

    std::ofstream notice_file(user_dir / CUSTOM_TROPHY / "Notice.txt");
    if (notice_file.is_open()) {
        notice_file
            << "++++++++++++++++++++++++++++++++\n+ Custom Trophy Images / Sound "
               "+\n++++++++++++++++++++++++++++++++\n\nYou can add custom images to the "
               "trophies.\n*We recommend a square resolution image, for example 200x200, 500x500, "
               "the same size as the height and width.\nIn this folder ('user\\custom_trophy'), "
               "add the files with the following "
               "names:\n\nbronze.png\nsilver.png\ngold.png\nplatinum.png\n\nYou can add a custom "
               "sound for trophy notifications.\n*By default, no audio is played unless it is in "
               "this folder and you are using the QT version.\nIn this folder "
               "('user\\custom_trophy'), add the files with the following names:\n\ntrophy.mp3";
        notice_file.close();
    }

    return paths;
}();

bool ValidatePath(const fs::path& path) {
    if (path.empty()) {
        LOG_ERROR(Common_Filesystem, "Input path is empty, path={}", PathToUTF8String(path));
        return false;
    }

#ifdef _WIN32
    if (path.u16string().size() >= MAX_PATH) {
        LOG_ERROR(Common_Filesystem, "Input path is too long, path={}", PathToUTF8String(path));
        return false;
    }
#else
    if (path.u8string().size() >= MAX_PATH) {
        LOG_ERROR(Common_Filesystem, "Input path is too long, path={}", PathToUTF8String(path));
        return false;
    }
#endif

    return true;
}

std::string PathToUTF8String(const std::filesystem::path& path) {
    const auto u8_string = path.u8string();
    return std::string{u8_string.begin(), u8_string.end()};
}

const fs::path& GetUserPath(PathType shad_path) {
    return UserPaths.at(shad_path);
}

std::string GetUserPathString(PathType shad_path) {
    return PathToUTF8String(GetUserPath(shad_path));
}

void SetUserPath(PathType shad_path, const fs::path& new_path) {
    if (!std::filesystem::is_directory(new_path)) {
        LOG_ERROR(Common_Filesystem, "Filesystem object at new_path={} is not a directory",
                  PathToUTF8String(new_path));
        return;
    }

    UserPaths.insert_or_assign(shad_path, new_path);
}

std::optional<fs::path> FindGameByID(const fs::path& dir, const std::string& game_id,
                                     int max_depth) {
    if (max_depth < 0) {
        return std::nullopt;
    }

    // Check if this is the game we're looking for
    if (dir.filename() == game_id && fs::exists(dir / "sce_sys" / "param.sfo")) {
        auto eboot_path = dir / "eboot.bin";
        if (fs::exists(eboot_path)) {
            return eboot_path;
        }
    }

    // Recursively search subdirectories
    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (!entry.is_directory()) {
            continue;
        }
        if (auto found = FindGameByID(entry.path(), game_id, max_depth - 1)) {
            return found;
        }
    }

    return std::nullopt;
}

} // namespace Common::FS
