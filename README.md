# wwise.js

*Disclaimer: I am not an Audiokinetic employee and I don't claim to represent them or their products in any official capacity.*

This repository contains ongoing efforts to develop a complete set of WASM bindings to utilize Wwise's Web platform in a pure-JS environemnt.

Everything in here is very much WIP and subject to change.

## Development

### Prerequisites

- GNU Make 4.0+
- Emscripten SDK 3.1.7

### Obtaining SDK files for Web Platform

This repo requires a bit of additional setup before being able to build the `wwise.js`/`wwise.wasm` files. Given that Wwise's Web Platform is still under a technical preview, the necessary SDK files are not immediately accessible from Audiokinetic's website. You will need to follow these steps laid out by [Mike Drummelsmith](mailto:mdrummelsmith@audiokinetic.com) (Head of Licensing at Audiokinetic):
  > First, register a non-commercial project in our system called something like 'XR Web Experiments' or similar.  Visit the website, and under the Get Started menu, click 'Register my Project'.  Then, choose 'Non-Commercial' and 'more than 200 sounds' then fill out the form.  That'll come to us for approval, and once you have been approved, you'll have a trial license to cover your project's needs.
  >
  > Be sure to mark in the Description that you need web platform access.
  >
  > Once that's done (you'll receive a notification when it's approved), grab that key, and from the Launcher you should be able to install the web platform and other required components (SDK and offline documentation).  Then, you should be able to find the instructions for getting set up in the docs.  Look in the SDK docs, under the Web-specific info.  It's all shown in the Wwise Up On Air video from a little while back (you'll have to edit a .wsettings file in order to expose the platform in Wwise itself).

Important to note here is that the included Integration Demo with the SDK is *not* functioning in versions of Wwise newer than 2022.1.3. While I haven't tested too thoroughly I've opted to build with the versions of the files that produce a working version of the Integration Demo for Web, so **I would suggest you use Wwise 2022.1.3**. From limited testing the 2023 builds also seem to have issues.

### Folder structures

Once you have access to the Web Platform, you'll need to create the following folders in this repo and copy the designated folders from the Wwise SDK files:

- `include`
  - `AK` from `<YOUR WWISE INSTALL LOCATION>/SDK/include/AK`
  - `SoundEngine` from `<YOUR WWISE INSTALL LOCATION>/SDK/samples/SoundEngine`
    - You can delete the Win32 folder as we don't need it to compile for Web.
- `lib`
  - `Debug` from `<YOUR WWISE INSTALL LOCATION>/SDK/Emscripten/Debug/lib`
  - `Profile` from `<YOUR WWISE INSTALL LOCATION>/SDK/Emscripten/Profile/lib`
  - `Release` from `<YOUR WWISE INSTALL LOCATION>/SDK/Emscripten/Release/lib`

### Building

Throughout development I gradually worked out the incantation to get these bindings to build. Just run

```sh
make
```

and you should get a copy of `wwise.js` and `wwise.wasm` in the `dist` folder. The makefile is a bit crude since I don't usually work with C/C++, so if anyone knows how to make it any neater that would be appreciated.

### Better VSCode Intellisense

If you're using clangd as your language server in VSCode I found you can add this to your `.vscode/settings.json` for some slightly more helpful hints (won't eliminate all the red squigglies unfortunately, but it's enough to get working Intellisense for writing the bindings).

```json
{
  "clangd.fallbackFlags": [
    "-I${workspaceFolder}/include",
    "-I${workspaceFolder}/include/SoundEngine/POSIX",
    "-I<PATH TO YOUR EMSDK FOLDER>/upstream/emscripten/system/include"
  ]
}
```
