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
	cout << "Loading Meshn File " << filename << endl;

	ifstream inFile;
	inFile.open(filename);

	vector<int> jointNumber;
	vector< Joint* > jointList;

	string line;
	while (getline(inFile, line)) {

		vector<string> jointString;

		stringstream s(line);
		string temp;
		while (getline(s, temp, ' ')) {
			jointString.push_back(temp);
		}

		Joint* joint = new Joint;
		joint->transform = Matrix4f::translation((float)atof(jointString[0].c_str()),
			(float)atof(jointString[1].c_str()),
			(float)atof(jointString[2].c_str())
		);
		int parentIndex = (int)atof(jointString[3].c_str());

		jointList.push_back(joint);
		jointNumber.push_back(parentIndex);
	}

	for (unsigned int i = 1; i < jointNumber.size(); i++) {
		jointList.at(jointNumber[i])->children.push_back(jointList[i]);
	}

	m_joints = jointList;
	m_rootJoint = jointList.front();
}

void SkeletalModel::getChildren(Joint* joint, MatrixStack myMatrixS) {
	myMatrixS.push(joint->transform);

	for (int i = 0; i < joint->children.size(); i++) {
		getChildren(joint->children[i], myMatrixS);
	}

	glLoadMatrixf(myMatrixS.top());
	glutSolidSphere(0.025f, 12, 12);
	myMatrixS.pop();
}

void SkeletalModel::drawJoints()
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

	getChildren(m_rootJoint, m_matrixStack);
}

void SkeletalModel::getComponents(Joint* joint, MatrixStack myMatrixS, Joint* topNode) {
	myMatrixS.push(joint->transform);

	for (int i = 0; i < joint->children.size(); i++) {
		getComponents(joint->children[i], myMatrixS, topNode);
	}

	myMatrixS.pop();	//Now referencing parent

	if (joint != topNode) {
		Matrix4f translateZ = Matrix4f(
			1, 0, 0, 0.0f,
			0, 1, 0, 0.0f,
			0, 0, 1, 0.5f,
			0, 0, 0, 1);

		Vector4f transCol = joint->transform.getCol(3);
		float distL = Vector3f(transCol[0], transCol[1], transCol[2]).abs();

		Matrix4f scaleMat = Matrix4f(
			.05f, 0, 0, 0,
			0, .05f, 0, 0,
			0, 0, distL, 0,
			0, 0, 0, 1);

		Vector3f rnd = Vector3f(0, 0, 1);

		Vector3f parentOffset = 1 * Vector3f(1 * transCol[0], 1 * transCol[1], 1 * transCol[2]);

		Vector3f z = parentOffset.normalized();
		Vector3f y = Vector3f::cross(z, rnd).normalized();
		Vector3f x = Vector3f::cross(y, z).normalized();

		Matrix4f coordReset = Matrix4f(x[0], y[0], z[0], 0.0f,
			x[1], y[1], z[1], 0.0f,
			x[2], y[2], z[2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		myMatrixS.push(coordReset);
		myMatrixS.push(scaleMat);
		myMatrixS.push(translateZ);

		glLoadMatrixf(myMatrixS.top());
		glutSolidCube(1.0f);

		//Pop matrices that should come off stack when traversing through tree
		myMatrixS.pop();
		myMatrixS.pop();
		myMatrixS.pop();

	}
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy
	getComponents(m_rootJoint, m_matrixStack, m_rootJoint);
	
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
	Matrix4f Xrot = Matrix4f::rotateX(rX);
	Matrix4f Yrot = Matrix4f::rotateY(rY);
	Matrix4f Zrot = Matrix4f::rotateZ(rZ);

	Matrix4f overallRotate = Xrot * Yrot * Zrot;
	Matrix3f upperLeftR = overallRotate.getSubmatrix3x3(0, 0);

	//Set current transform rotation matrix to rotation portion of overall rotation matrix
	m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, upperLeftR);
}

void SkeletalModel::recurseJointBind(Joint* joint, MatrixStack myStack, int flag) {
	
	myStack.push(joint->transform);

	//flag == 0: Bind; flag == 1: Current
	if (flag == 0) {
		//cout << endl << endl << "Recusing bindWorldToJointTransform " << endl;
		joint->bindWorldToJointTransform = myStack.top().inverse();
	}
	else {
		//cout << endl << endl << "Recusing currentJointToWorldTransform " << endl;
		joint->currentJointToWorldTransform = myStack.top();
	}
	

	for (int i = 0; i < joint->children.size(); i++) {
		recurseJointBind(joint->children[i], myStack, flag);
	}
	myStack.pop();
	
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

	m_matrixStack.clear();
	recurseJointBind(m_rootJoint, m_matrixStack, 0);

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
	m_matrixStack.clear();
	recurseJointBind(m_rootJoint, m_matrixStack, 1);
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.

	vector <Vector3f> newVertices;

	for (int v = 0; v < m_mesh.currentVertices.size(); v++) {
		Vector4f sumPoints = Vector4f();

		for (int a = 0; a < m_mesh.attachments[v].size(); a++) {
			sumPoints = sumPoints + m_mesh.attachments[v][a] *
				(m_joints[a + 1]->currentJointToWorldTransform *
					m_joints[a + 1]->bindWorldToJointTransform *
					Vector4f(m_mesh.bindVertices[v][0], m_mesh.bindVertices[v][1],
						m_mesh.bindVertices[v][2], 1.0f));

		}
		newVertices.push_back(Vector3f(sumPoints[0], sumPoints[1], sumPoints[2]));

	}

	m_mesh.currentVertices = newVertices;
}

