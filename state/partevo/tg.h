	
#include<map>
#include<string>
#include<algorithm>
#include"state.h"
#include"framebuffer.h"
#include"texture.h"
#include"midi.h"
#include"music.h"
#include"shaders.h"
#include"state/common/camera.h"


class graph;

struct node{

	glm::vec3 pos;
	glm::vec3 acc;

	//This is a complete one-way set of connections. It will contain only edges to nodes added after this node.
	std::set<uint> connections;

	node(glm::vec3 pos) : pos(pos) {}
	void fix(float *p){
		//All physics for this node and tick have been run. Finalize results.
		pos += acc;
		p[0] = pos.x;
		p[1] = pos.y;
		p[2] = pos.z;
	}
	void ready(){
		//ready node state for a physics tick.
		acc = glm::vec3(0.0,0.0,0.0);
	}
	void attract(node &n){
		//this && n are connected, apply physics based on this fact.
		
		glm::vec3 dir = n.pos-pos;
		float dist = glm::length(dir);
		dir = glm::normalize(dir);

		n.acc -= dist*dist*dist*dir;
		acc += dist*dist*dist*dir;
		n.acc += dist*dist*dir;
		acc -= dist*dist*dir;
	}
	void detract(node &n){
		//this && n are NOT connected, apply physics based on this fact.
		glm::vec3 dir = n.pos-pos;
		float dist = glm::length(dir);
		dir = glm::normalize(dir);

		n.acc += dist*dist*dir;
		acc -= dist*dist*dir;
	}

};

class graph {
	std::vector<node> nodes;
	GLfloat *data;
	size_t sz;
	Instance<Buffer<GLfloat>> buf;
	Instance<VArray> va;
public:

	graph() : sz(1) {
		data = (GLfloat*)malloc(sz*3);
	}
	~graph(){
		free(data);
	}

	void init(){
		va = VArray();
		buf = Buffer<GLfloat>();
	}

	uint add(glm::vec3 pos){
		nodes.push_back(node(pos));
		if(sz<nodes.size()){
			free(data);
			sz *= 2;
			data = (GLfloat*) malloc(sz*3*sizeof(GLfloat));
		}
		return nodes.size()-1;
	}

	//Connect two nodes with an edge.
	void connect(uint a, uint b){

		//ignore nonexistant nodes.
		if(nodes.size() <= a || nodes.size() <= b) return;

		//ignore same node connections, connect low value to high value.
		if(a<b)
			nodes[a].connections.insert(b);
		else if(b>a)
			nodes[b].connections.insert(a);
	}

	//Run a physics tick on all nodes.
	void update(){
		for(uint i=0;i<nodes.size(); ++i){
			nodes[i].ready();
		}
		for(uint i=0;i<nodes.size(); ++i){
			node &n = nodes[i];
			auto a = n.connections.begin();
			auto b = n.connections.end();
			for(uint j=i+1;j<nodes.size(); ++j){
				while(a!=b && *a < j) ++a;
				if(a!=b && *a == j){
					n.attract(nodes[j]);
					++a;
				} else {
					n.detract(nodes[j]);
				}
			}
			//Done what we want to do with n, so set pos=npos.
			n.fix(data+sizeof(GLfloat)*3*i);
		}
	}

	void draw(){
		buf->data(sz, data);
		va->vertices(0, sz, *buf);

		va->draw(0, nodes.size());
	}
};


struct tgdata{
	midi mi;
	music *m;
	FSShader fss;
	Texture t, t2, t3, t4;
	bool reload;
	camera c;
	graph g;

	tgdata() : mi(midi("statedata/df/gating.mid")),
				 fss(Shader::version330()+Shader::noiselibs()+"statedata/df/hdr.frag"),
				 m(new music("statedata/df/DH-Final1.mp3")),
				 c(),
				 t(1, GL_R32F, GL_TEXTURE_2D, 128, 24),
				 t2(1, GL_R32F, GL_TEXTURE_2D, 24, 1), 
				 t3(1, GL_R32F, GL_TEXTURE_2D, 24, 1), 
				 t4(1, GL_R32F, GL_TEXTURE_2D, 1024, 1), reload(false) {
	}
	~tgdata(){
	}
	void refresh(){
		reload = false;
		uint a1 = g.add(glm::vec3(0,0,0));
		uint a2 = g.add(glm::vec3(0,0.5,0));
		uint a3 = g.add(glm::vec3(0.5,0,0));
		g.connect(a1, a2);
		g.connect(a1, a3);
	}
};

class tg : public shiva::behaviour {
	virtual void run(shiva::eventmap &received, shiva::state *instance, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		if(received.count("kinput")){
			std::pair<int,bool> g = boost::any_cast<std::pair<int,bool>>(received["kinput"]);
			if(g.first == GLFW_KEY_ESC)
				shiva::core::stop();
			if(g.first == GLFW_KEY_LEFT){
				d.m->time(d.m->time()-1.0);
			}
			if(g.first == GLFW_KEY_RIGHT){
				d.m->time(d.m->time()+1.0);
			}
			if(g.first == GLFW_KEY_SPACE && g.second){
				d.m->pause(!d.m->pause());
			}
		}
		
		if(received.count("resourcechange")){
			d.reload = true;
		}
		d.mi.run(d.m->time());
		d.c.run(received, d.m->time(), false);
		//if(d.m->time() > 155) shiva::core::stop();
	}
	virtual void render(shiva::childvec children, boost::any &data) const {
		auto &d = boost::any_cast<tgdata&>(data);
		std::for_each(children.begin(), children.end(), [](shiva::state* s){s->render();});

		if(d.reload){
			d.refresh();
		}

		d.t.bind();
		for(int i=0;i<24;++i){
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, 128, 1, GL_RED, GL_FLOAT, d.mi.vn[i].data());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float f[24];
		float g[24];
		for(int i=0;i<24; ++i){
			f[i] = d.mi.v[i];
			g[i] = d.mi.pos[i];
		}
		d.t2.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,24,1,GL_RED, GL_FLOAT, f);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		d.t3.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,24,1,GL_RED, GL_FLOAT, g);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		auto h = d.m->spectrum();
		d.t4.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0,0,0,1024,1,GL_RED, GL_FLOAT, h.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		float speed = 3.0;
		glm::mat4 cam = glm::transpose(glm::gtc::matrix_transform::perspective(90.f, (float)shiva::core::width()/(float)shiva::core::height(), 1.0f, 5000.0f)
				*glm::mat4_cast(glm::rotate(glm::quat(), -d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), -d.c.rot.x, glm::vec3(0,1,0)))
				*glm::translate(glm::mat4(1.0f), d.c.trans*speed*glm::vec3(-1.f,-1.f,1.f))
				*glm::mat4(glm::vec4(1.f,0.f,0.f,0.f), glm::vec4(0.f,1.f,0.f,0.f), glm::vec4(0.f,0.f,-1.f,0.f), glm::vec4(0.f,0.f,0.f,1.f)));
		glm::mat4 rotcam = glm::transpose(glm::mat4_cast(glm::rotate(glm::quat(), d.c.rot.y, glm::vec3(1,0,0))*glm::rotate(glm::quat(), d.c.rot.x, glm::vec3(0,1,0))));
		glm::vec4 ptrans(d.c.trans, 0.0); 

		
		d.fss.program().bindTexture("bt", d.t,0);
		d.fss.program().bindTexture("track", d.t2,1);
		d.fss.program().bindTexture("pos", d.t3,2);
		d.fss.program().bindTexture("wform", d.t4,3);
		
		d.fss.set("rot", glm::mat3(rotcam));
		d.fss.set("trans", glm::vec3(ptrans*speed));
		d.fss.set("time", d.m->time());
		d.fss.program().set("camera", cam);

		d.fss.display();
	}

	virtual void init(shiva::state *instance, boost::any &data) const {
		//auto s = instance->create("hdr_node", "hdr");
		//s->create("particle_node", "particles");
		instance->reg("kinput");
		instance->reg("resourcechange");


		data = boost::any(tgdata());
		auto &d = boost::any_cast<tgdata&>(data);

		d.c.init(instance, "statedata/df/na.camera");

		d.m->pause(false);
	}
	virtual void uninit(shiva::state *instance, boost::any &data) const {}
};
behaviour_add(tg);