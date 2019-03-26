#ifndef UTILSH
#define UTILSH


#ifndef EIGENH
#define EIGENH
#include <Eigen/Core>
#endif 

namespace utils{

  float randf();

  /**
  * The returned vector originates from the centre of the sphere
  * and is not normalized.
  **/
  Eigen::Vector3f sample_unit_sphere();
  
  /**
  * Sample the unit square until we get a vector that fits in
  * the unit disk.
  **/
  Eigen::Vector3f sample_unit_disk();


  Eigen::Vector3f reflect(const Eigen::Vector3f &v, const Eigen::Vector3f &n);
  bool refract(const Eigen::Vector3f &v, const Eigen::Vector3f &n, float ni_over_nt, Eigen::Vector3f &refracted);

  float schlick(float cosine, float ref_idx);
}


#endif // !UTILSH
