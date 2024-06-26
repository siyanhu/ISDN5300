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
    const float gLineLen = 1.0f;
    
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
    void loadBezierCurve(int step, int surfstep, int rev);
    void loadBSplineCurve(int step, int surfstep, int rev);
    void saveObject();

    Vector3f cp0(3.9, 0.0, 0);
    Vector3f cp1(4.2, -1.2, 0);
    Vector3f cp2(4.5, 1.5, 0);
    Vector3f cp3(4.8, -2.5, 0);
    Vector3f cp4(-4.5, 1.1, 0);
    Vector3f cp5(-2.8, 2.3, 0);
    Vector3f cp6(-4.5, 2.5, 0);
    Vector3f cp7(-2.8, 2.7, 0);
    Vector3f cp8(2.5, 1.5, 0);
    Vector3f cp9(2.4, -1.8, 0);
    Vector3f cp10(2.3, 1.5, 0);
    Vector3f cp11(2.2,- 1.8, 0);
    Vector3f cp12(1.8, 2.1, 0);
    Vector3f cp13(- 1.8, 1.9, 0);
    Vector3f cp14(1.0, 1.76, 0);
    Vector3f cp15(-1.0, 1.5, 0);
    int total_curve_num = 4;

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
            case 'b':
            case 'B':
                saveObject();
                break;
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

    vector <Vector3f> normalise_vector_list(const vector<Vector3f>& vec) {
        vector < Vector3f> norm_vec;
        for (int i = 0; i < vec.size(); i++) {
            Vector3f newn (normalise_vector(vec[i]));
            norm_vec.push_back(newn);
        }
        return norm_vec;
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

    void saveObject() {
        // This does OBJ file output
        cerr << endl << "*** writing obj files ***" << endl;
        cout << "Please type the file name: (exclude .obj && Generate files at current directory.)" << endl;
        string prefix;
        cin >> prefix;

        for (unsigned i = 0; i < gSurfaceNames.size(); i++)
        {
            if (gSurfaceNames[i] != ".")
            {
                string filename =
                    prefix + string("_")
                    + gSurfaceNames[i]
                    + string(".obj");

                ofstream out(filename.c_str());

                if (!out)
                {
                    cerr << "\acould not open file " << filename << ", skipping" << endl;
                    out.close();
                    continue;
                }
                else
                {
                    cout << filename << i << endl;
                    outputObjFile(out, gSurfaces[i]);
                    cerr << "wrote " << filename << endl;
                }
            }
        }

    }

    void loadBezierCurve(int step, int surfstep, int rev) {
        
        vector<Curve> cyl_surf;
        for (int i = 0; i < total_curve_num; i++) {
            vector<Vector3f> bz_controlPoints;
            if (i == 0) {
                bz_controlPoints.push_back(cp0);
                bz_controlPoints.push_back(cp1);
                bz_controlPoints.push_back(cp2);
                bz_controlPoints.push_back(cp3);
            }
            else if (i == 1) {
                bz_controlPoints.push_back(cp4);
                bz_controlPoints.push_back(cp5);
                bz_controlPoints.push_back(cp6);
                bz_controlPoints.push_back(cp7);
            }
            else if (i == 2) {
                bz_controlPoints.push_back(cp8);
                bz_controlPoints.push_back(cp9);
                bz_controlPoints.push_back(cp10);
                bz_controlPoints.push_back(cp11);
            }
            else {
                bz_controlPoints.push_back(cp12);
                bz_controlPoints.push_back(cp13);
                bz_controlPoints.push_back(cp14);
                bz_controlPoints.push_back(cp15);
            }

            gCtrlPoints.push_back(normalise_vector_list(bz_controlPoints));
            Curve curve = evalBezier(bz_controlPoints, step);
            gCurves.push_back(curve);

            if (rev == 1) {
                Surface surface = makeSurfRev(curve, step);
                gSurfaces.push_back(surface);
                string surface_index = to_string(i);
                gSurfaceNames.push_back(surface_index);
            }
            else if (rev == 2) {
                cyl_surf.push_back(curve);
            }
        }

        if (rev == 2) {
            for (int i = 0; i < cyl_surf.size() - 1; i+=2) {
                if (i == 0) {
                    continue;
                }
                Curve profile = cyl_surf[i];
                Curve sweep = cyl_surf[i + 1];
                Surface surface = makeGenCyl(profile, sweep);
                gSurfaces.push_back(surface);

                string surface_index = to_string(i);
                gSurfaceNames.push_back(surface_index);

            }
        }
    }

    void loadBSplineCurve(int step, int surfstep, int rev) {

        vector<Curve> cyl_surf;
        for (int i = 0; i < total_curve_num; i++) {
            vector<Vector3f> bz_controlPoints;
            if (i == 0) {
                bz_controlPoints.push_back(cp0);
                bz_controlPoints.push_back(cp1);
                bz_controlPoints.push_back(cp2);
                bz_controlPoints.push_back(cp3);
            }
            else if (i == 1) {
                bz_controlPoints.push_back(cp4);
                bz_controlPoints.push_back(cp5);
                bz_controlPoints.push_back(cp6);
                bz_controlPoints.push_back(cp7);
            }
            else if (i == 2) {
                bz_controlPoints.push_back(cp8);
                bz_controlPoints.push_back(cp9);
                bz_controlPoints.push_back(cp10);
                bz_controlPoints.push_back(cp11);
            }
            else {
                bz_controlPoints.push_back(cp12);
                bz_controlPoints.push_back(cp13);
                bz_controlPoints.push_back(cp14);
                bz_controlPoints.push_back(cp15);
            }

            gCtrlPoints.push_back(normalise_vector_list(bz_controlPoints));
            Curve curve = evalBspline(bz_controlPoints, step);
            gCurves.push_back(curve);

            if (rev == 1) {
                Surface surface = makeSurfRev(curve, step);
                gSurfaces.push_back(surface);
                string surface_index = to_string(i);
                gSurfaceNames.push_back(surface_index);
            }
            else if (rev == 2) {

                cyl_surf.push_back(curve);

            }
        }

        if (rev == 2) {
            for (int i = 0; i < cyl_surf.size() - 1; i += 2) {
                if (i == 0) {
                    continue;
                }
                Curve profile = cyl_surf[i];
                Curve sweep = cyl_surf[i + 1];
                Surface surface = makeGenCyl(profile, sweep);
                gSurfaces.push_back(surface);

                string surface_index = to_string(i);
                gSurfaceNames.push_back(surface_index);
            }
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
                int surfstep;
                int rev;
                cout << endl << "type the steps of each curve:" << endl;
                cin >> step;
                cout << endl << "type the steps of surface:" << endl;
                cin >> surfstep;
                cout << endl << "type 1 for Revolution, 2 for sweep:"<<endl<<"NOTES: preset curves only have two sets for revolution; one sets for sweep." << endl;
                cin >> rev;
                loadBezierCurve(step, surfstep, rev);
            }
            else if (order.compare("BS") == 0) {
                int step;
                int surfstep;
                int rev;
                cout << endl << "type the steps of each curve:" << endl;
                cin >> step;
                cout << endl << "type the steps of surface:" << endl;
                cin >> surfstep;
                cout << endl << "type 1 for Revolution, 2 for sweep:" << endl << "NOTES: preset curves only have two sets for revolution; one sets for sweep." << endl;
                cin >> rev;
                loadBSplineCurve(step, surfstep, rev);
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
