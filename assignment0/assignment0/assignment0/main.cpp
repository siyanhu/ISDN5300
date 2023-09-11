#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif
// --------------------------------

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <string>
#include "vecmath.h"
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

#pragma mark - Editting Area
// You will need more global variables to implement color and position changes
bool color_change_indicator = 0;
float current_color_0 = 0.0;
float current_color_1 = 0.1;
float current_color_2 = 0.2;
float current_color_3 = 0.3;

int light_source_indicator = 0;
GLfloat updown_pos = 0.0f;
GLfloat leftright_pos = 0.0f;

string file_path = "torus.obj";

float get_a_random_indicator(void) {
    random_device device;
    mt19937 random_num(device());
    uniform_int_distribution<mt19937::result_type> dist_color(0, 255);
    int int_indicator = dist_color(random_num);
    //(x - min) / (max - min)
    float normalised_indicator = (float)int_indicator/255.0;
    return normalised_indicator;
}

float *update_color_with_a_random_value(void) {
    float indicator_first = get_a_random_indicator();
    float indicator_second = get_a_random_indicator();
    float indicator_third = get_a_random_indicator();
    float indicator_forth = get_a_random_indicator();
//    cout<<"Next Color: "<<indicator_first<<","<<indicator_second<<","<<indicator_third<<","<<indicator_forth<< endl;
    float r[4] = {indicator_first, indicator_second, indicator_third, indicator_forth};
    return r;
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

bool readOBJ(const char * path) {
    
    FILE * file = fopen(path, "r");
    if(file == NULL){
        printf("Impossible to open the file !\n");
        return 0;
    }
    rewind(file);
    char* line = NULL;
    size_t len = 0;
    while ((getline(&line, &len, file)) != -1) {
        if (line[0] == 'v') {
            Vector3f vertex;
            string tag;
            istringstream split_stream(line);
            split_stream >> tag >> vertex.x() >>vertex.y() >>vertex.z();
            ws(split_stream);
            
            if (tag == "v") {
                vecv.push_back(vertex);
            } else if (tag == "vn") {
                vecn.push_back(vertex);
            }
        } else if (line[0] == 'f') {
            string full = line;
            string sub = full.substr(2, full.length() - 1);
            sub = replace_all(sub, "/", " ");
            unsigned a, b, c, d, e, f, g, h, i;
            char* line_new = new char[sub.length() + 1];
            strcpy(line_new, sub.c_str());
            istringstream split_stream(sub);
            split_stream>>a>>b>>c>>d>>e>>f>>g>>h>>i;
            vector<unsigned> face = {a,b,c,d,e,f,g,h,i};
            ws(split_stream);
            vecf.push_back(face);
        }
    }
    fclose(file);
    return 1;
}

bool loadInput(char * path) {
    // load the OBJ file here
    bool success = readOBJ(path);
    if (success == 0) {
        return 0;
    } else {
        for(unsigned int index =0; index < vecf.size(); index++) {
            float a = vecf[index][0];
            float b = vecf[index][1];
            float c = vecf[index][2];
            float d = vecf[index][3];
            float e = vecf[index][4];
            float f = vecf[index][5];
            float g = vecf[index][6];
            float h = vecf[index][7];
            float i = vecf[index][8];
            
            //do something with v[0], v[1], v[2]
            glBegin(GL_TRIANGLES);
            glNormal3d(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
            glVertex3d(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);
            glNormal3d(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
            glVertex3d(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);
            glNormal3d(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
            glVertex3d(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);
            glEnd();
        }
        return 1;
    }
    return 0;
}

#pragma mark - Fixed Functions
// These are convenience functions which allow us to call OpenGL
// methods on Vec3d objects
inline void glVertex(const Vector3f &a)
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a)
{ glNormal3fv(a); }

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

    // Here are some colors you might use - feel free to add more
//    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
//                                 {0.9, 0.5, 0.5, 1.0},
//                                 {0.5, 0.9, 0.3, 1.0},
//                                 {0.3, 0.8, 0.9, 1.0}};
//
//    // Here we use the first color entry as the diffuse color
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[0]);
    
    if (color_change_indicator) {
        float *random_color = update_color_with_a_random_value();
        GLfloat diffColors[4] = {random_color[0], random_color[1], random_color[2], random_color[3]};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors);
        
        current_color_0 = diffColors[0];
        current_color_1 = diffColors[1];
        current_color_2 = diffColors[2];
        current_color_3 = diffColors[3];
        color_change_indicator = 0;
    } else {
        GLfloat diffColors[4] = {current_color_0, current_color_1, current_color_2, current_color_3};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors);
    }
    
    
    // Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

    // Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties
//    void glLightfv(    GLenum light,
//         GLenum pname,
//         const GLfloat * params);
    
//    GLfloat updown_pos = 0.0f;
//    GLfloat leftright_pos = 0.0f;
    switch(light_source_indicator) {
        case 1:
            updown_pos += 0.5f;
            break;
        case 2:
            updown_pos -= 0.5f;
            break;
        case 3:
            leftright_pos  -= 0.5f;
            break;
        case 4:
            leftright_pos += 0.5;
            break;
        default:
            break;
    }
    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
    GLfloat Lt0pos[] = {leftright_pos, updown_pos, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

    // This GLUT method draws a teapot.  You should replace
    // it with code which draws the object you loaded.
    bool load_designated_obj = loadInput("turos.obj");
    
    // Dump the image to the screen.
    glutSwapBuffers();

}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        color_change_indicator = 1;
        glutDisplayFunc( drawScene );
        cout << "Smash C for no matter how many times you want xD" << key << "." << endl;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;
    }

    // this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        light_source_indicator = 1;
        cout << "Light source moving up 0.5: up arrow." << endl;
        break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        light_source_indicator = 2;
        cout << "Light source moving down 0.5: down arrow." << endl;
        break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        light_source_indicator = 3;
        cout << "Light source moving left 0.5: left arrow." << endl;
        break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        light_source_indicator = 4;
        cout << "Light source moving right 0.5: right arrow." << endl;
        break;
    }

    // this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
//    loadInput()
    
    file_path = argv[0];
    cout<<file_path;

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;    // This line is never reached.
}
