#if !defined __UTILS_H__
#define __UTILS_H__


#include <math.h>

// Class to store 3d points
class Vector3 {

   public:
      float x;
      float y;
      float z;

      // Constructor
      Vector3(float in_x, float in_y, float in_z) :
         x(in_x), y(in_y), z(in_z) {}

      Vector3() {}

      // Utility methods
      Vector3 crossP(Vector3 const &v) const
      {
         return Vector3(y*v.z - v.y*z, v.x*z - x*v.z, x*v.y - v.x*y);
      }

      float dotP(Vector3 const &v) const
      {
         return x*v.x + y*v.y + z*v.z;
      }

      float length() const
      {
         return sqrtf(x*x + y*y + z*z);
      }
};

#endif
