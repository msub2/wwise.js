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
#include <AK/SoundEngine/Common/AkQueryParameters.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>


using namespace emscripten;

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

// Helpers
struct AkAudioSettings createAkAudioSettings(AkUInt32 uNumSamplesPerFrame, AkUInt32 uNumSamplesPerSecond) {
  struct AkAudioSettings ret;
  ret.uNumSamplesPerFrame = uNumSamplesPerFrame;
  ret.uNumSamplesPerSecond = uNumSamplesPerSecond;
  return ret;
}
struct AkSegmentInfo createAkSegmentInfo(AkTimeMs iCurrentPosition, AkTimeMs iPreEntryDuration,
                                         AkTimeMs iActiveDuration, AkTimeMs iPostExitDuration,
                                         AkTimeMs iRemainingLookAheadTime, AkReal32 fBeatDuration,
                                         AkReal32 fBarDuration, AkReal32 fGridDuration, AkReal32 fGridOffset) {
  struct AkSegmentInfo ret;
  ret.iCurrentPosition = iCurrentPosition;
  ret.iPreEntryDuration = iPreEntryDuration;
  ret.iActiveDuration = iActiveDuration;
  ret.iPostExitDuration = iPostExitDuration;
  ret.iRemainingLookAheadTime = iRemainingLookAheadTime;
  ret.fBeatDuration = fBeatDuration;
  ret.fBarDuration = fBarDuration;
  ret.fGridDuration = fGridDuration;
  ret.fGridOffset = fGridOffset;
  return ret;
}

EMSCRIPTEN_BINDINGS(my_module) {
  /**
  * AK
  */

  // Enums
  enum_<AKRESULT>("AKRESULT")
    .value("NotImplemented", AK_NotImplemented)
    .value("Success", AK_Success)
    .value("Failure", AK_Fail)
    .value("PartialSuccess", AK_PartialSuccess)
    .value("NotCompatible", AK_NotCompatible)	///< Incompatible formats
    .value("AlreadyConnected", AK_AlreadyConnected)	///< The stream is already connected to another node.
    .value("InvalidFile", AK_InvalidFile)	///< The provided file is the wrong format or unexpected values causes the file to be invalid.
    .value("AudioFileHeaderTooLarge", AK_AudioFileHeaderTooLarge) ///< The file header is too large.
    .value("MaxReached", AK_MaxReached) 	///< The maximum was reached.
    .value("InvalidID", AK_InvalidID)	///< The ID is invalid.
    .value("IDNotFound", AK_IDNotFound)	///< The ID was not found.
    .value("InvalidInstanceID", AK_InvalidInstanceID) ///< The InstanceID is invalid.
    .value("NoMoreData", AK_NoMoreData)	///< No more data is available from the source.
    .value("InvalidStateGroup", AK_InvalidStateGroup) ///< The StateGroup is not a valid channel.
    .value("ChildAlreadyHasAParent", AK_ChildAlreadyHasAParent) //< The child already has a parent.
    .value("InvalidLanguage", AK_InvalidLanguage) 	///< The language is invalid (applies to the Low-Level I/O).
    .value("CannotAddItseflAsAChild", AK_CannotAddItseflAsAChild) //< It is not possible to add itself as its own child.
    .value("InvalidParameter", AK_InvalidParameter) 	///< Something is not within bounds, check the documentation of the function returning this code.
    .value("ElementAlreadyInList", AK_ElementAlreadyInList) ///< The item could not be added because it was already in the list.
    .value("PathNotFound", AK_PathNotFound)	///< This path is not known.
    .value("PathNoVertices", AK_PathNoVertices) 	///< Stuff in vertices before trying to start it
    .value("PathNotRunning", AK_PathNotRunning) 	///< Only a running path can be paused.
    .value("PathNotPaused", AK_PathNotPaused) 	///< Only a paused path can be resumed.
    .value("PathNodeAlreadyInList", AK_PathNodeAlreadyInList) //< This path is already there.
    .value("PathNodeNotInList", AK_PathNodeNotInList) ///< This path is not there.
    .value("DataNeeded", AK_DataNeeded)	///< The consumer needs more.
    .value("NoDataNeeded", AK_NoDataNeeded)	///< The consumer does not need more.
    .value("DataReady", AK_DataReady)	///< The provider has available data.
    .value("NoDataReady", AK_NoDataReady)	///< The provider does not have available data.
    .value("InsufficientMemory", AK_InsufficientMemory) ///< Memory error.
    .value("Cancelled", AK_Cancelled)	///< The requested action was cancelled (not an error).
    .value("UnknownBankID", AK_UnknownBankID) 	///< Trying to load a bank using an ID which is not defined.
    .value("BankReadError", AK_BankReadError) 	///< Error while reading a bank.
    .value("InvalidSwitchType", AK_InvalidSwitchType) ///< Invalid switch type (used with the switch container)
    .value("FormatNotReady", AK_FormatNotReady)   ///< Source format not known yet.
	  .value("WrongBankVersion", AK_WrongBankVersion) 	///< The bank version is not compatible with the current bank reader.
    .value("FileNotFound", AK_FileNotFound)   ///< File not found.
    .value("DeviceNotReady", AK_DeviceNotReady)   ///< Specified ID doesn't match a valid hardware device: either the device doesn't exist or is disabled.
    .value("BankAlreadyLoaded", AK_BankAlreadyLoaded) ///< The bank load failed because the bank is already loaded.
    .value("RenderedFX", AK_RenderedFX)	///< The effect on the node is rendered.
    .value("ProcessNeeded", AK_ProcessNeeded) 	///< A routine needs to be executed on some CPU.
    .value("ProcessDone", AK_ProcessDone)	///< The executed routine has finished its execution.
    .value("MemManagerNotInitialized", AK_MemManagerNotInitialized) //< The memory manager should have been initialized at this point.
    .value("StreamMgrNotInitialized", AK_StreamMgrNotInitialized) //< The stream manager should have been initialized at this point.
    .value("SSEInstructionsNotSupported", AK_SSEInstructionsNotSupported) ///< The machine does not support SSE instructions (required on PC).
    .value("Busy", AK_Busy) ///< The system is busy and could not process the request.
    .value("UnsupportedChannelConfig", AK_UnsupportedChannelConfig) //< Channel configuration is not supported in the current execution context.
    .value("PluginMediaNotAvailable", AK_PluginMediaNotAvailable) ///< Plugin media is not available for effect.
    .value("MustBeVirtualized", AK_MustBeVirtualized) ///< Sound was Not Allowed to play.
    .value("CommandTooLarge", AK_CommandTooLarge) 	///< SDK command is too large to fit in the command queue.
    .value("RejectedByFilter", AK_RejectedByFilter) 	///< A play request was rejected due to the MIDI filter parameters.
    .value("InvalidCustomPlatformName", AK_InvalidCustomPlatformName) ///< Detecting incompatibility between Custom platform of banks and custom platform of connected application
    .value("DLLCannotLoad", AK_DLLCannotLoad) 	///< Plugin DLL could not be loaded, either because it is not found or one dependency is missing.
    .value("DLLPathNotFound", AK_DLLPathNotFound) 	///< Plugin DLL search path could not be found.
    .value("NoJavaVM", AK_NoJavaVM)	///< No Java VM provided in AkInitSettings.
    .value("OpenSLError", AK_OpenSLError)	///< OpenSL returned an error.  Check error log for more details.
    .value("PluginNotRegistered", AK_PluginNotRegistered) ///< Plugin is not registered.  Make sure to implement a AK::PluginRegistration class for it and use AK_STATIC_LINK_PLUGIN in the game binary.
    .value("DataAlignmentError", AK_DataAlignmentError) ///< A pointer to audio data was not aligned to the platform's required alignment (check AkTypes.h in the platform-specific folder)
    .value("DeviceNotCompatible", AK_DeviceNotCompatible) ///< Incompatible Audio device.
    .value("DuplicateUniqueID", AK_DuplicateUniqueID) ///< Two Wwise objects share the same ID.
    .value("InitBankNotLoaded", AK_InitBankNotLoaded) ///< The Init bank was not loaded yet, the sound engine isn't completely ready yet.
    .value("DeviceNotFound", AK_DeviceNotFound) 	///< The specified device ID does not match with any of the output devices that the sound engine is currently using.
    .value("PlayingIDNotFound", AK_PlayingIDNotFound) ///< Calling a function with a playing ID that is not known.
    .value("InvalidFloatValue", AK_InvalidFloatValue) ///< One parameter has a invalid float value such as NaN, INF or FLT_MAX.
    .value("FileFormatMismatch", AK_FileFormatMismatch) ///< Media file format unexpected
    .value("NoDistinctListener", AK_NoDistinctListener) ///< No distinct listener provided for AddOutput
    .value("ACP_Error", AK_ACP_Error) ///< Generic XMA decoder error.
    .value("ResourceInUse", AK_ResourceInUse) 	///< Resource is in use and cannot be released.
    .value("InvalidBankType", AK_InvalidBankType) ///< Invalid bank type. The bank type was either supplied through a function call (e.g. LoadBank) or obtained from a bank loaded from memory.
    .value("AlreadyInitialized", AK_AlreadyInitialized) 	///< Init() was called but that element was already initialized.
    .value("NotInitialized", AK_NotInitialized)	///< The component being used is not initialized. Most likely AK::SoundEngine::Init() was not called yet, or AK::SoundEngine::Term was called too early.
    .value("FilePermissionError", AK_FilePermissionError) 	///< The file access permissions prevent opening a file.
    .value("UnknownFileError", AK_UnknownFileError)	///< Rare file error occured, as opposed to AK_FileNotFound or AK_FilePermissionError. This lumps all unrecognized OS file system errors.
  ;

  // Structs
  class_<AkAudioSettings>("AkAudioSettings")
    .constructor<>()
    .property("uNumSamplesPerFrame", &AkAudioSettings::uNumSamplesPerFrame)
    .property("uNumSamplesPerSecond", &AkAudioSettings::uNumSamplesPerSecond)
  ;

  // Helpers
  function("createAkAudioSettings", &createAkAudioSettings);
  function("createAkSegmentInfo", &createAkSegmentInfo);

  /**
  * MemoryMgr
  */

  //Initialization
  function("MemoryMgr_Init", optional_override([]() -> AKRESULT {
    // Memory Manager
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the memory manager.')");
      return AK_Fail;
    }

    return AK_Success;
  }));
  function("MemoryMgr_IsInitialized", &AK::MemoryMgr::IsInitialized);
  function("MemoryMgr_Term", &AK::MemoryMgr::Term);
  function("MemoryMgr_InitForThread", &AK::MemoryMgr::InitForThread);
  function("MemoryMgr_TermForThread", &AK::MemoryMgr::TermForThread);
  // XXX: How many more of the AK::MemoryMgr functions are applicable in the browser?

  /**
  * MusicEngine
  */

  class_<AkSegmentInfo>("AkSegmentInfo")
    .constructor<>()
    .property("iCurrentPosition", &AkSegmentInfo::iCurrentPosition)
    .property("iPreEntryDuration", &AkSegmentInfo::iPreEntryDuration)
    .property("iActiveDuration", &AkSegmentInfo::iActiveDuration)
    .property("iPostExitDuration", &AkSegmentInfo::iPostExitDuration)
    .property("iRemainingLookAheadTime", &AkSegmentInfo::iRemainingLookAheadTime)
    .property("fBeatDuration", &AkSegmentInfo::fBeatDuration)
    .property("fBarDuration", &AkSegmentInfo::fBarDuration)
    .property("fGridDuration", &AkSegmentInfo::fGridDuration)
    .property("fGridOffset", &AkSegmentInfo::fGridOffset)
  ;
  // Initialization
  function("MusicEngine_Init", optional_override([]() {
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if ( AK::MusicEngine::Init(&musicInit) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Music Engine.')");
      return AK_Fail;
    }

    return AK_Success;
  }));
  function("MusicEngine_Term", &AK::MusicEngine::Term);
  function("MusicEngine_GetPlayingSegmentInfo", &AK::MusicEngine::GetPlayingSegmentInfo);

  /**
  * SoundEngine
  */

  // Enums
  enum_<AK::SoundEngine::AkActionOnEventType>("SoundEngine_AkActionOnEventType")
    .value("Stop", AK::SoundEngine::AkActionOnEventType_Stop)
    .value("Pause", AK::SoundEngine::AkActionOnEventType_Pause)
    .value("Resume", AK::SoundEngine::AkActionOnEventType_Resume)
    .value("Break", AK::SoundEngine::AkActionOnEventType_Break)
    .value("ReleaseEnvelope", AK::SoundEngine::AkActionOnEventType_ReleaseEnvelope)
  ;
  enum_<AK::SoundEngine::AkBankContent>("SoundEngine_AkBankContent")
    .value("StructureOnly", AK::SoundEngine::AkBankContent::AkBankContent_StructureOnly)
    .value("All", AK::SoundEngine::AkBankContent::AkBankContent_All)
  ;
  enum_<AK::SoundEngine::MultiPositionType>("SoundEngine_MultiPositionType")
    .value("SingleSource", AK::SoundEngine::MultiPositionType_SingleSource)
    .value("MultiSources", AK::SoundEngine::MultiPositionType_MultiSources)
    .value("MultiDirections", AK::SoundEngine::MultiPositionType_MultiDirections)
  ;
  enum_<AK::SoundEngine::PreparationType>("SoundEngine_PreparationType")
    .value("Load", AK::SoundEngine::Preparation_Load)
    .value("Unload", AK::SoundEngine::Preparation_Unload)
    .value("LoadAndDecode", AK::SoundEngine::Preparation_LoadAndDecode)
  ;

  // Initialization
  function("SoundEngine_Init", optional_override([]() -> AKRESULT {
    // Sound Engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Sound Engine.')");
      return AK_Fail;
    }

    return AK_Success;
  }));
  function("SoundEngine_IsInitialized", &AK::SoundEngine::IsInitialized);
  function("SoundEngine_Term", &AK::SoundEngine::Term);
  function("SoundEngine_GetAudioSettings", &AK::SoundEngine::GetAudioSettings);

  function("SoundEngine_RenderAudio", optional_override([]() {
    // I suspect the optional flag here controls whether AudioWorklet is used, if so we should always want to use it
    AK::SoundEngine::RenderAudio();
  }));
  function("SoundEngine_LoadBank", optional_override([](const std::wstring& bankId) {
    AkBankID id;
    return AK::SoundEngine::LoadBank(bankId.c_str(), id);
  }));
  function("SoundEngine_UnloadBank", select_overload<AKRESULT(const char*, const void*, AkBankType)>(&AK::SoundEngine::UnloadBank), allow_raw_pointers());
  function("SoundEngine_RegisterGameObj", optional_override([](AkGameObjectID gameObjectID, const std::string& name) {
    return AK::SoundEngine::RegisterGameObj(gameObjectID, name.c_str());
  }), allow_raw_pointers());
  function("SoundEngine_UnregisterGameObj", &AK::SoundEngine::UnregisterGameObj);
  function("SoundEngine_UnregisterAllGameObj", &AK::SoundEngine::UnregisterAllGameObj);
  function("SoundEngine_SetDefaultListeners", optional_override([](const AkGameObjectID gameObjectID, AkUInt32 numListeners) {
    return AK::SoundEngine::SetDefaultListeners(&gameObjectID, numListeners);
  }), allow_raw_pointers());
  function("SoundEngine_PostEvent", optional_override([](const std::wstring& eventName, AkGameObjectID gameObjectID) {
    return AK::SoundEngine::PostEvent(eventName.c_str(), gameObjectID);
  }), allow_raw_pointers());

  /**
  * SoundEngine::DynamicDialogue
  */

  /**
  * SoundEngine::DynamicSequence
  */

  /**
  * SoundEngine::Query
  */

  // Enums
  enum_<AK::SoundEngine::Query::RTPCValue_type>("SoundEngine_Query_RTPCValue")
    .value("Default", AK::SoundEngine::Query::RTPCValue_type::RTPCValue_Default)
    .value("Global", AK::SoundEngine::Query::RTPCValue_type::RTPCValue_Global)
    .value("GameObject", AK::SoundEngine::Query::RTPCValue_type::RTPCValue_GameObject)
    .value("PlayingID", AK::SoundEngine::Query::RTPCValue_type::RTPCValue_PlayingID)
    .value("Unavailable", AK::SoundEngine::Query::RTPCValue_type::RTPCValue_Unavailable)
  ;

  function("StreamMgr_Create", optional_override([]() {
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

    g_lowLevelIO.SetBasePath(AKTEXT("/"));
    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    return AK_Success;
  }));
  function("SpatialAudio_Init", optional_override([]() {
    // Spatial Audio
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings.
    if (AK::SpatialAudio::Init(settings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Spatial Audio.')");
      return AK_Fail;
    }

    return AK_Success;
  }));
  class_<AkVector>("AkVector")
    .constructor<>()
    .property("x", &AkVector::X)
    .property("y", &AkVector::Y)
    .property("z", &AkVector::Z)
  ;
  class_<AkVector64>("AkVector64")
    .constructor<>()
    .property("x", &AkVector64::X)
    .property("y", &AkVector64::Y)
    .property("z", &AkVector64::Z)
  ;
  class_<AkTransform>("AkTransform")
    .constructor()
    .property("Position", &AkTransform::Position, select_overload<void(const AkVector&)>(&AkTransform::SetPosition))
    .property("OrientationFront", &AkTransform::OrientationFront)
    .property("OrientationTop", &AkTransform::OrientationTop)
    .function("SetOrientation", select_overload<void(const AkVector&, const AkVector&)>(&AkTransform::SetOrientation))
    .function("Set", select_overload<void(const AkVector&, const AkVector&, const AkVector&)>(&AkTransform::Set))
  ;
  class_<AkWorldTransform>("AkWorldTransform")
    .constructor()
    .property("Position", &AkWorldTransform::Position, select_overload<void(const AkVector64&)>(&AkWorldTransform::SetPosition))
    .property("OrientationFront", &AkWorldTransform::OrientationFront)
    .property("OrientationTop", &AkWorldTransform::OrientationTop)
    .function("SetOrientation", select_overload<void(const AkVector&, const AkVector&)>(&AkWorldTransform::SetOrientation))
    .function("Set", select_overload<void(const AkVector64&, const AkVector&, const AkVector&)>(&AkWorldTransform::Set))
  ;
  enum_<AkSetPositionFlags>("AkSetPositionFlags")
    .value("Emitter", AkSetPositionFlags_Emitter)
    .value("Listener", AkSetPositionFlags_Listener)
    .value("Default", AkSetPositionFlags_Default)
  ;
  function("SoundEngine_SetPosition", &AK::SoundEngine::SetPosition);
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