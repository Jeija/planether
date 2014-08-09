#include <fstream>
#include <string>
#include "gllibs.hpp"

#include "drawutil.hpp"
#include "splash.hpp"

void drawSplashScreen_abstract(std::string screensfile)
{
	std::ifstream file;
	file.open(texPath(screensfile).c_str());

	std::vector<std::string> splashscreens;
	std::string line;
	while (std::getline(file, line))
		splashscreens.push_back(line);

	// Choose one from the available splashscreens
	std::string splashfile = splashscreens.at(rand() % splashscreens.size());

	float width  = glutGet(GLUT_WINDOW_WIDTH);
	float height = glutGet(GLUT_WINDOW_WIDTH);
	Image2d img(splashfile, -1, 1, 2, -2);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, width, 0, height, -10, 10);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	{
		img.render();
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glFlush();
	glutSwapBuffers();
}

void drawSplashScreen()
{
	drawSplashScreen_abstract("splashscreens");
}

void drawLoseScreen()
{
	drawSplashScreen_abstract("losescreens");
}
