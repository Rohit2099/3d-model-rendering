#include <fstream>
#include <string>
#include <iostream>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <vector>
#include <sstream>
#include <math.h>

using namespace std;

class Vertex{
	public:
		float x, y, z;
};

class triangle{
	public:
		Vertex v1, v2, v3;
};

// GLOBAL VARIABLES
string FILENAME = "humanoid.txt";				// Input file name
string WHITESPACE = " \n\r\t\f\v";	

GLint WINWIDTH = 800, WINHEIGHT = 600;		// Window height and width

float xmax = 0, ymax = 0, zmax = 0;			// maximum x,y,z coordinates of the figure
float phi = 0.0, theta = 0.0;				// angles of rotation
float zoom = 1.0;							// scale factor

int downX = 0, downY = 0;
bool leftButton = false;

vector<triangle> triangles;					// Contains all the points stored as triangles




/*
An auxillary function to left trim a string and return the trimmed string. 
It is called from parse_string_vertex() and is used in processing the input file.
*/
string ltrim(const string& s){
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}




/*
An auxillary function used in processing the input file.
The function parses a line from the input file and returns the 
vertex contained in the line. It is called from STLImport(). 
*/
Vertex parse_string_vertex(string str, int size){
    string word = "";
	str = ltrim(str);
	int word_count = 0;
	Vertex vertex;
	string contents[size];

    for (char i = 0; i < str.size(); ++i){
		if(str[i] == ' ' && str[i + 1] == ' '){
			continue;
		}
        if (str[i] == ' '){
			contents[word_count] = word;
			word_count++;
            word = "";
        }
        else{
            word = word + str[i];
        }
    }	
	contents[word_count] = word;
	vertex.x = stof(contents[size - 3]);
	vertex.y = stof(contents[size - 2]);
	vertex.z = stof(contents[size - 1]);
	if(vertex.x > xmax){
		xmax = vertex.x;
	}
	if(vertex.y > ymax){
		ymax = vertex.y;
	}	
	if(vertex.z > zmax){
		zmax = vertex.z;
	}		
	return vertex;	
}




/*
The main function used to read the contents of the input file.
STLImport() passes every line from the file to parse_string_vertex()
and stores the vertices in the 'triangles' data structure.
The normal vectors are not read as it does not serve any purpose in
the problem to be solved.
*/
void STLImport(const string & fileName) {
	ifstream ifs(fileName, ios::in);
    string str;
    vector<string> file_contents;				// Holds the contents of the file
	vector<string>::iterator it;

    while(getline(ifs, str)){
        file_contents.push_back(str);
    } 

	ifs.close();

    file_contents.pop_back();
	it = file_contents.begin();
	file_contents.erase(it);

    long long line_count = file_contents.size(), triangle_num = 0;
	Vertex v1, v2, v3, normal;
	triangles.resize(line_count / 7);

	while(triangle_num < line_count / 7){
		long long idx = 7 * triangle_num;

		v1 = parse_string_vertex(file_contents[idx + 2], 4);
		v2 = parse_string_vertex(file_contents[idx + 3], 4);
		v3 = parse_string_vertex(file_contents[idx + 4], 4);

		triangles[triangle_num].v1 = v1;
		triangles[triangle_num].v2 = v2;
		triangles[triangle_num].v3 = v3;
		triangle_num++;
	}
}




/*
This is the callback function for glutDisplayFunc().
Every time the function is called, the color and depth buffers
are cleared, the model matrix is reset and the required geometric
transformations are done and the figure is drawn.
*/
void drawFigure(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	
	glScalef(zoom, zoom, zoom);					// Scales the figure by 'zoom' factor
  	glRotatef(theta, 1.0, 0.0, 0.0);			// Rotates the plane by 'theta' angle
  	glRotatef(phi, 0.0, 1.0, 0.0);		

    glBegin(GL_TRIANGLES);
        for(int i = 0; i < triangles.size(); ++i){
            glVertex3f(triangles[i].v1.x, triangles[i].v1.y, triangles[i].v1.z);
            glVertex3f(triangles[i].v2.x, triangles[i].v2.y, triangles[i].v2.z);
            glVertex3f(triangles[i].v3.x, triangles[i].v3.y, triangles[i].v3.z);		
        }
    glEnd();
    glutSwapBuffers();
}





/*
This is the callback function for glutMouseFunc().
It checks for left mouse button drag action and
scroll action. They are used to determine if the plane
needs to be scaled or rotated.
*/
void mouse(int button, int state, int x, int y){
	leftButton = (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
	downX = x; downY = y;

    if (state == GLUT_UP && button == 3){
        zoom += 0.15;
		glutPostRedisplay();
    }
 
    else if (state == GLUT_UP && button == 4){
		if (zoom > 0.15){
        	zoom -= 0.15;
		}
		glutPostRedisplay();
	}
}




/*
This is the callback function for glutMotionFunc().
Once a left mouse button click is detected, this function
checks for the distance the mouse has moved to determine
how much the plane should be rotated.
*/
void mouseMotion(int x, int y){
	if(leftButton){
		phi += (x - downX) / 10.0;
		theta += (y - downY) / 10.0;

		glutPostRedisplay();
		downX = x;   
		downY = y;		
	}
}




/*
This is the callback function for glutReshapeFunc().
This function is called whenever the window is resized.
In this case, the function also serves as an initialize function
where all the initial parameters such as line color, bg color are set.
*/
void reshape(GLsizei width, GLsizei height){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			// Draw line triangles
	glDisable(GL_CULL_FACE);	
	glClearColor(1.0, 1.0, 1.0, 0.0);					// BG color
	glColor3ub(255, 0, 255);							// Brush color

	glViewport(0, 0, width, height);
	WINWIDTH = width;
	WINHEIGHT = height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Setting appropriate values for glOrtho() so that 
	// any rendered figure with any coordinate values
	// is always inside the view volume.
	int maxval = max(max(abs(xmax), abs(ymax)), abs(zmax));
	glOrtho(-(abs(xmax) + 20), abs(xmax) + 20, -(abs(ymax) + 20), abs(ymax) + 20, -(maxval + 20), maxval + 20);	
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitWindowSize(WINWIDTH, WINHEIGHT);
    glutCreateWindow("B170998CS"); 
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);

	STLImport(FILENAME);	
    glutDisplayFunc(drawFigure); 
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);	

    glutMainLoop();
    return 0;
}