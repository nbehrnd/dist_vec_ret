/*! @file mpi_utility.cpp
 *
 */

#include "mpi_utility.hpp"

#include <iostream>

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>

#include <time_utility.hpp>

#include "mpi_async.hpp"
#include "mpi_async_dispatcher.hpp"

enum struct com_tags : int {
    indexing,
    querying,
};

void organize_serving_nodes(const int count_processes, const char *path) {
    namespace fileapi = boost::filesystem;
    if (!fileapi::exists(path)) {
        throw std::runtime_error(std::string{"Path "} + path + " doesn't exist");
    }
    fileapi::path path_to_index{path};
    mpi::mpi_async_dispatcher index_com_dispatcher{};
    std::cout << "Dispatcher created" << std::endl;
    auto indexing_timer = information_retrieval::time_utility{"Indexing"};
    size_t file_count = 0;
    for (const auto &entry : fileapi::recursive_directory_iterator(path_to_index)) {
        if (!fileapi::is_regular_file(entry)) {
            continue;
        }
        ++file_count;
        auto request = std::unique_ptr<mpi::mpi_async_send<char, std::string>>
                {new mpi::mpi_async_send<char, std::string>
                         (entry.path().string())};
        request->send(file_count % (count_processes - 1) + 1, static_cast<int>(com_tags::indexing));
        index_com_dispatcher.add_request(std::move(request), std::function<void()>{});
    }
    indexing_timer.checkpoint("Sending indexing commands");
    index_com_dispatcher.stop();
    indexing_timer.checkpoint("Waiting for all nodes to finish indexing");
    indexing_timer.stop();
    std::cout << indexing_timer;
}

void mpi_query_cli_node_main() {
    std::cout << "Querying" << std::endl;
    std::string query;
    std::cin >> query;
}

void mpi_serving_node_main() {
    std::cout << "Call recv";
    auto data = mpi::mpi_async_recv<char>(0, static_cast<int>(com_tags::indexing)).get();
    std::cout << "Finished recv.get";
    data.push_back('\0');
    std::cout << std::string{data.data()} << std::endl;
}
