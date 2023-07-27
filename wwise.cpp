#include <emscripten.h>
#include <emscripten/bind.h>
#include <AK/SoundEngine/Common/AkSoundEngineExport.h>
#include <AK/SoundEngine/Common/AkNumeralTypes.h>
#include <AK/SoundEngine/Platforms/POSIX/AkTypes.h>
#include <AK/SoundEngine/Platforms/Emscripten/AkTypes.h>
#include <AK/AkPlatforms.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkModule.h>

#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <SoundEngine/Common/AkFilePackage.h>
#include <SoundEngine/POSIX/AkFileHelpers.h>
#include <SoundEngine/POSIX/AkFilePackageLowLevelIOBlocking.h>
#include <SoundEngine/POSIX/stdafx.h>

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

using namespace emscripten;

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

EMSCRIPTEN_BINDINGS(my_module) {
  enum_<AKRESULT>("AKRESULT")
    .value("AK_NotImplemented", AK_NotImplemented)
    .value("AK_Success", AK_Success)
    .value("AK_Failure", AK_Fail)
    .value("AK_PartialSuccess", AK_PartialSuccess)
    .value("AK_NotCompatible", AK_NotCompatible)	///< Incompatible formats
    .value("AK_AlreadyConnected", AK_AlreadyConnected)	///< The stream is already connected to another node.
    .value("AK_InvalidFile", AK_InvalidFile)	///< The provided file is the wrong format or unexpected values causes the file to be invalid.
    .value("AK_AudioFileHeaderTooLarge", AK_AudioFileHeaderTooLarge) ///< The file header is too large.
    .value("AK_MaxReached", AK_MaxReached) 	///< The maximum was reached.
    .value("AK_InvalidID", AK_InvalidID)	///< The ID is invalid.
    .value("AK_IDNotFound", AK_IDNotFound)	///< The ID was not found.
    .value("AK_InvalidInstanceID", AK_InvalidInstanceID) ///< The InstanceID is invalid.
    .value("AK_NoMoreData", AK_NoMoreData)	///< No more data is available from the source.
    .value("AK_InvalidStateGroup", AK_InvalidStateGroup) ///< The StateGroup is not a valid channel.
    .value("AK_ChildAlreadyHasAParent", AK_ChildAlreadyHasAParent) //< The child already has a parent.
    .value("AK_InvalidLanguage", AK_InvalidLanguage) 	///< The language is invalid (applies to the Low-Level I/O).
    .value("AK_CannotAddItseflAsAChild", AK_CannotAddItseflAsAChild) //< It is not possible to add itself as its own child.
    .value("AK_InvalidParameter", AK_InvalidParameter) 	///< Something is not within bounds, check the documentation of the function returning this code.
    .value("AK_ElementAlreadyInList", AK_ElementAlreadyInList) ///< The item could not be added because it was already in the list.
    .value("AK_PathNotFound", AK_PathNotFound)	///< This path is not known.
    .value("AK_PathNoVertices", AK_PathNoVertices) 	///< Stuff in vertices before trying to start it
    .value("AK_PathNotRunning", AK_PathNotRunning) 	///< Only a running path can be paused.
    .value("AK_PathNotPaused", AK_PathNotPaused) 	///< Only a paused path can be resumed.
    .value("AK_PathNodeAlreadyInList", AK_PathNodeAlreadyInList) //< This path is already there.
    .value("AK_PathNodeNotInList", AK_PathNodeNotInList) ///< This path is not there.
    .value("AK_DataNeeded", AK_DataNeeded)	///< The consumer needs more.
    .value("AK_NoDataNeeded", AK_NoDataNeeded)	///< The consumer does not need more.
    .value("AK_DataReady", AK_DataReady)	///< The provider has available data.
    .value("AK_NoDataReady", AK_NoDataReady)	///< The provider does not have available data.
    .value("AK_InsufficientMemory", AK_InsufficientMemory) ///< Memory error.
    .value("AK_Cancelled", AK_Cancelled)	///< The requested action was cancelled (not an error).
    .value("AK_UnknownBankID", AK_UnknownBankID) 	///< Trying to load a bank using an ID which is not defined.
    .value("AK_BankReadError", AK_BankReadError) 	///< Error while reading a bank.
    .value("AK_InvalidSwitchType", AK_InvalidSwitchType) ///< Invalid switch type (used with the switch container)
    .value("AK_FormatNotReady", AK_FormatNotReady)   ///< Source format not known yet.
	  .value("AK_WrongBankVersion", AK_WrongBankVersion) 	///< The bank version is not compatible with the current bank reader.
    .value("AK_FileNotFound", AK_FileNotFound)   ///< File not found.
    .value("AK_DeviceNotReady", AK_DeviceNotReady)   ///< Specified ID doesn't match a valid hardware device: either the device doesn't exist or is disabled.
    .value("AK_BankAlreadyLoaded", AK_BankAlreadyLoaded) ///< The bank load failed because the bank is already loaded.
    .value("AK_RenderedFX", AK_RenderedFX)	///< The effect on the node is rendered.
    .value("AK_ProcessNeeded", AK_ProcessNeeded) 	///< A routine needs to be executed on some CPU.
    .value("AK_ProcessDone", AK_ProcessDone)	///< The executed routine has finished its execution.
    .value("AK_MemManagerNotInitialized", AK_MemManagerNotInitialized) //< The memory manager should have been initialized at this point.
    .value("AK_StreamMgrNotInitialized", AK_StreamMgrNotInitialized) //< The stream manager should have been initialized at this point.
    .value("AK_SSEInstructionsNotSupported", AK_SSEInstructionsNotSupported) ///< The machine does not support SSE instructions (required on PC).
    .value("AK_Busy", AK_Busy) ///< The system is busy and could not process the request.
    .value("AK_UnsupportedChannelConfig", AK_UnsupportedChannelConfig) //< Channel configuration is not supported in the current execution context.
    .value("AK_PluginMediaNotAvailable", AK_PluginMediaNotAvailable) ///< Plugin media is not available for effect.
    .value("AK_MustBeVirtualized", AK_MustBeVirtualized) ///< Sound was Not Allowed to play.
    .value("AK_CommandTooLarge", AK_CommandTooLarge) 	///< SDK command is too large to fit in the command queue.
    .value("AK_RejectedByFilter", AK_RejectedByFilter) 	///< A play request was rejected due to the MIDI filter parameters.
    .value("AK_InvalidCustomPlatformName", AK_InvalidCustomPlatformName) ///< Detecting incompatibility between Custom platform of banks and custom platform of connected application
    .value("AK_DLLCannotLoad", AK_DLLCannotLoad) 	///< Plugin DLL could not be loaded, either because it is not found or one dependency is missing.
    .value("AK_DLLPathNotFound", AK_DLLPathNotFound) 	///< Plugin DLL search path could not be found.
    .value("AK_NoJavaVM", AK_NoJavaVM)	///< No Java VM provided in AkInitSettings.
    .value("AK_OpenSLError", AK_OpenSLError)	///< OpenSL returned an error.  Check error log for more details.
    .value("AK_PluginNotRegistered", AK_PluginNotRegistered) ///< Plugin is not registered.  Make sure to implement a AK::PluginRegistration class for it and use AK_STATIC_LINK_PLUGIN in the game binary.
    .value("AK_DataAlignmentError", AK_DataAlignmentError) ///< A pointer to audio data was not aligned to the platform's required alignment (check AkTypes.h in the platform-specific folder)
    .value("AK_DeviceNotCompatible", AK_DeviceNotCompatible) ///< Incompatible Audio device.
    .value("AK_DuplicateUniqueID", AK_DuplicateUniqueID) ///< Two Wwise objects share the same ID.
    .value("AK_InitBankNotLoaded", AK_InitBankNotLoaded) ///< The Init bank was not loaded yet, the sound engine isn't completely ready yet.
    .value("AK_DeviceNotFound", AK_DeviceNotFound) 	///< The specified device ID does not match with any of the output devices that the sound engine is currently using.
    .value("AK_PlayingIDNotFound", AK_PlayingIDNotFound) ///< Calling a function with a playing ID that is not known.
    .value("AK_InvalidFloatValue", AK_InvalidFloatValue) ///< One parameter has a invalid float value such as NaN, INF or FLT_MAX.
    .value("AK_FileFormatMismatch", AK_FileFormatMismatch) ///< Media file format unexpected
    .value("AK_NoDistinctListener", AK_NoDistinctListener) ///< No distinct listener provided for AddOutput
    .value("AK_ACP_Error", AK_ACP_Error) ///< Generic XMA decoder error.
    .value("AK_ResourceInUse", AK_ResourceInUse) 	///< Resource is in use and cannot be released.
    .value("AK_InvalidBankType", AK_InvalidBankType) ///< Invalid bank type. The bank type was either supplied through a function call (e.g. LoadBank) or obtained from a bank loaded from memory.
    .value("AK_AlreadyInitialized", AK_AlreadyInitialized) 	///< Init() was called but that element was already initialized.
    .value("AK_NotInitialized", AK_NotInitialized)	///< The component being used is not initialized. Most likely AK::SoundEngine::Init() was not called yet, or AK::SoundEngine::Term was called too early.
    .value("AK_FilePermissionError", AK_FilePermissionError) 	///< The file access permissions prevent opening a file.
    .value("AK_UnknownFileError", AK_UnknownFileError)	///< Rare file error occured, as opposed to AK_FileNotFound or AK_FilePermissionError. This lumps all unrecognized OS file system errors.
  ;

  function("InitSoundEngine", optional_override([]() -> AKRESULT {
    // Memory Manager
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the memory manager.')");
      return AK_Fail;
    }

    // Streaming Manager
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    if ( !AK::StreamMgr::Create(stmSettings)) {
      emscripten_run_script("console.error('Could not create the streaming manager.')");
      return AK_Fail;
    }

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    if (g_lowLevelIO.Init(deviceSettings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the streaming device and Low-Level I/O system.')");
      return AK_Fail;
    }

    // Sound Engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Sound Engine.')");
      return AK_Fail;
    }

    // Music Engine
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if ( AK::MusicEngine::Init(&musicInit) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Music Engine.')");
      return AK_Fail;
    }

    // Spatial Audio
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings.
    if ( AK::SpatialAudio::Init(settings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Spatial Audio.')");
      return AK_Fail;
    }

    return AK_Success;
  }));
  function("ProcessAudio", optional_override([]() {
    AK::SoundEngine::RenderAudio();
  }));
  function("LoadBank", optional_override([]() {
    g_lowLevelIO.SetBasePath(AKTEXT("/"));
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    AkBankID bankID; // Not used. These banks can be unloaded with their file name.
    AKRESULT eResult;

    eResult = AK::SoundEngine::LoadBank(L"Init.bnk", bankID);
    eResult = AK::SoundEngine::LoadBank(L"Main.bnk", bankID);

    return AK_Success;
  }));
  function("RegisterTestGameObjectsAndListeners", optional_override([]() {
    AkGameObjectID MY_DEFAULT_LISTENER = 0;

    // Register the main listener.
    AK::SoundEngine::RegisterGameObj(MY_DEFAULT_LISTENER, "My Default Listener");

    // Set one listener as the default.
    AK::SoundEngine::SetDefaultListeners(&MY_DEFAULT_LISTENER, 1);

    // Register a game object for playing sounds
    AkGameObjectID MY_EMITTER = 1;

    AK::SoundEngine::RegisterGameObj(MY_EMITTER, "My Emitter");

    // At this point "My Emitter" has 1 listener, "My Default Listener", because we designated it as the default listener.

    AK::SoundEngine::PostEvent(L"Ambience", 1);

    return AK_Success;
  }));

}



// .field("pfInitForThread", &AkMemSettings::pfInitForThread, allow_raw_pointers())
// .field("pfTermForThread", &AkMemSettings::pfTermForThread, allow_raw_pointers())
// .field("pfMalloc", &AkMemSettings::pfMalloc, allow_raw_pointers())
// .field("pfMalign", &AkMemSettings::pfMalign, allow_raw_pointers())
// .field("pfRealloc", &AkMemSettings::pfRealloc, allow_raw_pointers())
// .field("pfReallocAligned", &AkMemSettings::pfReallocAligned, allow_raw_pointers())
// .field("pfFree", &AkMemSettings::pfFree, allow_raw_pointers())
// .field("pfTotalReservedMemorySize", &AkMemSettings::pfTotalReservedMemorySize, allow_raw_pointers())
// .field("pfSizeOfMemory", &AkMemSettings::pfSizeOfMemory, allow_raw_pointers())

// .field("pfAllocVM", &AkMemSettings::pfAllocVM, allow_raw_pointers())
// .field("pfFreeVM", &AkMemSettings::pfFreeVM, allow_raw_pointers())
// .field("pfAllocDevice", &AkMemSettings::pfAllocDevice, allow_raw_pointers())
// .field("pfFreeDevice", &AkMemSettings::pfFreeDevice, allow_raw_pointers())
// .field("uVMPageSize", &AkMemSettings::uVMPageSize)
// .field("uDevicePageSize", &AkMemSettings::uDevicePageSize)
// .field("pfDebugMalloc", &AkMemSettings::pfDebugMalloc, allow_raw_pointers())
// .field("pfDebugMalign", &AkMemSettings::pfDebugMalign, allow_raw_pointers())
// .field("pfDebugRealloc", &AkMemSettings::pfDebugRealloc, allow_raw_pointers())
// .field("pfDebugReallocAligned", &AkMemSettings::pfDebugReallocAligned, allow_raw_pointers())
// .field("pfDebugFree", &AkMemSettings::pfDebugFree, allow_raw_pointers())
// .field("uMemoryDebugLevel", &AkMemSettings::uMemoryDebugLevel)