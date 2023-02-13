/*
* tsm_matlab.cpp - using Timed Sparse Matrices in MATLAB
*
* Usage : from MATLAB
*     Loading:
*         >> times, tensor_data, dims = tsm('load', filename);
*         >> times, formatted_tensors = tsm_load(filename); -- MATLAB wrapper
*     Saving:
*         >> tsm('save', filename, times, tensor_data, dims);
*         >> tsm_save(filename, times, formatted_tensors); -- MATLAB wrapper
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
"\t\t>> times, tensor_data, dims = tsm('load', filename);\n"
"\t\t>> times, formatted_tensors = tsm_load(filename); -- MATLAB wrapper\n"
"\tSaving:\n"
"\t\t>> tsm('save', filename, type, times, tensor_data, dims, default_value=0.);\n"
"\t\t>> tsm_save(filename, type, times, formatted_tensors, default_value); -- MATLAB wrapper\n"
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

            outputs[1] = factory.createArray<double>({ 1, tsm.data.size() });
            for (size_t i = 0; i < tsm.data.size(); i++)
                outputs[1][0][i] = tsm.data[i];

            outputs[2] = factory.createArray<size_t>({ 1, tsm.dims.size() });
            for (size_t i = 0; i < tsm.dims.size(); i++)
                outputs[2][0][i] = tsm.dims[i];
            
            return;
        }

        // SAVE
        // tsm('save', filename, type, times, tensors, dims, default_value);
        if (firstArgument == "save") {
            if (inputs.size() < 6) {
                _error("At least six inputs required.");
                return;
            }

            // interpret the variables
            // type
            string type;
            if (inputs[2].getType() != matlab::data::ArrayType::CHAR) {
                _error("Type argument has wrong data type.");
            }
            else {
                type = matlab::data::CharArray(inputs[2]).toAscii();
            }

            // times
            matlab::data::TypedArray<double> m_times = std::move(inputs[3]);
            vector<double> time(m_times.getNumberOfElements());
            int i_e = 0;
            for (auto e : m_times)
                time[i_e++] = e;

            // tensors
            matlab::data::TypedArray<double> m_tensors = std::move(inputs[4]);
            vector<double> data(m_tensors.getNumberOfElements());
            i_e = 0;
            for (auto e : m_tensors)
                data[i_e++] = e;

            // dims
            matlab::data::TypedArray<double> m_dims = std::move(inputs[5]);
            vector<size_t> dims(m_dims.getNumberOfElements());
            i_e = 0;
            for (auto e : m_dims)
                dims[i_e++] = (size_t) e;

            // default_value
            double default_value = 0.;
            if (inputs.size() > 6)
                default_value = inputs[6][0];

            TSM::Tsm tsm(time, data, dims);
            tsm.save(filename, type, default_value);

            return;
        }


        // checkArguments(outputs, inputs);
        // double multiplier = inputs[0][0];
        // matlab::data::TypedArray<double> in = std::move(inputs[1]);
        // arrayProduct(in, multiplier);
        // outputs[0] = std::move(in);

        _error("Unknown first argument keyword. Use tsm(help) to see available options.");
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
