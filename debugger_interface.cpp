#include <debugger_interface.hpp>

const std::string SlamInterface::id_string()
{
    return "slam algorithm";
}

SlamInterface::SlamInterface (arstudio::Config * c) : IAbstractAlgorithm (c)
{
    frames_counter = 0;
    frames_step = config->get("coreslam.frames_step").toInt();
}

bool SlamInterface::create()
{
    slam = new CoreSlam ();
    return true;
}

bool SlamInterface::run(const cv::Mat &image, const cv::Mat &dmap)
{
    LuxFrame * f = new LuxFrame;
    f->image = image;
    f->depth_map = dmap;

    if (frames_counter%frames_step == 0)
        slam->run(f);

    frames_counter++;

    return true;
}
