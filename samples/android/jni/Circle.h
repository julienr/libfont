#ifndef CIRCLE_H_
#define CIRCLE_H_

//A circle with a radius of 0.5 (unit size diameter)
class Circle {
  public:
    static void create (unsigned numSubdiv=20);

    static void draw (bool enableTexture);
};

#endif
