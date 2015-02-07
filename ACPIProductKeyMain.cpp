// ACPIProductKey.cpp : Read MSDM license information from BIOS ACPI tables.
//

/* Copyright (C) 2014 Christian Nilsson and Michael Andersson
 * Copyright (C) 2013 aghoneim in a post on technet.microsoft.com - Seen as Public Domain?
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "acpi.h"
#include <iostream>



bool  verbose = false;
bool  cmd_generate = false;



int main( int argc, char* argv[] )
{
    for (int argidx = 1; argidx < argc; argidx++) 
    {
        if (strcmp(argv[argidx], ("-v")) == 0)
            verbose = true;
        else if (strcmp(argv[argidx], ("-h")) == 0 ||
            strcmp(argv[argidx], ("/h")) == 0 ||
            strcmp(argv[argidx], ("-?")) == 0 ||
            strcmp(argv[argidx], ("/?")) == 0 ||
            strcmp(argv[argidx], ("--help")) == 0 ||
            strcmp(argv[argidx], ("/help")) == 0) 
        {
            std::wcout << ("Usage for ") << argv[0] << std::endl;
            std::wcout << ("\t\tShows Windows Product key saved on PC motherboard") << std::endl;
            std::wcout << ("\t\tAnd possible other ACPI data.") << std::endl;
            std::wcout << ("\t-h\tShow this help and exit") << std::endl;
            std::wcout << ("\t-v\tDisplay more information about the process") << std::endl;
            std::wcout << ("\t-g\tGenerate list of commands for activating with key (export to cmd file instead of using -a)") << std::endl;
            return 0;
        }
        else if (strcmp(argv[argidx], ("-g")) == 0)
            cmd_generate = true;
        else 
        {
            std::wcout << ("Unknown option: ") << argv[argidx] << std::endl;
            std::wcout << ("\tUse -h for help") << std::endl;
            return 1;
        }
    }

    AcpiTableQuery tables;
    tables.queryTables( {AcpiTableQuery::msDigitalLicenceId, AcpiTableQuery::msSoftwareLicenceId} );
    //tables.dumpTables( tables.msDigitalLicenceId, verbose );
    auto pTable = AcpiTable::getTable( AcpiTableQuery::apicId );
    std::cout << *pTable;
    //tables.dumpTables( pTable, verbose );


#ifdef _DEBUG
    std::wcout << std::endl << std::endl << ("Press [Enter] to exit . . .");
    fflush(stdout);
    getchar();
#endif

    return 0;
}
