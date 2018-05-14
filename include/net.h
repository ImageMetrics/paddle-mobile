/* Copyright (c) 2016 Baidu, Inc. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
==============================================================================*/
#ifndef MDL_NET_H
#define MDL_NET_H

#include "base/layer.h"
#include "math/gemm.h"

#include <memory>

namespace mdl {
    class Loader;

    /**
     * do object detection with Net object
     */
    class Net {
    public:
        /**
         * net init
         * @param config
         * @return
         */
        Net(Loader *loader);

        ~Net();

        /**
         * object detection
         * @param image
         * @param inputName
         * @return vector of float
         */
        vector<float> predict(float *image, const string &inputName = matrix_name_test_data);

        /**
         * forward
         * @param image
         * @param start
         * @param end
         * @param inputName
         * @param sampling
         * @return vector of float
         */
        vector<float> forward_from_to(float *image, int start, int end, const string &inputName = matrix_name_test_data, bool sampling = false);

        /**
         * set the thread num
         * @param thread_num
         */
        void set_thread_num(int thread_num)
        {
            _thread_num = thread_num;
            _gemmers.clear();
            for (int i = 0; i < _thread_num; ++i)
            {
                _gemmers.push_back(std::shared_ptr<Gemmer>(new Gemmer()));
            }
        }

        vector< std::shared_ptr<Gemmer> > &getGemmers()
        {
            return _gemmers;
        }

        Loader *getLoader()
        {
          return _loader;
        }

#ifdef NEED_DUMP

        void dump(string filename);

#endif
    private:
        string _name;

        int _thread_num;

        vector<Layer *> _layers;
      
        Loader * _loader;

        vector< std::shared_ptr<Gemmer> > _gemmers;

#ifdef NEED_DUMP
        void dump_with_quantification(string filename);

        void dump_without_quantification(string filename);
#endif
    };
};

#endif
