#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

// SIYAN defined Functions
bool file_exist(const char* fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)
	cout << "SkeletalModel::draw" << endl;

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);
	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton(const char* filename)
{
	// Load the skeleton from file here.
	if (file_exist(filename) == 0) {
		cerr << "ERROR: File Does Not Exist." << endl;
		exit(0);
	}

	ifstream in(filename);
	if (!in) {
		cerr << "ERROR: File " << filename << "Not Found." << endl;
		exit(0);
	}

	cout << "Loading Skeleteon File" << filename << endl;
	GLfloat a, b, c;
	int d;
	vector<Vector4f> readings;
	while (in >> a >> b >> c >> d) {
		Vector4f vec(a, b, c, d);
		readings.push_back(vec);
	}

	for (unsigned i = 0; i < readings.size(); ++i) {
		Vector4f vec = readings[i];
		Joint* newJ = new Joint;
		newJ->transform = Matrix4f(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			a, b, c, 0);
		m_joints.push_back(newJ);
	}

	for (unsigned i = 0; i < m_joints.size(); ++i) {
		Joint* currentJ = m_joints[i];
		Vector4f vec = readings[i];
		d = vec[3];

		if (d == -1) {
			m_rootJoint = currentJ;
			m_joints[i] = currentJ;
		}
		else {
			Joint* parentJ = m_joints[d];
			parentJ->children.push_back(currentJ);
			m_joints[i] = currentJ;
			m_joints[d] = parentJ;
		}
	}
}

void SkeletalModel::getChildren(const Joint* parent) {
	Matrix4f parent_trans = parent->transform;
	m_matrixStack.push(parent_trans);
	glLoadMatrixf(m_matrixStack.top());

	unsigned counter = 1;
	for (int i = 0; i < parent->children.size(); ++i) {
		if (nullptr != parent->children[i]) {
			Joint* kid = parent->children[i];
			if (kid->children.size() > 0) {
				getChildren(kid);
			}
		}
	}

	glLoadMatrixf(m_matrixStack.top());
	m_matrixStack.pop();
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.

	Joint* current_node = m_rootJoint;
	Matrix4f current_trans = m_rootJoint->transform;

	m_matrixStack.push(current_trans);
	glutSolidSphere(0.025f, 12, 12);
	glLoadMatrixf(m_matrixStack.top());

	getChildren(m_rootJoint);

	m_matrixStack.pop();

	glEnd();
	
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy

	glBegin(GL_POLYGON);
	glutSolidCube(1.0f);

	//GLfloat new_sx_n = sx_n * cos(theta) - sz_n * sin(theta);
	//GLfloat new_sy_n = sy_n;
	//GLfloat new_sz_n = sx_n * sin(theta) + sz_n * cos(theta);


	
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
}

