#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_

#pragma comment(lib,"jpeg.lib")

#include <malloc.h>
#include <setjmp.h>
#include <cstdio>
#include <jpeglib.h>
#include <GL\glut.h>

namespace ImageLoader{
	// 大尺寸纹理贴图
	const int BIG_HEIGHT = 1024;
	const int BIG_WIDTH = 1024;
	struct BigPixmap{
		int width, height;
		double relativeW, relativeH;
		GLubyte pixmap[BIG_HEIGHT][BIG_WIDTH][4];
	};

	// 小尺寸纹理贴图
	const int SMALL_HEIGHT = 512;
	const int SMALL_WIDTH = 512;
	struct SmallPixmap{
		int width, height;
		double relativeW, relativeH;
		GLubyte pixmap[SMALL_HEIGHT][SMALL_WIDTH][4];
	};

	// 此处定义了一些全局变量，所以是非Thread Safe
	jpeg_decompress_struct cinfo;
	struct my_error_mgr {
		struct jpeg_error_mgr pub;    /* "public" fields */
		jmp_buf setjmp_buffer;    /* for return to caller */
	} jerr;
	typedef struct my_error_mgr * my_error_ptr;

	METHODDEF(void) my_error_exit(j_common_ptr cinfo)
	{
		my_error_ptr myerr = (my_error_ptr)cinfo->err;
		(*cinfo->err->output_message) (cinfo);
		longjmp(myerr->setjmp_buffer, 1);
	}

	// 读取大纹理素材
	int bigJpgLoader(const char *filename, BigPixmap *out){
		FILE *infile = NULL;
		fopen_s(&infile, filename, "rb");
		if (infile == NULL){
			printf("JPG Open Error!\n");
			return -1;
		}

		// 抛异常处理
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return -1;
		}

		// 创建解码对象
		jpeg_create_decompress(&cinfo);

		// 初始化源数据
		jpeg_stdio_src(&cinfo, infile);

		// 读取JPG头信息
		jpeg_read_header(&cinfo, TRUE);

		// 开始解码
		jpeg_start_decompress(&cinfo);

		// 读取解码数据
		out->width = cinfo.output_width;
		out->height = cinfo.output_height;
		out->relativeW = (double)cinfo.output_width / BIG_WIDTH;
		out->relativeH = (double)cinfo.output_height / BIG_HEIGHT;

		printf("\nJPG Size: %d %d\n", cinfo.output_width, cinfo.output_height);
		printf("\nJPG Relative: %lf %lf\n", out->relativeW, out->relativeH);

		unsigned char* buffer = (unsigned char*)malloc(cinfo.output_width * cinfo.output_components);

		int height = 0;
		while (cinfo.output_scanline < cinfo.output_height){
			jpeg_read_scanlines(&cinfo, &buffer, 1);
			for (int i = 0; i < cinfo.output_width; i++) {
				out->pixmap[height][i][0] = buffer[i * 3];
				out->pixmap[height][i][1] = buffer[i * 3 + 1];
				out->pixmap[height][i][2] = buffer[i * 3 + 2];
				out->pixmap[height][i][3] = 255;
			}
			height++;
		}

		free(buffer);

		// 结束解码
		jpeg_finish_decompress(&cinfo);

		// 删除解码对象
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
	}

	// 读取小纹理素材
	int smallJpgLoader(const char *filename, SmallPixmap *out){
		FILE *infile = NULL;
		fopen_s(&infile, filename, "rb");
		if (infile == NULL){
			printf("JPG Open Error!\n");
			return -1;
		}

		// 抛异常处理
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return -1;
		}

		// 创建解码对象
		jpeg_create_decompress(&cinfo);

		// 初始化源数据
		jpeg_stdio_src(&cinfo, infile);

		// 读取JPG头信息
		jpeg_read_header(&cinfo, TRUE);

		// 开始解码
		jpeg_start_decompress(&cinfo);

		// 读取解码数据
		out->width = cinfo.output_width;
		out->height = cinfo.output_height;
		out->relativeW = (double)cinfo.output_width / SMALL_WIDTH;
		out->relativeH = (double)cinfo.output_height / SMALL_HEIGHT;

		printf("\nJPG Size: %d %d\n", cinfo.output_width, cinfo.output_height);
		printf("\nJPG Relative: %lf %lf\n", out->relativeW, out->relativeH);

		unsigned char* buffer = (unsigned char*)malloc(cinfo.output_width * cinfo.output_components);

		int height = 0;
		while (cinfo.output_scanline < cinfo.output_height){
			jpeg_read_scanlines(&cinfo, &buffer, 1);
			for (int i = 0; i < cinfo.output_width; i++) {
				out->pixmap[height][i][0] = buffer[i * 3];
				out->pixmap[height][i][1] = buffer[i * 3 + 1];
				out->pixmap[height][i][2] = buffer[i * 3 + 2];
				out->pixmap[height][i][3] = 255;
			}
			height++;
		}

		free(buffer);

		// 结束解码
		jpeg_finish_decompress(&cinfo);

		// 删除解码对象
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
	}
}

#endif