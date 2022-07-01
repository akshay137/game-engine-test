# Game Engine [C++17|SDL2|OpenGL4.5]

## Features:
  * Open design (no hidden states, no private variables)
  * graphics
    * SDF text rendering (colored | outlines)
    * draw circles or quads (textured | colored | rotated | color_key)
    * batch rendering (2D)
    * framebuffers
  * audio
    * play, pause, stop, audio playback
    * play audio in loop
    * volume control over each audio sample at any time
    * multiple audio playback (simple mixing)

## Build instructions:
* use premake5 to generate build files

depends on:
  1. SDL2 [provided in `ext/` ]
  2. glad [provided in `ext/`]
  3. glm [provided in `ext/`]

_windows build is not tested_

### 12 June 2022
updated this project to use C++, for _speed of development_

# Contains 2 games for demo

---
## Pong
> pong game with few changes.
## changes:
  1. circular ground
  2. pads are balls too!
  3. score points [score only if touched the specific point of ground wall]
---
## Color Switch
---