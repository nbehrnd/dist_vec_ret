/*! @file mpi_utility.cpp
 *
 */

#include "mpi_utility.hpp"

#include <iostream>

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>


void organize_serving_nodes(const int count_processes, const char *path) {
    namespace fileapi = boost::filesystem;
    if (!fileapi::exists(path)) {
        throw std::runtime_error(std::string{"Path "} + path + " doesn't exist");
    }
    fileapi::path path_to_index{path};

    size_t file_count = 0;
    for (const auto &entry : fileapi::recursive_directory_iterator(path_to_index)) {
        if (!fileapi::is_regular_file(entry)) {
            continue;
        }
        ++file_count;
    }
    std::cout << "File count " << file_count << std::endl;
}

void mpi_query_cli_node_main() {
    std::cout << "Querying" << std::endl;
    std::string query;
    std::cin >> query;
}

void mpi_serving_node_main() {
    std::cout << "Serving" << std::endl;
}