


#include <ns3/core-module.h>
#include <ns3/demo-setup-helper.h>

#include <ns3/sat-node-helper.h>
#include <ns3/sat-isl-antenna.h>
#include <ns3/sat-node-tag.h>
#include <ns3/sat-isl-channel.h>
#include <ns3/sat-isl-terminal.h>
#include <ns3/sat-isl-intercon-table.h>
#include <iostream>
#include <valarray>
#include <math.h>
#include <cmath>


using namespace ns3;


/**
 *  Let's start by calculating the latency - interconnect matrix
 * 
 * @param cid 
*/

// alt = 2000 km

static double get_crit_distance(double altitude)
{
    double hyp = WalkerConstellationHelper::meanEarthRadius + altitude;
    double sid = WalkerConstellationHelper::meanEarthRadius + 80;

    return 2 * sqrt((hyp * hyp) - (sid * sid));
}


static size_t find_shortest_right(orbid_t orb, Ptr<Node> sat)
{
    Ptr<MobilityModel> mob1 = sat->GetObject<MobilityModel>();
    double min_dist = 0;
    size_t min_arg = 0;

    for (satid_t sid : SatelliteNodeTag::SatsByOrbit(orb))
    {
        Ptr<Node> dst = SatelliteNodeTag::GetSatellite(sid);
        Ptr<MobilityModel> mob2 = dst->GetObject<MobilityModel>();

        double dist = mob1->GetDistanceFrom(mob2);

        if ((min_arg == 0) || (dist < min_dist))
        {
            min_arg = sid;
            min_dist = dist;
        }

    }

    // std::cout << "arg: " << min_arg << "\t" << min_dist << "\n";

    return min_arg;
}


static void setup_gridplus(size_t cid)
{
    //size_t No = SatelliteNodeTag::GetOrbitsN();
    size_t No = SatelliteNodeTag::GetOrbitsN();
    size_t Ns = SatelliteNodeTag::GetSatsN() / No;
    
    size_t o = 1;
    size_t so = 0;
    SatISLInterconTable tbl;


    std::cout << "\n\nPlus Grid Setup\n> No: " << No << "\n> Ns: " << Ns << "\n"; 

    for (orbid_t oid : SatelliteNodeTag::OrbitsByConstellation(cid))
    {
        so = 1;
        for (satid_t sat_id : SatelliteNodeTag::SatsByOrbit(oid))
        {
            Ptr<Node> sat = SatelliteNodeTag::GetSatellite(sat_id);

            std::cout << "Orb: " << oid << "\tSat: " << sat_id << "\n";

            // First Sat on Orbit            
            if (so == 1)
            {   
                // Leader
                tbl.Add(sat_id, sat_id + 1);
                // Follower
                tbl.Add(sat_id, (sat_id + Ns) - 1);
            }
            else if (so < Ns)
            {
                // Leader
                tbl.Add(sat_id, sat_id + 1);
                // Follower
                tbl.Add(sat_id, sat_id - 1);
            }
            else
            {
                // Leader
                tbl.Add(sat_id, (sat_id - Ns) + 1);
                // Follower
                tbl.Add(sat_id, sat_id - 1);
            }

            // Left
            if (o > 1)
            {
                satid_t s = find_shortest_right(o-1, sat);
                tbl.Add(sat_id, s); //sat_id - Ns + 1);
                //std::cout << "> Left: " << (sat_id - Ns) << "\n";
            }

            // Right
            if (o < No)
            {
                satid_t s = find_shortest_right(o+1, sat);
                tbl.Add(sat_id, s);
                //if (so < Ns) tbl.Add(sat_id, sat_id + Ns - 1);
                //std::cout << "> Right: " << (sat_id + Ns) << "\n";
            }

            so++;
        }

        o++;
    }

}


static uint64_t check_link_available(Ptr<Node> sat1, Ptr<Node> sat2)
{

    Ptr<SatelliteISLNetDevice> net1 = StaticCast<SatelliteISLNetDevice>(sat1->GetDevice(0));
    Ptr<SatelliteISLNetDevice> net2 = StaticCast<SatelliteISLNetDevice>(sat2->GetDevice(0));

    Ptr<MobilityModel> mob1 = sat1->GetObject<MobilityModel>();
    Ptr<MobilityModel> mob2 = sat2->GetObject<MobilityModel>();
    Ptr<SatelliteISLChannel> chn = StaticCast<SatelliteISLChannel>(net1->GetChannel());

    Ptr<LVLHReference> ref = net1->GetLocalReference();
    ref->UpdateLocalReference(mob1->GetPosition(), mob1->GetVelocity());

    DataRate rate(0);
    Ptr<SatelliteISLTerminal> term;

    Ptr<SatelliteNodeTag> tag1 = sat1->GetObject<SatelliteNodeTag>();
    Ptr<SatelliteNodeTag> tag2 = sat2->GetObject<SatelliteNodeTag>();

    SatISLInterconTable tbl;


    if (!tbl.IsAvailable(tag1->GetId(), tag2->GetId()))
    {
        return rate.GetBitRate();
    }


    for (auto terminal = net1->BeginTerminals(); terminal != net1->EndTerminals(); terminal++)
    {
        //NS_LOG_UNCOND(terminal);
        DataRate new_rate = (*terminal)->GetRateEstimation(mob1, mob2, chn->GetPropagationLossModel()); //.GetRateEstimation(mob1, mob2, chn->GetPropagationLossModel());
        
        if ((new_rate > 0) && (new_rate > rate))
        {
            //term = terminal.base;
            rate = new_rate;
        } 
        
    }
    
    return rate.GetBitRate();
}




static void plot_latency_matrix()
{

    double crit_dist = get_crit_distance(200);

    std::ofstream out("./pyplot/data/sat-latency-matrix.txt");

    std::ofstream bit("./pyplot/data/sat-datarate-matrix.txt");

    std::ofstream edr("./pyplot/data/sat-edr-matrix.txt");


    for (size_t sc = 1; sc <= SatelliteNodeTag::GetSatsN(); sc++)
    {
        Ptr<Node> sat1 = SatelliteNodeTag::GetSatellite(sc);
        Ptr<MobilityModel> mob1 = sat1->GetObject<MobilityModel>();

        for (size_t so = 1; so <= SatelliteNodeTag::GetSatsN(); so++)
        {
            Ptr<Node> sat2 = SatelliteNodeTag::GetSatellite(so);
            Ptr<MobilityModel> mob2 = sat2->GetObject<MobilityModel>();
            
            double dist = mob1->GetDistanceFrom(mob2) * 1e-3;
            

            if (so == sc)
            {
                out << "\t0";
                bit << "\t0";
                edr << "\t0";
            }
            else
            {
                
                if (dist > crit_dist)
                {
                    out << "\t-20";
                    bit << "\t0";
                    edr << "\t0";
                    continue;
                }


                uint64_t bitrate = (check_link_available(sat1, sat2) * 1e-6);   // BR in Mbps
                double latency = round(dist / 299.792458);                      // latency in ms
                double effbw = bitrate * (latency * 1e-3);                      // Effective Bandwidth in Mbit

                bit << "\t" << bitrate;
                out << "\t" << latency;
                edr << "\t" << effbw;
            }
        }

        //std::cout << "Min to " << sc << " - " << arg_min << " dist: " << ((crit_dist - d_min ) > 0 ? d_min : 0)  << "\n";

        bit << "\n";
        out << "\n";
        edr << "\n";
    }

    out.close();
    bit.close();
    edr.close();
}



static void generate_lsdb()
{


    plot_latency_matrix();

    // setup_gridplus(3);




    
    //std::cout << "Crit Distance: " << get_critHeight(550) << "\n";
}






int main (int argc, char *argv[] )
{
    Ptr<SatelliteISLChannel> channel = CreateObjectWithAttributes<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss_model = CreateObject<FriisPropagationLossModel>();
    Ptr<ConstantSpeedPropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();
    channel->SetPropagationLossModel(loss_model);
    channel->SetPropagationDelayModel(delay);

    SatDemoSatClusterForFL::Initialize();

    //SatDemoSetup::Initialize();

    // NodeContainer nodes = SatDemoSetup::GetDefaultNodes();

    SatelliteNodeHelper nodehelper;
    NodeContainer nodes;

    for (uint16_t oid : SatelliteNodeTag::OrbitsByConstellation(3))
    {
        for (auto sat_id : SatelliteNodeTag::SatsByOrbit(oid))
        {
            Ptr<Node> sat = SatelliteNodeTag::GetSatellite(sat_id);
            nodes.Add(sat);
        }
    }

    SatelliteISLInterfaceHelper itf_helper = DefaultISLInterfaceSetup::GetDefaultFactory(DefaultISLInterfaceSetup::SYMMETRIC_4x);
    NetDeviceContainer net_devs = itf_helper.Install(nodes, channel);

        for (uint16_t oid : SatelliteNodeTag::OrbitsByConstellation(3))
    {
        for (auto sat_id : SatelliteNodeTag::SatsByOrbit(oid))
        {
            Ptr<Node> sat = SatelliteNodeTag::GetSatellite(sat_id);
            nodehelper.printSatNodeInfo(std::cout, sat);
        }
    }


    setup_gridplus(3);

    // std::for_each(Nit.first, Nit.second, 
    //     [&](const auto &n) {std::cout << n.first << "\t" << n.second << "\n";} 
    // );


    // nodehelper.SetISLChannel(channel);
    // nodehelper.Install(nodes);

    // SatelliteNodeTag tag;

    // Ptr<Node> sat1 = nodehelper.Create();

    // for (NodeContainer::Iterator it = nodes.Begin(); it != nodes.End(); it++)
    // {
    //     nodehelper.printSatNodeInfo(std::cout, *it);
    // }


    // for (uint16_t oid : SatelliteNodeTag::OrbitsByConstellation(1))
    // {
    //     for (auto sat_id : SatelliteNodeTag::SatsByOrbit(oid))
    //     {
    //         Ptr<Node> sat = SatelliteNodeTag::GetSatellite(sat_id);

    //         nodehelper.printSatNodeInfo(std::cout, sat);
    //     }
    // }


    // for (uint16_t oid : SatelliteNodeTag::OrbitsByConstellation(2))
    // {
    //     for (auto sat_id : SatelliteNodeTag::SatsByOrbit(oid))
    //     {
    //         Ptr<Node> sat = SatelliteNodeTag::GetSatellite(sat_id);

    //         nodehelper.printSatNodeInfo(std::cout, sat);
    //     }
    // }

    generate_lsdb();


    return 0;
}