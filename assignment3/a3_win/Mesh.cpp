#include "Mesh.h"

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

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".

	cout << "draw mesh now..." << endl;

	//glDisable(GL_LIGHTING);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glColor4f(0.8f, 0.8f, 0.6f, 1.f);
	//glLineWidth(5);

	glBegin(GL_TRIANGLES);
	for (unsigned i = 0; i < faces.size(); i++) {
		Tuple3u surface = faces[i];

		Vector3f v1 = currentVertices[surface[0]];
		Vector3f v2 = currentVertices[surface[0]];
		Vector3f v3 = currentVertices[surface[0]];

		Vector3f n1(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
		Vector3f n2(v2[1] * v3[2] - v2[2] * v3[1], v2[2] * v3[0] - v2[0] * v3[2], v2[0] * v3[1] - v2[1] * v3[0]);
		Vector3f n3(v3[1] * v1[2] - v3[2] * v1[1], v3[2] * v1[0] - v3[0] * v1[2], v3[0] * v1[1] - v3[1] * v1[0]);

		glVertex3d(v1[0], v1[1], v1[2]);
		glNormal3d(n1[0], n1[1], n1[2]);
		glVertex3d(v2[0], v2[1], v2[2]);
		glNormal3d(n2[0], n2[1], n2[2]);
		glVertex3d(v3[0], v3[1], v3[2]);
		glNormal3d(n3[0], n3[1], n3[2]);

	}
	glEnd();
	glPopAttrib();

}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
	// Add your code here.
	ifstream in(filename);
	if (!in) {
		cerr << "ERROR: File " << filename << "Not Found." << endl;
		exit(0);
	}

	cout << "Loading Attachments File " << filename << endl;
	float a, b, c, d, e, f, g, h,
		j, k, l, m, n, o, p, q, r, s; 
	while (in >> a >> b >> c >> d >> e >> f >> g >> h >> j >> k >> l >> m >> n >> o >> p >> q >> r >> s) {
		vector<float> newline;
		newline.insert(newline.end(), {a, b, c, d, e, f, g, h, j, k, l, m, n, o, p, q, r, s});
		attachments.push_back(newline);
	}
}
