#ifndef __GPIOS_H
#define __GPIOS_H

namespace gpios {
  static constexpr unsigned int kMotorRightEn  {25};
  static constexpr unsigned int kMotorRightIn1 {26};
  static constexpr unsigned int kMotorRightIn2 {27};

  static constexpr unsigned int kMotorLeftEn   {17};
  static constexpr unsigned int kMotorLeftIn1  {16};
  static constexpr unsigned int kMotorLeftIn2  {4};

  static constexpr unsigned int kBladeMotorPin {5};
}

#endif // __GPIOS_H