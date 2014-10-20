//! [snippet1]

// We include what we need for the test
#include <gatb/gatb_core.hpp>
#include <iostream>

/********************************************************************************/
/*                              Bank management                                 */
/*                                                                              */
/* This snippet shows how to open a FASTA bank and iterate its sequences.       */
/* Some attributes of the iterated Sequence objects are used.                   */
/*                                                                              */
/********************************************************************************/
int main (int argc, char* argv[])
{
    // We get the file name from the user arguments
    const char* filename = argc >= 2 ? argv[1] : "";

    // We get information about the bank.
    u_int64_t nbSequences=0, dataSize=0, seqMaxSize=0, seqMinSize=~0;

    // We declare a Bank instance.
    BankFasta bank (filename);

    ProgressIterator<Sequence,Progress> it (bank, "iterate", 1000);
    for (it.first(); !it.isDone(); it.next())
    {
        Data& data = it.item().getData();

        nbSequences ++;
        if (data.size() > seqMaxSize)  { seqMaxSize = data.size(); }
        if (data.size() < seqMinSize)  { seqMinSize = data.size(); }
        dataSize += data.size ();
    }

    std::cout << "data size         : " << dataSize     << std::endl;
    std::cout << "sequence number   : " << nbSequences  << std::endl;
    std::cout << "sequence max size : " << seqMaxSize   << std::endl;
    std::cout << "sequence min size : " << seqMinSize   << std::endl;
}
//! [snippet1]
