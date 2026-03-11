# clang-bang 💥

Welcome to clang-bang, your ticket out of the C++ configuration labyrinth.

If you've ever spent a weekend trying to get a C++ project to build on Windows instead of actually writing code, you know the pain. This project is a minimalist, smart and delightfully simple template to get you up and running with a modern C++ development environment using LLVM (Clang) on Windows.

No overly complex build matrices, no cryptic wizardry - just a sane, working environment built on top of VSCode. 

## 1. The Big Question: Why LLVM and not MSVC?

You might be asking, "I'm on Windows, why not just use the standard Microsoft compiler?" That's a fair question. Here is what you gain and lose by stepping into the LLVM light.

### The What you gain

* Sanity via Simplicity  
   MSVC is powerful, but it brings its own massive ecosystem and complex CLI. Clang follows a more universally understood Unix-style structure. It's the same friend you'll meet on Linux and macOS.
* Readable Errors  
   Ever had an MSVC template error that spanned three monitors? Clang's error messages are notoriously more human-readable. It actually wants to help you learn and fix your code.
* A Toolset, Not Just a Compiler  
   LLVM isn't just about turning code into binaries. It comes with incredibly powerful toys like `clang-tidy` to catch your mistakes before they become bugs.
* The Magic of Clangd  
   You get access to `clangd`, arguably the fastest and smartest cross-platform language server available. Your IntelliSense will fly.
* Cross-Platform Readiness  
   By basing your environment on VSCode and LLVM instead of Visual Studio, your codebase is naturally primed to be compiled on other operating systems without massive headaches.

### What you lose

* The "It Just Works" Windows Ecosystem  
   MSVC is natively integrated into everything Windows. Sometimes, using Clang on Windows means you have to explicitly tell tools where things are.
* Edge Cases with certain Libraries  
   A few very specific, deeply Windows-entrenched libraries might assume you are using MSVC and complain when you aren't (don't worry, we've got a workaround for that in our triplets).

---

## How to use this template?

It's not a framework, it's a starting point. All you have to do is copy the files you need from here into your own project folder and make the necessary tweaks for your specific setup.

---

## Requirements

Before you copy anything, make sure you have the essentials:

* **LLVM**: Installed and accessible (our triplets can usually find it if it's in the default `Program Files`).
* **Ninja Build System:** This is crucial. `clang-cl` plays best with Ninja on Windows. If you try to use standard generators like MSBuild, you're going to have a bad time. Install Ninja and make sure it's in your system `PATH`.
* **vcpkg:** For managing those sweet dependencies.

---

## 2. Project Structure

We kept it lean. Here is what you're looking at:

* `.vscode/` - The beating heart of the workspace. Contains sample VSCode templates and settings to make the editor behave exactly how we want.
* `vcpkg/` - Your dependency management override directory.
  * `vcpkg/custom-triplets/` - The secret sauce. These are customized instructions telling `vcpkg` exactly how to build libraries using our LLVM setup.
* `.clang-format` - Keeps your code pretty and consistent so you don't argue with yourself about bracket placement.
* `.clang-tidy` - Your strict but fair code reviewer.

---

## 3. The Custom Triplets

When you tell vcpkg to download and build a library, it uses a triplet to know how to build it. We've crafted three custom triplets for LLVM on Windows.

| Triplet Name | Purpose & Linkage | Configuration Details |
| :--- | :--- | :--- |
| **`x64-windows-llvm`** | Standard Dynamic Build. <br>*(Dynamic CRT, Dynamic Library)* | The default go-to. Uses `clang-cl` as the compiler. It gracefully falls back to MSVC for known troublesome ports (like Boost or Qt) to save you from build failures. |
| **`x64-windows-llvm-static`** | Standard Static Build. <br>*(Dynamic CRT, Static Library)* | Same as above, but links libraries statically. Great for when you want fewer `.dll` files floating around your final executable. |
| **`x64-windows-llvm-lto-static`** | Optimized Static Build. <br>*(Dynamic CRT, Static Library)* | The performance beast. It turns on ThinLTO (`-flto=thin`) and uses the incredibly fast LLD linker (`-fuse-ld=lld`) for Release builds. Use this when you want your code to run as fast as possible. |

*Note: You don't need to manually configure the `LLVMInstallDir` environment variable if you installed LLVM in the default Program Files directory. The triplets are smart enough to find it.*

---

## 4. Vcpkg Integration: Escaping the Build Bottleneck

Building C++ libraries from scratch every time you nuke your build folder is a great way to waste your youth. Let's use vcpkg smartly.

### Step A: Using the Triplets

To use these triplets in your project, you just point `vcpkg` to our custom folder. When installing a package, tell it which triplet to use:

```bash
vcpkg install nlohmann-json --overlay-triplets=./vcpkg/custom-triplets --triplet=x64-windows-llvm-static
```

### Step B: The "Export" Trick

Instead of having CMake rebuild or re-evaluate everything directly from the global vcpkg source tree continuously (or trying to manage it across different machines), it's vastly superior to **export** your built dependencies into a neat little self-contained directory.

1. Install the packages first:  
   ```bash
   vcpkg install --overlay-triplets=./vcpkg/custom-triplets --triplet=x64-windows-llvm-static
   ```
   **Note**: This command requires a `vcpkg.json` file. If you do not have such a file you must pass all libraries manually to the `vcpkg install` command. You can find a tiny vcpkg.json file inside this template.

2. Export them raw (or zip):  
   Tell vcpkg to bundle everything you just built into a specific directory.
   ```bash
   vcpkg export  --overlay-triplets=./vcpkg/custom-triplets --raw --output-dir=./exported-x64-windows-llvm-static
   ```

3. Feed CMake:  
   The export command creates a self-contained vcpkg environment inside your output directory (e.g., `./exported-x64-windows-llvm-static/vcpkg-export-...`). Now, you just tell CMake to use this specific folder's toolchain instead of the global one. 
   Simply add this parameter to your CMake configuration command:
   ```bash
   -DCMAKE_TOOLCHAIN_FILE=D:/projects/my/clang-bang/exported-x64-windows-llvm-static/vcpkg-export-.../scripts/buildsystems/vcpkg.cmake
   ```

4. Feed VSCode:  
   To ensure your `c_cpp_properties.json` IntelliSense finds your newly exported headers directly, add the include path:
   ```json
   "includePath": [
       "${workspaceFolder}/**",
       "${workspaceFolder}/exported-x64-windows-llvm-static/vcpkg-export-.../include"
   ]
   ```

Boom. No more bottleneck. Your project instantly finds the headers, and you have a fully portable, snapshotted dependency folder.

---

## 5. Taming VSCode for Debugging and IntelliSense

VSCode is a text editor trying very hard to be an IDE. To make it a *good* C++ IDE, we have to establish some ground rules.

### The Golden Rule: `compile_commands.json`

`clangd` is brilliant, but it's not psychic. It needs to know exactly how your project is compiled to give you accurate IntelliSense, warnings and error highlighting. 

You **must** generate a `compile_commands.json` file. If you are using CMake, this is as simple as adding this to your `CMakeLists.txt`:
```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Once generated (usually in your `build/` folder), `clangd` will read it and magically understand your entire project structure.

### Silencing the Microsoft Conflict

If you have the standard `C/C++` extension by Microsoft installed (which is great for debugging), its IntelliSense engine will fight with `clangd` to the death, melting your CPU in the process.

You need to tell the Microsoft extension to back off. We've set this up in `.vscode/settings.json`, but here is the magic line that disables the default IntelliSense so `clangd` can rule peacefully:

```json
"C_Cpp.intelliSenseEngine": "disabled"
```

### Debugging

You will still use the Microsoft C/C++ extension for the actual debugging process (stepping through code, inspecting variables), which is configured in `.vscode/launch.json`. You get the intelligence of LLVM while typing and the robust debugging engine of Windows when running. It's the best of both worlds.

---

## 6. CMake Examples

### 1. Directly with CLI

Let's look at a real-world example of how to configure and build your project using our setup from the command line with.

```cmd
...\clang-bang\build>set LLVMInstallDir=d:\apps\llvm\bin

...\clang-bang\build>cmake -G "Ninja" -DCMAKE_C_COMPILER="%LLVMInstallDir%\clang-cl.exe" -DCMAKE_CXX_COMPILER="%LLVMInstallDir%\clang-cl.exe" -DCMAKE_TOOLCHAIN_FILE=d:\projects\my\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-llvm-static -DVCPKG_OVERLAY_TRIPLETS=./../vcpkg/custom-triplets -DCMAKE_BUILD_TYPE=Debug ..
-- Running vcpkg install
Detecting compiler hash for triplet x64-windows...
Compiler found: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe
Detecting compiler hash for triplet x64-windows-llvm-static...
Compiler found: D:/apps/llvm/bin/clang-cl.exe
The following packages will be built and installed:
  * boost-assert:x64-windows-llvm-static@1.90.0#1
  * boost-cmake:x64-windows-llvm-static@1.90.0#1
  ...

...\clang-bang\build>cmake --build . --config Debug
[2/2] Linking CXX executable U1.exe

...\clang-bang\build>U1.exe
333ce889-7f04-4095-bd22-53abd12ce6dd
```

### What Actually Happened Here?

1. We set our LLVM path (optional if it's installed in standard locations, but good for custom installs).
2. We fired off the CMake configuration command.
3. **vcpkg intercepted the build:** Before CMake even generated the build files, vcpkg jumped in, saw our custom triplet, found `clang-cl.exe` and started building our dependencies (like `boost`) using LLVM!
4. Finally, we ran `cmake --build .` and Ninja swiftly compiled and linked our executable.

---

Now go forth and write some C++. May your compile times be short and your segfaults be few!

