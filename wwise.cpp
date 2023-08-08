#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
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
#include <AK/SoundEngine/Common/AkDynamicDialogue.h>
#include <AK/SoundEngine/Common/AkDynamicSequence.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>
#include <AK/SpatialAudio/Common/AkReverbEstimation.h>
#include <AK/Comm/AkCommunication.h>

#include <AK/Plugin/AkOpusDecoderFactory.h>
#include <AK/Plugin/AkVorbisDecoderFactory.h>

using namespace emscripten;

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

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
  enum_<AK::AkChannelOrdering>("AkChannelOrdering")
    .value("Standard", AK::AkChannelOrdering::ChannelOrdering_Standard)
    .value("RunTime", AK::AkChannelOrdering::ChannelOrdering_RunTime)
  ;
  enum_<AkChannelConfigType>("AkChannelConfigType")
    .value("Anonymous", AK_ChannelConfigType_Anonymous)
    .value("Standard", AK_ChannelConfigType_Standard)
    .value("Ambisonic", AK_ChannelConfigType_Ambisonic)
    .value("Objects", AK_ChannelConfigType_Objects)
    .value("UseDeviceMain", AK_ChannelConfigType_UseDeviceMain)
    .value("UseDevicePassthrough", AK_ChannelConfigType_UseDevicePassthrough)
  ;
  enum_<AkCurveInterpolation>("AkCurveInterpolation")
    .value("Log3", AkCurveInterpolation_Log3)
    .value("Sine", AkCurveInterpolation_Sine)
    .value("Log1", AkCurveInterpolation_Log1)
    .value("InvSCurve", AkCurveInterpolation_InvSCurve)
    .value("Linear", AkCurveInterpolation_Linear)
    .value("SCurve", AkCurveInterpolation_SCurve)
    .value("Exp1", AkCurveInterpolation_Exp1)
    .value("SineRecip", AkCurveInterpolation_SineRecip)
    .value("Exp3", AkCurveInterpolation_Exp3)
    .value("LastFadeCurve", AkCurveInterpolation_LastFadeCurve)
    .value("Constant", AkCurveInterpolation_Constant)
  ;
  enum_<AkPanningRule>("AkPanningRule")
    .value("Speakers", AkPanningRule_Speakers)
    .value("Headphones", AkPanningRule_Headphones)
  ;
  enum_<AkSetPositionFlags>("AkSetPositionFlags")
    .value("Emitter", AkSetPositionFlags_Emitter)
    .value("Listener", AkSetPositionFlags_Listener)
    .value("Default", AkSetPositionFlags_Default)
  ;

  // Structs
  class_<AkAudioSettings>("AkAudioSettings")
    .constructor<>()
    .property("uNumSamplesPerFrame", &AkAudioSettings::uNumSamplesPerFrame)
    .property("uNumSamplesPerSecond", &AkAudioSettings::uNumSamplesPerSecond)
  ;
  class_<AkAuxSendValue>("AkAuxSendValue")
    .constructor<>()
    .property("listenerID", &AkAuxSendValue::listenerID)
    .property("auxBusID", &AkAuxSendValue::auxBusID)
    .property("fControlValue", &AkAuxSendValue::fControlValue)
  ;
  class_<AkChannelConfig>("AkChannelConfig")
    .constructor<AkUInt32, AkUInt32>()
    .function("Clear", &AkChannelConfig::Clear)
    .function("SetStandard", &AkChannelConfig::SetStandard)
    .function("SetStandardOrAnonymous", &AkChannelConfig::SetStandardOrAnonymous)
    .function("SetAnonymous", &AkChannelConfig::SetAnonymous)
    .function("SetAmbisonic", &AkChannelConfig::SetAmbisonic)
    .function("SetObject", &AkChannelConfig::SetObject)
    .function("SetSameAsMainMix", &AkChannelConfig::SetSameAsMainMix)
    .function("SetSameAsPassthrough", &AkChannelConfig::SetSameAsPassthrough)
    .function("IsValid", &AkChannelConfig::IsValid)
    .function("Serialize", &AkChannelConfig::Serialize)
    .function("Deserialize", &AkChannelConfig::Deserialize)
    .function("RemoveLFE", &AkChannelConfig::RemoveLFE)
    .function("RemoveCenter", &AkChannelConfig::RemoveCenter)
    .function("HasLFE", &AkChannelConfig::HasLFE)
    .function("HasCenter", &AkChannelConfig::HasCenter)
    // TODO: uNumChannels, eConfigType, uChannelMask
  ;
  class_<AkObstructionOcclusionValues>("AkObstructionOcclusionValues")
    .constructor<>()
    .property("occlusion", &AkObstructionOcclusionValues::occlusion)
    .property("obstruction", &AkObstructionOcclusionValues::obstruction)
  ;
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
  // FIXME: This doesn't seem to want to bind with initialized arrays
  // class_<AkReflectionPathInfo>("AkReflectionPathInfo")
  //   .constructor<>()
  //   .property("imageSource", &AkReflectionPathInfo::imageSource)
  //   .property("pathPoint", &AkReflectionPathInfo::pathPoint)
  //   .property("surfaces", &AkReflectionPathInfo::surfaces)
  //   .property("numPathPoints", &AkReflectionPathInfo::numPathPoints)
  //   .property("numReflections", &AkReflectionPathInfo::numReflections)
  //   .property("diffraction", &AkReflectionPathInfo::diffraction)
  //   .property("level", &AkReflectionPathInfo::level)
  // ;
  class_<AkTransform>("AkTransform")
    .constructor()
    .property("Position", &AkTransform::Position, select_overload<void(const AkVector&)>(&AkTransform::SetPosition))
    .property("OrientationFront", &AkTransform::OrientationFront)
    .property("OrientationTop", &AkTransform::OrientationTop)
    .function("SetOrientation", select_overload<void(const AkVector&, const AkVector&)>(&AkTransform::SetOrientation))
    .function("Set", select_overload<void(const AkVector&, const AkVector&, const AkVector&)>(&AkTransform::Set))
  ;
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
  class_<AkWorldTransform>("AkWorldTransform")
    .constructor()
    .property("Position", &AkWorldTransform::Position, select_overload<void(const AkVector64&)>(&AkWorldTransform::SetPosition))
    .property("OrientationFront", &AkWorldTransform::OrientationFront)
    .property("OrientationTop", &AkWorldTransform::OrientationTop)
    .function("SetOrientation", select_overload<void(const AkVector&, const AkVector&)>(&AkWorldTransform::SetOrientation))
    .function("Set", select_overload<void(const AkVector64&, const AkVector&, const AkVector&)>(&AkWorldTransform::Set))
  ;

  // Vectors
  register_vector<AkGameObjectID>("vector<AkGameObjectID>");
  register_vector<AkReal32>("vector<AkReal32>");
  register_vector<AK::SpeakerVolumes::VectorPtr>("vector<AK::SpeakerVolumes::VectorPtr>");
  register_vector<AkReflectionPathInfo>("vector<AkReflectionPathInfo>");

  /**
  * Comm
  */

#ifndef AK_OPTIMIZED
  // Initialization
  function("Comm_Init", optional_override([](const std::string& name, const std::string& url) {
    AkCommSettings settings;
    AK::Comm::GetDefaultInitSettings(settings);
    strncpy(settings.szAppNetworkName, name.c_str(), sizeof(name));
    strncpy(settings.szCommProxyServerUrl, url.c_str(), sizeof(url));
    if (AK::Comm::Init(settings) != AK_Success) {
      emscripten_run_script("console.error('Could not initialize communications.')");
      return AK_Fail;
    }
    return AK_Success;
  }));
  // XXX: Not defined in Web build?
  // function("Comm_GetLastError", &AK::Comm::GetLastError);
  function("Comm_Term", &AK::Comm::Term);
  function("Comm_Reset", &AK::Comm::Reset);
#endif

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
  function("SoundEngine_GetSpeakerConfiguration", &AK::SoundEngine::GetSpeakerConfiguration);
  function("SoundEngine_GetOutputDeviceConfiguration", &AK::SoundEngine::GetOutputDeviceConfiguration);
  function("SoundEngine_GetPanningRule", optional_override([](val out_ePanningRule, AkOutputDeviceID in_idOutput = 0) {
    AkPanningRule out;
    AKRESULT result = AK::SoundEngine::GetPanningRule(out, in_idOutput);
    if (result == AK_Success) {
      out_ePanningRule.set("val", val(out));
    }
    return result;
  }));
  function("SoundEngine_SetPanningRule", &AK::SoundEngine::SetPanningRule);
  function("SoundEngine_GetSpeakerAngles", optional_override([](val io_pfSpeakerAngles, val out_fHeightAngle, AkOutputDeviceID in_idOutput=0) {
    // Get expected number of angle values
    AkUInt32 numAngles;
    AkReal32 heightAngle;
    AK::SoundEngine::GetSpeakerAngles(NULL, numAngles, heightAngle, in_idOutput);

    // Create a vector of AkReal32 and then pass pointer with .data()
    std::vector<AkReal32> speakerAngles(numAngles);
    AKRESULT result = AK::SoundEngine::GetSpeakerAngles(speakerAngles.data(), numAngles, heightAngle, in_idOutput);
    if (result == AK_Success) {
      io_pfSpeakerAngles.set("val", val(speakerAngles));
      out_fHeightAngle.set("val", val(heightAngle));
    }
    return result;
  }));
  // function("SoundEngine_SetSpeakerAngles", &AK::SoundEngine::SetSpeakerAngles, allow_raw_pointers());
  function("SoundEngine_SetVolumeThreshold", &AK::SoundEngine::SetVolumeThreshold);
  function("SoundEngine_SetMaxNumVoicesLimit", &AK::SoundEngine::SetMaxNumVoicesLimit);

  // Rendering Audio
  function("SoundEngine_RenderAudio", optional_override([]() {
    // I suspect the optional flag here controls whether AudioWorklet is used, if so we should always want to use it
    AK::SoundEngine::RenderAudio();
  }));

  // Game Objects
  function("SoundEngine_RegisterGameObj", optional_override([](AkGameObjectID gameObjectID, const std::string& name) {
    return AK::SoundEngine::RegisterGameObj(gameObjectID, name.c_str());
  }), allow_raw_pointers());
  function("SoundEngine_UnregisterGameObj", &AK::SoundEngine::UnregisterGameObj);
  function("SoundEngine_UnregisterAllGameObj", &AK::SoundEngine::UnregisterAllGameObj);
  function("SoundEngine_SetPosition", &AK::SoundEngine::SetPosition);
  // TODO: Figure out which overload to support (or both)
  // function("SoundEngine_SetMultiplePositions", &AK::SoundEngine::SetMultiplePositions);
  function("SoundEngine_SetScalingFactor", &AK::SoundEngine::SetScalingFactor);
  function("SoundEngine_SetDistanceProbe", &AK::SoundEngine::SetDistanceProbe);

  // Listeners
  function("SoundEngine_SetListeners", &AK::SoundEngine::SetListeners, allow_raw_pointers());
  function("SoundEngine_AddListener", &AK::SoundEngine::AddListener);
  function("SoundEngine_RemoveListener", &AK::SoundEngine::RemoveListener);
  function("SoundEngine_SetDefaultListeners", optional_override([](const AkGameObjectID gameObjectID, AkUInt32 numListeners) {
    return AK::SoundEngine::SetDefaultListeners(&gameObjectID, numListeners);
  }), allow_raw_pointers());
  function("SoundEngine_AddDefaultListener", &AK::SoundEngine::AddDefaultListener);
  function("SoundEngine_RemoveDefaultListener", &AK::SoundEngine::RemoveDefaultListener);
  function("SoundEngine_ResetListenersToDefault", &AK::SoundEngine::ResetListenersToDefault);
  function("SoundEngine_SetListenerSpatialization", optional_override([](AkGameObjectID in_uListenerID, bool in_bSpatialized, AkChannelConfig in_channelConfig) {
    // XXX: Omitting AK::SpeakerVolumes::VectorPtr in_pVolumeOffsets for now
    return AK::SoundEngine::SetListenerSpatialization(in_uListenerID, in_bSpatialized, in_channelConfig);
  }), allow_raw_pointers());

  // Game Syncs
  // Note: Need to override AkRtpcValue as regular float for binding to take
  function("SoundEngine_SetRTPCValue", optional_override([](const std::string& in_pszRtpcName, float in_value, AkGameObjectID in_gameObjectID=AK_INVALID_GAME_OBJECT, AkTimeMs in_uValueChangeDuration=0, AkCurveInterpolation in_eFadeCurve=AkCurveInterpolation_Linear, bool in_bBypassInternalValueInterpolation=false) {
    return AK::SoundEngine::SetRTPCValue(in_pszRtpcName.c_str(), in_value, in_gameObjectID=AK_INVALID_GAME_OBJECT, in_uValueChangeDuration, in_eFadeCurve, in_bBypassInternalValueInterpolation);
  }), allow_raw_pointers());
  function("SoundEngine_SetRTPCValueByPlayingID", optional_override([](const std::string& in_pszRtpcName, float in_value, AkPlayingID in_playingID, AkTimeMs in_uValueChangeDuration=0, AkCurveInterpolation in_eFadeCurve=AkCurveInterpolation_Linear, bool in_bBypassInternalValueInterpolation=false) {
    return AK::SoundEngine::SetRTPCValueByPlayingID(in_pszRtpcName.c_str(), in_value, in_playingID, in_uValueChangeDuration, in_eFadeCurve, in_bBypassInternalValueInterpolation);
  }));
  function("SoundEngine_ResetRTPCValue", optional_override([](const std::string& in_pszRtpcName, AkGameObjectID in_gameObjectID=AK_INVALID_GAME_OBJECT, AkTimeMs in_uValueChangeDuration=0, AkCurveInterpolation in_eFadeCurve=AkCurveInterpolation_Linear, bool in_bBypassInternalValueInterpolation=false) {
    return AK::SoundEngine::ResetRTPCValue(in_pszRtpcName.c_str(), in_gameObjectID, in_uValueChangeDuration, in_eFadeCurve, in_bBypassInternalValueInterpolation);
  }));
  function("SoundEngine_SetSwitch", optional_override([](const std::string& in_pszSwitchGroup, const std::string& in_pszSwitchState, AkGameObjectID in_gameObjectID) {
    return AK::SoundEngine::SetSwitch(in_pszSwitchGroup.c_str(), in_pszSwitchState.c_str(), in_gameObjectID);
  }));
  function("SoundEngine_PostTrigger", optional_override([](const std::string& in_pszTrigger, AkGameObjectID in_gameObjectID) {
    return AK::SoundEngine::PostTrigger(in_pszTrigger.c_str(), in_gameObjectID);
  }));
  function("SoundEngine_SetState", optional_override([](const std::string& in_pszStateGroup, const std::string& in_pszState) {
    return AK::SoundEngine::SetState(in_pszStateGroup.c_str(), in_pszState.c_str());
  }));

  // Environments
  function("SoundEngine_SetGameObjectAuxSendValues", &AK::SoundEngine::SetGameObjectAuxSendValues, allow_raw_pointers());
  // TODO: Look into binding callbacks, if possible
  // function("RegisterBusMeteringCallback", &AK::SoundEngine::RegisterBusMeteringCallback);
  // function("RegisterOutputDeviceMeteringCallback", &AK::SoundEngine::RegisterOutputDeviceMeteringCallback);
  function("SoundEngine_SetGameObjectOutputBusVolume", &AK::SoundEngine::SetGameObjectOutputBusVolume);
  function("SoundEngine_SetActorMixerEffect", &AK::SoundEngine::SetActorMixerEffect);
  function("SoundEngine_SetBusEffect", optional_override([](const std::string& in_pszBusName, AkUInt32 in_uFXIndex, AkUniqueID in_shareSetID) {
    return AK::SoundEngine::SetBusEffect(in_pszBusName.c_str(), in_uFXIndex, in_shareSetID);
  }));
  function("SoundEngine_SetOutputDeviceEffect", &AK::SoundEngine::SetOutputDeviceEffect);
  function("SoundEngine_SetMixer", optional_override([](const std::string& in_pszBusName, AkUniqueID in_shareSetID) {
    return AK::SoundEngine::SetMixer(in_pszBusName.c_str(), in_shareSetID);
  }));
  function("SoundEngine_SetBusConfig", optional_override([](const std::string& in_pszBusName, AkChannelConfig in_channelConfig) {
    return AK::SoundEngine::SetBusConfig(in_pszBusName.c_str(), in_channelConfig);
  }));
  function("SoundEngine_SetObjectObstructionAndOcclusion", &AK::SoundEngine::SetObjectObstructionAndOcclusion);
  function("SoundEngine_SetMultipleObstructionAndOcclusion", &AK::SoundEngine::SetMultipleObstructionAndOcclusion, allow_raw_pointers());
  // XXX: Are these applicable to web?
  // function("GetContainerHistory", &AK::SoundEngine::GetContainerHistory);
  // function("SetContainerHistory", &AK::SoundEngine::SetContainerHistory);

  // Capture
  // XXX: Is this applicable to web?

  // Offline Rendering
  // XXX: Is this applicable to web?

  // Secondary Outputs
  // XXX: Is this applicable to web?

  // Event Management
  // TODO: Look into callbacks support
  function("SoundEngine_PostEvent", optional_override([](const std::string& eventName, AkGameObjectID gameObjectID) {
    return AK::SoundEngine::PostEvent(eventName.c_str(), gameObjectID);
  }), allow_raw_pointers());
  function("SoundEngine_ExecuteActionOnEvent", optional_override([](const std::string& in_pszEventName, AK::SoundEngine::AkActionOnEventType in_ActionType, AkGameObjectID in_gameObjectID=AK_INVALID_GAME_OBJECT, AkTimeMs in_uTransitionDuration=0, AkCurveInterpolation in_eFadeCurve=AkCurveInterpolation_Linear, AkPlayingID in_PlayingID=AK_INVALID_PLAYING_ID) {
    return AK::SoundEngine::ExecuteActionOnEvent(in_pszEventName.c_str(), in_ActionType, in_gameObjectID, in_uTransitionDuration, in_eFadeCurve, in_PlayingID);
  }));
  function("SoundEngine_PostMIDIOnEvent", &AK::SoundEngine::PostMIDIOnEvent, allow_raw_pointers());
  function("SoundEngine_StopMIDIOnEvent", &AK::SoundEngine::StopMIDIOnEvent);
  function("PinEventInStreamCache", optional_override([](const std::string& in_pszEventName, AkPriority in_uActivePriority, AkPriority in_uInactivePriority) {
    return AK::SoundEngine::PinEventInStreamCache(in_pszEventName.c_str(), in_uActivePriority, in_uInactivePriority);
  }));
  function("SoundEngine_UnpinEventInStreamCache", optional_override([](const std::string& in_pszEventName) {
    return AK::SoundEngine::UnpinEventInStreamCache(in_pszEventName.c_str());
  }));
  function("SoundEngine_GetBufferStatusForPinnedEvent", optional_override([](const std::string& in_pszEventName, val out_fPercentBuffered, val out_bCachePinnedMemoryFull) {
    AkReal32 percentBuffered;
    bool cachePinnedMemoryFull;
    AKRESULT result = AK::SoundEngine::GetBufferStatusForPinnedEvent(in_pszEventName.c_str(), percentBuffered, cachePinnedMemoryFull);
    if (result == AK_Success) {
      out_fPercentBuffered.set("val", val(percentBuffered));
      out_bCachePinnedMemoryFull.set("val", val(cachePinnedMemoryFull));
    }
    return result;
  }));
  function("SoundEngine_SeekOnEventPosition", optional_override([](const std::string& in_pszEventName, AkGameObjectID in_gameObjectID, AkTimeMs in_iPosition, bool in_bSeekToNearestMarker=false, AkPlayingID in_PlayingID=AK_INVALID_PLAYING_ID) {
    return AK::SoundEngine::SeekOnEvent(in_pszEventName.c_str(), in_gameObjectID, in_iPosition, in_bSeekToNearestMarker, in_PlayingID);
  }));
  function("SoundEngine_SeekOnEventPercent", optional_override([](const std::string& in_pszEventName, AkGameObjectID in_gameObjectID, AkReal32 in_fPercent, bool in_bSeekToNearestMarker=false, AkPlayingID in_PlayingID=AK_INVALID_PLAYING_ID) {
    return AK::SoundEngine::SeekOnEvent(in_pszEventName.c_str(), in_gameObjectID, in_fPercent, in_bSeekToNearestMarker, in_PlayingID);
  }));
  // XXX: Look into callbacks
  // function("CancelEventCallbackCookie", &AK::SoundEngine::CancelEventCallbackCookie);
  // function("CancelEventCallbackGameObject", &AK::SoundEngine::CancelEventCallbackGameObject);
  // function("CancelEventCallback", &AK::SoundEngine::CancelEventCallback);
  function("SoundEngine_GetSourcePlayPosition", &AK::SoundEngine::GetSourcePlayPosition, allow_raw_pointers());
  function("SoundEngine_GetSourcePlayPositions", &AK::SoundEngine::GetSourcePlayPositions, allow_raw_pointers());
  function("SoundEngine_GetSourceStreamBuffering", optional_override([](AkPlayingID in_PlayingID, val out_buffering, val out_bIsBuffering) {
    AkTimeMs buffering;
    bool isBuffering;
    AKRESULT result = AK::SoundEngine::GetSourceStreamBuffering(in_PlayingID, buffering, isBuffering);
    if (result == AK_Success) {
      out_buffering.set("val", val(buffering));
      out_bIsBuffering.set("val", val(isBuffering));
    }
    return result;
  }));
  function("SoundEngine_StopAll", &AK::SoundEngine::StopAll);
  function("SoundEngine_StopPlayingID", &AK::SoundEngine::StopPlayingID);
  function("SoundEngine_ExecuteActionOnPlayingID", &AK::SoundEngine::ExecuteActionOnPlayingID);
  function("SoundEngine_SetRandomSeed", &AK::SoundEngine::SetRandomSeed);
  function("SoundEngine_MuteBackgroundMusic", &AK::SoundEngine::MuteBackgroundMusic);
  function("SoundEngine_GetBackgroundMusicMute", &AK::SoundEngine::GetBackgroundMusicMute);
  // XXX: Is this applicable on the web?
  // function("SendPluginCustomGameData", &AK::SoundEngine::SendPluginCustomGameData);

  // Bank Management
  function("SoundEngine_ClearBanks", &AK::SoundEngine::ClearBanks);
  function("SoundEngine_SetBankLoadIOSettings", &AK::SoundEngine::SetBankLoadIOSettings);
  function("SoundEngine_LoadBank", optional_override([](const std::string& bankId) {
    AkBankID id;
    return AK::SoundEngine::LoadBank(bankId.c_str(), id);
  }));
  // TODO: Look into these.
  // function("SoundEngine_LoadBankMemoryView", &AK::SoundEngine::LoadBankMemoryView);
  // function("SoundEngine_LoadBankMemoryView", &AK::SoundEngine::LoadBankMemoryView);
  // function("SoundEngine_LoadBankMemoryCopy", &AK::SoundEngine::LoadBankMemoryCopy);
  // function("SoundEngine_LoadBankMemoryCopy", &AK::SoundEngine::LoadBankMemoryCopy);
  // function("SoundEngine_DecodeBank", &AK::SoundEngine::DecodeBank);
  // function("SoundEngine_LoadBank", &AK::SoundEngine::LoadBank);
  // function("SoundEngine_LoadBank", &AK::SoundEngine::LoadBank);
  // function("SoundEngine_LoadBankMemoryView", &AK::SoundEngine::LoadBankMemoryView);
  // function("SoundEngine_LoadBankMemoryView", &AK::SoundEngine::LoadBankMemoryView);
  // function("SoundEngine_LoadBankMemoryCopy", &AK::SoundEngine::LoadBankMemoryCopy);
  function("SoundEngine_UnloadBank", optional_override([](const std::string& bankId) {
    return AK::SoundEngine::UnloadBank(bankId.c_str(), nullptr, AkBankType_User);
  }));
  // XXX: All callback related
  // function("SoundEngine_UnloadBank", &AK::SoundEngine::UnloadBank);
  // function("SoundEngine_UnloadBank", &AK::SoundEngine::UnloadBank);
  // function("SoundEngine_CancelBankCallbackCookie", &AK::SoundEngine::CancelBankCallbackCookie);
  // TODO: Double check how to bind string arrays
  function("SoundEngine_PrepareBank", optional_override([](AK::SoundEngine::PreparationType in_PreparationType, const std::string& in_pszString, AK::SoundEngine::AkBankContent in_uFlags=AK::SoundEngine::AkBankContent::AkBankContent_All, AkBankType in_bankType=AkBankType_User) {
    return AK::SoundEngine::PrepareBank(in_PreparationType, in_pszString.c_str(), in_uFlags, in_bankType);
  }));
  // XXX: Callback variant
  // function("SoundEngine_PrepareBank", &AK::SoundEngine::PrepareBank);
  function("SoundEngine_ClearPreparedEvents", &AK::SoundEngine::ClearPreparedEvents);
  // TODO: Double check how to bind string arrays
  // function("SoundEngine_PrepareEvent", optional_override([](PreparationType in_PreparationType, const char** in_ppszString, AkUInt32 in_uNumEvent) {
  //   AkBankID id;
  //   return AK::SoundEngine::PrepareEvent();
  // }));
  // XXX: Callback variant
  // function("SoundEngine_PrepareEvent", &AK::SoundEngine::PrepareEvent);
  // function("SoundEngine_PrepareEvent", &AK::SoundEngine::PrepareEvent);
  // TODO: Look into these
  // function("SoundEngine_SetMedia", &AK::SoundEngine::SetMedia);
  // function("SoundEngine_UnsetMedia", &AK::SoundEngine::UnsetMedia);
  // function("SoundEngine_TryUnsetMedia", &AK::SoundEngine::TryUnsetMedia);
  // TODO: Double check how to bind string arrays
  // function("SoundEngine_PrepareGameSyncs", &AK::SoundEngine::PrepareGameSyncs);
  // function("SoundEngine_PrepareGameSyncs", &AK::SoundEngine::PrepareGameSyncs);
  // XXX: Callback variant
  // function("SoundEngine_PrepareGameSyncs", &AK::SoundEngine::PrepareGameSyncs);
  // function("SoundEngine_PrepareGameSyncs", &AK::SoundEngine::PrepareGameSyncs);

  /**
  * SoundEngine::DynamicDialogue
  */

  // TODO: Double check how to bind string arrays
  // function("SoundEngine_DynamicDialogue_ResolveDialogueEvent", optional_override([](const char *in_pszEventName, const char **in_aArgumentValueNames, AkUInt32 in_uNumArguments, AkPlayingID in_idSequence=AK_INVALID_PLAYING_ID, AkCandidateCallbackFunc in_candidateCallbackFunc=NULL, void *in_pCookie=NULL) {
  //   return AK::SoundEngine::DynamicDialogue::ResolveDialogueEvent();
  // }));
  function("SoundEngine_DynamicDialogue_GetDialogueEventCustomPropertyValueInt", optional_override([](AkUniqueID in_eventID, AkUInt32 in_uPropID, val out_iValue) {
    AkInt32 value;
    AKRESULT result = AK::SoundEngine::DynamicDialogue::GetDialogueEventCustomPropertyValue(in_eventID, in_uPropID, value);
    if (result == AK_Success) {
      out_iValue.set("val", val(value));
    }
    return result;
  }));
  function("SoundEngine_DynamicDialogue_GetDialogueEventCustomPropertyValueFloat", optional_override([](AkUniqueID in_eventID, AkUInt32 in_uPropID, val out_fValue) {
    AkReal32 value;
    AKRESULT result = AK::SoundEngine::DynamicDialogue::GetDialogueEventCustomPropertyValue(in_eventID, in_uPropID, value);
    if (result == AK_Success) {
      out_fValue.set("val", val(value));
    }
    return result;
  }));

  /**
  * SoundEngine::DynamicSequence
  */

  // TODO: Implement namespace and playlists

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

  // Structs
  class_<AK::SoundEngine::Query::GameObjDst>("SoundEngine_Query_GameObjDst")
    .constructor<AkGameObjectID, AkReal32>()
    .property("m_gameObjID", &AK::SoundEngine::Query::GameObjDst::m_gameObjID)
    .property("m_dst", &AK::SoundEngine::Query::GameObjDst::m_dst)
  ;

  // Game Objects
  function("SoundEngine_Query_GetPosition", &AK::SoundEngine::Query::GetPosition);

  // Listeners
  function("SoundEngine_Query_GetListeners", optional_override([](AkGameObjectID in_GameObjectID, val out_ListenerObjectIDs/*, AkUInt32 &oi_uNumListeners*/) {
    // Query required size
    AkUInt32 numListeners;
    AK::SoundEngine::Query::GetListeners(in_GameObjectID, NULL, numListeners);

    // Create a vector of AkGameObjectIDs and then pass pointer with .data()
    std::vector<AkGameObjectID> listenerObjects(numListeners);
    AKRESULT result = AK::SoundEngine::Query::GetListeners(in_GameObjectID, listenerObjects.data(), numListeners);
    if (result == AK_Success) {
      out_ListenerObjectIDs.set("val", val(listenerObjects));
    }
    return result;
  }));
  function("SoundEngine_Query_GetListenerPosition", &AK::SoundEngine::Query::GetListenerPosition, allow_raw_pointers());
  // FIXME: This now runs successfully but throws errors accessing VectorPtrs, look into
  function("SoundEngine_Query_GetListenerSpatialization", optional_override([](AkUInt32 in_uIndex, val out_rbSpatialized, val out_pVolumeOffsets, val out_channelConfig) {
    bool spatialized;
    std::vector<AK::SpeakerVolumes::VectorPtr> volumeOffsets(7);
    AkChannelConfig channelConfig;
    AKRESULT result = AK::SoundEngine::Query::GetListenerSpatialization(in_uIndex, spatialized, volumeOffsets[0], channelConfig);
    if (result == AK_Success) {
      out_rbSpatialized.set("val", val(spatialized));
      out_pVolumeOffsets.set("val", val(volumeOffsets));
      out_channelConfig.set("val", val(channelConfig));
    }
    return result;
  }));

  // Game Syncs
  // Need to split up the in/out values for what was originally io_rValueType
  function("SoundEngine_Query_GetRTPCValue", optional_override([](const std::string& in_pszRtpcName, AkGameObjectID in_gameObjectID, AkPlayingID in_playingID, val out_rValue, AK::SoundEngine::Query::RTPCValue_type in_rValueType, val out_rValueType) {
    AkRtpcValue value;
    AK::SoundEngine::Query::RTPCValue_type valueType = in_rValueType;
    AKRESULT result = AK::SoundEngine::Query::GetRTPCValue(in_pszRtpcName.c_str(), in_gameObjectID, in_playingID, value, valueType);
    if (result == AK_Success) {
      out_rValue.set("val", val(value));
      out_rValueType.set("val", val(valueType));
    }
    return result;
  }));
  function("SoundEngine_Query_GetSwitch", optional_override([](const std::string& in_pstrSwitchGroupName, AkGameObjectID in_GameObj, val out_rSwitchState) {
    AkSwitchStateID switchState;
    AKRESULT result = AK::SoundEngine::Query::GetSwitch(in_pstrSwitchGroupName.c_str(), in_GameObj, switchState);
    if (result == AK_Success) {
      out_rSwitchState.set("val", val(switchState));
    }
    return result;
  }));
  function("SoundEngine_Query_GetState", optional_override([](const std::string& in_pstrStateGroupName, val out_rState) {
    AkStateID state;
    AKRESULT result = AK::SoundEngine::Query::GetState(in_pstrStateGroupName.c_str(), state);
    if (result == AK_Success) {
      out_rState.set("val", val(state));
    }
    return result;
  }));

  // Environments
  // TODO: Fill out more after figuring out outvalue binding issue

  /**
  * SpatialAudio
  */

  // Basic functions
  function("SpatialAudio_Init", optional_override([]() {
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings.
    if (AK::SpatialAudio::Init(settings) != AK_Success) {
      emscripten_run_script("console.error('Could not create the Spatial Audio.')");
      return AK_Fail;
    }

    return AK_Success;
  }));
  function("SpatialAudio_RegisterListener", &AK::SpatialAudio::RegisterListener);
  function("SpatialAudio_UnregisterListener", &AK::SpatialAudio::UnregisterListener);
  function("SpatialAudio_SetGameObjectRadius", &AK::SpatialAudio::SetGameObjectRadius);

  // Helper functions for passing game data to the Reflect plug-in
  function("SpatialAudio_SetImageSource", optional_override([](AkImageSourceID in_srcID, const AkImageSourceSettings &in_info, const std::string& in_name, AkUniqueID in_AuxBusID=AK_INVALID_AUX_ID, AkGameObjectID in_gameObjectID=AK_INVALID_GAME_OBJECT) {
    AK::SpatialAudio::SetImageSource(in_srcID, in_info, in_name.c_str(), in_AuxBusID, in_gameObjectID);
  }));
  function("SpatialAudio_RemoveImageSource", &AK::SpatialAudio::RemoveImageSource);
  function("SpatialAudio_ClearImageSources", &AK::SpatialAudio::ClearImageSources);

  // Geometry
  function("SpatialAudio_SetGeometry", &AK::SpatialAudio::SetGeometry);
  function("SpatialAudio_RemoveGeometry", &AK::SpatialAudio::RemoveGeometry);
  function("SpatialAudio_SetGeometryInstance", &AK::SpatialAudio::SetGeometryInstance);
  function("SpatialAudio_RemoveGeometryInstance", &AK::SpatialAudio::RemoveGeometryInstance);
  // FIXME: Need to get AkReflectionPathInfo to bind correctly for this function to work
  // function("SpatialAudio_QueryReflectionPaths", optional_override([](AkGameObjectID in_gameObjectID, AkUInt32 in_positionIndex, AkVector64 &out_listenerPos, AkVector64 &out_emitterPos, val out_aPaths) {
  //   AkUInt32 arraySize = 8;
  //   std::vector<AkReflectionPathInfo> paths(arraySize);
  //   AKRESULT result = AK::SpatialAudio::QueryReflectionPaths(in_gameObjectID, in_positionIndex, out_listenerPos, out_emitterPos, paths.data(), arraySize);
  //   if (result == AK_Success) {
  //     out_aPaths.set("val", val(paths));
  //   }
  //   return result;
  // }));

  // Rooms and Portals
  function("SpatialAudio_SetRoom", optional_override([](AkRoomID in_RoomID, const AkRoomParams &in_Params, const std::string& in_RoomName=nullptr) {
    return AK::SpatialAudio::SetRoom(in_RoomID, in_Params, in_RoomName.c_str());
  }));
  function("SpatialAudio_RemoveRoom", &AK::SpatialAudio::RemoveRoom);
  function("SpatialAudio_SetPortal", optional_override([](AkPortalID in_PortalID, const AkPortalParams &in_Params, const std::string& in_PortalName=nullptr) {
    return AK::SpatialAudio::SetPortal(in_PortalID, in_Params, in_PortalName.c_str());
  }));
  function("SpatialAudio_RemovePortal", &AK::SpatialAudio::RemovePortal);
  function("SpatialAudio_SetGameObjectInRoom", &AK::SpatialAudio::SetGameObjectInRoom);
  function("SpatialAudio_SetReflectionsOrder", &AK::SpatialAudio::SetReflectionsOrder);
  function("SpatialAudio_SetDiffractionOrder", &AK::SpatialAudio::SetDiffractionOrder);
  function("SpatialAudio_SetNumberOfPrimaryRays", &AK::SpatialAudio::SetNumberOfPrimaryRays);
  function("SpatialAudio_SetLoadBalancingSpread", &AK::SpatialAudio::SetLoadBalancingSpread);
  function("SpatialAudio_SetEarlyReflectionsAuxSend", &AK::SpatialAudio::SetEarlyReflectionsAuxSend);
  function("SpatialAudio_SetEarlyReflectionsVolume", &AK::SpatialAudio::SetEarlyReflectionsVolume);
  function("SpatialAudio_SetPortalObstructionAndOcclusion", &AK::SpatialAudio::SetPortalObstructionAndOcclusion);
  function("SpatialAudio_SetGameObjectToPortalObstruction", &AK::SpatialAudio::SetGameObjectToPortalObstruction);
  function("SpatialAudio_SetPortalToPortalObstruction", &AK::SpatialAudio::SetPortalToPortalObstruction);
  function("SpatialAudio_QueryWetDiffraction", optional_override([](AkPortalID in_portal, val out_wetDiffraction) {
    AkReal32 wetDiffraction;
    AKRESULT result = AK::SpatialAudio::QueryWetDiffraction(in_portal, wetDiffraction);
    if (result == AK_Success) {
      out_wetDiffraction.set("val", val(wetDiffraction));
    }
    return result;
  }));
  // FIXME: Figure out how to bind array of AkDiffractionPathInfos, likely similar situation to AkRefractionPathInfo
  // function("SpatialAudio_QueryDiffractionPaths", optional_override([](AkGameObjectID in_gameObjectID, AkUInt32 in_positionIndex, AkVector64 &out_listenerPos, AkVector64 &out_emitterPos, AkDiffractionPathInfo *out_aPaths, val io_uArraySize) {
  //   AkUInt32 arraySize;
  //   AKRESULT result = AK::SpatialAudio::QueryDiffractionPaths(in_gameObjectID, in_positionIndex, out_listenerPos, out_emitterPos, out_aPaths, arraySize);
  //   if (result == AK_Success) {
  //     io_uArraySize.set("val", val(arraySize));
  //   }
  //   return result;
  // }));
  function("SpatialAudio_ResetStochasticEngine", &AK::SpatialAudio::ResetStochasticEngine);

  // XXX: Does this need to be exposed?
  constant("kOutdoorRoomID", AK::SpatialAudio::kOutdoorRoomID);

  /**
  * SpatialAudio::ReverbEstimation
  */

  function("SpatialAudio_ReverbEstimation_CalculateSlope", &AK::SpatialAudio::ReverbEstimation::CalculateSlope);
  // FIXME: Figure out how to bind input arrays
  // function("GetAverageAbsorptionValues", &AK::SpatialAudio::ReverbEstimation::GetAverageAbsorptionValues, allow_raw_pointers());
  function("SpatialAudio_ReverbEstimation_EstimateT60Decay", optional_override([](AkReal32 in_volumeCubicMeters, AkReal32 in_surfaceAreaSquaredMeters, AkReal32 in_environmentAverageAbsorption, val out_decayEstimate) {
    AkReal32 decayEstimate;
    AKRESULT result = AK::SpatialAudio::ReverbEstimation::EstimateT60Decay(in_volumeCubicMeters, in_surfaceAreaSquaredMeters, in_environmentAverageAbsorption, decayEstimate);
    if (result == AK_Success) {
      out_decayEstimate.set("val", val(decayEstimate));
    }
    return result;
  }));
  function("SpatialAudio_ReverbEstimation_EstimateTimeToFirstReflection", optional_override([](AkVector in_environmentExtentMeters, val out_timeToFirstReflectionMs, AkReal32 in_speedOfSound=343.0f) {
    AkReal32 timeToFirstReflectionMs;
    AKRESULT result = AK::SpatialAudio::ReverbEstimation::EstimateTimeToFirstReflection(in_environmentExtentMeters, timeToFirstReflectionMs, in_speedOfSound);
    if (result == AK_Success) {
      out_timeToFirstReflectionMs.set("val", val(timeToFirstReflectionMs));
    }
    return result;
  }));
  // FIXME: Figure out how to bind input arrays
  // function("SpatialAudio_ReverbEstimation_EstimateHFDamping", &AK::SpatialAudio::ReverbEstimation::EstimateHFDamping, allow_raw_pointers());

  /**
  * StreamMgr
  */

  // Initialization
  function("StreamMgr_Create", optional_override([]() {
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

  // Language Management
  function("StreamMgr_SetCurrentLanguage", optional_override([](const std::string& in_pszLanguageName) {
    return AK::StreamMgr::SetCurrentLanguage(in_pszLanguageName.c_str());
  }));
  function("StreamMgr_GetCurrentLanguage", optional_override([]() {
    return std::string(AK::StreamMgr::GetCurrentLanguage());
  }));
  // XXX: Callback related
  // function("StreamMgr_AddLanguageChangeObserver", &AK::StreamMgr::AddLanguageChangeObserver);
  // function("StreamMgr_RemoveLanguageChangeObserver", &AK::StreamMgr::RemoveLanguageChangeObserver);

}


// XXX: Are these memory allocation hooks usable on web for AkMemSettings?
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