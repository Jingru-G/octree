//
//  main.cpp
//  Octree
//
//  Created by JING on 18/05/2018.
//  Copyright © 2018 JING. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Octree.h"
using namespace std;

void controls(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS)
        if(key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* initWindow(const int resX, const int resY){
    if(!glfwInit()){
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    
    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(resX, resY, "Octree", NULL, NULL);
    
    if(window == NULL){
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);
    
    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
    
    glEnable(GL_DEPTH_TEST); // Depth Testing
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

// tuples of vertices to form a wired cube
static const GLint index_list[][2] ={
    {0, 1},
    {2, 3},
    {4, 5},
    {6, 7},
    {0, 2},
    {1, 3},
    {4, 6},
    {5, 7},
    {0, 4},
    {1, 5},
    {7, 3},
    {2, 6}
};

// draw a wired cube
static double MaxRadius=100;//
void drawBorder(const Point *center, double radius){
    int i,j;
    float half=radius/4.0;
    glColor3f(-radius*0.6 / MaxRadius + 1.0f, 0.5f,-radius / MaxRadius +  0.5f);
    //a cube divided into eight
    for (int m=-1;m<2;m+=2){
        for (int n=-1;n<2;n+=2){
            for (int k=-1;k<2;k+=2){
                float x=center->x + half * m;
                float y=center->y + half * n;
                float z=center->z + half * k;
                //coordinates
                float vertex_list[][3] =
                {
                    x-half, y-half, z-half,
                    x+half, y-half, z-half,
                    x-half, y+half, z-half,
                    x+half, y+half, z-half,
                    x-half, y-half, z+half,
                    x+half, y-half, z+half,
                    x-half, y+half, z+half,
                    x+half, y+half, z+half
                };
                glBegin(GL_LINES);
                for(i=0; i<12; ++i){
                    for(j=0; j<2; ++j){
                        glVertex3fv(vertex_list[index_list[i][j]]);
                    }
                }
                glEnd();
            }
        }
    }
}
void drawCube(float x, float y, float z){
    GLfloat vertices[] ={
        x, y, z,    x, y,  z+1,    x,  y+1,  z+1,    x, y+1, z,
        x+1, y, z,  x+1, y,  z+1,  x+1,  y+1,  z+1,  x+1, y+1, z,
        x, y, z,    x, y,  z+1,    x+1,  y,  z+1,    x+1, y, z,
        x, y+1, z,  x, y+1,  z+1,  x+1,  y+1,  z+1,  x+1, y+1, z,
        x, y, z,    x, y+1,  z,    x+1,  y+1,  z,    x+1, y, z,
        x, y, z+1,  x, y+1,  z+1,  x+1,  y+1,  z+1,  x+1, y, z+1
    };
    GLfloat colors[] ={
        0, 0, 0,   0, 0, 1,   0, 1, 1,   0, 1, 0,
        1, 0, 0,   1, 0, 1,   1, 1, 1,   1, 1, 0,
        0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,
        0, 1, 0,   0, 1, 1,   1, 1, 1,   1, 1, 0,
        0, 0, 0,   0, 1, 0,   1, 1, 0,   1, 0, 0,
        0, 0, 1,   0, 1, 1,   1, 1, 1,   1, 0, 1
    };
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_QUADS, 0, 24);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void visit(const Octree *tree){
    if(tree->getRadius()==0){
        const Point p=tree->getCenter();
//        cout<< p[0]->x<<" "<<p[0]->y<<"  "<<p[0]->z<<"  "<<tree->getRadius()<<endl;
        glColor3f(1.0f, 0.5f, 0.5f);
        drawCube(p.x, p.y, p.z);
    }
    else{
        const Octree *t=tree;
        //paint the border of the cube
        const Point p=t->getCenter();
        drawBorder(&p, t->getRadius());
//           cout<<t->getRadius()<<endl;
        for (int i=0;i<8;i++){
            t=tree->getChild(i);
            if(t!=NULL){
                visit(t);// visit the child
            }
        }
    }
}

//initialize a list of points
static Point coor[]={
    {0, 0, 0, 1},
    {100, 100, 100, 1},
    {10, 15, 5, 1},
    {12, 50, 12, 1},
    {82, 5, 12, 1},
    {30, 60, 70, 1},
    {52, 5, 12, 1},
    {60, 60, 70, 1},
    {72, 90, 52, 1},
    {82, 50, 12, 1},
    {90, 30, 17, 1}
};
static Octree octree=Octree();
Octree* creatTree(){
    int length = sizeof(coor) / sizeof(coor[0]);
    Point *points[]={&coor[0],&coor[1],&coor[2],&coor[3],&coor[4],&coor[5],&coor[6],&coor[7],&coor[8],&coor[9],&coor[10]};
    Bounds bound=octree.calcCubicBounds(points, length);
    if(octree.build(points,length,1,4,bound,0)){
        return &octree;
    }
    exit(2);
}

//move the points randomly
void move(){
    srand((unsigned)time(0));
    Point offset;
    float random1,random2,random3;
    for(int i=2;i<sizeof(coor) / sizeof(coor[0]);i++){
        random1=rand()%10*0.1*(rand()%3-1);
        random2=rand()%10*0.1*(rand()%3-1);
        random3=rand()%10*0.1*(rand()%3-1);
        offset={random1*0.1,1*0.05,random3*0.1};
        if(coor[i].n>0)
            coor[i]=coor[i]+offset;
        else
            coor[i]=coor[i]-offset;
        while(coor[i].x>100||coor[i].y>100||coor[i].z>100||coor[i].x<0||coor[i].y<0||coor[i].z<0){
            coor[i].n*=-1;
            coor[i].x=rand()%100*1.0;
            coor[i].y=rand()%100*1.0;
            coor[i].z=rand()%100*1.0;
        }
    }
}

void display( GLFWwindow* window ){
    int running=GL_TRUE;
    double time;
    while(running&&!glfwWindowShouldClose(window)){
        // Scale to window size
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth*2, windowHeight*2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective( 45, (double)windowWidth / (double)windowHeight, 3, 500 );
        gluLookAt(120, 70, -135, 50, 50, 50, 0, 1, 0);
        
        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0,0,-5);
        move();
        creatTree();
        visit(&octree);
        // Update Screen
        glfwSwapBuffers(window);
        glfwSwapInterval(1);
        glClear( GL_COLOR_BUFFER_BIT );
        time = glfwGetTime();
        // close automatically
        if(time>10){
            running=GL_FALSE;
            exit( EXIT_SUCCESS );
        }
        glfwPollEvents();
    }
}

int main(int argc, char** argv){
    GLFWwindow* window = initWindow(1024, 620);
    if( NULL != window )
    {
        display( window );
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


