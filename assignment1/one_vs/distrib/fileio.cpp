#include "fileio.h"
#include <sys/stat.h>
#include <string>
#include <fstream>

using namespace std;

bool file_exist(const char* fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}