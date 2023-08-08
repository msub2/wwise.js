// Organize functions to live under namespaces e.g. AK.SoundEngine.Func instead of AK.SoundEngine_Func
// If I can't do it in the binding step at least I can just make it a function call
function organizeNamespaces() {
  const namespaces = {
    Comm: { value: {}, enumerable: true },
    Instrument: { value: {}, enumerable: true },
    MemoryMgr: { value: {}, enumerable: true },
    Monitor: { value: {}, enumerable: true },
    MusicEngine: { value: {}, enumerable: true },
    SoundEngine: { value: {
      DynamicDialogue: {},
      DynamicSequence: {},
      Query: {}
    }, enumerable: true },
    SpatialAudio: { value: {
      ReverbEstimation: {},
    }, enumerable: true },
    SpeakerVolumes: { value: {}, enumerable: true },
    StreamMgr: { value: {}, enumerable: true },
  };
  const nsKeys = Object.keys(namespaces);
  Object.keys(Module).forEach(key => {
    for (const nsKey in nsKeys) {
      if (key.includes(nsKeys[nsKey])) {
        const values = key.split('_');
        if (values.length == 2) {
          const namespace = values[0];
          const item = values[1];
          namespaces[namespace].value[item] = Module[key];
        } else if (values.length == 3) {
          const namespace1 = values[0];
          const namespace2 = values[1];
          const item = values[2];
          namespaces[namespace1].value[namespace2][item] = Module[key];
        }
        delete(Module[key]);
        break;
      }
    }
  })
  Module = Object.defineProperties(Module, namespaces);
}
Module["organizeNamespaces"] = organizeNamespaces;