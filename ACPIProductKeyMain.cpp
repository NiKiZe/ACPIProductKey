// ReadAcpi.cpp : Read MSDM license information from BIOS ACPI tables.
//

#include "stdafx.h"
#include "Windows.h"

CONST DWORD FirmwareTableProviderSignature_ACPI = 'ACPI';

void showACPITable(DWORD FirmwareTableID, BOOL verbose)
{
	PVOID pFirmwareTableBuffer = NULL;
	DWORD BufferSize = NULL;
	UINT  BytesWritten;

	// get buffer size, call with null values
	BufferSize = GetSystemFirmwareTable(FirmwareTableProviderSignature_ACPI,
		FirmwareTableID,
		NULL,
		NULL);
	// alloc memory
	pFirmwareTableBuffer = malloc(BufferSize);

	BytesWritten = GetSystemFirmwareTable(FirmwareTableProviderSignature_ACPI,
		FirmwareTableID,
		pFirmwareTableBuffer,
		BufferSize);

	/*
	Table description form Miocrosoft at: http://go.microsoft.com/fwlink/p/?LinkId=234834

	Microsoft Software Licensing Tables (SLIC and MSDM)
	http://msdn.microsoft.com/library/windows/hardware/hh673514
	Byte 			Byte
	Field							Lenght			Offset	Description
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

	BYTE *Signature = (BYTE*)memset(malloc(4 + 1), NULL, 4 + 1);
	UINT Length;
	BYTE Revision;
	BYTE Checksum;
	BYTE *OEMID = (BYTE*)memset(malloc(6 + 1), NULL, 6 + 1);
	BYTE *OEMTbleID = (BYTE*)memset(malloc(8 + 1), NULL, 8 + 1);
	BYTE OEMRev;
	BYTE *CreatorID = (BYTE*)memset(malloc(4 + 1), NULL, 4 + 1);
	UINT CreatorRev;
	UINT SLS_Size = BytesWritten - 36;
	BYTE *SLS = (BYTE*)memset(malloc(SLS_Size), NULL, SLS_Size);
	UINT SLS_Version;
	UINT SLS_Reserved;
	UINT SLS_DataType;
	UINT SLS_DataReserved;
	UINT SLS_DataLenght;
	BYTE *ProductKey = (BYTE*)memset(malloc(30 + 1), NULL, 30 + 1);


	memcpy_s(Signature, 4 + 1, (BYTE*)pFirmwareTableBuffer + 0, 4);
	Length = *(DWORD*)((BYTE*)pFirmwareTableBuffer + 4);
	Revision = *((BYTE*)pFirmwareTableBuffer + 8);
	Checksum = *((BYTE*)pFirmwareTableBuffer + 9);
	memcpy_s(OEMID, 6 + 1, (BYTE*)pFirmwareTableBuffer + 10, 6);
	memcpy_s(OEMTbleID, 8 + 1, (BYTE*)pFirmwareTableBuffer + 16, 8);
	OEMRev = *(DWORD*)((BYTE*)pFirmwareTableBuffer + 24);
	memcpy_s(CreatorID, 4 + 1, (BYTE*)pFirmwareTableBuffer + 28, 4);
	CreatorRev = *(DWORD*)((BYTE*)pFirmwareTableBuffer + 32);
	memcpy_s(SLS, SLS_Size, (BYTE*)pFirmwareTableBuffer + 36, SLS_Size);
	SLS_Version = *(DWORD*)((BYTE*)SLS);
	SLS_Reserved = *(DWORD*)((BYTE*)SLS + 4);
	SLS_DataType = *(DWORD*)((BYTE*)SLS + 8);
	SLS_DataReserved = *(DWORD*)((BYTE*)SLS + 12);
	SLS_DataLenght = *(DWORD*)((BYTE*)SLS + 16);
	memcpy_s(ProductKey, SLS_DataLenght, (BYTE*)SLS + 20, SLS_DataLenght);

	if (verbose) {
		printf("Signature         : %s\n", Signature);
		printf("Length            : %d\n", Length);
		printf("Revision          : %d\n", Revision);
		printf("Checksum          : %d\n", Checksum);
		printf("OEMID             : %s\n", OEMID);
		printf("OEM Table ID      : %s\n", OEMTbleID);
		printf("OEM Revision      : %d\n", OEMRev);
		printf("Creator ID        : %s\n", CreatorID);
		printf("Creator Revision  : %d\n", CreatorRev);
		printf("SLS Version       : %d\n", SLS_Version);
		printf("SLS Reserved      : %d\n", SLS_Reserved);
		printf("SLS Data Type     : %d\n", SLS_DataType);
		printf("SLS Data Reserved : %d\n", SLS_DataReserved);
		printf("SLS Data Lenght   : %d\n", SLS_DataLenght);
		printf("Key               : %s\n", ProductKey);
	}
	else {
		printf("%s", ProductKey);
	}

	free(pFirmwareTableBuffer);
	pFirmwareTableBuffer = NULL;

}

void enumACPITables(BOOL verbose) {
	PVOID pFirmwareTableBuffer = NULL;
	DWORD BufferSize = NULL;
	UINT  BytesWritten;
	DWORD FirmwareTableID;
	DWORD *pFirmwareTableID;

	// get buffer size, call with null values
	BufferSize = EnumSystemFirmwareTables(FirmwareTableProviderSignature_ACPI,
		NULL,
		NULL);

	// alloc memory
	pFirmwareTableBuffer = malloc(BufferSize);

	// enum acpi tables
	BytesWritten = EnumSystemFirmwareTables(FirmwareTableProviderSignature_ACPI,
		pFirmwareTableBuffer,
		BufferSize);

	// enumerate ACPI tables, look for MSDM table
	pFirmwareTableID = (DWORD*)pFirmwareTableBuffer;
	const DWORD SearchMSDMFirmwareTableID = _byteswap_ulong('MSDM');

	if (verbose) printf("Found ACPI Tables: ");
	for (UINT i = 0; i < BytesWritten / 4; i++) // 4 == bytesize of DWORD
	{
		FirmwareTableID = *pFirmwareTableID;
		if (verbose) printf("%.*s ", 4, pFirmwareTableID);
		if (FirmwareTableID == SearchMSDMFirmwareTableID) {
			if (verbose) printf("\n");
			showACPITable(FirmwareTableID, verbose);
		}
		pFirmwareTableID++;
	}

	free(pFirmwareTableBuffer);
	pFirmwareTableBuffer = NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL  verbose = FALSE;

	if (argc > 1) {
		if (wcscmp(argv[1], _T("-v")) == 0)
			verbose = TRUE;
	}

	enumACPITables(verbose);

#ifdef _DEBUG
	printf("\n\nPress [Enter] to exit . . .");
	fflush(stdout);
	getchar();
#endif

	return 0;
}
