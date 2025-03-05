
#include <ns3/core-module.h>

#include <ns3/demo-setup-helper.h>
#include <ns3/sat-isl-interface-helper.h>
#include <ns3/sat-isl-antenna.h>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("mlxsat_Paper_Demo");

typedef struct
{
    /** Simulate Antenna Gain Constant (=false) / Dynamic (=true) */
    bool simDynamic;

    /** Channel */
    Ptr<SatelliteISLChannel> channel;

    /** Node 0 -> Transmitter / Nodes 1-N -> Receivers */
    NodeContainer nodes; 

    /** Iteration Counter */
    size_t idx;

    /** Noise Temperature */
    float T0;

    /** Output File Stream */
    std::ofstream *output;

} sim_params_t;


/**
 * TODO: Include Random Gaussian Noise
 * TODO: 
 * 
 * 
 */

// static void plot_terminals(Ptr<SatelliteISLTerminal> )



static void plot_channel_gain(sim_params_t *params)
{
    Ptr<Node> tx_node = params->nodes.Get(0);
    Ptr<SatelliteISLNetDevice> tx_net = StaticCast<SatelliteISLNetDevice>(tx_node->GetDevice(0));
    Ptr<MobilityModel> tx_mob =  tx_node->GetObject<MobilityModel>();
    Ptr<PropagationLossModel> loss = params->channel->GetPropagationLossModel();

    Ptr<LVLHReference> ref = tx_net->GetLocalReference();
    ref->UpdateLocalReference(tx_mob->GetPosition(), tx_mob->GetVelocity());

    Ptr<MobilityModel> rx_mob = params->nodes.Get(4)->GetObject<MobilityModel>();


    *params->output << params->idx++ << "\t";

    for (auto ter_it = tx_net->BeginTerminals(); ter_it != tx_net->EndTerminals(); ter_it++)
    {
        if (params->simDynamic)
        {

            Ptr<SatelliteISLTerminal> ter = *ter_it;
            double Gtx = ter->GetAntennaGain(tx_mob, rx_mob) - 32.13;
            
            Ptr<SatelliteISLAntenna> ant = StaticCast<SatelliteISLAntenna>(ter->GetAntennaModel());
            double Gerr = ant->GetPointingErrorDb(Gtx);
            ant->SetAttribute("PointingErrorScalingFactor", DoubleValue(10));
            
            *params->output << Gtx - Gerr << "\t";
            
            // std::cout << Gerr << "\n";
        }
        else
        {
            Ptr<SatelliteISLTerminal> ter = *ter_it;
            double Gtx = ter->GetAntennaGain(tx_mob, rx_mob) - 32.13;

            *params->output << Gtx << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t";
        }
    }

    *params->output << loss->CalcRxPower(0, tx_mob, rx_mob) << "\t";

    *params->output << "\n";
}



int main( int argc, char* argv[] )
{

    size_t N = 24 * 60;
    Time step = Seconds(10);
    
    bool simDynamic = true;


    SatDemoSatClusterForFL::Initialize();
    //SatDemoSetup::Initialize();
    NodeContainer nodes = SatDemoSatClusterForFL::GetDefaultNodes();


    Ptr<SatelliteISLChannel> channel = CreateObject<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss = CreateObject<FriisPropagationLossModel>();
    Ptr<ConstantSpeedPropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();
    channel->SetPropagationLossModel(loss);
    channel->SetPropagationDelayModel(delay);

    std::ofstream out;

    if (simDynamic)
    {
        SatelliteISLInterfaceHelper itf_helper = DefaultISLInterfaceSetup::GetDefaultFactory(DefaultISLInterfaceSetup::SYMMETRIC_4x);
        itf_helper.Install(nodes, channel);
        out.open("../pyplot/data/paper-demo-gain-dynamic.txt");
    }
    else
    {
        SatelliteISLInterfaceHelper itf_helper = DefaultISLInterfaceSetup::GetDefaultFactory(DefaultISLInterfaceSetup::ISOTROPIC);
        itf_helper.Install(nodes, channel);
        out.open("../pyplot/data/paper-demo-gain-constant.txt");
    }


    

    sim_params_t params = {
        .simDynamic = simDynamic,
        .channel = channel,
        .nodes = nodes,
        .idx = 0,
        .T0 = 500,
        .output = &out
    };

    for (size_t n = 0; n < N; n++)
    {
        Simulator::Schedule(step * n, plot_channel_gain, &params);
    }

    NS_LOG_UNCOND("Starting Simulator...");
    Simulator::Run();

    NS_LOG_UNCOND("Clear Simulator...");
    Simulator::Destroy();

    out.close();

    return 0;
}