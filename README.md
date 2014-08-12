Planeteher
==========

A pseudorealistic 3d space simulation game written in C++.

[![Build Status](https://travis-ci.org/Jeija/planether.svg?branch=master)](https://travis-ci.org/Jeija/planether)

## [YouTube Video](https://www.youtube.com/watch?v=f7FtJkNxH2M)

## Screenshots
![Earth](http://mesecons.net/random/pscreens/s12.png)
![Saturn](http://mesecons.net/random/pscreens/s6.png)
![Mercury](http://mesecons.net/random/pscreens/s7.png)
![Proxima centauri](http://mesecons.net/random/pscreens/s10.png)
![Flying](http://mesecons.net/random/pscreens/s11.png)

### Aim
Planether is supposed to show how fascinating space exploration and the solar system we live in can be. We can try to describe planets and stars with words, facts and numbers, but one can hardly grasp the enormous relations and distances between planets, stars or moons and how small we really are. Museums, pictures or models can help our imagination, but despite all these we lack a feel of how gigantic our cosmic neighbors really are.

However, there are only some realistic displays of our solar system, e.g. using correct distances, realistic gravity and size and even fewer computer games that can afford to do that. And that is even though computer games have a history of space-related content, starting with "Asteroids" on Atari and continuing today with well-funded titles like "Star Citizen". Of course, a small game like Planether could never compete with games like these, but it has a different niche: Mostly realistic displays of our cosmos, but also fun to play. Therefore, Planether has to combine two aspects: Fun and closeness to reality. Some critical design issues involve:
* **Fun**: As long as it doesn't completely ruin the realistic requirement, fun is the more important aspect. For instance, the planet surfaces are not realistic and there is sound, even though it would be impossible in space.
* **Closeness to reality**: While playing, players incidentally realize start to understand the incredible scale of the solar system or basic laws of physics like gravity. Distance, size and mass are therefore very accurate.
* **Technical coneption**: Planether takes advantage of multicore processors and highly parallelized GPUs for planetary surfaces. Planets and other objects can easily be added to Planether using an easy object-oriented base.
* **Free software** enables portability. Planether uses OpenGL, GLUT, GLM, SOIL, OpenAL and libvorbis for graphics and sound. There are few textures, those supplied are under a free license.

## Installation
### Archlinux
Planether was developed under Archlinux, so it is the preferred platform (but of course other Linuxes work just as well).
* Install dependencies using `sudo pacman -S base-devel git freeglut glew glu openal libogg libvorbis` (You will propably have a lot of it already installed on your system when using a desktop environment)
* Download the sources; go to your preferred directory and `git clone https://github.com/Jeija/planether.git`
* `cd planether`
* `make -j2` (You may also want to use higher numbers after -j on multicore systems for faster compilation)
* Run the game using: `./bin/planether`
* You will also find a starter for the game in the planether directory

### Debian & Ubuntu
* Install dependencies using `sudo apt install g++ make git freeglut3-dev libopenal-dev libglew-dev libglu1-mesa-dev libogg-dev libvorbis-dev`
* Download the sources; go to your preferred directory and `git clone https://github.com/Jeija/planether.git`
* `cd planether`
* `make -j2` (You may also want to use higher numbers after -j on multicore systems for faster compilation)
* Run the game using: `./bin/planether`
* You will also find a starter for the game in the planether directory

### Windows
(not recommended, I told you)

You may not always get the latest version of the game, but you can download precompiled snapshots. Visit the releases section of this GitHub page for downloads.

## Controls
In order of importance.

| Key                 | Function                                                                 |
|:--------------------|:-------------------------------------------------------------------------|
| **V**               | Change camera view (bound, relative with rotation, relative, free)       |
| **Arrow Keys**      | Rotate spaceship                                                         |
| **Mouse Movement**  | Rotate camera (pitch, yaw)                                               |
| **Mouse R/L Keys**  | Rotate camera (roll)                                                     |
| **C**               | Brake spaceship rotation (that was initiated with the arrow keys)        |
| **WASD**            | Move camera in free / relative modes                                     |
| **Space**           | Move camera up in free /relative modes                                   |
| **E**               | Move camera down in free / relative modes                                |
| **Page up**         | Turn on engine                                                           |
| **Page down**       | Turn spaceship in opposite direction of flight                           |
| **T**               | Open teleport dialog: Choose planet to teleport to using right-left      |
|                     | arrow keys and choose teleportation / navigation using up-down keys      |
| **B**               | In navigation mode, move spaceship in direction of planet to navigate to |
| **+/-**             | Timelapse                                                                |
| **F** / **Shift-F** | Switch to fullscreen / switch back to windowed mode                      |
| **esc**             | Exit mouse capturing                                                     |
| **P/O**             | Change seed (during gameplay)                                            |
| **Mouse wheel**     | Fast-forward/backward of camera in space                                 |
| **Q**               | Quit the game                                                            |
| **Numpad**          | Alternative to mouse movement for camera rotation, make sure it uses     |
|                     | number input mode (num key active)                                       |

Depending on your mouse, you may also be able to use other mouse keys to move sideways or up-down with the camera.

## Bugs / Development / Questions
You can report bugs in GitHub issues. New features or patches can be sent via GitHub pull requests (or directly eMailed to me). You can ask questions by eMailing me or using the comment section of the YouTube video for this game (channel norrepli). I may set up a forum and website for this game in case it becomes popular.

Developer documentation is available, just execute `make doxygen` with doxygen installed in the planether directory. Open doc/doxygen/html/index.html in your browser to take a look at it.

## More docmentation? Cross-compilation?
There is more documentation available in [a pdf in German](http://mesecons.net/random/planether_dokumentation.pdf) as this game was written in the course of a school project.

