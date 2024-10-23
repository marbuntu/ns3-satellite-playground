


#include <ns3/applications-module.h>
#include <ns3/core-module.h>
#include <ns3/demo-setup-helper.h>

#include <ns3/sat-isl-interface-helper.h>
#include <ns3/sat-isl-channel.h>

#include <ns3/internet-module.h>
#include <ns3/network-module.h>

#include <ns3/sat-isl-pck-tag.h>

using namespace ns3;


typedef struct 
{
    Ptr<Node> udp_server;
    Ptr<Node> udp_client;
    Ptr<Node> router;

} sim_params_t;




void serverRxClbk(sim_params_t *params, const Ptr<const Packet> packet, const Address& srcAddress, const Address& destAddress)
{
    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "\t\t" << packet << "\t" << srcAddress << "\t" << destAddress);

    Ptr<MobilityModel> mob_srv = params->udp_server->GetObject<MobilityModel>();
    Ptr<MobilityModel> mob_clt = params->udp_client->GetObject<MobilityModel>();
    Ptr<MobilityModel> mob_rtr = params->router->GetObject<MobilityModel>();

    NS_LOG_UNCOND(mob_clt->GetDistanceFrom(mob_rtr) << "\t" << mob_rtr->GetDistanceFrom(mob_srv) << "\n"); 

}



int main(int argc, char* argv[])
{

    LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable("UdpServer", LOG_LEVEL_INFO);

    // LogComponentEnable("SatelliteISLNetDevice", LOG_LEVEL_ALL);
    // LogComponentEnable("SatelliteISLChannel", LOG_LEVEL_ALL);
    // LogComponentEnable("SatelliteISLTerminal", LOG_LEVEL_ALL);

    NS_LOG_UNCOND("Testing");

    // Initialize Demo Constellation
    SatDemoSetup::Initialize();
    NodeContainer nodes = SatDemoSetup::GetDefaultNodes();

    // InternetStackHelper internet;
    // internet.Install(nodes);


    SatelliteISLInterfaceHelper itf_helper = DefaultISLInterfaceSetup::GetDefaultFactory(DefaultISLInterfaceSetup::SYMMETRIC_4x);
    Ptr<SatelliteISLChannel> channelAB = CreateObjectWithAttributes<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss_modelAB = CreateObject<FriisPropagationLossModel>();
    Ptr<ConstantSpeedPropagationDelayModel> delayAB = CreateObject<ConstantSpeedPropagationDelayModel>();
    channelAB->SetPropagationLossModel(loss_modelAB);
    channelAB->SetPropagationDelayModel(delayAB);

    Ptr<SatelliteISLChannel> channelBC = CreateObjectWithAttributes<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss_modelBC = CreateObject<FriisPropagationLossModel>();
    Ptr<ConstantSpeedPropagationDelayModel> delayBC = CreateObject<ConstantSpeedPropagationDelayModel>();
    channelBC->SetPropagationLossModel(loss_modelBC);
    channelBC->SetPropagationDelayModel(delayBC);


    // NetDeviceContainer devs_container;
    // for (size_t n = 0; n < 4; n++)
    // {
    //     /* Create Channel Model */
    //     Ptr<SatelliteISLChannel> channel = CreateObjectWithAttributes<SatelliteISLChannel>();
    //     Ptr<FriisPropagationLossModel> loss_model = CreateObject<FriisPropagationLossModel>();
    //     channel->SetPropagationLossModel(loss_model);

    //     devs_container.Add(itf_helper.Install(nodes.Get(0), channel));
    //     devs_container.Add(itf_helper.Install(nodes.Get(n+1), channel));
    // }

    NodeContainer c = NodeContainer(nodes.Get(1), nodes.Get(0), nodes.Get(2));

    
    //NodeContainer c = NodeContainer(3);

    InternetStackHelper internet;
    internet.Install(c);

    // Setup p2p Links
    NodeContainer nAnB = NodeContainer(c.Get(0), c.Get(1));
    NodeContainer nBnC = NodeContainer(c.Get(1), c.Get(2)); 

    // PointToPointHelper p2p;
    // p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    // p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    
    NetDeviceContainer dAB = itf_helper.Install(nAnB, channelAB); //NetDeviceContainer(c.Get(0)->GetDevice(0), c.Get(1)->GetDevice(0)); //p2p.Install(nAnB);
    NetDeviceContainer dBC = itf_helper.Install(nBnC, channelBC); //NetDeviceContainer(c.Get(1)->GetDevice(0), c.Get(2)->GetDevice(0)); //p2p.Install(nBnC);


    // Ptr<CsmaNetDevice> deviceA = CreateObject<CsmaNetDevice>();
    // deviceA->SetAddress(Mac48Address::Allocate());
    // c.Get(0)->AddDevice(deviceA);
    // deviceA->SetQueue(CreateObject<DropTailQueue<Packet>>());

    // Ptr<CsmaNetDevice> deviceC = CreateObject<CsmaNetDevice>();
    // deviceC->SetAddress(Mac48Address::Allocate());
    // c.Get(2)->AddDevice(deviceC);
    // deviceC->SetQueue(CreateObject<DropTailQueue<Packet>>());

    // Later, we add IP addresses.
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.252");
    Ipv4InterfaceContainer iAiB = ipv4.Assign(dAB);

    ipv4.SetBase("10.1.1.4", "255.255.255.252");
    Ipv4InterfaceContainer iBiC = ipv4.Assign(dBC);

    Ptr<Ipv4> ipv4A = c.Get(0)->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4B = c.Get(1)->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4C = c.Get(2)->GetObject<Ipv4>();

    // int32_t ifIndexA = ipv4A->AddInterface(deviceA);
    // int32_t ifIndexC = ipv4C->AddInterface(deviceC);

    // Ipv4InterfaceAddress ifInAddrA =
    //     Ipv4InterfaceAddress(Ipv4Address("172.16.1.1"), Ipv4Mask("/32"));
    // ipv4A->AddAddress(ifIndexA, ifInAddrA);
    // ipv4A->SetMetric(ifIndexA, 1);
    // ipv4A->SetUp(ifIndexA);

    // Ipv4InterfaceAddress ifInAddrC =
    //     Ipv4InterfaceAddress(Ipv4Address("10.1.1.6"), Ipv4Mask("/32"));
    // //    Ipv4InterfaceAddress(Ipv4Address("192.168.1.1"), Ipv4Mask("/32"));
    // ipv4C->AddAddress(ifIndexC, ifInAddrC);
    // ipv4C->SetMetric(ifIndexC, 1);
    // ipv4C->SetUp(ifIndexC);

    for ( auto i = iBiC.Begin(); i != iBiC.End(); i++)
    {
        Ptr<Ipv4> ip4 = i->first;
        uint32_t itf = i->second;
        NS_LOG_UNCOND(ip4->GetAddress(itf, 0));
    }

    // for ( auto i = iBiC.Begin(); i != iAiB.End(); i++)
    // {
    //     Ptr<Ipv4> ip4 = i->first;
    //     uint32_t itf = i->second;
    //     NS_LOG_UNCOND(ip4->GetAddress(itf, 0));
    // }

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    // Create static routes from A to C
    Ptr<Ipv4StaticRouting> staticRoutingA = ipv4RoutingHelper.GetStaticRouting(ipv4A);
    // The ifIndex for this outbound route is 1; the first p2p link added
    staticRoutingA->AddHostRouteTo(Ipv4Address("192.168.1.1"), Ipv4Address("10.1.1.2"), 1);
    staticRoutingA->AddHostRouteTo(Ipv4Address("10.1.1.6"), Ipv4Address("10.1.1.2"), 1);
    Ptr<Ipv4StaticRouting> staticRoutingB = ipv4RoutingHelper.GetStaticRouting(ipv4B);
    // The ifIndex we want on node B is 2; 0 corresponds to loopback, and 1 to the first point to
    // point link
    staticRoutingB->AddHostRouteTo(Ipv4Address("192.168.1.1"), Ipv4Address("10.1.1.6"), 2);
    staticRoutingB->AddHostRouteTo(Ipv4Address("10.1.1.6"), Ipv4Address("10.1.1.6"), 2);
    // Create the OnOff application to send UDP datagrams of size
    // 210 bytes at a rate of 448 Kb/s
    
    Ipv4Address serverAddress = Ipv4Address("10.1.1.6"); //("192.168.1.1");
    uint16_t serverPort = 9; // Discard port (RFC 863)
    uint32_t MaxPacketSize = 1024;
    Time interPacketInterval = Seconds (60);
    uint32_t maxPacketCount = 10;

    UdpServerHelper server(serverPort);
    ApplicationContainer apps = server.Install(c.Get(2));
    apps.Start(Seconds(1.0));
    apps.Stop(interPacketInterval * (maxPacketCount + 1));


    UdpClientHelper client(serverAddress, serverPort);
    client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    client.SetAttribute ("Interval", TimeValue (interPacketInterval));
    client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

    apps = client.Install(c.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(interPacketInterval * (maxPacketCount + 1));

    // OnOffHelper onoff("ns3::UdpSocketFactory",
    //                   Address(InetSocketAddress(ifInAddrC.GetLocal(), port)));
    // onoff.SetConstantRate(DataRate(6000));
    // ApplicationContainer apps = onoff.Install(c.Get(0));
    // apps.Start(Seconds(1.0));
    // apps.Stop(Seconds(10.0));

    // // Create a packet sink to receive these packets
    // PacketSinkHelper sink("ns3::UdpSocketFactory",
    //                       Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    // apps = sink.Install(c.Get(2));
    // apps.Start(Seconds(1.0));
    // apps.Stop(Seconds(10.0));




    // AsciiTraceHelper ascii;
    // p2p.EnableAsciiAll(ascii.CreateFileStream("sat-routing-demo.tr"));
    // p2p.EnablePcapAll("sat-routing-demo");

    sim_params_t params = {
        .udp_server = c.Get(0),
        .udp_client = c.Get(1),
        .router = c.Get(2)
    };


    if (server.GetServer()->TraceConnectWithoutContext("RxWithAddresses", MakeBoundCallback(&serverRxClbk, &params)))
    {
        NS_LOG_UNCOND("Trace connected successfully!");
    }


    NS_LOG_UNCOND("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_UNCOND("Done.");

    return 0;
}