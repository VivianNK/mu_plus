/** @file AdvancedLoggerDxeLibGoogleTest.cpp

    This file contains the unit tests for the Advanced Logger DXE Library.

    Copyright (c) Microsoft Corporation.
    SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/GoogleTestLib.h>
#include <GoogleTest/Library/MockUefiBootServicesTableLib.h>
#include <GoogleTest/Protocol/MockAdvancedLogger.h>
// #include <GoogleTest/Protocol/MockDebugPort.h> // TODO mock DebugPort Protocol
/*#include <Protocol/AdvancedLogger.h>
  #include <Library/UefiBootServicesTableLib.h>
*/

extern "C" {
  #include <Uefi.h>
  #include <Library/BaseLib.h>
  #include <Library/DebugLib.h>
  #include <PiDxe.h>
  /*#include <AdvancedLoggerInternal.h>
    #include <Protocol/DebugPort.h> // TODO mock DebugPort Protocol
    #include "../../AdvancedLoggerCommon.h"
  */

  // including the library under test for static variables
  #include "../AdvancedLoggerLib.c"
}

using namespace testing;

/**
  Test class for AdvancedLoggerWrite
**/
class AdvancedLoggerWriteTest : public Test {
protected:
  MockUefiBootServicesTableLib gBSMock;
  MockAdvancedLogger AdvLoggerProtocolMock;

  UINTN DebugLevel;
  CHAR8 *Buffer;
  UINTN NumberOfBytes;
  EFI_GUID gMockAdvLoggerProtocolGuid =
  { 0x434f695c, 0xef26, 0x4a12, { 0x9e, 0xba, 0xdd, 0xef, 0x00, 0x97, 0x49, 0x7c }
  };

  virtual void
  SetUp (
    )
  {
    CHAR8  OutputBuf[] = "MyUnitTestLog";

    NumberOfBytes          = sizeof (OutputBuf);
    Buffer                 = OutputBuf;
    DebugLevel             = DEBUG_ERROR;
    gALProtocol->Signature = ADVANCED_LOGGER_PROTOCOL_SIGNATURE;
    gALProtocol->Version   = ADVANCED_LOGGER_PROTOCOL_VERSION;
  }
};

TEST_F (AdvancedLoggerWriteTest, AdvLoggerWriteSuccess) {
  EXPECT_CALL (
    gBSMock,
    gBS_LocateProtocol (
      BufferEq (&gAdvancedLoggerProtocolGuid, sizeof (EFI_GUID)),
      Eq (nullptr), // Registration Key
      NotNull ()    // Protocol Pointer OUT
      )
    )
    .WillOnce (
       DoAll (
         SetArgPointee<2> (ByRef (gALProtocol)),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (
    AdvLoggerProtocolMock,
    gAL_AdvancedLoggerWriteProtocol (
      Pointer (gALProtocol),
      Eq (DebugLevel),
      BufferEq (Buffer, NumberOfBytes),
      Eq (NumberOfBytes)
      )
    )
    .WillOnce (
       Return ()
       );


  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytes);
}

/* Passing an invalid buffer - should be caught/handled by the protocol */
TEST_F (AdvancedLoggerWriteTest, AdvLoggerWriteInvalidBuffer) {
  mInitialized = FALSE;
  Buffer = nullptr;

  EXPECT_CALL (
    gBSMock,
    gBS_LocateProtocol (
      BufferEq (&gAdvancedLoggerProtocolGuid, sizeof (EFI_GUID)),
      Eq (nullptr), // Registration Key
      NotNull ()    // Protocol Pointer OUT
      )
    )
    .WillOnce (
       DoAll (
         SetArgPointee<2> (ByRef (gALProtocol)),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (
    AdvLoggerProtocolMock,
    gAL_AdvancedLoggerWriteProtocol (
      Pointer (gALProtocol),
      Eq (DebugLevel),
      BufferEq (Buffer, 0),
      Eq (NumberOfBytes)
      )
    )
    .WillOnce (
       Return ()
       );

  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytes);
}

/* Passing an invalid buffer - should be caught/handled by the protocol */
TEST_F (AdvancedLoggerWriteTest, AdvLoggerWriteZeroBytes) {
  mInitialized = FALSE;
  UINTN NumberOfBytesZero = 0;

  EXPECT_CALL (
    gBSMock,
    gBS_LocateProtocol (
      BufferEq (&gAdvancedLoggerProtocolGuid, sizeof (EFI_GUID)),
      Eq (nullptr), // Registration Key
      NotNull ()    // Protocol Pointer OUT
      )
    )
    .WillOnce (
       DoAll (
         SetArgPointee<2> (ByRef (gALProtocol)),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (
    AdvLoggerProtocolMock,
    gAL_AdvancedLoggerWriteProtocol (
      Pointer (gALProtocol),
      Eq (DebugLevel),
      BufferEq (Buffer, NumberOfBytes),
      Eq (NumberOfBytesZero)
      )
    )
    .WillOnce (
       Return ()
       );

  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytesZero);
}

/* Passing a mismatched signature. Asserts are disables so it will continue */
TEST_F (AdvancedLoggerWriteTest, AdvLoggerWriteFailMismatchedSignature) {
  mInitialized = FALSE;
  gALProtocol->Signature = SIGNATURE_32 ('T', 'E', 'S', 'T');

  EXPECT_CALL (
    gBSMock,
    gBS_LocateProtocol (
      _,            // Guid
      Eq (nullptr), // Registration Key
      NotNull ()    // Protocol Pointer OUT
      )
    )
    .WillOnce (
       DoAll (
         SetArgPointee<2> (ByRef (gALProtocol)),
         Return (EFI_SUCCESS)
         )
       );

  EXPECT_CALL (
    AdvLoggerProtocolMock,
    gAL_AdvancedLoggerWriteProtocol (
      Pointer (gALProtocol),
      Eq (DebugLevel),
      BufferEq (Buffer, NumberOfBytes),
      Eq (NumberOfBytes)
      )
    )
    .WillOnce (
       Return ()
       );

  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytes);
}

/* Waiting on edk2 PR to add DebugPortProtocolMock
TEST_F (AdvancedLoggerWriteTest, AdvLoggerProtocolInvalidParam) {
  EXPECT_CALL (
    gBSMock,
    gBS_LocateProtocol (
      BufferEq (&gAdvancedLoggerProtocolGuid, sizeof (EFI_GUID)),
      Eq (nullptr), // Registration Key
      NotNull ()    // Protocol Pointer OUT
      )
    )
    .WillOnce (
       Return (EFI_INVALID_PARAMETER)
       );


  EXPECT_CALL (
    gEfiDebugPortProtocolMock,
    gDP_Write (
      Pointer (gALProtocol),
      Eq (DEBUG_ERROR),
      Eq (NumberOfBytes),
      BufferEq (Buffer, NumberOfBytes)
      )
    )
    .WillOnce (
       Return ()
       );


  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytes);
}
*/
int
main (
  int   argc,
  char  *argv[]
  )
{
  InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
