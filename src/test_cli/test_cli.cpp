/*! @file test_cli.cpp
 *
 */

#include <iostream>
#include <iomanip>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <dist_vec_ret.hpp>
#include <dist_vec_ret_manager.hpp>
#include <time_utility.hpp>

#include <algorithm>

namespace fileapi = boost::filesystem;
int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage " << argv[0] << " <Path_to_index>" << std::endl;
            return EXIT_FAILURE;
        }
        if (!fileapi::exists(argv[1])) {
            std::cerr << "Path " << argv[1] << " doesn't exist" << std::endl;
            return EXIT_FAILURE;
        }
        fileapi::path path_to_index{argv[1]};

        auto global_state = std::make_shared<information_retrieval::global_weight_state_t>();
        auto manager = information_retrieval::dist_vec_ret_manager{global_state};

        auto indexing_timer = information_retrieval::time_utility{"Indexing"};
        for (const auto &entry : fileapi::recursive_directory_iterator(path_to_index)) {
            if (fileapi::is_regular_file(entry)) {
                manager.add_document(entry.path(), entry.path());
                indexing_timer.checkpoint(entry.path().filename().string());
            }
        }
        indexing_timer.stop();
        std::cout << indexing_timer;

        std::cout << "\n Finished indexing, please state your query to match: " << std::endl;
        std::string query;
        while (std::getline(std::cin, query)) {
            auto query_timer = information_retrieval::time_utility{"Query"};
            auto results = manager.find_match_for(query);
            query_timer.checkpoint("Comparing");
            std::sort(results.begin(), results.end(),
                      [](const decltype(results)::value_type &l, const decltype(results)::value_type &r)
                              -> bool { return std::get<0>(l) < std::get<0>(r); });
            query_timer.checkpoint("Sorting");
            query_timer.stop();
            std::cout << "Reults by ascending match order: \n";
            for (const auto &doc : results) {
                std::cout << std::setw(30) << std::get<1>(doc).common_name << ": " << std::get<0>(doc) << "\n";
            }
            std::cout << query_timer;
            std::cout << "\nQuery: " << std::endl;
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}