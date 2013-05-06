/*****************************************************************************
 *   GATB : Genome Assembly Tool Box                                         *
 *   Copyright (c) 2013                                                      *
 *                                                                           *
 *   GATB is free software; you can redistribute it and/or modify it under   *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

/** \file FileSystemCommon.hpp
 *  \date 01/03/2013
 *  \author edrezen
 *  \brief Implementations common to various OS.
 */

#ifndef _GATB_CORE_SYSTEM_IMPL_FILE_SYSTEM_COMMON_HPP_
#define _GATB_CORE_SYSTEM_IMPL_FILE_SYSTEM_COMMON_HPP_

/********************************************************************************/

#include <gatb/system/api/IFileSystem.hpp>
#include <gatb/system/api/Exception.hpp>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/********************************************************************************/
namespace gatb      {
namespace core      {
namespace system    {
/** \brief Implementation of Operating System abstraction layer */
namespace impl      {
/********************************************************************************/

/** \brief Default implementation of IFile interface
 *
 *  This implementation uses standard C functions (stdio.h).
 */
class CommonFile : public IFile
{
public:

    /** Constructor. */
    CommonFile (const char* path, const char* mode) : _path(path), _handle(0), _isStdout(false)
    {
        _isStdout = path && strcmp(path,"stdout")==0;
        _handle   = _isStdout ? stdout : fopen (path, mode);
    }

    /** Destructor. */
    virtual ~CommonFile ()  {  if (_handle && !_isStdout)  {  fclose (_handle);  }  }

    /** \copydoc IFile::isOpen */
    bool isOpen ()  { return getHandle() != 0; }

    /** \copydoc IFile::isEOF */
    bool isEOF ()  {  return (isOpen() ? feof (getHandle()) : true); }

    /** \copydoc IFile::get */
    int get ()   {  return (isOpen() ? fgetc (getHandle()) : 0); }

    /** \copydoc IFile::unget */
    int unget (int c)   {  return (isOpen() ? ungetc (c, getHandle()) : 0); }

    /** \copydoc IFile::gets */
    int gets (char *s, int size)
    {
        int result = 0;

        /** We read the current line, up to 'size' characters. */
        char* tmp = (isOpen() ? fgets (s, size, getHandle()) : 0);

        /** Note: it may happen that the line is longer than the 'size' parameter.
         * Since this function is intended to read a line, we have to skip characters until the end of the line. */
        if (tmp != 0)
        {
            result = strlen (tmp);

            /** we skip all characters until we reach the next '\n'. */
            if (result > 0)  {  for (char c = tmp[result-1];  c !='\n' &&  c!=EOF;  c = fgetc (getHandle()))  {}  }
        }

        /** We return the result. */
        return result;
    }

    /** \copydoc IFile::print */
    void print (const char* format, ...)
    {
        if (isOpen())
        {
              va_list args;
              va_start (args, format);
              vfprintf (getHandle(), format, args);
              va_end (args);
        }
    }

    /** \copydoc IFile::fread */
    size_t fread (void* ptr, size_t size, size_t nmemb)
    {
        return ::fread (ptr, size, nmemb, getHandle());
    }

    /** \copydoc IFile::fwrite */
    size_t fwrite (const void* ptr, size_t size, size_t nmemb)
    {
        return ::fwrite (ptr, size, nmemb, getHandle());
    }

    /** \copydoc IFile::flush */
    void flush ()  { if (isOpen())  {  fflush (getHandle()); } }

    /** \copydoc IFile::getSize */
    u_int64_t getSize ()
    {
        /** Note: we have first to flush the buffer. */
        flush();

        struct stat st;
        return (stat (_path.c_str(), &st) == 0) ? st.st_size : 1000;
    }

    /** \copydoc IFile::getPath */
    const std::string& getPath () const  {  return _path;  }

protected:
    std::string _path;
    FILE*       _handle;
    bool        _isStdout;

    FILE* getHandle()
    {
        if (_handle == 0)  { throw Exception ("Bad handle"); }
        return _handle;
    }
};

/********************************************************************************/

/** \brief default implementation
 */
class FileSystemCommon : public IFileSystem
{
public:

    /** \copydoc IFileSystem::getAvailableSpace */
    u_int64_t  getAvailableSpace (const Path& path);

    /** \copydoc IFileSystem::getCurrentDirectory */
    Path getCurrentDirectory ();

    /** \copydoc IFileSystem::getTemporaryDirectory */
    Path getTemporaryDirectory ();

    /** \copydoc IFileSystem::doesExist */
    bool doesExist (const Path& path);

    /** \copydoc IFileSystem::getSize */
    u_int64_t  getSize (const Path& path);

    /** \copydoc IFileSystem::mkdir */
    int mkdir (const Path& path, u_int64_t mode)  {  return ::mkdir (path.c_str(), mode);  }

    /** \copydoc IFileSystem::rmdir */
    int rmdir (const Path& path)  { return ::rmdir (path.c_str()); }

    /** \copydoc IFileSystem::remove */
    int remove (const Path& path)  { return ::remove (path.c_str()); }

    /** \copydoc IFileSystem::iterate */
    void iterate (const Path& path, void (*callback) (const Path& entry, void* data), void* data);
};

/********************************************************************************/
} } } } /* end of namespaces. */
/********************************************************************************/

#endif /* _GATB_CORE_SYSTEM_IMPL_FILE_SYSTEM_COMMON_HPP_ */