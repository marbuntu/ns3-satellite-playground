

#include <ns3/core-module.h>
#include <ns3/constant-velocity-mobility-model.h>
#include <ns3/sat-isl-channel.h>
#include <ns3/vector.h>
#include <ns3/vector-extensions.h>
#include <ns3/satellite-const-variables.h>
#include <limits>

using namespace ns3;


NS_LOG_COMPONENT_DEFINE("Satellite_ISL_Channel_Demo");


typedef struct 
{
    Ptr<Channel> channel;

    Ptr<MobilityModel> mob_tx;
    Ptr<MobilityModel> mob_rx;

    double frequencies[3];

    bool compensated;

    std::ofstream *output;
    size_t idx;
    size_t inv;

} sim_params_t;


/**
 * @brief   Calculate Doppler Compensated Frequency
 * 
 * @param fc        Center Frequency
 * @param mob_tx    Mobility Model Transmitter
 * @param mob_rx    Mobility Model Receiver
 * @return double 
 */
static double doppler_compensated_fc(double fc, Ptr<MobilityModel> mob_tx, Ptr<MobilityModel> mob_rx)
{
    Vector e_rs = Normalize(mob_rx->GetPosition() - mob_tx->GetPosition());
    double f_comp = fc * ((SatConstVariables::SPEED_OF_LIGHT - DotProduct(mob_rx->GetVelocity(), e_rs)) / (SatConstVariables::SPEED_OF_LIGHT - DotProduct(mob_tx->GetVelocity(), e_rs)));

    return f_comp;
}



/**
 * @brief   Generate and Plot
 * 
 * @param params Simulation Parameters
 */
static void plot_channel_gain(sim_params_t *params)
{

    Ptr<SatelliteISLChannel> chn = StaticCast<SatelliteISLChannel>(params->channel);
    Ptr<FriisPropagationLossModel> loss_model = StaticCast<FriisPropagationLossModel>(chn->GetPropagationLossModel());

    
    double dist = params->mob_tx->GetDistanceFrom(params->mob_rx);
    *params->output << params->idx << "\t" << dist;

    if (params->idx >= params->inv)
    {
        Ptr<ConstantVelocityMobilityModel> mob = StaticCast<ConstantVelocityMobilityModel>(params->mob_tx);
        mob->SetVelocity(mob->GetVelocity() * -1);
        params->inv = std::numeric_limits<size_t>::max();
    }

    for (const double &fc : params->frequencies)
    {
        loss_model->SetFrequency(fc);
        double loss = loss_model->CalcRxPower(0, params->mob_tx, params->mob_rx);

        double fc_comp = doppler_compensated_fc(fc, params->mob_tx, params->mob_rx);
        loss_model->SetFrequency(fc_comp);
        double loss_comp = loss_model->CalcRxPower(0, params->mob_tx, params->mob_rx);


        *params->output << "\t" << loss << "\t" << fc_comp << "\t" << loss_comp << "\t" << loss - loss_comp;
    }

    *params->output << "\n";
    params->idx++;
    
}



int main( int argc, char* argv[] )
{

    // Simulation Time Settings
    size_t N = 24 * 60;         //! Number of Intervals
    Time step = Seconds(1);    //! Period between Intervals
    
    
    // Channel Setup
    Ptr<SatelliteISLChannel> channel = CreateObjectWithAttributes<SatelliteISLChannel>();
    Ptr<FriisPropagationLossModel> loss_model = CreateObject<FriisPropagationLossModel>();
    loss_model->SetFrequency(10e9);
    channel->SetPropagationLossModel(loss_model);


    // Satellite Setup
    Ptr<ConstantVelocityMobilityModel> mob_tx = CreateObject<ConstantVelocityMobilityModel>();
    mob_tx->SetPosition(Vector(100, 0, 0));
    mob_tx->SetVelocity(Vector(2000, 0, 0));

    Ptr<ConstantVelocityMobilityModel> mob_rx = CreateObject<ConstantVelocityMobilityModel>();
    mob_rx->SetPosition(Vector(0, 0, 0));
    mob_rx->SetVelocity(Vector(0, 0, 0));


    std::ofstream outp("./pyplot/data/sat-isl-channel-gain.txt");
    outp << "Index \t Distance_m \t Gain_dB \t fc_comp_Hz \t Gain_comp_dB \t Gain_diff_dB";
    outp << "\t Gain_dB \t fc_comp_Hz \t Gain_comp_dB \t Gain_diff_dB";
    outp << "\t Gain_dB \t fc_comp_Hz \t Gain_comp_dB \t Gain_diff_dB" << "\n";
    
    sim_params_t params = {
        .channel = channel,
        .mob_tx = mob_tx,
        .mob_rx = mob_rx,
        .frequencies = { 30e9, 250e9, 200e12 },
        .compensated = true,
        .output = &outp,
        .idx = 0,
        .inv = size_t(N >> 1)
    };


    for (size_t n = 0; n < N; n++)
    {
        Simulator::Schedule(step * n, plot_channel_gain, &params);
    }

    NS_LOG_UNCOND("Starting Simulator...");
    Simulator::Run();

    NS_LOG_UNCOND("Clear Simulator...");
    Simulator::Destroy();


    return 0;
}