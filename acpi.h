#include <stdint.h>
/**
* An ACPI description header
*
* This is the structure common to the start of all ACPI system
* description tables.
* Copy from iPXE project Released under GPL 2
*/
struct acpi_table_header {
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
struct acpi_MSDM_1 {
	struct acpi_table_header hdr;

	UINT Version;
	UINT Reserved;
	UINT DataType;
	UINT DataReserved;
	UINT DataLength;
	char ProductKey[29];
};
