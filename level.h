#ifndef __LEVEL_H_INCLUDED__
#define __LEVEL_H_INCLUDED__

#include "EasyBMP.h"
#include <vector>

using namespace std;

enum color { RED, GREEN, BLUE };

class pixel {
  public:
    pixel(int red, int green, int blue);
    int _red;
    int _green;
    int _blue;
};

class leveler {
  private:
    int _width;
    int _height;
    int _maxframenum;
    int _framenum;
    int _bitdepth;
  public:
    leveler(int width, int height, int maxframenum, int bitdepth); 
    void level(BMP& previousImage);
    void doWorkAvg(BMP& curImage, BMP& previousImage);
    void doWorkLines(BMP& curImage, BMP& previousImage);
};

class volcano {
  private:
    int _xpos;
    int _ypos;
    double _power;
  public:
    volcano(int xpos, int ypos, int power, color outColor);
    double getPower(int curx, int cury, int framenum);
    color _outColor;
};

class lava {
  private:
    int _width;
    int _height;
    int _maxframenum;
    int _framenum;
    int _bitdepth;
    vector<volcano *> volcanos;
  public:
    lava(int width, int height, int maxframenum, int bitdepth);
    void init();
    void explode(BMP& prevImage);
};
#endif
