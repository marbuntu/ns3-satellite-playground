


#include <ns3/core-module.h>
#include <ns3/applications-module.h>
#include <ns3/onoff-application.h>

#include "ns3/mega-const-static-channel.h"


using namespace ns3;


NS_LOG_COMPONENT_DEFINE("Sat_Traffic_Generator");



int main( int argc, char* argv[] )
{

    MegaConstellationStaticChannel chn; // CreateObject<MegaConstellationStaticChannel>();
    OnOffApplication onOffHelper;

    //onOffHelper.SetAttribute("DataRate", StringValue("10Mbps"));

    //PointToPointChannel chn;


    Simulator::Run();

    Simulator::Destroy();


    return 0;
}