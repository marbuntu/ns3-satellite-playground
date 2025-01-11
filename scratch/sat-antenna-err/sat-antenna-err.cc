

#include <ns3/core-module.h>
#include <ns3/sat-isl-antenna.h>
#include <ns3/double.h>
#include <ns3/enum.h>
using namespace ns3;



int main ( int argc, char* argv[] )
{

    Ptr<SatelliteISLAntenna> ant = CreateObjectWithAttributes<SatelliteISLAntenna>(
        "RadiationPattern", EnumValue(SatelliteISLAntenna::RP_Cosine),
        "MaxGainDbi", DoubleValue(30),
        "OpeningAngle", DoubleValue(160.0),
        "PointingErrorScalingFactor", DoubleValue(0.5)
    );

    //ant->GetPointingErrorModel()->SetAttribute()


    for(size_t n = 0; n < 100; n++)
    {
        double gain_db = ant->GetGainDb(Angles(0, 0));
        std::cout << n << "\t" << gain_db << "\t" << ant->GetPointingErrorDb(gain_db) << "\n";
    }


    return 0;
}