#include "pkg.h"
#include <iostream>
#include <omp.h>

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

    std::cout << "Title ID: " << p.GetTitleID() << "\n";
    std::cout << "Number of files: " << p.GetNumberOfFiles() << "\n";

    #ifdef _OPENMP
        omp_set_num_threads(std::min(4, omp_get_max_threads()));
    #else
        std::cout << "No OpenMP support configured, running single-threaded\n";
    #endif
    #pragma omp parallel
    {
        #ifdef _OPENMP
            int thrid = omp_get_thread_num();
            int num_thr = omp_get_num_threads();
            std::cout << "Thread: " << thrid+1 << "/" << num_thr << " active\n";
        #endif
        #pragma omp for
        for (int i = 0; i < nfiles; i++) {
            p.ExtractFiles(i);
            std::cout << "Extracted file " << i+1 << "/" << nfiles << "\n";
        }
    }

    if (!reason.empty())
        std::cout << reason << "\n";

    return result ? 0 : 1;
}
