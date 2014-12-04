#ifndef __LEVEL_H_INCLUDED__
#define __LEVEL_H_INCLUDED__

#include "EasyBMP.h"

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
#endif
