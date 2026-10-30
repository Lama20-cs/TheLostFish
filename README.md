# The Lost Fish

A university group project for the **Computer Graphics** course (CS2206), built with **C++** and **OpenG (GLUT)**. It is a 2D animated story about a small fish who gets distracted, loses her parents, and finds her way back to them.

## The Story

The animation has 8 scenes:

1. A fish family swims together.
2. The small fish notices a shiny object on the sea floor and stops, while her parents keep swimming.
3. She dives down toward the object.
4. She rises back up and realizes her parents are gone.
5. She swims left, searching for them.
6. She reaches the entrance of a dark cave.
7. A shark appears, and the small fish escapes.
8. The family reunites, and the screen zooms in on the message *"Stay close to your family."*

## Features

- Hand-drawn 2D graphics using OpenGL primitives (polygons, triangles, lines)
- Gradient backgrounds, seaweed, floating bubbles, and a cave scene
- BMP texture loading for the cave and rock floor
- Smooth animation using timer-based updates
- Transformations (translate, rotate, scale) to move and flip the characters
- Zoom effect in the final scene using orthographic projection
- Keyboard navigation between scenes

## Controls

- **Right arrow:** next scene
- **Left arrow:** previous scene

## Technologies

- C++
- OpenGL / GLUT
- BMP textures (`cava2.bmp`, `rock.bmp`)

## My Contribution

This was a group project. I was responsible for:

- **`drawScene3()`:** the scene where the small fish dives toward the shiny object
- **`drawScene4()`:** the scene where she rises back up and looks around for her parents
- **`drawSeaweed()`:** the seaweed plants on the sea floor
- **`drawBubbles()`:** the floating bubbles
- **`specialKeys()`:** the arrow-key controls for moving between scenes
