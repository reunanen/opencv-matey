#ifndef OPENCV_MATEY_H
#define OPENCV_MATEY_H

#include <opencv2/core/core.hpp>

class Matey
{
public:
    void create(int rows, int cols, int type)
    {
        if (type != allocated_memory.type()) {
            // It's better to create a new object, rather than
			// change the type of an existing one.
            assert(allocated_memory.empty());
			
            recreate(rows, cols, type);
            return;
        }

        if (rows > allocated_memory.rows || cols > allocated_memory.cols) {
            recreate(rows, cols, type);
        }
        else {
            valid_rect = cv::Rect(0, 0, cols, rows);
        }
    }

    void create(cv::Size size, int type)
    {
        create(size.height, size.width, type);
    }

    cv::Mat get() const
    {
        return allocated_memory(valid_rect);
    }

    void reset()
    {
        allocated_memory = cv::Mat();
        valid_rect = cv::Rect();
    }

    void set_growth_factors(double rows, double cols)
    {
        if (rows < 1.0 || cols < 1.0) {
            throw std::runtime_error("The growth factors cannot be less than 1.0");
        }

        growth_factor_rows = rows;
        growth_factor_cols = cols;
    }

private:
    void recreate(int rows, int cols, int type)
    {
        try {
            const auto get_new_dim = [](int current_dim, int desired_dim, double growth_factor) {
                const int new_dim = static_cast<int>(std::ceil(growth_factor * desired_dim));
                return new_dim < current_dim
                    ? current_dim
                    : new_dim;
            };

            const auto rows_to_allocate = get_new_dim(allocated_memory.rows, rows, growth_factor_rows);
            const auto cols_to_allocate = get_new_dim(allocated_memory.cols, cols, growth_factor_cols);

            assert(rows_to_allocate > allocated_memory.rows || cols_to_allocate > allocated_memory.cols);

            // NB: contents not preserved
            allocated_memory.create(rows_to_allocate, cols_to_allocate, type);
            valid_rect = cv::Rect(0, 0, cols, rows);
        }
        catch (const cv::Exception& e) {
            if (e.code == cv::Error::StsNoMem) {
                reset();
            }
            throw;
        }
    }

    cv::Mat allocated_memory;
    cv::Rect valid_rect;

    double growth_factor_rows = 1.2;
    double growth_factor_cols = 1.2;
};

#endif // OPENCV_MATEY_H