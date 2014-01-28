/*****************************************************************************
 *   GATB : Genome Assembly Tool Box
 *   Copyright (C) 2014  R.Chikhi, G.Rizk, E.Drezen
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

/** \file Model.tpp
 *  \date 01/03/2013
 *  \author edrezen
 *  \brief Kmer management
 */

#ifndef _GATB_CORE_KMER_IMPL_MODEL_TPP_
#define _GATB_CORE_KMER_IMPL_MODEL_TPP_

/********************************************************************************/
namespace gatb      {
namespace core      {
namespace kmer      {
namespace impl      {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
Kmer<span>::Model::Model (size_t sizeKmer, KmerMode mode) : _kmerSize(sizeKmer), _mode(mode)
{
    /** We check that the Type precision is enough for the required kmers span. */
    if (sizeKmer >= span)
    {
        throw system::Exception ("Type '%s' has too low precision (%d bits) for the required %d kmer size",
            Type().getName(), Type().getSize(), sizeKmer
        );
    }

    /** We compute the mask of the kmer. Useful for computing kmers in a recursive way. */
    Type un = 1;
    _kmerMask = (un << (_kmerSize*2)) - un;

    size_t shift = 2*(_kmerSize-1);

    /** The _revcompTable is a shortcut used while computing revcomp recursively. */
    /** Important: don't forget the Type cast, otherwise the result in only on 32 bits. */
    for (size_t i=0; i<4; i++)
    {
        Type tmp  = comp_NT[i];
        _revcompTable[i] = tmp << shift;
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
typename Kmer<span>::Type Kmer<span>::Model::codeSeed (const char* seq, tools::misc::Data::Encoding_e encoding, KmerMode mode)  const
{
    Type direct = 0;

          if (encoding == tools::misc::Data::ASCII)    { for (size_t i=0; i<this->_kmerSize; ++i)  {  direct = direct*4 + NT2int(seq[i]);  }  }
    else  if (encoding == tools::misc::Data::INTEGER)  { for (size_t i=0; i<this->_kmerSize; ++i)  {  direct = direct*4 +       (seq[i]);  }  }
    else  if (encoding == tools::misc::Data::BINARY)   { for (size_t i=0; i<this->_kmerSize; ++i)  {  direct = direct*4 + (((seq[i>>2] >> ((3-(i&3))*2)) & 3));  }  }
    else  { throw system::Exception ("BAD FORMAT IN codeSeed"); }

    if (mode == KMER_DIRECT)  { return direct; }

    Type rev = revcomp (direct, _kmerSize);

    if (mode == KMER_REVCOMP)  { return rev; }

    return std::min (direct, rev);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
typename Kmer<span>::Type Kmer<span>::Model::codeSeedRight (const Type& seed, char nucl, tools::misc::Data::Encoding_e encoding, KmerMode mode)  const
{
    Type direct;

    if (encoding == tools::misc::Data::ASCII)   {  direct = ( (seed << 2) + NT2int(nucl)) & _kmerMask;  }
    else                                        {  direct = ( (seed << 2) +       (nucl)) & _kmerMask;  }

    if (mode == KMER_DIRECT)  { return direct; }

    Type rev = revcomp (direct, _kmerSize);

    if (mode == KMER_REVCOMP)  { return rev; }

    if (mode == KMER_MINIMUM)  { return std::min(direct,rev); }

    throw system::Exception ("BAD MODE for computing kmer successor");
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
std::pair <typename Kmer<span>::Type, bool>  Kmer<span>::Model::getKmer (const tools::misc::Data& data, size_t idx, KmerMode mode)  const
{
    /** We compute the first kmer as a polynomial value. */
    Type graine  = codeSeed (data.getBuffer()+idx, data.getEncoding(), KMER_DIRECT);
    Type revcomp = codeSeed (data.getBuffer()+idx, data.getEncoding(), KMER_REVCOMP);

    bool isForward = graine < revcomp;
    Type value     = isForward ? graine : revcomp;

         if (mode == KMER_MINIMUM)  {  return std::make_pair (value, isForward); }
    else if (mode == KMER_DIRECT)   {  return std::make_pair (graine, true); }
    else  {  throw system::Exception ("BAD KMER MODE");  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
template<typename Functor>
bool Kmer<span>::Model::iterate (tools::misc::Data& data, Functor fct, KmerMode mode) const
{
    /** We compute the number of kmers for the provided data. Note that we have to check that we have
     * enough nucleotides according to the current kmer size. */
    int32_t nbKmers = data.size() - this->getKmerSize() + 1;
    if (nbKmers <= 0)  { return false; }

    /** Shortcut used for computing kmers recursively. */
    char* buffer = data.getBuffer() + this->getKmerSize() - 1;

    /** We compute the first kmer as a polynomial value. */
    Type graine  = codeSeed (data.getBuffer(), data.getEncoding(), KMER_DIRECT);
    Type revcomp = codeSeed (data.getBuffer(), data.getEncoding(), KMER_REVCOMP);
    if(mode == KMER_MINIMUM)
    {
        fct (std::min (graine, revcomp), 0);
    }
    else if (mode == KMER_DIRECT)
    {
        fct (graine, 0);
    }
    else  {  throw system::Exception ("BAD KMER MODE");  }

    /** NOTE: we have some kind of duplicated code here: the only difference is the way we retrieve the 'c'
     *  character according toe the encoding mode. We could have some function pointer for factorizing this.
     *
     *  BUT, using function pointer would lead to many function calls with bad performance. We prefer therefore
     *  duplicate code and let inline work when needed.
     */
    if (data.getEncoding() == tools::misc::Data::ASCII && mode == KMER_MINIMUM)
    {
        /** We compute the next kmers in a recursive way. */
        for (size_t i=1; i<nbKmers; i++)
        {
            char c = NT2int(buffer[i]);
            graine  = ( (graine  << 2) +  c                     ) & this->_kmerMask;
            revcomp = ( (revcomp >> 2) +  this->_revcompTable[c]) & this->_kmerMask;
            fct (std::min (graine, revcomp), i);
        }
    }
    else if (data.getEncoding() == tools::misc::Data::INTEGER && mode == KMER_MINIMUM)
    {
        /** We compute the next kmers in a recursive way. */
        for (size_t i=1; i<nbKmers; i++)
        {
            char c = NTidentity(buffer[i]);
            graine  = ( (graine  << 2) +  c                     ) & this->_kmerMask;
            revcomp = ( (revcomp >> 2) +  this->_revcompTable[c]) & this->_kmerMask;
            fct (std::min (graine, revcomp), i);
        }
    }
    else if (data.getEncoding() == tools::misc::Data::BINARY && mode == KMER_MINIMUM)
    {
        buffer = data.getBuffer();

        size_t i0 = this->getKmerSize();
        size_t i1 = data.size();

        /** We compute the next kmers in a recursive way. */
        for (size_t i=i0; i<i1; i++)
        {
            char c = ((buffer[i>>2] >> ((3-(i&3))*2)) & 3);

            graine  = ( (graine  << 2) +  c                     ) & this->_kmerMask;
            revcomp = ( (revcomp >> 2) +  this->_revcompTable[c]) & this->_kmerMask;
            fct (std::min (graine, revcomp), i-i0+1);
        }
    }
    else if (data.getEncoding() == tools::misc::Data::ASCII && mode == KMER_DIRECT) //GR modif, check this with erwan
    {
        /** We compute the next kmers in a recursive way. */
        for (size_t i=1; i<nbKmers; i++)
        {
            char c = NT2int(buffer[i]);
            graine  = ( (graine  << 2) +  c                     ) & this->_kmerMask;
            fct (graine, i);
        }
    }
    else if (data.getEncoding() == tools::misc::Data::INTEGER && mode == KMER_DIRECT)
    {
        /** We compute the next kmers in a recursive way. */
        for (size_t i=1; i<nbKmers; i++)
        {
            char c = NTidentity(buffer[i]);
            graine  = ( (graine  << 2) +  c                     ) & this->_kmerMask;
            fct (graine, i);
        }
    }
    else if (data.getEncoding() == tools::misc::Data::BINARY && mode == KMER_DIRECT)
    {
        /** We compute the next kmers in a recursive way. */
        for (size_t i=1; i<nbKmers; i++)
        {
            char c = ((buffer[i>>2] >> ((3-(i&3))*2)) & 3);
            graine  = ( (graine  << 2) +  c                     ) & this->_kmerMask;
            fct (graine, i);
        }
    }
    else  {  throw system::Exception ("BAD DATA FORMAT TO BE CONVERTED IN KMERS...");  }

    return true;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
template<typename Functor>
bool Kmer<span>::Model::iterate (Functor fct) const
{
    Type kmerMax = ((Type)1) << (2*_kmerSize);

    /** We iterate each possible kmer. */
    for (Type kmer (0); kmer < kmerMax; kmer += 1)  {  fct (kmer);  }

    return true;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
template<typename Functor>
void Kmer<span>::Model::iterateNeighbors (const Type& source, const Functor& fct)  const
{
    Type rev = core::tools::math::revcomp (source, getKmerSize());

    /** We compute the 8 possible neighbors. */
    for (size_t nt=0; nt<4; nt++)
    {
        {
            Type next1 = (((source) * 4 )  + nt) & getMask();
            Type next2 = revcomp (next1, getKmerSize());
            fct (std::min (next1, next2));
        }
        {
            Type next1 = (((rev) * 4 )  + nt) & getMask();
            Type next2 = revcomp (next1, getKmerSize());
            fct (std::min (next1, next2));
        }
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
bool Kmer<span>::Model::build (tools::misc::Data& data, std::vector<Kmer<span>::Type>& kmersBuffer, KmerMode mode)  const
{
    /** We compute the number of kmers for the provided data. Note that we have to check that we have
     * enough nucleotides according to the current kmer size. */
    int32_t nbKmers = data.size() - this->getKmerSize() + 1;
    if (nbKmers <= 0)  { return false; }

    /** We resize the resulting kmers vector. */
    kmersBuffer.resize (nbKmers);

    /** Shortcut used for computing kmers recursively. */
    char* buffer = data.getBuffer() + this->getKmerSize() - 1;

    /** We fill the vector through a functor. */
    this->iterate (data, [&] (const Type& kmer, size_t idx)
    {
        kmersBuffer[idx] = kmer;
    });

    return true;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
void Kmer<span>::Model::Iterator::setData (tools::misc::Data& d)
{
    /** We fill the vector with the items to be iterated. */
    _ref.build (d, this->_items);

    /** We set the vector size. */
    this->_nb = this->_items.size();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS : We could implement this with Model::iterateNeighbors
*********************************************************************/
template<size_t span>
void Kmer<span>::Model::NeighborIterator::setSource (const Type& source)
{
    size_t idx = 0;

    Type rev = core::tools::math::revcomp (source, _ref.getKmerSize());

    /** We compute the 8 possible neighbors. */
    for (size_t nt=0; nt<4; nt++)
    {
        {
            Type next1 = (((source) * 4 )  + nt) & _ref.getMask();
            Type next2 = revcomp (next1, _ref.getKmerSize());
            this->_items[idx++] = std::min (next1, next2);
        }
        {
            Type next1 = (((rev) * 4 )  + nt) & _ref.getMask();
            Type next2 = revcomp (next1, _ref.getKmerSize());
            this->_items[idx++] = std::min (next1, next2);
        }
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
template<size_t span>
hid_t Kmer<span>::Count::hdf5 (bool& isCompound)
{
    hid_t abundanceType = H5Tcopy (H5T_NATIVE_UINT16);

    hid_t result = H5Tcreate (H5T_COMPOUND, sizeof(Count));
    H5Tinsert (result, "value",      HOFFSET(Count, value),     Type::hdf5(isCompound));
    H5Tinsert (result, "abundance",  HOFFSET(Count, abundance), abundanceType);

    isCompound = true;

    return result;
}

/********************************************************************************/
} } } } /* end of namespaces. */
/********************************************************************************/

#endif /* _GATB_CORE_KMER_IMPL_MODEL_TPP_ */
