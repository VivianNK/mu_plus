## @file
# Host Test DSC for the Advanced Logger Package
#
# Copyright (c) Microsoft Corporation
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

################################################################################
[Defines]
  PLATFORM_NAME                  = AdvLoggerPkgHostTest
  PLATFORM_GUID                  = 1BF313CF-F093-42A9-8676-3445F2544295
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/AdvLoggerPkg/HostTest
  SUPPORTED_ARCHITECTURES        = IA32|X64
  SKUID_IDENTIFIER               = DEFAULT
  BUILD_TARGETS                  = NOOPT

!include UnitTestFrameworkPkg/UnitTestFrameworkPkgHost.dsc.inc

[PcdsPatchableInModule]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x17

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses.common.HOST_APPLICATION]
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  DebugLib|AdvLoggerPkg/Library/BaseDebugLibAdvancedLogger/BaseDebugLibAdvancedLogger.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf

  #
  # Mocked Libs
  #
  UefiRuntimeLib|MdePkg/Test/Mock/Library/GoogleTest/MockUefiRuntimeLib/MockUefiRuntimeLib.inf
  MemoryAllocationLib|MdePkg/Test/Mock/Library/GoogleTest/MockMemoryAllocationLib/MockMemoryAllocationLib.inf
  UefiBootServicesTableLib|MdePkg/Test/Mock/Library/GoogleTest/MockUefiBootServicesTableLib/MockUefiBootServicesTableLib.inf

[LibraryClasses.X64]
  AssertLib|AdvLoggerPkg/Library/AssertLib/AssertLib.inf

################################################################################
#
# Components section - list of all Components needed by this Platform.
#
################################################################################
[Components]
  #
  # Build HOST_APPLICATIONs that test AdvLoggerPkg
  #
  AdvLoggerPkg/AdvLoggerOsConnectorPrm/Library/AdvLoggerOsConnectorPrmConfigLib/GoogleTest/AdvLoggerPrmConfigLibGoogleTest.inf{
    <LibraryClasses>
      AdvancedLoggerLib|AdvLoggerPkg/Library/AdvancedLoggerLib/Dxe/AdvancedLoggerLib.inf
  }
  AdvLoggerPkg/AdvLoggerOsConnectorPrm/GoogleTest/AdvLoggerOsConnectorPrmGoogleTest.inf {
    <LibraryClasses>
      AdvancedLoggerLib|AdvLoggerPkg/Library/AdvancedLoggerLib/Dxe/AdvancedLoggerLib.inf
  }
  AdvLoggerPkg/Library/AdvancedLoggerLib/Dxe/GoogleTest/AdvancedLoggerDxeLibGoogleTest.inf {
    <LibraryClasses>
      # TODO try a null lib?
      AdvancedLoggerLib|AdvLoggerPkg/Library/AdvancedLoggerLib/Dxe/AdvancedLoggerLib.inf
  }
