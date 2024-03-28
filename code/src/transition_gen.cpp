#include "transition_gen.hpp"

void generate(Map &m)
{
    h_res_clock::time_point start_time = h_res_clock::now();
    std::vector<uint16_t> tr1, tr2;
    std::vector<std::array<uint16_t, 6>> output;
    uint16_t half = (m.get_height() * m.get_width() + 1) / 2;
    for (int i = 1; i < half; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (m.all_map_moves[i].transitions[j] == 0)
            {
                tr1.push_back(i);
                tr1.push_back(j);
            }
        }
    }
    for (int i = half; i < (m.get_height() * m.get_width() + 1); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (m.all_map_moves[i].transitions[j] == 0)
            {
                tr2.push_back(i);
                tr2.push_back(j);
            }
        }
    }
    std::array<uint16_t, 6> temp_bigger = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < tr1.size(); i++)
    {
        tr1[i] % m.width == 0 ? temp_bigger[0] = m.width : temp_bigger[0] = tr1[i] % m.width;
        temp_bigger[1] = temp_bigger[1] / m.width + 1;
        temp_bigger[2] = tr1[i + 1];
        tr2[i] % m.width == 0 ? temp_bigger[3] = m.width : temp_bigger[3] = tr2[i] % m.width;
        temp_bigger[4] = temp_bigger[4] / m.width + 1;
        temp_bigger[5] = tr2[i + 1];
        output.push_back(temp_bigger);
        i++;
    }
    for (auto elem : output)
    {
        std::cout << elem[0] << " " << elem[1] << " " << elem[2] << " <-> " << elem[3] << " " << elem[4] << " " << elem[5] << " " << std::endl;
    }
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Elapsed time (generate_transitions): " << elapsed_time.count() << " microseconds" << std::endl;
}