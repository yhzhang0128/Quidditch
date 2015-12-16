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
		std::vector<Vertex3D> v;	// ������
		std::vector<Vertex3D> vt;	// ��Ӧ��������
		std::vector<Vertex3D> vn;	// ����
		Vertex3D maxAxis, minAxis;

		// ��������Ƭ
		struct Face3D{
			int v[3];		// �������������v�е�index
			int vt[3];      // �����������������vt�е�index
			int vn[4];      // ������ķ���������vn�е�index
		};

		struct Object3D{
			char name[128];				// һ��ģ��������Object���
			std::vector<Face3D> face;	// ��

			Object3D()	{}
		};
		// ������ɲ��ֻ���
		std::vector<Object3D> objects;
		int modelType;	// ��Ӧ������ĺ궨��

	public:
		int parseFile(const char *filename, int type){
			modelType = type;

			// ��ģ���ļ�
			FILE *infile = NULL;
			fopen_s(&infile, filename, "r");
			if (infile == NULL){
				printf("Model File Open Fail");
				return -1;
			}

			// .obj �ļ�ÿ�ж���������һ��256��buffer
			char buf[256];
			double x, y, z;
			int nObjects = 0;
			Vertex3D min, max;
			min.x = min.y = min.z = MAX_AXIS_VALUE;
			max.x = max.y = max.z = MIN_AXIS_VALUE;

			// ���ж�ȡ����
			while (!feof(infile)){
				fgets(buf, 256, infile);
				switch (buf[0]){
				case 'g':
					// һ��Object
					objects.push_back(Object3D());
					sprintf_s(objects[nObjects].name, "Object%d", nObjects);
					nObjects++;
					break;
				case 'v':
					if (buf[1] == 't') {
						// ����
						sscanf_s(buf, "vt %lf %lf %lf", &x, &y, &z);

						// ��ֹ����������곬��1
						x = (x > 1.0 ? 0.9999 : x);
						y = (y > 1.0 ? 0.9999 : y);

						vt.push_back(Vertex3D(x, y, z));
					}
					else if (buf[1] == 'n'){
						// ����
						sscanf_s(buf, "vn %lf %lf %lf", &x, &y, &z);
						vn.push_back(Vertex3D(x, y, z));
					}
					else{
						// ����
						sscanf_s(buf, "v %lf %lf %lf", &x, &y, &z);
						v.push_back(Vertex3D(x, y, z));
						if (min.x > x) min.x = x; if (min.y > y) min.y = y; if (min.z > z) min.z = z;
						if (max.x < x) max.x = x; if (max.y < y) max.y = y; if (max.z < z) max.z = z;
					}
					break;
				case 'f':
					// ��������Ƭ
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
			// ���һ��ģ�͵�ԭʼ���귶Χ
			printf("Model:\nmax: %lf, %lf, %lf\nmin: %lf, %lf, %lf\n", maxAxis.x, maxAxis.y, maxAxis.z, minAxis.x, minAxis.y, minAxis.z);
			printf("Vertex number: %d\n", v.size());
			fclose(infile);
		}

		void Render(Vertex3D center, double scale){
			// center������λ��ת����scale�������ߴ�ת��
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


					// ��һ�������������
					idx = objects[i].face[j].vt[0] - 1;
					if (i == 1)
						glTexCoord2f(Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeW * vt[idx].x,
						Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeH * vt[idx].y);
					else
						glTexCoord2f(Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeW * vt[idx].x,
						Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeH * vt[idx].y);
					// ��һ�������������תΪ��Ⱦ����
					idx = objects[i].face[j].v[0] - 1;
					glVertex3d((v[idx].x + delta.x) * scale, (v[idx].y + delta.y) * scale, (v[idx].z + delta.z) * scale);

					// �ڶ��������������
					idx = objects[i].face[j].vt[1] - 1;
					if (i == 1)
						glTexCoord2f(Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeW * vt[idx].x,
						Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeH * vt[idx].y);
					else
						glTexCoord2f(Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeW * vt[idx].x,
						Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeH * vt[idx].y);
					// �ڶ��������������תΪ��Ⱦ����
					idx = objects[i].face[j].v[1] - 1;
					glVertex3d((v[idx].x + delta.x) * scale, (v[idx].y + delta.y) * scale, (v[idx].z + delta.z) * scale);

					// �����������������
					idx = objects[i].face[j].vt[2] - 1;
					if (i == 1)
						glTexCoord2f(Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeW * vt[idx].x,
						Texture::smallPixmaps[PIXMAP_DESK_BORDER].relativeH * vt[idx].y);
					else
						glTexCoord2f(Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeW * vt[idx].x,
						Texture::bigPixmaps[PIXMAP_DESK_MIDDLE].relativeH * vt[idx].y);
					// �����������������תΪ��Ⱦ����
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
