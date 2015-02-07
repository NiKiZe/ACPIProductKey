#ifndef ACPI_HEADER_INCLUDE_GUARD
#define ACPI_HEADER_INCLUDE_GUARD


#include <Windows.h>
#include <stdint.h>
#include <memory>
#include <vector>
#include <ostream>

/**
* An ACPI description header
*
* This is the structure common to the start of all ACPI system
* description tables.
* Copy from iPXE project Released under GPL 2
*/
//---------------------------------------------------------------------------
    class AcpiTable 
//---------------------------------------------------------------------------
    {
        static const DWORD acpiFirmwareTableProviderSignature = 'ACPI';


    public:

        typedef std::shared_ptr<AcpiTable> Ptr;

        static DWORD getProviderSignature() { return acpiFirmwareTableProviderSignature; }

        struct Header
        {
            /** ACPI signature (4 ASCII characters) */
            uint32_t signature;
            /** Length of table, in bytes, including header */
            uint32_t length;
            /** ACPI Specification minor version number */
            uint8_t revision;
            /** To make sum of entire table == 0 */
            uint8_t checksum;
            /** OEM identification */
            char oem_id[6];
            /** OEM table identification */
            char oem_table_id[8];
            /** OEM revision number */
            uint32_t oem_revision;
            /** ASL compiler vendor ID */
            char asl_compiler_id[4];
            /** ASL compiler revision number */
            uint32_t asl_compiler_revision;
        };


        union
        {
            char *pBuffer = nullptr;
            Header *pHeader;
        }
        data;


        ~AcpiTable();
        static Ptr getTable( DWORD firmwareTableId );
    };



/*
Table description form Microsoft at: http://go.microsoft.com/fwlink/p/?LinkId=234834
Microsoft Software Licensing Tables (SLIC and MSDM)
http://msdn.microsoft.com/library/windows/hardware/hh673514
Byte 							Byte
Field							Length			Offset	Description
======							=====			======	===========
Signature						4				0		MSDM
Length							4				4		Length, in bytes, of the entire table.
Revision						1				8		0x01
Checksum						1				9		Checksum of the entire table.
OEMID							6				10		An OEM-supplied string that identifies the OEM.
OEM Table ID					8				16		Optional motherboard/BIOS logical identifier.
OEM Revision					4				24		OEM revision number of the table for the supplied OEM Table ID.
Creator ID						4				28		Vendor ID of the utility that created the table.
Creator Revision				4				32		Revision of the utility that created the table.
Software Licensing Structure	Variable length	36		Proprietary data structure that contains all the licensing
data necessary to enable Windows activation.
Details can be found in the appropriate Microsoft OEM
licensing kit by first visiting the Microsoft OEM website
(http://www.microsoft.com/oem/pages/index.aspx).
*/

/* Future types of this struct is to be expected */

//---------------------------------------------------------------------------
    struct AcpiMsdm_Type1 : AcpiTable::Header
//---------------------------------------------------------------------------
    {
        uint32_t            Version;
        uint32_t            Reserved;
        uint32_t            DataType;
        uint32_t            DataReserved;
        uint32_t            DataLength;
        char                ProductKey[29];


        AcpiMsdm_Type1( AcpiTable const &rBase );
    };



    std::ostream& operator<< (std::ostream& rStream, AcpiMsdm_Type1 const &rData);
    std::ostream& operator<< (std::ostream& rStream, AcpiTable::Header const &rHeader);



//---------------------------------------------------------------------------
    struct AcpiTableQuery
//---------------------------------------------------------------------------
    {
        enum ULONG 
        { 
            msDigitalLicenceId = 'MSDM', 
            msSoftwareLicenceId = 'SLIC',
            apicId = 'APIC',
            //testTableId = 'FACP',
        };


        enum 
        {
            FLAG_verbose = 0x01,
            FLAG_cmd_generate = 0x02,
        };


        typedef std::vector<DWORD> Tables;


        AcpiTableQuery();


        void dumpTables( DWORD requesteTableId, unsigned flags );
        bool hasTable( ULONG tableId );
        Tables queryTables( std::initializer_list<ULONG> const &rTableIds );


        template<typename TABLE>
        std::shared_ptr<TABLE> obtainTableCopy( ULONG tableId )
        {
            typedef std::shared_ptr<TABLE> TablePtr;

            auto swapped = _byteswap_ulong( tableId );

            TablePtr pT;

            if (hasTable(tableId))
                pT = TablePtr( new TABLE(*AcpiTable::getTable(swapped)) );

            return pT;
        }


    private:

        Tables tables;
        Tables enumerateTables();
    };







#endif
