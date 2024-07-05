#pragma once

#include <filesystem>
#include "paradigm.h"
#include "lodepng.h"

namespace euclid {

class SavePng : public Name {
private:
    std::string save_directory = "/tmp/euclidean-dreams";
    int file_name_iterator = 0;

public:
    SavePng() {
        std::filesystem::create_directory(save_directory);
    }

    void write(Lattice &lattice) {
        vect<unsigned char> image;
        for (int y = lattice.height - 1; y >= 0; y--) {
            for (int x = 0; x < lattice.width; x++) {
                auto color = lattice.get_color(x, y);
                image.push_back(color.red);
                image.push_back(color.green);
                image.push_back(color.blue);
                image.push_back(255);
            }
        }
        std::string file_name = fmt::format("{}/{}.png", save_directory, file_name_iterator);
        file_name_iterator++;
        lodepng_encode32_file(file_name.c_str(), image.data(), OBSERVATION_WIDTH, OBSERVATION_HEIGHT);
    }
};

}
