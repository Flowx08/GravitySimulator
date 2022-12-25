#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace config
{
	//simulation
	static const char* mode = "barneshut"; //"precise" or "barneshut"
	static const unsigned int particlesCount = 1500000;
	static const bool drawParticles = true;
	static const float gForce = 0.36;
	static const float minForceDistance = 4.0;

	//hardware
	static const unsigned int cpuCores = 10;

	//view
	static const float startViewZoom = 0.2;	

	//UI
	static const bool showUI = true;
	
	//recording
	static const bool record = false;
	static const bool recordLimit = false;
	static const unsigned int recordForT = 60 * 11;
	static const char* recordFilename = "recording.bin";
	static const bool playFromRecord = true;

	//barnes hut
	static const float alpha = 0.9;

}

#endif 
