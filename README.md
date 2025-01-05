# Optical Character Recognition

A linux based software that recognizes digital images, reconnects them to form the text using the concepts of machine learning and Model training. 

## How to Run?

1) Clone this repository
2) Install SDL2 and gtk+3
3) Go to the OCR directory, and type on the terminal
  ```
    unset GTK_PATH
    make
  ```
  This command compiles and outputs an executable called UI_Code.exe
  
4) Run the executable: 
  ```
  ./UI_Code
  ```
5) Once a window pops out, you can select an image having digital characters(there are few in given test files), click on run and the images will be processed. 
6) This process will output a file called Result. This file can be found in the current directory.

