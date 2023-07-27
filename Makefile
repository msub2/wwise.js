wwise.js: wwise.cpp
	emcc --bind -o dist/wwise.js wwise.cpp \
	include/SoundEngine/Common/AkFilePackageLUT.cpp \
	include/SoundEngine/POSIX/AkDefaultIOHookBlocking.cpp \
	include/SoundEngine/Common/AkMultipleFileLocation.cpp \
	include/SoundEngine/Common/AkGeneratedSoundBanksResolver.cpp \
	-Iinclude -Iinclude/SoundEngine/POSIX \
	lib/Debug/libAkSoundEngine.a lib/Debug/libAkMemoryMgr.a \
	lib/Debug/libAkStreamMgr.a lib/Debug/libAkMusicEngine.a \
	lib/Debug/libAkSpatialAudio.a \
	-s WASM_BIGINT -msse4.1 -msimd128 -fno-rtti -DEMSCRIPTEN_HAS_UNBOUND_TYPE_NAMES=0 -fPIC \
	-s LLD_REPORT_UNDEFINED --no-entry -s EXPORTED_RUNTIME_METHODS=getWasmTableEntry,FS_createPreloadedFile -s FILESYSTEM=1 \
	-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE='["$$Browser"]' -s ALLOW_MEMORY_GROWTH=1 \
	-s MODULARIZE=1 -s EXPORT_NAME="AKModule"