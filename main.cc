// 版本3

#include <chrono>
#include <filesystem>
#include <iostream>
#include <set>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif

// 跨平台获取家目录
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
        ".cpp", ".cc", ".cxx", ".c++", ".C", ".h", ".hh", ".hxx", ".h++", ".hpp",
        ".cu", ".hip", ".ixx", ".cppm", ".cxxm", ".c++m",
        ".txt",
        ".py"
    };

    // 根据当前月份自动创建文件夹，以当前月份命名，小写
    std::chrono::time_point start = std::chrono::system_clock::now();  // 获取月份
    std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> time =
        std::chrono::floor<std::chrono::seconds>(start);
    std::string month_abbr = std::format("{:%B}", time);
    std::transform(month_abbr.begin(), month_abbr.end(), month_abbr.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); }); // 字符串转小写
    std::filesystem::path target_dir = GetHomeDirectory() / "repositories" / "bug_forge" / month_abbr;
    std::cout << "目标路径：" << target_dir << std::endl;
    std::filesystem::create_directories(target_dir); // 创建存档文件，若不存在

    // 用于重命名的时间戳
    std::int64_t base_time = std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
    int count = 0;  // 记录移动的文件数量

    std::filesystem::path root = ".";  // 递归搜素起点
    for (std::filesystem::recursive_directory_iterator it = std::filesystem::recursive_directory_iterator(root, std::filesystem::directory_options::skip_permission_denied);
        it != std::filesystem::recursive_directory_iterator(); ++it) { // 开始循环
        if (it->is_directory() && it->path().filename() == "build") {
            it.disable_recursion_pending();
            continue;
            ;
        }
        //std::filesystem::directory_entry
        if (!it->is_regular_file()) {
            continue;
        }

        // 开始处理文件
        std::string extension_str = it->path().extension().string();
        if (extensions.find(extension_str) != extensions.end()) { // 找到符合后缀需求的文件
            std::string stem = it->path().stem().string();
            // 根据时间戳和原文件名创建新文件名
            std::string new_name = std::to_string(base_time) + '_' + stem + extension_str;

            std::filesystem::path dest_path = target_dir / new_name;
            std::filesystem::rename(it->path(), dest_path);
            ++count;
            std::cout << new_name << std::endl;
        }
    }
    std::chrono::time_point end = std::chrono::system_clock::now(); // 程序结束实际
    std::int64_t dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << std::format("本次共存档: {} 个文件, 耗时: {}ms", count, dt) << std::endl;
}