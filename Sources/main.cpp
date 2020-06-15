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

    string command = "Command error:\n\tCorrect format: " + string(argv[0]) + " -f fileName [-o (1-2)] [-t (0-255)] [-m (0-3)] [-s]";
    int c;
    while ((c = getopt(argc, argv, "o:t:f:m:s")) != -1) {
        
        switch (c) {
            case '?':
                cout << command << endl;
                return -1;
        
            case 'o':
                order = atoi(optarg);
                
                if (order != 1 && order != 2) {
                    cout << command << endl;
                    return -1;
                }
                
                break;

            case 't':
                threshold = atof(optarg);

                if (threshold < 0 || threshold > 255) {
                    cout << command << endl;
                    return -1;
                }

                break;

            case 'f':
                fileName = string(_strdup(optarg));                
                break;
            
            case 'm':
                method = atoi(optarg);
                
                if (method < 0 || method > 3) {
                    cout << command << endl;
                    return -1;
                }

                break;
            
            case 's':
                save = true;
                break;
        }

    }

    if (fileName.compare("") == 0) {
        cout << "Command error:\n\tCorrect format: " + string(argv[0]) + " -f fileName [-o (1-2)] [-t (0-255)] [-s]" << endl;
        return -2;
    }

    /// General instructions
    cout << "Edge Locator:" << endl;
    cout << " ------------------" << endl;
    cout << " * [t] -> View Image Test" << endl;
    cout << " * [u] -> Zoom in" << endl;
    cout << " * [d] -> Zoom out" << endl;
    cout << " * [UP] -> Move up" << endl;
    cout << " * [DOWN] -> Move down" << endl;
    cout << " * [LEFT] -> Move left" << endl;
    cout << " * [RIGHT] -> Move right" << endl;
    cout << " * [q] -> Close program" << endl;

    cv::Mat src;
    const char* window_name = "Edge Locator";

    src = cv::imread("images/" + fileName);

    if (!src.data)
    {
        printf(" No data! -- Exiting the program n");
        return -1;
    }

    EdgeLocator::AbstractEdgeLocator * tmp;
    EdgeLocator::EdgeLocatorFactory elf;

    tmp = elf.getEdgeLocator(method, src, threshold, order);
    std::size_t found = fileName.find(".");
    
    if(save){
        tmp->saveEdges("jsonData/" + fileName.substr(0, found) + ".json");
    }    

    int IMAGE_WIDTH = src.cols;
    int IMAGE_HEIGHT = src.rows;

    const int MAX_WINDOW_WIDTH = 800;
    const int MAX_WINDOW_HEIGHT = 600;

    int zoom = 2;

    int x = 0;
    int y = 0;

    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    bool view_norm = false;
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

        case 116: // t
        {            
            int radius_in = 20;
            int radius_out = 25;

            float aux_tam = radius_out * 3;
            cv::Size tam(aux_tam, aux_tam);

            cv::Point2f center(aux_tam/2.f, aux_tam/2.f);

            EdgeLocator::ImageTest("images/imgTest.jpg", tam, (float)radius_in, (float)radius_out, center, cv::Vec3f(0, 50, 255), cv::Vec3f(255, 255, 255), 20);
            src = cv::imread("images/imgTest.jpg");

            IMAGE_WIDTH = src.cols;
            IMAGE_HEIGHT = src.rows;

            delete(tmp);

            tmp = elf.getEdgeLocator(method, src, threshold, order);

            std::stringstream ss;
            ss << "accuracyResults/AccuracyResults-" << radius_in << "_" << radius_out << "-" << elf.method << ".txt";
            
            string fileResults;
            ss >> fileResults;

            tmp->generateAccuracyResults(tam, radius_in, radius_out, center, fileResults);

            zoom = 2;
            view_norm = true;

            cout << " ** Zoom x " << zoom << endl;

            x = 0;
            y = 0;

        }            
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