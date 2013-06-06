/*****************************************************************************
 *   GATB : Genome Assembly Tool Box                                         *
 *   Authors: [R.Chikhi, G.Rizk, E.Drezen]                                   *
 *   Based on Minia, Authors: [R.Chikhi, G.Rizk], CeCILL license             *
 *   Copyright (c) INRIA, CeCILL license, 2013                               *
 *****************************************************************************/

/** \file BagFile.hpp
 *  \date 01/03/2013
 *  \author edrezen
 *  \brief Bag implementation for file
 */

#ifndef _GATB_CORE_TOOLS_COLLECTIONS_IMPL_BAG_FILE_HPP_
#define _GATB_CORE_TOOLS_COLLECTIONS_IMPL_BAG_FILE_HPP_

/********************************************************************************/

#include <gatb/tools/collections/api/Bag.hpp>
#include <gatb/system/impl/System.hpp>

#include <string>

/********************************************************************************/
namespace gatb          {
namespace core          {
namespace tools         {
namespace collections   {
/** Implementation of API from collections */
namespace impl          {
/********************************************************************************/

/** \brief Bag implementation for file
 */
template <typename Item> class BagFile : public Bag<Item>
{
public:

    /** Constructor. */
    BagFile (const std::string& filename) : _filename(filename), _file(0)
    {
        _file = system::impl::System::file().newFile (filename, "wb");
    }

    /** Destructor. */
    ~BagFile ()
    {
        if (_file)  { delete _file; }
    }

    /** */
    const std::string& getName () const { return _filename; }

    /**  \copydoc Bag::insert */
    void insert (const Item& item)  {  _file->fwrite (&item, sizeof(Item), 1);  }

    void insert (const std::vector<Item>& items, size_t length)
    {
        if (length == 0)  { length = items.size(); }
        _file->fwrite (items.data(), sizeof(Item), length);
    }

    /**  \copydoc Bag::flush */
    void flush ()  { _file->flush(); }

private:
    std::string _filename;
    system::IFile* _file;
};

/********************************************************************************/
} } } } } /* end of namespaces. */
/********************************************************************************/

#endif /* _GATB_CORE_TOOLS_COLLECTIONS_IMPL_BAG_FILE_HPP_ */
