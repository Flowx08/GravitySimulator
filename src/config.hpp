#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace config
{
	//simulation	
	static const unsigned int particlesCount = 150000;
	static const float gForce = 0.01 * 80;

	//view
	static const float startViewZoom = 0.25;	

	//UI
	static const bool showUI = true;
	
	//recording
	
	static const bool record = false;
	static const bool recordLimit = false;
	static const unsigned int recordForT = 60 * 10;
	static const char* recordFilename = "recording.bin";
	static const bool playFromRecord = true;

	//barnes hut
	static const float alpha = 0.9;

}

#endif 
