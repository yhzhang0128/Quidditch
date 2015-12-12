#ifndef _TEXTURE_H_	
#define _TEXTURE_H_

#include <GL\glut.h>
#include "ImageLoader.hpp"

namespace Texture{
#define TEX_DESK_MIDDLE 0
#define TEX_DESK_BORDER 1
#define TEX_DESK_DECORATE 2
#define TEX_BALL_MOTHER 3
#define TEX_BALL_ENEMY 4
#define TEX_BALL_TARGET 5
#define TEX_BALL_MAGIC 6
#define TEX_FLOOR 7
#define TEX_WALL 8

#define TEX_BANNER0 9
#define TEX_BANNER1 10

	// ������
#define PIXMAP_DESK_MIDDLE 0	
#define PIXMAP_WALL 1
	// С����
#define PIXMAP_DESK_BORDER 0	
#define PIXMAP_DESK_DECORATE 1
#define PIXMAP_BALL_MOTHER 2
#define PIXMAP_BALL_ENEMY 3
#define PIXMAP_BALL_TARGET 4
#define PIXMAP_BALL_MAGIC 5
#define PIXMAP_FLOOR 6

#define PIXMAP_BANNER0 7
#define PIXMAP_BANNER1 8

	GLuint textureNames[15];
	ImageLoader::BigPixmap  bigPixmaps[5];
	ImageLoader::SmallPixmap smallPixmaps[10];

	// ׼�����������ģ��
	void prepareDesk(){
		// ׼����������
		ImageLoader::bigJpgLoader("images/desk_middle.jpg", bigPixmaps + PIXMAP_DESK_MIDDLE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_DESK_MIDDLE]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_DESK_MIDDLE]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::BIG_WIDTH, ImageLoader::BIG_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			bigPixmaps[PIXMAP_DESK_MIDDLE].pixmap);

		// ׼�����ӱ�Ե����
		ImageLoader::smallJpgLoader("images/heavy.jpg", smallPixmaps + PIXMAP_DESK_BORDER);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_DESK_BORDER]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_DESK_BORDER]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_DESK_BORDER].pixmap);

		// ׼����ɫװ������
		ImageLoader::smallJpgLoader("images/desk_decorate.jpg", smallPixmaps + PIXMAP_DESK_DECORATE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_DESK_DECORATE]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_DESK_DECORATE]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_DESK_DECORATE].pixmap);
	}

	void prepareBall(){
		/***************** ׼��Mother Ball���� *****************/
		ImageLoader::smallJpgLoader("images/Snoopy.jpg", smallPixmaps + PIXMAP_BALL_MOTHER);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_BALL_MOTHER]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_BALL_MOTHER]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//��������
		//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����s����������Զ�����
		//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����t����������Զ�����

		//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_BALL_MOTHER].pixmap);

		/***************** ׼��Enemy Ball���� *****************/
		ImageLoader::smallJpgLoader("images/ball_enemy.jpg", smallPixmaps + PIXMAP_BALL_ENEMY);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_BALL_ENEMY]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_BALL_ENEMY]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//��������
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����s����������Զ�����
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����t����������Զ�����

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_BALL_ENEMY].pixmap);

		/***************** ׼��Target Ball���� *****************/
		ImageLoader::smallJpgLoader("images/ball_target.jpg", smallPixmaps + PIXMAP_BALL_TARGET);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_BALL_TARGET]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_BALL_TARGET]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//��������
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����s����������Զ�����
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����t����������Զ�����

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_BALL_TARGET].pixmap);

		/***************** ׼��Magic Ball���� *****************/
		ImageLoader::smallJpgLoader("images/ball_magic.jpg", smallPixmaps + PIXMAP_BALL_MAGIC);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_BALL_MAGIC]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_BALL_MAGIC]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//��������
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����s����������Զ�����
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����t����������Զ�����

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_BALL_MAGIC].pixmap);
	}

	void prepareRoom(){
		// ׼���ذ�����
		ImageLoader::smallJpgLoader("images/floor_small.jpg", smallPixmaps + PIXMAP_FLOOR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_FLOOR]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_FLOOR]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_FLOOR].pixmap);

		// ׼��ǽ������
		ImageLoader::bigJpgLoader("images/wall_big.jpg", bigPixmaps + PIXMAP_WALL);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_WALL]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_WALL]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::BIG_WIDTH, ImageLoader::BIG_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			bigPixmaps[PIXMAP_WALL].pixmap);
	}

	void prepareBanner(){
		ImageLoader::smallJpgLoader("images/banner0.jpg", smallPixmaps + PIXMAP_BANNER0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_BANNER0]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_BANNER0]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_BANNER0].pixmap);


		ImageLoader::smallJpgLoader("images/banner1.jpg", smallPixmaps + PIXMAP_BANNER1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureNames[TEX_BANNER1]);

		glBindTexture(GL_TEXTURE_2D, textureNames[TEX_BANNER1]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageLoader::SMALL_WIDTH, ImageLoader::SMALL_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			smallPixmaps[PIXMAP_BANNER1].pixmap);
	}
}

#endif