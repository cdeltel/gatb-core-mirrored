//! [snippet1]

// We include what we need for the test
#include <gatb/gatb_core.hpp>

/********************************************************************************/
/*                          Branching nodes neighbors                           */
/********************************************************************************/
int main (int argc, char* argv[])
{
    size_t kmerSize = 7;

    // We define some sequences used for building our test graph.
    // Note that the sequences have a difference at index==kmerSize
    const char* sequences[] =
    {
        //      x <- difference here
        "AGGCGCTAGGGAGAGGATGATGAAA",
        "AGGCGCTCGGGAGAGGATGATGAAA",
        "AGGCGCTTGGGAGAGGATGATGAAA"
    };

    // We create the graph.
    Graph graph = Graph::create (new BankStrings (sequences, ARRAY_SIZE(sequences)),  "-kmer-size %d  -nks 1", kmerSize);

    // We get the first node (should be AGGCGCT); this is a branching node.
    Node node = graph.buildNode ((char*)sequences[0]);

    // We retrieve the branching neighbors for the node.
    Graph::Vector<BranchingNode> branchingNeighbors = graph.successors<BranchingNode> (node);

    std::cout << "We found " << branchingNeighbors.size() << " branching neighbors from node " << graph.toString(node) << std::endl;

    // We loop over the branching neighbors. Here, we should have 3 branching neighbors, being the same GGGAGAG
    for (size_t i=0; i<branchingNeighbors.size(); i++)
    {
        std::cout << graph.toString (branchingNeighbors[i])  << std::endl;
    }

    return EXIT_SUCCESS;
}
//! [snippet1]
