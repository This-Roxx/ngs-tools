#include <iostream>
#include <chrono>
#include <set>
#include <map>
#include "config_merge_kmers_to_tree.h"
#include "../seq_loader.h"
#include "../file_list_loader.h"

#include "features.h"
#include "feature_tree.h"
#include "feature_tree_io.h"
#include "feature_tree_builder.h"

using namespace std;
using namespace std::chrono;

const string VERSION = "0.11";

void merge_kmers_to_tree(TreeBuilder<SimilarityOf1Mers> &builder, const string &filename, int kmer_len, int data_frame)
{
    SeqLoader::for_every_clean_sequence_do(filename, [&](p_string line)
    {
        const int STEP = 2; // todo: make it right
		for (int kmer_from = data_frame; kmer_from <= line.len - kmer_len; kmer_from += STEP)
        {
            builder.insert(FeatureOperations::calculate_features(line.s + kmer_from, kmer_len));
            //cout << "---------------------- tree nodes:" << builder.tree.nodes.size() << endl;
            //for (int i = 0; i< kmer_len; i++)
            //    cout << line.s[kmer_from + i];
            //cout << endl;
            //print_tree(builder.tree, builder.tree.head_id);
        }
    });
};

void merge_kmers_to_tree_list(const Config &config)
{
    SimilarityOf1Mers sim;
    TreeBuilder<SimilarityOf1Mers> builder(sim);

    if (TreeIO::file_exists(config.tree_file))
        TreeIO::load_tree(builder.tree, config.tree_file);

	FileListLoader file_list(config.file_list);
    int file_number = 0;

    for (int file_number = 0; file_number < int(file_list.files.size()); file_number ++)
    {
        auto &file = file_list.files[file_number];
        cerr << file_number << " of " << file_list.files.size() << " loading file " << file.filename << endl;
        merge_kmers_to_tree(builder, file.filename, config.kmer_len, config.data_frame);

        cout << "---------------------- tree nodes:" << builder.tree.nodes.size() << endl;
        cerr << "---------------------- tree nodes:" << builder.tree.nodes.size() << endl;

        cout << "saving: levels: " << config.levels_to_save << endl;
//        TreeIO::print_tree<char>(builder.tree, config.levels_to_save);
        TreeIO::save_tree(builder.tree, config.tree_file, config.levels_to_save);
        TreeIO::load_tree(builder.tree, config.tree_file);
        cout << "loaded: " << endl;
        TreeIO::print_tree<char>(builder.tree);

        cout << "loaded  -------------- tree nodes:" << builder.tree.nodes.size() << endl;
        cerr << "loaded  -------------- tree nodes:" << builder.tree.nodes.size() << endl;
    }

//    save_tree(builder.tree, config.tree_file);
//    cout << "-----------------" << endl;
    //print_tree(builder.tree, builder.tree.head_id);
}

int main(int argc, char const *argv[])
{
    try
    {
		cerr << "merge_kmers_to_tree version " << VERSION << endl;
		Config config(argc, argv);
		cerr << "kmer len: " << config.kmer_len << endl;
		cerr << "data frame: " << config.data_frame << endl;
		cerr << "levels to save: " << config.levels_to_save << endl;

		auto before = high_resolution_clock::now();

        merge_kmers_to_tree_list(config);

		cerr << "total time (sec) " << std::chrono::duration_cast<std::chrono::seconds>( high_resolution_clock::now() - before ).count() << endl;
        return 0;
    }
    catch ( exception & x )
    {
        cerr << x.what() << endl;
		cerr << "exit 3" << endl;
		return 3;
    }
    catch ( string & x )
    {
        cerr << x << endl;
		cerr << "exit 4" << endl;
		return 4;
    }
    catch ( const char * x )
    {
        cerr << x << endl;
		cerr << "exit 5" << endl;
		return 5;
    }
    catch ( ... )
    {
        cerr << "unknown exception" << endl;
//		cerr << "exit 6" << endl;
		return 6;
    }
}