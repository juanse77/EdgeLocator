#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "EdgeLocatorFloatingWindows.h"
#include "BasicEdgeLocator.h"
#include "BasicEdgeLocatorSmoothed.h"
#include "EdgeLocatorFloatingWindowsSmoothed.h"
#include "ImageTest.h"
#include "Edge.h"
#include "getopt.h"
#include "EdgeLocatorFactory.h"
#include <string>
#include<sstream>

#define UP 2490368
#define DOWN 2621440
#define LEFT 2424832
#define RIGHT 2555904

using namespace std;

int moveImage(int x, int direction, int bound, int max_bound, int zoom);
int adjustMargin(int x, int bound, int max_bound, int zoom);

int main(int argc, char** argv)
{
    int order = 2;
    string fileName = "";
    float threshold = 20.f;
    bool save = false;
    int method = 0;
    int radius_in = 20;
    int radius_out = 25;
    bool view_norm = false;

    string command = "\n\tCorrect format:\n\n\t\t" + string(argv[0]) + " -f (fileName | \"test\" [-e (10-60) -i (8-58)]) [-o (1-2)] [-t (10-255)] [-m (0-3)] [-n] [-s]\n"
        "\n\tDefault values:\n\n\t\t-m (0)\n\t\t-o (2)\n\t\t-t (20)\n\t\t-i (20)\n\t\t-e (25)";

    string error = "\nCommand error:\n";

    // Option flags
    bool opts[8];
    
    for (int i = 0; i < 8; i++) {
        opts[i] = false;
    }

    int c;
    while ((c = getopt(argc, argv, "o:t:f:m:i:e:ns")) != -1) {
        
        switch (c) {
            case '?':
                cout << "\nCommand error :\n" + command << endl;
                return -1;
        
            case 'o':
                
                if (opts[0]) {
                    cout << error + "\n\tOption -o repeated." << endl;
                    cout << command << endl;
                    return -4;
                }
                
                order = atoi(optarg);
                
                if (order != 1 && order != 2) {
                    cout << command << endl;
                    return -1;
                }
                opts[0] = true;
                
                break;

            case 't':
                
                if (opts[1]) {
                    cout << error + "\n\tOption -t repeated." << endl;
                    cout << command << endl;
                    return -4;
                }
                
                threshold = atof(optarg);

                if (threshold < 10 || threshold > 255) {
                    cout << command << endl;
                    return -1;
                }

                opts[1] = true;

                break;

            case 'f':
                
                if (opts[2]) {
                    cout << error + "\n\tOption -f repeated." << endl;
                    cout << command << endl;
                    return -4;
                }

                fileName = string(_strdup(optarg));
                opts[2] = true;

                break;
            
            case 'm':
                
                if (opts[3]) {
                    cout << error + "\n\tOption -m repeated." << endl;
                    cout << command << endl;
                    return -4;
                }
                
                method = atoi(optarg);
                
                if (method < 0 || method > 3) {
                    cout << command << endl;
                    return -1;
                }

                opts[3] = true;

                break;

            case 'n':

                if (opts[4]) {
                    cout << error + "\n\tOption -n repeated." << endl;
                    cout << command << endl;
                    return -4;
                }

                view_norm = true;
                opts[4] = true;

                break;
                
            case 's':
                
                if (opts[5]) {
                    cout << error + "\n\tOption -s repeated." << endl;
                    cout << command << endl;
                    return -4;
                }
                
                save = true;
                opts[5] = true;

                break;

            case 'i':
                
                if (opts[6]) {
                    cout << error + "\n\tOption -i repeated." << endl;
                    cout << command << endl;
                    return -4;
                }

                radius_in = atoi(optarg);
                opts[6] = true;

                break;

            case 'e':

                if (opts[7]) {
                    cout << error + "\n\tOption -e repeated." << endl;
                    cout << command << endl;
                    return -4;
                }

                radius_out = atoi(optarg);
                opts[7] = true;

                break;

        }

    }

    if (fileName.compare("") == 0) {
        cout << command << endl;
        return -2;
    }

    cv::Mat src;
    const char* window_name = "Edge Locator";

    EdgeLocator::AbstractEdgeLocator* tmp;
    EdgeLocator::EdgeLocatorFactory elf;

    if (fileName.compare("test") == 0) {

        if(opts[6] && opts[7]){
            
            if (radius_out < 10 || radius_out > 60) {
            
                cout << command << endl;
                return -3;
            
            }else if(radius_in < 8 || radius_in > radius_out - 2){
                
                cout << error + "\n\tMinimun distance betwen radiuses is two." << endl;
                cout << command << endl;
                return -3;

            }
        
        }
        else if(opts[6] || opts[7]){

            cout << error + "\n\tOptions -i and -e unpaired." << endl;
            cout << command << endl;
            return -3;

        }

        float aux_tam = radius_out * 3;
        cv::Size tam(aux_tam, aux_tam);

        cv::Point2f center(aux_tam / 2.f, aux_tam / 2.f);

        EdgeLocator::ImageTest("images/imgTest.jpg", tam, (float)radius_in, (float)radius_out, center, cv::Vec3f(0, 50, 255), cv::Vec3f(255, 255, 255), 20);
        src = cv::imread("images/imgTest.jpg");
    
        tmp = elf.getEdgeLocator(method, src, threshold, order);

        std::stringstream ss;
        ss << "accuracyResults/AccuracyResults-" << radius_in << "_" << radius_out << "-" << elf.method << ".txt";

        string fileResults;
        ss >> fileResults;

        tmp->generateAccuracyResults(tam, radius_in, radius_out, center, fileResults);

    }
    else {
        src = cv::imread("images/" + fileName);
        tmp = elf.getEdgeLocator(method, src, threshold, order);
    }
    
    if (!src.data)
    {
        printf(" No data! -- Exiting the program n");
        return -1;
    }

    // General instructions
    cout << "Edge Locator:" << endl;
    cout << " ------------------" << endl;
    cout << " * [u] -> Zoom in" << endl;
    cout << " * [d] -> Zoom out" << endl;
    cout << " * [UP] -> Move up" << endl;
    cout << " * [DOWN] -> Move down" << endl;
    cout << " * [LEFT] -> Move left" << endl;
    cout << " * [RIGHT] -> Move right" << endl;
    cout << " * [q] -> Close program" << endl;
    
    std::size_t found = fileName.find(".");
    
    if(save){
        tmp->saveEdges("jsonData/" + fileName.substr(0, found) + ".json");
    }

    const int IMAGE_WIDTH = src.cols;
    const int IMAGE_HEIGHT = src.rows;

    const int MAX_WINDOW_WIDTH = 800;
    const int MAX_WINDOW_HEIGHT = 600;

    int zoom = 2;

    int x = 0;
    int y = 0;

    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    bool loop = true;
    while (loop)
    {
        int width = IMAGE_WIDTH * zoom, height = IMAGE_HEIGHT * zoom;

        bool widthOverSize = false;
        bool heightOverSize = false;

        if (IMAGE_WIDTH * zoom > MAX_WINDOW_WIDTH) {
            width = MAX_WINDOW_WIDTH;
            widthOverSize = true;
        }

        if (IMAGE_HEIGHT * zoom > MAX_WINDOW_HEIGHT) {
            height = MAX_WINDOW_HEIGHT;
            heightOverSize = true;
        }

        cv::Mat im_result = tmp->getImageWithEdges(zoom, x, y, width, height, view_norm);
        cv::imshow(window_name, im_result);
        
        int c = cv::waitKeyEx(0);
       
        switch (c) {
        
        case 113: // q
            
            loop = false;
            delete(tmp);

            break;

        case 117: // u
            
            if (zoom < 16)
            {
                zoom *= 2;

                x = x * 2 + width / 2;
                y = y * 2 + height / 2;

                x = adjustMargin(x, IMAGE_WIDTH, MAX_WINDOW_WIDTH, zoom);
                y = adjustMargin(y, IMAGE_HEIGHT, MAX_WINDOW_HEIGHT, zoom);

                cout << " ** Zoom x " << zoom << endl;
            }
            break;

        case 100: // d
            
            if (zoom > 2)
            {
                zoom /= 2;
                
                x = x / 2 - width / 4;
                y = y / 2 - width / 4;

                x = adjustMargin(x, IMAGE_WIDTH, MAX_WINDOW_WIDTH, zoom);
                y = adjustMargin(y, IMAGE_HEIGHT, MAX_WINDOW_HEIGHT, zoom);
                
                cout << " ** Zoom x " << zoom << endl;
            }
            break;

        case UP:
            
            y = moveImage(y, UP, IMAGE_HEIGHT, MAX_WINDOW_HEIGHT, zoom);
            break;

        case DOWN:
            
            y = moveImage(y, DOWN, IMAGE_HEIGHT, MAX_WINDOW_HEIGHT, zoom);
            break;

        case LEFT:
            
            x = moveImage(x, LEFT, IMAGE_WIDTH, MAX_WINDOW_WIDTH, zoom);
            break;

        case RIGHT:
            
            x = moveImage(x, RIGHT, IMAGE_WIDTH, MAX_WINDOW_WIDTH, zoom);
            break;

        }      
    }

    cv::destroyAllWindows();
    return 0;
}

int moveImage(int x, int direction, int bound, int max_bound, int zoom) {

    int increment = 16;

    switch (direction) {

        case UP:
        case LEFT:
            if (x - increment < 0) {
                return 0;
            }
            else {
                return (x - increment);
            }
 
        case DOWN:
        case RIGHT:
            int window_size = bound * zoom;

            if (window_size > max_bound) {
                window_size = max_bound;
            }

            if (x + increment + window_size > bound* zoom) {
                return bound * zoom - window_size;
            }
            else {
                return (x + increment);
            }
    }

    return x;
}

int adjustMargin(int x, int bound, int max_bound, int zoom) {

    int window_size = bound * zoom;

    if (window_size > max_bound) {
        window_size = max_bound;
    }

    if (x + window_size > zoom * bound) {
        return bound * zoom - window_size;
    }
    else if (x < 0) {
        return 0;
    }
    else {
        return x;
    }

}