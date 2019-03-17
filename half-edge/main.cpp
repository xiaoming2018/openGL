#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <gl/glew.h>
#include <sstream>
#include <map>
#include <utility>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tools.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>


using namespace std;

//gl prototypes
void init(void);
void reshape(void);
void display(void);

void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

void set_face_normals();
void set_vertex_normals();
void draw_faces();

// gl globals
GLUquadricObj *quadratic;

size_t mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;
static const float gridSize = 100.0;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;

Mesh* mesh_data = new Mesh;
vector<glm::vec3> vertices;
vector<glm::vec3> vertex_nomals;
vector<glm::vec3> face_normals;
vector<float> face_areas;
half_edge::half_edge_t mesh;

vector<string> &split(const string &str, char delim, vector<string> &tokens)
{
	stringstream ss(str);
	string token;

	while (getline(ss, token, delim))
		if (!token.empty())
			tokens.push_back(token);

	return tokens;
}

/* This function splits a given string into tokens based on the specified
 * delimiter and returns a new vector with all the tokens.
 */
vector<string> split(const string &str, char delim)
{
	vector<string> tokens;
	split(str, delim, tokens);
	return tokens;
}

/* String to integer conversion.
 */
float stoi_(const string str)
{
	return atoi(str.c_str());
}

/* String to float conversion.
 */
float stof_(const string str)
{
	return atof(str.c_str());
}

/* This function fills a passed-in mesh data container with a list of vertices
 * and faces specified by the given .obj file.
 */
void parse(istream &data) {
	mesh_data->points = new vector<point*>;
	mesh_data->faces= new vector<face*>;

	string read_line;
	while (getline(data,read_line)) {
		vector<string> tokens = split(read_line, ' ');
		if (tokens[0].at(0) == 'v') {
			point *p = new point;

			p->x = stof_(tokens[1]);
			p->y = stof_(tokens[2]);
			p->z = stof_(tokens[3]);
			mesh_data->points->push_back(p);
		}
		else if (tokens[0].at(0) == 'f') {
			face *f = new face;

			f->ind1 = stoi_(tokens[1]) - 1;
			f->ind2 = stoi_(tokens[2]) - 1;
			f->ind3 = stoi_(tokens[3]) - 1;

			mesh_data->faces->push_back(f);
		}
	}
}

void set_vertex_normals() {
	int num_vertices = mesh_data->points->size();
	vector<half_edge::index_t> neighs;
	for (int vi = 0; vi < neighs.size();vi++) {
		mesh.vertex_face_neighbors(vi, neighs);
		glm::vec3 vertex_normal(0.0, 0.0, 0.0);

		if (!mesh.vertex_is_boundary(vi)) {
			for (int i = 0; i < neighs.size(); i++)
			{
				vertex_normal += face_areas[neighs.at(i)] * face_normals[neighs.at(i)];
			}
			vertex_normal = glm::normalize(vertex_normal);
		}
		vertex_nomals.push_back(vertex_normal);
	}
}

//void reshape(int width, int height)
//{
//	height = (height == 0) ? 1 : height;
//	width = (width == 0) ? 1 : width;
//	glViewport(0, 0, width, height);
//	mouse_scale_x = (float)(right_param - left_param) / (float)width;
//	mouse_scale_y = (float)(top_param - bottom_param) / (float)height;
//	glutPostRedisplay();
//}
//
//void display(void)
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
//	glRotatef(-cam_orientation_angle,
//		cam_orientation_axis[0], cam_orientation_axis[1],
//		cam_orientation_axis[2]);
//	glTranslatef(-cam_position[0], -cam_position[1], -cam_position[2]);
//	set_lights();
//	glEnable(GL_TEXTURE_2D);
//	draw_objects();
//	glutSwapBuffers();
//}

void set_face_normals()
{
	vector<point *> * points = mesh_data->points;
	vector<face *> * faces = mesh_data->faces;

	for (int i = 0; i < faces->size(); i++)
	{
		face * curr = (*faces)[i];
		int ia = curr->ind1;
		int ib = curr->ind2;
		int ic = curr->ind3;

		glm::vec3 vert1, vert2, vert3;
		point2vector((*points)[ia], vert1);
		point2vector((*points)[ib], vert2);
		point2vector((*points)[ic], vert3);

		glm::vec3 normal;
		normal = glm::normalize(glm::cross(vert2 - vert1, \
			vert3 - vert1));
		face_normals.push_back(normal);

		float area = 0;
		area += glm::length(glm::cross(vert2 - vert1, vert3 - vert1));
		face_areas.push_back(area / 2);
	}
}
//
//void draw_faces()
//{
//	vector<face*> * faces = mesh_data->faces;
//	vector<point*> * vertices = mesh_data->points;
//	for (int f = 0; f < faces->size(); f++)
//	{
//		face * curr = (*faces)[f];
//		glBegin(GL_TRIANGLES);
//		glTexCoord2d(0.4, 0.4);
//		glNormal3f(vertex_normals[curr->ind1].x, \
//			vertex_normals[curr->ind1].y, vertex_normals[curr->ind1].z);
//		glVertex3f((*vertices)[curr->ind1]->x, \
//			(*vertices)[curr->ind1]->y, (*vertices)[curr->ind1]->z);
//
//		glTexCoord2d(0.4, 0.6);
//		glNormal3f(vertex_normals[curr->ind2].x, \
//			vertex_normals[curr->ind2].y, vertex_normals[curr->ind2].z);
//		glVertex3f((*vertices)[curr->ind2]->x, \
//			(*vertices)[curr->ind2]->y, (*vertices)[curr->ind2]->z);
//
//		glTexCoord2d(0.6, 0.6);
//		glNormal3f(vertex_normals[curr->ind3].x, \
//			vertex_normals[curr->ind3].y, vertex_normals[curr->ind3].z);
//		glVertex3f((*vertices)[curr->ind3]->x, \
//			(*vertices)[curr->ind3]->y, (*vertices)[curr->ind3]->z);
//
//		glEnd();
//
//	}
//}

//void draw_objects()
//{
//	float colrs1[3]; float colrs2[3]; float colrs3[3];
//	colrs1[0] = .5; colrs1[1] = .5; colrs1[2] = .5;
//	glMaterialfv(GL_FRONT, GL_AMBIENT, colrs1);
//
//	colrs2[0] = .9; colrs2[1] = .9; colrs2[2] = .9;
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, colrs2);
//
//	colrs3[0] = .3; colrs3[1] = .3; colrs3[2] = .3;
//	glMaterialfv(GL_FRONT, GL_SPECULAR, colrs3);
//
//	glMaterialf(GL_FRONT, GL_SHININESS, 8);
//
//	glPushMatrix();
//
//	glRotatef(x_view_angle, 0, 1, 0);
//	glRotatef(y_view_angle, 1, 0, 0);
//
//	draw_faces();
//
//	glPopMatrix();
//}

void build_half_edge()
{
	set_face_normals();
	vector<point*> * points = mesh_data->points;
	vector<face*> * faces = mesh_data->faces;
	int num_points = points->size();
	vector<half_edge::edge_t> edges;
	half_edge::faces_to_edges(faces, edges);
	mesh.build_he_structure(num_points, faces, edges.size(), &edges[0]);
	set_vertex_normals();
}


int main(int argc, char ** argv)
{
	if (argc != 2)
	{
		cout << "Invalid number of arguments!" << endl;
		cout << "Usage: " << argv[0] << \
			" [texture.bmp] < [example.obj]" << endl;
		return 1;
	}


	printf("Parsing...\n");
	parse(cin);
	printf("Finished parsing.\n");

	build_half_edge();

	// use opengl stuff
	int xres = 500;
	int yres = 500;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(xres, yres);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CS171 Homework 5");
	init();

	glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	glutMouseFunc(mouse_pressed);
	glutMotionFunc(mouse_moved);
	glutKeyboardFunc(key_pressed);
	glutMainLoop();
	return 0;
}