
#include"shiva.h"

class parametric {
	std::map<float, std::function<glm::vec2 (float a, boost::any)>> slices;
	std::function<float (float a, boost::any)> width;
	std::function<glm::vec3 (float a, boost::any)> path;

	std::function<glm::vec2 (float a, boost::any)> prevslice(float f){

	}
	std::function<glm::vec2 (float a, boost::any)> nextslice(float f){

	}

	std::vector<float> generate(float lendetail=0.1f, float rotdetail=0.1f, float start=0.f, float end = 1.0f, float rotstart = 0.f, float rotend = 3.141593f){
		if(lendetail <= 0) lendetail = 0.01;
		if(rotdetail <= 0) rotdetail = 0.01;
		
		for(float f=rotstart; f<=rotend; f+=rotdetail){
			for(float g=start; g<=end; g+=lendetail){
				auto prev = prevslice(g);
				auto next = nextslice(g);


				
			}
		}
	}


};