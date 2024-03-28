#include "transition_gen.hpp"

void tranition_generate(Map &m)
{
    h_res_clock::time_point start_time = h_res_clock::now();
    std::vector<std::array<uint16_t, 2>> tr;
    std::vector<std::array<uint16_t, 6>> output;
    std::array<uint16_t, 2> temp = {0, 0};
    for (int i = 1; i < (m.height * m.width + 1); i++)
    {
        if (m.all_map_moves[i].symbol != '-')
        {
            for (int j = 0; j < 8; j++)
            {
                if (m.all_map_moves[i].transitions[j] == 0)
                {
                    temp[0] = i;
                    temp[1] = j;
                    tr.push_back(temp);
                }
            }
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    shuffle(tr.begin(), tr.end(), g);
    std::array<uint16_t, 6> temp_bigger = {0, 0, 0, 0, 0, 0};
    uint16_t temp_size = tr.size() / 2;
    for (int i = 0; i < temp_size; i++)
    {
        tr.back()[0] % m.width == 0 ? temp_bigger[0] = m.width : temp_bigger[0] = tr.back()[0] % m.width;
        temp_bigger[1] = (tr.back()[0] - 1) / m.width + 1;
        temp_bigger[2] = tr.back()[1];
        tr.pop_back();
        tr.back()[0] % m.width == 0 ? temp_bigger[3] = m.width : temp_bigger[3] = tr.back()[0] % m.width;
        temp_bigger[4] = (tr.back()[0] - 1) / m.width + 1;
        temp_bigger[5] = tr.back()[1];
        tr.pop_back();
        if (!(temp_bigger[0] == 0 && temp_bigger[1] == 0 && temp_bigger[3] == 0 && temp_bigger[4] == 0))
        {
            output.push_back(temp_bigger);
        }
        temp_bigger = {0, 0, 0, 0, 0, 0};
    }
    for (auto elem : output)
    {
        std::cout << elem[0] - 1 << " " << elem[1] - 1 << " " << elem[2] << " <-> " << elem[3] - 1 << " " << elem[4] - 1 << " " << elem[5] << " " << std::endl;
    }
    h_res_clock::time_point end_time = h_res_clock::now();
    std::chrono::duration<double, std::micro> elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Elapsed time (generate_transitions): " << elapsed_time.count() << " microseconds" << std::endl;
}