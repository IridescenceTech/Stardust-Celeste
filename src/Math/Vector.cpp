#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>

namespace Stardust_Celeste::Math {


    Vector3<float> multMatrix(Vector3<float> v, Matrix transform) {
        return Vector3<float>(
                transform.rows[0].x * v.x + transform.rows[0].y * v.y + transform.rows[0].z * v.z + transform.rows[0].w,
                transform.rows[1].x * v.x + transform.rows[1].y * v.y + transform.rows[1].z * v.z + transform.rows[1].w,
                transform.rows[2].x * v.x + transform.rows[2].y * v.y + transform.rows[2].z * v.z + transform.rows[2].w
        );
    }


    Vector4<float> multMatrix(Vector4<float> v, Matrix transform)
    {
        return Vector4<float>(
                transform.rows[0].x * v.x + transform.rows[0].y * v.y + transform.rows[0].z * v.z + transform.rows[0].w * v.w,
                transform.rows[1].x * v.x + transform.rows[1].y * v.y + transform.rows[1].z * v.z + transform.rows[1].w * v.w,
                transform.rows[2].x * v.x + transform.rows[2].y * v.y + transform.rows[2].z * v.z + transform.rows[2].w * v.w,
                transform.rows[3].x * v.x + transform.rows[3].y * v.y + transform.rows[3].z * v.z + transform.rows[3].w * v.w
        );
    }

}