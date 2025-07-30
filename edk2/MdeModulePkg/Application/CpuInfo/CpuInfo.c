#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

STATIC
VOID
Cpuid(UINT32 EaxIn, UINT32 EcxIn, UINT32 *Regs)
{
  __asm__ __volatile__ (
    "cpuid"
    : "=a" (Regs[0]), "=b" (Regs[1]), "=c" (Regs[2]), "=d" (Regs[3])
    : "a" (EaxIn), "c" (EcxIn)
  );
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  UINT32 Regs[4];
  CHAR8 VendorId[13] = {0};

  // 获取最大支持的 CPUID 功能号和 Vendor ID
  Cpuid(0, 0, Regs);
  UINT32 MaxFunction = Regs[0];
  *(UINT32*)&VendorId[0] = Regs[1]; // EBX
  *(UINT32*)&VendorId[4] = Regs[3]; // EDX
  *(UINT32*)&VendorId[8] = Regs[2]; // ECX

  Print(L"CPU Vendor ID: %a\n", VendorId);
  Print(L"Max Supported CPUID Function: 0x%x\n", MaxFunction);

  // 获取处理器信息（型号、系列等）
  Cpuid(1, 0, Regs);
  UINT8 Stepping = Regs[0] & 0xF;
  UINT8 Model    = (Regs[0] >> 4) & 0xF;
  UINT8 Family   = (Regs[0] >> 8) & 0xF;
  UINT8 ProcessorType = (Regs[0] >> 12) & 0x3;
  UINT8 ExtModel = (Regs[0] >> 16) & 0xF;
  UINT8 ExtFamily = (Regs[0] >> 20) & 0xFF;

  UINT16 DisplayFamily = Family;
  UINT16 DisplayModel = Model;

  if (Family == 6 || Family == 15) {
    DisplayModel += (ExtModel << 4);
  }
  if (Family == 15) {
    DisplayFamily += ExtFamily;
  }

  Print(L"CPU Type: %u\n", ProcessorType);
  Print(L"CPU Family: %u\n", DisplayFamily);
  Print(L"CPU Model: %u\n", DisplayModel);
  Print(L"CPU Stepping: %u\n", Stepping);

  return EFI_SUCCESS;
}
