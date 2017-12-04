#include <stdio.h>
#include <math.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#define PI 3.14159265
#define MAX_AIRPORT_COORDS_BUFFER 20000
static GLFWwindow* window;
static float deltaTime = 1.0;

int numAirportCoords=MAX_AIRPORT_COORDS_BUFFER;
float airportCoords[MAX_AIRPORT_COORDS_BUFFER];

double getXFromLatLong(double r, double latitude, double longitude)
{
	return r*cos(latitude)*cos(longitude);
}
double getYFromLatLong(double r, double latitude, double longitude)
{
	return r*cos(latitude)*sin(longitude);
}
double getZFromLatLong(double r, double latitude, double longitude)
{
	return r*sin(latitude);
}
void rotate(float angle, int x, int y, int z)
{
	glRotatef(angle,x,y,z);
}

//Draws lat+long lines at 0,0. Also draws in pink the prime meridian and equator
void drawEarth()
{
	double t =0;
	double b =0;
	double p=PI;
	double numVerticesLat=90/2;
	double numVerticesLong=90/2;

	glBegin(GL_LINES);
	for(t=-p/2;t<p/2;t+=(p/2)/numVerticesLat)
	{
		for(b=-p;b<p;b+=(p)/numVerticesLong)
		{
			double r=300;
			double t2=t+(p/2)/numVerticesLat;
			double b2=b+(p)/numVerticesLong;

			double x=getXFromLatLong(r,t,b);//The coordinates for the point of the sphere
			double y=getYFromLatLong(r,t,b);
			double z=getZFromLatLong(r,t,b);

			double x2=getXFromLatLong(r,t2,b);//The next coordinates in latitude
			double y2=getYFromLatLong(r,t2,b);
			double z2=getZFromLatLong(r,t2,b);

			double x3=getXFromLatLong(r,t,b2);//The next coordinates in longitude
			double y3=getYFromLatLong(r,t,b2);
			double z3=getZFromLatLong(r,t,b2);

			glColor4f(0.1f,0.7f,0.9f,0.1);
			if(fabs(b)<(p/numVerticesLong)/2|b==-p)
				glColor4f(1,0.7f,0.9f,0.1);

			//Longitude lines
			glVertex3f(x,y,z);
			glVertex3f(x2,y2,z2);

			glColor4f(0.1f,0.7f,0.9f,0.1);
			if(fabs(t)<((p/2)/numVerticesLong)/2|t==-p/2)
				glColor4f(1,0.7f,0.9f,0.1);
			//Latitude Lines
			glVertex3f(x,y,z);
			glVertex3f(x3,y3,z3);
		}
	}
	glEnd();
}
void update(float deltaTime)
{
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glTranslatef(400,300,0);
	rotate(deltaTime*10,0,0,1);
	drawEarth();
	int a=0;
	glPointSize(4);
	glBegin(GL_POINTS);
	for(a=0;a<numAirportCoords;a+=2)
	{
		double latitude = airportCoords[a]*(PI/180);
		double longitude = airportCoords[a+1]*(PI/180);
		double r=300;
		double x=getXFromLatLong(r,latitude,longitude);
		double y=getYFromLatLong(r,latitude,longitude);
		double z=getZFromLatLong(r,latitude,longitude);
		glColor4f(0,1,0,1);
		glVertex3f(x,y,z);
	}
	glEnd();
	glTranslatef(-400,-300,0);
}
//A parser for the open flights data loading in airport locations into airportCoords[],
//And storing the number of airport lat/long coordinates in numAirportCoords
//As such, numAirportCoords should be numAirportCoords*2
void airportParser(char* filelocation)
{
	FILE* filePointer;
    char* line = NULL;
    ssize_t len = 0;
    ssize_t read;
    filePointer = fopen(filelocation,"r");
    if(filePointer==NULL)
    {
		printf("Could not open file!");
	}else
	{
		int numEntries=0;
		int currEntry=0;
		while((read = getline(&line, &len, filePointer))!=-1)
		{
			numEntries++;
		}
		rewind(filePointer);
		float coords[numEntries*2];
		while((read = getline(&line, &len, filePointer))!=-1)
		{
			char *ptoken;
			ptoken = strtok (line,",\n");
			int counter=0;
			while (ptoken != NULL) {
				counter++;
				if((counter==7)|(counter==8))
				{
					double v = atof(ptoken);
					coords[currEntry++]=v;
				}
				ptoken = strtok (NULL, ",\n");
			}
		}
		fclose(filePointer);
		if(line)
		{
			free(line);
		}

		int a;
		for(a=0;a<numAirportCoords;a+=1)
		{
			printf("Clearing[%d]\n",a);
			airportCoords[a]=0;
		}
		numAirportCoords=numEntries*2;

		for(a=0;a<numAirportCoords;a+=1)
		{
			printf("Copying[%d]/[%d]\n",a,numAirportCoords);
			airportCoords[a]=coords[a];
		}
		printf("Done copying...\n");
	}
}
int main(int argc, char** argv)
{
	double lastTime = 0;
	double nowTime = 0;
	if(argc<2)
	{
		printf("Needs the location of the openflights dat file\n");
		return 1;
	}
	if(!glfwInit())
    {
		printf("Could not init glfw.\n");
        return 1;
    }
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    window = glfwCreateWindow(800,600,"Aircraft",NULL,NULL);
    if(window==NULL)
    {
		printf("Could not create window.\n");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double near=0;//This is a dirty trick such that only the front of the sphere is rendered.
					//The sphere is drawn in full with both back and front, but
					//Then the camera itself cuts the back away because it's too far away.
					//Mess around with these values to see what I mean.
	double far=1000;
	glOrtho(0,800,600,0,near,far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    printf("Loading...\n");
	airportParser(argv[1]);
	printf("Loaded!\n");

	glTranslatef(400,300,0);
	rotate(90,1,0,0);
	rotate(180,0,0,1);
	rotate(-15,0,1,0);
	glTranslatef(-400,-300,0);
    while(!glfwWindowShouldClose(window))
    {
		lastTime = glfwGetTime();
		(update)(deltaTime);
		glfwSwapBuffers(window);
		glfwPollEvents();

		nowTime=glfwGetTime();
		deltaTime = (float)(nowTime - lastTime);
    }

	glfwDestroyWindow(window);
	glfwTerminate();

    return 0;
}
