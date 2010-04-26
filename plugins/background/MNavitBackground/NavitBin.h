//***************************************************************
// CLass: NavitBin
//
// Description:
//
//
// Author: Chris Browet <cbro@semperpax.com> (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//******************************************************************

#ifndef NAVITBIN_H
#define NAVITBIN_H

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

class NavitBin
{
public:
    NavitBin();

    bool setFilename(QString filename);
    void readTile(QString fn);

private:
    QuaZip* zip;
    QuaZipFile* file;
};

#endif // NAVITBIN_H
