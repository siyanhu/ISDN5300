#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth(float depthInput, float depthMin, float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
    // Fill in your implementation here.
    char* in_path, * out_path;
    int w, h;

    float depth_min, depth_max;
    char* depth_out_path, * normal_out_path;
    bool depth_on, normal_on;
    depth_on = false;
    normal_on = false;
    
    // This loop loops over each of the input arguments.
    // argNum is initialized to 1 because the first
    // "argument" provided to the program is actually the
    // name of the executable (in our case, "a4").
    // for( int argNum = 1; argNum < argc; ++argNum ) {
    //   std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    // }
    
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.

    for (int i = 1; i < argc; i++) {
        char* p = argv[i];
        if (strcmp(p, "-input") == 0) {
            if (argc <= i + 1) {
                cout << "ERROR! Not enough parameters -input. Try again..." << endl;
                exit(0);
            }

            in_path = argv[i + 1];
            i += 1;
        }
        else if (strcmp(p, "-output") == 0) {
            if (argc <= i + 1) {
                cout << "ERROR! Not enough parameters -output. Try again..." << endl;
                exit(0);
            }

            out_path = argv[i + 1];
            i += 1;
        }
        else if (strcmp(p, "-size") == 0) {
            if (argc <= i + 2) {
                cout << "ERROR! Not enough parameters -size. Try again..." << endl;
                exit(0);
            }

            w = atoi(argv[i + 1]);
            h = atoi(argv[i + 2]);
            i += 2;
        }
        else if (strcmp(p, "-depth") == 0) {
            if (argc <= i + 3) {
                cout << "ERROR! Not enough parameters - depth. Try again..." << endl;
                exit(0);
            }

            depth_on = true;
            depth_min = atof(argv[i + 1]);
            depth_max = atof(argv[i + 2]);
            depth_out_path = argv[i + 3];
            i += 3;
        }
        else if (strcmp(p, "-normal") == 0) {
            if (argc <= i + 1) {
                cout << "ERROR! Not enough parameters -normal. Try again..." << endl;
                exit(0);
            }

            normal_on = true;
            normal_out_path = argv[i + 1];
            i += 1;
        }
    }

    cout << "The program is going to input file at " << in_path << ", and generate output image at " << out_path
        << " with size (width, height) = [" << to_string(w) << ", " << to_string(h) << "]." << endl;
    if (depth_on) {
        cout << "With depth's paramters (min, max) [" << to_string(depth_min) << ", " << to_string(depth_max) << "], and output depth file at " << depth_out_path << "." << endl;
    }
    if (normal_on) {
        cout<< "With normal's file output at " << normal_out_path << "." << endl;
    }
    
    ///TODO: below demonstrates how to use the provided Image class
    SceneParser scene = SceneParser(in_path);
    Image image(w, h);
    image.SetAllPixels(scene.getBackgroundColor());

    for (int ii = 0; ii < w; ii++) {
        for (int jj = 0; jj < h; jj++) {
            Vector2f pos = Vector2f((2.0 * float(ii) / (w - 1)) - 1, (2.0 * float(jj) / (h - 1)) - 1);
            Ray ray = scene.getCamera()->generateRay(pos);
            Hit hit = Hit(FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));

            bool goint_to_intersect = false;
            goint_to_intersect = scene.getGroup()->intersect(ray, hit, scene.getCamera()->getTMin());
            if (goint_to_intersect == 1) {
                Vector3f pixel_colol = Vector3f(0.0, 0.0, 0.0);

                for (int l_num = 0; l_num < scene.getNumLights(); l_num++) {
                    Light* light = scene.getLight(l_num);

                    Vector3f light_dir, light_col;
                    float dist_2_light;

                    light->getIllumination(ray.pointAtParameter(hit.getT()), light_dir, light_col, dist_2_light);
                    //cout << "col " << light_dir[0] << ", " << light_dir[1] << ", " << light_dir[2] << endl;

                    Vector3f shading_col = hit.getMaterial()->Shade(ray, hit, light_dir, light_col);
                    pixel_colol += shading_col;
                    //cout << shading_col[0] << "," << shading_col[1] << ", " << shading_col[2] << "," << pixel_colol[0] << "," << pixel_colol[1] << "," << pixel_colol[2] << endl;
                }

                pixel_colol += hit.getMaterial()->getDiffuseColor() * scene.getAmbientLight();
                image.SetPixel(ii, jj, pixel_colol);
                //cout << ii << "," << jj << "," << pixel_colol[0] << "," << pixel_colol[1] << "," << pixel_colol[2] << endl;
            }
        }
    }

    image.SaveBMP(out_path);

    ///Should be removed when you start
    // Vector3f pixelColor (1.0f,0,0);
    // width and height
    // Image image( 10 , 15 );
    // image.SetPixel( 5,5, pixelColor );
    // image.SaveImage("demo.bmp");

    char* test_path = "feet.bmp";
    Image imagee(w, h);
    imagee.ReadBmp(test_path);
    imagee.SaveBMP("output33.bmp");


    return 0;
}

