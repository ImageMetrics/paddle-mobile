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
#include <iostream>
#include <fstream>

#include "net.h"
#include "loader/loader.h"
#include "math/gemm.h"

using namespace std;

int run();

int main() {
    int run_count = 1;
    for (int i = 0; i < run_count; i++) {
        cout << "start running cycle : " << i << endl;
//        EXCEPTION_HEADER
        run();
//        EXCEPTION_FOOTER
        cout << "end running cycle : " << i << endl;
    }
}

bool is_equal(float a, float b) {
    // epsilon is too strict about correctness
    // return abs(a - b) <= std::numeric_limits<float>::epsilon();
    return abs(a - b) <= 0.001;
}

bool is_correct_result(vector<float> &result) {
    // // the correct result without quantification is : 87.5398 103.573 209.723 196.812
    // vector<float> correct_result{87.5398, 103.573, 209.723, 196.812};
    // the correct result with quantification is : 87.4985 103.567 209.752 196.71
    vector<float> correct_result{64.777f, 101.88f, 210.735f, 199.144f};
    if (result.size() != 4) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        if (!is_equal(result[i], correct_result[i])) {
            return false;
        }
    }
    return true;
}

int find_max(vector<float> data) {
    float max = -1000000;
    int index = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > max) {
            max = data[i];
            index = i + 1;
        }
    }
    return index;
}

int run() {
    // thread num should set 1 while using mobilenet & resnet
    std::shared_ptr<mdl::Loader> loader(new mdl::Loader());
    std::string prefix("/Users/jeffpai/dev/linux/Dockerfile/test/");
    auto t1 = mdl::time();
    bool load_success = loader->load(prefix + "model.min.json", prefix + "data.min.bin");
    auto t2 = mdl::time();
    cout << "load time : " << mdl::time_diff(t1, t2) << "ms" << endl;
    if (!load_success) {
        cout << "load failure" << endl;
        loader->clear();
        return -1;
    }
    if (!loader->get_loaded()) {
        throw_exception("loader is not loaded yet");
    }
  
  const int size = 39 * 39;
  float data[size];
  
  fstream fin(prefix + "input_normalised_transposed.txt", fstream::in);
  for(int i = 0; i < size; ++i)
  {
    fin >> data[i];
//    cout << data[i] << " ";
  }
  fin.close();
    mdl::Net *net = new mdl::Net(loader.get());
  
    int count = 1;
    double total = 0;
    vector<float> result;
    for (int i = 0; i < count; i++) {
        Time t1 = mdl::time();
        result = net->predict(data, "images");
        Time t2 = mdl::time();
        double diff = mdl::time_diff(t1, t2);
        total += diff;
    }
    cout << "total cost: " << total / count << "ms." << endl;
    for (float num: result) {
        cout << num << " ";
    }
    cout <<endl;
    // uncomment while testing clacissification models
//    cout << "the max prob index = "<<find_max(result)<<endl;
    cout << "Done!" << endl;
//    cout << "it " << (is_correct_result(result) ? "is" : "isn't") << " a correct result." << endl;
    delete net;
    return 0;
}
