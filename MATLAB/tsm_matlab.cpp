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
"\t\t>> tsm('save', filename, type, times, tensors, dims, default_value);\n"
"\t\t>> tsm('save_vectors', filename, type, times, vectors, default_value);\n"
"\t\t>> tsm('save_matrices', filename, type, times, matrices, default_value);\n"
"\ttype has to be 'stamps' or 'period'.\n"
"\tdefault_value can be omitted, then 0 will be used.\n"
);

class MexFunction : public matlab::mex::Function
{
private:
    void _warning(const string str) {
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
        matlab::data::ArrayFactory factory;
        matlabPtr->feval(u"warning", 0, std::vector<matlab::data::Array>({ factory.createScalar(str) }));
    }
    void _error(const string str) {
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
        matlab::data::ArrayFactory factory;
        matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar(str) }));
    }

public:
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();
        matlab::data::ArrayFactory factory;

        // check arguments
        bool call_help = false;
        if (inputs.size() == 0)
            call_help = true;

        string firstArgument;
        if (inputs.size() > 0) {
            if (inputs[0].getType() != matlab::data::ArrayType::CHAR) {
                _warning("First argument has to be a character array.");
                call_help = true;
            }
            else {
                firstArgument = matlab::data::CharArray(inputs[0]).toAscii();
            }
        }
        if (!call_help) {
            if ((firstArgument == "help") || (firstArgument == "h"))
                call_help = true;
        }

        // starting with help messages
        if (call_help) {
            cout << HELP;
            return;
        }

        // inputs
        if (inputs.size() < 2) {
            _error("At least two input arguments required.");
            return;
        }

        // get filename
        if (inputs[1].getType() != matlab::data::ArrayType::CHAR) {
            _error("Second argument has to be a filename as a character array.");
            return;
        }
        string filename = matlab::data::CharArray(inputs[1]).toAscii();

        // TEST
        if (firstArgument == "test") {
            outputs[0] = factory.createArray<double>({ 1, 1 }, { 0 });
            return;
        }

        // LOAD
        // standard tensor load
        if (firstArgument == "load") {
            if (outputs.size() != 3) {
                _error("Three outputs required.");
                return;
            }

            TSM::Tsm tsm(filename);
            // times, tensors, dims
            outputs[0] = factory.createArray<double>({1, tsm.time.size()});
            for (size_t i = 0; i < tsm.time.size(); i++)
                outputs[0][0][i] = tsm.time[i];
            cout << "set ou 1" << endl;

            outputs[1] = factory.createArray<double>({ 1, tsm.data.size() });
            for (size_t i = 0; i < tsm.data.size(); i++)
                outputs[1][0][i] = tsm.data[i];
            cout << "set ou 2" << endl;

            outputs[2] = factory.createArray<size_t>({ 1, tsm.dims.size() });
            for (size_t i = 0; i < tsm.dims.size(); i++)
                outputs[2][0][i] = tsm.dims[i];
            cout << "set ou 3" << endl;
            
            return;
        }

        // vector and matrix load
        if ((firstArgument == "load_vectors") || (firstArgument == "load_matrices")) {

            if (outputs.size() != 3) {
                _error("Two outputs required.");
                return;
            }

        }

        // SAVE


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
