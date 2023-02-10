/*
* tsm_matlab.cpp - using Timed Sparse Matrices in Matlab
*
* Usage : from MATLAB
*     Loading:
*         >> times, tensors, dims = tsm_matlab('load', filename);
*         >> times, vectors = tsm_matlab('load_vectors', filename);
*         >> times, matrices = tsm_matlab('load_matrices', filename);
*     Saving:
*         >> tsm_matlab('save', filename, times, tensors, dims);
*         >> tsm_matlab('save_vectors', filename, times, vectors);
*         >> tsm_matlab('save_matrices', filename, times, matrices);
*
* This is a C++ MEX-file for MATLAB.
* Copyright 2023 Anton Sobinov
*
*/

#include "mex.hpp"
#include "mexAdapter.hpp"
#include "tsm.h"

using namespace std;

const string HELP (
"\nUsage :\n"
"\ttsm('help'), tsm('h'), or just tsm displays this message;\n"
"\tLoading:\n"
"\t\t>> times, tensors, dims = tsm('load', filename);\n"
"\t\t>> times, vectors = tsm('load_vectors', filename);\n"
"\t\t>> times, matrices = tsm('load_matrices', filename);\n"
"\tSaving:\n"
"\t\t>> tsm('save', filename, times, tensors, dims);\n"
"\t\t>> tsm('save_vectors', filename, times, vectors);\n"
"\t\t>> tsm('save_matrices', filename, times, matrices);\n"
);

class MexFunction : public matlab::mex::Function {
public:
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
        matlab::data::ArrayFactory factory;

        // check arguments
        // starting with help messages
        if (inputs.size() == 0) {
            cout << HELP;
            return;
        }
        if (inputs.size() == 1) {
            if (inputs[0].getType() == matlab::data::ArrayType::CHAR)
        }


        // checkArguments(outputs, inputs);
        // double multiplier = inputs[0][0];
        // matlab::data::TypedArray<double> in = std::move(inputs[1]);
        // arrayProduct(in, multiplier);
        // outputs[0] = std::move(in);
    }

    void arrayProduct(matlab::data::TypedArray<double>& inMatrix, double multiplier) {
        
        for (auto& elem : inMatrix) {
            elem *= multiplier;
        }
    }

    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
        matlab::data::ArrayFactory factory;

        if (inputs.size() != 2) {
            matlabPtr->feval(u"error", 
                0, std::vector<matlab::data::Array>({ factory.createScalar("Two inputs required") }));
        }

        if (inputs[0].getNumberOfElements() != 1) {
            matlabPtr->feval(u"error", 
                0, std::vector<matlab::data::Array>({ factory.createScalar("Input multiplier must be a scalar") }));
        }
        
        if (inputs[0].getType() != matlab::data::ArrayType::DOUBLE ||
            inputs[0].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE) {
            matlabPtr->feval(u"error", 
                0, std::vector<matlab::data::Array>({ factory.createScalar("Input multiplier must be a noncomplex scalar double") }));
        }

        if (inputs[1].getType() != matlab::data::ArrayType::DOUBLE ||
            inputs[1].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE) {
            matlabPtr->feval(u"error", 
                0, std::vector<matlab::data::Array>({ factory.createScalar("Input matrix must be type double") }));
        }

        if (inputs[1].getDimensions().size() != 2) {
            matlabPtr->feval(u"error", 
                0, std::vector<matlab::data::Array>({ factory.createScalar("Input must be m-by-n dimension") }));
        }
    }
};
