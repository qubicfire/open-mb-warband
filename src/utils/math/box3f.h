#ifndef _BOX3F_H
#define _BOX3F_H

#include <glm/ext/vector_float3.hpp>

struct Box3f
{
    glm::vec3 min;
    glm::vec3 max;

    inline Box3f() noexcept
        : min(glm::vec3(1.0f, 1.0f, 1.0f))
        , max(glm::vec3(-1.0f, -1.0f, -1.0f))
    { }

    inline Box3f(const Box3f& box) noexcept
        : min(box.min)
        , max(box.max)
    { }

    inline Box3f(const glm::vec3& min, const glm::vec3& max) 
        : min(min)
        , max(max)
    { }

    inline Box3f(const glm::vec3& center, const float radius)
        : min(center - glm::vec3(radius))
        , max(center + glm::vec3(radius))
    { }

    inline ~Box3f() = default;

    inline bool operator==(Box3f const& p) const
    {
        return min == p.min && max == p.max;
    }
    inline bool operator!=(Box3f const& p) const
    {
        return min != p.min || max != p.max;
    }
    /** Offset of a vector (s,s,s)
    */
    void offset(const float scalar)
    {
        offset(glm::vec3(scalar));
    }
    /** Offset the two corner of the box of a vector delta.
        *  adding delta to max and -delta to min.
        @param delta offset vector
    */
    void offset(const glm::vec3& delta)
    {
        min -= delta;
        max += delta;
    }

    /// Initializing the bounding box
    void set_point(const glm::vec3& point)
    {
        min = max = point;
    }

    /// Set the bounding box to a null value
    void set_default()
    {
        min = glm::vec3(1.0f, 1.0f, 1.0f);
        max = glm::vec3(-1.0f, -1.0f, -1.0f);
    }
    /** Modify the current bbox to contain also the passed box.
        *  Adding a null bounding box does nothing
    */
    void add(Box3f const& b)
    {
        if (b.is_valid()) 
            return; // Adding a null bbox should do nothing

        if (is_valid())
        {
            *this = b;
        }
        else
        {
            if (min.x > b.min.x) 
                min.x = b.min.x;

            if (min.y > b.min.y) 
                min.y = b.min.y;

            if (min.z > b.min.z) 
                min.z = b.min.z;

            if (max.x < b.max.x) 
                max.x = b.max.x;

            if (max.y < b.max.y) 
                max.y = b.max.y;

            if (max.z < b.max.z) 
                max.z = b.max.z;
        }
    }
    /** Modify the current bbox to contain also the passed point
    */
    void add(const glm::vec3& p)
    {
        if (is_valid())
        {
            set_point(p);
        }
        else
        {
            if (min.x > p.x) 
                min.x = p.x;
            if (min.y > p.y)
                min.y = p.y;
            if (min.z > p.z) 
                min.z = p.z;

            if (max.x < p.x) 
                max.x = p.x;
            if (max.y < p.y) 
                max.y = p.y;
            if (max.z < p.z) 
                max.z = p.z;
        }
    }

    /** Modify the current bbox to contain also the passed sphere
    */
    void add(const glm::vec3& p, const float radius)
    {
        if (is_valid())
        {
            set_point(p);
        }
        else
        {
            min.x = std::min(min.x, p.x - radius);
            min.y = std::min(min.y, p.y - radius);
            min.z = std::min(min.z, p.z - radius);
                                      
            max.x = std::max(max.x, p.x + radius);
            max.y = std::max(max.y, p.y + radius);
            max.z = std::max(max.z, p.z + radius);
        }
    }
    /** Modify the current bbox to contain also the box b trasformed according to the matrix m
    */
    //void Add(const Matrix44<BoxScalarType>& m, const Box3<BoxScalarType>& b)
    //{
    //    if (b.IsNull()) return; // Adding a null bbox should do nothing

    //    const glm::vec3& mn = b.min;
    //    const glm::vec3& mx = b.max;
    //    Add(m * (Point3<BoxScalarType>(mn[0], mn[1], mn[2])));
    //    Add(m * (Point3<BoxScalarType>(mx[0], mn[1], mn[2])));
    //    Add(m * (Point3<BoxScalarType>(mn[0], mx[1], mn[2])));
    //    Add(m * (Point3<BoxScalarType>(mx[0], mx[1], mn[2])));
    //    Add(m * (Point3<BoxScalarType>(mn[0], mn[1], mx[2])));
    //    Add(m * (Point3<BoxScalarType>(mx[0], mn[1], mx[2])));
    //    Add(m * (Point3<BoxScalarType>(mn[0], mx[1], mx[2])));
    //    Add(m * (Point3<BoxScalarType>(mx[0], mx[1], mx[2])));
    //}
    /** Calcola l'intersezione tra due bounding box. Al bounding box viene assegnato il valore risultante.
        @param b Il bounding box con il quale si vuole effettuare l'intersezione
    */
    void intersect(const Box3f& b)
    {
        if (min.x < b.min.x) 
            min.x = b.min.x;
        if (min.y < b.min.y) 
            min.y = b.min.y;
        if (min.z < b.min.z) 
            min.z = b.min.z;

        if (max.x > b.max.x) 
            max.x = b.max.x;
        if (max.y > b.max.y) 
            max.y = b.max.y;
        if (max.z > b.max.z) 
            max.z = b.max.z;

        if (min.x > max.x || min.y > max.y || min.z > max.z) 
            set_default();
    }
    /** Trasla il bounding box di un valore definito dal parametro.
        @param p Il bounding box trasla sulla x e sulla y in base alle coordinate del parametro
    */
    void translate(const glm::vec3& p)
    {
        min += p;
        max += p;
    }
    /** true if the point belong to the closed box
    */
    bool in_point(glm::vec3 const& p) const
    {
        return (
            min.x <= p.x && p.x <= max.x &&
            min.y <= p.y && p.y <= max.y &&
            min.z <= p.z && p.z <= max.z
        );
    }
    /** true if the point belong to the open box (open on the max side)
        * e.g. if p in [min,max)
    */
    bool in_box(glm::vec3 const& p) const
    {
        return (
            min.x <= p.x && p.x < max.x &&
            min.y <= p.y && p.y < max.y &&
            min.z <= p.z && p.z < max.z
        );
    }
    /** Verifica se due bounding box collidono cioe' se hanno una intersezione non vuota. Per esempio
        due bounding box adiacenti non collidono.
        @param b A bounding box
        @return True se collidoo, false altrimenti
    */
    /* old version
    bool Collide(Box3<BoxScalarType> const &b)
    {
        Box3<BoxScalarType> bb=*this;
        bb.Intersect(b);
        return bb.IsValid();
    }
    */
    bool is_collide(Box3f const& b) const
    {
        return b.min.x < max.x && b.max.x > min.x &&
            b.min.y < max.y && b.max.y > min.y &&
            b.min.z < max.z && b.max.z > min.z;
    }
    /**
        return true if the box is null (e.g. invalid or not initialized);
    */
    inline bool is_valid() const 
    { 
        return min.x > max.x || min.y > max.y || min.z > max.z; 
    }
    /** return true if the box is empty (e.g. if min == max)
    */
    inline bool empty() const
    { 
        return min == max;
    }
    /// Return the lenght of the diagonal of the box .
    /*float Diag() const
    {
        return Distance(min, max);
    }*/
    /// Calcola il quadrato della diagonale del bounding box.
    /*float SquaredDiag() const
    {
        return SquaredDistance(min, max);
    }*/
    /// Return the center of the box.
    inline glm::vec3 get_center() const
    {
        return (min + max) / 2.0f;
    }

    inline glm::vec3 compute_bounding_box() const
    {
        return max - min;
    }

    /// Returns global coords of a local point expressed in [0..1]^3
    glm::vec3 local_to_world(glm::vec3 const& p) const
    {
        return glm::vec3(
            min.x + p.x * (max.x - min.x),
            min.y + p.y * (max.y - min.y),
            min.z + p.z * (max.z - min.z)
        );
    }
    /// Returns local coords expressed in [0..1]^3 of a point in 3D
    glm::vec3 world_to_local(glm::vec3 const& p) const 
    {
        return glm::vec3(
            (p.x - min.x) / (max.x - min.x),
            (p.y - min.y) / (max.y - min.y),
            (p.z - min.z) / (max.z - min.z)
        );
    }
    /// Return the volume of the box.
    inline float volume() const
    {
        return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
    }
    /// Calcola la dimensione del bounding box sulla x.
    inline float dimension_x() const
    { 
        return max.x - min.x; 
    }
    /// Calcola la dimensione del bounding box sulla y.
    inline float dimension_y() const 
    { 
        return max.y - min.y; 
    }
    /// Calcola la dimensione del bounding box sulla z.
    inline float dimension_z() const 
    {
        return max.z - min.z; 
    }

    inline uint8_t max_dim() const
    {
        glm::vec3 diag = max - min;

        if (diag.x > diag.y)
            return diag.x > diag.z ? 0 : 2;
        else
            return diag.y > diag.z ? 1 : 2;
    }
    inline uint8_t min_dim() const 
    {
        glm::vec3 diag = max - min;

        if (diag.x < diag.y)
            return diag.x < diag.z ? 0 : 2;
        else 
            return diag.y < diag.z ? 1 : 2;
    }

    //inline void Import(const Box3f& b)
    //{
    //    min.Import(b.min);
    //    max.Import(b.max);
    //}

    /// gives the ith box vertex in order: (x,y,z),(X,y,z),(x,Y,z),(X,Y,z),(x,y,Z),(X,y,Z),(x,Y,Z),(X,Y,Z)
    glm::vec3 get_order(const int& i) const 
    {
        return glm::vec3(
            min.x + (i % 2) * dimension_x(),
            min.y + ((i / 2) % 2) * dimension_y(),
            min.z + (i > 3) * dimension_z()
        );
    }
}; // end class definition

//template <class T> Box3f Point3<T>::GetBBox(Box3<T>& bb) const {
//    bb.Set(*this);
//}

#endif