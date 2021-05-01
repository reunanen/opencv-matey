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

private:
    void recreate(int rows, int cols, int type)
    {
        try {
            // NB: contents not preserved
            allocated_memory.create(rows, cols, type);
            valid_rect = cv::Rect(0, 0, cols, rows);
        }
        catch (const cv::Exception& e) {
            if (e.code == cv::Error::StsNoMem) {
                allocated_memory = cv::Mat();
                valid_rect = cv::Rect();
            }
            throw;
        }
    }

    cv::Mat allocated_memory;
    cv::Rect valid_rect;
};

#endif // OPENCV_MATEY_H