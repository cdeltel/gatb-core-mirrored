/*****************************************************************************
 *   GATB : Genome Assembly Tool Box                                         *
 *   Authors: [R.Chikhi, G.Rizk, E.Drezen]                                   *
 *   Based on Minia, Authors: [R.Chikhi, G.Rizk], CeCILL license             *
 *   Copyright (c) INRIA, CeCILL license, 2013                               *
 *****************************************************************************/

/** \file ITime.hpp
 *  \date 01/03/2013
 *  \author edrezen
 *  \brief Interface for time retrieval.
 *
 * This is mainly used for debug/statistical information.
 */

#ifndef _GATB_CORE_SYSTEM_ITIME_HPP_
#define _GATB_CORE_SYSTEM_ITIME_HPP_

/********************************************************************************/

#include <gatb/system/api/types.hpp>

/********************************************************************************/
namespace gatb      {
namespace core      {
namespace system    {
/********************************************************************************/

/** \brief Interface that provides methods returning time information.
 *
 * This interface mainly defines an operation that returns a time stamp, according to
 * the beginning of the current process.
 *
 * The unit of the returned value may depend on the implementation. It could be second,
 * millisecond and so on, or we can also use a unit independent of time, like the number
 * of CPU cycles, which may be interesting for comparing the same test on different machines
 * with different clock frequencies.
 */
class ITime
{
public:

    /** \brief enumeration defining time units */
    enum Unit
    {
        USEC      = 1000000, // 10-6 second
        MSEC      = 1000,    // 10-3 second
        SEC       = 1,       // 10-0 second
        UNDEFINED = ~0
    };

    /** Alias for the possible values for time stamping. Note that we use a large integer because we may have huge values
     * in case we use the CPUCLOCK unit (for instance, value=2000000000 for one second with CPU frequency of 2 GHz). */
    typedef u_int64_t Value;

    /** Returns a time stamp, ie the number of time atoms (depending on chosen time unit) since some define T0 time. Such
     * a starting time can be the 01.01.1970 for instance, or the beginning of the process. It implies that clients should
     * be interested not in the absolute value returned by the method but by the distance between two such values.
     * \return elapsed time atoms number.
     */
    virtual Value getTimeStamp() = 0;

    /** \return the time unit used by getTimeStamp. */
    virtual Unit getUnit () = 0;

    /** Get a string holding information about the current date.
     * \return the formatted date string. */
    virtual std::string getDateString () = 0;

    /** Destructor. */
    virtual ~ITime () {}
};

/********************************************************************************/
} } } /* end of namespaces. */
/********************************************************************************/

#endif /* _GATB_CORE_SYSTEM_ITIME_HPP_ */
