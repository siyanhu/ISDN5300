#include "MatrixStack.h"

MatrixStack::MatrixStack()
{
	// Initialize the matrix stack with the identity matrix.
	m_matrices = std::vector<Matrix4f>();
	Matrix4f id_ma;
	id_ma.identity();
	m_matrices.push_back(id_ma);
}

void MatrixStack::clear()
{
	// Revert to just containing the identity matrix.
	m_matrices.clear();
	Matrix4f id_ma;
	id_ma.identity();
	m_matrices.push_back(id_ma);
}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	Matrix4f top_elem = m_matrices[m_matrices.size() - 1];
	return top_elem;
	// return Matrix4f();
}

void MatrixStack::push( const Matrix4f& m )
{
	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m

	Matrix4f old_top = m_matrices.back();
	Matrix4f new_top = old_top * m;
	m_matrices.push_back(new_top);
	//m_matrices.push_back(m);

}

void MatrixStack::pop()
{
	// Remove the top element from the stack
	Matrix4f id_ma;
	id_ma.identity();
	m_matrices[m_matrices.size() - 1] = id_ma;
	m_matrices.erase(m_matrices.end() - 1, m_matrices.end());
}
