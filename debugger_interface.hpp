#include <coreslam.h>
#include <opencv2/highgui/highgui.hpp>

#include <Logger.hpp>
#include <Config.hpp>
#include <AlgoInterface.hpp>
#include <AlgorithmFactory.hpp>

using namespace LuxSlam;

class SlamInterface : public IAbstractAlgorithm
{
    int frames_counter;
    int frames_step;
public:
    const std::string id_string (void);
    SlamInterface (arstudio::Config *);
    bool create ();
    bool run (const cv::Mat &, const cv::Mat &, int);

    CoreSlam * slam;
};
