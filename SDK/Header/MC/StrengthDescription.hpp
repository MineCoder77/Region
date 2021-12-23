// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "Json.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class StrengthDescription {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_STRENGTHDESCRIPTION
public:
    class StrengthDescription& operator=(class StrengthDescription const&) = delete;
    StrengthDescription(class StrengthDescription const&) = delete;
    StrengthDescription() = delete;
#endif

public:
    /*0*/ virtual char const* getJsonName() const;
    /*1*/ virtual ~StrengthDescription();
    /*2*/ virtual void deserializeData(struct DeserializeDataParams);
    /*3*/ virtual void serializeData(class Json::Value&) const;
    /*
    inline  ~StrengthDescription(){
         (StrengthDescription::*rv)();
        *((void**)&rv) = dlsym("??1StrengthDescription@@UEAA@XZ");
        return (this->*rv)();
    }
    */

protected:

private:

};