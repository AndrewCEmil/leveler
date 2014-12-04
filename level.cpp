#include <string>
#include <stack>
#include <set>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <memory>
#include <thread>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "EasyBMP.h"
#include "level.h"

using std::shared_ptr;
using namespace std;

leveler::leveler(int width, int height, int maxframenum, int bitdepth) {
    _width = width;
    _height = height;
    _maxframenum = maxframenum;
    _framenum = 0;
    _bitdepth = bitdepth;
}

void leveler::doWorkLines(BMP& curImage, BMP& previousImage) {
    double curR;
    double curG;
    double curB;
    double newR;
    double newG;
    double newB;
    double curSumR;
    double curSumG;
    double curSumB;
    double adjCount;
    double rowAvgsR[_height] ;
    double collAvgsR[_width];
    double rowAvgsG[_height];
    double collAvgsG[_width];
    double rowAvgsB[_height];
    double collAvgsB[_width];

    //precompute avgs
    for(int i=0;i<_width;i++) {
        curSumR = 0;
        curSumG = 0;
        curSumB = 0;
        for(int j=0;j<_height;j++) {
            curSumR += previousImage.GetPixel(i, j).Red;
            curSumG += previousImage.GetPixel(i, j).Green;
            curSumB += previousImage.GetPixel(i, j).Blue;
        }
        collAvgsR[i] = curSumR / _width;
        collAvgsG[i] = curSumG / _width;
        collAvgsB[i] = curSumB / _width;
    }
    for(int i=0;i<_height;i++) {
        curSumR = 0;
        curSumG = 0;
        curSumB = 0;
        for(int j=0;j<_width;j++) {
            curSumR += previousImage.GetPixel(j, i).Red;
            curSumG += previousImage.GetPixel(j, i).Green;
            curSumB += previousImage.GetPixel(j, i).Blue;
        }
        rowAvgsR[i] = curSumR / _width;
        rowAvgsG[i] = curSumG / _width;
        rowAvgsB[i] = curSumB / _width;
    }

    int curPosX;
    int curPosY;
    for(int i=0;i<_width;i++) {
        for(int j=0;j<_height;j++) {
            //get current values
            curR = previousImage.GetPixel(i, j).Red;
            curG = previousImage.GetPixel(i, j).Green;
            curB = previousImage.GetPixel(i, j).Blue;

            //adjust current
            //TODO check for negative?
            //newR = curR + rowAvgsR[i] + collAvgsR[j] / 3;
            //newG = curG + rowAvgsG[i] + collAvgsG[j] / 3;
            //newB = curB + rowAvgsB[i] + collAvgsB[j] / 3;
            newR = curR - ((curR - ((rowAvgsR[j] + collAvgsR[i])/2)) / 32);
            newG = curG - ((curG - ((rowAvgsG[j] + collAvgsG[i])/2)) / 32);
            newB = curB - ((curB - ((rowAvgsB[j] + collAvgsB[i])/2)) / 32);

            //save into new image
            curImage(i,j)->Red = (int) newR;
            curImage(i,j)->Green = (int) newG;
            curImage(i,j)->Blue = (int) newB;
            if(i == 0 && j == 0) {
                cout << "Previous R: " << curR << endl;
                cout << "AvgR: " << curSumR << endl;
                cout << "New R: " << newR << endl;
                cout << "adjCount: " << adjCount << endl;
                cout << "RightR: " << previousImage.GetPixel(i + 1, j).Red << endl;
                cout << "BottomR: " << previousImage.GetPixel(i, j + 1).Red << endl;
            }
        }
    }
}

void leveler::doWorkAvg(BMP& curImage, BMP& previousImage) {
    double curR;
    double curG;
    double curB;
    double newR;
    double newG;
    double newB;
    double curSumR;
    double curSumG;
    double curSumB;
    double adjCount;
    int curPosX;
    int curPosY;
    for(int i=0;i<_width;i++) {
        for(int j=0;j<_height;j++) {
            //get current values
            curR = previousImage.GetPixel(i, j).Red;
            curG = previousImage.GetPixel(i, j).Green;
            curB = previousImage.GetPixel(i, j).Blue;
            //get surrounding avg
            curSumR = 0;
            curSumG = 0;
            curSumB = 0;
            adjCount = 0;
            for(int x=-1; x<2; x++) {
                for(int y=-1; y<2; y++) {
                    if(!(x == 0 && y ==0)) { //if we are not at the current spot
                        curPosX = i + x;
                        curPosY = j + y;
                        if(curPosX >=0 && //and if we are on the board
                           curPosX <_width &&
                           curPosY >=0 &&
                           curPosY <_height) {
                            curSumR += previousImage.GetPixel(curPosX, curPosY).Red;
                            curSumG += previousImage.GetPixel(curPosX, curPosY).Green;
                            curSumB += previousImage.GetPixel(curPosX, curPosY).Blue;
                            adjCount += 1;
                        }
                    }
                }
            }
            curSumR = curSumR / adjCount;
            curSumG = curSumG / adjCount;
            curSumB = curSumB / adjCount;

            //adjust current
            //TODO check for negative?
            newR = curR  - ((curR - curSumR) / 2);
            newG = curG  - ((curG - curSumG) / 2);
            newB = curB  - ((curB - curSumB) / 2);
            //save into new image
            curImage(i,j)->Red = (int) newR;
            curImage(i,j)->Green = (int) newG;
            curImage(i,j)->Blue = (int) newB;
            if(i == 0 && j == 0) {
                cout << "Previous R: " << curR << endl;
                cout << "AvgR: " << curSumR << endl;
                cout << "New R: " << newR << endl;
                cout << "adjCount: " << adjCount << endl;
                cout << "RightR: " << previousImage.GetPixel(i + 1, j).Red << endl;
                cout << "BottomR: " << previousImage.GetPixel(i, j + 1).Red << endl;
            }
        }
    }
}

void leveler::level(BMP& previousImage) {
    BMP curImage;
    char imgName[13];
    curImage.SetSize(_width, _height);
    curImage.SetBitDepth(_bitdepth);

    doWorkLines(curImage, previousImage);

    sprintf(imgName, "img%05d.bmp", _framenum);
    curImage.WriteToFile(imgName);
    _framenum++;
    if(_framenum >= _maxframenum) {
        cout << "Finished" << endl;
        return;
    } else {
        cout << "Recurring" << endl;
        return level(curImage);
    }

}

BMP getRandomImage(int width, int height, int bitdepth, BMP &curImage) {
    curImage.SetSize(width, height);
    curImage.SetBitDepth(bitdepth);
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            curImage(i,j)->Red = rand() % 256;
            curImage(i,j)->Green = rand() % 256;
            curImage(i,j)->Blue = rand() % 256;
            curImage(i,j)->Alpha = 0;
        }
    }
    return curImage;
}


int main(int argc, char **argv) {
    int opt;
    double curT;
    char *inval = NULL;
    int num_threads = 8;
    int width = 256;
    int height = 256;
    int framecount = 256;
    int bitdepth = 32;
    int seed = time(NULL);
    while ((opt = getopt(argc, argv, "w:h:f:t:b:")) != -1) {
      switch (opt) {
        case 'w':
          width = atoi(optarg);
          break;
        case 'h':
          height = atoi(optarg);
          break;
        case 'f':
          framecount = atoi(optarg);
          break;
        case 't':
          num_threads = atoi(optarg);
          break;
        case 'b':
          bitdepth = atoi(optarg);
          break;
        default:
          cout << "Unknown flag" << endl;
          exit(1);
      }
    }
    //TODO pull into a flag to specify
    BMP *curImage = new BMP();
    curImage->ReadFromFile("/Users/ace/pvid/vgen/nebula.bmp");
    srand(seed);
    //BMP curImage;
    //getRandomImage(height, width, bitdepth, curImage);
    //leveler *lv = new leveler(curImage, height, framecount, bitdepth);
    //lv->level(*curImage);
    cout << "Width: " << curImage->TellWidth() << endl;
    leveler *lv = new leveler(curImage->TellWidth(), curImage->TellHeight(), framecount, bitdepth);
    lv->level(*curImage);
}
