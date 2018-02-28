# Juggler-Game
=======================================
Task code for game designed as a real time operating system for a Keil Microcontroller.
User controls a platform using a joystick, switching between left, center, and right positions.
Balls drop randomly, and must be bounced at the bottom, where they will launch off the platform with a velocity randomized within a range.
More balls are added until 3 balls are dropped. Each bounce counts for one score.
Hard mode is toggled by pressing push-button. Balls turn red and accelerate downwards faster.
========================================
juggler_game.c is the main code for the game, including tasks and initialization.
