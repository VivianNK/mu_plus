/** @file -- EventAuditApp.c
This Shell App writes event information to SFS.

Copyright (c) Microsoft Corporation.
SPDX-License-Identifier: BSD-2-Clause-Patent
**/

// in UefiTestingPkg, need to go out two levels (MU->Common->) then go down to MU_BASECORE
// that's why need protocol. but wit the protocol is in MU_BASECORE\MdeModule package as well..
// #include <MU_BASECORE\MdeModulePkg\Core\Dxe\Event\Event.h>
#include <Uefi.h>

#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EventAudit.h>

/**
  SmmPagingAuditAppEntryPoint

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point executed successfully.
  @retval other           Some error occurred when executing this entry point.

**/

EVENT_AUDIT_PROTOCOL  *mEventAuditProtocol;
EFI_FILE              *mFs_Handle;

EFI_STATUS
EFIAPI
EventAuditAppEntryPoint (
  IN     EFI_HANDLE        ImageHandle,
  IN     EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  DEBUG ((DEBUG_ERROR, "%a entered - %r\n", __FUNCTION__, Status));

  Status = DumpaEventInfo ();

  DEBUG ((DEBUG_ERROR, "%a leave - %r\n", __FUNCTION__, Status));

  return EFI_SUCCESS;
} // PagingAuditDxeAppEntryPoint()


/**

  Opens the SFS volume and if successful, returns a FS handle to the opened volume.

  @param    mFs_Handle       Handle to the opened volume.

  @retval   EFI_SUCCESS     The FS volume was opened successfully.
  @retval   Others          The operation failed.

**/
EFI_STATUS
OpenVolumeSFS (
  OUT EFI_FILE  **Fs_Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  BOOLEAN                          Found;
  EFI_HANDLE                       Handle;
  EFI_HANDLE                       *HandleBuffer;
  UINTN                            Index;
  UINTN                            NumHandles;
  EFI_DEVICE_PATH_PROTOCOL         *OrigDevicePath;
  EFI_STRING                       PathNameStr;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *SfProtocol;
  EFI_STATUS                       Status;

  Status       = EFI_SUCCESS;
  SfProtocol   = NULL;
  NumHandles   = 0;
  HandleBuffer = NULL;

  //
  // Locate all handles that are using the SFS protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status) != FALSE) {
    DEBUG ((DEBUG_ERROR, "%a: failed to locate all handles using the Simple FS protocol (%r)\n", __FUNCTION__, Status));
    goto CleanUp;
  }

  //
  // Search the handles to find one that is on a GPT partition on a hard drive.
  //
  Found = FALSE;
  for (Index = 0; (Index < NumHandles) && (Found == FALSE); Index += 1) {
    DevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    if (DevicePath == NULL) {
      continue;
    }

    //
    // Save the original device path because we change it as we're checking it
    // below. We'll need the unmodified version if we determine that it's good.
    //
    OrigDevicePath = DevicePath;

    //
    // Convert the device path to a string to print it.
    //
    PathNameStr = ConvertDevicePathToText (DevicePath, TRUE, TRUE);
    DEBUG ((DEBUG_ERROR, "%a: device path %d -> %s\n", __FUNCTION__, Index, PathNameStr));

    //
    // Check if this is a block IO device path. If it is not, keep searching.
    // This changes our locate device path variable, so we'll have to restore
    // it afterwards.
    //
    Status = gBS->LocateDevicePath (
                    &gEfiBlockIoProtocolGuid,
                    &DevicePath,
                    &Handle
                    );

    if (EFI_ERROR (Status) != FALSE) {
      DEBUG ((DEBUG_ERROR, "%a: not a block IO device path\n", __FUNCTION__));
      continue;
    }

    //
    // Restore the device path and check if this is a GPT partition. We only
    // want to write our log on GPT partitions.
    //
    DevicePath = OrigDevicePath;
    while (IsDevicePathEnd (DevicePath) == FALSE) {
      //
      // If the device path is not a hard drive, we don't want it.
      //
      if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
          (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP))
      {
        //
        // Check if this is a gpt partition. If it is, we'll use it. Otherwise,
        // keep searching.
        //
        if ((((HARDDRIVE_DEVICE_PATH *)DevicePath)->MBRType == MBR_TYPE_EFI_PARTITION_TABLE_HEADER) &&
            (((HARDDRIVE_DEVICE_PATH *)DevicePath)->SignatureType == SIGNATURE_TYPE_GUID))
        {
          DevicePath = OrigDevicePath;
          Found      = TRUE;
          break;
        }
      }

      //
      // Still searching. Advance to the next device path node.
      //
      DevicePath = NextDevicePathNode (DevicePath);
    }

    //
    // If we found a good device path, stop searching.
    //
    if (Found != FALSE) {
      DEBUG ((DEBUG_ERROR, "%a: found GPT partition Index:%d\n", __FUNCTION__, Index));
      break;
    }
  }

  //
  // If a suitable handle was not found, return error.
  //
  if (Found == FALSE) {
    Status = EFI_NOT_FOUND;
    goto CleanUp;
  }

  Status = gBS->HandleProtocol (
                  HandleBuffer[Index],
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **)&SfProtocol
                  );

  if (EFI_ERROR (Status) != FALSE) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to locate Simple FS protocol using the handle to fs0: %r \n", __FUNCTION__, Status));
    goto CleanUp;
  }

  //
  // Open the volume/partition.
  //
  Status = SfProtocol->OpenVolume (SfProtocol, Fs_Handle);
  if (EFI_ERROR (Status) != FALSE) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to open Simple FS volume fs0: %r \n", __FUNCTION__, Status));
    goto CleanUp;
  }

CleanUp:
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  return Status;
}

/**
  Creates a new file and writes the contents of the caller's data buffer to the file.

  @param    Fs_Handle           Handle to an opened filesystem volume/partition.
  @param    FileName            Name of the file to create.
  @param    DataBufferSize      Size of data to buffer to be written in bytes.
  @param    Data                Data to be written.

  @retval   EFI_STATUS          File was created and data successfully written.
  @retval   Others              The operation failed.

**/
EFI_STATUS
CreateAndWriteFileSFS (
  IN EFI_FILE  *Fs_Handle,
  IN CHAR16    *FileName,
  IN UINTN     DataBufferSize,
  IN VOID      *Data
  )
{
  EFI_STATUS  Status      = EFI_SUCCESS;
  EFI_FILE    *FileHandle = NULL;

  DEBUG ((DEBUG_ERROR, "%a: Creating file: %s \n", __FUNCTION__, FileName));

  // Create the file with RW permissions.
  //
  Status = Fs_Handle->Open (
                        Fs_Handle,
                        &FileHandle,
                        FileName,
                        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                        0
                        );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to create file %s: %r !\n", __FUNCTION__, FileName, Status));
    goto CleanUp;
  }

  // Write the contents of the caller's data buffer to the file.
  //
  Status = FileHandle->Write (
                         FileHandle,
                         &DataBufferSize,
                         Data
                         );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to write to file %s: %r !\n", __FUNCTION__, FileName, Status));
    goto CleanUp;
  }

  FileHandle->Flush (Fs_Handle);

CleanUp:

  // Close the file if it was successfully opened.
  //
  if (FileHandle != NULL) {
    FileHandle->Close (FileHandle);
  }

  return Status;
}


/**
  Creates a new file and writes the contents of the caller's data buffer to the file.

  @param    Fs_Handle           Handle to an opened filesystem volume/partition.
  @param    FileName            Name of the file to create.
  @param    DataBufferSize      Size of data to buffer to be written in bytes.
  @param    Data                Data to be written.

  @retval   EFI_STATUS          File was created and data successfully written.
  @retval   Others              The operation failed.

**/
EFI_STATUS
CreateAndWriteFileSFS (
  IN EFI_FILE  *Fs_Handle,
  IN CHAR16    *FileName,
  IN UINTN     DataBufferSize,
  IN VOID      *Data
  )
{
  EFI_STATUS  Status      = EFI_SUCCESS;
  EFI_FILE    *FileHandle = NULL;

  DEBUG ((DEBUG_ERROR, "%a: Creating file: %s \n", __FUNCTION__, FileName));

  // Create the file with RW permissions.
  //
  Status = Fs_Handle->Open (
                        Fs_Handle,
                        &FileHandle,
                        FileName,
                        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                        0
                        );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to create file %s: %r !\n", __FUNCTION__, FileName, Status));
    goto CleanUp;
  }

  // Write the contents of the caller's data buffer to the file.
  //
  Status = FileHandle->Write (
                         FileHandle,
                         &DataBufferSize,
                         Data
                         );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Failed to write to file %s: %r !\n", __FUNCTION__, FileName, Status));
    goto CleanUp;
  }

  FileHandle->Flush (Fs_Handle);

CleanUp:

  // Close the file if it was successfully opened.
  //
  if (FileHandle != NULL) {
    FileHandle->Close (FileHandle);
  }

  return Status;
}


/**
  todo

**/
EFI_STATUS
DumpEventInfo (
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN  EventCount  = 0;
  //  *Pte1GEntries = NULL;
  CHAR8       TempString[MAX_STRING_SIZE];

  Status = gBS->LocateProtocol (&gEventAuditProtocolGuid, NULL, (VOID **)&mEventAuditProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a error finding event audit protocol - %r\n", __FUNCTION__, Status));
    return EFI_ERROR;
  }

  Status = OpenVolumeSFS (&mFs_Handle);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a error opening sfs volume - %r\n", __FUNCTION__, Status));
    return EFI_ERROR;
  }

  CreateAndWriteFileSFS (mFs_Handle, L"eventInfo.csv", Pte1GCount * sizeof (PAGE_TABLE_1G_ENTRY), Pte1GEntries);


Cleanup:

  DEBUG ((DEBUG_ERROR, "%a leave - %r\n", __FUNCTION__, Status));
}