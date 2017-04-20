# imageConverter
An image to c-code converter for microcontrollers.
It creates a c-file for every image.
The c-file contains:
- an integer variable that stores the image width
- an integer variable that stores the image height
- a byte-array with the pixel data

accepted image formats:
- bmp
- png
- gif
- jpeg

possible output formats:
- black/white (1bit/pixel)
  every byte in the array contains 8 pixel
- 8Bit grayscale (8bit/pixel)
  every byte in the array contains 1 pixel
- RGB444 (12bit/pixel)
  every 3 bytes in the array contains 2 pixel (2x12bit = 24bit)
- RGB565 (16bit/pixel)
  every 2 bytes in the array contains 1 pixel
- RGB666 (18bit/pixel)
  every 3 bytes in the array contains 1 pixel
- RGB888 (24bit/pixel)
  every 3 bytes in the array contains 1 pixel

standard pixel processing direction
- from left to right
- from top to bottom
- possible to change the directions
