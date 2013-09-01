#pragma once
#include<string>
#include"shiva.h"
#include<png.h>

class PNGImage{
	uint *refs;
	uchar **image_data;
	double *display_exponent;
	uint *w, *h;
	int *bit_depth, *color_type;
	double  *gamma;
	
	const std::string path;
public:
	static void readData(png_structp a, png_bytep b, png_size_t c){
		std::ifstream &ifs = *(std::ifstream *)(png_get_io_ptr(a));
		ifs.read((char*)b, c);
		if(!ifs) png_error(a, "PNG ifstream error");
	}

	const uint &width() const {
		return *w;
	}
	const uint &height() const {
		return *h;
	}

	const uchar *data() const {
		return *image_data;
	}

	bool rebuild(){
		std::ifstream ifs(path.c_str(), std::ios::binary);
		
		auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) return false;
		auto info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr){
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}

		if (setjmp(png_jmpbuf(png_ptr))) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return false;
		}

		png_set_read_fn(png_ptr, &ifs, &PNGImage::readData);
		png_read_info(png_ptr, info_ptr); 
		png_get_IHDR(png_ptr, info_ptr, w, h, bit_depth, color_type, NULL, NULL, NULL);
		
		if (*color_type == PNG_COLOR_TYPE_PALETTE || (*color_type == PNG_COLOR_TYPE_GRAY && *bit_depth < 8) || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) 
			png_set_expand(png_ptr);
		if (*color_type == PNG_COLOR_TYPE_GRAY || *color_type == PNG_COLOR_TYPE_GRAY_ALPHA) 
			png_set_gray_to_rgb(png_ptr);
		
		if (*bit_depth == 16) 
			png_set_scale_16(png_ptr);
		if (png_get_gAMA(png_ptr, info_ptr, gamma))
			png_set_gamma(png_ptr, *display_exponent, *gamma);

		if(*color_type != PNG_COLOR_TYPE_RGBA && *color_type != PNG_COLOR_TYPE_RGB_ALPHA && *color_type != PNG_COLOR_TYPE_GRAY_ALPHA){
			png_set_add_alpha(png_ptr, 0xFF, 0);
		}
		png_read_update_info(png_ptr, info_ptr);

		auto rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		if(*image_data != 0) delete [] *image_data;
		*image_data = new uchar[rowbytes*(*h)];
		{
			auto row_pointers = new png_bytep[*h*sizeof(png_bytep)];
			for (size_t i = 0;  i < *h; ++i) row_pointers[i] = (*image_data) + i*rowbytes;
			png_read_image(png_ptr, row_pointers);
			delete [] row_pointers;
		}

		png_read_end(png_ptr, NULL);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		png_ptr = NULL;
		info_ptr = NULL;
		return true;
	}
	PNGImage(std::string ipath) : 
		path("resources/"+ipath), 
			display_exponent(new double(1.0)), 
			image_data(new uchar*(0)), 
			refs(new uint(1))
			,w(new uint), h(new uint), bit_depth(new int), color_type(new int), gamma(new double){
		
		if(!rebuild()){
			Log::info()<<"Failed reading PNG."<<std::endl;
			shiva::core::exit(1);
		}
	}
	PNGImage(const PNGImage &rhs) :  refs(rhs.refs), image_data(rhs.image_data), display_exponent(rhs.display_exponent)
		, w(rhs.w), h(rhs.h), bit_depth(rhs.bit_depth), color_type(rhs.color_type), gamma(rhs.gamma), path(rhs.path) {
		(*refs)++;

	}
	~PNGImage(){
		if(--*refs == 0){
			delete [] *image_data;
			delete image_data;
			delete display_exponent;
			delete refs;
			delete w, h, bit_depth, color_type, gamma;
		}
	}
};