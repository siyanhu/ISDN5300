#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include "RayTracer.h"
#include <string.h>

using namespace std;

#include "bitmap_image.hpp"
float clampedDepth(float depthInput, float depthMin, float depthMax);

void depth_saver(const char* depth_out_path, int w, int h, float depth_min, float depth_max, SceneParser& scene) {
    Image image_depth = Image(w, h);
    image_depth.SetAllPixels(Vector3f(0.0f, 0.0f, 0.0f));
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            Vector2f coord = Vector2f((2 * float(i) / (w - 1)) - 1, (2 * float(j) / (h - 1)) - 1);
            Ray r = scene.getCamera()->generateRay(coord);
            Hit h = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));

            if (scene.getGroup()->intersect(r, h, scene.getCamera()->getTMin())) {
                if (h.getT() < depth_min) {
                    image_depth.SetPixel(j, i, Vector3f(1.0f, 1.0f, 1.0f));
                }
                else if (h.getT() > depth_max) {
                    image_depth.SetPixel(j, i, Vector3f(0.0f, 0.0f, 0.0f));
                }
                else {
                    float grayScale = (depth_min - h.getT()) / (depth_max - depth_min);
                    image_depth.SetPixel(i, j, Vector3f(grayScale, grayScale, grayScale));
                }
            }
        }
    }
    image_depth.SaveBMP(depth_out_path);
}

void normal_saver(const char* normal_out_path, int w, int h, SceneParser& scene) {
    Image image_normal = Image(w, h);
    image_normal.SetAllPixels(Vector3f(0.0f, 0.0f, 0.0f));
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            Vector2f coord = Vector2f((2 * float(i) / (w - 1)) - 1, (2 * float(j) / (h - 1)) - 1);
            Ray r = scene.getCamera()->generateRay(coord);
            Hit h = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
            if (scene.getGroup()->intersect(r, h, 0.0f)) {
                Vector3f pixColNorm = h.getNormal().normalized();
                for (int x = 0; x < 3; x++) {
                    if (pixColNorm[x] < 0) {
                        pixColNorm[x] = pixColNorm[x] * -1.0f;
                    }
                }
                image_normal.SetPixel(i, j, pixColNorm);
            }
        }
    }
    image_normal.SaveBMP(normal_out_path);
}

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

    int bounces;
    bool shadows;
    bool jitter;
    bool filter;
    bounces = 4;
    shadows = false;
    jitter = false;
    filter = false;
    
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
        else if (strcmp(p, "-bounces") == 0) {
            if (argc <= i + 1) {
                cout << "ERROR! Not enough parameters -bounces. Try again..." << endl;
                exit(0);
            }
            bounces = atoi(argv[i + 1]);
            i += 1;
        }
        else if (strcmp(p, "-shadows") == 0) {
            shadows = true;
        }
        else if (strcmp(p, "-jitter") == 0) {
            jitter = true;
        }
        else if (strcmp(p, "-filter") == 0) {
            filter = true;
        }
    }

    cout << "The program is going to input file from " << in_path << ", and generate output image at " << out_path
        << " with size (width, height) = [" << to_string(w) << ", " << to_string(h) << "]." << endl;
    if (depth_on) {
        cout << "With depth's paramters (min, max) [" << to_string(depth_min) << ", " << to_string(depth_max) << "], and output depth file at " << depth_out_path << "." << endl;
    }
    if (normal_on) {
        cout<< "With normal's file output at " << normal_out_path << "." << endl;
    }
    cout << "show shadows? " << shadows << endl << "with jitter? " << jitter << endl << "with filter? " << filter << endl;
    
    ///TODO: below demonstrates how to use the provided Image class
    SceneParser scene = SceneParser(in_path);
    RayTracer tracer = RayTracer(&scene, bounces, shadows);
    //image.SetAllPixels(scene.getBackgroundColor());

    if ((jitter == false) && (filter == false)) {
        Image image(w, h);

        for (int ii = 0; ii < w; ii++) {
            for (int jj = 0; jj < h; jj++) {
                Vector2f pos = Vector2f((2.0 * float(ii) / (w - 1)) - 1, (2.0 * float(jj) / (h - 1)) - 1);
                Ray ray = scene.getCamera()->generateRay(pos);
                Hit hit = Hit(FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));

                Vector3f pixel_color = tracer.traceRay(ray, scene.getCamera()->getTMin(), 0, 1.0f, hit);
                image.SetPixel(ii, jj, pixel_color);
            }
        }

        image.SaveBMP(out_path);
        if (depth_on) {
            depth_saver(depth_out_path, w, h, depth_min, depth_max, scene);
        }
        if (normal_on) {
            normal_saver(normal_out_path, w, h, scene);
        }
        exit(0);
    }
    else {
        int width_hres = w * 3;
        int height_hres = h * 3;
        Image image(width_hres, height_hres);
        vector<Vector3f> pixel_colors_all;

        for (int ii = 0; ii < width_hres; ii++) {
            for (int jj = 0; jj < height_hres; jj++) {
                Vector2f pos = Vector2f((2.0 * float(ii) / (width_hres - 1)) - 1, (2.0 * float(jj) / (height_hres - 1)) - 1);
                float r_i = -0.5 + (static_cast <float> (rand())) / (static_cast <float> (RAND_MAX));
                float r_j = -0.5 + (static_cast <float> (rand())) / (static_cast <float> (RAND_MAX));
                Vector2f new_pos = Vector2f(pos[0] + r_i, pos[1] + r_j);
                Ray ray = scene.getCamera()->generateRay(pos);
                Hit hit = Hit(FLT_MAX, NULL, Vector3f(0.0, 0.0, 0.0));

                Vector3f pixel_color = tracer.traceRay(ray, scene.getCamera()->getTMin(), 0, 1.0f, hit);
                pixel_colors_all.push_back(pixel_color);

                if (filter == false) {
                    image.SetPixel(ii, jj, pixel_color);
                }
            }
        }

        if (filter) {

            Image compressed_image(w, h);
            vector<Vector3f> pixel_colors_blur_w;
            vector<Vector3f> pixel_colors_blur_h;
            const float K[] = { 0.1201, 0.2339, 0.2931, 0.2339, 0.1201 };

            for (int ii = 0; ii < width_hres; ii++) {
                for (int jj = 0; jj < height_hres; jj++) {
                    if (jj == 0) {
                        pixel_colors_blur_w.push_back(
                            pixel_colors_all[ii * height_hres + jj] * K[2]
                            + pixel_colors_all[ii * height_hres + jj + 1] * K[3]
                            + pixel_colors_all[ii * height_hres + jj + 2] * K[4]
                        );
                    }
                    else if (jj == 0) {
                        pixel_colors_blur_w.push_back(
                            pixel_colors_all[ii * height_hres + jj - 1] * K[1]
                            + pixel_colors_all[ii * height_hres + jj] * K[2]
                            + pixel_colors_all[ii * height_hres + jj + 1] * K[3]
                            + pixel_colors_all[ii * height_hres + jj + 2] * K[4]
                        );
                    }
                    else if (jj == height_hres - 1) {
                        pixel_colors_blur_w.push_back(
                            pixel_colors_all[ii * height_hres + jj] * K[2]
                            + pixel_colors_all[ii * height_hres + jj - 1] * K[1]
                            + pixel_colors_all[ii * height_hres + jj - 2] * K[0]
                        );
                    }
                    else if (jj == height_hres - 2) {
                        pixel_colors_blur_w.push_back(
                            pixel_colors_all[ii * height_hres + jj + 1] * K[3]
                            + pixel_colors_all[ii * height_hres + jj] * K[2]
                            + pixel_colors_all[ii * height_hres + jj - 1] * K[1]
                            + pixel_colors_all[ii * height_hres + jj - 2] * K[0]
                        );
                    }
                    else {
                        pixel_colors_blur_w.push_back(
                            pixel_colors_all[ii * height_hres + jj - 2] * K[0]
                            + pixel_colors_all[ii * height_hres + jj - 1] * K[1]
                            + pixel_colors_all[ii * height_hres + jj] * K[2]
                            + pixel_colors_all[ii * height_hres + jj + 1] * K[3]
                            + pixel_colors_all[ii * height_hres + jj + 2] * K[4]
                        );
                    }
                }
            }

            for (int jj = 0; jj < height_hres; jj++) {
                for (int ii = 0; ii < width_hres; ii++) {
                    if (ii == 0) {
                        pixel_colors_blur_h.push_back(
                            pixel_colors_blur_w[jj * width_hres + ii] * K[2]
                            + pixel_colors_blur_w[jj * width_hres + ii + 1] * K[3]
                            + pixel_colors_blur_w[jj * width_hres + ii + 2] * K[4]
                        );
                    }
                    else if (ii == 1) {
                        pixel_colors_blur_h.push_back(
                            pixel_colors_blur_w[jj * width_hres + ii - 1] * K[1]
                            + pixel_colors_blur_w[jj * width_hres + ii] * K[2]
                            + pixel_colors_blur_w[jj * width_hres + ii + 1] * K[3]
                            + pixel_colors_blur_w[jj * width_hres + ii + 2] * K[4]
                        );
                    }
                    else if (ii == width_hres - 1) {
                        pixel_colors_blur_h.push_back(
                            pixel_colors_blur_w[jj * width_hres + ii] * K[2]
                            + pixel_colors_blur_w[jj * width_hres + ii - 1] * K[1]
                            + pixel_colors_blur_w[jj * width_hres + ii - 2] * K[0]
                        );
                    }
                    else if (ii == width_hres - 2) {
                        pixel_colors_blur_h.push_back(
                            pixel_colors_blur_w[jj * width_hres + ii + 1] * K[3]
                            + pixel_colors_blur_w[jj * width_hres + ii] * K[2]
                            + pixel_colors_blur_w[jj * width_hres + ii - 1] * K[1]
                            + pixel_colors_blur_w[jj * width_hres + ii - 2] * K[0]
                        );
                    }
                    else {
                        pixel_colors_blur_h.push_back(
                            pixel_colors_blur_w[jj * width_hres + ii - 2] * K[0]
                            + pixel_colors_blur_w[jj * width_hres + ii - 1] * K[1]
                            + pixel_colors_blur_w[jj * width_hres + ii] * K[2]
                            + pixel_colors_blur_w[jj * width_hres + ii + 1] * K[3]
                            + pixel_colors_blur_w[jj * width_hres + ii + 2] * K[4]
                        );
                    }
                }
            }

            //DownSample
            for (int ii = 0; ii < w; ii++) {
                for (int jj = 0; jj < h; jj++) {
                    Vector3f pixel_color_compressed =
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + 0] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + 1] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + 2] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + width_hres + 0] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + width_hres + 1] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + width_hres + 2] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + 2 * width_hres + 0] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + 2 * width_hres + 1] +
                        pixel_colors_blur_w[3 * (ii + width_hres * jj) + 2 * width_hres + 2];
                    pixel_color_compressed = pixel_color_compressed / 9.0f;
                    compressed_image.SetPixel(jj, ii, pixel_color_compressed);
                }
            }

            compressed_image.SaveBMP(out_path);
            if (depth_on) {
                depth_saver(depth_out_path, w, h, depth_min, depth_max, scene);
            }
            if (normal_on) {
                normal_saver(normal_out_path, w, h, scene);
            }
            exit(0);
        }

        image.SaveBMP(out_path);
        if (depth_on) {
            depth_saver(depth_out_path, width_hres, height_hres, depth_min, depth_max, scene);
        }
        if (normal_on) {
            normal_saver(normal_out_path, width_hres, height_hres, scene);
        }
        exit(0);
    }


    ///Should be removed when you start
    // Vector3f pixelColor (1.0f,0,0);
    // width and height
    // Image image( 10 , 15 );
    // image.SetPixel( 5,5, pixelColor );
    // image.SaveImage("demo.bmp");

    //char* test_path = "feet.bmp";
    //Image imagee(w, h);
    //imagee.ReadBmp(test_path);
    //imagee.SaveBMP("output33.bmp");


    return 0;
}

