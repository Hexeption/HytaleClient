HytaleClient - Modded Client
=========================================

What this is
------------
A small modded client and injector for the Hytale game. It provides:
- A DLL client (`Client.dll`) with a module system
- An injector (`Injector.exe`) for loading the client into `HytaleClient.exe`
- ImGui-based in-game UI
- Uses minhook for function hooking

Prerequisites
-------------
- Windows 10/11 (x64)
- Visual Studio 2022 or newer (MSVC toolset)
- CMake (latest recommended)
- Ninja (recommended) or Visual Studio generator

Quick build (from PowerShell)
-----------------------------
Open a Developer PowerShell (x64) or regular PowerShell with the MSVC environment available and run:

```powershell
# from repo root
cmake -S . -B cmake-build-debug -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20
cmake --build cmake-build-debug --target Client -j 14
cmake --build cmake-build-debug --target Injector -j 14
```

After a successful build the main artifacts are in:
- `cmake-build-debug/bin/Client.dll` (client DLL)
- `cmake-build-debug/bin/Injector.exe` (injector)

Run / Inject
-------------
1. Start the official `HytaleClient.exe` (game).
2. Run `Injector.exe` and inject `Client.dll` into the running `HytaleClient.exe` process (or use your preferred injector).
3. Open the in-game ImGui overlay with the `Insert` key (toggle).

License & Use
-------------
Use for research and learning. Respect the game's terms of service and applicable laws. This tooling is provided as-is.
