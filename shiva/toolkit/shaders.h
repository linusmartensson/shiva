#pragma once
#include"stdafx.h"
#include"utils.h"
#include"texture.h"
#include"varray.h"
#include<sstream>
#include<string>
struct ShaderCompilerException{};
struct ProgramLinkerException{};


class Shader{
	uint *refs;
public:
	const GLuint shader;
	const GLenum type;
	const std::string source;

	static std::string noiselibs(){
		return "builtins/glsllib/simplexnoise/noisecommon.glsl \
			    builtins/glsllib/simplexnoise/noise2D.glsl \
				builtins/glsllib/simplexnoise/noise3D.glsl \
				builtins/glsllib/simplexnoise/noise4D.glsl ";
	}
	static std::string version330(){
		return "builtins/glsllib/version330.glsl ";
	}
	
	Shader(GLenum type, std::string source) : type(type), source(source), shader(glCreateShader(type)), refs(new uint(1)){
		const GLchar *c = source.c_str();
		const GLint l = source.length();
		glShaderSource(shader, 1, &c, &l);
		glCompileShader(shader);
		GLint comp;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &comp);
		if(comp == GL_FALSE){
			std::cout << "Shader compilation failed." << std::endl;
			Log::shaderCompiler(shader);
			throw ShaderCompilerException();
		}
	}
	Shader(const Shader &rhs) : refs(rhs.refs), shader(shader), type(type), source(source) {
		(*refs)++;
	}
	~Shader(){
		if(--*refs == 0){
			delete refs;
			glDeleteShader(shader);
		}
	}
	void attach(GLuint program){
		glAttachShader(program, shader);
	}
};

class Program{
	
	uint *refs;
	uint *program;
	static uint current;
	std::string *frag, *geom, *vert;
public:
	
	static void reset(){
		current = 0;
	}

	Program& attach(Shader *s){
		s->attach(*program);
		return *this;
	}
	void link(){
		glLinkProgram(*program);
		GLint l;
		glGetProgramiv(*program, GL_LINK_STATUS, &l);
		if(l == GL_FALSE){
			std::cout << "Shader program linking failed." << std::endl;
			Log::programLinker(*program);
			throw ProgramLinkerException();
		}
	}

	Program() : program(new uint), refs(new uint(1)) { *program = glCreateProgram(); }
	Program(std::string frag, std::string geom="common/shapes/quad.geom", std::string vert="builtins/null.vert") : program(new uint), refs(new uint(1)){
		this->frag = new std::string(frag); this->vert = new std::string(vert); this->geom = new std::string(geom);
		*program = glCreateProgram();
		Shader	v(GL_VERTEX_SHADER,		loadFile(vert)),
				g(GL_GEOMETRY_SHADER,	loadFile(geom)),
				f(GL_FRAGMENT_SHADER,	loadFile(frag));
		attach(&v).attach(&g).attach(&f).link();
	}
	Program(const Program &rhs) : refs(rhs.refs), program(rhs.program), frag(rhs.frag), geom(rhs.geom), vert(rhs.vert) { (*refs)++; }
	~Program(){
		if(--*refs == 0){
			delete refs;
			if(current == *program){
				current = 0;
				glUseProgram(0);
			}
			glDeleteProgram(*program);
			delete program;
			delete frag;
			delete geom;
			delete vert;
		}
	}
	bool rebuild(std::string frag, std::string geom="common/shapes/quad.geom", std::string vert="builtins/null.vert"){
		uint oldprogram = *program;
		*program = glCreateProgram();
		*this->frag = frag;
		*this->geom = geom;
		*this->vert = vert;
		try{
			Shader	v(GL_VERTEX_SHADER,		loadFile(vert)),
				g(GL_GEOMETRY_SHADER,	loadFile(geom)),
				f(GL_FRAGMENT_SHADER,	loadFile(frag));
			attach(&v).attach(&g).attach(&f).link();
		} catch (...){
			glDeleteProgram(*program);
			*program = oldprogram;
			return false;
		}
		glDeleteProgram(oldprogram);
		current = 0;
		return true;

	}
	bool rebuild(){
		return rebuild(*frag, *geom, *vert);
	}
	void use(){
		if(current != *program)
			glUseProgram(*program);
		current = *program;
	}
	void bindAttrib(GLuint index, const std::string &attrib){
		glBindAttribLocation(*program, index, attrib.c_str());
		link();
	}
	int getAttribLocation(const std::string &attrib){
		return glGetAttribLocation(*program, attrib.c_str());
	}
	int getUniformLocation(const std::string &uniform){
		return glGetUniformLocation(*program, uniform.c_str());
	}
	void bindFragData(GLuint index, const std::string &fragdata){
		glBindFragDataLocation(*program, index, fragdata.c_str());
		link();
	}
	GLint uniform(const std::string &name){
		GLint r = glGetUniformLocation(*program, name.c_str());
		Log::GLErrors("glGetUniformLocation(program, name.c_str());");
		return r;
	}
	void bindTexture(const std::string &name, Texture &t, uint texturepos = 0){

		uint p = current;
		use();
		
		Log::GLErrors("before glActiveTexture(GL_TEXTURE0+texturepos);");
		glActiveTexture(GL_TEXTURE0+texturepos);
		Log::GLErrors("glActiveTexture(GL_TEXTURE0+texturepos);");
		t.bind();
		
		glUniform1i(uniform(name), texturepos);
		if(p!=*program) glUseProgram(p);
		current = p;
	}
	
	void set(std::string var, float f){
		use();
		glUniform1f(uniform(var), f);
	}
	void set(std::string var, glm::mat3 m){
		use();
		glUniformMatrix3fv(uniform(var), 1, GL_FALSE, glm::value_ptr(m));
	}
	void set(std::string var, glm::mat4 m){
		use();
		glUniformMatrix4fv(uniform(var), 1, GL_FALSE, glm::value_ptr(m));
	}
	void set(std::string var, glm::vec2 m){
		use();
		glUniform2fv(uniform(var), 1, glm::value_ptr(m));
	}
	void set(std::string var, glm::vec3 m){
		use();
		glUniform3fv(uniform(var), 1, glm::value_ptr(m));
	}
	void set(std::string var, glm::vec4 m){
		use();
		glUniform4fv(uniform(var), 1, glm::value_ptr(m));
	}
};

class FSShader {
	uint *refs, *reps;
	bool *useclear;
	Program p;
	VArray va;
	std::vector<std::pair<std::string, Instance<Texture>>> *tins;
public:
	void input(std::string var, Texture &t){
		size_t i=0;
		for(;i!=tins->size();++i){
			if((*tins)[i].first == var) break;
		}
		if(i==tins->size())
			tins->push_back(std::make_pair(var, Instance<Texture>(t)));
		else
			(*tins)[i] = std::make_pair(var, Instance<Texture>(t));
	}
	GLint uniform(const std::string &s){
		return p.uniform(s);
	}
	Program& program(){
		return p;
	}
	template<typename T>
	void set(std::string name, T data){
		p.set(name, data);
	}
	void clear(bool doit=false){
		*useclear = doit;
	}
	void display(){
		Log::GLErrors("void FSShader::display() entry");
		if(*useclear){
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Log::GLErrors("glClear(GL_COLOR_BUFFER_BIT);");
		}
		p.use();
		Log::GLErrors("p.use();");
		for(int i= 0; i!=tins->size(); ++i){
			p.bindTexture((*tins)[i].first, *((*tins)[i].second), i);
			Log::GLErrors("p.bindTexture(tins[i].first, *(tins[i].second), i);");
		}
		va.draw(0,1,*reps,GL_POINTS);
	}
	template<typename T>
	void transform(T &m){
		p.set("m", m);
	}
	FSShader(std::string frag, std::string geom="common/shapes/quad.geom", std::string vert="builtins/null.vert",uint rr=1) : reps(new uint(rr)), useclear(new bool(true)), p(frag,geom,vert), refs(new uint(1)), va(), tins(new std::vector<std::pair<std::string, Instance<Texture>>>) {
		p.set("m", glm::mat4(1.0));
	}
	bool rebuild(std::string frag, std::string geom="common/shapes/quad.geom", std::string vert="builtins/null.vert"){
		bool ret = p.rebuild(frag,geom,vert);
		p.set("m", glm::mat4(1.0));
		return ret;
	}
	bool rebuild(){
		bool ret = p.rebuild();
		p.set("m", glm::mat4(1.0));
		return ret;
	}
	~FSShader(){
		if(--*refs == 0){
			delete refs;
			delete reps;
			delete tins;
			delete useclear;

		}
	}
	FSShader(const FSShader &rhs) : reps(rhs.reps), p(rhs.p), useclear(rhs.useclear), refs(rhs.refs), va(rhs.va), tins(rhs.tins) {
		(*refs)++;
	}
};


