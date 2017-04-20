# imageConverter
An image to c-code converter for microcontrollers.
It creates a c-file for every image that contains:
- an integer variable that stores the image width
- an integer variable that stores the image height
- a byte-array with the pixel data

accepted image formats:
- bmp
- png
- gif
- jpeg

possible output formats:
- Schwarz/weis (1Bit/Pixel)
- 8Bit Graustufen (8Bit/Pixel)
- RGB444 (12Bit/Pixel)
- RGB565 (16Bit/Pixel)
- RGB666 (18Bit/Pixel)
- RGB888 (24Bit/Pixel)

standard pixel processing direction
- from left to right
- from top to bottom
- possible to change the directions
