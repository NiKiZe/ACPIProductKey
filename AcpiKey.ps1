# based on https://gist.github.com/lwalthert/fe52f7fa98b4ea491345a0518750baa9
# and https://github.com/NiKiZe/ACPIProductKey/blob/master/ACPIProductKeyMain.cpp

$id = get-random
$CSCode = @"
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

public static class MsdmHelper$id {

[DllImport("kernel32")]
private static extern uint EnumSystemFirmwareTables (uint FirmwareTableProviderSignature, IntPtr pFirmwareTableBuffer, uint BufferSize);
[DllImport("kernel32")]
private static extern uint GetSystemFirmwareTable   (uint FirmwareTableProviderSignature, uint FirmwareTableID, IntPtr pFirmwareTableBuffer, uint BufferSize);

private delegate uint GetterDlg(IntPtr pFirmwareTableBuffer, uint BufferSize);
private static byte[] GetDataHelper(GetterDlg getter) {
    uint bSize = getter(IntPtr.Zero, 0);
    IntPtr FirmwareTableBuffer = Marshal.AllocHGlobal((int)bSize);
    var buffer = new byte[bSize];
    getter(FirmwareTableBuffer, bSize);
    Marshal.Copy(FirmwareTableBuffer, buffer, 0, buffer.Length);
    Marshal.FreeHGlobal(FirmwareTableBuffer);
    return buffer;
}

public static string GetMSDM()
{
    const uint firmwareTableProvider = 'A' << 24 | 'C' << 16 | 'P' << 8 | 'I';
    var buffer = GetDataHelper((p, s) => EnumSystemFirmwareTables(firmwareTableProvider, p, s));
    string str = Encoding.ASCII.GetString(buffer);

    for (int i = 0; i <= str.Length - 1; i += 4)
    {
        if (str.Substring(i, 4) != "MSDM")
            continue;

        const uint firmwareTableMsdm = 'M' << 24 | 'D' << 16 | 'S' << 8 | 'M'; // 0x4d44534d; // 
        buffer = GetDataHelper((p, s) => GetSystemFirmwareTable(firmwareTableProvider, firmwareTableMsdm, p, s));
        return Encoding.ASCII.GetString(buffer, 56, 29);
    }
    return null;
}
}
"@

# the delegate injects so we get a list, and not just one item https://stackoverflow.com/questions/65021670
$MSDMHelper = (Add-Type -TypeDefinition $CSCode -Language CSharp -PassThru)[0]
$key = $MSDMHelper::GetMSDM()

if ($key -notLike $null) {
  Write-Host "Trying to install found MSDM key $key"
  cscript //Nologo c:\Windows\System32\slmgr.vbs /ipk $key | Add-content $Logfile
  if ($?)
  {
    cscript //Nologo c:\Windows\System32\slmgr.vbs /ato | Add-content $Logfile
  }
}
