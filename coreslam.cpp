#include "coreslam.h"
#include "QDebug"
#include "math.h"
#include <Logger.hpp>

namespace LuxSlam
{
/**
\mainpage The mainpage documentation
<b> Simultaneous localization and mapping (SLAM) is a technique used by robots and autonomous vehicles to build up a map within an unknown environment. The solution is based on the depth map and RGB-image, obtained with the Kinect sensor and includes the following stages:
calculation of the displacement relative to the previous position of the robot as a vector r = (x, y, z). Here, each component of the vector corresponds to the displacement of each of the axes in the 3-dimensional space.
finding a rotation matrix.
In the first stage find feature points on each frame. Then find corresponding points between adjacent frames.

Identified the same points on different RGB-image frames, and knowing the distances to them (from the kinect frame), we can calculate the  displacement of the robot relative to the previous position.

The RTFindTrilateration class make it using the trilateration.
It find the translation vector, and having three points from the current and previous frame, it find a rotation matrix by solving three systems of linear equations of the form
</b>
\image html "system.png"
*/
    CoreSlam::CoreSlam()
    {
        fdetector = new FeatureDetectorSurf();
        fmatcher = new FeatureMatcherFlann();
        rtfinder = new RTFinderTrilateration();
        ffilter = new MatchesFilterCVTeam();
        prev_frame = 0;
        prev_features = 0;
        bundle_adjusment = new OpenCVBoundleAdjustemnt(5);
    }

    void CoreSlam::Get3dPointsOfMatches(const std::vector<cv::DMatch>& matches,  std::vector<MatchPoints>& points)
    {
        for (unsigned int i = 0 ; i< matches.size(); i++)
        {
            cv::KeyPoint p1 = prev_features->points.at(matches.at(i).queryIdx);
            cv::KeyPoint p2 = curr_features->points.at(matches.at(i).trainIdx);
            MatchPoints mp;
            mp.first2d = p1.pt;
            mp.second2d = p2.pt;

            mp.first3d = prev_frame->getPoint3D(mp.first2d.x,mp.first2d.y);
            mp.second3d = curr_frame->getPoint3D(mp.second2d.x,mp.second2d.y);

            if (mp.first3d.z && mp.second3d.z) points.push_back(mp);
        }
    }

    void CoreSlam::run(LuxFrame * input_frame)
    {
        // algorithm
        curr_frame = input_frame;
        Triple result;

        // get keypoints and descriptors
        curr_features = fdetector->getFeatures(curr_frame);

        if (curr_features->points.size() == 0)
        {
            delete curr_features;
            return;
        }

        // drawing results
        cv::Mat results;
        curr_frame->image.copyTo(results);
        for (int i=0; i<curr_features->points.size();i++)
            cv::circle(results,curr_features->points.at(i).pt,5,cvScalar(100),3);

        if (prev_frame)  // if this is the first frame
        {
            // matching
            std::vector< cv::DMatch > matches = fmatcher->getMatches(prev_features,curr_features);

            // getting 3d coordinates of matches
            std::vector <MatchPoints> points;

            Get3dPointsOfMatches(matches, points);

            // filtering the matches
            points = ffilter->filterMatches(points);

            // drawing results
            for (int i=0;i<points.size();i++)
                cv::line(results,points.at(i).first2d,points.at(i).second2d,cvScalar(0,190),3);

            // getting rotation, translation vector
            result = rtfinder->getRTVector(points);

            cv::Mat eulerAngles = StaticFunctions::getEulerAngles(result.rotation_matrix);

            result.rotation_matrix = StaticFunctions::getRotationMatrix(eulerAngles);

            cv::Point3d not_rotated_vector = result.translation_vector;

            result.translation_vector.x=
                    global_transformation_vector.rotation_matrix.at<float>(0,0)*not_rotated_vector.x+
                    global_transformation_vector.rotation_matrix.at<float>(0,1)*not_rotated_vector.y+
                    global_transformation_vector.rotation_matrix.at<float>(0,2)*not_rotated_vector.z;

            result.translation_vector.y=
                    global_transformation_vector.rotation_matrix.at<float>(1,0)*not_rotated_vector.x+
                    global_transformation_vector.rotation_matrix.at<float>(1,1)*not_rotated_vector.y+
                    global_transformation_vector.rotation_matrix.at<float>(1,2)*not_rotated_vector.z;

            result.translation_vector.z=
                    global_transformation_vector.rotation_matrix.at<float>(2,0)*not_rotated_vector.x+
                    global_transformation_vector.rotation_matrix.at<float>(2,1)*not_rotated_vector.y+
                    global_transformation_vector.rotation_matrix.at<float>(2,2)*not_rotated_vector.z;

            // calculate the global rotation matrix and the translation vector
            global_transformation_vector.rotation_matrix =
                    global_transformation_vector.rotation_matrix * result.rotation_matrix;

            global_transformation_vector.translation_vector =
                    global_transformation_vector.translation_vector + result.translation_vector;

//            bundle_adjusment->pushFrame(points,global_transformation_vector);

        }

        // Logging the camera

        cv::Mat eulerAngles = StaticFunctions::getEulerAngles(global_transformation_vector.rotation_matrix);

        Logger & l = Logger::getInstance ();

        l.logCamera(global_transformation_vector.translation_vector,eulerAngles.at<float>(0)/M_PI*180,eulerAngles.at<float>(1)/M_PI*180,eulerAngles.at<float>(2)/M_PI*180);
        l.addImage(results,"fatures");

        delete prev_frame;
        delete prev_features;

        prev_frame = new LuxFrame(*curr_frame);
        prev_features = curr_features;
    }
}
