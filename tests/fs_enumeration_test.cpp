#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;
using Entries = std::set<fs::path>;
Entries collect(const fs::path& root, int filter, bool cached, bool recursive) {
    Entries result;
    auto visit = [&](const fs::directory_entry& e) {
        bool file = false, directory = false;
        if (filter & 1) file = cached ? e.is_regular_file() : e.status().type() == fs::file_type::regular;
        if (!file && (filter & 2)) directory = cached ? e.is_directory() : e.status().type() == fs::file_type::directory;
        if (file || directory) result.insert(e.path().lexically_relative(root));
    };
    if (recursive) for (auto const& e : fs::recursive_directory_iterator(root)) visit(e);
    else for (auto const& e : fs::directory_iterator(root)) visit(e);
    return result;
}
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    fs::path root = argv[1];
    if (fs::exists(root)) throw std::runtime_error("Fixture must be new");
    fs::create_directories(root / "nested directory" / "child");
    for (int i=0;i<512;++i) std::ofstream(root / ("file_" + std::to_string(i))).put('x');
    std::ofstream(root / "nested directory" / "child" / "leaf").put('y');
    std::error_code ec;
    fs::create_symlink(root / "file_0", root / "file link", ec);
    bool file_link = !ec;
    ec.clear();
    fs::create_directory_symlink(root / "nested directory", root / "directory link", ec);
    bool dir_link = !ec;
    for (bool recursive : {false,true}) for (int filter : {1,2,3}) {
        auto start=std::chrono::steady_clock::now();
        auto previous=collect(root,filter,false,recursive);
        auto middle=std::chrono::steady_clock::now();
        auto current=collect(root,filter,true,recursive);
        auto finish=std::chrono::steady_clock::now();
        if (previous != current) throw std::runtime_error("Enumeration changed");
        std::cout << "recursive=" << recursive << " filter=" << filter << " entries=" << current.size()
                  << " status_us=" << std::chrono::duration_cast<std::chrono::microseconds>(middle-start).count()
                  << " cached_us=" << std::chrono::duration_cast<std::chrono::microseconds>(finish-middle).count() << '\n';
    }
    std::cout << "PASS file_symlink=" << file_link << " directory_symlink=" << dir_link << '\n';
}
