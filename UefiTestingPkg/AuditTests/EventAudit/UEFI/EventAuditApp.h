/** @file -- EventAuditApp.h

T

Copyright (c) Microsoft Corporation. All rights reserved.
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _EVENT_AUDIT_APP_H_
#define _EVENT_AUDIT_APP_H_

#include <Protocol/EventAudit.h>
#include <Library/DevicePathLib.h>

/**

  Opens the SFS volume and if successful, returns a FS handle to the opened volume.

  @param    mFs_Handle       Handle to the opened volume.

  @retval   EFI_SUCCESS     The FS volume was opened successfully.
  @retval   Others          The operation failed.

**/
EFI_STATUS
OpenVolumeSFS (
  OUT EFI_FILE  **Fs_Handle
  );


/**
  DumpEventInfo

  @retval EFI_STATUS  The list of event info items were written to a file in SFS successfully.
  @retval other       Some error occurred.
**/
EFI_STATUS
EFIAPI
DumpEventInfo (
  );


/**
 * @brief      Writes a buffer to file.
 *
 * @param      FileName     The name of the file being written to.
 * @param      Buffer       The buffer to write to file.
 * @param[in]  BufferSize   Size of the buffer.
 * @param[in]  WriteCount   Number to append to the end of the file.
 */
VOID
EFIAPI
WriteBufferToFile (
  IN CONST CHAR16  *FileName,
  IN       VOID    *Buffer,
  IN       UINTN   BufferSize
  );

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
  );

#endif // _EVENT_AUDIT_APP_H_