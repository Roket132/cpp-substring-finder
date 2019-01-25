#include "data.h"

data::data() {}

data &data::get_instance()
{
    static data instance;
    return instance;
}
