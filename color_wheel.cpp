#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "yet_another_list.h"
#define PI 3.1415926535898

#define GET_RED(ANGLE) get_function_by_angle(ANGLE)(ANGLE)
#define GET_GREEN(ANGLE) get_function_by_angle(ANGLE - 2 * PI/3)(ANGLE)
#define GET_BLUE(ANGLE) get_function_by_angle(ANGLE - 4 * PI/3)(ANGLE)
#define GET_COLOR(HUE, VALUE, SATURATION) (max(HUE, (1 - SATURATION)) * VALUE)
#define SET_RGB(R, G, B, ANGLE, VALUE, SATURATION)\
                R = GET_COLOR(GET_RED(ANGLE), VALUE, SATURATION);\
                G = GET_COLOR(GET_GREEN(ANGLE), VALUE, SATURATION);\
                B = GET_COLOR(GET_BLUE(ANGLE), VALUE, SATURATION)

#define TRANSFORM_X (double)(((x - (double)(glutGet(GLUT_WINDOW_WIDTH)/2))) / PIX_COORD_SCALE) // mouse coordinate to Cartesian coordinate
#define TRANSFORM_Y -((double)(((y - (double)(glutGet(GLUT_WINDOW_HEIGHT)/2))) / PIX_COORD_SCALE)) // mouse coordinate to Cartesian coordinate
#define initialWindowWidth 640

typedef struct CL_POINT{
    double x;
    double y;
    double r;
    double g;
    double b;
}CL_POINT;

double PIX_COORD_SCALE = initialWindowWidth / 20;

int rund = 100;
double hue[3], value = 1, satur = 1, siz = 10, thik = 2,
       tempHUE[3] = {};

/*****************************************************************************************/
//Equation by parts.
YALIST* function_wheel = new_yalist();

double get_color_full(double angle){
    return 1;
}

double get_color_fraction(double angle){//Do not touch
    return (cos(3 * angle - PI) + 1)/2;
}

double get_color_none(double angle){
    return 0;
}

//HAHAHAHAHAHAHAHAHA, HAHAHAH. TRY TO FIGURE THIS ONE OUT!
double (*get_function_by_angle(double ang))(double){//more or less OK, don't push it tho
    ang = fabs(ang);
    int index = (double)(ang/(2*PI))*(double)function_wheel->length;
    return (double (*)(double))yalget(function_wheel, index);
}
/*****************************************************************************************/
//Utility
//![0 -- 1] real random
double simpleRand(void){
    return (double)(rand()%1000)/1000;
}

void toClipboard(char* st){
    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, strlen(st) + 1);
    if(!hg){
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), st, strlen(st));
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}

//!float RGB to 0x######
char* get_hex_color(double r, double g, double b){
  int rbyte = r*255, gbyte = g*255, bbyte = b*255;
  char rhex[3], ghex[3], bhex[3];
  itoa(rbyte, rhex, 16);
    if(strlen(rhex) < 2) sprintf(rhex, "%s%s", "0", rhex);
  itoa(gbyte, ghex, 16);
    if(strlen(ghex) < 2) sprintf(ghex, "%s%s", "0", ghex);
  itoa(bbyte, bhex, 16);
    if(strlen(bhex) < 2) sprintf(bhex, "%s%s", "0", bhex);
  char* out = (char*)malloc(7);
  sprintf(out, "%s%s%s", rhex, ghex, bhex);
  return out;
}

void printColor(void)
{
    char* colorHex = get_hex_color(GET_COLOR(hue[0], value, satur),
                                   GET_COLOR(hue[1], value, satur),
                                   GET_COLOR(hue[2], value, satur));
    printf("%s\n", colorHex);
    toClipboard(colorHex);//!Optional.
    free(colorHex);
    memccpy(tempHUE, hue, 3, sizeof(double));
    return;
}

void simple_quad(CL_POINT start, CL_POINT finish){
    glPushMatrix();
      glBegin(GL_QUADS);
          glColor3f(start.r, start.g, start.b);
          glVertex2f(start.x, start.y);               //
          glVertex2f(finish.x, start.y);              //
          glColor3f(finish.r, finish.g, finish.b);
          glVertex2f(finish.x, finish.y);             //
          glVertex2f(start.x, finish.y);              //
      glEnd();
    glPopMatrix();
}

void wheel(double thi, double radius, double offset_x, double offset_y, int roundness){//"legacy"
  int i, j=0;
  double r, g, b;
      for(i = 0; i < roundness+2; i++){
        r = cos(i*((2*PI/roundness)) - PI/2); r = (r > 0) * r;
        g = cos(i*((2*PI/roundness)) - 7*PI/6); g = (g > 0) * g;
        b = cos(i*((2*PI/roundness)) - 11*PI/6); b = (b > 0) * b;
        glColor3d(r, g, b);
        glVertex2d(cos(i*PI/(roundness/2)) * (radius+thi*j) + offset_x,
                   sin(i*PI/(roundness/2)) * (radius+thi*j) + offset_y);
        j = !j;
      }
}

void color_wheel(double rad, double thi, double off_x, double off_y, int steps)
{
    int i, j=0;
    double r, g, b, ang, mult;
      for(i = 0; i < steps+2; i++){
        ang = i * PI/(steps/2); mult = rad + thi * j;
        SET_RGB(r, g, b, ang, value, satur);
        glColor3d(r, g, b);
        //>For polar-------(<r|g|b>)
        glVertex2d(cos(ang) * mult + off_x,
                   sin(ang) * mult + off_y);
        j = !j;
      }
}
void inverse_wheel(double rad, double thi, double off_x, double off_y, int steps)
{
    int i, j=0;
    double r, g, b, ang;
      for(i = 0; i < steps+2; i++){
        ang = i*PI/(steps/2);
        r = 1 - GET_COLOR(hue[0], value, satur);
        g = 1 - GET_COLOR(hue[1], value, satur);
        b = 1 - GET_COLOR(hue[2], value, satur);

        glColor3d(r, g, b);
        glVertex2d(cos(ang) * (rad+thi*j) + off_x,
                   sin(ang) * (rad+thi*j) + off_y);
        j = !j;
      }
}

void value_slider(double height)
{
    simple_quad((CL_POINT){-17.5, height,
                            max(hue[0], 1 - satur),
                            max(hue[1], 1 - satur),
                            max(hue[2], 1 - satur)},
                (CL_POINT){-19, -height,
                            0,      0,      0});
}
void satur_slider(double height)
{
    simple_quad((CL_POINT){17.5, height,
                            hue[0] * value, hue[1] * value, hue[2] * value},
                (CL_POINT){19, -height,
                            1,              1,              1});
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3d(1,0,0);

    glBegin(GL_TRIANGLE_STRIP);
      //wheel(2, 2, 0, 0, rund);//segunda feira é dia de reinventar a roda.
      color_wheel(siz, thik, 0, 0, rund);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
      inverse_wheel(siz/5, thik/2, 0, 0, rund);
    glEnd();

    value_slider(siz+thik);
    satur_slider(siz+thik);

    glLoadIdentity();
      gluOrtho2D(-20, 20, -(double)glutGet(GLUT_WINDOW_HEIGHT)/(double)glutGet(GLUT_WINDOW_WIDTH)*20, (double)glutGet(GLUT_WINDOW_HEIGHT)/(double)glutGet(GLUT_WINDOW_WIDTH)*20);

    glutSwapBuffers();
}


void setBG(void)
{
    glClearColor( GET_COLOR(hue[0], value, satur),
                  GET_COLOR(hue[1], value, satur),
                  GET_COLOR(hue[2], value, satur),0);
}
void pickHue(double x, double y)
{
    double incl, angle;
    if(y == 0)
      {
        if(x != 0)//horizontal
          angle = (x > 0)? 0: PI;
        else//center
          {
            hue[0] = 0;
            hue[1] = 0;
            hue[2] = 0;
            glutPostRedisplay();
            return;
          }
      }
    else if(x == 0)//vertical
      angle = (y > 0)? PI/2: 3*PI/2;
    else{//any other
      incl = y/x;
      angle = atan(incl);
      if(x < 0)
        angle += PI;
    }

    hue[0] = GET_RED(angle);
    hue[1] = GET_GREEN(angle);
    hue[2] = GET_BLUE(angle);
}
double pickBar(double x, double range)//pick value on range, centered.
{
    return (abs(x) >= range)? (x > 0): ((x / range) + 1)/2;//one-liner ;P
}
void pick(int x, int y)//btw it's matiz in brazillian.
{
    PIX_COORD_SCALE = glutGet(GLUT_WINDOW_WIDTH)/39.999;
    double rel_x = TRANSFORM_X, rel_y = TRANSFORM_Y;

    if((rel_x <= -17.5) && (rel_x >= -19) && (abs(rel_y) <= (siz + thik + 0.25)))//shorten?
      value = pickBar(rel_y, siz + thik);
    else if((rel_x >= 17.5) && (rel_x <= 19) && (abs(rel_y) <= (siz + thik + 0.25)))
      satur = pickBar(rel_y, siz + thik);
    else if(sqrt((rel_x*rel_x) + (rel_y*rel_y)) <= (siz/5))
      return printColor();
    else
      pickHue(rel_x, rel_y);//too specific to generalize.

    setBG();

    //printf("Angle = [%lf]deg\n", angle/PI*180);

    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '-':
            rund--;
            break;
        case '+':
            rund++;
            break;
        case 'r':
            pick((int)(simpleRand()*(double)glutGet(GLUT_WINDOW_WIDTH)), (int)(simpleRand()*(double)glutGet(GLUT_WINDOW_HEIGHT)));//hold r :)
            glutPostRedisplay();
            return;
        case ' ' :
        case '\r':
            return printColor();
        case 27 :
        case 'q':
            yalclear(function_wheel);
            free(function_wheel);
            exit(0);
            break;
    }
    pick(x, y);//optional here, but welcome.
    glutPostRedisplay();
}

static void mou(int button, int state, int x, int y){
  if(state == GLUT_DOWN)
    pick(x, y);
}

static void mot(int x, int y){
  pick(x, y);
}

static void idle(void)
{
    glutPostRedisplay();
}

void registerPolarFunctions(void){
    yaladd(function_wheel, (void*)get_color_full,      0);//HECK YE
    yaladd(function_wheel, (void*)get_color_fraction, -1);//enqueue from here, I'm just not sure "yet another list's" enqueue is OK
    yaladd(function_wheel, (void*)get_color_none,     -1);
    yaladd(function_wheel, (void*)get_color_none,     -1);
    yaladd(function_wheel, (void*)get_color_fraction, -1);
    yaladd(function_wheel, (void*)get_color_full,     -1);
}

int main(int argc, char *argv[])
{
    registerPolarFunctions();

    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("Color wheel");

    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    glutMouseFunc(mou);
    glutMotionFunc(mot);
    //glutPassiveMotionFunc(mot);//for playing around

    glClearColor(1,1,1,1);

    glutMainLoop();

    return EXIT_SUCCESS;
}
