#include "surf.h"
#include "extra.h"
#include <windows.h>
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}


Vector3f normalise_vector(Vector3f vec) {
    GLfloat x = vec.x();
    GLfloat y = vec.y();
    GLfloat z = vec.z();
    GLfloat len = std::sqrt(x * x + y * y + z * z);

    if (len != 0.)
    {
        x /= len;
        y /= len;
        z /= len;

        Vector3f n_vec(x, y, z);
        return n_vec;
    }
    else {
        return vec;
    }
    return vec;
} 

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    vector<Tup3u> VF;
    vector<Vector3f> VV;
    vector<Vector3f> VN;
    double theta = 0.0f;
    Curve last_curve = profile;
    Curve new_curve;

    for (int div = 0; div < steps; ++div) {
        if (div > 0.0f) {
            theta = 360.0f / steps * (M_PI / 360.0f);
        }

        vector<Vector3f> temp_VV;
        vector<Vector3f> temp_VN;

        for (int i = 0; i < last_curve.size(); i++) {
            CurvePoint cp = last_curve[i];
            GLfloat sx = cp.V.x();
            GLfloat sy = cp.V.y();
            GLfloat sz = cp.V.z();

            GLfloat sx_n = cp.N.x();
            GLfloat sy_n = cp.N.y();
            GLfloat sz_n = cp.N.z();

            GLfloat new_sx = (sx * cos(theta) + sz * sin(theta));
            GLfloat new_sy = sy;
            GLfloat new_sz = (sz * cos(theta) - sx * sin(theta));
            Vector3f sV(new_sx, new_sy, new_sz);

            GLfloat new_sx_n = sx_n * cos(theta) - sz_n * sin(theta);
            GLfloat new_sy_n = sy_n;
            GLfloat new_sz_n = sx_n * sin(theta) + sz_n * cos(theta);
            Vector3f sN(new_sx_n, new_sy_n, new_sz_n);

            if (i > 0) {
                Vector3f last_old_vector(sx, sy, sz);
                Vector3f last_old_normal(sx_n, sy_n, sz_n);

                VV.push_back(last_old_vector);
                VN.push_back(last_old_normal);
                int current_tag_index = VV.size() - 1;

                VV.push_back(sV);
                VN.push_back(sN);

                Tup3u sF(current_tag_index, current_tag_index - 1, current_tag_index + 1);
                VF.push_back(sF);

                Tup3u sF_sym(current_tag_index, current_tag_index - 2, current_tag_index - 1);
                VF.push_back(sF_sym);
            }
            else {
                Vector3f last_new_vector = sV;
                Vector3f last_new_normal = sN;

                Vector3f last_old_vector(sx, sy, sz);
                Vector3f last_old_normal(sx_n, sy_n, sz_n);

                VV.push_back(last_old_vector);
                VN.push_back(last_old_normal);

                VV.push_back(last_new_vector);
                VN.push_back(last_new_normal);
            }

            temp_VV.push_back(sV);
            temp_VN.push_back(sN);

            CurvePoint newcp;
            newcp.V = sV;
            newcp.N = sN;
            new_curve.push_back(newcp);
        }

        last_curve.clear();
        last_curve = new_curve;
        new_curve.clear();
    }
    surface.VF = VF;
    surface.VN = VN;
    surface.VV = VV;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
    vector<Tup3u> VF;
    vector<Vector3f> VV;
    vector<Vector3f> VN;
    Curve last_curve = profile;
    Curve new_curve;

    for (int div = 0; div < sweep.size(); ++div) {

        CurvePoint sweep_cp = sweep[div];
        //Vector3f N = normalise_vector(sweep_cp.N);
        //Vector3f B = normalise_vector(sweep_cp.B);
        //Vector3f T = normalise_vector(sweep_cp.T);
        //Vector3f V = normalise_vector(sweep_cp.V);

        Vector3f N = sweep_cp.N;
        Vector3f B = sweep_cp.B;
        Vector3f T = sweep_cp.T;
        Vector3f V = sweep_cp.V;

        GLfloat trans[4][4] = {
            {N.x(), B.x(), T.x(), V.x()},
            {N.y(), B.y(), T.y(), V.y()},
            {N.z(), B.z(), T.z(), V.z()},
            {0.0f, 0.0f, 0.0f, 1.0}
        };

        vector<Vector3f> temp_VV;
        vector<Vector3f> temp_VN;

        for (int i = 0; i < last_curve.size(); i++) {
            CurvePoint cp = last_curve[i];
            GLfloat sx = cp.V.x();
            GLfloat sy = cp.V.y();
            GLfloat sz = cp.V.z();

            GLfloat sx_n = cp.N.x();
            GLfloat sy_n = cp.N.y();
            GLfloat sz_n = cp.N.z();

            GLfloat new_sx = sx * N.y() + sx * B.y() + sx * T.y() + V.x();
            GLfloat new_sy = sy * N.y() + sy * B.y() + sy * T.y() + V.y();
            GLfloat new_sz = sz * N.z() + sz * B.z() + sz * T.z() + V.z();
            Vector3f sV(new_sx, new_sy, new_sz);

            GLfloat new_sx_n = sx_n * N.x() + sx_n * B.x() + sx_n * T.x() + V.x();
            GLfloat new_sy_n = sy_n * N.y() + sy_n * B.y() + sy_n * T.y() + V.y();
            GLfloat new_sz_n = sz_n * N.z() + sz_n * B.z() + sz_n * T.z() + V.z();
            Vector3f sN(new_sx_n, new_sy_n, new_sz_n);

            if (i > 0) {
                Vector3f last_old_vector(sx, sy, sz);
                Vector3f last_old_normal(sx_n, sy_n, sz_n);

                VV.push_back(last_old_vector);
                VN.push_back(last_old_normal);
                int current_tag_index = VV.size() - 1;

                VV.push_back(sV);
                VN.push_back(sN);

                Tup3u sF(current_tag_index, current_tag_index - 1, current_tag_index + 1);
                VF.push_back(sF);

                Tup3u sF_sym(current_tag_index, current_tag_index - 2, current_tag_index - 1);
                VF.push_back(sF_sym);
            }
            else {
                Vector3f last_new_vector = sV;
                Vector3f last_new_normal = sN;

                Vector3f last_old_vector(sx, sy, sz);
                Vector3f last_old_normal(sx_n, sy_n, sz_n);

                VV.push_back(last_old_vector);
                VN.push_back(last_old_normal);

                VV.push_back(last_new_vector);
                VN.push_back(last_new_normal);
            }

            temp_VV.push_back(sV);
            temp_VN.push_back(sN);

            CurvePoint newcp;
            newcp.V = sV;
            newcp.N = sN;
            new_curve.push_back(newcp);
        }

        last_curve.clear();
        last_curve = new_curve;
        new_curve.clear();
    }
    surface.VF = VF;
    surface.VN = VN;
    surface.VV = VV;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.8f,0.8f,0.6f,1.f);
        glLineWidth(5);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
