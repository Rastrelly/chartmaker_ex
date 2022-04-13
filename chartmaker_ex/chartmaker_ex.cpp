#include <iostream>
#include <thread>
#include <GL/freeglut.h>
#include <vector>
#include <string>

using namespace std;

struct point { double x, y; };
struct chclr { double r, g, b; } gcol = {1,0,0};

class vecholder
{
public:
	vector < point > vec;
	chclr ccol;
	double wid;
	vecholder()
	{
		vec = {};
		ccol = {1,0,0};
		wid = 1;
	}
};


bool needresfresh=false;
bool canrefresh=false;
bool needexit = false;
bool canexit = false;
int ww=800, wh=600;
double xmin = -10, ymin = 0, xmax = 10, ymax = 0;
double stp = 100;
double chw = 5;
double cha = 2, chb = -4, chc = 0, chd = 0;
int cht = 0;

vector<vecholder> vecs = {};

vector<point> coords = {};

//попередні виклики
void cb_idle();
void cb_reshape(int wx, int wy);
void cb_render();

//математика
double func(int tp, double x, double a, double b, double c, double d)
{
	if (tp == 0)
		return a * pow(x, 2) + b*x + c;
	if (tp == 1)
		return a * sin(b*x);
	if (tp == 2)
		return a * pow(x, 3) + b * pow(x, 2) + c * x + d;
}

void storevector(int n)
{
	vecs[n].vec.clear();
	for (int i = 0; i < coords.size(); i++)
	{
		vecs[n].vec.push_back(coords[i]);
		vecs[n].ccol = gcol;
		vecs[n].wid = chw;
	}
}

void gendata()
{

	canrefresh = false;

	double sw = xmax - xmin;
	double d = sw / stp;
	coords.clear();
	for (int i = 0; i <= stp; i++)
	{
		double cx = xmin + (double)i*d;
		double cy = func(cht, cx, cha, chb, chc, chd);
		coords.push_back({cx, cy});
		if (i == 0) { ymin = cy; ymax = cy; }
		else
		{
			if (cy < ymin) ymin = cy;
			if (cy > ymax) ymax = cy;
		}
	}

	canrefresh = true;
	needresfresh = true;

}

//колбеки потоку
void runGLUTthread(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(ww, wh);
	glutCreateWindow("Chartmaker");

	glutIdleFunc(cb_idle);
	glutReshapeFunc(cb_reshape);
	glutDisplayFunc(cb_render);

	glClearColor(0,0,0,1);

	glutMainLoop();
		
}

//колбеки GLUT
void cb_idle()
{
	if (needresfresh)
	{
		needresfresh = false;
		cb_render();		
	}
	if (needexit)
	{
		printf("Exiting main loop...\n");
		needexit = false;
		glutLeaveMainLoop();		
		canexit = true;
	}
}

void cb_reshape(int wx, int wy)
{
	glViewport(0,0,wx,wy);
	needresfresh = true;
}

void cb_render()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (canrefresh)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(xmin, xmax, ymin, ymax);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//система координат
		glColor3d(1,1,1);
		glLineWidth(1);

		glBegin(GL_LINES);

		glVertex2d(xmin, 0);
		glVertex2d(xmax, 0);
		glVertex2d(0, ymin);
		glVertex2d(0, ymax);

		glEnd();

		vector<point> dtv = {};
		int sz = 0;

		//графік
		for (int j = 0; j <= vecs.size(); j++)
		{
			cout << "Iteration " << j << endl;
			if (j == vecs.size())
			{
				sz = coords.size();
				for (int i = 0; i < sz; i++)
				{
					if (sz == coords.size())
						dtv.push_back(coords[i]);
				}
				
				glColor3d(gcol.r, gcol.g, gcol.b);
				glLineWidth((float)chw);
			}
			else
			{
				dtv.clear();
				for (int i = 0; i < vecs[j].vec.size(); i++)
				{
					dtv.push_back(vecs[j].vec[i]);
				}
				glColor3d(vecs[j].ccol.r, vecs[j].ccol.g, vecs[j].ccol.b);
				glLineWidth(vecs[j].wid);
			}
					   
			sz = dtv.size();

			glBegin(GL_LINE_STRIP);

			for (int i = 0; i < sz; i++)
			{
				glVertex2d(dtv[i].x, dtv[i].y);
			}

			glEnd();
		}
	}

	glutSwapBuffers();

}


void outpucurrentstats()
{
	printf("-------- + --------\n");
	printf("Current stats: xmin = %f, xmax = %f, color = %f, %f, %f\n",xmin,xmax,gcol.r,gcol.g,gcol.b);
	printf("line width = %f, steps no = %f, a = %f, b = %f, c = %f, d = %f\n", chw,stp,cha,chb,chc,chd);
	printf("chart type = %d\nvectors available=%d\n", cht,vecs.size());
	printf("Select operation:\n");
	printf("1 - Input xmin\n2 - input xmax\n3 - regen data\n4 - exit\n");
	printf("5 - chart color\n6 - chart thickness\n7 - chart coefficients\n");
	printf("8 - number of steps\n9 - chart type\n10 - add new vector storage\n11 - store current vector\n");
	if (!canrefresh)
	{
		printf("\nUse data regen to show the chart\n");
	}
}

//головна функція
int main(int argc, char **argv)
{
	thread glthread(runGLUTthread, argc, argv);
	glthread.detach();

	while (true)
	{
		outpucurrentstats();
		
		string k="";
		getline(cin, k);
		if (k == "4")
		{
			needexit = true;
			break;
		}
		if (k == "1")
		{
			cout << "Input xmin:\n";
			cin >> xmin;
			needresfresh = true;
		}
		if (k == "2")
		{
			cout << "Input xmax:\n";
			cin >> xmax;
			needresfresh = true;
		}
		if (k == "3")
		{
			cout << "Generating data...\n";
			gendata();
			cout << "Generation complete\n";
			needresfresh = true;
		}
		if (k == "5")
		{
			cout << "Input r g b:\n";
			cin >> gcol.r >> gcol.g >> gcol.b;
			needresfresh = true;
		}
		if (k == "6")
		{
			cout << "Input chart line thickness:\n";
			cin >> chw;
			needresfresh = true;
		}
		if (k == "7")
		{
			cout << "Input a b c d:\n";
			cin >> cha >> chb >> chc >> chd;
			needresfresh = true;
		}
		if (k == "8")
		{
			cout << "Input number of steps:\n";
			cin >> stp;
			if (stp < 1) stp = 1;
			needresfresh = true;
		}
		if (k == "9")
		{
			cout << "Select chart type:\n 0 - parabola; 1 - sinusoid; 2 - cubic parabola";
			cin >> cht;
			if (cht < 0) cht = 0;
			if (cht > 2) cht = 2;
			needresfresh = true;
		}
		if (k == "10")
		{
			cout << "Adding a new vector of current type\n";
			vecholder cv = vecholder();
			vecs.push_back(cv);
		}
		if (k == "11")
		{
			cout << "Store current vector to vecholder #\n";
			int vcn = 0;
			cin >> vcn;
			storevector(vcn);
		}
	}

	while (!canexit) {};

	return 0;
}

