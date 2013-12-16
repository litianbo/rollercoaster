// assign2.cpp : Defines the entry point for the console application.
//

/*
CSCI 480 Computer Graphics
Assignment 2: Simulating a Roller Coaster
C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <math.h>
/* represents one control point along the spline */
struct point {
	double x;
	double y;
	double z;
};

/* spline struct which contains how many control points, and an array of control points */
struct spline {
	int numControlPoints;
	struct point *points;
};

/* the spline array */
struct spline *g_Splines;

/* total number of splines */
int g_iNumOfSplines;

int width;
int height;
int g_iMenuId;
//variables to count screenshoot times.
int counter = 0,n = 0;
//screenshot name.
char buffer[50];
int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;
Pic * g_pGroundData;
Pic * g_pSkyData;
Pic * g_pRailData;
Pic * g_pSky2Data;
unsigned char texture[640][480][4];
/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};
//float points
std::vector<float> cPointsX;
std::vector<float> cPointsY;
std::vector<float> cPointsZ;
//tangent vectors
std::vector<float> tangentsX; 
std::vector<float> tangentsY; 
std::vector<float> tangentsZ; 
//normal vectors
std::vector<float> normalX; 
std::vector<float> normalY; 
std::vector<float> normalZ; 
//binormal vectors
std::vector<float> binormalX; 
std::vector<float> binormalY; 
std::vector<float> binormalZ; 
float x,y,z;//temp variables,needed in many blocks, so I set this up as global
bool renderTube = true;
bool level1 = true;
bool stop = false;
bool track = false;
bool tShape = false;
int focusFactor = 5;
int centralFactor = 5;
float splineFactor = 4.0;
//counter for camera;
int counterCamera = 0;
int frameCounter = 0;
int currentTime = 0;
int previousTime = 0;
int fps = 0;
void calculateFPS();
void drawFPS();
/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
	int i, j;
	Pic *in = NULL;

	if (filename == NULL)
		return;

	/* Allocate a picture buffer */
	in = pic_alloc(640, 480, 3, NULL);

	printf("File to save to: %s\n", filename);

	for (i=479; i>=0; i--) {
		glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
			&in->pix[i*in->nx*in->bpp]);
	}

	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);
}

void myinit()
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
}
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* clear buffer */
	//enable z-buffer here
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);



	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(60.0,(GLfloat)(640/480) , 0.01, 10000.0 ); 
	//call glulookat
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(!renderTube || track)
		gluLookAt((cPointsX[counterCamera] + 2*(normalX[counterCamera]+binormalX[counterCamera])+
		(cPointsX[counterCamera] + 2*(-normalX[counterCamera]+binormalX[counterCamera])))/2 + binormalX[counterCamera]*focusFactor,(cPointsY[counterCamera] + 
		2*(normalY[counterCamera]+binormalY[counterCamera])+
		(cPointsY[counterCamera] + 2*(-normalY[counterCamera]+binormalY[counterCamera])))/2 + binormalY[counterCamera]*focusFactor,(cPointsZ[counterCamera] + 
		2*(normalZ[counterCamera]+binormalZ[counterCamera])+
		(cPointsZ[counterCamera] + 2*(-normalZ[counterCamera]+binormalZ[counterCamera])))/2 + binormalZ[counterCamera]*focusFactor,
		cPointsX[counterCamera] + tangentsX[counterCamera]*20,
		cPointsY[counterCamera] + tangentsY[counterCamera]*20 ,
		cPointsZ[counterCamera]+ tangentsZ[counterCamera]*20 ,
		binormalX[counterCamera],binormalY[counterCamera],binormalZ[counterCamera]);
	else
		gluLookAt((cPointsX[counterCamera] + 2*(normalX[counterCamera]+binormalX[counterCamera])+
		(cPointsX[counterCamera] + 2*(-normalX[counterCamera]+binormalX[counterCamera])))/2 - binormalX[counterCamera] ,(cPointsY[counterCamera] + 
		2*(normalY[counterCamera]+binormalY[counterCamera])+
		(cPointsY[counterCamera] + 2*(-normalY[counterCamera]+binormalY[counterCamera])))/2 - binormalY[counterCamera] ,(cPointsZ[counterCamera] + 
		2*(normalZ[counterCamera]+binormalZ[counterCamera])+
		(cPointsZ[counterCamera] + 2*(-normalZ[counterCamera]+binormalZ[counterCamera])))/2 - binormalZ[counterCamera] ,cPointsX[counterCamera] 
	+ tangentsX[counterCamera]*20,
		cPointsY[counterCamera] + tangentsY[counterCamera]*20 ,
		cPointsZ[counterCamera]+ tangentsZ[counterCamera]*20 ,
		binormalX[counterCamera],binormalY[counterCamera],binormalZ[counterCamera]);
	if(!stop){
		if(counterCamera>cPointsX.size()-11)
			counterCamera=0;
		else
			counterCamera+=10 ;

	}

	glRotatef(g_vLandRotate[0], 1.0, 0.0,0.0);
	glRotatef(g_vLandRotate[1], 0.0, 1.0,0.0);
	glRotatef(g_vLandRotate[2], 0.0, 0.0,1.0);
	glScalef(g_vLandScale[0],g_vLandScale[1],g_vLandScale[2]);
	glTranslatef(g_vLandTranslate[0],g_vLandTranslate[1],g_vLandTranslate[2]);
	//texture maping
	glBindTexture(GL_TEXTURE_2D, 1);



	glEnable(GL_TEXTURE_2D); // turn texture mapping on
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB, g_pGroundData->nx, g_pGroundData->ny,GL_RGB,GL_UNSIGNED_BYTE,g_pGroundData->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_pGroundData->nx, g_pGroundData->ny, 0,
		GL_RGB, GL_UNSIGNED_BYTE, g_pGroundData->pix);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
		GL_REPLACE);
	glBegin(GL_QUADS); //draw the ground
	glTexCoord2f(0,0); 
	glVertex3f(-800.0,800.0,-800);
	glTexCoord2f(1,0); 
	glVertex3f(800.0,800.0,-800);
	glTexCoord2f(1,1); 
	glVertex3f(800.0,-800.0,-800);
	glTexCoord2f(0,1); 
	glVertex3f(-800.0,-800.0,-800);


	glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB, g_pSky2Data->nx, g_pSky2Data->ny,GL_RGB,GL_UNSIGNED_BYTE,g_pSky2Data->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_pSky2Data->nx, g_pSky2Data->ny, 0,
		GL_RGB, GL_UNSIGNED_BYTE, g_pSky2Data->pix);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
		GL_REPLACE);
	glBegin(GL_QUADS); // draw the star sky
	glTexCoord2f(0,0); 
	glVertex3f(-800.0,800.0,-100);
	glTexCoord2f(1,0); 
	glVertex3f(800.0,800.0,-100);
	glTexCoord2f(1,1); 
	glVertex3f(800.0,-800.0,-100);
	glTexCoord2f(0,1); 
	glVertex3f(-800.0,-800.0,-100);
	glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB, g_pSkyData->nx, g_pSkyData->ny,GL_RGB,GL_UNSIGNED_BYTE,g_pSkyData->pix);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_pSkyData->nx, g_pSkyData->ny, 0,
		GL_RGB, GL_UNSIGNED_BYTE, g_pSkyData->pix);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
		GL_REPLACE);
	//sky
	glBegin(GL_QUADS);

	glTexCoord2f(0,1); 
	glVertex3f(-800.0,800.0,-800);
	glTexCoord2f(1,1); 
	glVertex3f(800.0,800.0,-800);
	glTexCoord2f(1,0); 
	glVertex3f(800.0,800.0,-100);
	glTexCoord2f(0,0); 
	glVertex3f(-800.0,800.0,-100);


	glTexCoord2f(1,1); 
	glVertex3f(800.0,-800.0,-800);
	glTexCoord2f(0,1); 
	glVertex3f(800.0,800.0,-800);
	glTexCoord2f(0,0); 
	glVertex3f(800.0,800.0,-100);
	glTexCoord2f(1,0); 
	glVertex3f(800.0,-800.0,-100);


	glTexCoord2f(1,0); 
	glVertex3f(-800.0,800.0,-100);
	glTexCoord2f(0,0); 
	glVertex3f(-800.0,-800.0,-100);
	glTexCoord2f(0,1); 
	glVertex3f(-800.0,-800.0,-800);
	glTexCoord2f(1,1); 
	glVertex3f(-800.0,800.0,-800);

	glTexCoord2f(1,0); 
	glVertex3f(800.0,-800.0,-100);
	glTexCoord2f(0,0); 
	glVertex3f(-800.0,-800.0,-100);
	glTexCoord2f(0,1); 
	glVertex3f(-800.0,-800.0,-800);
	glTexCoord2f(1,1); 
	glVertex3f(800.0,-800.0,-800);

	glEnd();

	glDisable(GL_TEXTURE_2D); // turn texture mapping off



	if(level1){//draw the single splines, I enlarged the spines 8 times to have a better view.
		glBegin(GL_LINE_STRIP);
		glColor3f(1.0,0.0,0.0);
		for(int i = 1; i <g_Splines->numControlPoints-2;i++ ){
			for(float t = 0.00; t < 1; t +=0.01){
				glColor3f(1.0,0.0,0.0);
				x = -15+splineFactor *((2 * g_Splines->points[i].x) +
					(-g_Splines->points[i-1].x + g_Splines->points[i+1].x) * t +
					(2*g_Splines->points[i-1].x - 5*g_Splines->points[i].x + 4*g_Splines->points[i+1].x - g_Splines->points[i+2].x) * t*t +
					(-g_Splines->points[i-1].x + 3*g_Splines->points[i].x- 3*g_Splines->points[i+1].x + g_Splines->points[i+2].x) * t*t*t);
				y = -15+splineFactor *((2 * g_Splines->points[i].y) +
					(-g_Splines->points[i-1].y + g_Splines->points[i+1].y) * t +
					(2*g_Splines->points[i-1].y - 5*g_Splines->points[i].y + 4*g_Splines->points[i+1].y - g_Splines->points[i+2].y) * t*t +
					(-g_Splines->points[i-1].y + 3*g_Splines->points[i].y- 3*g_Splines->points[i+1].y + g_Splines->points[i+2].y) * t*t*t);
				z = -450+splineFactor *((2 * g_Splines->points[i].z) +
					(-g_Splines->points[i-1].z + g_Splines->points[i+1].z) * t +
					(2*g_Splines->points[i-1].z - 5*g_Splines->points[i].z + 4*g_Splines->points[i+1].z - g_Splines->points[i+2].z) * t*t +
					(-g_Splines->points[i-1].z + 3*g_Splines->points[i].z- 3*g_Splines->points[i+1].z + g_Splines->points[i+2].z) * t*t*t);
				glVertex3f(x,y,z);
			}
		}
		glEnd();
	}
	else{
		//draw real trails;
		if(!renderTube && !track){
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10;i+=10){
				glColor3f(1,0,0);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=10){
				glColor3f(1,0,0);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
			}
			glEnd();
			//draw the line rail bar;
			glBegin(GL_LINES);
			for(int i = 0; i < cPointsX.size()-11;i+=10){
				glColor3f(0,1,1);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);		
			}
			glEnd();
		}else if(renderTube && !track){//renderTube & !track = render tube 
			//mark the four corners as vertices, then connect them together as the bondary of the tube.
			glBegin(GL_LINES);
			for(int i = 0; i < cPointsX.size()-11;i+=10){
				glColor3f(0,1,1);
				float v0x =	cPointsX[i] + 2*(normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);

				glVertex3f(v1x,v1y,v1z);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				glVertex3f(v2x,v2y,v2z);

				float v3x = cPointsX[i] + 2*(-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);
				glVertex3f(v3x,v3y,v3z);
				glVertex3f(v0x,v0y,v0z);

			}
			glEnd();
			float x,y,z;
			//render the routines
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-11;i+=10){
				glColor3f(1,0,0);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);

			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-11; i+=10){
				glColor3f(1,0,0);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-11;i+=10){
				glColor3f(1,0,0);
				float v0x = cPointsX[i] + 2*(normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-11; i+=10){
				glColor3f(1,0,0);
				float v3x = cPointsX[i] + 2*(-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);
			}
			glEnd();
		}


		else{//when user select rail mode.
			glLineWidth(1.0);
			//render the line of the cross-section, draft
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v0x = cPointsX[i] + 2*(-normalX[i]+binormalX[i])+ (GLfloat)0.3 * (normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(-normalY[i]+binormalY[i])+ (GLfloat)0.3 * (normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i])+ (GLfloat)0.3 * (normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);


			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);


			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);

			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10;i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v1x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			//render the cross-section;
			//Method: we know the consecutive points and how to compute their four corner points (ex: V1 = P0 + a*(N1 + B1)) , 
			//then, we can compute the the corner points's corner points by doing silimar math. I use this method to render the rail's cross section.
			//
			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				v2x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]+binormalX[i+2]);
				v2y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]+binormalY[i+2]);
				v2z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v2x,v2y,v2z);
				v3x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]-binormalX[i+2]);
				v3y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]-binormalY[i+2]);
				v3z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v3x,v3y,v3z);
			}
			glEnd();
			/*****************/

			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v1x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				v2x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]+binormalX[i+2]);
				v2y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]+binormalY[i+2]);
				v2z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v2x,v2y,v2z);
				v1x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]+binormalX[i+2]);
				v1y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]+binormalY[i+2]);
				v1z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			/**************/

			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v1x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				float v0x = cPointsX[i] + 2*(-normalX[i]+binormalX[i])+ (GLfloat)0.3 * (normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(-normalY[i]+binormalY[i])+ (GLfloat)0.3 * (normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i])+ (GLfloat)0.3 * (normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);
				v0x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]-binormalX[i+2]);
				v0y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]-binormalY[i+2]);
				v0z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v0x,v0y,v0z);
				v1x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]+binormalX[i+2]);
				v1y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]+binormalY[i+2]);
				v1z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			/***********/
			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);
				float v0x = cPointsX[i] + 2*(-normalX[i]+binormalX[i])+ (GLfloat)0.3 * (normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(-normalY[i]+binormalY[i])+ (GLfloat)0.3 * (normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i])+ (GLfloat)0.3 * (normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);
				v0x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]-binormalX[i+2]);
				v0y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]-binormalY[i+2]);
				v0z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v0x,v0y,v0z);
				v3x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]-binormalX[i+2]);
				v3y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]-binormalY[i+2]);
				v3z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v3x,v3y,v3z);
			}
			glEnd();
			/************/
			//render the right side cross-section base line
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v0x = cPointsX[i] + 2*(normalX[i]+binormalX[i])+ (GLfloat)0.3 * (normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(normalY[i]+binormalY[i])+ (GLfloat)0.3 * (normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i])+ (GLfloat)0.3 * (normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);

			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v2x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);

			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);

			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < cPointsX.size()-10;i+=2){
				glColor3f(0,(GLfloat)51/255,(GLfloat)102/255);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			/**************/
			//render the right side rail cross-section

			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);
				float v2x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				v2x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]+binormalX[i+2]);
				v2y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]+binormalY[i+2]);
				v2z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v2x,v2y,v2z);
				v3x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]-binormalX[i+2]);
				v3y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]-binormalY[i+2]);
				v3z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v3x,v3y,v3z);
			}
			glEnd();
			/*****************/

			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				float v2x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				v2x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]+binormalX[i+2]);
				v2y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]+binormalY[i+2]);
				v2z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v2x,v2y,v2z);
				v1x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]+binormalX[i+2]);
				v1y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]+binormalY[i+2]);
				v1z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			/**************/

			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				float v0x = cPointsX[i] + 2*(normalX[i]+binormalX[i])+ (GLfloat)0.3 * (normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(normalY[i]+binormalY[i])+ (GLfloat)0.3 * (normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i])+ (GLfloat)0.3 * (normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);
				v0x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]-binormalX[i+2]);
				v0y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]-binormalY[i+2]);
				v0z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v0x,v0y,v0z);
				v1x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]+binormalX[i+2]);
				v1y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]+binormalY[i+2]);
				v1z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]+binormalZ[i+2]);
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			/***********/
			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-10; i+=2){
				glColor3f((GLfloat)102/255,(GLfloat)102/255,(GLfloat)102/255);
				float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + (GLfloat)0.3 * (-normalX[i]-binormalX[i]);
				float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + (GLfloat)0.3 * (-normalY[i]-binormalY[i]);
				float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + (GLfloat)0.3 * (-normalZ[i]-binormalZ[i]);
				glVertex3f(v3x,v3y,v3z);
				float v0x = cPointsX[i] + 2*(normalX[i]+binormalX[i])+ (GLfloat)0.3 * (normalX[i]-binormalX[i]);
				float v0y = cPointsY[i] + 2*(normalY[i]+binormalY[i])+ (GLfloat)0.3 * (normalY[i]-binormalY[i]);
				float v0z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i])+ (GLfloat)0.3 * (normalZ[i]-binormalZ[i]);
				glVertex3f(v0x,v0y,v0z);
				v0x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (normalX[i+2]-binormalX[i+2]);
				v0y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (normalY[i+2]-binormalY[i+2]);
				v0z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v0x,v0y,v0z);
				v3x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + (GLfloat)0.3 * (-normalX[i+2]-binormalX[i+2]);
				v3y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + (GLfloat)0.3 * (-normalY[i+2]-binormalY[i+2]);
				v3z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + (GLfloat)0.3 * (-normalZ[i+2]-binormalZ[i+2]);
				glVertex3f(v3x,v3y,v3z);
			}
			glEnd();
			if(tShape){//render the T shape rail
				glBegin(GL_LINE_STRIP);
				for(int i = 0; i < cPointsX.size()-10; i+=2){
					glColor3f(float(255/255),float(128/255),float(0/255));
					float v0x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]-binormalX[i]);
					float v0y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]-binormalY[i]);
					float v0z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]-binormalZ[i]);
					glVertex3f(v0x,v0y,v0z);

				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				for(int i = 0; i < cPointsX.size()-10; i+=2){

					float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]+binormalX[i]);
					float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]+binormalY[i]);
					float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]+binormalZ[i]);
					glVertex3f(v1x,v1y,v1z);
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				for(int i = 0; i < cPointsX.size()-10; i+=2){

					float v2x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]+binormalX[i]);
					float v2y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]+binormalY[i]);
					float v2z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]+binormalZ[i]);
					glVertex3f(v2x,v2y,v2z);
				}
				glEnd();


				glBegin(GL_LINE_STRIP);
				for(int i = 0; i < cPointsX.size()-10; i+=2){

					float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]-binormalX[i]);
					float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]-binormalY[i]);
					float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]-binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
				}
				glEnd();
				/**************/
				glBegin(GL_LINES);
				for(int i = 0; i < cPointsX.size()-10; i+=2){
					glColor3f(float(255/255),float(128/255),float(0/255));
					float v0x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]-binormalX[i]);
					float v0y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]-binormalY[i]);
					float v0z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]-binormalZ[i]);
					glVertex3f(v0x,v0y,v0z);
					v0x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]-binormalX[i+2]);
					v0y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]-binormalY[i+2]);
					v0z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v0x,v0y,v0z);

				}
				glEnd();
				glBegin(GL_LINES);
				for(int i = 0; i < cPointsX.size()-10; i+=2){

					float v1x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]+binormalX[i]);
					float v1y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]+binormalY[i]);
					float v1z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]+binormalZ[i]);
					glVertex3f(v1x,v1y,v1z);
					v1x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]+binormalX[i+2]);
					v1y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]+binormalY[i+2]);
					v1z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v1x,v1y,v1z);
				}
				glEnd();
				glBegin(GL_LINES);
				for(int i = 0; i < cPointsX.size()-10; i+=2){

					float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]+binormalX[i]);
					float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]+binormalY[i]);
					float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]+binormalZ[i]);
					glVertex3f(v2x,v2y,v2z);
					v2x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]+binormalX[i+2]);
					v2y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]+binormalY[i+2]);
					v2z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v2x,v2y,v2z);
				}
				glEnd();


				glBegin(GL_LINES);
				for(int i = 0; i < cPointsX.size()-10; i+=2){

					float v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]-binormalX[i]);
					float v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]-binormalY[i]);
					float v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]-binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
					v3x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]-binormalX[i+2]);
					v3y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]-binormalY[i+2]);
					v3z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v3x,v3y,v3z);
				}
				glEnd();
				//render the lines vertically;
				/*******************/
				glBegin(GL_LINES);
				for(int i = 0; i < cPointsX.size()-11; i+=10){
					glColor3f((GLfloat)52/255,(GLfloat)222/255,(GLfloat)100/255);
					float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i];
					float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i];
					float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i];
					glVertex3f(v3x,v3y,v3z);
					v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]);
					v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]);
					v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
					v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i];
					v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i];
					v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i];
					glVertex3f(v3x,v3y,v3z);
					v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]);
					v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]);
					v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);

				}
				glEnd();
				//render the upper rail cross-section;
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-11;i+=2){
					glColor3f((GLfloat)51/255,(GLfloat)51/255,(GLfloat)51/255);
					float v0x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]-binormalX[i]);
					float v0y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]-binormalY[i]);
					float v0z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]-binormalZ[i]);
					glVertex3f(v0x,v0y,v0z);
					float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]+binormalX[i]);
					float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]+binormalY[i]);
					float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]+binormalZ[i]);
					glVertex3f(v1x,v1y,v1z);
					v1x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]+binormalX[i+2]);
					v1y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]+binormalY[i+2]);
					v1z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v1x,v1y,v1z);
					v0x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]-binormalX[i+2]);
					v0y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]-binormalY[i+2]);
					v0z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v0x,v0y,v0z);

				}
				glEnd();
				/***********/
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-11;i+=2){

					float v1x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]+binormalX[i]);
					float v1y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]+binormalY[i]);
					float v1z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]+binormalZ[i]);
					glVertex3f(v1x,v1y,v1z);
					float v2x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]+binormalX[i]);
					float v2y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]+binormalY[i]);
					float v2z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]+binormalZ[i]);
					glVertex3f(v2x,v2y,v2z);
					v2x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]+binormalX[i+2]);
					v2y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]+binormalY[i+2]);
					v2z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v2x,v2y,v2z);
					v1x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]+binormalX[i+2]);
					v1y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]+binormalY[i+2]);
					v1z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v1x,v1y,v1z);

				}
				glEnd();
				/**************/
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-11;i+=2){

					float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]-binormalX[i]);
					float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]-binormalY[i]);
					float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]-binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
					float v2x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]+binormalX[i]);
					float v2y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]+binormalY[i]);
					float v2z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]+binormalZ[i]);
					glVertex3f(v2x,v2y,v2z);
					v2x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]+binormalX[i+2]);
					v2y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]+binormalY[i+2]);
					v2z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v2x,v2y,v2z);

					v3x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]-binormalX[i+2]);
					v3y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]-binormalY[i+2]);
					v3z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v3x,v3y,v3z);


				}
				glEnd();
				/**********/
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-10;i+=2){

					float v3x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]-binormalX[i]);
					float v3y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]-binormalY[i]);
					float v3z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]-binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
					float v0x = cPointsX[i] + 2*(normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]-binormalX[i]);
					float v0y = cPointsY[i] + 2*(normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]-binormalY[i]);
					float v0z = cPointsZ[i] + 2*(normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]-binormalZ[i]);
					glVertex3f(v0x,v0y,v0z);
					v0x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]-binormalX[i+2]);
					v0y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]-binormalY[i+2]);
					v0z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v0x,v0y,v0z);
					v3x = cPointsX[i+2] + 2*(normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]-binormalX[i+2]);
					v3y = cPointsY[i+2] + 2*(normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]-binormalY[i+2]);
					v3z = cPointsZ[i+2] + 2*(normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v3x,v3y,v3z);


				}
				glEnd();
				/*************/
				//render the left cross section;
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-11;i+=2){
					glColor3f((GLfloat)51/255,(GLfloat)51/255,(GLfloat)51/255);
					float v0x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]-binormalX[i]);
					float v0y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]-binormalY[i]);
					float v0z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]-binormalZ[i]);
					glVertex3f(v0x,v0y,v0z);
					float v1x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]+binormalX[i]);
					float v1y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]+binormalY[i]);
					float v1z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]+binormalZ[i]);
					glVertex3f(v1x,v1y,v1z);
					v1x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]+binormalX[i+2]);
					v1y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]+binormalY[i+2]);
					v1z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v1x,v1y,v1z);
					v0x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]-binormalX[i+2]);
					v0y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]-binormalY[i+2]);
					v0z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v0x,v0y,v0z);

				}
				glEnd();
				/***********/
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-11;i+=2){

					float v1x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]+binormalX[i]);
					float v1y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]+binormalY[i]);
					float v1z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]+binormalZ[i]);
					glVertex3f(v1x,v1y,v1z);
					float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]+binormalX[i]);
					float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]+binormalY[i]);
					float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]+binormalZ[i]);
					glVertex3f(v2x,v2y,v2z);
					v2x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]+binormalX[i+2]);
					v2y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]+binormalY[i+2]);
					v2z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v2x,v2y,v2z);
					v1x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]+binormalX[i+2]);
					v1y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]+binormalY[i+2]);
					v1z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v1x,v1y,v1z);

				}
				glEnd();
				/**************/
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-11;i+=2){

					float v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]-binormalX[i]);
					float v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]-binormalY[i]);
					float v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]-binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
					float v2x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]+binormalX[i]);
					float v2y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]+binormalY[i]);
					float v2z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]+binormalZ[i]);
					glVertex3f(v2x,v2y,v2z);
					v2x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]+binormalX[i+2]);
					v2y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]+binormalY[i+2]);
					v2z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]+binormalZ[i+2]);
					glVertex3f(v2x,v2y,v2z);

					v3x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]-binormalX[i+2]);
					v3y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]-binormalY[i+2]);
					v3z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v3x,v3y,v3z);


				}
				glEnd();
				/**********/
				glBegin(GL_QUADS);
				for(int i = 0; i < cPointsX.size()-10;i+=2){

					float v3x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (-normalX[i]-binormalX[i]);
					float v3y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (-normalY[i]-binormalY[i]);
					float v3z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (-normalZ[i]-binormalZ[i]);
					glVertex3f(v3x,v3y,v3z);
					float v0x = cPointsX[i] + 2*(-normalX[i]+binormalX[i]) + 2* binormalX[i] + 0.3 * (normalX[i]-binormalX[i]);
					float v0y = cPointsY[i] + 2*(-normalY[i]+binormalY[i]) + 2* binormalY[i] + 0.3 * (normalY[i]-binormalY[i]);
					float v0z = cPointsZ[i] + 2*(-normalZ[i]+binormalZ[i]) + 2* binormalZ[i] + 0.3 * (normalZ[i]-binormalZ[i]);
					glVertex3f(v0x,v0y,v0z);
					v0x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (normalX[i+2]-binormalX[i+2]);
					v0y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (normalY[i+2]-binormalY[i+2]);
					v0z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v0x,v0y,v0z);
					v3x = cPointsX[i+2] + 2*(-normalX[i+2]+binormalX[i+2]) + 2* binormalX[i+2] + 0.3 * (-normalX[i+2]-binormalX[i+2]);
					v3y = cPointsY[i+2] + 2*(-normalY[i+2]+binormalY[i+2]) + 2* binormalY[i+2] + 0.3 * (-normalY[i+2]-binormalY[i+2]);
					v3z = cPointsZ[i+2] + 2*(-normalZ[i+2]+binormalZ[i+2]) + 2* binormalZ[i+2] + 0.3 * (-normalZ[i+2]-binormalZ[i+2]);
					glVertex3f(v3x,v3y,v3z);


				}
				glEnd();
			}
			/*************/
			glLineWidth(1.0);
			//texture maping
			glBindTexture(GL_TEXTURE_2D, 1);
			glEnable(GL_TEXTURE_2D); // turn texture mapping on
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB, g_pRailData->nx, g_pRailData->ny,GL_RGB,GL_UNSIGNED_BYTE,g_pRailData->pix);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_pRailData->nx, g_pRailData->ny, 0,
				GL_RGB, GL_UNSIGNED_BYTE, g_pRailData->pix);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
				GL_REPLACE);
			//render the texture mapped rail bar
			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-50;i+=30){
				glTexCoord2f(0,1);
				float v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				glTexCoord2f(0,0);
				float v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,0);
				v2x = cPointsX[i+10] + 1.8*(-normalX[i+10]+binormalX[i+10]);
				v2y = cPointsY[i+10] + 1.8*(-normalY[i+10]+binormalY[i+10]);
				v2z = cPointsZ[i+10] + 1.8*(-normalZ[i+10]+binormalZ[i+10]);
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,1);
				v1x = cPointsX[i+10] + 1.8*(normalX[i+10]+binormalX[i+10]);
				v1y = cPointsY[i+10] + 1.8*(normalY[i+10]+binormalY[i+10]);
				v1z = cPointsZ[i+10] + 1.8*(normalZ[i+10]+binormalZ[i+10]);
				glVertex3f(v1x,v1y,v1z);


				/********************************************/

			}
			glEnd();
			/**********/
			//render the back of rail bar
			glBegin(GL_QUADS);
			for(int i = 10; i < cPointsX.size()-50;i+=30){
				glTexCoord2f(0,1);
				float v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				glTexCoord2f(0,0);
				float v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,0);
				v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,1);
				v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v1x,v1y,v1z);
			}
			/*****************/
			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-50;i+=30){
				glTexCoord2f(0,1);
				float v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);
				glTexCoord2f(0,0);
				float v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,0);
				v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,1);
				v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			/*****************/
			//render the bottom
			glBegin(GL_QUADS);
			for(int i = 0; i < cPointsX.size()-50;i+=30){
				glTexCoord2f(0,1);
				float v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				float v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				float v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v1x,v1y,v1z);
				glTexCoord2f(0,0);
				float v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				float v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				float v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,0);
				v2x = cPointsX[i+10] + 1.8*(-normalX[i+10]+binormalX[i+10])- 0.2 * binormalX[i+10];
				v2y = cPointsY[i+10] + 1.8*(-normalY[i+10]+binormalY[i+10])- 0.2 * binormalY[i+10];
				v2z = cPointsZ[i+10] + 1.8*(-normalZ[i+10]+binormalZ[i+10])- 0.2 * binormalZ[i+10];
				glVertex3f(v2x,v2y,v2z);
				glTexCoord2f(1,1);
				v1x = cPointsX[i+10] + 1.8*(normalX[i+10]+binormalX[i+10])- 0.2 * binormalX[i+10];
				v1y = cPointsY[i+10] + 1.8*(normalY[i+10]+binormalY[i+10])- 0.2 * binormalY[i+10];
				v1z = cPointsZ[i+10] + 1.8*(normalZ[i+10]+binormalZ[i+10])- 0.2 * binormalZ[i+10];
				glVertex3f(v1x,v1y,v1z);
			}
			glEnd();
			glDisable(GL_TEXTURE_2D); 
			//draw the line in the texture map
			glBegin(GL_LINES);
			for(int i = 0; i < cPointsX.size()-50;i+=30){
				glColor3f(0,(GLfloat) 191/255,1);
				float v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i]);
				float v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i]);
				float v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i]);
				glVertex3f(v1x,v1y,v1z);

				float v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i]);
				float v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i]);
				float v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i]);
				glVertex3f(v2x,v2y,v2z);

				v1x = cPointsX[i] + 1.8*(normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				v1y = cPointsY[i] + 1.8*(normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				v1z = cPointsZ[i] + 1.8*(normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v1x,v1y,v1z);

				v2x = cPointsX[i] + 1.8*(-normalX[i]+binormalX[i])- 0.2 * binormalX[i];
				v2y = cPointsY[i] + 1.8*(-normalY[i]+binormalY[i])- 0.2 * binormalY[i];
				v2z = cPointsZ[i] + 1.8*(-normalZ[i]+binormalZ[i])- 0.2 * binormalZ[i];
				glVertex3f(v2x,v2y,v2z);

			}
			glEnd();
		}
	}
	glutSwapBuffers();//swap buffers 
	//use sprintf to manipulate the name of screenshot images.
	/*
	if(counter<10)
		n = sprintf (buffer, "00%d.jpeg", counter);
	else if(counter<100)
		n = sprintf (buffer, "0%d.jpeg", counter);
	else
		n = sprintf (buffer, "%d.jpeg", counter);
	counter = counter+1;
	saveScreenshot((char*)buffer);
	*/

}

void menufunc(int value)
{
	switch (value)
	{
	case 0:
		exit(0);
		break;
	}
}

void doIdle()
{


	/* do some stuff... */
	//compute the vectors
	//compute V0
	calculateFPS();
	cPointsX.clear();
	cPointsY.clear();
	cPointsZ.clear();

	float x = splineFactor *(
		(-g_Splines->points[0].x + g_Splines->points[1].x)+
		(2*g_Splines->points[0].x - 5*g_Splines->points[1].x + 4*g_Splines->points[2].x - g_Splines->points[3].x) * 2*0.01 +
		(-g_Splines->points[0].x + 3*g_Splines->points[1].x- 3*g_Splines->points[2].x + g_Splines->points[3].x) * 3*0.01*0.01);
	float y = splineFactor *(
		(-g_Splines->points[0].y + g_Splines->points[1].y) +
		(2*g_Splines->points[0].y - 5*g_Splines->points[1].y + 4*g_Splines->points[2].y - g_Splines->points[3].y) * 2*0.01 +
		(-g_Splines->points[0].y + 3*g_Splines->points[1].y- 3*g_Splines->points[2].y + g_Splines->points[3].y) * 3*0.01*0.01);
	float z = splineFactor *(
		(-g_Splines->points[0].z + g_Splines->points[1].z) +
		(2*g_Splines->points[0].z - 5*g_Splines->points[1].z + 4*g_Splines->points[2].z - g_Splines->points[3].z) * 2*0.01 +
		(-g_Splines->points[0].z + 3*g_Splines->points[1].z- 3*g_Splines->points[2].z + g_Splines->points[3].z) * 3*0.01*0.01);
	//normalize tangent vectors
	float tempX = x;
	float tempY = y;
	float tempZ = z;
	x = tempX/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	y = tempY/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	z = tempZ/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	tangentsX.push_back(x);
	tangentsY.push_back(y);
	tangentsZ.push_back(z);

	//compute first normal vector
	tempX = x;
	tempY = y;
	tempZ = z;
	//assume the 0,0,-1 is an arbitrary vector
	x = tempY*(-1)-0;
	y = 0-(-1)*tempX;
	z = 0;
	//normalize normal vector
	tempX = x;
	tempY = y;
	tempZ = z;
	x = tempX/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	y = tempY/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	z = tempZ/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	normalX.push_back(x);
	normalY.push_back(y);
	normalZ.push_back(z);
	//compute the first binormal vector

	tempX = x;
	tempY = y;
	tempZ = z;
	x = tangentsY[0]*tempZ -tangentsZ[0]*tempY;
	y = tangentsZ[0]*tempX - tangentsX[0]*tempZ;
	z = tangentsX[0]*tempY - tangentsY[0]*tempX;
	//normalize binormal
	tempX = x;
	tempY = y;
	tempZ = z;
	x = tempX/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	y = tempY/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	z = tempZ/(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
	binormalX.push_back(x);
	binormalY.push_back(y);
	binormalZ.push_back(z);



	for(int i = 1; i <g_Splines->numControlPoints-1;i++ ){
		for(float t = 0.00; t < 1; t +=0.01){
			glColor3f(1.0,0.0,0.0);

			float x =-15 +splineFactor *((2 * g_Splines->points[i].x) +
				(-g_Splines->points[i-1].x + g_Splines->points[i+1].x) * t +
				(2*g_Splines->points[i-1].x - 5*g_Splines->points[i].x + 4*g_Splines->points[i+1].x - g_Splines->points[i+2].x) * t*t +
				(-g_Splines->points[i-1].x + 3*g_Splines->points[i].x- 3*g_Splines->points[i+1].x + g_Splines->points[i+2].x) * t*t*t);
			float y = -15+splineFactor *((2 * g_Splines->points[i].y) +
				(-g_Splines->points[i-1].y + g_Splines->points[i+1].y) * t +
				(2*g_Splines->points[i-1].y - 5*g_Splines->points[i].y + 4*g_Splines->points[i+1].y - g_Splines->points[i+2].y) * t*t +
				(-g_Splines->points[i-1].y + 3*g_Splines->points[i].y- 3*g_Splines->points[i+1].y + g_Splines->points[i+2].y) * t*t*t);
			float z =-450+ splineFactor *((2 * g_Splines->points[i].z) +
				(-g_Splines->points[i-1].z + g_Splines->points[i+1].z) * t +
				(2*g_Splines->points[i-1].z - 5*g_Splines->points[i].z + 4*g_Splines->points[i+1].z - g_Splines->points[i+2].z) * t*t +
				(-g_Splines->points[i-1].z + 3*g_Splines->points[i].z- 3*g_Splines->points[i+1].z + g_Splines->points[i+2].z) * t*t*t);

			cPointsX.push_back(x);
			cPointsY.push_back(y);
			cPointsZ.push_back(z);



			//set camera;

			//compute tangent vectors
			x = (
				(-g_Splines->points[i-1].x + g_Splines->points[i+1].x)+
				(2*g_Splines->points[i-1].x - 5*g_Splines->points[i].x + 4*g_Splines->points[i+1].x - g_Splines->points[i+2].x) * 2*t +
				(-g_Splines->points[i-1].x + 3*g_Splines->points[i].x- 3*g_Splines->points[i+1].x + g_Splines->points[i+2].x) * 3*t*t);
			y = (
				(-g_Splines->points[i-1].y + g_Splines->points[i+1].y) +
				(2*g_Splines->points[i-1].y - 5*g_Splines->points[i].y + 4*g_Splines->points[i+1].y - g_Splines->points[i+2].y) * 2*t +
				(-g_Splines->points[i-1].y + 3*g_Splines->points[i].y- 3*g_Splines->points[i+1].y + g_Splines->points[i+2].y) * 3*t*t);
			z = (
				(-g_Splines->points[i-1].z + g_Splines->points[i+1].z) +
				(2*g_Splines->points[i-1].z - 5*g_Splines->points[i].z + 4*g_Splines->points[i+1].z - g_Splines->points[i+2].z) * 2*t +
				(-g_Splines->points[i-1].z + 3*g_Splines->points[i].z- 3*g_Splines->points[i+1].z + g_Splines->points[i+2].z) * 3*t*t);
			//normalize tangent vectors
			float tempX = x;
			float tempY = y;
			float tempZ = z;
			x = tempX/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			y = tempY/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			z = tempZ/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			tangentsX.push_back(x);
			tangentsY.push_back(y);
			tangentsZ.push_back(z);

			//compute normals

			tempX = binormalY.back()* tangentsZ[binormalX.size()] - binormalZ.back() * tangentsY[binormalX.size()];
			tempY = binormalZ.back() * tangentsX[binormalX.size()] - binormalX.back() * tangentsZ[binormalX.size()];
			tempZ = binormalX.back() * tangentsY[binormalX.size()] - binormalY.back() * tangentsX[binormalX.size()];
			x = tempX/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			y = tempY/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			z = tempZ/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			normalX.push_back(x);
			normalY.push_back(y);
			normalZ.push_back(z);
			//compute the binormal
			tempX =  tangentsZ[normalX.size()-1] * normalY.back() - tangentsY[normalX.size()-1] * normalZ.back();
			tempY =  tangentsX[normalX.size()-1] * normalZ.back() - tangentsZ[normalX.size()-1] * normalX.back();
			tempZ =  tangentsY[normalX.size()-1] * normalX.back() - tangentsX[normalX.size()-1] * normalY.back();
			x = tempX/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			y = tempY/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			z = tempZ/(GLfloat)(sqrtf(tempX*tempX+tempY*tempY+tempZ*tempZ));
			binormalX.push_back(x);
			binormalY.push_back(y);
			binormalZ.push_back(z);
		}
	}
	/* make the screen update */
	glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
	int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};

	switch (g_ControlState)
	{
	case TRANSLATE:  
		if (g_iLeftMouseButton)
		{
			g_vLandTranslate[0] += vMouseDelta[0]*0.01;
			g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandTranslate[2] += vMouseDelta[1]*0.01;
		}
		break;
	case ROTATE:
		if (g_iLeftMouseButton)
		{
			g_vLandRotate[0] += vMouseDelta[1];
			g_vLandRotate[1] += vMouseDelta[0];
		}

		if (g_iMiddleMouseButton)
		{
			g_vLandRotate[2] += vMouseDelta[1];
		}
		break;
	case SCALE:
		if (g_iLeftMouseButton)
		{
			g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
			g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
		}
		break;
	}
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state==GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state==GLUT_DOWN);
		break;
	}

	switch(glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		g_ControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		g_ControlState = SCALE;
		break;
	default:
		g_ControlState = ROTATE;
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}




int loadSplines(char *argv) {
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, i = 0, j, iLength;

	//display();

	/* load the track file */
	fileList = fopen(argv, "r");
	if (fileList == NULL) {
		printf ("can't open file\n");
		exit(1);
	}

	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &g_iNumOfSplines);

	g_Splines = (struct spline *)malloc(g_iNumOfSplines * sizeof(struct spline));

	/* reads through the spline files */
	for (j = 0; j < g_iNumOfSplines; j++) {
		i = 0;
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf ("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		g_Splines[j].points = (struct point *)malloc(iLength * sizeof(struct point));
		g_Splines[j].numControlPoints = iLength;

		/* saves the data to the struct */
		while (fscanf(fileSpline, "%lf %lf %lf", 
			&g_Splines[j].points[i].x, 
			&g_Splines[j].points[i].y, 
			&g_Splines[j].points[i].z) != EOF) {
				i++;
		}
	}

	free(cName);

	return 0;
}
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 116: // 27 is the code for the ESC key.
		renderTube=!renderTube;
		break;
	case 119:
		focusFactor+=10;
		break;
	case 115:
		focusFactor-=10;
		break;
	case 49:
		level1=!level1;
		break;
	case 97:
		stop=!stop;
		break;
	case 100:
		track = !track;
		break;
	case 102:
		tShape = !tShape;
		break;
	}
}

void calculateFPS()
{
	frameCounter++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;
	if(timeInterval > 1000)
	{
		fps = frameCounter / (timeInterval / 1000.0f);
		previousTime = currentTime;
		frameCounter = 0;
	}
	drawFPS();
}
void drawFPS()
{
	//  This draw FPS doesn't work right now
	/*
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glRasterPos2f(200.0, 200.0);

	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, fps);
	glEnable(GL_DEPTH_TEST);
	//  Print the FPS to the window
	//glutSetWindowTitle (fpsChar);*/
}
int _tmain(int argc, _TCHAR* argv[])
{
	// I've set the argv[1] to track.txt.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your track file name for the "Command Arguments"
	//arg[2] is ground image;
	if (argc<2)
	{  
		printf ("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}

	loadSplines(argv[1]);
	g_pGroundData = jpeg_read("ground1.jpg", NULL);
	g_pSkyData = jpeg_read("sky1.jpg",NULL);
	g_pRailData = jpeg_read("wood2.jpg",NULL);
	g_pSky2Data = jpeg_read("sky2.jpg",NULL);
	if (!g_pGroundData)
	{
		printf ("error reading %s.\n", "ground1.jpg");
		exit(1);
	}
	if (!g_pSkyData)
	{
		printf ("error reading %s.\n", "sky1.jpg");
		exit(1);
	}
	if (!g_pRailData)
	{
		printf ("error reading %s.\n", "wood2.jpg");
		exit(1);
	}
	if(!g_pSky2Data){
		printf ("error reading %s.\n", "sky2.jpg");
		exit(1);
	}
	glutInit(&argc,(char**)argv);

	/*
	create a window here..should be double buffered and use depth testing
	*/

	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (640, 480);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);

	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	/* callback for key pressess */
	glutKeyboardFunc(keyboard);



	/* do initialization */
	myinit();

	glutMainLoop();
	return 0;



}