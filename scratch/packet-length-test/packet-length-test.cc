

#include <ns3/core-module.h>
#include <ns3/walker-constellation-helper.h>

#include <ns3/internet-module.h>
#include <ns3/csma-module.h>
#include <ns3/applications-module.h>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Pck_Length_Testing");




typedef struct {

    Ptr<WalkerConstellationHelper> constellation;

} simparams_t;



static void generate_packet(simparams_t *params)
{
    Ptr<MobilityModel> sat0 = params->constellation->getSatellite(10);

    std::cout << sat0->GetVelocity().GetLength() << "\n";
}



int main ( int argc, char* argv[] )
{

    NS_LOG_UNCOND("Setting up Packet Length Test");
    

    Time interval (Seconds(100));
    Time duration (Seconds(60.0 * 10.0));

    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);


    Ptr<WalkerConstellationHelper> constellation = CreateObjectWithAttributes<WalkerConstellationHelper>
    (
        "Inclination", DoubleValue(60.0),
        "NumOfSats", IntegerValue(10),
        "NumOfOrbits", IntegerValue(5),
        "RaanShift", DoubleValue(0.0),
        "Altitude", DoubleValue(920.0),
        "Phasing", DoubleValue(0.0)
    );

    constellation->Initialize();

    NodeContainer n;
    n.Create(2);

    InternetStackHelper internet;
    internet.Install(n);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(DataRate(5000000)));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    csma.SetDeviceAttribute("Mtu", UintegerValue(1400));
    NetDeviceContainer d = csma.Install(n);


    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(d);
    Address serverAddress = Address(i.GetAddress(1));


    NS_LOG_INFO("Create UdpServer application on node 1.");
    uint16_t port = 4000;
    UdpServerHelper server(port);
    ApplicationContainer apps = server.Install(n.Get(1));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    NS_LOG_INFO("Create UdpClient application on node 0 to send to node 1.");
    uint32_t MaxPacketSize = 1024;
    Time interPacketInterval = Seconds(0.05);
    uint32_t maxPacketCount = 320;
    UdpClientHelper client(serverAddress, port);
    client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
    client.SetAttribute("Interval", TimeValue(interPacketInterval));
    client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
    apps = client.Install(n.Get(0));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(10.0));


    /*  Set Simulation Params */
    simparams_t params = 
    {
        .constellation = constellation
    };


/*
    Time tim(Seconds(0.0));
    while (tim < duration)
    {

        Simulator::Schedule 
        (
            tim,
            &generate_packet,
            &params
        );

        tim += interval;
    }
*/


    NS_LOG_UNCOND("Starting Simulator...");
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}