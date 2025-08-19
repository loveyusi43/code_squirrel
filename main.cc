#include <chrono>
#include <filesystem>
#include <iostream>
#include <set>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

std::filesystem::path GetHomeDirectory() {
#ifdef _WIN32
    PWSTR raw_path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &raw_path))) {
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> path_guard(raw_path, CoTaskMemFree);
        return std::filesystem::path(path_guard.get());
    }
    return {};
#else
    if (passwd* pw = getpwuid(getuid())) {
        return std::filesystem::path(pw->pw_dir);
    }
    return {};
#endif
}

int main() {
    // 要存档的文件类型
    std::set<std::string> extensions = {
        ".cpp", ".cc", ".cxx", ".c++", ".C", ".h", ".hh", ".hxx", ".h++", "hpp",
        ".cu", ".hip", ".ixx", ".cppm", ".cxxm", ".c++m",
        ".txt",
        "py"};

    std::chrono::time_point now = std::chrono::system_clock::now();  // 获取月份
    std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> time =
        std::chrono::floor<std::chrono::seconds>(now);
    std::string month_abbr = std::format("{:%B}", time);
    std::transform(month_abbr.begin(), month_abbr.end(), month_abbr.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::filesystem::path root = ".";  // 递归搜素起点
    // 移动到
    std::filesystem::path target_dir = GetHomeDirectory() / "repositories" / "bug_forge" / month_abbr;

    std::filesystem::create_directories(target_dir);

    int count = 0;  // 记录移动的文件数量

    std::chrono::time_point start = std::chrono::steady_clock::now();  // 记录程序运行时间

    for (std::filesystem::recursive_directory_iterator it = std::filesystem::recursive_directory_iterator(root);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (it->is_directory() && it->path().filename() == "build") {
            it.disable_recursion_pending();
            continue;
            ;
        }
        if (!it->is_regular_file()) {
            continue;
        }

        std::string ext = it->path().extension().string();
        if (extensions.find(ext) != extensions.end()) {
            std::chrono::time_point now = std::chrono::system_clock::now();
            std::int64_t base_time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            std::string stem = it->path().stem().string();
            std::string new_name = std::to_string(base_time) + '_' + stem + ext;

            std::filesystem::path dest_path = target_dir / new_name;
            std::filesystem::rename(it->path(), dest_path);
            ++count;
            std::cout << new_name << std::endl;
        }
    }
    auto end = std::chrono::steady_clock::now();
    ino64_t dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << std::format("本次共存档: {} 个文件, 耗时: {}ms", count, dt) << std::endl;
}