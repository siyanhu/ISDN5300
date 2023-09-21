#ifdef WIN32
#include <windows.h>
#endif

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif
#include <vecmath.h>

#include "parse.h"
#include "curve.h"
#include "surf.h"
#include "extra.h"
#include "camera.h"
#include "fileio.h"

using namespace std;

// If you're really interested in what "namespace" means, see
// Stroustup.  But basically, the functionality of putting all the
// globals in an "unnamed namespace" is to ensure that everything in
// here is only accessible to code in this file.
namespace
{
    // Global variables here.

    // This is the camera
    Camera camera;

    // These are state variables for the UI
    bool gMousePressed = false;
    int  gCurveMode = 1;
    int  gSurfaceMode = 1;
    int  gPointMode = 1;

    // This detemines how big to draw the normals
    const float gLineLen = 0.1f;
    
    // These are arrays for display lists for each drawing mode.  The
    // convention is that drawmode 0 is "blank", and other drawmodes
    // just call the appropriate display lists.
    GLuint gCurveLists[3];
    GLuint gSurfaceLists[3];
    GLuint gAxisList;
    GLuint gPointList;
   
    // These STL Vectors store the control points, curves, and
    // surfaces that will end up being drawn.  In addition, parallel
    // STL vectors store the names for the curves and surfaces (as
    // given by the files).
    vector<vector<Vector3f> > gCtrlPoints;
    vector<Curve> gCurves;
    vector<string> gCurveNames;
    vector<Surface> gSurfaces;
    vector<string> gSurfaceNames;

    // Declarations of functions whose implementations occur later.
    void arcballRotation(int endX, int endY);
    void keyboardFunc( unsigned char key, int x, int y);
    void specialFunc( int key, int x, int y );
    void mouseFunc(int button, int state, int x, int y);
    void motionFunc(int x, int y);
    void reshapeFunc(int w, int h);
    void drawScene(void);
    void initRendering();
    //void loadObjects(int argc, char *argv[]);
    void makeDisplayLists();

    //DEFINED BY SIYAN
    void loadObjectFile(string filePath);
    void loadBezierCurve(int step);
    void loadBSplineCurve(int step);

    //Vector3f cp0(10.0, 18.0, 11.0);
    //Vector3f cp1(-11.5, 13.0, 12.0);
    //Vector3f cp2(-15.5, 14.0, -13.0);
    //Vector3f cp22(-15.5, 14.0, -13.0);
    //Vector3f cp3(-12.5, 10.0, -14.0);
    //Vector3f cp4(-16.0, -14.0, -15.0);
    //Vector3f cp44(-16.0, -14.0, -15.0);
    //Vector3f cp5(-11.5, -13.0, 15.0);
    //Vector3f cp6(10.0, -18.0, 11.0);
    //Vector3f cp66(10.0, -18.0, -11.0);
    //Vector3f cp7(11.0, -13.0, -14.0);
    //Vector3f cp8(16.0, -15.0, 13.0);
    //Vector3f cp9(13.0, 10.0, 12.0);
    //Vector3f cp10(16.5, 14.5, -11.0);
    //Vector3f cp100(16.5, 14.5, -11.0);
    //Vector3f cp11(11.5, 13.0, -10.0);
    //Vector3f cp12(10.0, 18.0, 10.0);

    Vector3f cp0(0.9, 1.0, 0);
    Vector3f cp1(1.2, -1.2, 0);
    Vector3f cp2(1.5, 1.5, 0);
    Vector3f cp3(1.8, -1.5, 0);
    Vector3f cp4(2.1, 1.5, 0);
    Vector3f cp5(2.3,- 1.8, 0);
    Vector3f cp6(2.5, 1.5, 0);
    Vector3f cp7(2.7, -1.8, 0);
    Vector3f cp8(2.5, 1.5, 0);
    Vector3f cp9(2.4, -1.8, 0);
    Vector3f cp10(2.3, 1.5, 0);
    Vector3f cp11(2.2,- 1.8, 0);
    Vector3f cp12(2.1, 1.8, 0);
    Vector3f cp13(1.9,- 1.8, 0);
    Vector3f cp14(1.7, 1.0, 0);
    Vector3f cp15(1.5,- 1.0, 0);

    //DEFINED BY SIYAN END

    // This function is called whenever a "Normal" key press is
    // received.
    void keyboardFunc( unsigned char key, int x, int y )
    {
        switch ( key )
        {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
        {
            Matrix4f eye = Matrix4f::identity();
            camera.SetRotation(eye);
            camera.SetCenter(Vector3f(0,0,0));
            break;
        }
        case 'c':
        case 'C':
            gCurveMode = (gCurveMode+1)%3;
            break;
        case 's':
        case 'S':
            gSurfaceMode = (gSurfaceMode+1)%3;
            break;
        case 'p':
        case 'P':
            gPointMode = (gPointMode+1)%2;
            break;            
        default:
            cout << "Unhandled key press " << key << "." << endl;        
        }

        glutPostRedisplay();
    }

    // This function is called whenever a "Special" key press is
    // received.  Right now, it does nothing.
    void specialFunc( int key, int x, int y )
    {
		/*
        switch ( key )
        {
		default:
			break;
        }
		*/

        //glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {
        if (state == GLUT_DOWN)
        {
            gMousePressed = true;
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                camera.MouseClick(Camera::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
            gMousePressed = false;
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);        
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        camera.SetPerspective(50);
        camera.ApplyPerspective();
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();              

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        GLfloat Lt0pos[] = {3.0,3.0,5.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        camera.ApplyModelview();

        // Call the relevant display lists.
        if (gSurfaceMode)
            glCallList(gSurfaceLists[gSurfaceMode]);

        if (gCurveMode)
            glCallList(gCurveLists[gCurveMode]);

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if (gMousePressed)
        {
            glPushMatrix();
            glTranslated(camera.GetCenter()[0], camera.GetCenter()[1], camera.GetCenter()[2]);
            glCallList(gAxisList);
            glPopMatrix();
        }

        if (gPointMode)
            glCallList(gPointList);
                 
        // Dump the image to the screen.
        glutSwapBuffers();


    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Antialiasing
        // This looks like crap
        /*
          glEnable(GL_BLEND);
          glEnable(GL_POINT_SMOOTH);
          glEnable(GL_LINE_SMOOTH);
          glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);    
          glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        */
        
        // Clear to black
        glClearColor(0,0,0,1);

        // Base material colors (they don't change)
        GLfloat diffColor[] = {0.4, 0.4, 0.4, 1};
        GLfloat specColor[] = {0.9, 0.9, 0.9, 1};
        GLfloat shininess[] = {50.0};

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }

    void loadObjectFile(string filePath) {
        if (file_exist(filePath.c_str()) == 0) {
            cout << "FILE DOES NOT EXIST." << endl;
            exit(0);
        }

        ifstream in(filePath.c_str());
        if (!in) {
            cerr<< filePath << " not found\a" << endl;
            exit(0);
        }
   
        cerr << endl << "*** loading and constructing curves and surfaces ***" << endl;
    
        if (!parseFile(in, gCtrlPoints,
                       gCurves, gCurveNames,
                       gSurfaces, gSurfaceNames))
        {
            cerr << "\aerror in file format\a" << endl;
            in.close();
            exit(-1);              
        }

        in.close();
        cerr << endl << "*** done ***" << endl;

    }

    void loadBezierCurve(int step) {

        vector<Vector3f> bz_controlPoints;
        bz_controlPoints.push_back(cp0);
        bz_controlPoints.push_back(cp1);
        bz_controlPoints.push_back(cp2);
        //bz_controlPoints.push_back(cp22);
        bz_controlPoints.push_back(cp3);
        bz_controlPoints.push_back(cp4);
        //bz_controlPoints.push_back(cp44);
        bz_controlPoints.push_back(cp5);
        bz_controlPoints.push_back(cp6);
        bz_controlPoints.push_back(cp6);
        bz_controlPoints.push_back(cp7);
        bz_controlPoints.push_back(cp8);
        bz_controlPoints.push_back(cp9);
        bz_controlPoints.push_back(cp10);
        //bz_controlPoints.push_back(cp100);
        bz_controlPoints.push_back(cp11);
        bz_controlPoints.push_back(cp12);
        bz_controlPoints.push_back(cp13);
        bz_controlPoints.push_back(cp14);
        bz_controlPoints.push_back(cp15);
        int curve_num = bz_controlPoints.size();

        for (int cur_index = 0; cur_index < curve_num - 3; cur_index+=3) {

            cout << cur_index << endl;

            Vector3f P0_vector(bz_controlPoints[cur_index + 0]);
            Vector3f P1_vector(bz_controlPoints[cur_index + 1]);
            Vector3f P2_vector (bz_controlPoints[cur_index + 2]);
            Vector3f P3_vector (bz_controlPoints[cur_index + 3]);

            vector<Vector3f> control_points;
            control_points.push_back(P0_vector);
            control_points.push_back(P1_vector);
            control_points.push_back(P2_vector);
            control_points.push_back(P3_vector);

            Curve curve = evalBezier(control_points, step);
            gCurves.push_back(curve);
        }
    }

    void loadBSplineCurve(int step) {

        vector<Vector3f> bz_controlPoints;

        bz_controlPoints.push_back(cp0);
        bz_controlPoints.push_back(cp1);
        bz_controlPoints.push_back(cp2);
        //bz_controlPoints.push_back(cp22);
        bz_controlPoints.push_back(cp3);
        bz_controlPoints.push_back(cp4);
        //bz_controlPoints.push_back(cp44);
        bz_controlPoints.push_back(cp5);
        bz_controlPoints.push_back(cp6);
        bz_controlPoints.push_back(cp6);
        bz_controlPoints.push_back(cp7);
        bz_controlPoints.push_back(cp8);
        bz_controlPoints.push_back(cp9);
        bz_controlPoints.push_back(cp10);
        //bz_controlPoints.push_back(cp100);
        bz_controlPoints.push_back(cp11);
        bz_controlPoints.push_back(cp12);
        bz_controlPoints.push_back(cp13);
        bz_controlPoints.push_back(cp14);
        bz_controlPoints.push_back(cp15);
        bz_controlPoints.push_back(cp15);

        int curve_num = bz_controlPoints.size();

        for (int cur_index = 3; cur_index < curve_num; cur_index += 3) {

            cout << cur_index << endl;

            Vector3f P0_vector(bz_controlPoints[cur_index - 3]);
            Vector3f P1_vector(bz_controlPoints[cur_index - 2]);
            Vector3f P2_vector(bz_controlPoints[cur_index - 1]);
            Vector3f P3_vector(bz_controlPoints[cur_index - 0]);

            vector<Vector3f> control_points;
            control_points.push_back(P0_vector);
            control_points.push_back(P1_vector);
            control_points.push_back(P2_vector);
            control_points.push_back(P3_vector);

            Curve curve = evalBspline(control_points, step);
            gCurves.push_back(curve);
        }
    }

    void makeDisplayLists()
    {
        gCurveLists[1] = glGenLists(1);
        gCurveLists[2] = glGenLists(1);
        gSurfaceLists[1] = glGenLists(1);
        gSurfaceLists[2] = glGenLists(1);
        gAxisList = glGenLists(1);
        gPointList = glGenLists(1);

        // Compile the display lists
        
        glNewList(gCurveLists[1], GL_COMPILE);
        {
            for (unsigned i=0; i<gCurves.size(); i++)
                drawCurve(gCurves[i],0.1f);
        }
        glEndList();
                
        glNewList(gCurveLists[2], GL_COMPILE);
        {
            for (unsigned i=0; i<gCurves.size(); i++)
                drawCurve(gCurves[i], gLineLen);
        }
        glEndList();
        
        glNewList(gSurfaceLists[1], GL_COMPILE);
        {
            for (unsigned i=0; i<gSurfaces.size(); i++)
                drawSurface(gSurfaces[i], true);
        }
        glEndList();

        glNewList(gSurfaceLists[2], GL_COMPILE);
        {
            for (unsigned i=0; i<gSurfaces.size(); i++)
            {
                drawSurface(gSurfaces[i], false);
                drawNormals(gSurfaces[i], gLineLen);
            }
        }
        glEndList();

        glNewList(gAxisList, GL_COMPILE);
        {
            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3d(0,0,0); glVertex3d(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3d(0,0,0); glVertex3d(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3d(0,0,0); glVertex3d(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3d(0,0,0); glVertex3d(-1,0,0);
            glVertex3d(0,0,0); glVertex3d(0,-1,0);
            glVertex3d(0,0,0); glVertex3d(0,0,-1);
        
            glEnd();
            glPopMatrix();
            
            glPopAttrib();
        }
        glEndList();

        glNewList(gPointList, GL_COMPILE);
        {
            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // Setup for point drawing
            glDisable(GL_LIGHTING);    
            glColor4f(1,1,0.0,1);
            glPointSize(4);
            glLineWidth(1);

            for (unsigned i=0; i<gCtrlPoints.size(); i++)
            {
                glBegin(GL_POINTS);
                for (unsigned j=0; j<gCtrlPoints[i].size(); j++)
                    glVertex(gCtrlPoints[i][j]);
                glEnd();

                glBegin(GL_LINE_STRIP);
                for (unsigned j=0; j<gCtrlPoints[i].size(); j++)
                    glVertex(gCtrlPoints[i][j]);
                glEnd();
            }
            
            glPopAttrib();
        }
        glEndList();

    }
    
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char* argv[] )
{

    glutInit(&argc, argv);

    // Load in from standard input
    cout << "Input your swp file. Or type \"CC\" to continue loading curves." << endl;
    string order;
    cin>>order;

    if (order.compare("CC") != 0) {
        loadObjectFile(order);
    }
    else {
        cout << "Type BE for bezier curve; type BS for b-spline:" << endl;
        cin >> order;
        if (order.length() <= 0) {
            cout << "ILLEGAL COMMAND: " << order << endl;
            exit(0);
        }
        else {
            if (order.compare("BE") == 0) {
                int step;
                cout << endl << "type the steps of each curve:" << endl;
                cin >> step;
                loadBezierCurve(step);
            }
            else if (order.compare("BS") == 0) {
                int step;
                cout << endl << "type the steps of each curve:" << endl;
                cin >> step;
                loadBSplineCurve(step);
            }
            else {
                cout << "ILLEGAL COMMAND: " << order << endl;
                exit(0);
            }
        }
    }
    
    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 1260, 780 );
    
    camera.SetDimensions(600, 600);

    camera.SetDistance(10);
    camera.SetCenter(Vector3f(0,0,0));
    
    glutCreateWindow("Assignment 1");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
    //  glutTimerFunc(20, timerFunc, 0);

    makeDisplayLists();
        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return 0;	// This line is never reached.
}
