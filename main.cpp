#include "pkg.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Insufficient args! Need <src pkg> and <dest dir>!\n";
        return 1;
    }

    std::string src = argv[1];
    std::string dest = argv[2];

    std::string reason;

    PKG p;
    p.Open(src, reason);

    std::string dest_title = dest + "/";
    dest_title += p.GetTitleID();

    bool result = p.Extract(src, dest_title, reason);

    int nfiles = p.GetNumberOfFiles();
    for (int i = 0; i < nfiles; i++)
        p.ExtractFiles(i);

    if (!reason.empty())
        std::cout << reason << "\n";

    return result ? 0 : 1;
}
