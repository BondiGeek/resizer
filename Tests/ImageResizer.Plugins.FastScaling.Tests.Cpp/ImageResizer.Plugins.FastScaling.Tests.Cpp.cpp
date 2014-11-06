// This is the main DLL file.

#include "stdafx.h"
#include "ImageResizer.Plugins.FastScaling.Tests.Cpp.h"

#include "..\..\Plugins\ImageResizer.Plugins.FastScaling\weighting.h"


bool test_contrib_windows(char *msg)
{
    int bad = -1;
    LineContribType *lct = 0;

    // assumes included edge cases

    unsigned int from_w = 6;
    unsigned int to_w = 3;
    unsigned int corr36[3][2] = { { 0, 1 }, { 2, 3 }, { 4, 5 } };
    lct = ContributionsCalc(to_w, from_w, DetailsDefault());

    for (int i = 0; i < lct->LineLength; i++)
    if (lct->ContribRow[i].Left != corr36[i][0]) { bad = i; break; }
    else if (lct->ContribRow[i].Right != corr36[i][1]) { bad = i; break; }
    
    if (bad != -1)
    {
        sprintf(msg, "at 6->3 invalid value (%d; %d) at %d, expected (%d; %d)",
            lct->ContribRow[bad].Left,
            lct->ContribRow[bad].Right,
            bad, corr36[bad][0], corr36[bad][1]);
        ContributionsFree(lct);
        return false;
    }
    ContributionsFree(lct);

    from_w = 6;
    to_w = 4;
    unsigned int corr46[4][2] = { { 0, 1 }, { 1, 2 }, { 3, 4 }, { 4, 5 } };
    lct = ContributionsCalc(to_w, from_w, DetailsDefault());

    for (int i = 0; i < lct->LineLength; i++)
    if (lct->ContribRow[i].Left != corr46[i][0]) { bad = i; break; }
    else if (lct->ContribRow[i].Right != corr46[i][1]) { bad = i; break; }

    if (bad != -1)
    {
        sprintf(msg, "at 6->4 invalid value (%d; %d) at %d, expected (%d; %d)",
            lct->ContribRow[bad].Left,
            lct->ContribRow[bad].Right,
            bad, corr46[bad][0], corr46[bad][1]);
        ContributionsFree(lct);
        return false;
    }
    ContributionsFree(lct);

    return true;
}

bool test_weigth_steps(InterpolationDetailsPtr details, char *msg, double c_val, double c_loc, double step, double limit)
{
    c_loc += step;

    if (abs(c_loc) > abs(limit))
        return true;

    double t_val = (*details->filter)(details, c_loc);
    if (t_val > c_val)
    {
        sprintf(msg+strlen(msg), "value %.2f exceeds previous %.2f at %.2f", t_val, c_val, c_loc);
        return false;
    }
    
    return test_weigth_steps(details, msg, t_val, c_loc, step, limit);
}

bool test_details(InterpolationDetailsPtr details, char *msg)
{
    double top = (*details->filter)(details, 0);

    if (!test_weigth_steps(details, msg, top, 0, +0.05, +1)) return false;
    if (!test_weigth_steps(details, msg, top, 0, -0.05, -1)) return false;

    double local_tony = 0.57;
    double edge = (*details->filter)(details, 1);
    if (edge - local_tony > 0 || edge + local_tony < 0)
    {
        sprintf(msg + strlen(msg), "near edge value exceeds TONY (%.2f at +1)", edge);
        return false;
    }

    edge = (*details->filter)(details, -1);
    if (edge - local_tony > 0 || edge + local_tony < 0)
    {
        sprintf(msg + strlen(msg), "near edge value exceeds TONY (%.2f at -1)", edge);
        return false;
    }

    return true;
}

bool test_weight_distrib(char *msg)
{
    InterpolationDetailsPtr details;

    sprintf(msg, "DetailsDefault() ");
    details = DetailsDefault();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsGeneralCubic() ");
    details = DetailsGeneralCubic();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsCatmullRom() ");
    details = DetailsCatmullRom();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsMitchell() ");
    details = DetailsMitchell();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsRobidoux() ");
    details = DetailsRobidoux();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsRobidouxSharp() ");
    details = DetailsRobidouxSharp();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsHermite() ");
    details = DetailsHermite();
    if (!test_details(details, msg)) return false;
    free(details);

    sprintf(msg, "DetailsLanczos() ");
    details = DetailsLanczos();
    if (!test_details(details, msg)) return false;
    free(details);

    return true;
}


#pragma managed

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Specialized;
using namespace Xunit;
using namespace ImageResizer;
using namespace ImageResizer::Configuration;
using namespace ImageResizer::Plugins::FastScaling;

namespace ImageResizerPluginsFastScalingTestsCpp {

    public ref class TestsCpp
    {
    public:
        /*[Fact]
        void DummyTest()
        {
            String ^imgdir = gcnew String("..\\..\\..\\..\\Samples\\Images\\");
            Config ^c = gcnew Config();
            FastScalingPlugin ^fs = gcnew FastScalingPlugin();

            fs->Install(c);

            NameValueCollection ^s = gcnew NameValueCollection();
            s->Add("width", "400");
            s->Add("fastscale", "true");

            //ImageJob ^ij = gcnew ImageJob();
            //ImageJob ^ij = gcnew ImageJob(imgdir + "red-leaf.jpg", "out.jpg", s);
            //c->CurrentImageBuilder->Build(ij);

            c->BuildImage(imgdir + "red-leaf.jpg", "out.jpg", "width=400&fastscale=true&turbo=true");
        }*/

        [Fact]
        void ContributionsCalcTest()
        {
            char msg[256];
            bool r = test_contrib_windows(msg);
            Assert::True(r, gcnew String(msg));
        }

        [Fact]
        void WeightFuncTest()
        {
            char msg[256];
            bool r = test_weight_distrib(msg);
            Assert::True(r, gcnew String(msg));
        }
    };
}