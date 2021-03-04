#include "../include/Cone.h"
#include "../include/Model.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <fstream>

using namespace std;

Cone::Cone() {
    radiusBase=1;
    nStacks=10;
    nSlices=10;
    height=2;
}

Cone::Cone(int radiusG,int heightG,int slicesG,int stacksG) {
    radiusBase=radiusG;
    nStacks=stacksG;
    nSlices=slicesG;
    height=heightG;
}


void Cone::addTop(int slice,int stack,int last) { //last será um se for última slice e 0 caso contrário
    Ponto3D *top=new Ponto3D(0.0f,height,0.0f);
    static int special=1;
    Triangulo *t;
    pair<int,int> point_before(slice-last,stack);
    pair<int,int> point_now(slice*last+(1-last),stack);
    Ponto3D *second=points[point_before];
    Ponto3D *third=points[point_now]; 
    t = new Triangulo(top,third,second,new Ponto3D(special,1.0f,0.0f));
    this->faces.push_back(t);
    special=1-special;
}
void Cone::addCircle(int last,int slice,int stack) {
    Ponto3D * middle=new Ponto3D();
    pair<int,int> before(slice-last,stack),now(slice*last+(1-last),stack);
    Ponto3D* beforePoint=points[before],*nowPoint=points[now];
    Triangulo *t=new Triangulo(beforePoint,nowPoint,middle,new Ponto3D(0.0f,1.0f,1.0f));
    faces.push_back(t);
}

void Cone::addSquare(int last,int slice,int stack) {//last=0 se for última, last=1 caso contrário;
    pair<int,int> topleft(slice-last,stack-1),topright(slice*last+(1-last),stack-1),bottomleft(slice-last,stack),bottomright(slice*last+(1-last),stack);
    Ponto3D * topRight=points[topright],*topLeft=points[topleft],*bottomLeft=points[bottomleft],*bottomRight=points[bottomright];
    Triangulo *t1=new Triangulo(topRight,topLeft,bottomLeft,new Ponto3D(0.0f,0.0f,1.0f));
    Triangulo *t2=new Triangulo(topRight,bottomLeft,bottomRight,new Ponto3D(1.0f,0.0f,0.0f));
    faces.push_back(t1);
    faces.push_back(t2);
}

void Cone::generate() {
    bool first;
    double slice_angle_increment=M_PI/nSlices;
    double cone_angle=atan(height/radiusBase);
    double cone_side_increment=sqrt(pow(height,2)+pow(radiusBase,2))/(nStacks+1);

    for (int stack=0;stack<=nStacks;stack++) {
        float y=sinf(cone_angle)*cone_side_increment*stack;
        float xz=radiusBase-cosf(cone_angle)*cone_side_increment*stack;
        first=true;

        for (int slice=1;slice<=nSlices*2;slice++) {
            float x=cos(slice_angle_increment*slice)*xz;
            float z=-sinf(slice_angle_increment*slice)*xz;

            Ponto3D * ponto = new Ponto3D(x,y,z);
            pair<int,int> sliceAndStack(slice,stack);
            points[sliceAndStack]=ponto;

            if (first) {first=false;continue;}
            if (stack==0) {
                addCircle(1,slice,stack);
                if (slice==2*nSlices) {
                    addCircle(0,slice,stack);
                }
            }
            else {
                addSquare(1,slice,stack);
                if (slice==2*nSlices) {
                    addSquare(0,slice,stack);
                }
            }
            if (stack==nStacks) {
                addTop(slice,stack,1);
                if (slice==2*nSlices) {
                    addTop(slice,stack,0);
                }
            }
        }
    }
}

void Cone::saveToFile(string filename) {
    ofstream fout(filename, ios::out) ; 
    fout<< "cone\n" << to_string(nSlices) << "\n" << to_string(nStacks) << "\n" << to_string(height) << "\n";
    for (int i=1;i<=2*nSlices;i++) {
        for (int j=0;j<=nStacks;j++) {
            Ponto3D* p = points[pair<int,int>(i,j)];
            fout<< to_string(i) << " " << to_string(j) << " " << to_string(p->x) << " " << to_string(p->y) << " " << to_string(p->z) << "\n";
        }
    }
} 

void Cone::draw() {
    for (auto& t : faces) {
        t->desenha();
    }
}