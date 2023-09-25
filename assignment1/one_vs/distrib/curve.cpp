#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
 

Vector3f normalise_vectorr(Vector3f vec) {
    return vec;
    //GLfloat x = vec.x();
    //GLfloat y = vec.y();
    //GLfloat z = vec.z();
    //GLfloat len = std::sqrt(x * x + y * y + z * z);

    //if (len != 0.)
    //{
    //    x /= len;
    //    y /= len;
    //    z /= len;

    //    Vector3f n_vec(x, y, z);
    //    return n_vec;
    //}
    //else {
    //    return vec;
    //}
}

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }


    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    Vector3f P0 = P[0];
    Vector3f P1 = P[1];
    Vector3f P2 = P[2];
    Vector3f P3 = P[3];

    vector<CurvePoint> curve_points;

    for (int div = 0; div < steps; ++div) {

        if (div == 0.0) {
            continue;
        }

        GLfloat t = (GLfloat)div / (GLfloat)steps;
        GLfloat inv = (1.0 - t);

        GLfloat B0 = inv * inv * inv;
        GLfloat B1 = 3.0 * inv * inv * t;
        GLfloat B2 = 3.0 * inv * t * t;
        GLfloat B3 = t * t * t;

        GLfloat x = P0[0] * B0 + P1[0] * B1 + P2[0] * B2 + P3[0] * B3;
        GLfloat y = P0[1] * B0 + P1[1] * B1 + P2[1] * B2 + P3[1] * B3;
        GLfloat z = P0[2] * B0 + P1[2] * B1 + P2[2] * B2 + P3[2] * B3;
        Vector3f V (x, y, z);

        GLfloat B0_p = -3.0 * inv * inv;
        GLfloat B1_p = 3.0 - 12.0 * t + 9.0 * t * t;
        GLfloat B2_p = 6.0 * t - 9.0 * t * t;
        GLfloat B3_p = 3.0 * t * t;

        GLfloat x_p = P0[0] * B0_p + P1[0] * B1_p + P2[0] * B2_p + P3[0] * B3_p;
        GLfloat y_p = P0[1] * B0_p + P1[1] * B1_p + P2[1] * B2_p + P3[1] * B3_p;
        GLfloat z_p = P0[2] * B0_p + P1[2] * B1_p + P2[2] * B2_p + P3[2] * B3_p;
        Vector3f T (x_p, y_p, z_p);

        GLfloat B0_pp = 6.0 * inv;
        GLfloat B1_pp = -12.0 * inv + 6.0 * inv;
        GLfloat B2_pp = 6.0 * inv - 12.0 * t;
        GLfloat B3_pp = 6.0 * t;

        GLfloat x_pp = P0[0] * B0_pp + P1[0] * B1_pp + P2[0] * B2_pp + P3[0] * B3_pp;
        GLfloat y_pp = P0[1] * B0_pp + P1[1] * B1_pp + P2[1] * B2_pp + P3[1] * B3_pp;
        GLfloat z_pp = P0[2] * B0_pp + P1[2] * B1_pp + P2[2] * B2_pp + P3[2] * B3_pp;
        Vector3f N (x_pp, y_pp, z_pp);

        GLfloat B0_ppp = -6.0;
        GLfloat B1_ppp = 18.0;
        GLfloat B2_ppp = -18.0;
        GLfloat B3_ppp = 6.0;

        GLfloat x_ppp = P0[0] * B0_ppp + P1[0] * B1_ppp + P2[0] * B2_ppp + P3[0] * B3_ppp;
        GLfloat y_ppp = P0[1] * B0_ppp + P1[1] * B1_ppp + P2[1] * B2_ppp + P3[1] * B3_ppp;
        GLfloat z_ppp = P0[2] * B0_ppp + P1[2] * B1_ppp + P2[2] * B2_ppp + P3[2] * B3_ppp;
        Vector3f B (x_ppp, y_ppp, z_ppp);
        
        CurvePoint cp;
        cp.V = normalise_vectorr(V);
        cp.N = normalise_vectorr(N);
        cp.T = normalise_vectorr(T);
        cp.B = normalise_vectorr(B);

        curve_points.push_back(cp);

    }
    return curve_points;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    // Return an empty curve right now.
    Vector3f P0 = P[0];
    Vector3f P1 = P[1];
    Vector3f P2 = P[2];
    Vector3f P3 = P[3];

    vector<CurvePoint> curve_points;

    for (int div = 0; div < steps; ++div) {

        if (div == 0.0) {
            continue;
        }

        GLfloat t = (GLfloat)div / (GLfloat)steps;
        GLfloat inv = (1.0 - t);

        GLfloat B0 = 1.0 / 6.0 * inv * inv * inv;
        GLfloat B1 = 1.0 / 6.0 * (3.0 * t * t * t - 6.0 * t * t + 4.0);
        GLfloat B2 = 1.0 / 6.0 * (-3.0 * t * t * t + 3 * t * t + 3 * t + 1);
        GLfloat B3 = 1.0 / 6.0 * t * t * t;

        GLfloat x = P0[0] * B0 + P1[0] * B1 + P2[0] * B2 + P3[0] * B3;
        GLfloat y = P0[1] * B0 + P1[1] * B1 + P2[1] * B2 + P3[1] * B3;
        GLfloat z = P0[2] * B0 + P1[2] * B1 + P2[2] * B2 + P3[2] * B3;
        Vector3f V(x, y, z);

        GLfloat B0_p = -0.5 * t * t + t - 0.5;
        GLfloat B1_p =1.5 * t * t - 2.0 * t;
        GLfloat B2_p = -1.5 * t * t - t - 2.0 / 3.0;
        GLfloat B3_p = 0.5 * t * t;

        GLfloat x_p = P0[0] * B0_p + P1[0] * B1_p + P2[0] * B2_p + P3[0] * B3_p;
        GLfloat y_p = P0[1] * B0_p + P1[1] * B1_p + P2[1] * B2_p + P3[1] * B3_p;
        GLfloat z_p = P0[2] * B0_p + P1[2] * B1_p + P2[2] * B2_p + P3[2] * B3_p;
        Vector3f T(x_p, y_p, z_p);

        GLfloat B0_pp = 1.0 - t;
        GLfloat B1_pp = 3.0 * t - 1;
        GLfloat B2_pp = - 3.0 * t - 1;
        GLfloat B3_pp = t;

        GLfloat x_pp = P0[0] * B0_pp + P1[0] * B1_pp + P2[0] * B2_pp + P3[0] * B3_pp;
        GLfloat y_pp = P0[1] * B0_pp + P1[1] * B1_pp + P2[1] * B2_pp + P3[1] * B3_pp;
        GLfloat z_pp = P0[2] * B0_pp + P1[2] * B1_pp + P2[2] * B2_pp + P3[2] * B3_pp;
        Vector3f N(x_pp, y_pp, z_pp);

        GLfloat B0_ppp = -1.0;
        GLfloat B1_ppp = 3.0;
        GLfloat B2_ppp = - 3.0;
        GLfloat B3_ppp = 1.0;

        GLfloat x_ppp = P0[0] * B0_ppp + P1[0] * B1_ppp + P2[0] * B2_ppp + P3[0] * B3_ppp;
        GLfloat y_ppp = P0[1] * B0_ppp + P1[1] * B1_ppp + P2[1] * B2_ppp + P3[1] * B3_ppp;
        GLfloat z_ppp = P0[2] * B0_ppp + P1[2] * B1_ppp + P2[2] * B2_ppp + P3[2] * B3_ppp;
        Vector3f B(x_ppp, y_ppp, z_ppp);

        CurvePoint cp;
        cp.V = V;
        cp.N = N;
        cp.T = T;
        cp.B = B;

        curve_points.push_back(cp);

    }
    return curve_points;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

