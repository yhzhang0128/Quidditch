#ifndef _MODEL3D_H_
#define _MODEL3D_H_

#include <cstdio>
#include <vector>
#include <sstream>
#include <GL\glut.h>
#include "Texture.hpp"
#include "Vertex3D.h"

#define MODEL_TYPE_DESK 0
#define MODEL_TYPE_BALL 1

namespace Model3D{

	const double MIN_AXIS_VALUE = -100000.0;
	const double MAX_AXIS_VALUE = 100000.0;

	class Model3D{
	private:
		std::vector<Vertex3D> v;	// 点坐标
		std::vector<Vertex3D> vt;	// 对应纹理坐标
		std::vector<Vertex3D> vn;	// 法线
		Vertex3D maxAxis, minAxis;

		// 三角形面片
		struct Face3D{
			int v[3];		// 三个点的坐标在v中的index
			int vt[3];      // 三个点的纹理坐标在vt中的index
			int vn[4];      // 三个点的法线坐标在vn中的index
		};

		struct Object3D{
			char name[128];				// 一个模型由若干Object组成
			std::vector<Face3D> face;	// 面

			Object3D()	{}
		};
		// 所有组成部分汇总
		std::vector<Object3D> objects;
		int modelType;	// 对应最上面的宏定义

	public:
		int parseFile(const char *filename, int type){
			modelType = type;

			// 打开模型文件
			FILE *infile = NULL;
			fopen_s(&infile, filename, "r");
			if (infile == NULL){
				printf("Model File Open Fail");
				return -1;
			}

			// .obj 文件每行都不长，开一个256的buffer
			char buf[256];
			double x, y, z;
			int nObjects = 0;
			Vertex3D min, max;
			min.x = min.y = min.z = MAX_AXIS_VALUE;
			max.x = max.y = max.z = MIN_AXIS_VALUE;

			// 逐行读取解析
			while (!feof(infile)){
				fgets(buf, 256, infile);
				switch (buf[0]){
				case 'g':
					// 一个Object
					objects.push_back(Object3D());
					sprintf_s(objects[nObjects].name, "Object%d", nObjects);
					nObjects++;
					break;
				case 'v':
					if (buf[1] == 't') {
						// 纹理
						sscanf_s(buf, "vt %lf %lf %lf", &x, &y, &z);

						// 禁止纹理相对坐标超过1
						x = (x > 1.0 ? 0.9999 : x);
						y = (y > 1.0 ? 0.9999 : y);

						vt.push_back(Vertex3D(x, y, z));
					}
					else if (buf[1] == 'n'){
						// 法线
						sscanf_s(buf, "vn %lf %lf %lf", &x, &y, &z);
						vn.push_back(Vertex3D(x, y, z));
					}
					else{
						// 顶点
						sscanf_s(buf, "v %lf %lf %lf", &x, &y, &z);
						v.push_back(Vertex3D(x, y, z));
						if (min.x > x) min.x = x; if (min.y > y) min.y = y; if (min.z > z) min.z = z;
						if (max.x < x) max.x = x; if (max.y < y) max.y = y; if (max.z < z) max.z = z;
					}
					break;
				case 'f':
					// 三角形面片
					Face3D *face = new Face3D();
					sscanf_s(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d",
						&(face->v[0]), &(face->vt[0]), &(face->vn[0]),
						&(face->v[1]), &(face->vt[1]), &(face->vn[1]),
						&(face->v[2]), &(face->vt[2]), &(face->vn[2]));
					objects[nObjects - 1].face.push_back(*face);
					break;
				}
			}

			minAxis.x = min.x; minAxis.y = min.y; minAxis.z = min.z;
			maxAxis.x = max.x; maxAxis.y = max.y; maxAxis.z = max.z;
			// 输出一个模型的原始坐标范围
			printf("Model:\nmax: %lf, %lf, %lf\nmin: %lf, %lf, %lf\n", maxAxis.x, maxAxis.y, maxAxis.z, minAxis.x, minAxis.y, minAxis.z);
			printf("Vertex number: %d\n", v.size());
			fclose(infile);
		}

		void Render(Vertex3D center, double scale){
			// center用来做位置转换，scale用来做尺寸转换
			Vertex3D modelCenter = (maxAxis + minAxis);
			modelCenter.x *= 0.5; modelCenter.y *= 0.5; modelCenter.z *= 0.5;
			Vertex3D delta = center - modelCenter;

			int nObjects = objects.size();

			int nowTexture;
			for (int i = 0; i < nObjects; i++){

				if (modelType == MODEL_TYPE_DESK){
					if (i > 1)
						nowTexture = TEX_DESK_DECORATE;
					else if (i == 1)
						nowTexture = TEX_DESK_BORDER;
					else
						nowTexture = TEX_DESK_MIDDLE;
				}
				else{
					nowTexture = TEX_BALL_MOTHER;
				}


				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_2D, Texture::textureNames[nowTexture]);

				int nFaces = objects[i].face.size();
				for (int j = 0, idx; j < nFaces; j++){
					glBegin(GL_TRIANGLES);


					// 第一个点的纹理坐标
					idx = objects[i].face[j].vt[0] - 1;
					if (i == 1)
						glTexCoord2f(Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeW * vt[idx].x,
						Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeH * vt[idx].y);
					else
						glTexCoord2f(Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeW * vt[idx].x,
						Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeH * vt[idx].y);
					// 第一个点的世界坐标转为渲染坐标
					idx = objects[i].face[j].v[0] - 1;
					glVertex3d((v[idx].x + delta.x) * scale, (v[idx].y + delta.y) * scale, (v[idx].z + delta.z) * scale);

					// 第二个点的纹理坐标
					idx = objects[i].face[j].vt[1] - 1;
					if (i == 1)
						glTexCoord2f(Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeW * vt[idx].x,
						Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeH * vt[idx].y);
					else
						glTexCoord2f(Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeW * vt[idx].x,
						Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeH * vt[idx].y);
					// 第二个点的世界坐标转为渲染坐标
					idx = objects[i].face[j].v[1] - 1;
					glVertex3d((v[idx].x + delta.x) * scale, (v[idx].y + delta.y) * scale, (v[idx].z + delta.z) * scale);

					// 第三个点的纹理坐标
					idx = objects[i].face[j].vt[2] - 1;
					if (i == 1)
						glTexCoord2f(Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeW * vt[idx].x,
						Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeH * vt[idx].y);
					else
						glTexCoord2f(Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeW * vt[idx].x,
						Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeH * vt[idx].y);
					// 第三个点的世界坐标转为渲染坐标
					idx = objects[i].face[j].v[2] - 1;
					glVertex3d((v[idx].x + delta.x) * scale, (v[idx].y + delta.y) * scale, (v[idx].z + delta.z) * scale);

					glEnd();

				}
				glDisable(GL_TEXTURE_2D);
			}
		}
	};

}
#endif
