#include "Mesh.h"
#include <random>

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
	ifstream in(filename);
	if (!in) {
		cerr << "ERROR: File " << filename << "Not Found." << endl;
		exit(0);
	}

	cout << "Loading Meshn File " << filename << endl;
	string a;
	GLfloat b, c, d;
	while (in >> a >> b >> c >> d) {
		if (a == "v") {
			Vector3f vector(b, c, d);
			bindVertices.push_back(vector);
		}
		else if (a == "f") {
			Tuple3u sF(static_cast<int>(b), static_cast<int>(c), static_cast<int>(d));
			faces.push_back(sF);
		}
	}

	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

float get_a_random_indicator(void) {
	random_device device;
	mt19937 random_num(device());
	uniform_int_distribution<mt19937::result_type> dist_color(0, 255);
	int int_indicator = dist_color(random_num);
	//(x - min) / (max - min)
	float normalised_indicator = (float)int_indicator / 255.0;
	return normalised_indicator;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".

	cout << "draw mesh now..." << endl;
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	vector<double> colorV;
	for (unsigned v = 0; v < currentVertices.size(); v++)
	{
		double sumC = 0.0;
		for (int a = 0; a < attachments[v].size(); a++) {
			sumC = sumC + (double)(attachments[v][a] * (a + 1));
		}
		colorV.push_back(floor(sumC + 0.5) - 1);    //produce 0-16 number

	}
	//==========================================================

	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < faces.size(); i++)
	{

		double c_1 = colorV.at(faces[i][0] - 1);
		double c_2 = colorV.at(faces[i][0] - 1);
		double c_3 = colorV.at(faces[i][0] - 1);

		double* c1 = &c_1;		//Pass by Reference
		double* c2 = &c_2;
		double* c3 = &c_3;

		Vector3f t1 = currentVertices[faces[i][0] - 1];
		Vector3f t2 = currentVertices[faces[i][1] - 1];
		Vector3f t3 = currentVertices[faces[i][2] - 1];

		Vector3f v1 = t2 - t1;
		Vector3f v2 = t3 - t1;

		Vector3f n1 = Vector3f::cross(v1, v2);
		n1 = n1.normalized();

		glNormal3f(n1[0], n1[1], n1[2]);
		float color_indicator = get_a_random_indicator();
		glColor3f(color_indicator, color_indicator, 0.0);
		glVertex3f(t1[0], t1[1], t1[2]);
		glColor3f(0.0, color_indicator, 0.0);
		glVertex3f(t3[0], t3[1], t3[2]);
		glColor3f(0.0, 0.0, color_indicator);
		glVertex3f(t2[0], t2[1], t2[2]);

	}
	glEnd();

}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
	// Add your code here.

	ifstream inFile;
	inFile.open(filename);

	string line;
	while (getline(inFile, line)) {

		vector<float> attachString;

		stringstream s(line);
		string temp;
		int count = 0;

		while (getline(s, temp, ' ') && count < 17) {
			count = count + 1;
			attachString.push_back((float)atof(temp.c_str()));

		}

		attachments.push_back(attachString);
	}

}
