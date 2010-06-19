#include "../include/Storage.h"

Storage::Storage(const std::string& fname) {
    file.open(fname.c_str(), std::ios::in);
    if (file) {
        while(!file.eof()) {
            std::string line;
            std::getline(file, line);
            if (line.substr(0,2) == "//")  continue; // //comments
            if (line[0] == '#') continue; // #comments
            size_t found = line.find_first_of(" ");
            if (found != std::string::npos) {
                keys[line.substr(0,found)] = line.substr(found+1,std::string::npos);
            }
        }
    }
    file.close();
};

