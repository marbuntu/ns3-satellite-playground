

#include <ns3/walker-orbit-helper.h>
#include <ns3/sat-fl-application-helper.h>
#include <ns3/sat-fl-application.h>
#include <ns3/demo-setup-helper.h>
#include <ns3/sat-node-helper.h>
#include <ns3/internet-module.h>
#include <ns3/network-module.h>
#include <ns3/sat-isl-antenna.h>
#include <ns3/sat-isl-channel.h>
#include <ns3/sat-isl-ipv4-routing.h>
#include <ns3/sat-ipv4-routing-helper.h>
#include <ns3/udp-client-server-helper.h>


using namespace ns3;


// static void DemoISLInterfaceSetup(SatelliteNodeHelper *sat_setup)
// {
//     std::cout << "Load Interface Setup ... ";

//     Ptr<SatelliteISLInterfaceHelper> itf_helper = sat_setup->GetInterfaceFactory();

//     itf_helper->AddTerminal(
//         "Phi", DoubleValue(0.0),
//         "Theta", DoubleValue(0.0),
//         "Psi", DoubleValue(0.0)
//         )->SetAntennaModel<SatelliteISLAntenna>(
//             "RadiationPattern", EnumValue(SatelliteISLAntenna::RP_Cosine),
//             "MaxGainDbi", DoubleValue(32.13),
//             "OpeningAngle", DoubleValue(160.0)
//         );

//     std::cout << " DONE!\n";
// }


// static NetDeviceContainer ExtractNetDevices(NodeContainer nodes)
// {
//     NetDeviceContainer netdevs;
//     for (auto it = nodes.Begin(); it != nodes.End(); it++)
//     {
//         Ptr<Node> node = *it;
//         netdevs.Add(node->GetDevice(0));

//         std::cout << node->GetNDevices() << "\n"; 
//     }

//     return netdevs;
// }


int main (int argc, char* argv[] )
{
    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
    LogComponentEnable("SatISLRoutingIPv4", LOG_LEVEL_ALL);


    SatDemoSetup::Initialize();
    NodeContainer satellites = SatDemoSetup::GetDefaultNodes();
    Ptr<SatelliteISLChannel> channel = SatDemoSetup::GetDefaultChannel();

    SatelliteISLInterfaceHelper itf_helper = DefaultISLInterfaceSetup::GetDefaultFactory(DefaultISLInterfaceSetup::SYMMETRIC_4x);

    // SatelliteIPv4RoutingHelper sat_router;
    
    InternetStackHelper internet;
    // internet.SetRoutingHelper(sat_router);
    internet.Install(satellites);

    NetDeviceContainer netdevs = itf_helper.Install(satellites, channel);

    //NetDeviceContainer netdevs = ExtractNetDevices(satellites);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer iAiB = ipv4.Assign(netdevs);


    // Ptr<SatelliteISLRoutingIPv4> routingA = satellites.Get(0)->GetObject<SatelliteISLRoutingIPv4>();
    // routingA->AddRoutingEntry(Ipv4Address("10.1.1.2"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.2"), 1);
    // routingA->AddRoutingEntry(Ipv4Address("10.1.1.3"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.3"), 1);

    // Ptr<SatelliteISLRoutingIPv4> routingB = satellites.Get(1)->GetObject<SatelliteISLRoutingIPv4>();
    // routingB->AddRoutingEntry(Ipv4Address("10.1.1.1"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.1"), 1);
    // //routingB->AddRoutingEntry(Ipv4Address("10.1.1.3"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.3"), 1);
    
    Ptr<Ipv4> ipv4A = satellites.Get(0)->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4B = satellites.Get(1)->GetObject<Ipv4>();

    for ( auto i = iAiB.Begin(); i != iAiB.End(); i++)
    {
        Ptr<Ipv4> ip4 = i->first;
        uint32_t itf = i->second;
        NS_LOG_UNCOND(itf << ip4->GetAddress(itf, 0));

        // if (use_sat_router)
        // {
        //     ip4->SetRoutingProtocol(satrouter);
        // }
    }

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> staticRoutingA = ipv4RoutingHelper.GetStaticRouting(ipv4A);
    // The ifIndex for this outbound route is 1; the first p2p link added
    staticRoutingA->AddHostRouteTo(Ipv4Address("10.1.1.2"), Ipv4Address("10.1.1.2"), 1);
    staticRoutingA->AddHostRouteTo(Ipv4Address("10.1.1.3"), Ipv4Address("10.1.1.3"), 1);
    Ptr<Ipv4StaticRouting> staticRoutingB = ipv4RoutingHelper.GetStaticRouting(ipv4B);
    // The ifIndex we want on node B is 2; 0 corresponds to loopback, and 1 to the first point to
    // point link
    staticRoutingB->AddHostRouteTo(Ipv4Address("192.168.1.1"), Ipv4Address("10.1.1.6"), 1);
    staticRoutingB->AddHostRouteTo(Ipv4Address("10.1.1.1"), Ipv4Address("10.1.1.1"), 1);

    // Ptr<Ipv4> ipv4A = c.Get(0)->GetObject<Ipv4>();
    // Ptr<Ipv4> ipv4B = c.Get(1)->GetObject<Ipv4>();

    // Ptr<SatelliteISLRoutingIPv4> RoutingA = Create<SatelliteISLRoutingIPv4>(); //StaticCast<SatelliteISLRoutingIPv4>(ipv4A->GetRoutingProtocol());
    // ipv4A->SetRoutingProtocol(RoutingA);

    // Ptr<SatelliteISLRoutingIPv4> RoutingB = Create<SatelliteISLRoutingIPv4>();
    // RoutingB->AddRoutingEntry(Ipv4Address("192.168.1.1"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.6"), 2);
    // RoutingB->AddRoutingEntry(Ipv4Address("10.1.1.6"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.6"), 2);
    // ipv4B->SetRoutingProtocol(RoutingB);


    Ipv4Address serverAddress = Ipv4Address("10.1.1.1"); //("192.168.1.1");
    uint16_t serverPort = 9; // Discard port (RFC 863)
    uint32_t MaxPacketSize = 1024;
    Time interPacketInterval = Seconds (60);
    uint32_t maxPacketCount = 10;

    UdpServerHelper server(serverPort);
    ApplicationContainer apps = server.Install(satellites.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(interPacketInterval * (maxPacketCount + 1));


    UdpClientHelper client(serverAddress, serverPort);
    client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    client.SetAttribute ("Interval", TimeValue (interPacketInterval));
    client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

    apps = client.Install(satellites.Get(1));
    apps.Start(Seconds(1.0));
    apps.Stop(interPacketInterval * (maxPacketCount + 1));


    NS_LOG_UNCOND("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_UNCOND("Done.");

    return 0;
}




// int main (int argc, char* argv[] )
// {   
//     LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
//     LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
//     LogComponentEnable("SatISLRoutingIPv4", LOG_LEVEL_ALL);
//     LogComponentEnable("SatelliteISLNetDevice", LOG_LEVEL_ALL);

// // Create Constellation
//     Ptr<WalkerOrbitHelper> orb = CreateObjectWithAttributes<WalkerOrbitHelper>(
//         "NumOfSats", IntegerValue(10),
//         "OrbitID", IntegerValue(1),
//         "ConstellationID", IntegerValue(1)
//     );
//     orb->Initialize();

//     NodeContainer satellites = orb->getSatellites();


// // Creat Channel
//     Ptr<SatelliteISLChannel> channel = CreateObject<SatelliteISLChannel>();
//     Ptr<FriisPropagationLossModel> loss_model = CreateObject<FriisPropagationLossModel>();
//     Ptr<ConstantSpeedPropagationDelayModel> delay_model = CreateObject<ConstantSpeedPropagationDelayModel>();
//     channel->SetPropagationLossModel(loss_model);
//     channel->SetPropagationDelayModel(delay_model);


// // Install Terminals and Net Devices
//     SatelliteNodeHelper sat_setup;
//     sat_setup.SetInitIpStack(true);
//     sat_setup.SetISLChannel(channel);
//     DemoISLInterfaceSetup(&sat_setup);

//     std::cout << "Install ISL Terminals ... ";
//     sat_setup.Install(satellites);
//     std::cout << " DONE!\n";

//     NetDeviceContainer netdevs = ExtractNetDevices(satellites);


//     Ipv4AddressHelper ipv4;
//     ipv4.SetBase("10.1.1.0", "255.255.255.0");
//     ipv4.Assign(netdevs);


// // Setup Routing Helper
//     Ptr<SatelliteISLRoutingIPv4> RoutingA = Create<SatelliteISLRoutingIPv4>(); //StaticCast<SatelliteISLRoutingIPv4>(ipv4A->GetRoutingProtocol());
//     RoutingA->AddRoutingEntry(Ipv4Address("192.168.1.1"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.2"), 1);
//     RoutingA->AddRoutingEntry(Ipv4Address("10.1.1.6"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.2"), 1);
//     satellites.Get(0)->GetObject<Ipv4>()->SetRoutingProtocol(RoutingA);

//     Ptr<SatelliteISLRoutingIPv4> RoutingB = Create<SatelliteISLRoutingIPv4>();
//     RoutingB->AddRoutingEntry(Ipv4Address("192.168.1.1"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.6"), 2);
//     RoutingB->AddRoutingEntry(Ipv4Address("10.1.1.6"), Ipv4Mask::GetOnes(), Ipv4Address("10.1.1.6"), 2);
//     satellites.Get(1)->GetObject<Ipv4>()->SetRoutingProtocol(RoutingB);
//     // RoutingA->AddRoutingEntry(Ipv4Address())


// // Setup FL Application on Nodes
//     std::cout << "Install FL Applications ... ";
//     SatFLApplicationHelper helper;
//     helper.SetAttribute("Port", UintegerValue(123));
//     std::cout << " DONE!\n";


//     // SatelliteFLApplication app;
//     // app.SetAttribute("Port", UintegerValue(123));

//     Ipv4Address serverAddress = Ipv4Address("10.1.1.6"); //("192.168.1.1");
//     uint16_t serverPort = 9; // Discard port (RFC 863)
//     uint32_t MaxPacketSize = 1024;
//     Time interPacketInterval = Seconds (60);
//     uint32_t maxPacketCount = 10;

//     UdpServerHelper server(serverPort);
//     ApplicationContainer apps = server.Install(satellites.Get(2));
//     apps.Start(Seconds(1.0));
//     apps.Stop(interPacketInterval * (maxPacketCount + 1));


//     UdpClientHelper client(serverAddress, serverPort);
//     client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
//     client.SetAttribute ("Interval", TimeValue (interPacketInterval));
//     client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

//     apps = client.Install(satellites.Get(0));
//     apps.Start(Seconds(1.0));
//     apps.Stop(interPacketInterval * (maxPacketCount + 1));



//     NS_LOG_UNCOND("Run Simulation.");
//     Simulator::Run();
//     Simulator::Destroy();
//     NS_LOG_UNCOND("Done.");


//     return 0;
// }
