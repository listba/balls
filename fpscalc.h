#ifndef FPSCALC_H
#define FPSCALC_H

class fpscalc {
public:
   fpscalc();
   int getfps();
   void tick();
private:
   int fps;
   int frames;
   int lastupdate;
};

#endif
