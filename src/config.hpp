#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace config
{
	//simulation
	static const char* mode = "barneshut"; //"precise" or "barneshut"
	static const unsigned int particlesCount = 1000000;
	static const bool drawParticles = false;
	static const float gForce = 0.26;
	static const float minForceDistance = 4.0;

	//hardware
	static const unsigned int cpuCores = 10;

	//view
	static const float startViewZoom = 0.2;	

	//UI
	static const bool showUI = true;
	
	//recording
	static const bool record = true;
	static const bool recordLimit = true;
	static const unsigned int recordForT = 60 * 10;
	static const char* recordFilename = "recording.bin";
	static const bool playFromRecord = false;

	//barnes hut
	static const float alpha = 0.9;

}

#endif 
