# PaintedEggs

This is a small game created for the [https://itch.io/jam/olc-beat-the-boredom](olc::BeatTheBoredom)
game jam. It is an Easter Egg Hunt.

## Credits

This game was made with the [https://github.com/OneLoneCoder/olcPixelGameEngine/wiki](olc::PixelGameEngine).
Big thanks to [https://www.youtube.com/javidx9](javidx9) and his [http://onelonecoder.com/index.html](OneLoneCoder)
initiative for making tools, videos, and hosting the jam that made this possible.

## Gameplay

Find all 20 eggs within 3 minutes. Use WASD, HJKL, or arrow keys to move.

## Installation
None. Extract the provided Zip and run from there.

## Building
Uses CMake, so you'll need that. On Linux needs OpenGL, LibPNG and X11 development libraries.

Run the following from a command prompt/terminal starting from the directory where you cloned this repository.

 * Windows: Shift+Right Click -> 'Open PowerSheel Here' from this folder.
 * Linux: Right Click -> 'Open in Terminal/Konsole' from this folder.

``bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

## License
This software is made available under the same license as the olc::PixelGameEngine. See LICENSE for details.
