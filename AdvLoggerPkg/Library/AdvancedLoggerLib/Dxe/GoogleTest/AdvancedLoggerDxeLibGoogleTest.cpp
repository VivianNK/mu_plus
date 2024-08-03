/** @file AdvancedLoggerDxeLibGoogleTest.cpp

    This file contains the unit tests for the Advanced Logger DXE Library.

    Copyright (c) Microsoft Corporation.
    SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/GoogleTestLib.h>
#include <GoogleTest/Library/MockUefiBootServicesTableLib.h>
#include <GoogleTest/Protocol/MockAdvancedLogger.h>
// #include <GoogleTest/Protocol/MockDebugPort.h> // TODO mock DebugPort Protocol

extern "C" {
  #include <Uefi.h>
  #include <Library/BaseLib.h>
  #include <Library/DebugLib.h>
  #include <PiDxe.h>
  #include <AdvancedLoggerInternal.h>
  #include <Protocol/DebugPort.h> // TODO mock DebugPort Protocol
  #include "../../AdvancedLoggerCommon.h"
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

  // Setup per TEST_F
  virtual void
  SetUp (
    )
  {
    CHAR8  OutputBuf[] = "MyUnitTestLog";

    NumberOfBytes = sizeof (OutputBuf);
    Buffer        = OutputBuf;
    DebugLevel    = DEBUG_ERROR; // TODO Q: are there any Debug levels that would not be okay/dont exist?

    /* TODO test different signatures and versions
    gALProtocol->Signature = ADVANCED_LOGGER_PROTOCOL_SIGNATURE;
    gALProtocol->Version   = ADVANCED_LOGGER_PROTOCOL_VERSION;
    */
  }
};

TEST_F (AdvancedLoggerWriteTest, AdvLoggerWriteSuccess) {
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
      Eq (DEBUG_ERROR),
      BufferEq (Buffer, NumberOfBytes),
      Eq (NumberOfBytes)
      )
    )
    .WillOnce (
       Return ()
       );

  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytes);
}

/*
TEST_F (AdvancedLoggerWriteTest, AdvLoggerWriteFailMismatchedSignature) {
  gALProtocol->Signature = SIGNATURE_32('T','E','S','T');

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

  // TODO expect assert
  AdvancedLoggerWrite (DebugLevel, Buffer, NumberOfBytes);
}
*/

// TODO test with invalid parameters:
//    NULL Buffer
//    0 NumberOfBytes

// TODO Test with gBS->LocateProtocol returning EFI_NOT_FOUND
//    Requires MockDebugPort.h

int
main (
  int   argc,
  char  *argv[]
  )
{
  InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
