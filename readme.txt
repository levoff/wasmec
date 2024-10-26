# Setup (Clang/LLVM variant)

- You need to install llvm from Homebrew. Xcode's clang doesn't have support for WebAssembly.

  `brew install llvm`

- (OPTIONAL) This will permanently replace the system llvm with the one from Homebrew:

  `brew link --force llvm`


- Compile with clang explicitly telling which clang want to use IF YOU DO NOT WANT TO DO 'brew link --force llvm': 

  Option 1: 
    PATH="/usr/local/opt/llvm/bin:$PATH" clang --target=wasm32 -O3 -nostdlib -Wl,--no-entry -Wl,--export-all -flto -Wl,--lto-O3 -o cmodule.wasm module.c
  Option 2: 
    PATH="/usr/local/opt/llvm/bin:$PATH" clang --target=wasm32 -O2 -nostdlib -Wl,--no-entry -Wl,--export-all -o cmodule.wasm module.c


- Run python HTTP server:
  
  `python3 -m http.server`



===============================================================================
# Project content

## level0
  - wasm-embedded-base64.html : basic example that draws red rectangle on canvas. WASM code is embedded in JS as base64 string.

## level1
  - index.html + module.c(cmodule.wasm) : basic example of compiling C code into wasm with clang and consuming it from JS to draw on canvas. 

## level2
  - embedded.html : standalone html file with JS script and embedded wasm code(compiled with emscripten). ~46fps(canvas 800x600)
  - index.html + module.c(cmodule.wasm) : compiled with clang/llvm. ~35fps(canvas 800x600)
  - TODO: explore and find out the reason behind 46 vs 35 fps difference. Difference may be either in compiler(emscripten vs clang) or math functions custom implementation. Another reason might be (less likely) that faster one is embedded into JS as base 64 string.

## level3
  - index.html + module.c (cmodule.wasm) : mouse events implemented. Events are handled and the x,y coords passing to the wasm module (c code)
  - olive.c : low level primitive graphics lib
  - wasmath.c, wastring.c : custom implementation of some math and string functions



===============================================================================
### Check out these resources:
 
  -http://cliffle.com/blog/bare-metal-wasm/    
  -https://dassur.ma/things/c-to-webassembly/
  -https://til.simonwillison.net/webassembly/compile-to-wasm-llvm-macos
  -https://compile.fi/canvas-filled-three-ways-js-webassembly-and-webgl/
  -https://hadean.com/blog/using-wasm-webassembly-c-to-run-in-browser/
  -https://schellcode.github.io/webassembly-without-emscripten

