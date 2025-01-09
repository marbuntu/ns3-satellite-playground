

#include <ns3/core-module.h>
#include <ns3/node-container.h>
#include <ns3/walker-constellation-helper.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/sat-isl-interface-helper.h>
#include <ns3/sat-isl-channel.h>

#include <ns3/internet-module.h>
#include <ns3/applications-module.h>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("satellite-tcp-traffic");


void PhyRxPacket(std::string context, Ptr<Packet> pck)
{
    NS_LOG_UNCOND(context << pck);
}



int main(int argc, char* argv[])
{

    Time interval (Seconds(100));
    Time duration (Seconds(60.0 * 10.0));

    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);
    // LogComponentEnable("Socket", LOG_LEVEL_ALL);

    LogComponentEnable("SatelliteISLNetDevice", LOG_LEVEL_ALL);
    //LogComponentEnable("SatelliteISLChannel", LOG_LEVEL_ALL);


    Config::SetDefault("ns3::Ipv4L3Protocol::EnableDuplicatePacketDetection", BooleanValue(true));
    Config::SetDefault("ns3::Ipv4L3Protocol::DuplicateExpire", TimeValue(Seconds(10)));



    Ptr<WalkerConstellationHelper> helper = CreateObjectWithAttributes<WalkerConstellationHelper>(
        "Inclination", DoubleValue(66.0),
        "SatsPerOrbit", IntegerValue(10),
        "NumOfOrbits", IntegerValue(10),
        "Altitude", DoubleValue(480)
    );
    helper->Initialize();

    /* Select Nodes */
    NodeContainer nodes;
    nodes.Add(helper->getSatellite(2 * 10 + 5)->GetObject<Node>());       // Sat 0 - Center
    nodes.Add(helper->getSatellite(2 * 10 + 6)->GetObject<Node>());       // Sat 1
    nodes.Add(helper->getSatellite(1 * 10 + 5)->GetObject<Node>());       // Sat 2
    nodes.Add(helper->getSatellite(2 * 10 + 4)->GetObject<Node>());       // Sat 3
    nodes.Add(helper->getSatellite(3 * 10 + 5)->GetObject<Node>());       // Sat 4


    Ptr<Node> src = nodes.Get(1);
    Ptr<Node> dst = nodes.Get(3);


    /* Create Channel Model */
    Ptr<SatelliteISLChannel> channel = CreateObjectWithAttributes<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss_model = CreateObject<FriisPropagationLossModel>();
    channel->SetPropagationLossModel(loss_model);


    SatelliteISLInterfaceHelper itf_helper = DefaultISLInterfaceSetup::GetDefaultFactory(DefaultISLInterfaceSetup::SYMMETRIC_4x);
    NetDeviceContainer devs = itf_helper.Install(nodes, channel);


    

    Ipv4StaticRoutingHelper routingHelper;
    InternetStackHelper internet;
    internet.SetIpv6StackInstall(false);
    internet.SetRoutingHelper(routingHelper);
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign(devs);
    Address serverAddress = Address(i.GetAddress(2));


    Ptr<Ipv4StaticRouting> staticRouting;
    //staticRouting = routingHelper.GetRouting<Ipv4StaticRouting>(src->GetObject<Ipv4>()->GetRoutingProtocol());
    //staticRouting->SetDefaultRoute("10.1.1.1", 1);

    staticRouting = routingHelper.GetStaticRouting(src->GetObject<Ipv4>());
    staticRouting->AddHostRouteTo(Ipv4Address("192.168.1.1"), Ipv4Address("10.1.1.1"), 1);

    staticRouting = routingHelper.GetStaticRouting(dst->GetObject<Ipv4>());
    staticRouting->AddHostRouteTo(Ipv4Address("192.168.1.1"), Ipv4Address("10.1.1.4"), 1);

    // Ipv4ListRoutingHelper listRouting;
    // listRouting.Add(staticRouting, 0);
    

    NS_LOG_INFO("Create UdpServer application on node 1.");
    uint16_t port = 4000;
    UdpServerHelper server(port);
    ApplicationContainer apps = server.Install(nodes.Get(3));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(100.0));


    NS_LOG_INFO("Create UdpClient application on node 0 to send to node 1.");
    uint32_t MaxPacketSize = 1024;
    Time interPacketInterval = Seconds(10); //(0.05);
    uint32_t maxPacketCount = 1;
    UdpClientHelper client(serverAddress, port);
    client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
    client.SetAttribute("Interval", TimeValue(interPacketInterval));
    client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
    apps = client.Install(nodes.Get(1));
    apps.Start(Seconds(2.0));
    apps.Stop(Seconds(100.0));


    nodes.Get(0)->TraceConnect("PhyRxPck", "RxPhy", MakeCallback(&PhyRxPacket));


    // Print Routing Table

    


    NS_LOG_UNCOND("Starting Simulator...");
    Simulator::Run();
    Simulator::Destroy();


    return 0;
}