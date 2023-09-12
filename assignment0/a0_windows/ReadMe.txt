****************************************************
USER NOTICE:
1. Run the code and add obj file name with -f as flag but not <. I have compilation error with "<". So i have updated the flat with "-f". 
    For example, ./a0 -f sphere.obj.
    if there is any error while streaming file, default teapot will be loaded.
2. Long press/smash c button: change color;
3. Long press/smash r button: spin;
4. Long press/smash up/down/left/right: change light source.
5. Click your mouse left/right button: move the camera's position.

****************************************************
[FIRST QUESTION: ANY COLLABORATION WITH CLASSMATES?]
No. I have completed the tasks on my own.

[SECOND QUESTION: REFERENCE]
1. parse arguements for c++: https://gist.github.com/lcosta/2854864.
2. gluLookAt: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml.
3. glutMouseFunc: https://www.opengl.org/resources/libraries/glut/spec3/node50.html.

[THIRD QUESTION: KNOWN PROBLMES?]
Yes. It requires a couple of seconds to load large mesh files.

[FORTH QUESTION: EXTRA CREDIT]
1. The sample solution (a0soln) lets you hit r to spin the model. Implement this functionality in your code (look up glutTimerFunc): 
    press r button whenever you want and for no matter how many times.
2. Modify the code so that the c key smoothly transitions between different colors (rather than just toggling it): 
    press c button whenever you want and for no matter how many times, and check a disco mesh.
3. Implement a mouse-based camera control to allow the user to rotate and zoom in on the object. Credit will vary depending on the quality of the implementation: 
    click mouse-left button to move towards left-lower corner, and click mouse-right button to move diagnally.

[FIFTH QUESTION]
I initially completed the coding and tested on Macbook Pro with Apple Silicon M1. Apple Clang does not have a full support for our requirements. So i have to borrow a windows from friends and finish the task.
I suggest there could probably be an update on makefile so that everyone could finish the tasks with less effort. Thank you all.