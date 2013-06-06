/*****************************************************************************
 *   GATB : Genome Assembly Tool Box                                         *
 *   Authors: [R.Chikhi, G.Rizk, E.Drezen]                                   *
 *   Based on Minia, Authors: [R.Chikhi, G.Rizk], CeCILL license             *
 *   Copyright (c) INRIA, CeCILL license, 2013                               *
 *****************************************************************************/

#include <gatb/tools/misc/impl/Progress.hpp>
#include <gatb/system/impl/System.hpp>

#include <stdarg.h>
#include <stdio.h>

#define DEBUG(a)  //printf a

using namespace std;
using namespace gatb::core::system;
using namespace gatb::core::system::impl;

/********************************************************************************/
namespace gatb {  namespace core { namespace tools {  namespace misc {  namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
Progress::Progress (u_int64_t ntasks, const char * msg, std::ostream& output)
    : os(output)
{
    message = (msg != NULL ? msg : "?");
    todo    = ntasks;
    done    = 0;
    partial = 0;
    subdiv  = 100;
    steps   = (double)todo / (double)subdiv;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Progress::init ()
{
    /** We format the string to be displayed. */
    snprintf (buffer, 256, "| %-*s |\n",101, message.c_str());

    /** We dump the message.*/
    os << buffer;

    /** We forward the remaining of the init to postInit method. */
    postInit ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Progress::finish ()
{
    /** We set the total done. */
    set (todo);

    /** We forward the remaining of the finish to postFinish method. */
    postFinish ();

    /** We reset all progression variables. */
    todo    = 0;
    done    = 0;
    partial = 0;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Progress::inc (u_int64_t ntasks_done)
{
    done    += ntasks_done;
    partial += ntasks_done;

    while (partial >= steps)
    {
        update ();
        partial -= steps;
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
void Progress::set (u_int64_t ntasks_done)
{
    if (ntasks_done > done)
    {
        inc (ntasks_done-done);
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
void Progress::setMessage (const char* format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);
    vsprintf (buffer, format, args);
    va_end(args);

    message.assign (buffer);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Progress::postInit ()
{
    os << "[";
    os.flush();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Progress::postFinish ()
{
    os << "]" << endl;
    os.flush();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void Progress::update ()
{
    os << "-";
    os.flush();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
ProgressTimer::ProgressTimer (u_int64_t ntasks, const char* msg, std::ostream& os)
    : Progress (ntasks, msg, os), heure_debut(0), heure_actuelle(0)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ProgressTimer::postInit ()
{
    /** We get the current hour (in msec) */
    heure_debut = System::time().getTimeStamp();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ProgressTimer::postFinish ()
{
    os << endl;
    os.flush();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void ProgressTimer::update ()
{
    /** We get the current hour (in msec) */
    heure_actuelle = System::time().getTimeStamp();

    /** We compute the difference between the current time and the starting time. */
    double elapsed = (heure_actuelle - heure_debut) / 1000.0;

    /** A little check. */
    if (elapsed > 0  &&  done > 0)
    {
        double speed  = done        / elapsed;
        double rem    = (todo-done) / speed;

        if (done>todo) rem=0;
        int min_e  = (int)(elapsed / 60) ;
        elapsed -= min_e*60;
        int min_r  = (int)(rem / 60) ;
        rem -= min_r*60;

        /** We format the string to be displayed. */
        snprintf (buffer, sizeof(buffer), "%c%-5.3g  %%     elapsed: %6i min %-4.0f  sec      estimated remaining: %6i min %-4.0f  sec ",
            13,100*(double)done/todo,min_e,elapsed,min_r,rem
        );

        /** We dump the string. */
        os << buffer;
    }
}

/********************************************************************************/
} } } } } /* end of namespaces. */
/********************************************************************************/
