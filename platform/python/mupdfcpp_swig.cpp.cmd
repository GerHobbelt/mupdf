"swig" \
 \
    -c++ \
    -doxygen \
    -python \
    -Wextra \
    -w-201,-314,-302,-312,-321,-322,-362,-451,-503,-512,-509,-560 \
    -module mupdf \
    -outdir build/shared-release \
    -o platform/python/mupdfcpp_swig.cpp \
    -includeall \
    -I./platform/python/include \
    -Iinclude \
    -Iplatform/c++/include \
    -ignoremissing \
    platform/python/mupdfcpp_swig.i