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

pixel::pixel(int red, int green, int blue) {
    _red = red;
    _green = green;
    _blue = blue;
}

volcano::volcano(int xpos, int ypos, int power, color outColor) {
    _xpos = xpos;
    _ypos = ypos;
    _power = power;
    _outColor = outColor;
}

double volcano::getPower(int curx, int cury, int framenum) {
    double xdist = curx - _xpos;
    double ydist = cury - _ypos;
    double totaldist = sqrt(xdist * xdist + ydist * ydist);
    if(totaldist - framenum < 0) {
        totaldist = 1;
    } else if(totaldist == 0) {
        return 256;
    } else {
        totaldist = totaldist - framenum;
    }
    //Distance one should always return full power
    //Distance inf should always return 0
    return _power / totaldist;

}

lava::lava(int width, int height, int maxframenum, int bitdepth) {
    _width = width;
    _height = height;
    _maxframenum = maxframenum;
    _framenum = 0;
    _bitdepth = bitdepth;
}

void lava::init() {
    int xpos;
    int ypos;
    double power;
    color outColor;
    volcano *v;
    //make a bunch of volcanos
    for(int i = 0; i < 5; i++) {
        xpos = rand() % _width;
        ypos = rand() % _height;
        outColor = static_cast<color>(rand() % 3);
        //between .5 and 1
        power = .5 + fmod(((double) rand()) / (RAND_MAX), .5);
        power += 3;
        cout << "Xpos: " << xpos << endl;
        cout << "Ypos: " << ypos << endl;
        cout << "Power: " << power << endl;
        volcanos.push_back(new volcano(xpos, ypos, power, outColor));
    }
    //for now, we put it in the corner
    //volcano *v = new volcano(0,0,1);
    //volcanos.push_back(v);
}

void lava::explode(BMP& prevImage) {
    pixel *addPixel;
    BMP curImage;
    char imgName[13];
    curImage.SetSize(_width, _height);
    curImage.SetBitDepth(_bitdepth);

    for(int i = 0; i < _width; i++) {
        for(int j = 0; j < _height; j++) {
            addPixel = new pixel(0, 0, 0);
            for(int v = 0; v < volcanos.size(); v++) {
                if(volcanos[v]->_outColor == RED) {
                    addPixel->_red += volcanos[v]->getPower(i, j, _framenum);
                } else if(volcanos[v]->_outColor == GREEN) {
                    addPixel->_green += volcanos[v]->getPower(i, j, _framenum);
                } else {
                    addPixel->_blue += volcanos[v]->getPower(i, j, _framenum);
                }
            }

            curImage(i, j)->Red = prevImage(i,j)->Red + addPixel->_red;
            curImage(i, j)->Green = prevImage(i,j)->Green + addPixel->_green;
            curImage(i, j)->Blue = prevImage(i,j)->Blue + addPixel->_blue;
            curImage(i, j)->Alpha = 0;
        }
    }

    sprintf(imgName, "img%05d.bmp", _framenum);
    curImage.WriteToFile(imgName);
    _framenum++;
    if(_framenum >= _maxframenum) {
        cout << "Finished" << endl;
        return;
    } else {
        cout << "Recurring" << endl;
        explode(curImage);
    }
}


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
        }
    }
}

void leveler::level(BMP& previousImage) {
    BMP curImage;
    char imgName[13];
    curImage.SetSize(_width, _height);
    curImage.SetBitDepth(_bitdepth);

    doWorkAvg(curImage, previousImage);

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

BMP getBlackImage(int width, int height, int bitdepth, BMP &curImage) {
    curImage.SetSize(width, height);
    curImage.SetBitDepth(bitdepth);
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            curImage(i,j)->Red = 0;
            curImage(i,j)->Green = 0;
            curImage(i,j)->Blue = 0;
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
    int width = 512;
    int height = 512;
    int framecount = 256;
    int bitdepth = 32;
    int seed = time(NULL);
    BMP *curImage = new BMP();
    while ((opt = getopt(argc, argv, "w:h:f:t:b:i:")) != -1) {
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
        case 'i':
          curImage->ReadFromFile(optarg);
          width = curImage->TellWidth();
          height = curImage->TellHeight();
          break;
        default:
          cout << "Unknown flag" << endl;
          exit(1);
      }
    }
    //TODO pull into a flag to specify
    //curImage->ReadFromFile("/Users/ace/pvid/vgen/nebula.bmp");
    srand(seed);
    getBlackImage(height, width, bitdepth, *curImage);
    //leveler *lv = new leveler(curImage, height, framecount, bitdepth);
    //lv->level(*curImage);
    lava *lv = new lava(curImage->TellWidth(), curImage->TellHeight(), framecount, bitdepth);
    lv->init();
    lv->explode(*curImage);
}
