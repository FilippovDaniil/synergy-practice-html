#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "gen/css.hpp"
#include "gen/html.hpp"
#include "gen/organization.hpp"

namespace fs = std::filesystem;

namespace {

// Writes bytes as-is (binary mode: UTF-8 without BOM, no CRLF translation).
void writeFile(const fs::path& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("cannot write file '" + path.string() + "'");
    }
    out.write(content.data(), static_cast<std::streamsize>(content.size()));
    if (!out) {
        throw std::runtime_error("write failed for '" + path.string() + "'");
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 3) {
        std::cerr << "usage: html_generator [input.json] [output_dir]\n"
                  << "  defaults: data/organization.json site\n";
        return 2;
    }
    const std::string input = argc > 1 ? argv[1] : "data/organization.json";
    const fs::path outDir = argc > 2 ? argv[2] : "site";

    gen::Organization org;
    try {
        org = gen::loadOrganization(input);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }

    try {
        fs::create_directories(outDir);
        const fs::path index = outDir / "index.html";
        writeFile(index, gen::renderHtml(org));
        writeFile(outDir / "styles.css", gen::renderCss(org));
        std::cout << fs::absolute(index).lexically_normal().string() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 3;
    }
    return 0;
}
