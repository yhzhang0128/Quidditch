#ifndef _IMAGE_LOADER_H_
#define _IMAGE_LOADER_H_

#pragma comment(lib,"jpeg.lib")

#include <malloc.h>
#include <setjmp.h>
#include <cstdio>
#include <jpeglib.h>
#include <GL\glut.h>

namespace ImageLoader{
	// ��ߴ�������ͼ
	const int BIG_HEIGHT = 1024;
	const int BIG_WIDTH = 1024;
	struct BigPixmap{
		int width, height;
		double relativeW, relativeH;
		GLubyte pixmap[BIG_HEIGHT][BIG_WIDTH][4];
	};

	// С�ߴ�������ͼ
	const int SMALL_HEIGHT = 512;
	const int SMALL_WIDTH = 512;
	struct SmallPixmap{
		int width, height;
		double relativeW, relativeH;
		GLubyte pixmap[SMALL_HEIGHT][SMALL_WIDTH][4];
	};

	// �˴�������һЩȫ�ֱ����������Ƿ�Thread Safe
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

	// ��ȡ�������ز�
	int bigJpgLoader(const char *filename, BigPixmap *out){
		FILE *infile = NULL;
		fopen_s(&infile, filename, "rb");
		if (infile == NULL){
			printf("JPG Open Error!\n");
			return -1;
		}

		// ���쳣����
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return -1;
		}

		// �����������
		jpeg_create_decompress(&cinfo);

		// ��ʼ��Դ����
		jpeg_stdio_src(&cinfo, infile);

		// ��ȡJPGͷ��Ϣ
		jpeg_read_header(&cinfo, TRUE);

		// ��ʼ����
		jpeg_start_decompress(&cinfo);

		// ��ȡ��������
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

		// ��������
		jpeg_finish_decompress(&cinfo);

		// ɾ���������
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
	}

	// ��ȡС�����ز�
	int smallJpgLoader(const char *filename, SmallPixmap *out){
		FILE *infile = NULL;
		fopen_s(&infile, filename, "rb");
		if (infile == NULL){
			printf("JPG Open Error!\n");
			return -1;
		}

		// ���쳣����
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) {
			jpeg_destroy_decompress(&cinfo);
			fclose(infile);
			return -1;
		}

		// �����������
		jpeg_create_decompress(&cinfo);

		// ��ʼ��Դ����
		jpeg_stdio_src(&cinfo, infile);

		// ��ȡJPGͷ��Ϣ
		jpeg_read_header(&cinfo, TRUE);

		// ��ʼ����
		jpeg_start_decompress(&cinfo);

		// ��ȡ��������
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

		// ��������
		jpeg_finish_decompress(&cinfo);

		// ɾ���������
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
	}
}

#endif