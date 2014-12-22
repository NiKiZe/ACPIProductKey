// ACPIProductKey.cpp : Read MSDM license information from BIOS ACPI tables.
//

/* Copyright (C) 2014 Christian Nilsson
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

#include "stdafx.h"
#include <Windows.h>
#include "acpi.h"

CONST DWORD FirmwareTableProviderSignature_ACPI = 'ACPI';

BOOL  verbose = FALSE;
BOOL  cmd_generate = FALSE;

void showACPITable(DWORD FirmwareTableID)
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

	// TODO check BytesWritten is atleast more then needed for acpi table


	acpi_MSDM_1 *msdm = (acpi_MSDM_1*)pFirmwareTableBuffer;

	//UINT SLS_Size = BytesWritten - 36;
	//BYTE *SLS = (BYTE*)memset(malloc(SLS_Size), NULL, SLS_Size);
	char *ProductKey = (char*)memset(malloc(msdm->DataLength + 1), NULL, msdm->DataLength + 1);

	//memcpy_s(SLS, SLS_Size, (BYTE*)pFirmwareTableBuffer + 36, SLS_Size);  // TODO use this for our own copy of structure
	if (msdm->DataLength < msdm->hdr.length) memcpy_s(ProductKey, msdm->DataLength + 1, msdm->ProductKey, msdm->DataLength);

	if (verbose) {
		printf("Signature         : %4.4s\n", &msdm->hdr.signature);
		printf("Length            : %d\n", msdm->hdr.length);
		printf("Revision          : %d\n", msdm->hdr.revision);
		printf("Checksum          : %d\n", msdm->hdr.checksum);
		printf("OEMID             : %6.6s\n", &msdm->hdr.oem_id);
		printf("OEM Table ID      : %8.8s\n", &msdm->hdr.oem_table_id);
		printf("OEM Revision      : %d\n", msdm->hdr.oem_revision);
		printf("Creator ID        : %4.4s\n", &msdm->hdr.asl_compiler_id);
		printf("Creator Revision  : %d\n", msdm->hdr.asl_compiler_revision);
		printf("SLS Version       : %d\n", msdm->Version);
		printf("SLS Reserved      : %d\n", msdm->Reserved);
		printf("SLS Data Type     : %d\n", msdm->DataType);
		printf("SLS Data Reserved : %d\n", msdm->DataReserved);
		printf("SLS Data Length   : %d\n", msdm->DataLength);
		printf("Key               : %s\n", ProductKey);
	}
	else if (cmd_generate) {
		std::wcout << _T("\"%SystemRoot%\\System32\\cscript.exe\" //NoLogo \"%SystemRoot%\\System32\\slmgr.vbs\" /ipk ") << ProductKey << std::endl;
		std::wcout << _T("\"%SystemRoot%\\System32\\cscript.exe\" //NoLogo \"%SystemRoot%\\System32\\slmgr.vbs\" /ato") << std::endl;
	}
	else {
		std::wcout << ProductKey << std::endl;
	}

	free(ProductKey);
	free(pFirmwareTableBuffer);
	pFirmwareTableBuffer = NULL;

}

void enumACPITables() {
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
			showACPITable(FirmwareTableID);
		}
		pFirmwareTableID++;
	}

	free(pFirmwareTableBuffer);
	pFirmwareTableBuffer = NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
	for (int argidx = 1; argidx < argc; argidx++) {
		if (wcscmp(argv[argidx], _T("-v")) == 0)
			verbose = TRUE;
		else if (wcscmp(argv[argidx], _T("-h")) == 0 ||
			wcscmp(argv[argidx], _T("/h")) == 0 ||
			wcscmp(argv[argidx], _T("-?")) == 0 ||
			wcscmp(argv[argidx], _T("/?")) == 0 ||
			wcscmp(argv[argidx], _T("--help")) == 0 ||
			wcscmp(argv[argidx], _T("/help")) == 0) {

			std::wcout << _T("Usage for ") << argv[0] << std::endl;
			std::wcout << _T("\t\tShows Windows Product key saved on PC motherboard") << std::endl;
			std::wcout << _T("\t\tAnd possible other ACPI data.") << std::endl;
			std::wcout << _T("\t-h\tShow this help and exit") << std::endl;
			std::wcout << _T("\t-v\tDisplay more information about the process") << std::endl;
			std::wcout << _T("\t-g\tGenerate list of commands for activating with key (export to cmd file instead of using -a)") << std::endl;
			return 0;
		}
		else if (wcscmp(argv[argidx], _T("-g")) == 0)
			cmd_generate = TRUE;
		else {
			std::wcout << _T("Unknown option: ") << argv[argidx] << std::endl;
			std::wcout << _T("\tUse -h for help") << std::endl;
			return 1;
		}

	}

	enumACPITables();
#ifdef _DEBUG
	std::wcout << std::endl << std::endl << _T("Press [Enter] to exit . . .");
	fflush(stdout);
	getchar();
#endif

	return 0;
}
