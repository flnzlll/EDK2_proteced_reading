#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print(L"=== UEFI 基础硬件信息读取 ===\n");
  
  // 显示 UEFI 版本
  UINT32 UefiVersion = SystemTable->Hdr.Revision;
  Print(L"UEFI 版本: %d.%02d\n", UefiVersion >> 16, UefiVersion & 0xffff);

  // 获取内存映射
  EFI_MEMORY_DESCRIPTOR *MemMap = NULL;
  EFI_MEMORY_DESCRIPTOR *MemMapPtr = NULL;
  UINTN MemMapSize = 0;
  UINTN MapKey;
  UINTN DescriptorSize;
  UINT32 DescriptorVersion;

  // 第一次调用获取大小
  EFI_STATUS Status = gBS->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    MemMapSize += 2 * DescriptorSize;
    MemMap = AllocatePool(MemMapSize);
    if (MemMap == NULL) {
      Print(L"内存分配失败！\n");
      return EFI_OUT_OF_RESOURCES;
    }
    //第二次调用读取实际内容
    Status = gBS->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (EFI_ERROR(Status)) {
      Print(L"获取内存映射失败：%r\n", Status);
      FreePool(MemMap);
      return Status;
    }

    Print(L"内存映射如下：\n");
    UINTN EntryCount = MemMapSize / DescriptorSize;
    MemMapPtr = MemMap;
    for (UINTN i = 0; i < EntryCount; i++) {
      Print(L"Type: %d, PhysStart: 0x%lx, Pages: %lu\n",
            MemMapPtr->Type,
            MemMapPtr->PhysicalStart,
            MemMapPtr->NumberOfPages);
      MemMapPtr = (EFI_MEMORY_DESCRIPTOR*)((UINT8*)MemMapPtr + DescriptorSize);
    }

    FreePool(MemMap);
  } else {
    Print(L"获取内存映射失败：%r\n", Status);
    return Status;
  }

  return EFI_SUCCESS;
}
