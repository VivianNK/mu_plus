#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#define EFI_ERROR 1
#define CONS_ERROR 2
#define FILE_ERROR 3
#define SUCCESS 0
#define MAX_VAR_LENGTH 1024*1024

typedef unsigned short UINT16;

typedef unsigned char BOOLEAN;

///
/// 1-byte Character
///
typedef char CHAR8;
///
/// 8-byte unsigned value
///
typedef unsigned long long UINT64;
///
/// 4-byte unsigned value
///
typedef unsigned int UINT32;
///
/// 2-byte signed value
///
typedef short INT16;
typedef unsigned char       UINT8;
typedef char                CHAR8;
typedef UINT64 EFI_PHYSICAL_ADDRESS;

#pragma pack(push, 1)
typedef struct{
    UINT16    Year;              // 1900 - 9999
    UINT8     Month;             // 1 - 12
    UINT8     Day;               // 1 - 31
    UINT8     Hour;              // 0 - 23
    UINT8     Minute;            // 0 - 59
    UINT8     Second;            // 0 - 59
    UINT8     Pad1;
    UINT32    Nanosecond;        // 0 - 999,999,999
    INT16     TimeZone;          // —1440 to 1440 or 2047
    UINT8     Daylight;
    UINT8     Pad2;
}   EFI_TIME;
#pragma pack(pop)

/*************************************************************************************/
#pragma pack(push, 1)
typedef volatile struct {
    UINT32                Signature;           //4  
    UINT16                Version;            //6  // Current Version
    UINT16                Reserved;           //8  // Reserved for future
    EFI_PHYSICAL_ADDRESS  LogBuffer;          //16  // Fixed pointer to start of log
    EFI_PHYSICAL_ADDRESS  LogCurrent;         //24  // Where to store next log entry.
    UINT32                DiscardedSize;      //28  // Number of bytes of messages missed
    UINT32                LogBufferSize;      //32  // Size of allocated buffer
    BOOLEAN               InPermanentRAM;     //33  // Log in permanent RAM
    BOOLEAN               AtRuntime;          //34  // After ExitBootServices
    BOOLEAN               GoneVirtual;        //35  // After VirtualAddressChage
    BOOLEAN               HdwPortInitialized; //36  // HdwPort initialized
    BOOLEAN               HdwPortDisabled;    //37  // HdwPort is Disabled
    BOOLEAN               Reserved2[3];       //40  //
    UINT64                TimerFrequency;     //48  // Ticks per second for log timing
    UINT64                TicksAtTime;        //56  // Ticks when Time Acquired
    EFI_TIME              Time;               //72  // Uefi Time Field
} HYBRID_ADVANCED_LOGGER_INFO;
#pragma pack(pop)

/*************************************************************************************/

// Developement Log Version(pre versioning) - V0
#pragma pack(push, 1)
typedef volatile struct {
    UINT32                Signature;
    UINT32                LogBufferSize;
    EFI_PHYSICAL_ADDRESS  LogBuffer;              // Fixed pointer to start of log
    EFI_PHYSICAL_ADDRESS  LogCurrent;             // Where to store next log entry.
    UINT32                DiscardedSize;          // Number of bytes of messages missed
    BOOLEAN               SerialInitialized;      // Serial port initialized
    BOOLEAN               InPermanentRAM;         // Log in permanent RAM
    BOOLEAN               ExitBootServices;       // Exit Boot Services occurred
    BOOLEAN               PeiAllocated;           // Pei allocated "Temp Ram"
} ADVANCED_LOGGER_INFO_V0;
#pragma pack(pop)


    //  Heuristic for V1 decoding :
    //  1. Version will appear as 0 as it is in the Low order 16 bits for the LogBufferSize.
    //   2. LogBuffer will be a constant value of BaseAddress + 32

#define V0_LOGGER_INFO_SIZE 32
#define V0_LOGGER_INFO_VERSION 0



/*************************************************************************************/


// First Supported Log with versioning - V1
#pragma pack(push, 1)
typedef volatile struct {
    UINT32                Signature;              // Signature 'ALOG'
    UINT16                Version;                // Current Version
    UINT16                Reserved;               // Reserved for future
    EFI_PHYSICAL_ADDRESS  LogBuffer;              // Fixed pointer to start of log
    EFI_PHYSICAL_ADDRESS  LogCurrent;             // Where to store next log entry.
    UINT32                DiscardedSize;          // Number of bytes of messages missed
    UINT32                LogBufferSize;          // Size of allocated buffer
    BOOLEAN               InPermanentRAM;         // Log in permanent RAM
    BOOLEAN               AtRuntime;              // After ExitBootServices
    BOOLEAN               GoneVirtual;            // After VirtualAddressChange
    BOOLEAN               Reserved2[5];           //
    UINT64                TimerFrequency;         // Ticks per second for log timing
} ADVANCED_LOGGER_INFO_V1;
#pragma pack(pop)

#define V1_LOGGER_INFO_SIZE 48
#define V1_LOGGER_INFO_VERSION 1

/*************************************************************************************/

// Second Supported Log with versioning - V2
#pragma pack(push, 1)
typedef volatile struct {
    UINT32                Signature;              //4  // Signature 'ALOG'
    UINT16                Version;            //6  // Current Version
    UINT16                Reserved;           //8  // Reserved for future
    EFI_PHYSICAL_ADDRESS  LogBuffer;          //16  // Fixed pointer to start of log
    EFI_PHYSICAL_ADDRESS  LogCurrent;         //24  // Where to store next log entry.
    UINT32                DiscardedSize;      //28  // Number of bytes of messages missed
    UINT32                LogBufferSize;      //32  // Size of allocated buffer
    BOOLEAN               InPermanentRAM;     //33  // Log in permanent RAM
    BOOLEAN               AtRuntime;          //34  // After ExitBootServices
    BOOLEAN               GoneVirtual;        //35  // After VirtualAddressChage
    BOOLEAN               HdwPortInitialized; //36  // HdwPort initialized
    BOOLEAN               HdwPortDisabled;    //37  // HdwPort is Disabled
    BOOLEAN               Reserved2[3];       //40  //
    UINT64                TimerFrequency;     //48  // Ticks per second for log timing
    UINT64                TicksAtTime;        //56  // Ticks when Time Acquired
    EFI_TIME              Time;               //72  // Uefi Time Field
} ADVANCED_LOGGER_INFO_V2;
#pragma pack(pop)

#define V2_LOGGER_INFO_SIZE 72
#define V2_LOGGER_INFO_VERSION 2

/*************************************************************************************/

// Third Supported Log with versioning - V3
#pragma pack(push, 1)
typedef volatile struct {
     UINT32                  Signature;              // Signature 'ALOG'
     UINT16                  Version;                // Current Version
     UINT16                  Reserved;               // Reserved for future
     EFI_PHYSICAL_ADDRESS    LogBuffer;              // Fixed pointer to start of log
     EFI_PHYSICAL_ADDRESS    LogCurrent;             // Where to store next log entry.
     UINT32                  DiscardedSize;          // Number of bytes of messages missed
     UINT32                  LogBufferSize;          // Size of allocated buffer
     BOOLEAN                 InPermanentRAM;         // Log in permanent RAM
     BOOLEAN                 AtRuntime;              // After ExitBootServices
     BOOLEAN                 GoneVirtual;            // After VirtualAddressChange
     BOOLEAN                 HdwPortInitialized;     // HdwPort initialized
     BOOLEAN                 HdwPortDisabled;        // HdwPort is Disabled
     BOOLEAN                 Reserved2[3];           //
     UINT64                  TimerFrequency;         // Ticks per second for log timing
     UINT64                  TicksAtTime;            // Ticks when Time Acquired
     EFI_TIME                Time;                   // Uefi Time Field
     UINT32                  HwPrintLevel;           // Logging level to be printed at hw port
     UINT32                  Reserved3;              //
 } ADVANCED_LOGGER_INFO_V3;
#pragma pack(pop)

#define V3_LOGGER_INFO_SIZE 80
#define V3_LOGGER_INFO_VERSION 3

/*************************************************************************************/

// V4 and V3 are the same definition, just an indicator for single firmware systems that
// starting from V4, all the message will have v2 message entry format.
#define V4_LOGGER_INFO_SIZE = V3_LOGGER_INFO_SIZE
#define V4_LOGGER_INFO_VERSION = 4
/*************************************************************************************/
#pragma pack(push, 1)
typedef struct {
    UINT32                Signature;              // Signature
    UINT32                DebugLevel;             // Debug Level
    UINT64                TimeStamp;              // Time stamp
    UINT16                MessageLen;             // Number of bytes in Message
    CHAR8                 MessageText[];          // Message Text
 } ADVANCED_LOGGER_MESSAGE_ENTRY_V1;
#pragma pack(pop)

#define MESSAGE_ENTRY_SIZE 18
#define MAX_MESSAGE_SIZE 512


/*************************************************************************************/
#pragma pack(push, 1)
typedef struct {
    UINT32    Signature;                            // Signature
    UINT8     MajorVersion;                         // Major version of advanced logger message structure
    UINT8     MinorVersion;                         // Minor version of advanced logger message structure
    UINT32    DebugLevel;                           // Debug Level
    UINT64    TimeStamp;                            // Time stamp
    UINT16    Phase;                                // Boot phase that produced this message entry
    UINT16    MessageLen;                           // Number of bytes in Message
    UINT16    MessageOffset;                        // Offset of Message from start of structure,
                                                    // used to calculate the address of the Message
    CHAR8     MessageText[];                        // Message Text
 } ADVANCED_LOGGER_MESSAGE_ENTRY_V2;
#pragma pack(pop)

#define MESSAGE_ENTRY_SIZE_V2 24
#define ADVANCED_LOGGER_PHASE_UNSPECIFIED 0
#define ADVANCED_LOGGER_PHASE_SEC 1
#define ADVANCED_LOGGER_PHASE_PEI 2
#define ADVANCED_LOGGER_PHASE_PEI64 3
#define ADVANCED_LOGGER_PHASE_DXE 4
#define ADVANCED_LOGGER_PHASE_RUNTIME 5
#define ADVANCED_LOGGER_PHASE_MM_CORE 6 
#define ADVANCED_LOGGER_PHASE_MM 7
#define ADVANCED_LOGGER_PHASE_SMM_CORE 8
#define ADVANCED_LOGGER_PHASE_SMM 9
#define ADVANCED_LOGGER_PHASE_TFA 10
#define ADVANCED_LOGGER_PHASE_CNT 11

//PHASE_STRING_LIST = ["[UNSPECIFIED] ", "[SEC] ", "[PEI] ", "[PEI64] ",
//"[DXE] ", "[RUNTIME] ", "[MM_CORE] ", "[MM] ",
//"[SMM_CORE] ", "[SMM] ", "[TFA] "]
/*************************************************************************************/
