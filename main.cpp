#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<GL/freeglut.h>
#include<GL/gl.h>

#define SOURCE_FILE "input.txt"

using namespace std;

struct Vertex{
	float x, y, z;
};

vector<string> lines;
vector<Vertex> vertices;

float zoom = 1;
float rotateX = 0;
float rotateY = 0;

float currentX = 0;
float currentY = 0;

bool valid;

void renderFunction();
void ReadFile();
void GetVertices();
void reshape(int width, int height);
void renderPrimitive();
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("B170226CS");

	glutDisplayFunc(renderFunction);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseMotionFunc);

	ReadFile();
	GetVertices();

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	glutMainLoop();
	return 0;
}

void renderFunction()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -100);
	glScalef(zoom, zoom, zoom);
	glRotatef(rotateX, 1, 0, 0);
	glRotatef(rotateY, 0, 1, 0);

	renderPrimitive();

	glutSwapBuffers();
}

void reshape (int width, int height) 
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	glOrtho(-width, width, -height, height, 5000, -5000); // might need to invert height  
	glMatrixMode(GL_MODELVIEW);
}  

void renderPrimitive()
{

	glBegin(GL_TRIANGLES);
		for(int i = 0; i < vertices.size(); ++i)
		{
			glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
		}	
	glEnd();
}

void mouseFunc(int button, int state, int x, int y)
{
	valid = (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
	if(button == 3)
	{
		zoom += 0.1;
	}
	else if(button == 4 && zoom - 0.1 > 0)
	{
		zoom -= 0.1;
	}
	else if(valid)
	{
		currentX = x;
		currentY = y;
	}

	glutPostRedisplay();
}

void mouseMotionFunc(int x, int y)
{
	if(valid)
	{
		rotateY += (x - currentX)/10;
		rotateX += (y - currentY)/10;

		currentX = x;
		currentY = y;

		glutPostRedisplay();
	}
}

void ReadFile()
{
	string line;
	ifstream file;
	file.open(SOURCE_FILE);

	while(getline(file, line))
		lines.push_back(line);

	file.close();
}

void GetVertices()
{
	Vertex v = Vertex();

	for(int i = 0; i < lines.size(); ++i)
	{

		stringstream ss(lines[i]);
		string first_word;

		ss >> first_word;

		if(first_word.compare("endsolid") == 0)
			break;
		else if(first_word.compare("vertex") != 0)
			continue;
		else
		{	
			ss >> v.x;
			ss >> v.y;
			ss >> v.z;
			vertices.push_back(v);
		}
	}
}