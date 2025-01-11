


#include <ns3/core-module.h>
#include <ns3/applications-module.h>
#include <ns3/onoff-application.h>

#include "ns3/sat-isl-channel.h"

#include <ns3/simple-net-device.h>
#include <ns3/sat-isl-net-device.h>
#include <ns3/sat-leo-propagation-loss.h>
#include <ns3/satellite-sgp4-mobility-model.h>

#include <ns3/walker-constellation-helper.h>
#include <ns3/cosine-antenna-model.h>


using namespace ns3;


NS_LOG_COMPONENT_DEFINE("Sat_Traffic_Generator");


/**
 * What defines a Satellite:
 * 
 *  + Mobility Model
 *  + NetDevice
 *  + Terminals[]
 *  + Node
 *  + Channel Model
 */



static Ptr<NetDevice> CreateNetDevice(Ptr<Node> node, Ptr<SatelliteISLChannel> channel, Ptr<MobilityModel> mob)
{
    //NS_LOG_UNCOND(channel);
    Ptr<SatelliteISLNetDevice> device = CreateObject<SatelliteISLNetDevice>();
    NS_LOG_UNCOND("Created Net Device");
    Ptr<SatelliteISLTerminal> terminal = CreateObject<SatelliteISLTerminal>();
    Ptr<CosineAntennaModel> ant = CreateObjectWithAttributes<CosineAntennaModel>(
        "MaxGain", DoubleValue(100)
    );

    terminal->SetAntennaModel(ant);
    device->RegisterISLTerminal(terminal);
    device->SetAddress(Mac48Address::Allocate());
    device->SetChannel(channel);
    node->AddDevice(device);

    device->SetNode(node);
    //node->AggregateObject(mob);
    return device;
}




/*           
*             1   
*             |    
*             |
*     2 ----- 0 ----- 4  
*             |
*             |
*             3
*/



int main( int argc, char* argv[] )
{

    Ptr<WalkerConstellationHelper> helper = CreateObjectWithAttributes<WalkerConstellationHelper>(
        "Inclination", DoubleValue(60.0),
        "NumOfSats", IntegerValue(10),
        "NumOfOrbits", IntegerValue(10),
        "Altitude", DoubleValue(480)
    );

    helper->Initialize();

    Ptr<MobilityModel> mob_container[] = {
        helper->getSatellite(2 * 10 + 5),       // Sat 0
        helper->getSatellite(2 * 10 + 4),       // Sat 1
        helper->getSatellite(1 * 10 + 5),       // Sat 2
        helper->getSatellite(2 * 10 + 6),       // Sat 3
        helper->getSatellite(3 * 10 + 5),       // Sat 4
    };

    NodeContainer nodes;
    //nodes.Create(4);

    LogComponentEnable("SatelliteISLNetDevice", LOG_LEVEL_ALL);
    LogComponentEnable("SatelliteISLChannel", LOG_LEVEL_ALL);
    LogComponentEnable("SatelliteISLTerminal", LOG_LEVEL_ALL);
    LogComponentEnable("SatellitePropagationLossLEO", LOG_LEVEL_ALL);

    

    //Ptr<SatellitePropagationLossLEO> loss_model = CreateObjectWithAttributes<SatellitePropagationLossLEO>();
    Ptr<SatelliteISLChannel> channel = CreateObject<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss_model = CreateObject<FriisPropagationLossModel>();

    channel->SetPropagationLossModel(loss_model);



    for (uint32_t i = 0; i < 5; i++)
    {
        NS_LOG_UNCOND("\nSetup Device " << i);
        Ptr<Node> node = mob_container[i]->GetObject<Node>();

        NS_LOG_UNCOND("Node: " << node);

        CreateNetDevice(node, channel, mob_container[i]);

        nodes.Add(node);
        NS_LOG_UNCOND("\n");
    }

    Ptr<Packet> pck = Create<Packet>(500);

    NS_LOG_UNCOND("\n\n\nStart Transmission");
    nodes.Get(2)->GetDevice(0)->Send(pck, nodes.Get(1)->GetDevice(0)->GetAddress(), 0);

    

    //MegaConstellationStaticChannel chn; // CreateObject<MegaConstellationStaticChannel>();

    //onOffHelper.SetAttribute("DataRate", StringValue("10Mbps"));

    //PointToPointChannel chn;

    NS_LOG_UNCOND("Starting Simulator...");
    Simulator::Run();

    NS_LOG_UNCOND("Clear Simulator..");
    Simulator::Destroy();


    return 0;
}