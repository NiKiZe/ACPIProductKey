#include "acpi.h"
#include <iostream>
#include <string>


namespace
{

//---------------------------------------------------------------------------
    void ShowAcpiTable( AcpiTable::Ptr pTable, unsigned flags )
//---------------------------------------------------------------------------
    {
        bool verbose = (flags & AcpiTableQuery::FLAG_verbose) == AcpiTableQuery::FLAG_verbose;
        bool generate = (flags & AcpiTableQuery::FLAG_cmd_generate) == AcpiTableQuery::FLAG_cmd_generate;

        char *pProductKey = nullptr;

        AcpiMsdm_Type1 msdm( *pTable ); 

        pProductKey = new char[msdm.DataLength + 1];

        if (msdm.DataLength < msdm.length) 
            memcpy_s( pProductKey, msdm.DataLength + 1, msdm.ProductKey, msdm.DataLength );

        if (verbose) 
            std::cout << *pTable;

        else if (generate) 
        {
            std::cout << ("\"%SystemRoot%\\System32\\cscript.exe\" //NoLogo \"%SystemRoot%\\System32\\slmgr.vbs\" /ipk ") << *pProductKey << std::endl;
            std::cout << ("\"%SystemRoot%\\System32\\cscript.exe\" //NoLogo \"%SystemRoot%\\System32\\slmgr.vbs\" /ato") << std::endl;
        }
        else 
            std::cout << pProductKey << std::endl;

        delete [] pProductKey;

    }

}



//---------------------------------------------------------------------------
    AcpiTable::~AcpiTable()
//---------------------------------------------------------------------------
    {
        delete [] data.pBuffer;
    }



//---------------------------------------------------------------------------
    AcpiTable::Ptr AcpiTable::getTable( DWORD firmwareTableId ) 
//---------------------------------------------------------------------------
    {
        Ptr pTable( new AcpiTable );

        DWORD bufferSize = 0;
        UINT  bytesWritten;

        bufferSize = GetSystemFirmwareTable( getProviderSignature(), firmwareTableId, nullptr, 0 );
        pTable->data.pBuffer = new char[bufferSize]; 
        bytesWritten = GetSystemFirmwareTable( getProviderSignature(), firmwareTableId, pTable->data.pBuffer, bufferSize );

        return pTable;
    }



//---------------------------------------------------------------------------
    AcpiMsdm_Type1::AcpiMsdm_Type1( AcpiTable const &rBase )
//---------------------------------------------------------------------------
    {
        //if (rBase.data.pHeader && rBase.data.pHeader->length <= sizeof(AcpiMsdm_Type1))
        if (rBase.data.pHeader)
            memcpy( this, &rBase, sizeof(rBase.data.pHeader->length) );
        else
            memset( this, 0, sizeof(AcpiMsdm_Type1) );
    }



//---------------------------------------------------------------------------
    AcpiTableQuery::AcpiTableQuery() 
//---------------------------------------------------------------------------
    {
        tables = enumerateTables();
    }



//---------------------------------------------------------------------------
    void AcpiTableQuery::dumpTables( DWORD requesteTableId, unsigned flags ) 
//---------------------------------------------------------------------------
    {
        bool verbose = (flags & FLAG_verbose) == FLAG_verbose;

        if (verbose) 
            printf( "Found ACPI Tables: " );

        for (auto tableId : tables)
        {
            if (verbose) printf("%.*s ", 4, &tableId );

            //AcpiTable table = AcpiTable::getTable( tableId );
            //AcpiTable::Header &rHeader = *table.pData->pHeader;

            if (tableId == requesteTableId) 
            {
                if (verbose) printf("\n");
                ShowAcpiTable( AcpiTable::getTable(tableId), flags );
            }
        }
    }



//---------------------------------------------------------------------------
    bool AcpiTableQuery::hasTable( ULONG tableId )
//---------------------------------------------------------------------------
    {
        return std::find( tables.begin(), tables.end(), _byteswap_ulong(tableId) ) != tables.end();
    }



//---------------------------------------------------------------------------
    AcpiTableQuery::Tables AcpiTableQuery::queryTables( std::initializer_list<ULONG> const &rTableIds )
//---------------------------------------------------------------------------
    {
        Tables foundTables;

        for (auto &id : rTableIds )
        {
            Tables::iterator it = tables.end();

            if ((it = std::find(tables.begin(), tables.end(), id)) != tables.end())
                foundTables.push_back( *it );
        }

        return foundTables;
    }



//---------------------------------------------------------------------------
    AcpiTableQuery::Tables AcpiTableQuery::enumerateTables() 
//---------------------------------------------------------------------------
    {
        DWORD bufferSize = 0;
        UINT  bytesWritten;

        Tables tables;

        // get buffer size, call with null values
        bufferSize = EnumSystemFirmwareTables( AcpiTable::getProviderSignature(), nullptr, 0 );

        tables.resize( bufferSize / sizeof(DWORD) );


        // enum acpi tables
        bytesWritten = EnumSystemFirmwareTables( AcpiTable::getProviderSignature(), &tables[0], bufferSize );

        //const DWORD searchMSDMFirmwareTableId = _byteswap_ulong( msDigitalLicenceId );

        return tables;
    }



//---------------------------------------------------------------------------
    std::ostream& operator<< (std::ostream& rStream, AcpiTable::Header const &rHeader)
//---------------------------------------------------------------------------
    {
        std::string s;
        s.resize( 255 );

        sprintf( &s[0], "Signature         : %4.4s\n",  &rHeader.signature );           rStream << s << std::endl; s.clear();
        sprintf( &s[0], "Length            : %d\n",     rHeader.length);                rStream << s << std::endl; s.clear();
        sprintf( &s[0], "Revision          : %d\n",     rHeader.revision);              rStream << s << std::endl; s.clear();
        sprintf( &s[0], "Checksum          : %d\n",     rHeader.checksum);              rStream << s << std::endl; s.clear();
        sprintf( &s[0], "OEMID             : %6.6s\n", &rHeader.oem_id);                rStream << s << std::endl; s.clear();
        sprintf( &s[0], "OEM Table ID      : %8.8s\n", &rHeader.oem_table_id);          rStream << s << std::endl; s.clear();
        sprintf( &s[0], "OEM Revision      : %d\n",     rHeader.oem_revision);          rStream << s << std::endl; s.clear();
        sprintf( &s[0], "Creator ID        : %4.4s\n", &rHeader.asl_compiler_id);       rStream << s << std::endl; s.clear();
        sprintf( &s[0], "Creator Revision  : %d\n",     rHeader.asl_compiler_revision); rStream << s << std::endl; s.clear();

        return rStream;
    }



//---------------------------------------------------------------------------
    std::ostream& operator<< (std::ostream& rStream, AcpiMsdm_Type1 const &msdm)
//---------------------------------------------------------------------------
    {
        rStream << static_cast<AcpiTable::Header const &>( msdm );

        std::string s;
        s.resize( 255 );

        sprintf( &s[0], "SLS Version       : %d\n",  msdm.Version);         rStream << s; s.clear();
        sprintf( &s[0], "SLS Reserved      : %d\n",  msdm.Reserved);        rStream << s; s.clear();
        sprintf( &s[0], "SLS Data Type     : %d\n",  msdm.DataType);        rStream << s; s.clear();
        sprintf( &s[0], "SLS Data Reserved : %d\n",  msdm.DataReserved);    rStream << s; s.clear();
        sprintf( &s[0], "SLS Data Length   : %d\n",  msdm.DataLength);      rStream << s; s.clear();
        sprintf( &s[0], "Key               : %s\n", &msdm.ProductKey);      rStream << s; s.clear();

        return rStream;
    }                                                                       
                                                                            
                                                                            

