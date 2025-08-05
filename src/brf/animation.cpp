#include "skeleton.h"

#include "animation.h"

using namespace brf;

template <class _Tx>
struct TmpCas
{
    void load(FileStreamReader& stream)
    {
        m_index = stream.read<int>();
        m_rotation = stream.read<_Tx>();
    }

    int m_index;
    _Tx m_rotation;
};

using TmpCas3 = TmpCas<glm::vec3>;
using TmpCas4 = TmpCas<glm::vec4>;

struct TmpBone4
{
    void load(FileStreamReader& stream)
    {
        load_impl(stream);

        for (auto& cas : m_cas)
        {
            cas.load(stream);
            cas.m_rotation = Bone::adjust_coordinate(cas.m_rotation);
        }
    }

    void load_root(FileStreamReader& stream)
    {
        load_impl(stream);

        for (auto& cas : m_cas)
        {
            cas.load(stream);
            cas.m_rotation = Bone::adjust_coordinate_half(cas.m_rotation);
        }
    }
private:
    inline void load_impl(FileStreamReader& stream)
    {
        const uint32_t length = stream.read<uint32_t>();

        m_cas.resize(length);
    }
public:
    std::vector<TmpCas4> m_cas;
};

static void cast_to_animation_frames(const std::vector<TmpBone4>& vb,
    const std::vector<TmpCas3>& vt,
    std::vector<AnimationFrame>& frames)
{
    int max_frames = 0;

    for (uint32_t i = 0; i < vb.size(); i++)
    {
        for (uint32_t j = 0; j < vb[i].m_cas.size(); j++)
        {
            int fi = vb[i].m_cas[j].m_index + 1;

            if (fi > max_frames)
                max_frames = fi;
        }
    }

    for (uint32_t j = 0; j < vt.size(); j++)
    {
        int fi = vt[j].m_index + 1;

        if (fi > max_frames) 
            max_frames = fi;
    }

    std::vector<int> present(max_frames, 0);

    // find and count frames that are acutally used
    int nf = 0;
    for (uint32_t i = 0; i < vb.size(); i++)
    {
        int ndup = 1;
        for (uint32_t j = 0; j < vb[i].m_cas.size(); j++)
        {
            int fi = vb[i].m_cas[j].m_index;
            if (j > 0) 
            {
                if (fi == vb[i].m_cas[j - 1].m_index) 
                    ndup++; 
                else
                    ndup = 1;
            }
            if (present[fi] < ndup) 
            {
                nf++;
                present[fi] = ndup;
            }
        }
    }
    // last round for translation
    {
        int ndup = 1;
        for (uint32_t j = 0; j < vt.size(); j++)
        {
            int fi = vt[j].m_index;
            if (j > 0) 
            {
                if (fi == vt[j - 1].m_index) 
                    ndup++; 
                else 
                    ndup = 1;
            }
            if (present[fi] < ndup) 
            {
                nf++;
                present[fi] = ndup;
            }
        }
    }

    // allocate and fill frames
    frames.resize(nf);
    for (int i = 0; i < nf; i++) 
    {
        frames[i].m_rotations.resize(vb.size());
        frames[i].m_was_implicit.resize(vb.size() + 1, false); // +1 for the translation
    }

    for (uint32_t bi = 0; bi < vb.size(); bi++)
    {
        int j = 0; // pos in current vb
        int m = 0; // pos in vf
        for (int fi = 0; fi < max_frames; fi++)
        {
            for (int dupl = 0; dupl < present[fi]; dupl++) // often zero times
            {
                int fi2;

                while ((fi2 = vb[bi].m_cas[j].m_index) < fi) 
                    j++;

                if (dupl > 0)
                { 
                    if (fi2 == fi) 
                    { 
                        j++; 
                        fi2 = vb[bi].m_cas[j].m_index; 
                    } 
                }

                frames[m].m_index = fi;

                if (fi2 == fi)
                {
                    frames[m].m_rotations[bi] = vb[bi].m_cas[j].m_rotation;
                    frames[m].m_was_implicit[bi] = false;
                }
                else
                {
                    // copy from prev
                    //assert(m > 0);
                    frames[m].m_rotations[bi] = frames[m - 1].m_rotations[bi];
                    frames[m].m_was_implicit[bi] = true;
                }
                m++;
            }
        }
        //assert(m == nf);
    }
    {
        // last round for translation
        int j = 0; // pos in current vb
        int m = 0; // pos in vf
        for (int fi = 0; fi < max_frames; fi++)
        {
            for (int dupl = 0; dupl < present[fi]; dupl++) // often zero times
            {
                int fi2;
                while ((fi2 = vt[j].m_index) < fi) 
                    j++;

                if (dupl > 0) 
                { 
                    if (fi2 == fi) 
                    { 
                        j++; 
                        fi2 = vt[j].m_index;
                    } 
                }

                frames[m].m_index = fi;
                if (fi2 == fi) 
                {
                    frames[m].m_transform = vt[j].m_rotation;
                    frames[m].m_was_implicit[vb.size()] = false;
                }
                else
                {
                    // copy from prev
                    //assert(m > 0);
                    frames[m].m_transform = frames[m - 1].m_transform;
                    frames[m].m_was_implicit[vb.size()] = true;
                }
                m++;
            }
        }
        //assert(m == nf);
    }
}

bool Animation::load(FileStreamReader& stream)
{
    std::vector<TmpBone4> tmp_bones {};
    std::vector<TmpCas3> tmp_cas {};

    m_name = stream.read_with_length();
    m_bones_count = stream.read<uint32_t>();

    tmp_bones.resize(m_bones_count);

    tmp_bones.front().load_root(stream);

    for (uint32_t i = 1; i < tmp_bones.size(); i++)
        tmp_bones[i].load(stream);

    const uint32_t cas_length = stream.read<uint32_t>();
    tmp_cas.resize(cas_length);

    for (auto& cas : tmp_cas)
        cas.load(stream);

    cast_to_animation_frames(tmp_bones, tmp_cas, m_frames);

    return true;
}
